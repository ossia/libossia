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
  using qml_node_base::qml_node_base;

private:
   void resetNode() override;
};
}
}
