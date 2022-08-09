#pragma once
#include <ossia/editor/state/state_element.hpp>

#include <QQmlExpression>
#include <QQmlListProperty>
#include <QQmlScriptString>
#include <QQuickItem>

#include <verdigris>
namespace ossia
{
namespace qt
{
class qml_state : public QQuickItem
{
  W_OBJECT(qml_state)

public:
  qml_state(QQuickItem* parent = nullptr);
  ~qml_state() override;

private:
  void reset();
  ossia::state m_impl;
};

}
}
