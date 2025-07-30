#include "bosh_errhand.h"

void unix_error(char *msg){
    fprintf(stderr, "%s: %s", msg, strerror(errno));
    exit(1);
}

void handle_null(const char * const p){
    if(p == NULL)
        unix_error("null assignment");
}
void handle_negreturn(int r){
    if(r == -1)
        unix_error("function exited with -1");
}
