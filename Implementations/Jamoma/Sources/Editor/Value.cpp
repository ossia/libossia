#include "Editor/Curve.h"
#include <Editor/Value/Value.h>
#include <Editor/Value/SafeValue.h>
#include "Network/Node.h"
#include <Editor/Value/ValueComparisonImpl.h>
#include <Editor/Value/Behavior.h>

using namespace OSSIA;

Impulse::~Impulse() = default;
Bool::~Bool() = default;
Int::~Int() = default;
Float::~Float() = default;
Char::~Char() = default;
String::~String() = default;
Tuple::~Tuple() = default;
Destination::~Destination() = default;
Behavior::~Behavior() = default;

bool Bool::operator== (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Bool::operator!= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Bool::operator> (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Bool::operator>= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Bool::operator< (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Bool::operator<= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Int::operator== (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Int::operator!= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Int::operator> (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Int::operator>= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Int::operator< (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Int::operator<= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Float::operator== (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Float::operator!= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Float::operator> (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Float::operator>= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Float::operator< (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Float::operator<= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Char::operator== (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Char::operator!= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Char::operator> (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Char::operator>= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Char::operator< (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Char::operator<= (const SafeValue& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }




bool String::operator== (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::equal_to<>{}); }

bool String::operator!= (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::not_equal_to<>{}); }

bool String::operator> (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::greater<>{}); }

bool String::operator>= (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::greater_equal<>{}); }

bool String::operator< (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::less<>{}); }

bool String::operator<= (const SafeValue& v) const
{ return Comparisons::StringValue::apply(*this, v, std::less_equal<>{}); }



Tuple::Tuple() = default;
Tuple::Tuple(std::initializer_list<SafeValue> v):
  value(v)
{
}

Tuple::Tuple(const std::vector<SafeValue>& v):
  value(v)
{
}

Tuple::Tuple(std::vector<SafeValue>&& v):
  value(std::move(v))
{
}

Tuple::Tuple(const SafeValue& v)
{
  value.push_back(v);
}


bool Tuple::operator== (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::equal_to<>{}); }

bool Tuple::operator!= (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::not_equal_to<>{}); }

bool Tuple::operator> (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::greater<>{}); }

bool Tuple::operator>= (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::greater_equal<>{}); }

bool Tuple::operator< (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::less<>{}); }

bool Tuple::operator<= (const SafeValue& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::less_equal<>{}); }


Destination::Destination(
        std::shared_ptr<Node> v,
        std::initializer_list<char> idx) :
    value(std::move(v))
{
    for (const auto & i : idx)
        index.push_back(i);
}

Destination::Destination(std::shared_ptr<Node> v, std::vector<char> idx) :
    value(v)
{
    for (const auto & i : idx)
        index.push_back(i);
}


bool Destination::operator== (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::equal_to<>{}); }

bool Destination::operator!= (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::not_equal_to<>{}); }

bool Destination::operator> (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::greater<>{}); }

bool Destination::operator>= (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::greater_equal<>{}); }

bool Destination::operator< (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::less<>{}); }

bool Destination::operator<= (const SafeValue& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::less_equal<>{}); }


bool SafeValue::operator==(const SafeValue &rhs) const
{ return v == rhs.v; }

bool SafeValue::operator!=(const SafeValue &rhs) const
{ return v != rhs.v; }

bool SafeValue::operator>(const SafeValue &rhs) const
{ return v > rhs.v; }

bool SafeValue::operator>=(const SafeValue &rhs) const
{ return v >= rhs.v; }

bool SafeValue::operator<(const SafeValue &rhs) const
{ return v < rhs.v; }

bool SafeValue::operator<=(const SafeValue &rhs) const
{ return v <= rhs.v; }

