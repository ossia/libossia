#pragma once
#include <Editor/Value/Int.h>
#include <Editor/Value/Impulse.h>
#include <Editor/Value/Float.h>
#include <Editor/Value/Bool.h>
#include <Editor/Value/Char.h>
#include <Editor/Value/Destination.h>
#include <Editor/Value/Behavior.h>
#include <Editor/Value/String.h>
#include <Editor/Value/Tuple.h>
#include <Editor/Value/Vec.h>

#include <Network/Address.h>

namespace OSSIA
{

namespace Comparisons
{
template<typename Kind, typename T>
bool equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 == v2; }); }

template<typename Kind, typename T>
bool different(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 != v2; }); }


template<typename Kind, typename T>
bool greater(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 > v2; }); }

template<typename Kind, typename T>
bool greater_equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 >= v2; }); }


template<typename Kind, typename T>
bool smaller(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 < v2; }); }

template<typename Kind, typename T>
bool smaller_equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 <= v2; }); }

struct Impulse_T
{
        template<typename T>
        friend bool operator==(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator!=(const T&, Impulse_T) { return false; }
        template<typename T>
        friend bool operator<=(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator>=(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator<(const T&, Impulse_T) { return false; }
        template<typename T>
        friend bool operator>(const T&, Impulse_T) { return true; }
};

struct NumericValue
{
        template<typename T, typename Fun>
        static bool apply(const T& lhs, const OSSIA::Value& v, Fun fun)
        {
          /*
            switch (v.getType())
            {
                case Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }
                case Type::BOOL :
                {
                    return fun(lhs.value, static_cast<const Bool&>(v).value);
                }
                case Type::INT :
                {
                    return fun(lhs.value, static_cast<const Int&>(v).value);
                }
                case Type::FLOAT :
                {
                    return fun(lhs.value, static_cast<const Float&>(v).value);
                }
                case Type::CHAR :
                {
                    return fun(lhs.value, static_cast<const Char&>(v).value);
                }
                case Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);
                    return (t.value.size() == 1) && (fun(lhs, *t.value[0]));
                }
                case Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);
                    if (d.value->getAddress())
                    {
                        auto c = d.value->getAddress()->cloneValue(d.index);
                        return fun(lhs, *c);
                    }
                    return false;
                }
                default :
                    return false;
            }
            */
        }
};

struct StringValue
{
        template<typename Fun>
        static bool apply(const String& lhs, const OSSIA::Value& v, Fun fun)
        {
          /*
            switch (v.getType())
            {
                case Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }
                case Type::STRING :
                {
                    return fun(lhs.value, static_cast<const String&>(v).value);
                }
                case Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);
                    return (t.value.size() == 1) && (fun(lhs, *t.value[0]));
                }
                case Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);
                    if (d.value->getAddress())
                    {
                        auto c = d.value->getAddress()->cloneValue(d.index);
                        return fun(lhs, *c);
                    }
                    return false;
                }
                default :
                    return false;
            }
            */
        }
};

struct TupleValue
{
        template<typename Fun>
        static bool apply(const Tuple& lhs, const OSSIA::Value& v, Fun fun)
        {
          /*
            switch (v.getType())
            {
                case Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }

                case Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);

                    if (lhs.value.size() != t.value.size())
                        return false;

                    bool result = true;
                    auto tit = t.value.begin();
                    for (const auto& val : lhs.value)
                    {
                        result &= fun(*val, **tit);
                        if (!result)
                            break;
                        tit++;
                    }

                    return result;
                }

                default :
                {
                    if (lhs.value.size() == 1)
                        return fun(*lhs.value[0], v);

                    return false;
                }
            }
            */
        }
};

struct DestinationValue
{
        template<typename Fun>
        static bool apply(const Destination& lhs, const OSSIA::Value& v, Fun fun)
        {
          /*
            switch (v.getType())
            {
                case Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }

                case Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);

                    // if there are addresses compare values
                    if (lhs.value->getAddress() && d.value->getAddress())
                    {
                        auto c1 = lhs.value->getAddress()->cloneValue(lhs.index);
                        auto c2 = d.value->getAddress()->cloneValue(d.index);
                        return fun(*c1, *c2);
                    }

                    // if no addresses, compare nodes
                    else if (!lhs.value->getAddress() && !d.value->getAddress())
                    {
                        return fun(lhs.value, d.value);
                    }
                    // TODO fallthrough ??
                }

                default :
                {
                    if (lhs.value->getAddress())
                    {
                        auto c = lhs.value->getAddress()->cloneValue(lhs.index);
                        return fun(*c, v);
                    }

                    return false;
                }
            }
            */
        }
};

struct VecValue
{
        template<typename Vec_T, typename Fun>
        static bool apply(const Vec_T& lhs, const OSSIA::Value& v, Fun fun)
        {
          /*
            const auto lhs_type = ValueTrait<Vec_T>::ossia_enum;
            const auto rhs_type = v.getType();

            if (rhs_type == Type::IMPULSE)
            {
                return fun(lhs.value, Impulse_T{});
            }
            else if(lhs_type == rhs_type)
            {
                auto& rhs = static_cast<const Vec_T&>(v);

                return fun(lhs.value, rhs.value);
            }
            else
            {
                return false;
            }
            */
        }
};
}

template<typename T, int N>
bool Vec<T, N>::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::VecValue>(*this, v); }

template<typename T, int N>
bool Vec<T, N>::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::VecValue>(*this, v); }

template<typename T, int N>
bool Vec<T, N>::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::VecValue>(*this, v); }

}
