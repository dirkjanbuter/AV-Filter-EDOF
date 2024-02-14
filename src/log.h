#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

#define LOG_MAX_PATH 1024

#define E(M) log_msg(M)

CRESULT log_create(char *filename, int (*logfuncptr)(char *));
void log_destroy();
CRESULT log_msg(char *message);

#endif
