//
// Created by pavlo on 17.07.24.
//

#ifndef MARS_SRC_REPORT_REPORTS_H
#define MARS_SRC_REPORT_REPORTS_H

#include <stdarg.h>

typedef enum {
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_EMERGENCY,
    LEVEL_BUG
} report_level_t;

typedef void (*report_writer_t)(void* self, report_level_t level, const char* fmt, ...);

typedef struct {
    void* env;
    report_writer_t write;
} reporter_t;

reporter_t report_new(void* env, report_writer_t writer);

void report_write(reporter_t* self, report_level_t level, char* fmt, ...);
void report_info(reporter_t* self, char* fmt, ...);
void report_warning(reporter_t* self, char* fmt, ...);
void report_error(reporter_t* self, char* fmt, ...);
void report_emergency(reporter_t* self, char* fmt, ...);
void report_bug(reporter_t* self, char* fmt, ...);

#endif //MARS_SRC_REPORT_REPORTS_H
