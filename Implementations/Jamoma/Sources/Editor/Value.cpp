#include "Editor/Curve.h"
#include "Editor/Value.h"
#include "Network/Node.h"

using namespace OSSIA;

Value::~Value()
{}

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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value == i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value == f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value == c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Bool::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Bool::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value > b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value > i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value > f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value > c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Bool::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value >= b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value >= i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value >= f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value >= c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Bool::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value < b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value < i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value < f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value < c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this < *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this < *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

bool Bool::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value <= b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value <= i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value <= f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value <= c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this <= *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this <= *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value == i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value == f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value == c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Int::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Int::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value > b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value > i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value > f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value > c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Int::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value >= b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value >= i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value >= f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value >= c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      return false;
    }
    case Value::Type::BEHAVIOR :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    default :
      return false;
  }
}

bool Int::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value < b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value < i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value < f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value < c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this < *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this < *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

bool Int::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value <= b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value <= i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value <= f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value <= c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this <= *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        return false;
      }
      case Value::Type::BEHAVIOR :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this <= *c;

          delete c;
          return result;
        }
      }
      default :
        return false;
    }
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value == i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value == f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value == c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Float::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Float::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value > b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value > i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value > f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value > c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Float::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value >= b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value >= i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value >= f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value >= c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Float::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value < b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value < i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value < f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value < c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this < *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this < *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

bool Float::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value <= b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value <= i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value <= f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value <= c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this <= *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this <= *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value == i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value == f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value == c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Char::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Char::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value > b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value > i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value > f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value > c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Char::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value >= b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value >= i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value >= f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value >= c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool Char::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value < b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value < i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value < f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value < c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this < *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this < *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

bool Char::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::BOOL :
      {
        auto b = static_cast<const Bool*>(&v);
        return value <= b->value;
      }
      case Value::Type::INT :
      {
        auto i = static_cast<const Int*>(&v);
        return value <= i->value;
      }
      case Value::Type::FLOAT :
      {
        auto f = static_cast<const Float*>(&v);
        return value <= f->value;
      }
      case Value::Type::CHAR :
      {
        auto c = static_cast<const Char*>(&v);
        return value <= c->value;
      }
      case Value::Type::STRING :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this <= *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this <= *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      return false;
    }
    case Value::Type::INT :
    {
      return false;
    }
    case Value::Type::FLOAT :
    {
      return false;
    }
    case Value::Type::CHAR :
    {
      return false;
    }
    case Value::Type::STRING :
    {
      auto s = static_cast<const String*>(&v);
      return value == s->value;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool String::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool String::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::BOOL :
    {
      return false;
    }
    case Value::Type::INT :
    {
      return false;
    }
    case Value::Type::FLOAT :
    {
      return false;
    }
    case Value::Type::CHAR :
    {
      return false;
    }
    case Value::Type::STRING :
    {
      auto s = static_cast<const String*>(&v);
      return value > s->value;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool String::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      return false;
    }
    case Value::Type::INT :
    {
      return false;
    }
    case Value::Type::FLOAT :
    {
      return false;
    }
    case Value::Type::CHAR :
    {
      return false;
    }
    case Value::Type::STRING :
    {
      auto s = static_cast<const String*>(&v);
      return value >= s->value;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      return false;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

bool String::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::BOOL :
      {
        return false;
      }
      case Value::Type::INT :
      {
        return false;
      }
      case Value::Type::FLOAT :
      {
        return false;
      }
      case Value::Type::CHAR :
      {
        return false;
      }
      case Value::Type::STRING :
      {
        auto s = static_cast<const String*>(&v);
        return value < s->value;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this < *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this < *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

bool String::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::BOOL :
      {
        return false;
      }
      case Value::Type::INT :
      {
        return false;
      }
      case Value::Type::FLOAT :
      {
        return false;
      }
      case Value::Type::CHAR :
      {
        return false;
      }
      case Value::Type::STRING :
      {
        auto s = static_cast<const String*>(&v);
        return value <= s->value;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);
        if (t->value.size() == 1)
          return *this <= *t->value[0];
        else
          return false;
      }
      case Value::Type::GENERIC :
      {
        return false;
      }
      case Value::Type::DESTINATION :
      {
        auto d = static_cast<const Destination*>(&v);
        if (d->value->getAddress())
        {
          const Value* c = d->value->getAddress()->cloneValue(d->index);
          bool result = *this <= *c;

          delete c;
          return result;
        }
      }
      case Value::Type::BEHAVIOR :
      {
        return false;
      }
      default :
        return false;
    }
}

# pragma mark -
# pragma mark Tuple

Tuple::Tuple()
{
    m_type = Type::TUPLE;
}

Tuple::~Tuple()
{
    for(auto val : value)
        delete val;
}

Tuple::Tuple(const Value* v)
{
  m_type = Type::TUPLE;

  value.push_back(v->clone());
}

Tuple::Tuple(std::initializer_list<const Value*> v)
{
  m_type = Type::TUPLE;

  for (const auto & e : v)
    value.push_back(e->clone());
}

Tuple::Tuple(const std::vector<const Value*>& v)
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);

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
    default :
    {
      if (value.size() == 1)
        return *value[0] == v;

      return false;
    }
  }
}

bool Tuple::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Tuple::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);

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
    default :
    {
      if (value.size() == 1)
        return *value[0] > v;

      return false;
    }
  }
}

bool Tuple::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);

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
    default :
    {
      if (value.size() == 1)
        return *value[0] >= v;

      return false;
    }
  }
}

bool Tuple::operator< (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return false;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);

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
      default :
      {
        if (value.size() == 1)
          return *value[0] < v;

        return false;
      }
    }
}

bool Tuple::operator<= (const Value& v) const
{
    switch (v.getType())
    {
      case Value::Type::IMPULSE :
      {
        return true;
      }
      case Value::Type::TUPLE :
      {
        auto t = static_cast<const Tuple*>(&v);

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
      default :
      {
        if (value.size() == 1)
          return *value[0] <= v;

        return false;
      }
    }
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
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value == i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value == f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value == c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this == *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value == g->value;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this == *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

template<typename T>
bool Generic<T>::operator!= (const Value& v) const
{
  return !(*this == v);
}

template<typename T>
bool Generic<T>::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value == b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value > i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value > f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value > c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this > *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value > g->value;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this > *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

template<typename T>
bool Generic<T>::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(&v);
      return value >= b->value;
    }
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(&v);
      return value >= i->value;
    }
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(&v);
      return value >= f->value;
    }
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(&v);
      return value >= c->value;
    }
    case Value::Type::STRING :
    {
      return false;
    }
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(&v);
      if (t->value.size() == 1)
        return *this >= *t->value[0];
      else
        return false;
    }
    case Value::Type::GENERIC :
    {
      Generic<T>* g = (Generic<T>*)&v;
      return value >= g->value;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (d->value->getAddress())
      {
        const Value* c = d->value->getAddress()->cloneValue(d->index);
        bool result = *this >= *c;

        delete c;
        return result;
      }
    }
    case Value::Type::BEHAVIOR :
    {
      return false;
    }
    default :
      return false;
  }
}

template<typename T>
bool Generic<T>::operator< (const Value& v) const
{
  return !(*this >= v);
}

template<typename T>
bool Generic<T>::operator<= (const Value& v) const
{
  return !(*this > v);
}

# pragma mark -
# pragma mark Destination

Destination::Destination(std::shared_ptr<Node> v, std::initializer_list<char> idx) : value(v)
{
  m_type = Type::DESTINATION;

  for (const auto & i : idx)
    index.push_back(i);
}

Destination::Destination(std::shared_ptr<Node> v, std::vector<char> idx) : value(v)
{
  m_type = Type::DESTINATION;

  for (const auto & i : idx)
    index.push_back(i);
}

Value * Destination::clone() const
{
  return new Destination(value, index);
}

bool Destination::operator== (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);

      // if there are addresses compare values
      if (value->getAddress() && d->value->getAddress())
      {
        const Value* c1 = value->getAddress()->cloneValue(index);
        const Value* c2 = d->value->getAddress()->cloneValue(d->index);
        bool result = *c1 == *c2;

        delete c1;
        delete c2;
        return result;
      }

      // if no addresses, compare nodes
      else if (!value->getAddress() && !d->value->getAddress())
        return value == d->value;
    }
    default :
    {
      if (value->getAddress())
      {
        const Value* c = value->getAddress()->cloneValue(index);
        bool result = *c == v;

        delete c;
        return result;
      }

      return false;
    }
  }
}

bool Destination::operator!= (const Value& v) const
{
  return !(*this == v);
}

bool Destination::operator> (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (value->getAddress() && d->value->getAddress())
      {
        const Value* c1 = value->getAddress()->cloneValue(index);
        const Value* c2 = d->value->getAddress()->cloneValue(d->index);
        bool result = *c1 > *c2;

        delete c1;
        delete c2;
        return result;
      }
    }
    default :
    {
      if (value->getAddress())
      {
        const Value* c = value->getAddress()->cloneValue(index);
        bool result = *c > v;

        delete c;
        return result;
      }

      return false;
    }
  }
}

bool Destination::operator>= (const Value& v) const
{
  switch (v.getType())
  {
    case Value::Type::IMPULSE :
    {
      return true;
    }
    case Value::Type::DESTINATION :
    {
      auto d = static_cast<const Destination*>(&v);
      if (value->getAddress() && d->value->getAddress())
      {
        const Value* c1 = value->getAddress()->cloneValue(index);
        const Value* c2 = d->value->getAddress()->cloneValue(d->index);
        bool result = *c1 >= *c2;

        delete c1;
        delete c2;
        return result;
      }
    }
    default :
    {
      if (value->getAddress())
      {
        const Value* c = value->getAddress()->cloneValue(index);
        bool result = *c >= v;

        delete c;
        return result;
      }

      return false;
    }
  }
}

bool Destination::operator< (const Value& v) const
{
  return !(*this >= v);
}

bool Destination::operator<= (const Value& v) const
{
  return !(*this > v);
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
  return false;
}

bool Behavior::operator!= (const Value& v) const
{
  return false;
}

bool Behavior::operator> (const Value& v) const
{
  return false;
}

bool Behavior::operator>= (const Value& v) const
{
  return false;
}

bool Behavior::operator< (const Value& v) const
{
  return false;
}

bool Behavior::operator<= (const Value& v) const
{
  return false;
}
