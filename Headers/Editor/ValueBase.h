/*!
 * \file Value.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <string>
#include <vector>
#include <initializer_list>
#include "Network/Node.h"
#include <ossia_export.h>

namespace OSSIA
{

class OSSIA_EXPORT Value
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  Value(Type t):
      m_type{t} { }

  /*! clone */
  virtual Value * clone() const = 0;

  /*! destructor */
  virtual ~Value();

#if 0
# pragma mark -
# pragma mark Operators
#endif

  /*! equal operator */
  virtual bool operator== (const Value& v) const = 0;

  /*! different operator */
  virtual bool operator!= (const Value& v) const = 0;

  /*! greater than operator */
  virtual bool operator> (const Value& v) const = 0;

  /*! greater than and equal operator */
  virtual bool operator>= (const Value& v) const = 0;

  /*! less than operator */
  virtual bool operator< (const Value& v) const = 0;

  /*! less than and equal operator */
  virtual bool operator<= (const Value& v) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return m_type;}

protected:
  Type m_type;
};

#if 0
# pragma mark -
# pragma mark Impulse
#endif

/*! \details Impulse value */
struct OSSIA_EXPORT Impulse final : public Value
{
  /*! constructor */
    Impulse():
        Value{Type::IMPULSE}
    {
    }

  virtual ~Impulse();
  Impulse(const Impulse&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override { return true; }

  /*! different operator */
  bool operator!= (const Value&) const override { return false; }

  /*! greater than operator */
  bool operator> (const Value&) const override { return false; }

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override { return true; }

  /*! less than operator */
  bool operator< (const Value&) const override { return false; }

  /*! less than and equal operator */
  bool operator<= (const Value&) const override { return true; }
};

#if 0
# pragma mark -
# pragma mark Bool
#endif

/*! \details Bool value */
struct OSSIA_EXPORT Bool final : public Value
{
  bool value;

  /*! constructor */
  Bool(bool v = false) :
      Value{Type::BOOL},
      value(v)
  {
  }

  virtual ~Bool();
  Bool(const Bool&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

const Bool False{false};
const Bool True{true};

#if 0
# pragma mark -
# pragma mark Int
#endif

/*! \details Int value */
struct OSSIA_EXPORT Int final : public Value
{
  int value;

  /*! constructor
   \param int value */
  Int(int v = 0) :
      Value{Type::INT},
      value(v)
  {
  }
  virtual ~Int();
  Int(const Int&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Float
#endif

/*! \details Float value */
struct OSSIA_EXPORT Float final : public Value
{
  float value;

  /*! constructor
   \param float value */
  Float(float v = 0.) :
      Value{Type::FLOAT},
      value(v)
  {
  }

  virtual ~Float();
  Float(const Float&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Char
#endif

/*! \details Char value */
struct OSSIA_EXPORT Char final : public Value
{
  char value;

  /*! constructor
   \param char value */
  Char(char v = 0x00) :
      Value{Type::CHAR},
      value(v)
  {
  }
  virtual ~Char();
  Char(const Char&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark String
#endif

/*! \details String value */
struct OSSIA_EXPORT String final : public Value
{
  std::string value;

  /*! constructor
   \param std::string value */
  String(std::string v = "") :
      Value{Type::STRING},
      value(std::move(v))
  {
  }
  virtual ~String();
  String(const String&) = delete;

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

#if 0
# pragma mark -
# pragma mark Tuple
#endif

/*! \details Tuple value */
struct OSSIA_EXPORT Tuple final : public Value
{
  std::vector<Value*> value;

  /*! constructor for an empty tuple */
  Tuple():
      Value{Type::TUPLE}
  {
  }
  Tuple(const Tuple&) = delete;
  virtual ~Tuple();

  /*! Mechanism for building a Tuple with a list of
   * values, to remove the need for spurious memory allocations.
   * Usage : Tuple t{Tuple::ValueInit, Int{2}, Float{3.145}, String{"foo"}};
   */
  struct ValueInit {} ;
  template<typename... Args>
  explicit Tuple(ValueInit, Args&&... args):
      Value{Type::TUPLE}
  {
      value.reserve(sizeof...(args));
      append(std::forward<Args>(args)...);
  }

  void append() { }
  template<typename Arg, typename... Args>
  void append(Arg&& arg, Args&&... args)
  {
      value.push_back(arg.clone());
      append(std::forward<Args>(args)...);
  }

  /*! constructor for one value
   \param const value */
  explicit Tuple(const Value* v):
      Value{Type::TUPLE}
  {
      value.push_back(v->clone());
  }

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<const Value*> v):
      Value{Type::TUPLE}
  {
      for (const auto & e : v)
          value.push_back(e->clone());
  }

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<const Value*>& v):
      Value{Type::TUPLE}
  {
      for (const auto & e : v)
          value.push_back(e->clone());
  }
  Tuple(std::vector<Value*>&& v):
      Value{Type::TUPLE},
      value(std::move(v))
  {
  }

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};


/*! \details Destination to an Address value and optionnally to several index of this value */
struct OSSIA_EXPORT Destination final : public Value
{
  std::shared_ptr<Node> value;
  std::vector<char> index;

  Destination():
      Value{Type::DESTINATION}
  {

  }

  /*! constructor for a node and optionnal index values
   \param std::shared_ptr<Node>
   \param char
   \param char
   \param ... */

  Destination(std::shared_ptr<Node> v, std::initializer_list<char> = {});

  /*! constructor for a node and an index vector
   \param std::shared_ptr<Node>
   \param std::vector<const #Value> value */
  Destination(std::shared_ptr<Node> v, std::vector<char>);
  Destination(const Destination& other) = delete;
  virtual ~Destination();

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

template<typename T, int N>
class Vec;

template<typename T>
struct ValueTrait;

template<>
struct ValueTrait<OSSIA::Impulse>
{
        using ossia_type = OSSIA::Impulse;
        using value_type = void;
        static const constexpr auto ossia_enum = Type::IMPULSE;
};

template<>
struct ValueTrait<OSSIA::Int>
{
        using ossia_type = OSSIA::Int;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::INT;
};

template<>
struct ValueTrait<OSSIA::Float>
{
        using ossia_type = OSSIA::Float;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::FLOAT;
};

template<>
struct ValueTrait<OSSIA::Bool>
{
        using ossia_type = OSSIA::Bool;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::BOOL;
};

template<>
struct ValueTrait<OSSIA::Char>
{
        using ossia_type = OSSIA::Char;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::CHAR;
};

template<>
struct ValueTrait<OSSIA::String>
{
        using ossia_type = OSSIA::String;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::STRING;
};

template<>
struct ValueTrait<OSSIA::Tuple>
{
        using ossia_type = OSSIA::Tuple;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::TUPLE;
};

template<>
struct ValueTrait<OSSIA::Destination>
{
        using ossia_type = OSSIA::Destination;
        using value_type = decltype(ossia_type{}.value);
        static const constexpr auto ossia_enum = Type::DESTINATION;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 2>>
{
        using ossia_type = OSSIA::Vec<float, 2>;
        using value_type = std::array<float, 2>;
        static const constexpr auto ossia_enum = Type::VEC2F;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 3>>
{
        using ossia_type = OSSIA::Vec<float, 3>;
        using value_type = std::array<float, 3>;
        static const constexpr auto ossia_enum = Type::VEC3F;
};

template<>
struct ValueTrait<OSSIA::Vec<float, 4>>
{
        using ossia_type = OSSIA::Vec<float, 4>;
        using value_type = std::array<float, 4>;
        static const constexpr auto ossia_enum = Type::VEC4F;
};

template<typename T, int N>
class Vec : public Value
{
    public:
        std::array<T, N> value;

        Vec():
            Value{ValueTrait<Vec>::ossia_enum}
        {
        }

        Vec(std::array<T, N> v) :
            Value{ValueTrait<Vec>::ossia_enum},
            value(std::move(v))
        {
        }

        virtual ~Vec() = default;
        Vec(const Vec&) = delete;

        /*! clone */
        Value * clone() const override
        {
            return new Vec{value};
        }

        /*! equal operator */
        bool operator== (const Value&) const override;

        /*! different operator */
        bool operator!= (const Value&) const override;

        /*! greater than operator */
        bool operator> (const Value&) const override;

        /*! greater than and equal operator */
        bool operator>= (const Value&) const override;

        /*! less than operator */
        bool operator< (const Value&) const override;

        /*! less than and equal operator */
        bool operator<= (const Value&) const override;
};

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;


}

