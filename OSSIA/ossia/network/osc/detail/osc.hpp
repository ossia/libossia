#pragma once
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>
#include <ossia/network/osc/detail/string_view.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/domain.hpp>

namespace oscpack
{
inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::Value& val);
}

namespace impl
{
struct OSCOutboundVisitor
{
        oscpack::OutboundPacketStream& p;
        void operator()(OSSIA::Impulse) const { }
        void operator()(OSSIA::Int i) const   { p << int32_t(i.value); }
        void operator()(OSSIA::Float f) const { p << f.value; }
        void operator()(OSSIA::Bool b) const  { p << int32_t(b.value); }
        void operator()(OSSIA::Char c) const  { p << int32_t(c.value); }
        void operator()(const OSSIA::String& str) const { p << boost::string_ref(str.value); }
        void operator()(OSSIA::Vec2f vec) const { p << vec.value[0] << vec.value[1]; }
        void operator()(OSSIA::Vec3f vec) const { p << vec.value[0] << vec.value[1] << vec.value[2]; }
        void operator()(OSSIA::Vec4f vec) const { p << vec.value[0] << vec.value[1] << vec.value[2]; }
        void operator()(const OSSIA::Destination& d) const { }
        void operator()(const OSSIA::Behavior&) const { }
        void operator()(const OSSIA::Tuple& t) const {
            for(const auto& val : t.value)
            {
                p << val;
            }
        }
};

struct OSCInboundVisitor
{
        oscpack::ReceivedMessageArgumentIterator cur_it;
        oscpack::ReceivedMessageArgumentIterator beg_it;
        oscpack::ReceivedMessageArgumentIterator end_it;
        int numArguments = 1;

        OSSIA::Value operator()(OSSIA::Impulse imp) const { return imp; }
        OSSIA::Value operator()(OSSIA::Int i) const
        {
            switch(cur_it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Int{cur_it->AsInt32Unchecked()};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Int{int32_t(cur_it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Int{int32_t(cur_it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Int{int32_t(cur_it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Int{int32_t(cur_it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::Int{1};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::Int{0};
                default:
                    return i;
            }
        }
        OSSIA::Value operator()(OSSIA::Float f) const
        {
            switch(cur_it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Float{float(cur_it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Float{float(cur_it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Float{float(cur_it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Float{float(cur_it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Float{float(cur_it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::Float{1.};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::Float{0.};
                default:
                    return f;
            }
        }

        OSSIA::Value operator()(OSSIA::Bool b) const
        {
            switch(cur_it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Bool{bool(cur_it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Bool{bool(cur_it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Bool{bool(cur_it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Bool{bool(cur_it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Bool{bool(cur_it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::Bool{true};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::Bool{false};
                default:
                    return b;
            }
        }
        OSSIA::Value operator()(OSSIA::Char c) const
        {
            switch(cur_it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Char{char(cur_it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Char{char(cur_it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Char{char(cur_it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Char{char(cur_it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Char{char(cur_it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::Char{'T'};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::Char{'F'};
                case oscpack::STRING_TYPE_TAG:
                    return OSSIA::Char{cur_it->AsStringUnchecked()[0]};
                case oscpack::SYMBOL_TYPE_TAG:
                    return OSSIA::Char{cur_it->AsSymbolUnchecked()[0]};
                default:
                    return c;
            }
        }

        OSSIA::Value operator()(const OSSIA::String& str) const
        {
            switch(cur_it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::String{std::to_string(cur_it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::String{std::to_string(cur_it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::String{std::to_string(cur_it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::String{std::to_string(cur_it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::String{std::to_string(cur_it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::String{"true"};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::String{"false"};
                case oscpack::STRING_TYPE_TAG:
                    return OSSIA::String{cur_it->AsStringUnchecked()};
                case oscpack::SYMBOL_TYPE_TAG:
                    return OSSIA::String{cur_it->AsSymbolUnchecked()};
                default:
                    return str;
            }
        }

        template<int N>
        OSSIA::Value operator()(OSSIA::Vec<float, N> vec) const
        {
            if(numArguments == N)
            {
                OSSIA::Vec<float, N> ret;
                int i = 0;
                auto vec_it = beg_it;
                auto vec_end = end_it;
                for(; vec_it != vec_end; ++vec_it)
                {
                    if(vec_it->IsFloat())
                        ret.value[i] = vec_it->AsFloatUnchecked();
                    else if(vec_it->IsDouble())
                        ret.value[i] = vec_it->AsDoubleUnchecked();
                    else
                        return vec;

                    i++;
                }
                return ret;
            }
            else
            {
                return vec;
            }
        }

        OSSIA::Value operator()(const OSSIA::Destination& d) const { return d; }
        OSSIA::Value operator()(const OSSIA::Behavior& b) const { return b; }
        OSSIA::Value operator()(OSSIA::Tuple t)
        {
            int n = t.value.size();
            if(numArguments == n)
            {
                for(int i = 0; i < n; i++)
                {
                    auto res = eggs::variants::apply(*this, t.value[i].v);
                    t.value[i] = std::move(res);
                    ++cur_it;
                }
            }
            return std::move(t);
        }
};


inline OSSIA::Value filterValue(
        const OSSIA::net::Domain& dom,
        const OSSIA::Value& base_val,
        OSSIA::BoundingMode mode)
{
    if(dom)
    {
        auto res = OSSIA::net::clamp(dom, mode, base_val);
        if(res.valid())
            return res;
        else
            return {};
    }
    else
    {
        return base_val;
    }
}

inline OSSIA::Value filterValue(const impl::BasicAddress& addr)
{
    if(addr.getRepetitionFilter() == OSSIA::RepetitionFilter::ON &&
       addr.getValue() == addr.PreviousValue)
        return {};

    return filterValue(addr.getDomain(), addr.cloneValue(), addr.getBoundingMode());
}

inline boost::string_ref getOSCAddress(const OSSIA::net::Address& address)
{
    auto& addr = address.getTextualAddress();
    auto begin = addr.find(':') + 1;
    return boost::string_ref(addr.data() + begin, addr.size() - begin);
}

inline std::string getOSCAddressAsString(const OSSIA::net::Address& address)
{
    auto& addr = address.getTextualAddress();
    return addr.substr(addr.find(':') + 1);
}

inline std::string getOSCAddressAsString(const OSSIA::net::Node& node)
{
  auto addr = OSSIA::net::getAddressFromNode(node);
  return addr.substr(addr.find(':') + 1);
}

inline OSSIA::Value toValue(
        const OSSIA::Value& current,
        oscpack::ReceivedMessageArgumentIterator beg_it,
        oscpack::ReceivedMessageArgumentIterator end_it,
        int N)
{
    if(current.valid())
    {
        OSCInboundVisitor vis{beg_it, beg_it, end_it, N};
        return eggs::variants::apply(vis, current.v);
    }
    return {};
}

inline void updateValue(OSSIA::net::Address& addr,
                 oscpack::ReceivedMessageArgumentIterator beg_it,
                 oscpack::ReceivedMessageArgumentIterator end_it,
                 int N)
{
    if(addr.getAccessMode() == OSSIA::AccessMode::SET)
        return;

    auto res = filterValue(
                addr.getDomain(),
                impl::toValue(addr.cloneValue(), beg_it, end_it, N),
                addr.getBoundingMode());

    if(res.valid())
    {
        addr.setValue(std::move(res));
    }
}

inline void updateValue(OSSIA::net::Address& addr,
                 const oscpack::ReceivedMessage& mess)
{
    return updateValue(
                addr,
                mess.ArgumentsBegin(),
                mess.ArgumentsEnd(),
                mess.ArgumentCount());
}
}


namespace oscpack
{
inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::Value& val)
{
    using namespace eggs::variants;
    if(val.valid())
    {
        eggs::variants::apply(impl::OSCOutboundVisitor{p}, val.v);
    }

    return p;
}



inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::net::Domain& dom)
{
    using namespace impl;

    auto dom_min = OSSIA::net::min(dom);
    auto dom_max = OSSIA::net::max(dom);
    if(bool(dom_min.v) && bool(dom_max.v))
        p << dom_min << dom_max;

    return p;
}
}

