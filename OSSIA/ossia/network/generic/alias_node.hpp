#pragma once
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{

/**
 * @brief A type of node that refers to another node
 *
 * Get and set the source with get_origin, set_origin.
 */
struct alias_node final : public generic_node_base
{
public:
  using generic_node_base::generic_node_base;

  ~alias_node();

  ossia::net::node_base* get_origin() const;
  void set_origin(ossia::net::node_base* o);

  parameter_base* create_address(val_type v) override;
  bool remove_address() override;
  parameter_base* get_address() const override;

protected:
  std::unique_ptr<node_base> make_child(const std::string& name) override;
  void removing_child(node_base&) override;

private:
  void on_deletion(const ossia::net::node_base& orig);

  ossia::net::node_base* m_origin{};
};

struct alias_path final :
    public generic_node_base,
    public ossia::net::parameter_base
{
public:
  using generic_node_base::generic_node_base;

  ~alias_path();

  const std::vector<ossia::net::node_base*>& get_roots()
  {
    return m_roots;
  }
  void set_roots(std::vector<ossia::net::node_base*> roots)
  {
    m_roots = std::move(roots);
    // TODO connect on deletion
  }

  traversal::path get_path() const;
  void set_path(traversal::path o);

private:
  parameter_base* create_address(val_type v) override { return this; }
  bool remove_address() override { return false; }
  parameter_base* get_address() const override { return (ossia::net::parameter_base*)(this); }

  std::unique_ptr<node_base> make_child(const std::string& name) override { return {}; }
  void removing_child(node_base&) override { }

  node_base& get_node() const override
  {
    return *(ossia::net::node_base*)this;
  }

  void pull_value() override
  {
  }

  std::future<void> pull_value_async() override
  {
    return {};
  }
  void request_value() override
  {
  }
  ossia::value value() const override
  {
    return {};
  }
  parameter_base&push_value(const ossia::value&) override
  {
    return *this;
  }
  parameter_base&push_value(ossia::value&&) override
  {
    return *this;
  }
  parameter_base&push_value() override
  {
    return *this;
  }
  parameter_base&set_value(const ossia::value&) override
  {
    return *this;
  }
  parameter_base&set_value(ossia::value&&) override
  {
    return *this;
  }
  void set_value_quiet(const ossia::value& v) override
  {
  }
  void set_value_quiet(ossia::value&& v) override
  {
  }
  val_type get_value_type() const override
  {
    return {};
  }
  parameter_base&set_value_type(val_type) override
  {
    return *this;
  }
  access_mode get_access() const override
  {
    return {};
  }
  parameter_base&set_access(access_mode) override
  {
    return *this;
  }
  const domain&get_domain() const override
  {
    static ossia::domain d;
    return d;
  }
  parameter_base&set_domain(const domain&) override
  {
    return *this;
  }
  bounding_mode get_bounding() const override
  {
    return {};
  }
  parameter_base&set_bounding(bounding_mode) override
  {
    return *this;
  }
  repetition_filter get_repetition_filter() const override
  {
    return {};
  }
  parameter_base&set_repetition_filter(repetition_filter) override
  {
    return *this;
  }
  bool filter_repetition(const ossia::value& val) const override
  {
    return {};
  }
  unit_t get_unit() const override
  {
    return {};
  }
  parameter_base&set_unit(const unit_t& v) override
  {
    return *this;
  }
  bool get_muted() const override
  {
    return {};
  }
  parameter_base&set_muted(bool) override
  {
    return *this;
  }
  bool get_critical() const override
  {
    return {};
  }
  parameter_base&set_critical(bool) override
  {
    return *this;
  }


  std::vector<ossia::net::node_base*> m_roots;
  traversal::path m_path;
};
}
}
