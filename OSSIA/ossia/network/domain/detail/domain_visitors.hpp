#pragma once
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{
struct domain_clamp_visitor
{
        bounding_mode b;

        template<typename T, typename U>
        ossia::value operator()(const T& value, const U& bad_domain)
        {
            return value;
        }

        template<typename T>
        ossia::value operator()(const T& value, const domain_base<T>& domain)
        {
            return domain.clamp(b, value);
        }
};

struct domain_min_visitor
{
        bounding_mode b;

        template<typename T>
        auto operator()(const T& value)
            -> decltype((T().min)(), ossia::value())
        {
            return value.min;
        }

        template<typename... T>
        auto operator()(const T&...)
        {
            return ossia::value{};
        }
};

struct domain_max_visitor
{
        bounding_mode b;

        template<typename T>
        auto operator()(const T& value)
            -> decltype((T().max)(), ossia::value())
        {
            return value.max;
        }

        template<typename... T>
        auto operator()(const T&...)
        {
            return ossia::value{};
        }
};

struct domain_creation_visitor
{
        domain operator()(const Int& min, const Int& max)
        { return domain_base<Int>{min.value, max.value}; }
        domain operator()(const Float& min, const Float& max)
        { return domain_base<Float>{min.value, max.value}; }
        domain operator()(const Bool& min, const Bool& max)
        { return domain_base<Bool>{min.value, max.value}; }
        domain operator()(const Char& min, const Char& max)
        { return domain_base<Char>{min.value, max.value}; }

        template<typename T>
        domain operator()(const T& min, const T& max)
        {
            // Cases where the domain has no min-max
            return domain_base<T>();
        }

        template<typename T, typename U>
        domain operator()(const T& min, const U& max)
        {
            // Cases where there is no possible domain
            return domain{};
        }
};
}
}
