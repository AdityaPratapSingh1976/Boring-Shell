#ifndef BOSH_PARSER_H
#define BOSH_PARSER_H
#include "bosh_utilities.h"
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
* token types
*/
typedef enum {T_WORD, T_BAR, T_AMP, T_SEMI, T_GT, T_GTGT, T_LT,
    T_NL, T_EOF, T_ERROR} TOKEN;

/*
* break into cmdargc and cmdargv
*/
int cmdparse();

/*
* evaluate command using advanced tokenizer
*/
int eval();

/*
* execute the command
*/
void execute();

/*
* get next token from input
*/
TOKEN gettoken(char *word, size_t maxword);

/*
* get next token from string input
*/
TOKEN gettoken_string(char *word, size_t maxword);

/*
* store character in word buffer
*/
bool store_char(char *word, size_t maxword, int c, size_t *np);

/*
* initialize string-based tokenizer
*/
void init_tokenizer(char *input);

/*
* command parser using tokenizer
*/
TOKEN command(pid_t *wpid, bool makepipe, int *pipefdp);

/*
* ignore signal function
*/
void ignore_sig(void);

#endif