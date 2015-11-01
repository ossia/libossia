/*!
 * \file CurveSegmentPower.h
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

#include "Editor/CurveSegment.h"

namespace OSSIA
{

  template <typename T>
  class CurveSegmentPower : public CurveSegment<T>
  {

  public:

# pragma mark -
# pragma mark Life cycle

    /*! factory
     \param std::shared_ptr<Curve<T>> parent
     \return std::shared_ptr<CurveSegmentPower<T>> */
    static std::shared_ptr<CurveSegmentPower<T>> create(std::shared_ptr<Curve<T>> = nullptr);

    /*! clone */
    virtual std::shared_ptr<CurveSegmentPower<T>> clone() const = 0;

    /*! destructor */
    virtual ~CurveSegmentPower() = default;

# pragma mark -
# pragma mark Accessors

    /*! get power value
     \return double */
    virtual double getPower() const = 0;

    /*! set power value
     \param double
     \return CurveSegmentPower */
    virtual CurveSegmentPower<T> & setPower(double) = 0;

    typename CurveSegment<T>::Type getType() const override final
    {return CurveSegment<T>::Type::POWER;}

};
}
