#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <wobjectdefs.h>
#include <ossia-qt/score/qml_sync.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <tsl/hopscotch_set.h>

namespace ossia
{
namespace qt
{
class qml_interval;
class qml_sync;
class qml_scenario : public qml_process
{
    W_OBJECT(qml_scenario)

    
  public:
    qml_scenario(QQuickItem* parent = nullptr);
    ~qml_scenario() override;

    void registerInterval(qml_interval*);
    void unregisterInterval(qml_interval*);

    void registerSync(qml_sync*);
    void unregisterSync(qml_sync*);

    void setup() override;
    std::shared_ptr<ossia::time_process> process() const override;

    qml_sync* startSync() const;
  public:
    void setStartSync(qml_sync* s); W_SLOT(setStartSync);

  public:
    void startSyncChanged(qml_sync* arg_1) E_SIGNAL(OSSIA_EXPORT, startSyncChanged, arg_1);

private:
    void reset_impl() override;

    qml_sync* m_startSync{};
    std::shared_ptr<ossia::scenario> m_impl;

    tsl::hopscotch_set<qml_sync*> m_syncs;
    tsl::hopscotch_set<qml_interval*> m_intervals;

W_PROPERTY(qml_sync*, startSync READ startSync WRITE setStartSync NOTIFY startSyncChanged)
};
}
}
