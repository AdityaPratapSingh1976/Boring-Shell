#include "bosh_parser.h"

void execute(){
    pid_t pid = -1;
    switch (pid = fork())
    {
    case -1:
        printf("fork error: %s\n", strerror(errno));
        break;
    case 0:
        execvp(cmdargv[0], cmdargv);
        printf("%s: command not found: %s\n", banner,cmdargv[0]);
        exit(-1);
        break;
    default:
        int status;
        wait(&status);
        printf("child [%d]: exit status %d\n", pid, WEXITSTATUS(status));
        
        break;
    }
}



TOKEN gettoken(char *word, size_t maxword)
{
    enum {NEUTRAL, GTGT, INQUOTE, INWORD} state = NEUTRAL;
    int c;
    size_t wordn = 0;
    while ((c = getchar()) != EOF) {
        switch (state) {
            case NEUTRAL:
                switch (c) {
                case ';':
                    return T_SEMI;
                case '&':
                    return T_AMP;
                case '|':
                    return T_BAR;
                case '<':
                    return T_LT;
                case '\n':
                    return T_NL;
                case ' ':
                case '\t':
                    continue;
                case '>':
                    state = GTGT;
                    continue;
                case '"':
                    state = INQUOTE;
                    continue;
                default:
                    state = INWORD;
                    store_char(word, maxword, c, &wordn); 
                    continue;
                }
            case GTGT:
                if (c == '>')
                    return T_GTGT;
                ungetc(c, stdin);
                return T_GT;
            case INQUOTE:
                switch (c) {
                    case '\\':
                        if ((c = getchar()) == EOF)
                        c = '\\';
                        store_char(word, maxword, c, &wordn); 
                        continue;
                    case '"':
                        store_char(word, maxword, '\0', &wordn); 
                        return T_WORD;
                    default:
                        store_char(word, maxword, c, &wordn); 
                        continue;
                }
            case INWORD:
                switch (c) {
                case ';':
                case '&':
                case '|':
                case '<':
                case '>':
                case '\n':
                case ' ':
                case '\t':
                    ungetc(c, stdin);
                    store_char(word, maxword, '\0', &wordn); 
                    return T_WORD;
                default:
                    store_char(word, maxword, c, &wordn); 
                    continue;
                }
            }
        }
    if(ferror(stdin))
        return T_ERROR;
    return T_EOF;

}

bool store_char(char *word, size_t maxword, int c, size_t *np)
{
    errno = E2BIG;
    if (*np >= maxword - 1) { errno = E2BIG; return false; }
    word[(*np)++] = c;
    return true;
}


int cmdparse(){
    char *cmdargp = cmdbuffer;
    cmdargc = 0;

    for(cmdargc = 0; cmdargc < MAXARGLENGTH; ++cmdargc){
        if((cmdargv[cmdargc] = strtok(cmdargp, " ")) == NULL)
            break;
        cmdargp = NULL;
    }

    if(cmdargc >= MAXARGLENGTH){
        fprintf(stderr,"command discarded: too many arguments\n");
        return -1;
    }   
    return 1;
}

TOKEN command(pid_t *wpid, bool makepipe, int *pipefdp)
{
    TOKEN token, term;
    int argc, srcfd, dstfd, pid, pfd[2] = {-1, -1};
    char *argv[MAXARGLENGTH], srcfile[MAXCMDLENGTH] = "", dstfile[MAXCMDLENGTH] = "";
    char word[MAXCMDLENGTH];
    bool append = false;
    argc = 0;
    srcfd = STDIN_FILENO;
    dstfd = STDOUT_FILENO;
    while (true) {
        switch (token = gettoken(word, sizeof(word))) {
        case T_WORD:
            if (argc >= MAXARGLENGTH - 1) {
                fprintf(stderr, "Too many args\n");
                continue;
            }
            if ((argv[argc] = malloc(strlen(word) + 1)) == NULL) {
                fprintf(stderr, "Out of arg memory\n");
                continue;
            }
            strcpy(argv[argc], word);
            argc++;
            continue;
        case T_LT:
            if (makepipe) {
                break;
                fprintf(stderr, "Extra <\n");
            }
            if (gettoken(srcfile, sizeof(srcfile)) != T_WORD) {
                fprintf(stderr, "Illegal <\n");
                break;
            }
            srcfd = -1;
            continue;
        case T_GT:
        case T_GTGT:
            if (dstfd != STDOUT_FILENO) {
                fprintf(stderr, "Extra > or >>\n");
                break;
            }
            if (gettoken(dstfile, sizeof(dstfile)) != T_WORD) {
                fprintf(stderr, "Illegal > or >>\n");
                break;
            }
            dstfd = -1;
            append = token == T_GTGT;
            continue;
        case T_BAR:
        case T_AMP:
        case T_SEMI:
        case T_NL:
            argv[argc] = NULL;
            if (token == T_BAR) {
                if (dstfd != STDOUT_FILENO) {
                    fprintf(stderr, "> or >> conflicts with |\n");
                    break;
                }
                term = command(wpid, true, &dstfd);
                if (term == T_ERROR)
                return T_ERROR;
            }
            else
                term = token;
            if (makepipe) {
                pipe(pfd);   
                *pipefdp = pfd[1];
                srcfd = pfd[0];
            }
            pid = invoke(argc, argv, srcfd, srcfile, dstfd,
            dstfile, append, term == T_AMP, pfd[1]);
            if (token != T_BAR)
                *wpid = pid;
            if (argc == 0 && (token != T_NL || srcfd > 1))
                fprintf(stderr, "Missing command\n");
            while (--argc >= 0)
                free(argv[argc]);
            return term;
        case T_EOF:
            exit(EXIT_SUCCESS);
        case T_ERROR:
            return T_ERROR;
        }
    }
    return T_ERROR;
}