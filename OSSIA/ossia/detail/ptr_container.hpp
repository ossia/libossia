#pragma once

#include <vector>
#include <memory>

namespace ossia
{
template<typename T> using ptr_container =
  std::vector<
    std::shared_ptr<T>,
    std::allocator<std::shared_ptr<T>>>;
}
