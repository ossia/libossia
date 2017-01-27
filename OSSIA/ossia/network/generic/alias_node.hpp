#pragma once
#include <ossia/network/generic/generic_node.hpp>

namespace ossia
{
namespace net
{

/**
 * @brief A type of node that refers to another node
 *
 * Get and set the source with getOrigin, setOrigin.
 */
struct alias_node final : public generic_node_base
{
public:
  using generic_node_base::generic_node_base;

  ~alias_node();

  ossia::net::node_base* getOrigin() const;
  void setOrigin(ossia::net::node_base* o);

  address_base* createAddress(val_type v) override;
  bool removeAddress() override;
  address_base* getAddress() const override;

protected:
  std::unique_ptr<node_base> makeChild(const std::string& name) override;
  void removingChild(node_base& ) override;

private:
  void onDeletion(const ossia::net::node_base& orig);

  ossia::net::node_base* m_origin{};
};

}
}
