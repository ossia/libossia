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
  Q_PROPERTY(qml_cond* startCond READ startCond WRITE setStartCond NOTIFY startCondChanged)
  Q_PROPERTY(qml_cond* endCond READ endCond WRITE setEndCond NOTIFY endCondChanged)
  Q_PROPERTY(qml_sync* startSync READ startSync WRITE setStartSync NOTIFY startSyncChanged)
  Q_PROPERTY(qml_sync* endSync READ endSync WRITE setEndSync NOTIFY endSyncChanged)

public:
  qml_loop(QQuickItem* parent = nullptr);
  ~qml_loop() override;

  qml_interval* interval() const;
  qml_cond* startCond() const;
  qml_cond* endCond() const;
  qml_sync* startSync() const;
  qml_sync* endSync() const;

  void setup() override { }
  std::shared_ptr<ossia::time_process> process() const override;

public slots:
  void setInterval(qml_interval* interval);
  void setStartCond(qml_cond* startCond);
  void setEndCond(qml_cond* endCond);
  void setStartSync(qml_sync* startSync);
  void setEndSync(qml_sync* endSync);

signals:
  void intervalChanged(qml_interval* interval);
  void startCondChanged(qml_cond* startCond);
  void endCondChanged(qml_cond* endCond);
  void startSyncChanged(qml_sync* startSync);
  void endSyncChanged(qml_sync* endSync);

private:
  void reset_impl() override;
  std::shared_ptr<ossia::loop> m_impl;
  qml_interval* m_interval;
  qml_cond* m_startCond;
  qml_cond* m_endCond;
  qml_sync* m_startSync;
  qml_sync* m_endSync;
};
}
}
