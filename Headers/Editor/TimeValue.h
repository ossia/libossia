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

namespace OSSIA
{

class TimeValue
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! constructor */
  TimeValue(const bool = false);
  
  /*! constructor
   \param int value */
  TimeValue(const double, const bool = false);
  
  /*! destructor */
  ~TimeValue();
  
# pragma mark -
# pragma mark Operator
  
  /*! assignation operator */
  TimeValue & operator= (const double);
  
  /*! addition operator */
  TimeValue & operator+= (const double);
  
  /*! soustraction operator */
  TimeValue & operator-= (const double);
  
  /*! equal operator */
  bool operator== (const TimeValue& t);
  
  /*! different operator */
  bool operator!= (const TimeValue& t);
  
  /*! greater than operator */
  bool operator> (const TimeValue& t);
  
  /*! greater than and equal operator */
  bool operator>= (const TimeValue& t);
  
  /*! less than operator */
  bool operator< (const TimeValue& t);
  
  /*! less than and equal operator */
  bool operator<= (const TimeValue& t);
  
  /*! double casting operator */
  operator double() const;
  
# pragma mark -
# pragma mark Accessors
  
  /*! is the time value infinite ?
   \return bool infinite */
  bool isInfinite() const;
  
protected:
  
  double  m_value;
  bool    m_infinite;
};
  
static TimeValue Infinite = TimeValue(true);
static TimeValue Zero = TimeValue(0.);

}
