// renum.h --- Renumber BASIC program lines
// License: MIT

#pragma once

#include <string>

#ifndef RENUM_LINENO_START
    #define RENUM_LINENO_START 10
#endif
#ifndef RENUM_LINENO_STEP
    #define RENUM_LINENO_STEP 10
#endif

typedef unsigned long renum_lineno_t;
typedef int renum_error_t;

void RENUM_version(void);
void RENUM_usage(void);

renum_error_t RENUM_add_line_numbers(std::string& text, renum_lineno_t start = RENUM_LINENO_START, renum_lineno_t step = RENUM_LINENO_STEP);
renum_error_t RENUM_renumber_lines(std::string& text, renum_lineno_t start = RENUM_LINENO_START, renum_lineno_t step = RENUM_LINENO_STEP);
