#pragma once
#include <QQuickItem>
#include <QQmlExpression>
#include <QQmlScriptString>
#include <QQmlListProperty>
#include <ossia-qt/score/qml_cond.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
namespace ossia
{
namespace qt
{
class qml_cond;
class qml_sync : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString expr READ expr WRITE setExpr NOTIFY exprChanged)

  public:
    qml_sync(QQuickItem* parent = nullptr);
    ~qml_sync() override;

    QQmlScriptString expr() const;
    qml_cond* defaultCond();

    void registerCond(qml_cond*);
    void unregisterCond(qml_cond*);

    void setup();
    std::shared_ptr<ossia::time_sync> sync() const { return m_impl; }
    void setSync(std::shared_ptr<ossia::time_sync> s) { m_impl = s; }
  public Q_SLOTS:
    void setExpr(QQmlScriptString expr);

  signals:
    void exprChanged(QQmlScriptString expr);

  private:
    void reset();
    QQmlScriptString m_expr;
    std::shared_ptr<ossia::time_sync> m_impl;
    qml_cond m_default;
    tsl::hopscotch_set<qml_cond*> m_conds;
};
}
}
