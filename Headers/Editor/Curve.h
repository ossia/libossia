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

#include "TimeValue.h"
#include "Value.h"
#include "Misc/Container.h"

namespace OSSIA
{
  
class CurveAbstract
{
    
public:
    
# pragma mark -
# pragma mark Life cycle
    
  /*! destructor */
  virtual ~CurveAbstract() = default;
};

template <typename T>
class CurveSegment;

template <typename T>
class Curve : public CurveAbstract
{

public:
  
  typedef T value_type;
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return std::shared_ptr<#Curve> */
  static std::shared_ptr<Curve> create();
  
  /*! clone */
  virtual std::shared_ptr<Curve> clone() const = 0;
  
  /*! destructor */
  virtual ~Curve() = default;

# pragma mark -
# pragma mark Execution
  
  /*! get value at an abscissa
   \param const TimeValue& abscissa between 0. and 1.
   \return T value */
  virtual T valueAt(const TimeValue&) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get initial curve value
   \return T value */
  virtual T getInitialValue() const = 0;
  
  /*! set initial curve value
   \param const T value */
  virtual void setInitialValue(const T) = 0;
  
  /*! get initial curve value
   \return std::map<const TimeValue, std::pair<T, std::shared_ptr<CurveSegment<T>>>> map of {abscissa, {value, previous segment} */
  virtual std::map<const TimeValue, std::pair<T, std::shared_ptr<CurveSegment<T>>>> getPointsMap() const = 0;

# pragma mark -
# pragma mark CurveSegments
  
  /*! add a point to the curve
   \param const TimeValue& point abscissa between 0. and 1.
   \param T point value
   \param std::shared_ptr<#CurveSegment<T>> segment
   \return bool */
  virtual bool addPoint(const TimeValue&, T, std::shared_ptr<CurveSegment<T>>) = 0;
  
  /*! remove a point from the curve
   \param const TimeValue& point abscissa between 0. and 1.
   \return bool */
  virtual bool removePoint(const TimeValue&) = 0;

  /*! get all segments of the curve
   \return #Container<#CurveSegment<T>> */
  Container<CurveSegment<T>>& segments()
  { return m_segments; }
  
  /*! get all segments of the curve
   \return const #Container<#CurveSegment<T>> */
  const Container<CurveSegment<T>>& segments() const
  { return m_segments; }

private:
  Container<CurveSegment<T>> m_segments;
};
  
# pragma mark -
# pragma mark Behavior
  
/*! \details Behavior value */
struct Behavior : public Value
{
  std::shared_ptr<CurveAbstract> value;
  
  /*! constructor */
  Behavior(std::shared_ptr<CurveAbstract> v) : value(v)
  {
    m_type = Type::BEHAVIOR;
  }
    
  /*! clone */
  Value * clone() const override
  {
    return new Behavior(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    return v.getType() == m_type;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    return v.getType() == m_type;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    return v.getType() == m_type;
  }
};

}
