/*!
 * \file AddressValue.h
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

#include "Editor/ExpressionValue.h"

namespace OSSIA
{

class AddressValue : public ExpressionValue
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
    GENERIC
  };
  
# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~AddressValue() = default;

# pragma mark -
# pragma mark Accessors
  
  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return type;}
  
protected:
  
  Type type;
};

# pragma mark -
# pragma mark Impulse
  
/*! \details Impulse value */
struct Impulse : public AddressValue
{
  /*! constructor */
  Impulse() {type = Type::IMPULSE;}
};

# pragma mark -
# pragma mark Bool
  
/*! \details Bool value */
struct Bool : public AddressValue
{
  /*! constructor 
   \param bool value */
  Bool(bool v = false) : value(v) {type = Type::BOOL;}
  
  bool value;
};
  
# pragma mark -
# pragma mark Int

/*! \details Int value */
struct Int : public AddressValue
{
  /*! constructor
   \param int value */
  Int(int v = 0) : value(v) {type = Type::INT;}
  
  int value;
};
  
# pragma mark -
# pragma mark Float

/*! \details Float value */
struct Float : public AddressValue
{
  /*! constructor
   \param float value */
  Float(float v = 0.) : value(v) {type = Type::FLOAT;}
  
  float value;
};
  
# pragma mark -
# pragma mark Char
  
/*! \details Char value */
struct Char : public AddressValue
{
  /*! constructor
   \param char value */
  Char(char v = 0x00) : value(v) {type = Type::CHAR;}
  
  char value;
};
  
# pragma mark -
# pragma mark String

/*! \details String value */
struct String : public AddressValue
{
  /*! constructor
   \param std::string value */
  String(std::string v = "") : value(v) {type = Type::STRING;}
  
  std::string value;
};
  
# pragma mark -
# pragma mark Tuple
  
/*! \details Tuple value */
struct Tuple : public AddressValue
{
  /*! constructor
  \param std::vector<#AddressValue> value*/
  Tuple(std::vector<AddressValue*> v = std::vector<AddressValue*>()) : value(v) {type = Type::TUPLE;}
    
  std::vector<AddressValue*> value;
};
  
# pragma mark -
# pragma mark Generic

/*! \details Generic value */
struct Generic : public AddressValue
{
  /*! constructor 
   \todo */
  char * start;
  int size;
};

}

