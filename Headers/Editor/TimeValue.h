/*!
 * \file TimeValue.h
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

namespace OSSIA
{

class TimeValue
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! constructor
    \param bool infinite */
  TimeValue(bool = false);
  
  /*! constructor
   \param int value */
  TimeValue(double);
  
  /*! destructor */
  ~TimeValue();
  
# pragma mark -
# pragma mark Operator
  
  /*! assignation operator */
  TimeValue & operator= (double);
  TimeValue & operator= (const TimeValue&);
  
  /*! self addition operator */
  TimeValue & operator+= (double);
  TimeValue & operator+= (const TimeValue&);
  
  /*! self substraction operator */
  TimeValue & operator-= (double);
  TimeValue & operator-= (const TimeValue&);
  
  /*! addition operator */
  TimeValue operator+ (double) const;
  TimeValue operator+ (const TimeValue&) const;
  
  /*! substraction operator */
  TimeValue operator- (double) const;
  TimeValue operator- (const TimeValue&) const;
  
  /*! equal operator */
  bool operator== (double) const;
  bool operator== (const TimeValue&) const;
  
  /*! different operator */
  bool operator!= (double) const;
  bool operator!= (const TimeValue&) const;
  
  /*! greater than operator */
  bool operator> (double) const;
  bool operator> (const TimeValue&) const;
  
  /*! greater than and equal operator */
  bool operator>= (double) const;
  bool operator>= (const TimeValue&) const;
  
  /*! less than operator */
  bool operator< (double) const;
  bool operator< (const TimeValue&) const;
  
  /*! less than and equal operator */
  bool operator<= (double) const;
  bool operator<= (const TimeValue&) const;
  
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
static TimeValue One = TimeValue(1.);

}
