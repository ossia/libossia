#pragma once
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>
#include <ossia/network/v2/osc/detail/string_view.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/v2/base/address.hpp>
#include <ossia/network/v2/domain.hpp>

namespace oscpack
{
inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::Value& val);

struct OSCOutboundVisitor
{
        oscpack::OutboundPacketStream& p;
        void operator()(OSSIA::Impulse) const { }
        void operator()(OSSIA::Int i) const   { p << int32_t(i.value); }
        void operator()(OSSIA::Float f) const { p << f.value; }
        void operator()(OSSIA::Bool b) const  { p << int32_t(b.value); }
        void operator()(OSSIA::Char c) const  { p << int32_t(c.value); }
        void operator()(const OSSIA::String& str) const { p << impl::string_view(str.value); }
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
        const oscpack::ReceivedMessage& mess;
        oscpack::ReceivedMessageArgumentIterator it;

        OSSIA::Value operator()(OSSIA::Impulse imp) const { return imp; }
        OSSIA::Value operator()(OSSIA::Int i) const
        {
            switch(it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Int{it->AsInt32Unchecked()};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Int{int32_t(it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Int{int32_t(it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Int{int32_t(it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Int{int32_t(it->AsCharUnchecked())};
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
            switch(it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Float{float(it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Float{float(it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Float{float(it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Float{float(it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Float{float(it->AsCharUnchecked())};
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
            switch(it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Bool{bool(it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Bool{bool(it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Bool{bool(it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Bool{bool(it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Bool{bool(it->AsCharUnchecked())};
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
            switch(it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::Char{char(it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::Char{char(it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::Char{char(it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::Char{char(it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::Char{char(it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::Char{'T'};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::Char{'F'};
                case oscpack::STRING_TYPE_TAG:
                    return OSSIA::Char{it->AsStringUnchecked()[0]};
                case oscpack::SYMBOL_TYPE_TAG:
                    return OSSIA::Char{it->AsSymbolUnchecked()[0]};
                default:
                    return c;
            }
        }

        OSSIA::Value operator()(const OSSIA::String& str) const
        {
            switch(it->TypeTag())
            {
                case oscpack::INT32_TYPE_TAG:
                    return OSSIA::String{std::to_string(it->AsInt32Unchecked())};
                case oscpack::INT64_TYPE_TAG:
                    return OSSIA::String{std::to_string(it->AsInt64Unchecked())};
                case oscpack::FLOAT_TYPE_TAG:
                    return OSSIA::String{std::to_string(it->AsFloatUnchecked())};
                case oscpack::DOUBLE_TYPE_TAG:
                    return OSSIA::String{std::to_string(it->AsDoubleUnchecked())};
                case oscpack::CHAR_TYPE_TAG:
                    return OSSIA::String{std::to_string(it->AsCharUnchecked())};
                case oscpack::TRUE_TYPE_TAG:
                    return OSSIA::String{"true"};
                case oscpack::FALSE_TYPE_TAG:
                    return OSSIA::String{"false"};
                case oscpack::STRING_TYPE_TAG:
                    return OSSIA::String{it->AsStringUnchecked()};
                case oscpack::SYMBOL_TYPE_TAG:
                    return OSSIA::String{it->AsSymbolUnchecked()};
                default:
                    return str;
            }
        }

        template<int N>
        OSSIA::Value operator()(OSSIA::Vec<float, N> vec) const
        {
            if(mess.ArgumentCount() == N)
            {
                OSSIA::Vec<float, N> ret;
                int i = 0;
                auto vec_it = mess.ArgumentsBegin();
                auto vec_end = mess.ArgumentsEnd();
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
            if(mess.ArgumentCount() == n)
            {
                for(int i = 0; i < n; i++)
                {
                    auto res = eggs::variants::apply(*this, t.value[i].v);
                    t.value[i] = std::move(res);
                    ++it;
                }
            }
            return std::move(t);
        }
};


inline OSSIA::Value toValue(
        const oscpack::ReceivedMessage& m,
        const OSSIA::Value& current)
{
    if(current.valid())
        return eggs::variants::apply(OSCInboundVisitor{m, m.ArgumentsBegin()}, current.v);
    return {};
}
inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::Value& val)
{
    using namespace eggs::variants;
    if(val.valid())
    {
        eggs::variants::apply(OSCOutboundVisitor{p}, val.v);
    }

    return p;
}



inline oscpack::OutboundPacketStream& operator<<(
        oscpack::OutboundPacketStream& p,
        const OSSIA::v2::Domain& dom)
{
    using namespace impl;

    auto dom_min = OSSIA::v2::min(dom);
    auto dom_max = OSSIA::v2::max(dom);
    if(bool(dom_min.v) && bool(dom_max.v))
        p << dom_min << dom_max;

    return p;
}
}


