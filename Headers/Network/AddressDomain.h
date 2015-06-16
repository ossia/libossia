/*!
 * \file AddressDomain.h
 *
 * \brief
 *
 * \details
 *
 * \author Jean-Michaël Celerier
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <string>
#include <vector>

#include "AddressValue.h"

namespace OSSIA
{
  
class AddressValue;

class AddressDomain
{
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! constructor */
  AddressDomain(AddressValue * min = new Impulse(),
                AddressValue * max = new Impulse(),
                std::vector<AddressValue*> values = std::vector<AddressValue*>()) :
  m_min(min),
  m_max(max),
  m_values(values)
  {

  }
  
  /*! destructor */
  virtual ~AddressDomain() = default;

# pragma mark -
# pragma mark Accessors
  
  /*! get the minimal value
   \return #AddressValue* minimal value */
  AddressValue * getMin() const {return m_min;}
  
  /*! set the minimal value
   \param #AddressValue* minimal value */
  void setMin(AddressValue * min) {m_min = min;}
  
  /*! get the maximal value
   \return #AddressValue* maximal value */
  AddressValue * getMax() const {return m_max;}
  
  /*! set the maximal value
   \param #AddressValue* maximal value */
  void setMax(AddressValue * max) {m_max = max;}
  
  /*! get authorized values
   \return std::vector<#AddressValue*> authorized values */
  std::vector<AddressValue*> getValues() const {return m_values;}
  
  /*! set authorized values
   \param std::vector<#AddressValue*> authorized values */
  void setValues(std::vector<AddressValue*> values) {m_values = values;}
  
private:
  
  AddressValue * m_min;
  AddressValue * m_max;
  std::vector<AddressValue*> m_values;
};
}

