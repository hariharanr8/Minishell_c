#include "header.h"

char *builtins[] = {"echo", "printf", "read", "fg", "bg", "jobs", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","clear",NULL};

char *get_command(char *input_string){
	char *cmd=malloc(10);
	//cmd==ip[i] untill space
	int i=0;
	while(input_string[i]){
		if(input_string[i]==' '){
			break;
		}
		cmd[i]=input_string[i];
		i++;
	}
	cmd[i]='\0';
	return cmd;
}

void extract_external_commands(char **external_commands){
	//while until EOF
	//open the file and read 1 byte 1 store in in buffer string untill \n
	//then that string size to create dynamic memory for column and store(use fssd)
	int fd;
	char buff;
	if((fd=(open("External_cmd.txt",O_RDONLY)))<0){
		perror("file does not exist!\n");
		return;
	}
	char str[50];
	int i=0,j=0;
	while(read(fd,&buff,1)>0){
		if(buff=='\n'){
			// why \r beacuse windows files end with \r\n.so \r present str(buff) close(\0).
			if(str[i-1]=='\r'){
				str[i-1]='\0';
			}
			else{
				str[i]='\0';
			}
			external_commands[j]=malloc(strlen(str)+1);
			strcpy(external_commands[j],str);
			j++;
			i=0;
		}
		else{
			str[i++]=buff;
		}
	}
	if(i>0){
		str[i]='\0';
		external_commands[j]=malloc(strlen(str)+1);
		strcpy(external_commands[j],str);
		j++;
	}
	external_commands[j]=NULL;
	close(fd);
}
int check_command_type(char *command){//args pass that return cmd to command 
	//compare builtins array with command
		//true return builtin macros
	// int len=sizeof(builtins)/8;
	for(int i=0;builtins[i]!=NULL;i++){
		if(strcmp(command,builtins[i])==0){
			free(command);
			return BUILTIN;
		}
	}
	//false compare ext_commands[] array global with cmd
		//true return EXTERNAL macros
	for(int i=0;ext_commands[i]!=NULL;i++){
		if(strcmp(command,ext_commands[i])==0){
			free(command);
			return EXTERNAL;
		}
	}
	free(command);
	return NO_COMMAND;  //false return NO_COMMAND macros 
}

void execute_internal_commands(char *input_string){
	if(strncmp(input_string,"exit",4)==0){
		exit(0);
	}
	else if(strcmp(input_string, "clear") == 0){
		system("clear");
	}
	else if(strcmp(input_string, "help") == 0){
        printf("--- Minishell Built-in Commands ---\n");
        printf("  cd <path>    : Change directory\n");
        printf("  pwd          : Print working directory\n");
        printf("  clear        : Clear terminal screen\n");
        printf("  echo $$      : Print process ID\n");
        printf("  echo $?      : Print exit status of last command\n");
        printf("  echo $SHELL  : Print current shell path\n");
        printf("  exit         : Exit minishell\n");
    }
	else if(strncmp(input_string,"pwd",3)==0){
		char buff[200];
		getcwd(buff,sizeof(buff));
		printf("%s\n",buff);
	}
	else if(strncmp(input_string,"cd",2)==0){
		char *path=input_string+2;
		//whitespace skip
		while(*path==' '){
			path++;
		}
		// cd or cd ~ command change to home directary
		if(*path=='\0' || strcmp(path,"~")==0){
			path=getenv("HOME");
		}
		if (chdir(path) == 0) {
			getcwd(path_cmd,sizeof(path_cmd));
    	}
		else {
			perror("minishell: cd");
		}
	}
	//echo print
	
	else if(strncmp(input_string, "echo", 4) == 0 &&
			(input_string[4] == '\0' || input_string[4] == ' ')) {
		//after 4bytes check(skip the echo)
		char *arg = input_string + 4;
		//leading space avoid
		while (*arg == ' ') {
			arg++;
		}
		
		if (strcmp(arg, "$$") == 0) {
			printf("%d\n", getpid());
		}
		else if (strcmp(arg, "$?") == 0) {
			if (WIFEXITED(st)) {
				printf("%d\n", WEXITSTATUS(st));
			}
			else {
				printf("0\n");
			}
		}
		else if (strcmp(arg, "$SHELL") == 0) {
			char *shell = getenv("SHELL");
			printf("%s\n", shell ? shell : "");
		}
		else {
			printf("%s\n", arg);
		}
	}

	//else if 'jobs'
		//print_list() linked list
	else if(strcmp(input_string,"jobs")==0){
		list_jobs();
	}
	//else if 'fg'
		//kill(pid,sigcont)
		//waitpid(s->pid,&st,WUNTRACED);
		//delte_first()
	else if(strcmp(input_string,"fg")==0){
		if(head == NULL){
            fprintf(stderr, "minishell: fg: Current: no such job\n");
            return;
        }
		//copy content before remove_job()
		pid_t job_pid = head->pid;
		char job_cmd[100];
		strcpy(job_cmd, head->cmd);
		remove_job(head->pid); //remove from the list
		pid = job_pid;  //resume childprocess so pid change to childpid
		kill(job_pid,SIGCONT);
		printf("%s\n",job_cmd);
		waitpid(job_pid,&st,WUNTRACED);
		if (WIFSTOPPED(st)) {
        	add_job(job_pid, job_cmd); // Put back if user presses Ctrl+Z again
			printf("\n[1]+ Stopped    %s\n", job_cmd);
    	}
		pid=0;
	}
	//else if 'bg'
		//signal(SIGCHLD,signal_handler);
		//print s->cmd
		//kill(s->pid,SIGCONT);
		//delete_first()
	else if(strcmp(input_string,"bg")==0){
		job *temp = head;
    	int job_id = 1;
		signal(SIGCHLD,signal_handler);
		// Traverse the list to find the latest STOPPED process
		while (temp!=NULL && temp->status!=STOPPED) {
			temp = temp->next;
			job_id++;
		}
		if (temp==NULL) {
       		fprintf(stderr, "minishell: bg: Current: no such jobs\n");
		} else {
			temp->status=RUNNING;
			printf("[%d]+ %s &\n",job_id,temp->cmd);
			kill(temp->pid,SIGCONT); // Resume process in background
		}
	}
}

void execute_External_commands(char *input_string){
	//create 2d array to pass execvp().store the input string (1darray) to the 2darray.
	int i=0,row=0,pipe_pre=0,j=0,count;
	while(input_string[i]){ //check no.of rows and pipe present
		if(input_string[i]==' '){
			row++;
		}
		if(pipe_pre==0 && input_string[i]=='|'){
			pipe_pre=1;
		}
		i++;
	}
	char *argv[row+2]; //+2 for NULL add in 2darray last and last word count(\n).
	i=0,j=0,count=0;
	char buff[50];
	while(input_string[i]){
		if(input_string[i]==' '){
			buff[count]='\0'; //buff endup
			argv[j]=malloc(sizeof(char)*count+1); //+1 for '\0'
			strcpy(argv[j],buff); //stored in argv
			j++;
			count=0; //reset count and buff
		}
		else if(input_string[i] != '"' && input_string[i] != '\''){
			buff[count]=input_string[i];
			count++;
		}
		i++;
	}
	if(count > 0){ //last word store
        buff[count]='\0';
        argv[j]=malloc(sizeof(char)*(count+1));
        strcpy(argv[j],buff);
        j++;
    }
	argv[j]=NULL;
	//check the pipe is present or not
	if(!pipe_pre){
		execvp(argv[0],argv);
	}
	else{
		int i=0,stdin_fd;
		int arr_pos[30],cmd=0;
		arr_pos[cmd++]=0;
		while(argv[i]){
			if(strcmp(argv[i],"|")==0){
				argv[i]=NULL;
				arr_pos[cmd++]=i+1;
			}
			i++;
		}
		int n_pipes=cmd-1;
		stdin_fd=dup(0);
		for(int i=0;i<cmd;i++){
			int fd[2];
			if(i<n_pipes){
				pipe(fd);
			}
			pid_t pid1=fork();
			if(pid1>0){
				if(i<n_pipes){
					dup2(fd[0],0);
				}
				close(fd[1]);
				close(fd[0]);
			}
			if(pid1==0){
				if(i<n_pipes){
					dup2(fd[1],1);
				}
				close(fd[1]);
				close(fd[0]);
				execvp(argv[arr_pos[i]],argv+arr_pos[i]);
			}
		}
		dup2(stdin_fd,0);
		close(stdin_fd);
		// Inside execute_External_commands after the pipe loop:
		while (wait(NULL) > 0); // Wait for EVERY grandchild to finish
		exit(0);// Terminate Child A(because child continues run in scan_input while(1) so force to exit) only AFTER all grandchildren are done
	}
}

