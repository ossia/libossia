#include <ossia/detail/string_view.hpp>
void f()
{
std::string s{ossia::string_view{}};
s+= ossia::string_view{};
}
