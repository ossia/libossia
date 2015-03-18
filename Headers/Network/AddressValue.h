/*!
 * \file AddressValue.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef ADDRESSVALUE_H_
#define ADDRESSVALUE_H_

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
    TUPLE, //todo
    GENERIC
  };

  virtual ~AddressValue() = default;

};

struct Bool : public AddressValue {
  bool value;
};

struct Int : public AddressValue {
  int value;
};

struct Float : public AddressValue {
  float value;
};

struct Char : public AddressValue {
  char value;
};

struct String : public AddressValue {
  std::string value;
};

struct Generic : public AddressValue {
  char * start;
  int size;
};

}

#endif /* ADDRESSVALUE_H_ */
