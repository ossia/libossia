#include "qml_cond.hpp"
#include <ossia-qt/score/qml_sync.hpp>

namespace ossia
{
namespace qt
{

qml_cond::qml_cond(QQuickItem* parent):
  QQuickItem{parent}
{
  connect(this, &QQuickItem::parentChanged,
          this, &qml_cond::reset);
  reset();
}

qml_cond::~qml_cond()
{

}

QQmlScriptString qml_cond::expr() const
{
  return m_expr;
}

qml_sync* qml_cond::sync() const
{
  return m_sync;
}

void qml_cond::setup()
{
  if(!m_sync)
    return;
  auto ts = m_sync->sync();
  if(!ts)
    return;

  m_impl = std::make_shared<ossia::time_event>(
             [] (ossia::time_event::status) {},
             *ts,
             ossia::expressions::make_expression_true());
}

void qml_cond::setExpr(QQmlScriptString expr)
{
  if (m_expr == expr)
    return;

  m_expr = expr;
  emit exprChanged(m_expr);
}

void qml_cond::setSync(qml_sync* sync)
{
  if (m_sync == sync)
    return;

  m_sync = sync;
  emit syncChanged(m_sync);
}

void qml_cond::reset()
{

}

}
}
