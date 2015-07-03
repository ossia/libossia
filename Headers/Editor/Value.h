/*!
 * \file Value.h
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
    DESTINATION, //! \note see Destination structure declaration in Node.h
    TUPLE,
    GENERIC
  };
  
# pragma mark -
# pragma mark Life cycle
  
  /*! clone */
  virtual Value * clone() const = 0;
  
  /*! destructor */
  virtual ~Value() = default;

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
  Impulse()
  {
    m_type = Type::IMPULSE;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Impulse();
  }
};

# pragma mark -
# pragma mark Bool
  
/*! \details Bool value */
struct Bool : public Value
{
  /*! constructor 
   \param bool value */
  Bool(bool v = false) : value(v)
  {
    m_type = Type::BOOL;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Bool(value);
  }
  
  bool value;
};

static Bool False = Bool(false);
static Bool True = Bool(true);

# pragma mark -
# pragma mark Int

/*! \details Int value */
struct Int : public Value
{
  /*! constructor
   \param int value */
  Int(int v = 0) : value(v)
  {
    m_type = Type::INT;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Int(value);
  }
  
  int value;
};
  
# pragma mark -
# pragma mark Float

/*! \details Float value */
struct Float : public Value
{
  /*! constructor
   \param float value */
  Float(float v = 0.) : value(v)
  {
    m_type = Type::FLOAT;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Float(value);
  }
  
  float value;
};
  
# pragma mark -
# pragma mark Char
  
/*! \details Char value */
struct Char : public Value
{
  /*! constructor
   \param char value */
  Char(char v = 0x00) : value(v)
  {
    m_type = Type::CHAR;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Char(value);
  }
  
  char value;
};
  
# pragma mark -
# pragma mark String

/*! \details String value */
struct String : public Value
{
  /*! constructor
   \param std::string value */
  String(std::string v = "") : value(v)
  {
    m_type = Type::STRING;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new String(value);
  }
  
  std::string value;
};

# pragma mark -
# pragma mark Tuple
  
/*! \details Tuple value */
struct Tuple : public Value
{
  /*! constructor
  \param std::vector<const #Value> value*/
  Tuple(std::vector<const Value*> v = std::vector<const Value*>())
  {
    m_type = Type::TUPLE;
    
    for (const auto & e : v)
      value.push_back(e->clone());
  }
  
  /*! clone */
  Value * clone() const override
  {
    std::vector<const Value*> newValue;
    for (const auto & e : value)
      newValue.push_back(e->clone());
      
    return new Tuple(newValue);
  }
  
  std::vector<Value*> value;
};
  
# pragma mark -
# pragma mark Generic
  
  /*! \details Generic value */
  template<typename T>
  struct Generic : public Value
  {
    /*! constructor */
    Generic(T v) : value(v)
    {
      m_type = Type::GENERIC;
    }
    
    /*! clone */
    Value * clone() const override
    {
      return new Generic(value);
    }
    
    T value;
  };

}

