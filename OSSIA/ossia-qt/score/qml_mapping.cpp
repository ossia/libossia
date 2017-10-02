#include "qml_mapping.hpp"
namespace ossia
{
namespace qt
{

qml_mapping::qml_mapping(QQuickItem* parent)
  : qml_process{parent}
{
  reset();
}

qml_mapping::~qml_mapping()
{

}

void qml_mapping::reset_impl()
{

}

}
}
