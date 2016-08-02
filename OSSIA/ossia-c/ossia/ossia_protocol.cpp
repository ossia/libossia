#include "ossia_utils.hpp"
#include <iostream>
extern "C"
{
// MOVEME
void ossia_string_free(char* str)
{
    free(str); // Not delete because str has been allocated with strdup.
}

ossia_protocol_t ossia_protocol_local_create()
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{ossia::Local::create()};
    });
}

ossia_protocol_t ossia_protocol_osc_create(
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{ossia::OSC::create(ip, in_port, out_port)};
    });
}

ossia_protocol_t ossia_protocol_minuit_create(
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{ossia::Minuit::create(ip, in_port, out_port)};
    });
}
}
