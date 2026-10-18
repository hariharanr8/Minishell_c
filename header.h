#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern char *ext_commands[154];
extern char prompt[25];
extern char path_cmd[100];
extern char input_string[50];
extern pid_t pid;
extern int st;
extern int job_id;

typedef enum {
    STOPPED,
    RUNNING
} JobStatus;

typedef struct job{
    pid_t pid;
    char cmd[100];
    JobStatus status;
    struct job *next;
}job;

extern struct job *head;

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);

int check_command_type(char *command);
void execute_internal_commands(char *input_string);
void execute_External_commands(char *input_string);
void signal_handler(int signum);
void extract_external_commands(char **external_commands);
void add_job(pid_t pid, char *command);
void remove_job(pid_t pid);
void list_jobs();

#endif