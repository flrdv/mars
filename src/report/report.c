//
// Created by pavlo on 28.07.24.
//

#include "report.h"

reporter_t report_new(void* env, report_writer_t writer) {
    return (reporter_t) {
        .env = env,
        .write = writer
    };
}

#define VARARGS(into)   \
    va_list into;       \
    va_start(into, fmt) \

void report_write(reporter_t* self, report_level_t level, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, level, fmt, args);
}

void report_info(reporter_t* self, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, LEVEL_INFO, fmt, args);
}

void report_warning(reporter_t* self, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, LEVEL_WARNING, fmt, args);
}

void report_error(reporter_t* self, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, LEVEL_ERROR, fmt, args);
}

void report_emergency(reporter_t* self, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, LEVEL_EMERGENCY, fmt, args);
}

void report_bug(reporter_t* self, char* fmt, ...) {
    VARARGS(args);
    self->write(self->env, LEVEL_BUG, fmt, args);
}
