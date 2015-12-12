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

namespace OSSIA
{

class Value
{

public:

# pragma mark -
# pragma mark Enumerations

  /*! type of value */
  enum class Type
  {
    IMPULSE,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    TUPLE,
    GENERIC,
    DESTINATION,  //! \note see Destination structure declaration in Node.h
    BEHAVIOR,     //! \note see Behavior structure declaration in Curve.h
  };

# pragma mark -
# pragma mark Life cycle

  /*! clone */
  virtual Value * clone() const = 0;

  /*! destructor */
  virtual ~Value();

# pragma mark -
# pragma mark Operators

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

# pragma mark -
# pragma mark Accessors

  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return m_type;}

protected:

  Type m_type;
};

# pragma mark -
# pragma mark Impulse

/*! \details Impulse value */
struct Impulse final : public Value
{
  /*! constructor */
  Impulse();

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

# pragma mark -
# pragma mark Bool

/*! \details Bool value */
struct Bool final : public Value
{
  bool value;

  /*! constructor */
  Bool(bool = false);

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

static Bool False = Bool(false);
static Bool True = Bool(true);

# pragma mark -
# pragma mark Int

/*! \details Int value */
struct Int final : public Value
{
  int value;

  /*! constructor
   \param int value */
  Int(int = 0);

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

# pragma mark -
# pragma mark Float

/*! \details Float value */
struct Float final : public Value
{
  float value;

  /*! constructor
   \param float value */
  Float(float = 0.);

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

# pragma mark -
# pragma mark Char

/*! \details Char value */
struct Char final : public Value
{
  char value;

  /*! constructor
   \param char value */
  Char(char = 0x00);

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

# pragma mark -
# pragma mark String

/*! \details String value */
struct String final : public Value
{
  std::string value;

  /*! constructor
   \param std::string value */
  String(std::string = "");

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

# pragma mark -
# pragma mark Tuple

/*! \details Tuple value */
struct Tuple final : public Value
{
  std::vector<Value*> value;

  /*! constructor for an empty tuple */
  Tuple();

  /*! constructor for one value
   \param const value */
  Tuple(const Value* v);

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<const Value*>);


  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<const Value*>&);

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

# pragma mark -
# pragma mark Generic

/*! \details Generic value */
template<typename T>
struct Generic final : public Value
{
  T value;

  /*! constructor */
  Generic(T v);

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

}

