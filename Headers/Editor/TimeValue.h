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
  TimeValue();
  
  /*! constructor
   \param int value */
  TimeValue(const double);
  
  /*! destructor */
  ~TimeValue();
  
# pragma mark -
# pragma mark Operator
  
  /*! assignation operator */
  TimeValue & operator= (const double);
  
  /*! addition operator */
  TimeValue & operator+ (const double);
  
  /*! soustraction operator */
  TimeValue & operator- (const double);

  /*! int casting operator */
  operator double();

};
}
