#include "ossia_utils.hpp"

extern "C"
{
ossia_value_t ossia_domain_get_min(
        ossia_domain_t domain)
{
    return safe_function(__func__, [=] () -> ossia_value_t {
        if(!domain)
            return nullptr;

        return convert(ossia::net::min(domain->domain));
    });
}

void ossia_domain_set_min(
        ossia_domain_t domain,
        ossia_value_t value)
{
    return safe_function(__func__, [=] {
        if(!domain)
            return;
        if(!value)
            return;

        ossia::net::set_min(domain->domain)
        domain->domain->setMin(convert(value));
    });
}

ossia_value_t ossia_domain_get_max(
        ossia_domain_t domain)
{
    return safe_function(__func__, [=] () -> ossia_value_t {
        if(!domain)
            return nullptr;

        return convert(domain->domain->getMax());
    });
}

void ossia_domain_set_max(
        ossia_domain_t domain,
        ossia_value_t value)
{
    return safe_function(__func__, [=] {
        if(!domain)
            return;
        if(!value)
            return;

        domain->domain->setMax(convert(value));
    });
}

void ossia_domain_free(ossia_domain_t domain)
{
    return safe_function(__func__, [=] {
        delete domain;
    });
}

}
