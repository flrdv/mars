//
// Created by pavlo on 19.07.24.
//

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "filewriter.h"

typedef struct {
    FILE* fd;
} report_filewriter_env_t;

static void report_filewriter_write(void* env, report_level_t level, const char* format, ...) {
    va_list args;
    va_start(args, format);

    report_filewriter_env_t* self = env;
    const char* prefix = "[%s] ";
    char* newformat = malloc(strlen(prefix) + strlen(format));
    strcpy(newformat, newformat);
    strcat(newformat, format);

    char* level_str = "UNKNOWN";
    switch (level) {
    case LEVEL_INFO:    level_str = "info";    break;
    case LEVEL_WARNING: level_str = "warning"; break;
    case LEVEL_ERROR:   level_str = "error";   break;
    case LEVEL_BUG:     level_str = "bug";     break;
    }

    fprintf(self->fd, newformat, level_str, args);
    free(newformat);
}

report_provider_t report_filewriter_new(FILE* fd) {
    report_filewriter_env_t* env = malloc(sizeof(report_filewriter_env_t));
    *env = (report_filewriter_env_t) {
        .fd = fd
    };

    return (report_provider_t) {
        .env = env,
        .write = report_filewriter_write
    };
}