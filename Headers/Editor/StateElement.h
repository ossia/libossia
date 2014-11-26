/*!
 * \file StateElement.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef STATEELEMENT_H_
#define STATEELEMENT_H_

namespace OSSIA {

class StateElement {

public:

  virtual ~StateElement() = default;

  virtual void launch() const = 0;

  enum class StateElementType {
    MESSAGE,
    STATE
  };

  virtual StateElementType getType() const = 0;

};

}

#endif /* STATEELEMENT_H_ */
