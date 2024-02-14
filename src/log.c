#include "log.h"

typedef struct LOG {
    FILE *file;
    FILE *out;
    FILE *err;
    int (*logfuncptr)(char *);
} LOG;

LOG *log_get() {
	static LOG data;
	return &data;
}

CRESULT log_create(char *filename, int (*logfuncptr)(char *)) {
    log_get()->file = NULL;
    log_get()->logfuncptr = NULL;
    if(logfuncptr) {
       log_get()->logfuncptr = logfuncptr;
    }
    if(filename) {
        //log_get()->out = freopen("stdout.txt", "a", stdout);
        //log_get()->err = freopen("stderr.txt", "a", stderr);
        if((log_get()->file = fopen("log.txt", "a"))==NULL) {
            return CFAILED;
        }
    }
    log_msg("-= START =-");

    return CSUCCESS;
}

void log_destroy() {
    log_msg("-= STOP =-");

    fclose(log_get()->file);
    //fclose(log_get()->out);
    //fclose(log_get()->err);
}

CRESULT log_msg(char *message) {
    struct tm *tm_info;
    char timestr[32];
    time_t timer;
    char buffer[1024];

    time(&timer);
    tm_info = localtime(&timer);
    strftime(timestr, sizeof(timestr), "[%Y-%m-%d %H:%M:%S] ", tm_info);

   if(log_get()->logfuncptr) {
        sprintf(buffer, "%s%s\r\n", timestr, message);
        (*log_get()->logfuncptr)(buffer);
    }
    if(log_get()->file) {
        fprintf(log_get()->file, "%s%s\r\n", timestr, message);
        fflush(log_get()->file);
    }
    return CFAILED;
}


