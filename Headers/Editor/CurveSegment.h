/*!
 * \file CurveSegment.h
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
#include <memory>
#include <ossia_export.h>

namespace OSSIA
{

class CurveAbstract;

template <typename Y>
class OSSIA_EXPORT CurveSegment
{

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! destructor */
  virtual ~CurveSegment() = default;

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! get value at an abscissa
   \param double ratio between 0. and 1.
   \param Y segment start value
   \param Y segment end value
   \return Y value */
  virtual Y valueAt(double, Y, Y) const = 0;

};
}
