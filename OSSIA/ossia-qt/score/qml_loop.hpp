#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <ossia-qt/score/qml_interval.hpp>
#include <ossia-qt/score/qml_cond.hpp>
#include <ossia-qt/score/qml_sync.hpp>

#include <ossia/editor/loop/loop.hpp>

namespace ossia
{
namespace qt
{
class qml_loop : public qml_process
{
  Q_OBJECT

  Q_PROPERTY(qml_interval* interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(QQmlScriptString startTrig READ startTrig WRITE setStartTrig NOTIFY startTrigChanged)
    Q_PROPERTY(QQmlScriptString endTrig READ endTrig WRITE setStartTrig NOTIFY endTrigChanged)
    Q_PROPERTY(QQmlScriptString startCond READ startCond WRITE setStartCond NOTIFY startCondChanged)
    Q_PROPERTY(QQmlScriptString endCond READ endCond WRITE setStartCond NOTIFY endCondChanged)
public:
  qml_loop(QQuickItem* parent = nullptr);
  ~qml_loop() override;

  qml_interval* interval() const;
  void setup() override;
  std::shared_ptr<ossia::time_process> process() const override;

  QQmlScriptString startCond() const;
  QQmlScriptString endCond() const;
  QQmlScriptString startTrig() const;
  QQmlScriptString endTrig() const;
public slots:
  void setInterval(qml_interval* interval);

  void setStartCond(QQmlScriptString s);
  void setEndCond(QQmlScriptString s);
  void setStartTrig(QQmlScriptString s);
  void setEndTrig(QQmlScriptString s);

signals:
  void intervalChanged(qml_interval* interval);
  void startTrigChanged(QQmlScriptString);
  void endTrigChanged(QQmlScriptString);
  void startCondChanged(QQmlScriptString);
  void endCondChanged(QQmlScriptString);

private:
  void reset_impl() override;
  std::shared_ptr<ossia::loop> m_impl;
  qml_interval* m_interval{};
  QQmlScriptString m_startCond, m_startTrig, m_endCond, m_endTrig;
};
}
}
