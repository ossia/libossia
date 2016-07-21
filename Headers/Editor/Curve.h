/*!
 * \file Curve.h
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

#include <map>
#include <utility>
#include <memory>

#include "Network/Node.h"
#include "Misc/Container.h"
#include <Editor/CurveAbstract.h>
#include <Editor/CurveSegment.h>
#include <ossia_export.h>
#include <functional>
namespace OSSIA
{
class Destination;

template <typename X, typename Y>
class Curve : public CurveAbstract
{

public:

  typedef X abscissa_type;
  typedef Y ordinate_type;

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<#Curve> */
  static std::shared_ptr<Curve> create();

  /*! clone */
  virtual std::shared_ptr<Curve> clone() const = 0;

  /*! destructor */
  virtual ~Curve();

#if 0
# pragma mark -
# pragma mark Edition
#endif

  /*! add a segment to reach a target point to the curve
   \param std::shared_ptr<#CurveSegment<Y>> segment to target point
   \param X target point abscissa
   \param Y target point ordinate
   \return bool */
  virtual bool addPoint(CurveSegment<Y>, X, Y) = 0;

  /*! remove a point from the curve
   \param X point abscissa
   \return bool */
  virtual bool removePoint(X) = 0;

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! get value at an abscissa
   \param X abscissa.
   \return Y ordinate */
  virtual Y valueAt(X) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif


  /*! get initial point abscissa
   \details if there is an initial abcissa destination, it will return the value of the address
   \return X value */
  virtual X getInitialPointAbscissa() const = 0;

  /*! get initial point ordinate
   \details if there is an initial ordinate destination, it will return the value of the address
   \return Y value */
  virtual Y getInitialPointOrdinate() const = 0;

  /*! set initial point abscissa
   \details if there is an initial abscissa destination, this accessor is useless
   \param X abscissa */
  virtual void setInitialPointAbscissa(X) = 0;

  /*! set initial point ordinate
   \details if there is an initial ordinate destination, this accessor is useless
   \param Y ordinate */
  virtual void setInitialPointOrdinate(Y) = 0;

  /*! get initial point abscissa destination
   \return const Destination* */
  virtual const Destination& getInitialPointAbscissaDestination() const = 0;

  /*! get initial point ordinate destination
   \return const Destination* */
  virtual const Destination& getInitialPointOrdinateDestination() const = 0;

  /*! set initial curve abscissa using a Destination
   \param const Destination* */
  virtual void setInitialPointAbscissaDestination(const Destination&) = 0;

  /*! set initial curve ordinate using a Destination
   \param const Destination* */
  virtual void setInitialPointOrdinateDestination(const Destination&) = 0;

  /*! get initial curve value
   \return std::map<X, pair<Y, CurveSegment<Y>>> map of {abscissa, {value, previous segment}

 */
  virtual std::map<X, std::pair<Y, CurveSegment<Y>>> getPointsMap() const = 0;

};

}

#if !defined(APIJamoma_EXPORTS)
extern template class OSSIA_EXPORT OSSIA::Curve<double, bool>;
extern template class OSSIA_EXPORT OSSIA::Curve<double, int>;
extern template class OSSIA_EXPORT OSSIA::Curve<double, float>;

extern template class OSSIA_EXPORT OSSIA::Curve<bool, bool>;
extern template class OSSIA_EXPORT OSSIA::Curve<bool, int>;
extern template class OSSIA_EXPORT OSSIA::Curve<bool, float>;

extern template class OSSIA_EXPORT OSSIA::Curve<int, bool>;
extern template class OSSIA_EXPORT OSSIA::Curve<int, int>;
extern template class OSSIA_EXPORT OSSIA::Curve<int, float>;

extern template class OSSIA_EXPORT OSSIA::Curve<float, bool>;
extern template class OSSIA_EXPORT OSSIA::Curve<float, int>;
extern template class OSSIA_EXPORT OSSIA::Curve<float, float>;
#endif
