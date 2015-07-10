/*!
 * \file Node.h
 *
 * \defgroup Network
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

#include "Editor/Value.h"
#include "Network/Address.h"
#include "Misc/Container.h"

namespace OSSIA
{

class Address;

class Node
{

public:
  
  using iterator = Container<Node>::iterator;
  using const_iterator = Container<Node>::const_iterator;

# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~Node() = default;

# pragma mark -
# pragma mark Navigation
  
  /*! get node's parent 
   \return std::shared_ptr<Node> the parent */
  virtual std::shared_ptr<Node> getParent() const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get node's name
   \return std::string the name */
  virtual std::string getName() const = 0;
  
  /*! get node's address
   \return std::shared_ptr<#Address> the address */
  virtual const std::shared_ptr<OSSIA::Address> & getAddress() const = 0;
  
# pragma mark -
# pragma mark Address
  
  /*! create node's address
   \param #Value::Type the type of the address to create
   \return std::shared_ptr<#Address> the new address */
  virtual std::shared_ptr<OSSIA::Address> createAddress(
                Value::Type = Value::Type::IMPULSE) = 0;
  
  /*! remove node's address
   \return bool true if the address is correctly removed */
  virtual bool removeAddress() = 0;

# pragma mark -
# pragma mark Children
  
  /*! create and store a child node
   \param #Container<#Node>::const_iterator where to store the child
   \param std::string child name
   \return #Container<#Node>::iterator */
  virtual iterator emplace(const_iterator, std::string) = 0;
  
  /*! get children of the node
   \return #Container<#Node> */
  Container<Node>& children()
  { return m_children; }
  
  /*! get children of the node
   \return #Container<#Node> */
  const Container<Node>& children() const
  { return m_children; }

protected:
  Container<Node> m_children;
};
  
# pragma mark -
# pragma mark Destination
  
/*! \details Destination value */
struct Destination : public Value
{
  std::shared_ptr<Node> value;
  
  /*! constructor */
  Destination(std::shared_ptr<Node> v) : value(v)
  {
    m_type = Type::DESTINATION;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Destination(value);
  }

  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) == (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) == v;
      }
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) != (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) != v;
      }
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) > (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) > v;
      }
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) >= (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) >= v;
      }
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) < (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) < v;
      }
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Destination* d = (Destination*)&v;
      if (value->getAddress() && d->value->getAddress())
        return (*value->getAddress()->getValue()) <= (*d->value->getAddress()->getValue());
    }
    else if (value->getAddress())
    {
      if (v.getType() == value->getAddress()->getValueType())
      {
        return (*value->getAddress()->getValue()) <= v;
      }
    }
    
    return false;
  }
};
  
}
