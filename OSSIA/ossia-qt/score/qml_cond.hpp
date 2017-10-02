#pragma once
#include <QQuickItem>
#include <QQmlExpression>
#include <QQmlScriptString>
#include <QQmlListProperty>
#include <ossia/editor/scenario/time_event.hpp>
namespace ossia
{
namespace qt
{
class qml_interval;
class qml_sync;
class qml_state;
class qml_cond : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expr READ expr WRITE setExpr NOTIFY exprChanged)
    Q_PROPERTY(qml_sync* sync READ sync WRITE setSync NOTIFY syncChanged)
  public:
    qml_cond(QQuickItem* parent = nullptr);

    ~qml_cond() override;

    QQmlScriptString expr() const;
    qml_sync* sync() const;


    void setup();

    void registerState(qml_state*);
    void unregisterState(qml_state*);

    std::shared_ptr<ossia::time_event> cond() const { return m_impl; }
public slots:
    void setExpr(QQmlScriptString expr);
    void setSync(qml_sync* sync);

signals:
    void exprChanged(QQmlScriptString expr);
    void syncChanged(qml_sync* sync);

private:
    void reset();
    QQmlScriptString m_expr;
    std::shared_ptr<ossia::time_event> m_impl;
    qml_sync* m_sync{};
};
}
}
