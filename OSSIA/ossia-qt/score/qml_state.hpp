#pragma once
#include <QQuickItem>
#include <QQmlExpression>
#include <QQmlScriptString>
#include <QQmlListProperty>
#include <ossia/editor/state/state_element.hpp>
namespace ossia
{
namespace qt
{
class qml_state : public QQuickItem
{
    Q_OBJECT

  public:
    qml_state(QQuickItem* parent = nullptr);
    ~qml_state() override;

  private:
    void reset();
    ossia::state m_impl;
};

}
}
