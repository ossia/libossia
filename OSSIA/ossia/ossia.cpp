// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// C++17 tests
#include <type_traits>
namespace ossia::net
{

template<typename T>
int foo()
{
  if constexpr(std::is_pointer_v<T>) { return 0;} 
  return 1;
}

}
