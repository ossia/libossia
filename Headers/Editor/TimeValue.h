/*!
 * \file TimeValue.h
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

namespace OSSIA {

class TimeValue {

    public:
      // Constructors, destructor, assignment
      TimeValue();
      TimeValue(const int);
      ~TimeValue();
      TimeValue & operator= (const int);

      // Cast
      operator int();

};

}
