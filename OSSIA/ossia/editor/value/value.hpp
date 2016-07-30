#pragma once
#include <ossia/editor/value/value_base.hpp>
#include <eggs/variant.hpp>
#include <ossia_export.h>

namespace ossia
{
class OSSIA_EXPORT value
{
public:
  using value_type = eggs::variant<Impulse, Bool, Int, Float, Char, String, Tuple, Vec2f, Vec3f, Vec4f, Destination, Behavior>;
  template<typename T>
  value(T*) = delete;
  value(Impulse val): v{val}  { }
  value(Bool val): v{val}  { }
  value(Int val): v{val}  { }
  value(Float val): v{val}  { }
  value(Char val): v{val}  { }
  value(const String& val): v{val}  { }
  value(const Tuple& val): v{val}  { }
  value(const Vec2f& val): v{val}  { }
  value(const Vec3f& val): v{val}  { }
  value(const Vec4f& val): v{val}  { }
  value(const Destination& val): v{val}  { }
  value(const Behavior& val): v{val}  { }

  value(String&& val): v{std::move(val)}  { }
  value(Tuple&& val): v{std::move(val)}  { }
  value(Destination&& val): v{std::move(val)}  { }
  value(Behavior&& val): v{std::move(val)}  { }

  value() = default;
  value(const value& other) = default;
  value(value&& other) = default;
  value& operator=(const value& other) = default;
  value& operator=(value&& other) = default;

  value_type v ;

  template<typename T>
  const T& get() const { return eggs::variants::get<T>(v); }

  template<typename T>
  T& get() { return eggs::variants::get<T>(v); }

  template<typename T>
  const T* try_get() const { return v.target<T>(); }

  template<typename T>
  T* try_get() { return v.target<T>(); }

  ossia::Type getType() const
  {
    auto t = v.which();
    if(t == v.npos) throw;

    return static_cast<ossia::Type>(t);
  }

  bool valid() const
  { return v.target(); }

  void reset()
  { v = value_type{}; }


  /*! equal operator */
  bool operator== (const value& rhs) const;

  /*! different operator */
  bool operator!= (const value& rhs) const;

  /*! greater than operator */
  bool operator> (const value& rhs) const;

  /*! greater than and equal operator */
  bool operator>= (const value& rhs) const;

  /*! less than operator */
  bool operator< (const value& rhs) const;

  /*! less than and equal operator */
  bool operator<= (const value& rhs) const;
};



/*!
 * \brief getValueAsString Returns a string corresponding to the value
 * \param val a valid value
 * \return a string in the format : "type: value".
 *
 * ex. "int: 3"
 *     "string: tutu"
 *     "tuple: [ int: 2, float: 3 ]"
 * etc...
 *
 */
OSSIA_EXPORT std::string getValueAsString(const ossia::value& val);



inline ossia::value initValue(ossia::Type type)
{
  switch(type)
  {
  case Type::IMPULSE:
    return Impulse{};
  case Type::BOOL:
    return Bool{};
  case Type::INT:
    return Int{};
  case Type::FLOAT:
    return Float{};
  case Type::CHAR:
    return Char{};
  case Type::STRING:
    return String{};
  case Type::TUPLE:
    return Tuple{};
  case Type::VEC2F:
    return Vec2f{};
  case Type::VEC3F:
    return Vec3f{};
  case Type::VEC4F:
    return Vec4f{};
  case Type::DESTINATION:
    return Destination{};
  case Type::BEHAVIOR:
    return Behavior{{}};
  }

  throw std::runtime_error("Invalid type");
}
}
