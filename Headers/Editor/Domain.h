/*!
 * \file Domain.h
 *
 * \defgroup Editor
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
#include <memory>
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
  static std::shared_ptr<Domain> create(const Value * = new Impulse(),
                                        const Value * = new Impulse(),
                                        std::vector<const Value*> = std::vector<const Value*>());

  /*! clone */
  virtual std::shared_ptr<Domain> clone() const = 0;

  /*! destructor */
  virtual ~Domain() = default;

# pragma mark -
# pragma mark Accessors

  /*! get the minimal value
   \return const #Value* minimal value */
  virtual const Value * getMin() const = 0;

  /*! set the minimal value
   \param const #Value* minimal value 
   \return #Domain the domain */
  virtual Domain & setMin(const Value*) = 0;

  /*! get the maximal value
   \return const #Value* maximal value */
  virtual const Value * getMax() const = 0;

  /*! set the maximal value
   \param const #Value* maximal value 
   \return #Domain the domain */
  virtual Domain & setMax(const Value*) = 0;

  /*! get authorized values
   \return std::vector<const #Value*> authorized values */
  virtual std::vector<const Value*> getValues() const = 0;

  /*! set authorized values
   \param std::vector<const #Value*> authorized values 
   \return #Domain the domain */
  virtual Domain & setValues(std::vector<const Value*> values) = 0;
};
}

