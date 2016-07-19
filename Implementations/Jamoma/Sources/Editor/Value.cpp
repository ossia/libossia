#include "Editor/Curve.h"
#include "Editor/Value.h"
#include "Network/Node.h"
#include <Editor/ValueComparisonImpl.h>
#include <Editor/Behavior.h>

using namespace OSSIA;

Value::~Value() = default;

# pragma mark -
# pragma mark Impulse



Impulse::~Impulse() = default;

Value * Impulse::clone() const
{
    return new Impulse();
}

# pragma mark -
# pragma mark Bool



Bool::~Bool() = default;

Value * Bool::clone() const
{
    return new Bool(value);
}

bool Bool::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Bool::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Bool::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Bool::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Bool::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Bool::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }

# pragma mark -
# pragma mark Int



Int::~Int() = default;

Value * Int::clone() const
{
    return new Int(value);
}

bool Int::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Int::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Int::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Int::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Int::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Int::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }

# pragma mark -
# pragma mark Float



Float::~Float() = default;

Value * Float::clone() const
{
    return new Float(value);
}

bool Float::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Float::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Float::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Float::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Float::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Float::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }
# pragma mark -
# pragma mark Char



Char::~Char() = default;

/*! clone */
Value * Char::clone() const
{
    return new Char(value);
}

bool Char::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Char::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Char::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Char::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Char::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Char::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }

# pragma mark -
# pragma mark String



String::~String() = default;

Value * String::clone() const
{ return new String(value); }

bool String::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::StringValue>(*this, v); }

bool String::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::StringValue>(*this, v); }

bool String::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::StringValue>(*this, v); }

bool String::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::StringValue>(*this, v); }

bool String::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::StringValue>(*this, v); }

bool String::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::StringValue>(*this, v); }


# pragma mark -
# pragma mark Tuple

Tuple::~Tuple()
{
    for(auto val : value)
        delete val;
}


Value * Tuple::clone() const
{
    std::vector<const Value*> newValue;
    for (const auto & e : value)
        newValue.push_back(e->clone());

    return new Tuple(newValue);
}

bool Tuple::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::TupleValue>(*this, v); }


# pragma mark -
# pragma mark Destination

Destination::Destination(
        std::shared_ptr<Node> v,
        std::initializer_list<char> idx) :
    Value{Type::DESTINATION},
    value(std::move(v))
{
    for (const auto & i : idx)
        index.push_back(i);
}

Destination::Destination(std::shared_ptr<Node> v, std::vector<char> idx) :
    Value{Type::DESTINATION},
    value(v)
{
    for (const auto & i : idx)
        index.push_back(i);
}

Destination::~Destination() = default;

Value * Destination::clone() const
{ return new Destination(value, index); }

bool Destination::operator== (const Value& v) const
{ return Comparisons::equal<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator!= (const Value& v) const
{ return Comparisons::different<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator> (const Value& v) const
{ return Comparisons::greater<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator>= (const Value& v) const
{ return Comparisons::greater_equal<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator< (const Value& v) const
{ return Comparisons::smaller<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator<= (const Value& v) const
{ return Comparisons::smaller_equal<Comparisons::DestinationValue>(*this, v); }

# pragma mark -
# pragma mark Behavior



Behavior::~Behavior() = default;

Value * Behavior::clone() const
{
    return new Behavior(value);
}
