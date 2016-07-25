#pragma once
#include <Editor/Value/Value.h>
#include <Editor/Value/ValueTraits.h>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <cmath>

namespace OSSIA
{
namespace v2
{

namespace maths
{
// http://stackoverflow.com/a/16659263/1495627
template<class T>
constexpr T clamp(T d, T min, T max)
{
  const T t = d < min ? min : d;
  return t > max ? max : t;
}

// Wrap & Fold code taken from Jamoma TTLimits.h.
// Credits : Nils Peters, Nov. 2008
template<class T>
constexpr T wrap(T val, const T low, const T high)
{
    if ((val >= low) && (val < high))
        return val;
    else if (val - low >= 0)
        return std::fmod(val - low, std::fabs(low - high)) + low;
    else
        return -1.0 * std::fmod(-1.0 * (val  - low), std::fabs(low - high)) + high;
}

template<class T>
constexpr T fold(T val, const T low, const T high)
{
    if ((val >= low) && (val <= high))
        return val;
    else {
        return std::fabs(
                    std::remainder(
                        val - low,
                        2. * std::fabs(low - high)
                        )
                    ) + low;
    }
}
}

template<typename T>
struct DomainBase
{
        using value_type = typename ValueTrait<T>::value_type;
        boost::optional<value_type> min;
        boost::optional<value_type> max;
        boost::container::flat_set<value_type> values;

        Value clamp(
                BoundingMode b,
                const T& val) const
        {
            if(b == BoundingMode::FREE)
                return val;

            if(values.empty())
            {
                bool has_min = min;
                bool has_max = max;
                if(has_min && has_max)
                {
                    switch(b)
                    {
                        case BoundingMode::CLIP:
                            return T(maths::clamp(val.value, *min, *max));
                        case BoundingMode::WRAP:
                            return T(maths::wrap(val.value, *min, *max));
                        case BoundingMode::FOLD:
                            return T(maths::fold(val.value, *min, *max));
                        default:
                            return val;
                    }
                }
                else if(has_min)
                {
                    if(b == BoundingMode::CLIP)
                        return T(val.value < *min ? *min : val.value);
                    else
                        return val;
                }
                else if(has_max)
                {
                    if(b == BoundingMode::CLIP)
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
                    return OSSIA::Value{};
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
struct DomainBase<Impulse>
{
        Value clamp(
                BoundingMode b,
                const Impulse& val) const
        {
            return val;
        }
};

template<>
struct DomainBase<Behavior>
{
        Value clamp(
                BoundingMode b,
                const Behavior& val) const
        {
            return val;
        }
};

template<>
struct DomainBase<Destination>
{
        Value clamp(
                BoundingMode b,
                const Destination& val) const
        {
            return val;
        }
};

template<>
struct DomainBase<String>
{

        boost::container::flat_set<std::string> values;
        Value clamp(
                BoundingMode b,
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
                    return OSSIA::Value{};
                }
            }
        }
};

template<>
struct DomainBase<Tuple>
{
        Value clamp(
                BoundingMode b,
                const Tuple& val) const
        {
            // TODO
            return val;
        }
};
template<int N>
struct DomainBase<Vec<float, N>>
{
        Value clamp(
                BoundingMode b,
                const Vec<float, N>& val) const
        {
            // TODO
            return val;
        }
};



using Domain =
    eggs::variant<DomainBase<Impulse>,
                  DomainBase<Bool>,
                  DomainBase<Int>,
                  DomainBase<Float>,
                  DomainBase<Char>,
                  DomainBase<String>,
                  DomainBase<Tuple>,
                  DomainBase<Vec2f>,
                  DomainBase<Vec3f>,
                  DomainBase<Vec4f>,
                  DomainBase<Destination>,
                  DomainBase<Behavior>>;

struct DomainClampVisitor
{
        BoundingMode b;

        template<typename T, typename U>
        OSSIA::Value operator()(const T& value, const U& bad_domain)
        {
            return value;
        }

        template<typename T>
        OSSIA::Value operator()(const T& value, const DomainBase<T>& domain)
        {
            return domain.clamp(b, value);
        }
};

struct DomainMinVisitor
{
        BoundingMode b;

        template<typename T>
        auto operator()(const T& value)
            -> decltype((T().min)(), OSSIA::Value())
        {
            return value.min;
        }

        template<typename... T>
        auto operator()(const T&...)
        {
            return OSSIA::Value{};
        }
};

struct DomainMaxVisitor
{
        BoundingMode b;

        template<typename T>
        auto operator()(const T& value)
            -> decltype((T().max)(), OSSIA::Value())
        {
            return value.max;
        }

        template<typename... T>
        auto operator()(const T&...)
        {
            return OSSIA::Value{};
        }
};

inline Value clamp(
        const Domain& dom,
        BoundingMode b,
        const Value& val)
{
    if(bool(dom) && bool(val.v))
    {
        return eggs::variants::apply(DomainClampVisitor{b}, val.v, dom);
    }
    return val;
}

inline Value min(const Domain& dom)
{
    if(dom)
    {
        return eggs::variants::apply(DomainMinVisitor{}, dom);
    }
    return {};
}

inline Value max(const Domain& dom)
{
    if(dom)
    {
        return eggs::variants::apply(DomainMaxVisitor{}, dom);
    }
    return {};
}
}
}
