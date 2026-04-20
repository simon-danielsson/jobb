/*

-------------- Details --------------
Name     : Analib
Version  : 0.2.3
Repo     : https://github.com/simon-danielsson/analib.h

Author   : Simon Danielsson
Email    : contact@simondanielsson.se
Website  : https://www.simondanielsson.se/

------------ MIT License ------------
Copyright © 2026 Simon Danielsson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files, to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef ANALIB_H_
#define ANALIB_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ANALIB_DEF
#define ANALIB_DEF
#endif // ANALIB_DEF

// INTERNAL DEFINITIONS
// =============================================================================

typedef struct {
  char *msg_col;
  int line;
  const char *function;
  const char *file;
  char *type;
  const char *msg;
} _al_db_type;

// generic label builder for debug functions
static inline void _al_db_lbl(const char *label, char *header,
                              int header_size) {
  int label_size = strlen(label);
  int wings_len = (header_size - label_size);

  if (wings_len <= 0 || header_size <= 0) {
    if (header_size > 0)
      header[0] = '\0';
    return;
  }

  char wing[wings_len + 1];
  memset(wing, ' ', wings_len);
  wing[wings_len] = '\0';

  snprintf(header, header_size, " %s %s", label, wing);
}

#define _al_reset_clr "\033[0m"
// generic msg call for debug functions
static inline void _al_db_msg(_al_db_type *t) {
  char label[9];
  _al_db_lbl(t->type, label, 9);

  char space[11 + 1];
  memset(space, ' ', 11);
  space[11] = '\0';

  fprintf(stderr, "%s%-8s%s%s:%d (%s)%-2s%-32s\n", t->msg_col, t->type,
          _al_reset_clr, t->file, t->line, t->function, " ", t->msg);
}

static inline int _al_min_int(int a, int b) { return a < b ? a : b; }
static inline int _al_max_int(int a, int b) { return a > b ? a : b; }
static inline double _al_min_double(double a, double b) {
  return a < b ? a : b;
}
static inline double _al_max_double(double a, double b) {
  return a > b ? a : b;
}

// PUBLIC API
// =============================================================================

#define _al_log_clr "\033[34m"
#define _al_assert_clr "\033[31m"
#define _al_error_clr "\033[31m"
#define _al_assert_succ_clr "\033[32m"
#define _al_todo_clr "\033[33m"

// formatted assert message
// prints to stderr regardless of condition
// does not abort on do_abort=true if condition was true
#define AL_assert(cond, do_abort)                                              \
  do {                                                                         \
    char msg[32];                                                              \
    char msg_col[32];                                                          \
    if (!(cond)) {                                                             \
      snprintf(msg, sizeof(msg), "failure -> %s", #cond);                      \
      snprintf(msg_col, sizeof(msg_col), "%s", _al_assert_clr);                \
    } else {                                                                   \
      snprintf(msg, sizeof(msg), "success -> %s", #cond);                      \
      snprintf(msg_col, sizeof(msg_col), "%s", _al_assert_succ_clr);           \
    }                                                                          \
    _al_db_msg(&(_al_db_type){.msg_col = msg_col,                              \
                              .line = __LINE__,                                \
                              .function = __func__,                            \
                              .file = __FILE_NAME__,                           \
                              .type = "[DEBUG]",                               \
                              .msg = msg});                                    \
    if (!(cond) && (do_abort)) {                                               \
      abort();                                                                 \
    }                                                                          \
  } while (0)

// formatted log message
#define AL_log(fmt, ...)                                                       \
  do {                                                                         \
    char msg[128];                                                             \
    snprintf(msg, sizeof(msg), fmt __VA_OPT__(, ) __VA_ARGS__);                \
    _al_db_msg(&(_al_db_type){.msg_col = _al_log_clr,                          \
                              .line = __LINE__,                                \
                              .function = __func__,                            \
                              .file = __FILE_NAME__,                           \
                              .type = "[INFO]",                                \
                              .msg = msg});                                    \
  } while (0)

// formatted log message
#define AL_error(fmt, ...)                                                     \
  do {                                                                         \
    char msg[128];                                                             \
    snprintf(msg, sizeof(msg), fmt __VA_OPT__(, ) __VA_ARGS__);                \
    _al_db_msg(&(_al_db_type){.msg_col = _al_error_clr,                        \
                              .line = __LINE__,                                \
                              .function = __func__,                            \
                              .file = __FILE_NAME__,                           \
                              .type = "[ERROR]",                               \
                              .msg = msg});                                    \
    exit(1);                                                                   \
  } while (0)

// rust-like formatted todo message that aborts the program if reached

#define AL_todo(fmt, ...)                                                      \
  do {                                                                         \
    char msg[128];                                                             \
    snprintf(msg, sizeof(msg), fmt __VA_OPT__(, ) __VA_ARGS__);                \
    _al_db_msg(&(_al_db_type){.msg_col = _al_todo_clr,                         \
                              .line = __LINE__,                                \
                              .function = __func__,                            \
                              .file = __FILE_NAME__,                           \
                              .type = "[TODO]",                                \
                              .msg = msg});                                    \
    abort();                                                                   \
  } while (0)

// compare two integers or doubles and return the smaller one
#define AL_cmp_min(a, b)                                                       \
  _Generic((a) + (b), int: _al_min_int, double: _al_min_double)(a, b)

// compare two integers or doubles and return the bigger one
#define AL_cmp_max(a, b)                                                       \
  _Generic((a) + (b), int: _al_max_int, double: _al_max_double)(a, b)

#endif // ANALIB_H_

// IMPLEMENTATIONS
// =============================================================================

#ifdef ANALIB_IMPLEMENTATION

#endif // ANALIB_IMPLEMENTATION
