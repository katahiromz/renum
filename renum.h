// renum.h --- Renumber BASIC program lines
// License: MIT

#pragma once

#include <string>

typedef unsigned long renum_lineno_t;
typedef int renum_error_t;

void RENUM_version(void);
void RENUM_usage(void);

renum_error_t RENUM_add_line_numbers(std::string& text, renum_lineno_t start = 10, renum_lineno_t step = 10);
renum_error_t RENUM_renumber_lines(std::string& text, renum_lineno_t start = 10, renum_lineno_t step = 10);
