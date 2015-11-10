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
#include <vector>
#include <initializer_list>

#include "TimeValue.h"
#include "Value.h"
#include "Network/Node.h"
#include "Misc/Container.h"

namespace OSSIA
{
  
class CurveAbstract
{
    
public:
    
# pragma mark -
# pragma mark Life cycle
    
  /*! destructor 
   \todo remove = default */
  virtual ~CurveAbstract() = default;
};

template <typename Y>
class CurveSegment;

template <typename X, typename Y>
class Curve : public CurveAbstract
{

public:
  
  typedef X abscissa_type;
  typedef Y value_type;
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return std::shared_ptr<#Curve> */
  static std::shared_ptr<Curve> create();
  
  /*! clone */
  virtual std::shared_ptr<Curve> clone() const = 0;
  
  /*! destructor */
  virtual ~Curve();
  
# pragma mark -
# pragma mark Edition
  
  /*! add a segment to reach a target point to the curve
   \param std::shared_ptr<#CurveSegment<Y>> segment to target point
   \param X target point abscissa
   \param Y target point value
   \return bool */
  virtual bool addPoint(std::shared_ptr<CurveSegment<Y>>, X, Y) = 0;
  
  /*! remove a point from the curve
   \param X point abscissa
   \return bool */
  virtual bool removePoint(X) = 0;

# pragma mark -
# pragma mark Execution
  
  /*! get value at an abscissa
   \param X abscissa.
   \return Y value */
  virtual Y valueAt(X) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get initial curve value
   \details if there is an initial destination, it will return the value of the address
   \return Y value */
  virtual Y getInitialValue() const = 0;
  
  /*! set initial curve value
   \details if there is an initial destination, this accessor is useless
   \param Y value */
  virtual void setInitialValue(Y) = 0;
  
  /*! get initial curve destination
   \return const Destination* */
  virtual const Destination* getInitialDestination() const = 0;
  
  /*! set initial curve value using a Destination
   \param const Destination* */
  virtual void setInitialDestination(const Destination*) = 0;
  
  /*! get which index are taken in case the initial destination is a Tuple of Tuple of Tuple ...
   \return std::vector<char> */
  virtual std::vector<char> getInitialDestinationIndex() const = 0;
  
  /*! precise which index to take in case the initial destination is a Tuple of Tuple of Tuple ...
   \param char for first level Tuple index
   \param char for second level Tuple index
   \param ... */
  virtual void setInitialDestinationIndex(std::initializer_list<char>) = 0;
  
  /*! get initial curve value
   \return std::map<X, std::pair<Y, std::shared_ptr<CurveSegment<Y>>>> map of {abscissa, {value, previous segment} */
  virtual std::map<X, std::pair<Y, std::shared_ptr<CurveSegment<Y>>>> getPointsMap() const = 0;

};
  
# pragma mark -
# pragma mark Behavior
  
/*! \details Behavior value */
struct Behavior : public Value
{
  std::shared_ptr<CurveAbstract> value;
  
  /*! constructor */
  Behavior(std::shared_ptr<CurveAbstract> v);
  
  /*! clone */
  Value * clone() const override;
  
  /*! equal operator */
  bool operator== (const Value&) const override;
  
  /*! different operator */
  bool operator!= (const Value&) const override;
  
  /*! greater than operator */
  bool operator> (const Value&) const override;
  
  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;
  
  /*! less than operator */
  bool operator< (const Value&) const override;
  
  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

}
