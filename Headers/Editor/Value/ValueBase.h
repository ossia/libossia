/*!
 * \file Value.h
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
#include <Network/AddressProperties.h>
#include <ossia_export.h>

namespace OSSIA
{
class SafeValue;
class OSSIA_EXPORT Value
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  Value(Type t):
      m_type{t} { }

  /*! destructor */
  virtual ~Value();

#if 0
# pragma mark -
# pragma mark Operators
#endif

  /*! equal operator */
  virtual bool operator== (const SafeValue& v) const = 0;

  /*! different operator */
  virtual bool operator!= (const SafeValue& v) const = 0;

  /*! greater than operator */
  virtual bool operator> (const SafeValue& v) const = 0;

  /*! greater than and equal operator */
  virtual bool operator>= (const SafeValue& v) const = 0;

  /*! less than operator */
  virtual bool operator< (const SafeValue& v) const = 0;

  /*! less than and equal operator */
  virtual bool operator<= (const SafeValue& v) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return m_type;}

protected:
  Type m_type;
};

}

