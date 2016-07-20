#pragma once
#include <eggs/variant.hpp>
#include <Editor/Value/Value.h>

namespace OSSIA
{

class SafeValue
{
public:
  using value_type = eggs::variant<Impulse, Bool, Int, Float, Char, String, Tuple, Vec2f, Vec3f, Vec4f, Destination, Behavior>;
  SafeValue(const Impulse& val): v{val}  { }
  SafeValue(const Bool& val): v{val}  { }
  SafeValue(const Int& val): v{val}  { }
  SafeValue(const Float& val): v{val}  { }
  SafeValue(const Char& val): v{val}  { }
  SafeValue(const String& val): v{val}  { }
  SafeValue(const Tuple& val): v{val}  { }
  SafeValue(const Vec2f& val): v{val}  { }
  SafeValue(const Vec3f& val): v{val}  { }
  SafeValue(const Vec4f& val): v{val}  { }
  SafeValue(const Destination& val): v{val}  { }
  SafeValue(const Behavior& val): v{val}  { }

  SafeValue() = default;
  SafeValue(const SafeValue& other) = default;
  SafeValue(SafeValue&& other) = default;
  SafeValue& operator=(const SafeValue& other) = default;
  SafeValue& operator=(SafeValue&& other) = default;


  value_type v ;

  OSSIA::Type getType() const
  { return static_cast<const OSSIA::Value*>(v.target())->getType(); }

  const OSSIA::Value& operator*() const
  { return *static_cast<const OSSIA::Value*>(v.target()); }
  OSSIA::Value& operator*()
  { return *static_cast<OSSIA::Value*>(v.target()); }

  template<typename T>
  const T& get() const
  { return eggs::variants::get<T>(v); }
  template<typename T>
  T& get()
  { return eggs::variants::get<T>(v); }

  bool valid() const
  { return v.target(); }

  void reset()
  { v = value_type{}; }


  /*! equal operator */
  bool operator== (const SafeValue&) const;

  /*! different operator */
  bool operator!= (const SafeValue&) const;

  /*! greater than operator */
  bool operator> (const SafeValue&) const;

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const;

  /*! less than operator */
  bool operator< (const SafeValue&) const;

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const;
};
}
