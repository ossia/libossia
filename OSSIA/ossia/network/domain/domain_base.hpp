#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/detail/math.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <type_traits>

namespace ossia
{
namespace net
{
template<typename T>
struct domain_base
{
        using value_type = typename value_trait<T>::value_type;
        boost::optional<value_type> min;
        boost::optional<value_type> max;
        boost::container::flat_set<value_type> values;

        domain_base() = default;
        domain_base(value_type min_v, value_type max_v):
            min{min_v},
            max{max_v}
        {

        }

        value clamp(
                bounding_mode b,
                const T& val) const
        {
            if(b == bounding_mode::FREE)
                return val;

            if(values.empty())
            {
                bool has_min = bool(min);
                bool has_max = bool(max);
                if(has_min && has_max)
                {
                    switch(b)
                    {
                        case bounding_mode::CLIP:
                            return T(ossia::clamp(val.value, *min, *max));
                        case bounding_mode::WRAP:
                            return T(ossia::wrap(val.value, *min, *max));
                        case bounding_mode::FOLD:
                            return T(ossia::fold(val.value, *min, *max));
                        default:
                            return val;
                    }
                }
                else if(has_min)
                {
                    if(b == bounding_mode::CLIP)
                        return T(val.value < *min ? *min : val.value);
                    else
                        return val;
                }
                else if(has_max)
                {
                    if(b == bounding_mode::CLIP)
                        return T(val.value > *max ? *max : val.value);
                    else
                        return val;
                }
                else
                {
                    return val;
                }
            }
            else
            {
                // Return a valid value only if it is in the given values
                auto it = values.find(val.value);
                if(it != values.end())
                {
                    return T(*it);
                }
                else
                {
                    return ossia::value{};
                }

                /* Alternative : return the closest element to value
                auto it = values.lower_bound(val.value);
                if(it != values.end())
                {
                    if(it == values.begin())
                    {
                        // The closest is the first element
                        return T(it);
                    }
                    else
                    {
                        // Find the closest element between this one and the previous.
                        auto prev = it - 1;
                        auto prev_diff = std::abs(val.value - *prev);
                        auto cur_diff = std::abs(val.value - *it);
                        return prev_diff > cur_diff ? *it : *prev;
                    }
                }
                else if(it == values.end())
                {
                    // Closest element is the latest
                    return T(*values.rbegin());
                }
                */
            }
        }
};

template<>
struct domain_base<Impulse>
{
        value clamp(
                bounding_mode b,
                const Impulse& val) const
        {
            return val;
        }
};

template<>
struct domain_base<Behavior>
{
        value clamp(
                bounding_mode b,
                const Behavior& val) const
        {
            return val;
        }
};

template<>
struct domain_base<Destination>
{
        value clamp(
                bounding_mode b,
                const Destination& val) const
        {
            return val;
        }
};

template<>
struct domain_base<String>
{
        boost::container::flat_set<std::string> values;
        value clamp(
                bounding_mode b,
                const String& val) const
        {
            if(values.empty())
            {
                return val;
            }
            else
            {
                auto it = values.find(val.value);
                if(it != values.end())
                {
                    return String(*it);
                }
                else
                {
                    return ossia::value{};
                }
            }
        }
};

template<>
struct domain_base<Tuple>
{
        value clamp(
                bounding_mode b,
                const Tuple& val) const
        {
            // TODO
            return val;
        }
};
template<int N>
struct domain_base<Vec<float, N>>
{
        value clamp(
                bounding_mode b,
                const Vec<float, N>& val) const
        {
            // TODO
            return val;
        }
};

using domain =
    eggs::variant<domain_base<Impulse>,
                  domain_base<Bool>,
                  domain_base<Int>,
                  domain_base<Float>,
                  domain_base<Char>,
                  domain_base<String>,
                  domain_base<Tuple>,
                  domain_base<Vec2f>,
                  domain_base<Vec3f>,
                  domain_base<Vec4f>,
                  domain_base<Destination>,
                  domain_base<Behavior>>;

}
}
