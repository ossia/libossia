#pragma once

#include <ossia/network/base/node_functions.hpp>

#include <ossia-qt/js_utilities.hpp>

#include <QQmlContext>
#include <QQmlEngine>

namespace ossia::net
{
class qml_engine_functions : public QObject
{
  W_OBJECT(qml_engine_functions)
public:
  device_base* m_dev{};

  explicit qml_engine_functions(QObject* parent);

  void addNode(QString address, QString type);
  W_SLOT(addNode)

  void removeNode(QString address, QString type);
  W_SLOT(removeNode)
};
}
