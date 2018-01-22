#pragma once

#include <memory>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <boost/container/flat_map.hpp>
#include <ossia_export.h>
#include <ossia/network/base/parameter.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/network/dataspace/dataspace_variant_visitors.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/detail/dataspace_convert.hpp>
#include <ossia/network/dataspace/detail/dataspace_merge.hpp>
#include <ossia/network/dataspace/detail/dataspace_parse.hpp>
#include <ossia/network/dataspace/detail/list_units.hpp>
#include <ossia/network/dataspace/detail/make_unit.hpp>
#include <ossia/network/dataspace/detail/make_value.hpp>
#include <ossia/network/dataspace/value_with_unit.hpp>
#include <ossia/network/value/detail/value_conversion_impl.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <boost/functional/hash.hpp>
#include <ossia/detail/hash_map.hpp>
namespace std
{
template<>
struct hash<std::pair<ossia::net::parameter_base*, ossia::unit_t>>
{
    struct vis
    {
        template<typename T>
        std::size_t operator()(const T& t)
        {
          return t.which();
        }

        std::size_t operator()()
        {
          return ossia::unit_variant::npos;
        }
    };
    std::size_t operator()(const std::pair<ossia::net::parameter_base*, ossia::unit_t>& k) const
    {
      std::size_t seed = 0;
      boost::hash_combine(seed, k.first);
      boost::hash_combine(seed, k.second.v.which());
      auto res = ossia::apply(vis{}, k.second.v);
      boost::hash_combine(seed, res);

      return seed;
    }
};
}
/**
 * \file flat_state.hpp
 */
namespace ossia
{
/**
 * @brief The flat_state class
 *
 * A state is a container of state_element.
 * Only non-null children will be added to the state.
 *
 * Launching the state launches all children.
 *
 * \see \ref state_element
 */
class OSSIA_EXPORT flat_set_state
{
  public:
    OSSIA_EXPORT friend bool operator==(const state& lhs, const state& rhs);
    OSSIA_EXPORT friend bool operator!=(const state& lhs, const state& rhs);

    auto begin()
    {
      return m_children.begin();
    }
    auto end()
    {
      return m_children.end();
    }
    auto begin() const
    {
      return m_children.begin();
    }
    auto end() const
    {
      return m_children.end();
    }
    auto cbegin() const
    {
      return m_children.cbegin();
    }
    auto cend() const
    {
      return m_children.cend();
    }

    auto& children() const
    {
      return m_children;
    }

    std::size_t size() const
    {
      return m_children.size();
    }

    bool empty() const
    {
      return m_children.empty();
    }

    void launch()
    {
      for (auto& state : m_children)
      {
        ossia::apply(state_execution_visitor{}, std::move(state.second));
      }
    }

    void add(const ossia::message& e)
    {
      // TODO push in vector instead... or boost::container::flat_multimap
      // But normally there should be a single one with a given address / unit pair... do an assert ?

      m_children[std::make_pair(&e.dest.value.get(), e.get_unit())] = e;
    }
    void add(ossia::message&& e)
    {
      // TODO push in vector instead... or boost::container::flat_multimap
      m_children[std::make_pair(&e.dest.value.get(), e.get_unit())] = std::move(e);

    }
    void add(const ossia::piecewise_message& e)
    {
      // TODO push in vector instead... or boost::container::flat_multimap
      m_children[std::make_pair(&e.address.get(), e.get_unit())] = e;

    }
    template <std::size_t N>
    void add(piecewise_vec_message<N>&& e)
    {
      // TODO push in vector instead... or boost::container::flat_multimap
      m_children[std::make_pair(&e.address.get(), e.get_unit())] = std::move(e);

    }
    template <std::size_t N>
    void add(const piecewise_vec_message<N>& e)
    {
      // TODO push in vector instead... or boost::container::flat_multimap
      m_children[std::make_pair(&e.address.get(), e.get_unit())] = e;
    }

    void add(const ossia::state_element& e)
    {
      switch(e.which())
      {
        case 0:
          return add(*static_cast<const ossia::message*>(e.target()));
        case 2:
          return add(*static_cast<const ossia::piecewise_message*>(e.target()));
        case 3:
          return add(*static_cast<const ossia::piecewise_vec_message<2>*>(e.target()));
        case 4:
          return add(*static_cast<const ossia::piecewise_vec_message<3>*>(e.target()));
        case 5:
          return add(*static_cast<const ossia::piecewise_vec_message<4>*>(e.target()));
        case 1:
        default:
          throw std::runtime_error("there shouldn't be a state here");
          break;
      }
    }
    void add(ossia::state_element&& e)
    {
      switch(e.which())
      {
        case 0:
          return add(std::move(*static_cast<ossia::message*>(e.target())));
        case 2:
          return add(std::move(*static_cast<ossia::piecewise_message*>(e.target())));
        case 3:
          return add(std::move(*static_cast<ossia::piecewise_vec_message<2>*>(e.target())));
        case 4:
          return add(std::move(*static_cast<ossia::piecewise_vec_message<3>*>(e.target())));
        case 5:
          return add(std::move(*static_cast<ossia::piecewise_vec_message<4>*>(e.target())));
        case 1:
        default:
          throw std::runtime_error("there shouldn't be a state here");
          break;
      }
    }


    auto find(const message& e)
    {
      return m_children.find({&e.dest.value.get(), e.get_unit()});
    }
    auto find(const piecewise_message& e)
    {
      return m_children.find({&e.address.get(), e.get_unit()});
    }
    template <std::size_t N>
    auto find(const piecewise_vec_message<N>& e)
    {
      return m_children.find({&e.address.get(), e.get_unit()});
    }

    void remove(const ossia::message& e)
    {
      m_children.erase(std::make_pair(&e.dest.value.get(), e.get_unit()));
    }

    void remove(const ossia::piecewise_message& e)
    {
      m_children.erase(std::make_pair(&e.address.get(), e.get_unit()));
    }


    using map_t = ossia::fast_hash_map<
      std::pair<
        ossia::net::parameter_base*,
        ossia::unit_t>,
      state_element>;
    using iterator = map_t::iterator;
    using const_iterator = map_t::const_iterator;
    void remove(iterator it)
    {
      m_children.erase(it);
    }
    void remove(const_iterator it)
    {
      m_children.erase(it);
    }

    template <std::size_t N>
    void remove(const ossia::piecewise_vec_message<N>& e)
    {
      m_children.erase(std::make_pair(&e.address.get(), e.get_unit()));
    }

    void reserve(std::size_t n)
    {
      m_children.reserve(n);
    }
    void clear()
    {
      m_children.clear();
    }

  private:
    map_t m_children;
};



struct flat_vec_state
{
    using vec_type = ossia::small_vector<ossia::state_element, 16>;
    using iterator = typename vec_type::iterator;
    using const_iterator = typename vec_type::const_iterator;
    ossia::small_vector<ossia::state_element, 16> m_children;
    void add(const ossia::state_element& other)
    { m_children.push_back(other); }
    void add(ossia::state_element&& other)
    { m_children.push_back(std::move(other)); }

    void remove(const_iterator other)
    { m_children.erase(other); }

    template<typename T>
    auto find(const T& val)
    { return ossia::find(m_children, val); }

    void launch()
    {
      for (auto& state : m_children)
      {
        ossia::apply(state_execution_visitor{}, std::move(state));
      }
    }

    auto reserve(std::size_t n) { return m_children.reserve(n); }
    auto clear() { return m_children.clear(); }
    auto begin() { return m_children.begin(); }
    auto end() { return m_children.end(); }
    auto begin() const { return m_children.begin(); }
    auto end() const { return m_children.end(); }
};

struct mono_state
{
    ossia::state_element e;
    void add(const ossia::state_element& other)
    { e = other; }
    void add(ossia::state_element&& other)
    { e = std::move(other); }
    template<typename T>
    void remove(const T& other)
    {
    }

    template<typename T>
    auto find(const T&)
    {
      return e ? &e : nullptr;
    }

    auto end() const { return nullptr; }
};

}
