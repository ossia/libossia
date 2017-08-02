// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_log.h"
#include <cstdio>
extern "C" {
void basic_log(const char* error)
{
  std::fprintf(stderr, "%s\n", error);
}

static ossia_log_fun_t log_fun = basic_log;
void ossia_set_debug_logger(ossia_log_fun_t fp)
{
  log_fun = fp;
}

void ossia_log_error(const char* error)
{
  if (log_fun)
    log_fun(error);
}
}
