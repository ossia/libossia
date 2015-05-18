/*!
 * \file StateElement.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

namespace OSSIA {

class StateElement {

    public:

      virtual ~StateElement() = default;

      virtual void launch() const = 0;

};

}
