#include "ossia_utils.hpp"
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
        return new ossia_protocol(new ossia::net::local_protocol);
    });
}

void ossia_protocol_local_expose_to(
    ossia_protocol_t local,
    ossia_protocol_t other)
{
    return safe_function(__func__, [=] {
        auto ptr = dynamic_cast<ossia::net::local_protocol*>(local->protocol);
        if(ptr)
        {
          ptr->exposeTo(std::unique_ptr<ossia::net::protocol_base>(other->protocol));
        }
    });
}

ossia_protocol_t ossia_protocol_osc_create(
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{new ossia::net::osc_protocol(ip, in_port, out_port)};
    });
}

ossia_protocol_t ossia_protocol_minuit_create(
        const char* local_name,
        const char* ip,
        int in_port,
        int out_port)
{
    return safe_function(__func__, [=] {
        return new ossia_protocol{new ossia::net::minuit_protocol(local_name, ip, in_port, out_port)};
    });
}

void ossia_protocol_free(
    ossia_protocol_t proto)
{
  return safe_function(__func__, [=] {
    if(proto)
      delete proto;
  });
}
}
