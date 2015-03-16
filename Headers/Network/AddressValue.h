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

namespace OSSIA {

enum class AddressValueType {
	NO_VALUE
};

class AddressValue {

public:
  AddressValue() = default;
  AddressValue(AddressValueType) {}
  virtual ~AddressValue() = default;



};

}



#endif /* ADDRESSVALUE_H_ */
