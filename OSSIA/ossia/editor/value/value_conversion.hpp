#pragma once
#include <ossia/editor/value/value.hpp>

namespace ossia
{


template<typename Target>
struct value_converter
{

};

template<typename T>
T convert(const ossia::value& val);

template<typename T>
struct numeric_value_converter
{
    T operator()(Impulse) const { return T{}; }
    T operator()(Int v) { return v.value; }
    T operator()(Float v) { return v.value; }
    T operator()(Bool v) { return v.value; }
    T operator()(Char v) { return v.value; }
    T operator()() const { return T{}; }

    T operator()(const Behavior&) const { return T{}; }
    T operator()(const Destination&) const { return T{}; }
    T operator()(const String& v) const { return T{}; }
    T operator()(Vec2f v) const { return v.value[0]; }
    T operator()(Vec3f v) const { return v.value[0]; }
    T operator()(Vec4f v) const { return v.value[0]; }

    T operator()(const Tuple& v) const
    { return !v.value.empty() ? convert<T>(v.value[0]) : T{}; }
};


template<>
struct value_converter<int> : public numeric_value_converter<int>
{
};
template<>
struct value_converter<float> : public numeric_value_converter<float>
{
};
template<>
struct value_converter<double> : public numeric_value_converter<double>
{
};
template<>
struct value_converter<bool> : public numeric_value_converter<bool>
{
};
template<>
struct value_converter<char> : public numeric_value_converter<char>
{
};

template<>
struct value_converter<std::string>
{
    using T = std::string;
    T operator()(Impulse) const { return T{}; }
    T operator()(Int v) { return std::to_string(v.value); }
    T operator()(Float v) { return std::to_string(v.value); }
    T operator()(Bool v) { return std::to_string(v.value); }
    T operator()(Char v) { return std::to_string(v.value); }
    T operator()() const { return T{}; }

    T operator()(const Behavior&) const { return T{}; }
    T operator()(const Destination&) const { return T{}; }
    T operator()(const String& v) const { return T{}; }

    template<int N>
    T operator()(Vec<float, N> v) const
    {
        std::string s = "[";
        s += std::to_string(v.value[0]);
        for(int i = 1; i < N; i++)
            s += ", " + std::to_string(v.value[i]);
        s += "]";
        return s;
    }

    T operator()(const Tuple& v) const
    {
        std::string s = "[";
        s += convert<std::string>(v.value[0]);

        std::size_t n = v.value.size();
        for(std::size_t i = 1; i < n; i++)
            s += ", " + convert<std::string>(v.value[i]);
        s += "]";
        return s;
    }
};


template<typename T>
T convert(const ossia::value& val)
{
    return val.apply(value_converter<T>{});
}

}
