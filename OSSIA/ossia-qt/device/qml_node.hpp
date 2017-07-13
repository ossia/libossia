#pragma once
#include <ossia-qt/device/qml_node_base.hpp>

namespace ossia
{
namespace qt
{
class qml_node : public qml_node_base
{
  Q_OBJECT
public:
  qml_node(QQuickItem* parent = nullptr);
  ~qml_node();

  void resetNode() override;

public slots:
  void reset_parent();

private:
  void on_node_deleted(const ossia::net::node_base&);
};
}
}
