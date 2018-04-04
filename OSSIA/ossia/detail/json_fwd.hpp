#pragma once
#include <ossia/detail/config.hpp>

namespace rapidjson
{
class CrtAllocator;
template <typename Encoding, typename Allocator>
class GenericStringBuffer;
template<typename CharType>
struct UTF8;
using StringBuffer = GenericStringBuffer<UTF8<char>, CrtAllocator>;
}
