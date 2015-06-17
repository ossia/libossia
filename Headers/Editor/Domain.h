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
  
  /*! factory
   \param #Value* minimal value of the domain
   \param #Value* maximal value of the domain
   \param std::vector<#Value*> set of values allowed into the domain
   \return std::shared_ptr<#Domain> */
  static std::shared_ptr<Domain> create(Value * = new Impulse(),
                                        Value * = new Impulse(),
                                        std::vector<Value*> = std::vector<Value*>());
  
  /*! destructor */
  virtual ~Domain() = default;

# pragma mark -
# pragma mark Accessors
  
  /*! get the minimal value
   \return #Value* minimal value */
  virtual Value * getMin() const = 0;
  
  /*! set the minimal value
   \param #Value* minimal value */
  virtual void setMin(Value * min) = 0;
  
  /*! get the maximal value
   \return #Value* maximal value */
  virtual Value * getMax() const = 0;
  
  /*! set the maximal value
   \param #Value* maximal value */
  virtual void setMax(Value * max) = 0;
  
  /*! get authorized values
   \return std::vector<#Value*> authorized values */
  virtual std::vector<Value*> getValues() const = 0;
  
  /*! set authorized values
   \param std::vector<#Value*> authorized values */
  virtual void setValues(std::vector<Value*> values) = 0;
};
}

