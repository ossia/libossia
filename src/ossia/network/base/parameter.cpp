// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/value_with_unit.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>

#include <future>
#include <ostream>
/*
template class ossia::callback_container<ossia::value_callback>;
*/

namespace ossia::net
{
parameter_base::~parameter_base() = default;

std::future<void> parameter_base::pull_value_async()
{
  return {};
}

ossia::net::protocol_base& parameter_base::get_protocol() const noexcept
{
  return m_node.get_device().get_protocol();
}

void parameter_base::request_value() { }

value parameter_base::value(destination_index idx) const
{
  return get_value_at_index(value(), idx);
}

std::vector<ossia::value>
parameter_base::value(const std::vector<destination_index>& indices) const
{
  std::vector<ossia::value> t;
  t.reserve(indices.size());

  auto v = value();
  for(auto idx : indices)
  {
    t.push_back(get_value_at_index(v, idx));
  }

  return t;
}

std::optional<ossia::value> parameter_base::get_default_value() const noexcept
{
  return ossia::net::get_default_value(get_node());
}

void parameter_base::set_default_value(const ossia::value& v)
{
  ossia::net::set_default_value(get_node(), std::move(v));
}

value parameter_base::fetch_value()
{
  pull_value();
  return value();
}

ossia::repetition_filter parameter_base::get_repetition_filter() const noexcept
{
  return m_repetitionFilter;
}

ossia::net::parameter_base&
parameter_base::set_repetition_filter(ossia::repetition_filter repetitionFilter)
{
  if(m_repetitionFilter != repetitionFilter)
  {
    m_repetitionFilter = repetitionFilter;
    m_node.get_device().on_attribute_modified(
        m_node, std::string(text_repetition_filter()));
  }
  return *this;
}

parameter_base& parameter_base::set_unit(const unit_t& u)
{
  if(m_unit != u)
  {
    m_unit = u;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_unit()));
  }
  return *this;
}

bool parameter_base::get_disabled() const noexcept
{
  return m_disabled;
}

parameter_base& parameter_base::set_disabled(bool v)
{
  if(m_disabled != v)
  {
    m_disabled = v;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_disabled()));
  }
  return *this;
}

bool parameter_base::get_muted() const noexcept
{
  return m_muted;
}

parameter_base& parameter_base::set_muted(bool v)
{
  if(m_muted != v)
  {
    m_muted = v;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_muted()));
  }
  return *this;
}

bool parameter_base::get_critical() const noexcept
{
  return m_critical;
}

parameter_base& parameter_base::set_critical(bool v)
{
  if(m_critical != v)
  {
    m_critical = v;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_critical()));
  }
  return *this;
}

parameter_type parameter_base::get_type() const noexcept
{
  return m_type;
}

value_with_unit get_value(const ossia::destination& d)
{
  ossia::net::parameter_base& addr = d.value.get();

  return make_value(addr.value(d.index), addr.get_unit());
}

void push_value(const destination& d, const value_with_unit& v)
{
  ossia::net::parameter_base& addr = d.value.get();
  addr.push_value(ossia::to_value(v)); // TODO what about destination_index ??
}

std::ostream& operator<<(std::ostream& s, const ossia::net::parameter_base& addr)
{
  return s << ossia::net::address_string_from_node(addr);
}

bool parameter_alphabetical_sort::operator()(
    const parameter_base* lhs, const parameter_base* rhs) const noexcept
{
  return lhs->get_node().osc_address() < rhs->get_node().osc_address();
}
}
