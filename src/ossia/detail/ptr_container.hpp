#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>

#include <memory>
#include <vector>

/**
 * \file ptr_container.hpp
 */
namespace ossia
{
/**
 * \typedef ptr_container is a shortcut for a vector of shared_ptr.
 *
 * It is not related to boost::ptr_container.
 */
template <typename T>
using ptr_container
    = std::vector<std::shared_ptr<T>, std::allocator<std::shared_ptr<T>>>;
template <typename T, std::size_t N>
using small_ptr_container = ossia::small_vector<std::shared_ptr<T>, N>;
}
