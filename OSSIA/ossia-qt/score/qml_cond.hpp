#pragma once
#include <QQuickItem>
#include <QQmlExpression>
#include <QQmlScriptString>
#include <QQmlListProperty>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/expression/expression_generic.hpp>
namespace ossia
{
namespace qt
{
class qml_interval;
class qml_sync;
class qml_state;
class qml_expr final : public ossia::expressions::expression_generic_base
{
  public:
    qml_expr(QQmlExpression* e): m_expr{e} { }

    void update() override
    {
    }
    bool evaluate() const override
    {
      return m_expr->evaluate().toBool();
    }
    void on_first_callback_added(expressions::expression_generic&) override
    {
    }
    void on_removing_last_callback(expressions::expression_generic&) override
    {
    }

  private:
    std::unique_ptr<QQmlExpression> m_expr;
};


ossia::expression_ptr make_expression(const QQmlScriptString& script, QObject* obj);
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
    void setCond(std::shared_ptr<ossia::time_event> c) { m_impl = c; }
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
