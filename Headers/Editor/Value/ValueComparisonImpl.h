#pragma once
#include <Editor/Value/SafeValue.h>

#include <Network/Address.h>

namespace OSSIA
{

namespace Comparisons
{
template<typename Kind, typename T>
bool equal(const T& lhs, const OSSIA::SafeValue& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 == v2; }); }

template<typename Kind, typename T>
bool different(const T& lhs, const OSSIA::SafeValue& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 != v2; }); }


template<typename Kind, typename T>
bool greater(const T& lhs, const OSSIA::SafeValue& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 > v2; }); }

template<typename Kind, typename T>
bool greater_equal(const T& lhs, const OSSIA::SafeValue& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 >= v2; }); }


template<typename Kind, typename T>
bool smaller(const T& lhs, const OSSIA::SafeValue& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 < v2; }); }

template<typename Kind, typename T>
bool smaller_equal(const T& lhs, const OSSIA::SafeValue& rhs)
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
        static bool apply(const T& lhs, const OSSIA::SafeValue& val, Fun fun)
        {
          struct visitor {
            const T& lhs;
            Fun fun;
          public:
              using return_type = bool;
              return_type operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
              return_type operator()(Int v) const { return fun(lhs.value, v.value); }
              return_type operator()(Float v) const { return fun(lhs.value, v.value); }
              return_type operator()(Bool v) const { return fun(lhs.value, v.value); }
              return_type operator()(Char v) const { return fun(lhs.value, v.value); }
              return_type operator()(const Tuple& v) const
              { return (v.value.size() == 1) && (fun(lhs, v.value[0])); }
              return_type operator()(const Destination& d) const {
                if (const auto& addr = d.value->getAddress())
                {
                    return fun(lhs, addr->cloneValue(d.index));
                }
              }

              return_type operator()(const String& v) const { return false; }
              return_type operator()(Vec2f v) const { return false; }
              return_type operator()(Vec3f v) const { return false; }
              return_type operator()(Vec4f v) const { return false; }
              return_type operator()(const Behavior&) const { return false; }

          } vis{lhs, fun};

          if(val.valid())
          {
            return eggs::variants::apply(vis, val.v);
          }
          return false;
        }
};

struct StringValue
{
        template<typename Fun>
        static bool apply(const String& lhs, const OSSIA::SafeValue& v, Fun fun)
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
          return false;
        }
};

struct TupleValue
{
        template<typename Fun>
        static bool apply(const Tuple& lhs, const OSSIA::SafeValue& v, Fun fun)
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
          return false;
        }
};

struct DestinationValue
{
        template<typename Fun>
        static bool apply(const Destination& lhs, const OSSIA::SafeValue& v, Fun fun)
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
          return false;
        }
};

struct VecValue
{
        template<typename Vec_T, typename Fun>
        static bool apply(const Vec_T& lhs, const OSSIA::SafeValue& v, Fun fun)
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
          return false;
        }
};
}

template<typename T, int N>
bool Vec<T, N>::operator== (const SafeValue& v) const
{ return Comparisons::equal<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::VecValue>(*this, v); }

template<typename T, int N>
bool Vec<T, N>::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::VecValue>(*this, v); }

template<typename T, int N>
bool Vec<T, N>::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::VecValue>(*this, v); }
template<typename T, int N>
bool Vec<T, N>::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::VecValue>(*this, v); }

}
