
#ifndef BOSH_ERRHAND_H
#define BOSH_ERRHAND_H
#include "bosh_utilities.h"

void unix_error(char *msg);
void handle_null(const char * const p);
void handle_negreturn(int r);
#endif