#include "header.h"

void add_job(pid_t pid, char *command) { // Insert_first
  // Add a new command to jobs list
  job *new = malloc(sizeof(job));
  if (new == NULL) {
    perror("minishell: malloc failed for job");
    return;
  }
  new->pid=pid;
  strcpy(new->cmd,command);
  new->status = STOPPED;
  new->next=head;
  head=new;
}

void remove_job(pid_t pid) {
  // Remove a job from jobs list
  job *temp=head,*prev=NULL;
  while(temp!=NULL){
    if(temp->pid==pid){
      if(prev==NULL){
        head=temp->next;  
      }
      else{
        prev->next=temp->next;
      }
      free(temp);
      return;
    }
    prev=temp;
    temp=temp->next;
  }
}

void list_jobs() { //print_list
  // Print all jobs with its detail.
  job *temp=head;
  int id=1;
  while(temp!=NULL){
    printf("[%d]+  %s\t\t%s\n",id++,temp->status==STOPPED?"Stopped":"Running",temp->cmd);
    temp=temp->next;
  }
}
