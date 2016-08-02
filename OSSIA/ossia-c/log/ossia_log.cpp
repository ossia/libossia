#include "ossia_log.h"

extern "C"
{
static ossia_log_fun_t log_fun = nullptr;
void ossia_set_debug_logger(ossia_log_fun_t fp)
{
    log_fun = fp;
}

void ossia_log_error(const char* error)
{
    if(log_fun)
        log_fun(error);
}
}
