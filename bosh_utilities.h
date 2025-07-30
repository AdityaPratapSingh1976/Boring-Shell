#ifndef BOSH_UTIL_H
#define BOSH_UTIL_H


/*- STANDARD INCLUDES -*/
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <stdbool.h>
/*-x-*/


/*-BOSH INCLUDE-*/
#include "bosh_defines.h"
#include "bosh_parser.h"
#include "bosh_errhand.h"
/*-x-*/


/*- STATIC MEMORY -*/
extern char cmdbuffer[MAXCMDLENGTH];
extern char* cmdargv[MAXARGLENGTH];
extern int cmdargc;

extern char banner[64], pidstr[16];
extern pid_t shell_pid;
/*-x-*/

// Constants
#define PERM_FILE 0644
#define MAXARG 50			/* max args in command */
#define MAXFNAME 500		/* max chars in file name */
#define MAXWORD 500	
/*
* print char** environ
*/
void set(void);


/*
* add and modify NAME=VALUE in char** environ
*/
int env_assign(char *str);


/*
*test if library is linked or not
*/
void testlink(void);

/*
 * read the command from terminal
 * returns a pointer to cmdline buffer or NULL if no command 
 * allocates memory dynamically must be freed after using
 * length: MAXCMDLENGTH
 */
char *readcmd(char * banner);

/*
* signal handler for SIGINT
*/
void sigint_handler(int signum);

/*
* signal handler for SIGTSTP
*/
void sigtstp_handler(int signum);

/*
* builtin command handler
*/
bool builtin(int argc, char *argv[], int srcfd, int dstfd);

/*
* entry signal handler
*/
void entry_sig(void);

/*
* file descriptor check
*/
void fd_check(void);

/*
* display status
*/
void display_status(pid_t wpid, int status);

/*
* redirect function
*/
void redirect(int srcfd, const char *srcfile, int dstfd,
    const char *dstfile, bool append, bool bckgrnd);

/*
* invoke function
*/
pid_t invoke(int argc, char *argv[], int srcfd, const char *srcfile,
    int dstfd, const char *dstfile, bool append, bool bckgrnd, int closefd);

/*
* wait and display function
*/
bool wait_and_display(pid_t pid);

#endif