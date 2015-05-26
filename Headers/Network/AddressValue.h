/*!
 * \file AddressValue.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <string>

#include "Editor/ExpressionValue.h"

namespace OSSIA {

class AddressValue : public ExpressionValue {

public:

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
  
  //Type type = Type::NONE;

  virtual ~AddressValue() = default;
};

struct Bool : public AddressValue {
  Bool(bool v) : /*type(Type::BOOL),*/ value(v) {}
  bool value;
};

struct Int : public AddressValue {
  Int(int v) : /*type(Type::INT),*/ value(v) {}
  int value;
};

struct Float : public AddressValue {
  Float(float v) : /*type(Type::FLOAT),*/ value(v) {}
  float value;
};

struct Char : public AddressValue {
  Char(char v) : /*type(Type::CHAR),*/ value(v) {}
  char value;
};

struct String : public AddressValue {
  String(std::string v) : /*type(Type::STRING),*/ value(v) {}
  std::string value;
};

struct Generic : public AddressValue {
  // todo : constructor
  char * start;
  int size;
};

}

