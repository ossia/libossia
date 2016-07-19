#include "Editor/Curve.h"
#include "Editor/Value.h"
#include "Network/Node.h"

using namespace OSSIA;


template<typename Kind, typename T>
bool equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 == v2; }); }

template<typename Kind, typename T>
bool different(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 != v2; }); }


template<typename Kind, typename T>
bool greater(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 > v2; }); }

template<typename Kind, typename T>
bool greater_equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 >= v2; }); }


template<typename Kind, typename T>
bool smaller(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 < v2; }); }

template<typename Kind, typename T>
bool smaller_equal(const T& lhs, const OSSIA::Value& rhs)
{ return Kind::apply(lhs, rhs, [] (auto&& v1, auto&& v2) { return v1 <= v2; }); }

struct Impulse_T
{
        template<typename T>
        friend bool operator==(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator!=(const T&, Impulse_T) { return false; }
        template<typename T>
        friend bool operator<=(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator>=(const T&, Impulse_T) { return true; }
        template<typename T>
        friend bool operator<(const T&, Impulse_T) { return false; }
        template<typename T>
        friend bool operator>(const T&, Impulse_T) { return true; }
};

struct NumericValue
{
        template<typename T, typename Fun>
        static bool apply(const T& lhs, const OSSIA::Value& v, Fun fun)
        {
            switch (v.getType())
            {
                case Value::Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }
                case Value::Type::BOOL :
                {
                    return fun(lhs.value, static_cast<const Bool&>(v).value);
                }
                case Value::Type::INT :
                {
                    return fun(lhs.value, static_cast<const Int&>(v).value);
                }
                case Value::Type::FLOAT :
                {
                    return fun(lhs.value, static_cast<const Float&>(v).value);
                }
                case Value::Type::CHAR :
                {
                    return fun(lhs.value, static_cast<const Char&>(v).value);
                }
                case Value::Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);
                    return (t.value.size() == 1) && (fun(lhs, *t.value[0]));
                }
                case Value::Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);
                    if (d.value->getAddress())
                    {
                        auto c = d.value->getAddress()->cloneValue(d.index);
                        return fun(lhs, *c);
                    }
                    return false;
                }
                default :
                    return false;
            }
        }
};

struct StringValue
{
        template<typename Fun>
        static bool apply(const String& lhs, const OSSIA::Value& v, Fun fun)
        {
            switch (v.getType())
            {
                case Value::Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }
                case Value::Type::STRING :
                {
                    return fun(lhs.value, static_cast<const String&>(v).value);
                }
                case Value::Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);
                    return (t.value.size() == 1) && (fun(lhs, *t.value[0]));
                }
                case Value::Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);
                    if (d.value->getAddress())
                    {
                        auto c = d.value->getAddress()->cloneValue(d.index);
                        return fun(lhs, *c);
                    }
                    return false;
                }
                default :
                    return false;
            }
        }
};

struct TupleValue
{
        template<typename Fun>
        static bool apply(const Tuple& lhs, const OSSIA::Value& v, Fun fun)
        {
            switch (v.getType())
            {
                case Value::Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }

                case Value::Type::TUPLE :
                {
                    auto& t = static_cast<const Tuple&>(v);

                    if (lhs.value.size() != t.value.size())
                        return false;

                    bool result = true;
                    auto tit = t.value.begin();
                    for (const auto& val : lhs.value)
                    {
                        result &= fun(*val, **tit);
                        if (!result)
                            break;
                        tit++;
                    }

                    return result;
                }

                default :
                {
                    if (lhs.value.size() == 1)
                        return fun(*lhs.value[0], v);

                    return false;
                }
            }
        }
};

struct DestinationValue
{
        template<typename Fun>
        static bool apply(const Destination& lhs, const OSSIA::Value& v, Fun fun)
        {
            switch (v.getType())
            {
                case Value::Type::IMPULSE :
                {
                    return fun(lhs.value, Impulse_T{});
                }

                case Value::Type::DESTINATION :
                {
                    auto& d = static_cast<const Destination&>(v);

                    // if there are addresses compare values
                    if (lhs.value->getAddress() && d.value->getAddress())
                    {
                        auto c1 = lhs.value->getAddress()->cloneValue(lhs.index);
                        auto c2 = d.value->getAddress()->cloneValue(d.index);
                        return fun(*c1, *c2);
                    }

                    // if no addresses, compare nodes
                    else if (!lhs.value->getAddress() && !d.value->getAddress())
                    {
                        return fun(lhs.value, d.value);
                    }
                    // TODO fallthrough ??
                }

                default :
                {
                    if (lhs.value->getAddress())
                    {
                        auto c = lhs.value->getAddress()->cloneValue(lhs.index);
                        return fun(*c, v);
                    }

                    return false;
                }
            }
        }
};

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
{ return equal<NumericValue>(*this, v); }

bool Bool::operator!= (const Value& v) const
{ return different<NumericValue>(*this, v); }

bool Bool::operator> (const Value& v) const
{ return greater<NumericValue>(*this, v); }

bool Bool::operator>= (const Value& v) const
{ return greater_equal<NumericValue>(*this, v); }

bool Bool::operator< (const Value& v) const
{ return smaller<NumericValue>(*this, v); }

bool Bool::operator<= (const Value& v) const
{ return smaller_equal<NumericValue>(*this, v); }

# pragma mark -
# pragma mark Int



Int::~Int() = default;

Value * Int::clone() const
{
    return new Int(value);
}

bool Int::operator== (const Value& v) const
{ return equal<NumericValue>(*this, v); }

bool Int::operator!= (const Value& v) const
{ return different<NumericValue>(*this, v); }

bool Int::operator> (const Value& v) const
{ return greater<NumericValue>(*this, v); }

bool Int::operator>= (const Value& v) const
{ return greater_equal<NumericValue>(*this, v); }

bool Int::operator< (const Value& v) const
{ return smaller<NumericValue>(*this, v); }

bool Int::operator<= (const Value& v) const
{ return smaller_equal<NumericValue>(*this, v); }

# pragma mark -
# pragma mark Float



Float::~Float() = default;

Value * Float::clone() const
{
    return new Float(value);
}

bool Float::operator== (const Value& v) const
{ return equal<NumericValue>(*this, v); }

bool Float::operator!= (const Value& v) const
{ return different<NumericValue>(*this, v); }

bool Float::operator> (const Value& v) const
{ return greater<NumericValue>(*this, v); }

bool Float::operator>= (const Value& v) const
{ return greater_equal<NumericValue>(*this, v); }

bool Float::operator< (const Value& v) const
{ return smaller<NumericValue>(*this, v); }

bool Float::operator<= (const Value& v) const
{ return smaller_equal<NumericValue>(*this, v); }
# pragma mark -
# pragma mark Char



Char::~Char() = default;

/*! clone */
Value * Char::clone() const
{
    return new Char(value);
}

bool Char::operator== (const Value& v) const
{ return equal<NumericValue>(*this, v); }

bool Char::operator!= (const Value& v) const
{ return different<NumericValue>(*this, v); }

bool Char::operator> (const Value& v) const
{ return greater<NumericValue>(*this, v); }

bool Char::operator>= (const Value& v) const
{ return greater_equal<NumericValue>(*this, v); }

bool Char::operator< (const Value& v) const
{ return smaller<NumericValue>(*this, v); }

bool Char::operator<= (const Value& v) const
{ return smaller_equal<NumericValue>(*this, v); }

# pragma mark -
# pragma mark String



String::~String() = default;

Value * String::clone() const
{ return new String(value); }

bool String::operator== (const Value& v) const
{ return equal<StringValue>(*this, v); }

bool String::operator!= (const Value& v) const
{ return different<StringValue>(*this, v); }

bool String::operator> (const Value& v) const
{ return greater<StringValue>(*this, v); }

bool String::operator>= (const Value& v) const
{ return greater_equal<StringValue>(*this, v); }

bool String::operator< (const Value& v) const
{ return smaller<StringValue>(*this, v); }

bool String::operator<= (const Value& v) const
{ return smaller_equal<StringValue>(*this, v); }


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
{ return equal<TupleValue>(*this, v); }

bool Tuple::operator!= (const Value& v) const
{ return different<TupleValue>(*this, v); }

bool Tuple::operator> (const Value& v) const
{ return greater<TupleValue>(*this, v); }

bool Tuple::operator>= (const Value& v) const
{ return greater_equal<TupleValue>(*this, v); }

bool Tuple::operator< (const Value& v) const
{ return smaller<TupleValue>(*this, v); }

bool Tuple::operator<= (const Value& v) const
{ return smaller_equal<TupleValue>(*this, v); }


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
{ return equal<DestinationValue>(*this, v); }

bool Destination::operator!= (const Value& v) const
{ return different<DestinationValue>(*this, v); }

bool Destination::operator> (const Value& v) const
{ return greater<DestinationValue>(*this, v); }

bool Destination::operator>= (const Value& v) const
{ return greater_equal<DestinationValue>(*this, v); }

bool Destination::operator< (const Value& v) const
{ return smaller<DestinationValue>(*this, v); }

bool Destination::operator<= (const Value& v) const
{ return smaller_equal<DestinationValue>(*this, v); }

# pragma mark -
# pragma mark Behavior



Behavior::~Behavior() = default;

Value * Behavior::clone() const
{
    return new Behavior(value);
}
