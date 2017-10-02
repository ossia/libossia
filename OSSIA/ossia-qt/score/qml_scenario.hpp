#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <hopscotch_set.h>

namespace ossia
{
namespace qt
{
class qml_interval;
class qml_sync;
class qml_scenario : public qml_process
{
    Q_OBJECT

    Q_PROPERTY(qml_sync* startSync READ startSync)
  public:
    qml_scenario(QQuickItem* parent = nullptr);
    ~qml_scenario() override;

    void registerInterval(qml_interval*);
    void unregisterInterval(qml_interval*);

    void registerSync(qml_sync*);
    void unregisterSync(qml_sync*);

    void setup() override;
    std::shared_ptr<ossia::time_process> process() const override;

    qml_sync* startSync() const
    {
      return m_startSync;
    }

private:
    void reset_impl() override;

    qml_sync* m_startSync{};
    std::shared_ptr<ossia::scenario> m_impl;

    tsl::hopscotch_set<qml_sync*> m_syncs;
    tsl::hopscotch_set<qml_interval*> m_intervals;
};
}
}
