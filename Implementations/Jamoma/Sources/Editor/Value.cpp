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
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Bool::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Bool::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Bool::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Bool::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Bool::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }



bool Int::operator== (const SafeValue& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Int::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Int::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Int::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Int::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Int::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }



bool Float::operator== (const SafeValue& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Float::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Float::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Float::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Float::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Float::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }



bool Char::operator== (const SafeValue& v) const
{ return Comparisons::equal<Comparisons::NumericValue>(*this, v); }

bool Char::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::NumericValue>(*this, v); }

bool Char::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::NumericValue>(*this, v); }

bool Char::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::NumericValue>(*this, v); }

bool Char::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::NumericValue>(*this, v); }

bool Char::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::NumericValue>(*this, v); }



bool String::operator== (const SafeValue& v) const
{ return Comparisons::equal<Comparisons::StringValue>(*this, v); }

bool String::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::StringValue>(*this, v); }

bool String::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::StringValue>(*this, v); }

bool String::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::StringValue>(*this, v); }

bool String::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::StringValue>(*this, v); }

bool String::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::StringValue>(*this, v); }


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
{ return Comparisons::equal<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::TupleValue>(*this, v); }

bool Tuple::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::TupleValue>(*this, v); }

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
{ return Comparisons::equal<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator!= (const SafeValue& v) const
{ return Comparisons::different<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator> (const SafeValue& v) const
{ return Comparisons::greater<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator>= (const SafeValue& v) const
{ return Comparisons::greater_equal<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator< (const SafeValue& v) const
{ return Comparisons::smaller<Comparisons::DestinationValue>(*this, v); }

bool Destination::operator<= (const SafeValue& v) const
{ return Comparisons::smaller_equal<Comparisons::DestinationValue>(*this, v); }



