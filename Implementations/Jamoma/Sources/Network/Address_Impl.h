/*!
 * \file Address_Impl.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef ADDRESS_IMPL_H_
#define ADDRESS_IMPL_H_

#include "Editor/Address.h"

#include "TTScore.h"

namespace OSSIA
{
  class Address_Impl : public Address
  {
  public:
    
    // Constructors, destructor, assignment
    Address_Impl();
    virtual ~Address_Impl();
    
    // Factories
    virtual Address addAddress(std::string) const;
    
    // Navigation
    virtual bool isRoot() const;
    virtual Address & getParent() const;
    virtual Device<T> & getDevice() const;
    
    // Iterators
    virtual const_iterator begin() const;
    virtual const_iterator end() const;
    virtual const_iterator find(const Address&) const;
    
    // Managing children
    virtual void addChild(const Address&);
    virtual bool removeChild(const Address&);
    
    // Accessors
    virtual std::string getAddress() const;
    
    /** Implementation Specific
     @details use mutable members to break constness of the API because Jamoma doesn't take care of it.
     */
    TTAddress   mAddress;
  };
}

#endif /* ADDRESS_IMPL_H_ */
