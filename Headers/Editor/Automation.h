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
template <typename T> class Curve;
class TimeValue;

template <typename T>
class Automation : public virtual TimeProcess
{
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param #TimeProcess::ExecutionCallback the function to use to be notified at each step
   \param std::shared_ptr<#State> the state at start
   \param std::shared_ptr<#State> the state at end
   \param std::shared_ptr<#Clock> the clock use to run
   \return std::shared_ptr<#Automation<T>> */
  static std::shared_ptr<Automation<T>> create(TimeProcess::ExecutionCallback,
                                               std::shared_ptr<State> = State::create(),
                                               std::shared_ptr<State> = State::create(),
                                               std::shared_ptr<Clock> = Clock::create());
  
  /*! clone */
  virtual std::shared_ptr<Automation<T>> clone() const = 0;
  
  /*! destructor */
  virtual ~Automation() = default;

# pragma mark -
# pragma mark Accessors
  
  virtual const Value * getStartValue() const = 0; //TODO doublon avec Curve ?
  virtual void setStartValue(const Value*) = 0;
  
  virtual const Value * getEndValue() const = 0;
  virtual void setEndValue(const Value*) = 0;
  
  virtual const std::shared_ptr<Curve<T>> & getCurve() const = 0;
  virtual void setCurve(std::shared_ptr<Curve<T>>) = 0;
  
  virtual const std::shared_ptr<Address> & getInputAddress() const = 0;
  virtual void setInputAddress(std::shared_ptr<Address>) = 0;

  Container<Address>& addresses()
  { return m_addresses; }
  
  const Container<Address>& addresses() const
  { return m_addresses; }

private:
  Container<Address> m_addresses;
};
}
