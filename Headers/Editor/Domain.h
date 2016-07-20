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
#include <ossia_export.h>

namespace OSSIA
{

class SafeValue;
class OSSIA_EXPORT Domain
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param #Value* minimal value of the domain
   \param #Value* maximal value of the domain
   \param std::vector<#Value*> set of values allowed into the domain
   \return std::shared_ptr<#Domain> */
  static std::shared_ptr<Domain> create(const SafeValue&,
                                        const SafeValue&,
                                        std::vector<SafeValue>);

  static std::shared_ptr<Domain> create(const SafeValue&,
                                        const SafeValue&);

  /*!
   * \brief create Create a domain initialized with a min Impulse, max Impulse
   * and no range of allowed values.
   */
  static std::shared_ptr<Domain> create();

  /*! clone */
  virtual std::shared_ptr<Domain> clone() const = 0;

  /*! destructor */
  virtual ~Domain();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the minimal value
   \return const #Value* minimal value */
  virtual const SafeValue& getMin() const = 0;

  /*! set the minimal value
   \param const #Value* minimal value
   \return #Domain the domain */
  virtual Domain & setMin(const SafeValue&) = 0;

  /*! get the maximal value
   \return const #Value* maximal value */
  virtual const SafeValue& getMax() const = 0;

  /*! set the maximal value
   \param const #Value* maximal value
   \return #Domain the domain */
  virtual Domain & setMax(const SafeValue&) = 0;

  /*! get authorized values
   \return std::vector<const #Value*> authorized values */
  virtual const std::vector<SafeValue>& getValues() const = 0;

  /*! set authorized values
   \param std::vector<const #Value*> authorized values
   \return #Domain the domain */
  virtual Domain & setValues(const std::vector<SafeValue>& values) = 0;
};
}

