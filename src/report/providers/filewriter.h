//
// Created by pavlo on 19.07.24.
//

#ifndef MARS_SRC_REPORT_PROVIDERS_FILEWRITER_H
#define MARS_SRC_REPORT_PROVIDERS_FILEWRITER_H

#include "report/report.h"

report_provider_t report_filewriter_new(FILE* fd);

#endif //MARS_SRC_REPORT_PROVIDERS_FILEWRITER_H
