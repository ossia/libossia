/*!
 * \file Domain.h
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

#include "Editor/Value.h"

namespace OSSIA
{
  
class Value;

class Domain
{
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! constructor */
  Domain(Value * min = new Impulse(),
                Value * max = new Impulse(),
                std::vector<Value*> values = std::vector<Value*>()) :
  m_min(min),
  m_max(max),
  m_values(values)
  {

  }
  
  /*! destructor */
  virtual ~Domain() = default;

# pragma mark -
# pragma mark Accessors
  
  /*! get the minimal value
   \return #Value* minimal value */
  Value * getMin() const {return m_min;}
  
  /*! set the minimal value
   \param #Value* minimal value */
  void setMin(Value * min) {m_min = min;}
  
  /*! get the maximal value
   \return #Value* maximal value */
  Value * getMax() const {return m_max;}
  
  /*! set the maximal value
   \param #Value* maximal value */
  void setMax(Value * max) {m_max = max;}
  
  /*! get authorized values
   \return std::vector<#Value*> authorized values */
  std::vector<Value*> getValues() const {return m_values;}
  
  /*! set authorized values
   \param std::vector<#Value*> authorized values */
  void setValues(std::vector<Value*> values) {m_values = values;}
  
private:
  
  Value * m_min;
  Value * m_max;
  std::vector<Value*> m_values;
};
}

