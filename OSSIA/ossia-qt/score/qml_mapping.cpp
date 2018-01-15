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

std::shared_ptr<time_process> qml_mapping::process() const
{
  return {};
  /* TODO
  return m_impl;
  */
}

void qml_mapping::reset_impl()
{

}

}
}
