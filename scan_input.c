#include "header.h"
pid_t pid,st;
char path_cmd[100];

void scan_input(char *prompt, char *input_string){
    extract_external_commands(ext_commands);
    signal(SIGINT,signal_handler);
    signal(SIGTSTP,signal_handler);
    while(1){
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ":" 
            ANSI_COLOR_BLUE "~%s" ANSI_COLOR_RESET "$ ",prompt,path_cmd); //print the prompt
        fflush(stdout);
        int ret;
        if ((ret=scanf("%[^\n]",input_string))==0) {
            getchar(); // Handle empty input
            continue;
        }
        if(ret==EOF){ //ctrl+D occurs exit
            printf("exit\n");
            exit(0);
        }
        getchar(); //clear \n
        if(strncmp("ps1=",input_string,4)==0){
            //trailing space remove
            int len=strlen(input_string)-1;
            while(len>3){
                if(input_string[len]!=' '){
                    input_string[len+1]='\0';
                    break;
                }
                len--;
            }
            if(strchr(input_string+4,' ')!=0){
                fprintf(stderr,"Command '%s' not found!\n",input_string);
            }
            else if(input_string[4]!='\0'){
                strcpy(prompt,input_string+4);
            }
        }
        else{
            char *cmd=get_command(input_string);
            int type=check_command_type(cmd);
            if(type==BUILTIN){
                execute_internal_commands(input_string);
            }
            else if(type==EXTERNAL){
                //create child process using fork()
                pid=fork();
                if(pid>0){
                    //wait(NULL); doesnot wait for stopped process
                    /* WUNTRACED flag: Tells the OS kernel, "Return control to my shell not only
                     if the child terminates, but also if it is suspended/stopped via Ctrl+Z 
                     (SIGTSTP)." */
                    // WUNTRACED its also wait for ctrl+z(stepped process)
                    waitpid(pid,&st,WUNTRACED);
                    if (WIFSTOPPED(st)) {
                        add_job(pid, input_string);
                        printf("\n[%d]+ Stopped    %s\n",++job_id,input_string);
                    }
                    pid=0;
                }
                else if(pid==0){
                    signal(SIGINT,SIG_DFL);  //ctrl+c signal
                    signal(SIGTSTP,SIG_DFL); //ctrl+z signal
                    execute_External_commands(input_string); //inside execvp() called
                    exit(1); // Only reachable if execution FAILED(execvp failed)
                }
            }
            else if(strcmp(input_string,"\n")!=0 && strcmp(input_string,"")!=0){
                fprintf(stderr,"Command '%s' not found!\n",input_string);
            }
        }
    }
}

void signal_handler(int signum){
    if(signum==SIGINT){
        if(pid==0){
            printf("\n"ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ":" 
            ANSI_COLOR_BLUE "~%s" ANSI_COLOR_RESET "$ ",prompt,path_cmd); //print the prompt
            fflush(stdout);
        }
        else{
            printf("\n");
            fflush(stdout);
        }
    }
    if(signum==SIGTSTP){
        if(pid==0){
            printf("\n"ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ":" 
            ANSI_COLOR_BLUE "~%s" ANSI_COLOR_RESET "$ ",prompt,path_cmd); //print the prompt
            fflush(stdout);
        }
        else{
            printf("\n");
            fflush(stdout);
        }
    }
    if(signum==SIGCHLD){
        pid_t bg_pid;
        int status;
        // Cleans up all background processes that have terminated
        while((bg_pid = waitpid(-1, &status, WNOHANG)) > 0){
            remove_job(bg_pid); // Removed once it dies(beacuse bg does run background process)
        }
    }
}