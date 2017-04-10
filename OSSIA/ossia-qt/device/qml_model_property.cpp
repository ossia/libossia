#include "qml_model_property.hpp"

namespace ossia
{
namespace qt
{

qml_model_property::qml_model_property(QObject* parent):
  QAbstractItemModel{parent}
{
}


qml_model_property::~qml_model_property()
{

}

void qml_model_property::on_node_deleted(const net::node_base&)
{

}

}
}
