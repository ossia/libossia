/*!
 * \file Automation_Impl.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef AUTOMATION_IMPL_H_
#define AUTOMATION_IMPL_H_

#include "Editor/Automation.h"

#include "TTScore.h"

namespace OSSIA
{
  class Automation_Impl : public Automation
  {
  public:
    
    // Constructors, destructor, assignment
    Automation_Impl();
    virtual ~Automation_Impl();
    
    // Feature
    virtual void play() const;
    
    // Iterators
    virtual const_iterator begin() const override;
    virtual const_iterator end() const override;
    virtual const_iterator find(const Address&) const override;
    
    // Managing Addresses
    virtual void addAddress(const Address&);
    virtual bool removeAddress(const Address&);
    
    // Accessors
    virtual TimeValue getPeriod() const;
    virtual void setPeriod(const TimeValue);
    virtual Curve & getCurve() const;
    virtual void setCurve(const Curve&);
    
    /** Implementation Specific
     @details use mutable members to break constness of the API because Jamoma doesn't take care of it.
     */
  };
}

#endif /* AUTOMATION_IMPL_H_ */
