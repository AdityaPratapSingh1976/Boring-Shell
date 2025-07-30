#include "bosh_utilities.h"
extern char **environ;
char cmdbuffer[MAXCMDLENGTH];
char* cmdargv[MAXARGLENGTH];
int cmdargc;

void testlink(void){
    printf("linked successfully\n");
}

void set(void){
    for(int i = 0; environ[i]!= NULL;++i){
        printf("%s\n", environ[i]);
    }
}

int env_assign(char *str){
    char *name = strtok(str, "=");
    char *value = strtok(NULL, "");
    if(name == NULL || value == NULL){
        fprintf(stderr, "bad assignment\n");
        return -1;
    }
    setenv(name, value , 1);
    return 1;
}

char *readcmd(char * banner){
    char *line = readline(banner);
    memcpy(cmdbuffer, line, strnlen(line, MAXCMDLENGTH - 1) + 1);
    add_history(line);
    if(line != NULL){
        free(line);
    }
    return cmdbuffer;
}

void sigint_handler(int signum){
    printf("sigint received\n");
    return ;
}

void sigtstp_handler(int signum){
    printf("sigtstp received\n");
    return ;
}

// Builtin command handler
bool builtin(int argc, char *argv[], int srcfd, int dstfd) {
    if (argc == 0) return true;
    
    if (strcmp(argv[0], "set") == 0) {
        set();
        return true;
    }
    else if (strcmp(argv[0], "exit") == 0) {
        exit(0);
    }
    else if (strchr(argv[0], '=') != NULL) {
        env_assign(argv[0]);
        return true;
    }
    
    return false;
}

// Entry signal handler
void entry_sig(void) {
    // Reset signal handlers for child process
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

// File descriptor check
void fd_check(void) {
    // Check if file descriptors are valid
    if (fcntl(STDIN_FILENO, F_GETFD) == -1) {
        fprintf(stderr, "Invalid stdin\n");
        _exit(EXIT_FAILURE);
    }
    if (fcntl(STDOUT_FILENO, F_GETFD) == -1) {
        fprintf(stderr, "Invalid stdout\n");
        _exit(EXIT_FAILURE);
    }
}

// Display status
void display_status(pid_t wpid, int status) {
    if (WIFEXITED(status)) {
        printf("child [%d]: exit status %d\n", wpid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child [%d]: killed by signal %d\n", wpid, WTERMSIG(status));
    }
}

// Redirect function
void redirect(int srcfd, const char *srcfile, int dstfd,
    const char *dstfile, bool append, bool bckgrnd)
{
    int flags;

    if (srcfd == STDIN_FILENO && bckgrnd) {
        srcfile = "/dev/null";
        srcfd = -1;
    }
    if (srcfile[0] != '\0')
        srcfd = open(srcfile, O_RDONLY, 0); 
    dup2(srcfd, STDIN_FILENO);     
    if (srcfd != STDIN_FILENO)
        close(srcfd); 
    if (dstfile[0] != '\0') {
        flags = O_WRONLY | O_CREAT;
        if (append)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
        dstfd = open(dstfile, flags, PERM_FILE); 
    }
    dup2(dstfd, STDOUT_FILENO); 
    if (dstfd != STDOUT_FILENO)
        close(dstfd); 
    fd_check();
}

// Invoke function
pid_t invoke(int argc, char *argv[], int srcfd, const char *srcfile,
    int dstfd, const char *dstfile, bool append, bool bckgrnd, int closefd)
{
    pid_t pid;
    char *cmdname, *cmdpath;

    if (argc == 0 || builtin(argc, argv, srcfd, dstfd))
        return 0;
    switch (pid = fork()) {
    case -1:
        fprintf(stderr, "Can't create new process\n");
        return 0;
    case 0:
        if (closefd != -1)
            close(closefd); 
        if (!bckgrnd)
            entry_sig(); 
        redirect(srcfd, srcfile, dstfd, dstfile, append, bckgrnd);
        cmdname = strchr(argv[0], '/');
        if (cmdname == NULL)
            cmdname = argv[0];
        else
            cmdname++;
        cmdpath = argv[0];
        argv[0] = cmdname;
        execvp(cmdpath, argv);
        fprintf(stderr, "Can't execute %s\n", cmdpath);
        _exit(EXIT_FAILURE);
    }
    /* parent */
    if (srcfd > STDOUT_FILENO)
        close(srcfd); 
    if (dstfd > STDOUT_FILENO)
        close(dstfd); 
    if (bckgrnd)
        printf("%ld\n", (long)pid);
    return pid;
}

// Wait and display function
bool wait_and_display(pid_t pid)
{
    pid_t wpid;
    int status;
    do {
        wpid = waitpid(-1, &status, 0); 
        display_status(wpid, status);
    } while (wpid != pid);
    return true;
}

