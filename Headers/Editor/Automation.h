/*!
 * \file Automation.h
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
#include <list>

#include "Editor/Clock.h"
#include "Editor/State.h"
#include "Editor/TimeProcess.h"
#include "Misc/Container.h"

namespace OSSIA
{

class Address;
class Value;
class CurveAbstract;
class TimeValue;

class Automation : public virtual TimeProcess
{
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param #TimeProcess::ExecutionCallback the function to use to be notified at each step
   \param std::shared_ptr<#Address> to drive
   \return std::shared_ptr<#Automation> */
  static std::shared_ptr<Automation> create(TimeProcess::ExecutionCallback,
                                            std::shared_ptr<Address>);
  
  /*! destructor */
  virtual ~Automation() = default;
  
  /*! clone */
  virtual std::shared_ptr<Automation> clone() const = 0;
  
# pragma mark -
# pragma mark Execution
  
  /*! \todo record */
  
# pragma mark -
# pragma mark Accessors
  
  Container<CurveAbstract>& curves()
  { return m_curves; }
  
  const Container<CurveAbstract>& curves() const
  { return m_curves; }

private:
  Container<CurveAbstract> m_curves;
};
}
