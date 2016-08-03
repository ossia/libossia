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
        return new ossia_protocol{std::make_unique<ossia::net::local_protocol>()};
    });
}

ossia_protocol_t ossia_protocol_osc_create(
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{std::make_unique<ossia::net::osc_protocol>(ip, in_port, out_port)};
    });
}

ossia_protocol_t ossia_protocol_minuit_create(
        const char* local_name,
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{std::make_unique<ossia::net::minuit_protocol>(local_name, ip, in_port, out_port)};
    });
}
}
