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

#include "Editor/ExpressionValue.h"

namespace OSSIA
{

class AddressValue : public ExpressionValue
{

public:

# pragma mark -
# pragma mark Enumerations
  
  /*! type of value */
  enum class Type {
    NONE,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    TUPLE, //TODO
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
  
struct None : public AddressValue {
  None() {type = Type::NONE;}
};

struct Bool : public AddressValue {
  Bool(bool v) : value(v) {type = Type::BOOL;}
  bool value;
};

struct Int : public AddressValue {
  Int(int v) : value(v) {type = Type::INT;}
  int value;
};

struct Float : public AddressValue {
  Float(float v) : value(v) {type = Type::FLOAT;}
  float value;
};

struct Char : public AddressValue {
  Char(char v) : value(v) {type = Type::CHAR;}
  char value;
};

struct String : public AddressValue {
  String(std::string v) : value(v) {type = Type::STRING;}
  std::string value;
};

struct Generic : public AddressValue {
  // todo : constructor
  char * start;
  int size;
};

}

