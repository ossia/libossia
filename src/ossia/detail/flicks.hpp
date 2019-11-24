#pragma once
namespace ossia
{
template<typename T>
static const constexpr T flicks_per_second = 705600000;
template<typename T>
static const constexpr T flicks_per_millisecond = flicks_per_second<T> / 1000;
template<typename T>
static const constexpr T quarter_duration = flicks_per_second<T> / 2;

static const constexpr double root_tempo = 120.;
}
