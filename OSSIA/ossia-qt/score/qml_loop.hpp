#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <wobjectdefs.h>
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
  W_OBJECT(qml_loop)

  
    
    
    
    
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
public:
  void setInterval(qml_interval* interval); W_SLOT(setInterval);

  void setStartCond(QQmlScriptString s); W_SLOT(setStartCond);
  void setEndCond(QQmlScriptString s); W_SLOT(setEndCond);
  void setStartTrig(QQmlScriptString s); W_SLOT(setStartTrig);
  void setEndTrig(QQmlScriptString s); W_SLOT(setEndTrig);

public:
  void intervalChanged(qml_interval* interval) E_SIGNAL(OSSIA_EXPORT, intervalChanged, interval);
  void startTrigChanged(QQmlScriptString arg_1) E_SIGNAL(OSSIA_EXPORT, startTrigChanged, arg_1);
  void endTrigChanged(QQmlScriptString arg_1) E_SIGNAL(OSSIA_EXPORT, endTrigChanged, arg_1);
  void startCondChanged(QQmlScriptString arg_1) E_SIGNAL(OSSIA_EXPORT, startCondChanged, arg_1);
  void endCondChanged(QQmlScriptString arg_1) E_SIGNAL(OSSIA_EXPORT, endCondChanged, arg_1);

private:
  void reset_impl() override;
  std::shared_ptr<ossia::loop> m_impl;
  qml_interval* m_interval{};
  QQmlScriptString m_startCond, m_startTrig, m_endCond, m_endTrig;

W_PROPERTY(QQmlScriptString, endCond READ endCond WRITE setStartCond NOTIFY endCondChanged)

W_PROPERTY(QQmlScriptString, startCond READ startCond WRITE setStartCond NOTIFY startCondChanged)

W_PROPERTY(QQmlScriptString, endTrig READ endTrig WRITE setStartTrig NOTIFY endTrigChanged)

W_PROPERTY(QQmlScriptString, startTrig READ startTrig WRITE setStartTrig NOTIFY startTrigChanged)

W_PROPERTY(qml_interval*, interval READ interval WRITE setInterval NOTIFY intervalChanged)
};
}
}
