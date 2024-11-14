// renum.h --- Renumber BASIC program lines by katahiromz
// Copyright (C) 2024 Katayama Hirofumi MZ
// License: MIT

#pragma once

#include <string>

#define RENUM_LINENO_START 10
#define RENUM_LINENO_STEP 10

typedef unsigned long renum_lineno_t;   // Line number
typedef int renum_error_t;              // Error code

/**
 * @brief Displays the version of the renum program.
 */
void RENUM_version(void);

/**
 * @brief Displays the usage information for the renum program.
 */
void RENUM_usage(void);

/**
 * @brief Adds line numbers to a BASIC program text.
 * @param text The BASIC program text to modify.
 * @param start The starting line number (default: 10).
 * @param step The increment step between lines (default: 10).
 * @param force Force renumbering even if an invalid line number is encountered.
 * @return Error code (0 for success).
 */
renum_error_t RENUM_add_line_numbers(
    std::string& text,
    renum_lineno_t start = RENUM_LINENO_START,
    renum_lineno_t step = RENUM_LINENO_STEP,
    bool force = false);

/**
 * @brief Renumbers the lines of a BASIC program text.
 * @param text The BASIC program text to modify.
 * @param new_start The new starting line number (default: 10).
 * @param old_start The old starting line number (default: 0).
 * @param step The increment step between lines (default: 10).
 * @param force Force renumbering even if an invalid line number is encountered.
 * @return Error code (0 for success).
 */
renum_error_t RENUM_renumber_lines(
    std::string& text,
    renum_lineno_t new_start = RENUM_LINENO_START,
    renum_lineno_t old_start = 0,
    renum_lineno_t step = RENUM_LINENO_STEP,
    bool force = false);
