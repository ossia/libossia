#pragma once
#include <Editor/Value/ValueBase.h>
#include <Editor/Value/ValueTraits.h>
#include <array>

namespace OSSIA
{

template<typename T, int N>
class Vec : public Value
{
    public:
        std::array<T, N> value;

        Vec():
            Value{ValueTrait<Vec>::ossia_enum}
        {
        }

        Vec(std::array<T, N> v) :
            Value{ValueTrait<Vec>::ossia_enum},
            value(std::move(v))
        {
        }

        virtual ~Vec() = default;
        Vec(const Vec&) = default;
        Vec(Vec&&) = default;
        Vec& operator=(const Vec&) = default;
        Vec& operator=(Vec&&) = default;

        /*! clone */
        Value * clone() const override
        {
            return new Vec{value};
        }

        /*! equal operator */
        bool operator== (const Value&) const override;

        /*! different operator */
        bool operator!= (const Value&) const override;

        /*! greater than operator */
        bool operator> (const Value&) const override;

        /*! greater than and equal operator */
        bool operator>= (const Value&) const override;

        /*! less than operator */
        bool operator< (const Value&) const override;

        /*! less than and equal operator */
        bool operator<= (const Value&) const override;
};

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

}
