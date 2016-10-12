#pragma once
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>

namespace ossia
{

namespace comparisons
{
struct Impulse_T
{
  template <typename T>
  friend bool operator==(const T&, Impulse_T)
  {
    return true;
  }
  template <typename T>
  friend bool operator!=(const T&, Impulse_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator<=(const T&, Impulse_T)
  {
    return true;
  }
  template <typename T>
  friend bool operator>=(const T&, Impulse_T)
  {
    return true;
  }
  template <typename T>
  friend bool operator<(const T&, Impulse_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator>(const T&, Impulse_T)
  {
    return false;
  }
};
struct String_T
{
  template <typename T>
  friend bool operator==(const T&, String_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator!=(const T&, String_T)
  {
    return true;
  }
  template <typename T>
  friend bool operator<=(const T&, String_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator>=(const T&, String_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator<(const T&, String_T)
  {
    return false;
  }
  template <typename T>
  friend bool operator>(const T&, String_T)
  {
    return false;
  }
};

struct NumericValue
{
  template <typename T, typename Fun>
  static bool apply(const T& lhs, const ossia::value& val, Fun fun)
  {
    struct visitor
    {
      const T& lhs;
      Fun fun;

    public:
      bool operator()(Impulse) const
      {
        return fun(lhs.value, Impulse_T{});
      }
      bool operator()(Int v) const
      {
        return fun(lhs.value, v.value);
      }
      bool operator()(Float v) const
      {
        return fun(lhs.value, v.value);
      }
      bool operator()(Bool v) const
      {
        return fun(lhs.value, v.value);
      }
      bool operator()(Char v) const
      {
        return fun(lhs.value, v.value);
      }
      bool operator()(const Tuple& v) const
      {
        return (v.value.size() == 1) && (fun(lhs, v.value[0]));
      }
      bool operator()(const Destination& d) const
      {
        return fun(lhs, d.value.get().cloneValue(d.index));
      }

      bool operator()(const String& v) const
      {
        return fun(lhs.value, String_T{});
      }
      bool operator()(Vec2f v) const
      {
        return false;
      }
      bool operator()(Vec3f v) const
      {
        return false;
      }
      bool operator()(Vec4f v) const
      {
        return false;
      }
      bool operator()(const Behavior&) const
      {
        return false;
      }

      bool operator()() const
      {
        return false;
      }

    } vis{lhs, fun};

    return val.apply(vis);
  }
};

struct StringValue
{
  template <typename Fun>
  static bool apply(const String& lhs, const ossia::value& val, Fun fun)
  {
    struct visitor
    {
      const String& lhs;
      Fun fun;

    public:
      bool operator()(Impulse) const
      {
        return fun(lhs.value, Impulse_T{});
      }
      bool operator()(const String& v) const
      {
        return fun(lhs.value, v.value);
      }
      bool operator()(Int v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(Float v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(Bool v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(Char v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(const Tuple& v) const
      {
        return (v.value.size() == 1) && (fun(lhs, v.value[0]));
      }

      bool operator()(const Destination& d) const
      {
        return fun(lhs, d.value.get().cloneValue(d.index));
      }

      bool operator()(Vec2f v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(Vec3f v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(Vec4f v) const
      {
        return fun(v.value, String_T{});
      }
      bool operator()(const Behavior&) const
      {
        return false;
      }

      bool operator()() const
      {
        return false;
      }

    } vis{lhs, fun};

    return val.apply(vis);
  }
};

template <typename Fun>
struct TupleVisitor
{
  const Tuple& lhs;
  const ossia::value& rhs;
  Fun fun;

public:
  bool operator()(Impulse) const
  {
    return fun(lhs.value, Impulse_T{});
  }
  bool operator()(const Tuple& t) const
  {
    if (lhs.value.size() != t.value.size())
      return false;

    bool result = true;
    auto tit = t.value.begin();
    for (const auto& val : lhs.value)
    {
      result &= fun(val, *tit);
      if (!result)
        break;
      tit++;
    }

    return result;
  }

  template <typename T>
  bool operator()(const T& v) const
  {
    if (lhs.value.size() == 1)
      return fun(lhs.value[0], rhs);

    return false;
  }

  bool operator()() const
  {
    return false;
  }
};
template <typename Fun>
auto make_tuple_visitor(const Tuple& lhs, const ossia::value& val, Fun f)
{
  return TupleVisitor<Fun>{lhs, val, f};
}

struct TupleValue
{
  template <typename Fun>
  static bool apply(const Tuple& lhs, const ossia::value& val, Fun fun)
  {
    auto vis = make_tuple_visitor(lhs, val, fun);

    return val.apply(vis);
  }
};

template <typename Fun>
struct DestinationVisitor
{
  const Destination& lhs;
  const ossia::value& rhs;
  Fun fun;

public:
  bool operator()(Impulse) const
  {
    return fun(lhs.value.get(), Impulse_T{});
  }
  bool operator()(const Destination& d) const
  {
    return fun(lhs.value.get().cloneValue(lhs.index), d.value.get().cloneValue(d.index));
  }

  template <typename T>
  bool operator()(const T& v) const
  {
    return fun(lhs.value.get().cloneValue(lhs.index), rhs);
  }

  bool operator()() const
  {
    return false;
  }
};

template <typename Fun>
auto make_destination_visitor(
    const Destination& lhs, const ossia::value& val, Fun f)
{
  return DestinationVisitor<Fun>{lhs, val, f};
}

struct DestinationValue
{
  template <typename Fun>
  static bool apply(const Destination& lhs, const ossia::value& val, Fun fun)
  {
    return val.apply(make_destination_visitor(lhs, val, fun));
  }
};

template <std::size_t N, typename Fun>
struct VecVisitor
{
  const Vec<float, N>& lhs;
  Fun fun;

public:
  bool operator()(Impulse) const
  {
    return fun(lhs.value, Impulse_T{});
  }
  bool operator()(const Vec<float, N>& d) const
  {
    return fun(lhs.value, d.value);
  }

  template <typename T>
  bool operator()(const T& v) const
  {
    return false;
  }

  bool operator()() const
  {
    return false;
  }
};

template <typename Vec_T, typename Fun>
auto make_vec_visitor(const Vec_T& lhs, Fun f)
{
  return VecVisitor<Vec_T::size_value, Fun>{lhs, f};
}

struct VecValue
{
  template <typename Vec_T, typename Fun>
  static bool apply(const Vec_T& lhs, const ossia::value& val, Fun fun)
  {
    return val.apply(make_vec_visitor(lhs, fun));
  }
};
}

template <typename T, std::size_t N>
bool Vec<T, N>::operator==(const ossia::value& v) const
{
  return comparisons::VecValue::apply(*this, v, std::equal_to<>{});
}
template <typename T, std::size_t N>
bool Vec<T, N>::operator!=(const ossia::value& v) const
{
  return !comparisons::VecValue::apply(*this, v, std::equal_to<>{});
}

template <typename T, std::size_t N>
bool Vec<T, N>::operator>(const ossia::value& v) const
{
  return comparisons::VecValue::apply(*this, v, std::greater<>{});
}
template <typename T, std::size_t N>
bool Vec<T, N>::operator>=(const ossia::value& v) const
{
  return comparisons::VecValue::apply(*this, v, std::greater_equal<>{});
}

template <typename T, std::size_t N>
bool Vec<T, N>::operator<(const ossia::value& v) const
{
  return comparisons::VecValue::apply(*this, v, std::less<>{});
}
template <typename T, std::size_t N>
bool Vec<T, N>::operator<=(const ossia::value& v) const
{
  return comparisons::VecValue::apply(*this, v, std::less_equal<>{});
}
}
