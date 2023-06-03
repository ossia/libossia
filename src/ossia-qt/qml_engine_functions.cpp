#include "qml_engine_functions.hpp"

#include <wobjectimpl.h>

W_OBJECT_IMPL(ossia::net::qml_engine_functions)
namespace ossia::net
{

qml_engine_functions::qml_engine_functions(QObject* parent)
    : QObject{parent}
{
}

void qml_engine_functions::addNode(QString address, QString type)
{
  if(!m_dev)
    return;

  auto n = address.toStdString();
  ossia::net::find_or_create_parameter(
      m_dev->get_root_node(), address.toStdString(), type.toStdString());
}

void qml_engine_functions::removeNode(QString address, QString type)
{
  if(!m_dev)
    return;

  if(auto res = ossia::net::find_node(m_dev->get_root_node(), address.toStdString()))
    if(auto p = res->get_parent())
      p->remove_child(*res);
}

}
