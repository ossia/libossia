#include "Editor/Curve.h"
#include "Editor/Value.h"
#include "Network/Node.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Impulse

Impulse::Impulse()
{
  m_type = Type::IMPULSE;
}

Value * Impulse::clone() const
{
  return new Impulse();
}

bool Impulse::operator== (const Value&) const
{
  //! \note an impulse is equal to anything
  return true;
}

bool Impulse::operator!= (const Value&) const
{
  //! \note an impulse cannot be different to anything
  return false;
}

bool Impulse::operator> (const Value&) const
{
  return false;
}

bool Impulse::operator>= (const Value&) const
{
  return true;
}

bool Impulse::operator< (const Value&) const
{
  return false;
}

bool Impulse::operator<= (const Value&) const
{
  return true;
}

# pragma mark -
# pragma mark Bool

Bool::Bool(bool v) : value(v)
{
  m_type = Type::BOOL;
}

Value * Bool::clone() const
{
  return new Bool(value);
}

bool Bool::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Bool* b = (Bool*)&v;
    return value == b->value;
  }
  
  return false;
}

bool Bool::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Bool* b = (Bool*)&v;
    return value != b->value;
  }
  
  return false;
}

bool Bool::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Bool* b = (Bool*)&v;
    return value > b->value;
  }
  
  return false;
}

bool Bool::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Bool* b = (Bool*)&v;
    return value >= b->value;
  }
  
  return false;
}

bool Bool::operator< (const Value& v) const
{
  if (v.getType() < m_type)
  {
    Bool* b = (Bool*)&v;
    return value < b->value;
  }
  
  return false;
}

bool Bool::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Bool* b = (Bool*)&v;
    return value <= b->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark Int

Int::Int(int v) : value(v)
{
  m_type = Type::INT;
}

Value * Int::clone() const
{
  return new Int(value);
}

bool Int::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Int* i = (Int*)&v;
    return value == i->value;
  }
  
  return false;
}

bool Int::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Int* i = (Int*)&v;
    return value != i->value;
  }
  
  return false;
}

bool Int::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Int* i = (Int*)&v;
    return value > i->value;
  }
  
  return false;
}

bool Int::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Int* i = (Int*)&v;
    return value >= i->value;
  }
  
  return false;
}

bool Int::operator< (const Value& v) const
{
  if (v.getType() < m_type)
  {
    Int* i = (Int*)&v;
    return value < i->value;
  }
  
  return false;
}

bool Int::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Int* i = (Int*)&v;
    return value <= i->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark Float

Float::Float(float v) : value(v)
{
  m_type = Type::FLOAT;
}

Value * Float::clone() const
{
  return new Float(value);
}

bool Float::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value == f->value;
  }
  
  return false;
}

bool Float::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value != f->value;
  }
  
  return false;
}

bool Float::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value > f->value;
  }
  
  return false;
}

bool Float::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value >= f->value;
  }
  
  return false;
}

bool Float::operator< (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value < f->value;
  }
  
  return false;
}

bool Float::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Float* f = (Float*)&v;
    return value <= f->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark Char

Char::Char(char v) : value(v)
{
  m_type = Type::CHAR;
}

/*! clone */
Value * Char::clone() const
{
  return new Char(value);
}

bool Char::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value == c->value;
  }
  
  return false;
}

bool Char::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value != c->value;
  }
  
  return false;
}

bool Char::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value > c->value;
  }
  
  return false;
}

bool Char::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value >= c->value;
  }
  
  return false;
}

bool Char::operator< (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value < c->value;
  }
  
  return false;
}

bool Char::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Char* c = (Char*)&v;
    return value <= c->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark String

String::String(std::string v) : value(v)
{
  m_type = Type::STRING;
}

Value * String::clone() const
{
  return new String(value);
}

bool String::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value == s->value;
  }
  
  return false;
}

bool String::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value != s->value;
  }
  
  return false;
}

bool String::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value > s->value;
  }
  
  return false;
}

bool String::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value >= s->value;
  }
  
  return false;
}

bool String::operator< (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value < s->value;
  }
  
  return false;
}

bool String::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    String* s = (String*)&v;
    return value <= s->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark Tuple

Tuple::Tuple(std::vector<const Value*> v)
{
  m_type = Type::TUPLE;
  
  for (const auto & e : v)
    value.push_back(e->clone());
}

Value * Tuple::clone() const
{
  std::vector<const Value*> newValue;
  for (const auto & e : value)
    newValue.push_back(e->clone());
  
  return new Tuple(newValue);
}

bool Tuple::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) == (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

bool Tuple::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) != (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

bool Tuple::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) > (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

bool Tuple::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) >= (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

bool Tuple::operator< (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) < (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

bool Tuple::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Tuple* t = (Tuple*)&v;
    
    if (value.size() != t->value.size())
      return false;
    
    bool result = true;
    auto tit = t->value.begin();
    for (auto it = value.begin(); it != value.end();it++)
    {
      result &= (**it) <= (**tit);
      if (!result)
        break;
      tit++;
    }
    
    return result;
  }
  
  return false;
}

# pragma mark -
# pragma mark Generic

template<typename T>
Generic<T>::Generic(T v) : value(v)
{
  m_type = Type::GENERIC;
}

template<typename T>
Value * Generic<T>::clone() const
{
  return new Generic(value);
}

template<typename T>
bool Generic<T>::operator== (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value == g->value;
  }
  
  return false;
}

template<typename T>
bool Generic<T>::operator!= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value != g->value;
  }
  
  return false;
}

template<typename T>
bool Generic<T>::operator> (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value > g->value;
  }
  
  return false;
}

template<typename T>
bool Generic<T>::operator>= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value >= g->value;
  }
  
  return false;
}

template<typename T>
bool Generic<T>::operator< (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value < g->value;
  }
  
  return false;
}

template<typename T>
bool Generic<T>::operator<= (const Value& v) const
{
  if (v.getType() == m_type)
  {
    Generic<T>* g = (Generic<T>*)&v;
    return value <= g->value;
  }
  
  return false;
}

# pragma mark -
# pragma mark Destination

Destination::Destination(std::shared_ptr<Node> v) : value(v)
{
  m_type = Type::DESTINATION;
}

Value * Destination::clone() const
{
  return new Destination(value);
}

bool Destination::operator== (const Value& v) const
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

bool Destination::operator!= (const Value& v) const
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

bool Destination::operator> (const Value& v) const
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

bool Destination::operator>= (const Value& v) const
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

bool Destination::operator< (const Value& v) const
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

bool Destination::operator<= (const Value& v) const
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

# pragma mark -
# pragma mark Behavior

Behavior::Behavior(std::shared_ptr<CurveAbstract> v) : value(v)
{
  m_type = Type::BEHAVIOR;
}

Value * Behavior::clone() const
{
  return new Behavior(value);
}

bool Behavior::operator== (const Value& v) const
{
  return v.getType() == m_type;
}

bool Behavior::operator!= (const Value& v) const
{
  return v.getType() != m_type;
}

bool Behavior::operator> (const Value& v) const
{
  return v.getType() != m_type;
}

bool Behavior::operator>= (const Value& v) const
{
  return v.getType() == m_type;
}

bool Behavior::operator< (const Value& v) const
{
  return v.getType() != m_type;
}

bool Behavior::operator<= (const Value& v) const
{
  return v.getType() == m_type;
}
