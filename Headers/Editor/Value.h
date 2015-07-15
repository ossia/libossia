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
  virtual ~Value() = default;
  
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
struct Impulse : public Value
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
struct Bool : public Value
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
struct Int : public Value
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
struct Float : public Value
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
struct Char : public Value
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
struct String : public Value
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
struct Tuple : public Value
{
  std::vector<Value*> value;
  
  /*! constructor
  \param std::vector<const #Value> value*/
  Tuple(std::vector<const Value*> = std::vector<const Value*>());
  
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
struct Generic : public Value
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

