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
    LEVEL_BUG
} report_level_t;

typedef struct {
    void* env;
    void (*write)(void* self, report_level_t level, const char* fmt, ...);
} report_provider_t;

typedef struct {
    report_provider_t provider;
} reporter_t;

static reporter_t report_new(report_provider_t provider) {
    return (reporter_t) {
        .provider = provider
    };
}

static void report_write(reporter_t* self, report_level_t level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    self->provider.write(self->provider.env, level, format, args);
}

#define REPORT_INFO(self, format, ...)    report_write((self), LEVEL_INFO, (format), __VA_ARGS__)
#define REPORT_WARNING(self, format, ...) report_write((self), LEVEL_WARNING, (format), __VA_ARGS__)
#define REPORT_ERROR(self, format, ...)   report_write((self), LEVEL_ERROR, (format), __VA_ARGS__)
#define REPORT_BUG(self, format, ...)     report_write((self), LEVEL_BUG, (format), __VA_ARGS__)

#endif //MARS_SRC_REPORT_REPORTS_H
