#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <cmath>
#include <type_traits>

namespace ossia
{
namespace net
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

        DomainBase(value_type min_v, value_type max_v):
            min{min_v},
            max{max_v}
        {

        }

        value clamp(
                BoundingMode b,
                const T& val) const
        {
            if(b == BoundingMode::FREE)
                return val;

            if(values.empty())
            {
                bool has_min = bool(min);
                bool has_max = bool(max);
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
struct DomainBase<Impulse>
{
        value clamp(
                BoundingMode b,
                const Impulse& val) const
        {
            return val;
        }
};

template<>
struct DomainBase<Behavior>
{
        value clamp(
                BoundingMode b,
                const Behavior& val) const
        {
            return val;
        }
};

template<>
struct DomainBase<Destination>
{
        value clamp(
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
        value clamp(
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
                    return ossia::value{};
                }
            }
        }
};

template<>
struct DomainBase<Tuple>
{
        value clamp(
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
        value clamp(
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
        ossia::value operator()(const T& value, const U& bad_domain)
        {
            return value;
        }

        template<typename T>
        ossia::value operator()(const T& value, const DomainBase<T>& domain)
        {
            return domain.clamp(b, value);
        }
};

struct DomainMinVisitor
{
        BoundingMode b;

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

struct DomainMaxVisitor
{
        BoundingMode b;

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

inline value clamp(
        const Domain& dom,
        BoundingMode b,
        const value& val)
{
    if(bool(dom) && bool(val.v))
    {
        return eggs::variants::apply(DomainClampVisitor{b}, val.v, dom);
    }
    return val;
}

inline value min(const Domain& dom)
{
    if(dom)
    {
        return eggs::variants::apply(DomainMinVisitor{}, dom);
    }
    return {};
}

inline value max(const Domain& dom)
{
    if(dom)
    {
        return eggs::variants::apply(DomainMaxVisitor{}, dom);
    }
    return {};
}

struct DomainCreationVisitor
{
        Domain operator()(const Int& min, const Int& max)
        { return DomainBase<Int>{min.value, max.value}; }
        Domain operator()(const Float& min, const Float& max)
        { return DomainBase<Float>{min.value, max.value}; }
        Domain operator()(const Bool& min, const Bool& max)
        { return DomainBase<Bool>{min.value, max.value}; }
        Domain operator()(const Char& min, const Char& max)
        { return DomainBase<Char>{min.value, max.value}; }

        template<typename T>
        Domain operator()(const T& min, const T& max)
        {
            // Cases where the domain has no min-max
            return DomainBase<T>();
        }

        template<typename T, typename U>
        Domain operator()(const T& min, const U& max)
        {
            // Cases where there is no possible domain
            return Domain{};
        }
};

inline Domain makeDomain(const ossia::value& min, const ossia::value& max)
{
    if(min.valid() && max.valid())
    {
        return eggs::variants::apply(DomainCreationVisitor{}, min.v, max.v);
    }
    return {};
}

struct DomainEqualVisitor
{
        template<typename T>
        bool operator()(const T& min, const T& max)
        {
            // Cases where the domain has no min-max
            return DomainBase<T>();
        }

        template<typename T, typename U>
        bool operator()(const T& min, const U& max)
        {
            // Cases where there is no possible domain
            return Domain{};
        }
};

inline bool operator==(const DomainBase<Int>& lhs, const DomainBase<Int>& rhs)
{ return lhs.min == rhs.min && lhs.max == rhs.max; }
inline bool operator==(const DomainBase<Float>& lhs, const DomainBase<Float>& rhs)
{ return lhs.min == rhs.min && lhs.max == rhs.max; }
inline bool operator==(const DomainBase<Bool>& lhs, const DomainBase<Bool>& rhs)
{ return lhs.min == rhs.min && lhs.max == rhs.max; }
inline bool operator==(const DomainBase<Char>& lhs, const DomainBase<Char>& rhs)
{ return lhs.min == rhs.min && lhs.max == rhs.max; }
inline bool operator==(const DomainBase<String>& lhs, const DomainBase<String>& rhs)
{ return lhs.values == rhs.values; }

template<typename T>
bool operator==(const DomainBase<T>& lhs, const DomainBase<T>& rhs)
{
  return true;
}
}
}
