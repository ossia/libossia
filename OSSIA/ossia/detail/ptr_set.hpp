#pragma once
#include <hopscotch_set.h>
#include <ossia/detail/math.hpp>
#include <type_traits>
namespace ossia
{

// https://stackoverflow.com/q/20953390/1495627
template<typename T>
struct EgurHash
{
    static const constexpr size_t shift = constexpr_log2(1 + sizeof(T));

    size_t operator()(const T* val) const
    {
      return (size_t)(val) >> shift;
    }
};

template<typename T>
using ptr_set = tsl::hopscotch_set<T, EgurHash<std::remove_pointer_t<T>>>;

}
