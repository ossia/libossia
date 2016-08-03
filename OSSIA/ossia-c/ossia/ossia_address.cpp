#include "ossia_utils.hpp"

extern "C"
{
void ossia_address_set_access_mode(
        ossia_address_t address,
        ossia_access_mode am)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;

        convert_address(address)->setAccessMode(convert(am));
    });
}

ossia_access_mode ossia_address_get_access_mode(ossia_address_t address)
{
    return safe_function(__func__, [=] {
        if(!address)
            return ossia_access_mode{};

        return convert(convert_address(address)->getAccessMode());
    });
}

void ossia_address_set_bounding_mode(
        ossia_address_t address,
        ossia_bounding_mode am)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;

        convert_address(address)->setBoundingMode(convert(am));
    });
}

ossia_bounding_mode ossia_address_get_bounding_mode(
        ossia_address_t address)
{
    return safe_function(__func__, [=] {
        if(!address)
            return ossia_bounding_mode{};

        return convert(convert_address(address)->getBoundingMode());
    });
}


void ossia_address_set_domain(
        ossia_address_t address,
        ossia_domain_t domain)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;
        if(!domain)
            return;

        convert_address(address)->setDomain(domain->domain);
    });
}

ossia_domain_t ossia_address_get_domain(
        ossia_address_t address)
{
    return safe_function(__func__, [=] () -> ossia_domain_t {
        if(!address)
            return nullptr;

        return new ossia_domain{convert_address(address)->getDomain()};
    });
}

void ossia_address_set_value(
        ossia_address_t address,
        ossia_value_t value)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;
        if(!value)
            return;

        convert_address(address)->setValue(convert(value));
    });
}

ossia_value_t ossia_address_clone_value(
        ossia_address_t address)
{
    return safe_function(__func__, [=] () -> ossia_value_t  {
        if(!address)
            return nullptr;

        return convert(convert_address(address)->cloneValue());
    });
}

void ossia_address_push_value(
        ossia_address_t address,
        ossia_value_t value)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;
        if(!value)
            return;

        convert_address(address)->pushValue(convert(value));
    });
}

ossia_value_t ossia_address_pull_value(
        ossia_address_t address)
{
    return safe_function(__func__, [=] () -> ossia_value_t {
        if(!address)
            return nullptr;

        return convert(convert_address(address)->fetchValue());
    });
}

ossia_value_callback_index_t ossia_address_add_callback(
        ossia_address_t address,
        ossia_value_callback_t callback)
{
    return safe_function(__func__, [=] () -> ossia_value_callback_index_t {
        if(!address)
            return nullptr;
        if(!callback)
            return nullptr;

        return new ossia_value_callback_index{
            convert_address(address)->add_callback([=] (const ossia::value& val)
            {
                DEBUG_LOG_FMT("inside added callback");
                callback(convert(val));
            })
        };
    });
}

void ossia_address_remove_callback(
        ossia_address_t address,
        ossia_value_callback_index_t index)
{
    return safe_function(__func__, [=] {
        if(!address)
            return;
        if(!index)
            return;

        convert_address(address)->remove_callback(index->it);
        delete index;
    });
}

}
