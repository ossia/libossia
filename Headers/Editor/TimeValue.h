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

#include "math.h"

#include <ossia_export.h>
namespace OSSIA
{

class OSSIA_EXPORT TimeValue
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! constructor
   \param int value */
  TimeValue(double = 0.);

  /*! destructor */
  ~TimeValue();

#if 0
# pragma mark -
# pragma mark Operator
#endif

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

  /*! double casting operator */
  operator double() const;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! is the time value infinite ?
   \return bool infinite */
  bool isInfinite() const;

protected:

  double  m_value;
};

const TimeValue Infinite{INFINITY};
const TimeValue Zero{0.};
const TimeValue One{1.};

}
