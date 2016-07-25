#pragma once
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscReceivedElements.h>
#include <OSC/string_view.hpp>
#include <Editor/Value/Value.h>
#include <Editor/Domain.h>

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
            if(it->IsInt32())
                return OSSIA::Int{it->AsInt32Unchecked()};
            else if(it->IsInt64())
                return OSSIA::Int{it->AsInt64Unchecked()};
            else
                return i;
        }
        OSSIA::Value operator()(OSSIA::Float f) const
        {
            if(it->IsFloat())
                return OSSIA::Float{it->AsFloatUnchecked()};
            else if(it->IsDouble())
                return OSSIA::Float{it->AsDoubleUnchecked()};
            else
                return f;
        }

        OSSIA::Value operator()(OSSIA::Bool b) const
        {
            if(it->IsBool())
                return OSSIA::Bool{it->AsBoolUnchecked()};
            else
                return b;
        }
        OSSIA::Value operator()(OSSIA::Char c) const
        {
            if(it->IsChar())
                return OSSIA::Char{it->AsCharUnchecked()};
            else if(it->IsString())
                return OSSIA::Char{it->AsStringUnchecked()[0]};
            else
                return c;
        }

        OSSIA::Value operator()(const OSSIA::String& str) const
        {
            if(it->IsString())
                return OSSIA::String{it->AsStringUnchecked()};
            else
                return str;
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
        const OSSIA::Domain& dom)
{
    using namespace impl;

    p << dom.getMin() << dom.getMax();

    return p;
}
}


