/*
NAME:        HARIHARAN R
DATE:        31/08/2026
DESCRIPTION:A custom Unix shell implementation in C that parses and executes 
            internal built-ins and external commands. Features robust job 
            control (jobs, fg, bg, SIGINT, SIGTSTP, SIGCHLD), dynamic multi-stage 
            piping (IPC using pipe, fork, and dup2), custom prompt configuration (PS1), 
            and non-blocking background process tracking.
 */
#include "header.h"
char *ext_commands[154];
char prompt[25]="minishell";
char input_string[50];
job *head=NULL;
int job_id;

int main(){
    system("clear");  //clear the screen
    scan_input(prompt,input_string);
}