#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/editor/curve/curve_abstract.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

/**
 * \file behavior.hpp
 */
namespace ossia
{

template <typename X, typename Y>
class curve;

struct behavior;
struct behavior_variant_type
{
public:
  struct dummy_t
  {
  };
  union Impl
  {
    std::shared_ptr<ossia::curve_abstract> m_value0;

    std::vector<ossia::behavior> m_value1;

    dummy_t m_dummy;
    Impl()
        : m_dummy{}
    {
    }
    ~Impl() { }
  };

  enum Type : int8_t
  {
    Type0,
    Type1,
    Npos
  };

  void destruct_impl();
  Impl m_impl;
  Type m_type;

public:
  static const constexpr auto npos = Npos;
  [[nodiscard]] int which() const;

  operator bool() const;
  template <typename T>
  const T* target() const;
  template <typename T>
  T* target();
  template <typename T>
  const T& get() const;
  template <typename T>
  T& get();

  template <typename T>
  static Type matching_type();
  behavior_variant_type();
  ~behavior_variant_type();
  behavior_variant_type(const std::shared_ptr<ossia::curve_abstract>& v);
  behavior_variant_type(std::shared_ptr<ossia::curve_abstract>&& v);
  behavior_variant_type(const std::vector<ossia::behavior>& v);
  behavior_variant_type(std::vector<ossia::behavior>&& v);
  behavior_variant_type(const behavior_variant_type& other);
  behavior_variant_type(behavior_variant_type&& other) noexcept;
  behavior_variant_type& operator=(const behavior_variant_type& other);
  behavior_variant_type& operator=(behavior_variant_type&& other) noexcept;
};
/**
 * @class Contains either no curve, a single curve, or multiple curves
 *
 * \see \ref curve_abstract
 * \see \ref curve
 */
struct OSSIA_EXPORT behavior final
{
  behavior_variant_type v;

  behavior() = default;
  behavior(const behavior&) = default;
  behavior(behavior&&) = default;
  behavior& operator=(const behavior&) = default;
  behavior& operator=(behavior&&) = default;

  behavior(std::shared_ptr<ossia::curve_abstract> c)
      : v{std::move(c)}
  {
  }
  template <typename T, typename U>
  behavior(std::shared_ptr<ossia::curve<T, U>> c)
      : v{curve_ptr{std::move(c)}}
  {
  }
  behavior(std::vector<ossia::behavior> c)
      : v{std::move(c)}
  {
  }

  /**
   * @brief reset Recursively calls reset on the curves of this behavior.
   */
  void reset();

  operator bool() const { return bool(v); }

  template <typename T>
  const T& get() const
  {
    return v.get<T>();
  }

  template <typename T>
  T& get()
  {
    return v.get<T>();
  }

  template <typename T>
  const T* target() const noexcept
  {
    return v.target<T>();
  }

  template <typename T>
  T* target() noexcept
  {
    return v.target<T>();
  }

  template <typename Visitor>
  auto apply(Visitor&& vis) -> decltype(auto);

  template <typename Visitor>
  auto apply(Visitor&& vis) const -> decltype(auto);
};
#include <ossia/editor/curve/behavior_variant_impl.hpp>

template <typename Visitor>
inline auto behavior::apply(Visitor&& vis) -> decltype(auto)
{
  return ossia::apply(std::forward<Visitor>(vis), this->v);
}

template <typename Visitor>
inline auto behavior::apply(Visitor&& vis) const -> decltype(auto)
{
  return ossia::apply(std::forward<Visitor>(vis), this->v);
}

template <typename Functor>
auto apply(Functor&& functor, const behavior& var) -> decltype(auto)
{
  return ossia::apply(std::forward<Functor>(functor), var.v);
}
template <typename Functor>
auto apply(Functor&& functor, behavior& var) -> decltype(auto)
{
  return ossia::apply(std::forward<Functor>(functor), var.v);
}
template <typename Functor>
auto apply(Functor&& functor, behavior&& var) -> decltype(auto)
{
  return ossia::apply(std::forward<Functor>(functor), std::move(var.v));
}
template <typename Functor>
auto apply_nonnull(Functor&& functor, const behavior& var) -> decltype(auto)
{
  return ossia::apply_nonnull(std::forward<Functor>(functor), var.v);
}
template <typename Functor>
auto apply_nonnull(Functor&& functor, behavior& var) -> decltype(auto)
{
  return ossia::apply_nonnull(std::forward<Functor>(functor), var.v);
}
template <typename Functor>
auto apply_nonnull(Functor&& functor, behavior&& var) -> decltype(auto)
{
  return ossia::apply_nonnull(std::forward<Functor>(functor), std::move(var.v));
}

}
