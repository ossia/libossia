/*!
 * \file Value.h
 *
 * \defgroup Editor
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
#include <string>
#include <vector>

namespace OSSIA
{

class Value
{

public:

# pragma mark -
# pragma mark Enumerations
  
  /*! type of value */
  enum class Type
  {
    IMPULSE,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    DESTINATION,  //! \note see Destination structure declaration in Node.h
    BEHAVIOR,     //! \note see Behavior structure declaration in Curve.h
    TUPLE,
    GENERIC
  };
  
# pragma mark -
# pragma mark Life cycle
  
  /*! clone */
  virtual Value * clone() const = 0;
  
  /*! destructor */
  virtual ~Value() = default;
  
# pragma mark -
# pragma mark Operators
  
  /*! equal operator */
  virtual bool operator== (const Value& v) const = 0;
  
  /*! different operator */
  virtual bool operator!= (const Value& v) const = 0;
  
  /*! greater than operator */
  virtual bool operator> (const Value& v) const = 0;
  
  /*! greater than and equal operator */
  virtual bool operator>= (const Value& v) const = 0;
  
  /*! less than operator */
  virtual bool operator< (const Value& v) const = 0;
  
  /*! less than and equal operator */
  virtual bool operator<= (const Value& v) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the address value type
   \return #Type of the address value */
  Type getType() const {return m_type;}
  
protected:
  
  Type m_type;
};

# pragma mark -
# pragma mark Impulse
  
/*! \details Impulse value */
struct Impulse : public Value
{
  /*! constructor */
  Impulse()
  {
    m_type = Type::IMPULSE;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Impulse();
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    return v.getType() == m_type;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    return v.getType() == m_type;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    return v.getType() != m_type;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    return v.getType() == m_type;
  }
};

# pragma mark -
# pragma mark Bool
  
/*! \details Bool value */
struct Bool : public Value
{
  bool value;
  
  /*! constructor
   \param bool value */
  Bool(bool v = false) : value(v)
  {
    m_type = Type::BOOL;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Bool(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Bool* b = (Bool*)&v;
      return value == b->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Bool* b = (Bool*)&v;
      return value != b->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Bool* b = (Bool*)&v;
      return value > b->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Bool* b = (Bool*)&v;
      return value >= b->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() < m_type)
    {
      Bool* b = (Bool*)&v;
      return value < b->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Bool* b = (Bool*)&v;
      return value <= b->value;
    }
    
    return false;
  }
};

static Bool False = Bool(false);
static Bool True = Bool(true);

# pragma mark -
# pragma mark Int

/*! \details Int value */
struct Int : public Value
{
  int value;
  
  /*! constructor
   \param int value */
  Int(int v = 0) : value(v)
  {
    m_type = Type::INT;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Int(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Int* i = (Int*)&v;
      return value == i->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Int* i = (Int*)&v;
      return value != i->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Int* i = (Int*)&v;
      return value > i->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Int* i = (Int*)&v;
      return value >= i->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() < m_type)
    {
      Int* i = (Int*)&v;
      return value < i->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Int* i = (Int*)&v;
      return value <= i->value;
    }
    
    return false;
  }
};
  
# pragma mark -
# pragma mark Float

/*! \details Float value */
struct Float : public Value
{
  float value;
  
  /*! constructor
   \param float value */
  Float(float v = 0.) : value(v)
  {
    m_type = Type::FLOAT;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Float(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value == f->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value != f->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value > f->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value >= f->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value < f->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Float* f = (Float*)&v;
      return value <= f->value;
    }
    
    return false;
  }
};
  
# pragma mark -
# pragma mark Char
  
/*! \details Char value */
struct Char : public Value
{
  char value;
  
  /*! constructor
   \param char value */
  Char(char v = 0x00) : value(v)
  {
    m_type = Type::CHAR;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Char(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value == c->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value != c->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value > c->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value >= c->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value < c->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Char* c = (Char*)&v;
      return value <= c->value;
    }
    
    return false;
  }
};
  
# pragma mark -
# pragma mark String

/*! \details String value */
struct String : public Value
{
  std::string value;
  
  /*! constructor
   \param std::string value */
  String(std::string v = "") : value(v)
  {
    m_type = Type::STRING;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new String(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value == s->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value != s->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value > s->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value >= s->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value < s->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      String* s = (String*)&v;
      return value <= s->value;
    }
    
    return false;
  }
};

# pragma mark -
# pragma mark Tuple
  
/*! \details Tuple value */
struct Tuple : public Value
{
  std::vector<Value*> value;
  
  /*! constructor
  \param std::vector<const #Value> value*/
  Tuple(std::vector<const Value*> v = std::vector<const Value*>())
  {
    m_type = Type::TUPLE;
    
    for (const auto & e : v)
      value.push_back(e->clone());
  }
  
  /*! clone */
  Value * clone() const override
  {
    std::vector<const Value*> newValue;
    for (const auto & e : value)
      newValue.push_back(e->clone());
      
    return new Tuple(newValue);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
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
  
  /*! different operator */
  bool operator!= (const Value& v) const override
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
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
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
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
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
  
  /*! less than operator */
  bool operator< (const Value& v) const override
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
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
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
};
  
# pragma mark -
# pragma mark Generic
  
/*! \details Generic value */
template<typename T>
struct Generic : public Value
{
  T value;
  
  /*! constructor */
  Generic(T v) : value(v)
  {
    m_type = Type::GENERIC;
  }
  
  /*! clone */
  Value * clone() const override
  {
    return new Generic(value);
  }
  
  /*! equal operator */
  bool operator== (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value == g->value;
    }
    
    return false;
  }
  
  /*! different operator */
  bool operator!= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value != g->value;
    }
    
    return false;
  }
  
  /*! greater than operator */
  bool operator> (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value > g->value;
    }
    
    return false;
  }
  
  /*! greater than and equal operator */
  bool operator>= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value >= g->value;
    }
    
    return false;
  }
  
  /*! less than operator */
  bool operator< (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value < g->value;
    }
    
    return false;
  }
  
  /*! less than and equal operator */
  bool operator<= (const Value& v) const override
  {
    if (v.getType() == m_type)
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value <= g->value;
    }
    
    return false;
  }
};

}

