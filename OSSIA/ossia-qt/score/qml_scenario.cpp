#include "qml_scenario.hpp"
#include <ossia-qt/score/qml_interval.hpp>
#include <ossia-qt/score/qml_cond.hpp>
#include <ossia-qt/score/qml_sync.hpp>
#include <ossia-qt/score/qml_exec.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
namespace ossia
{
namespace qt
{

qml_scenario::qml_scenario(QQuickItem* parent)
  : qml_process{parent}
{
  m_impl = std::make_shared<ossia::scenario>();
  reset();
}

qml_scenario::~qml_scenario()
{

}

void qml_scenario::registerInterval(qml_interval* itv)
{
  return;
  if(m_intervals.find(itv) == m_intervals.end())
  {
    m_intervals.insert(itv);

    auto scd = itv->follows();
    auto ecd = itv->precedes();
    if(scd && ecd)
    {
      auto sev = scd->cond();
      auto eev = ecd->cond();
    }

    if(auto iv = itv->interval())
    {
      qml_exec::get(this)->submitCommand([mpl=m_impl,iv] {

      });
    }
  }
}

void qml_scenario::unregisterInterval(qml_interval* itv)
{
  return;
  auto it = m_intervals.find(itv);
  if(it != m_intervals.end())
  {
    if(auto iv = itv->interval())
    {
      qml_exec::get(this)->submitCommand([impl=m_impl,iv] {
        boost::remove_erase(iv->get_start_event().next_time_intervals(), iv);
        boost::remove_erase(iv->get_end_event().previous_time_intervals(), iv);
        impl->remove_time_interval(iv);
      });
    }

    m_intervals.erase(it);
  }
}

void qml_scenario::registerSync(qml_sync* s)
{
  return;
  if(m_syncs.find(s) == m_syncs.end())
  {
    m_syncs.insert(s);
  }
}

void qml_scenario::unregisterSync(qml_sync* s)
{
  return;
  auto it = m_syncs.find(s);
  if(it != m_syncs.end())
  {
    m_syncs.erase(it);
  }
}

void qml_scenario::setup()
{
  m_impl = std::make_shared<ossia::scenario>();
  if(!m_startSync)
    return;
  for(qml_sync* sync : this->findChildren<qml_sync*>(QString{}, Qt::FindDirectChildrenOnly))
  {
    if(sync != m_startSync)
    {
      sync->setup();
      if(auto s = sync->sync())
        m_impl->add_time_sync(s);
    }
    else
    {
      sync->setSync(m_impl->get_start_time_sync());
      sync->setup();
    }
  }

  for(qml_interval* ival: this->findChildren<qml_interval*>(QString{}, Qt::FindDirectChildrenOnly))
  {
    ival->setup();
    if(auto iv = ival->interval())
      m_impl->add_time_interval(iv);
  }
}

std::shared_ptr<time_process> qml_scenario::process() const
{
  return m_impl;
}

qml_sync* qml_scenario::startSync() const
{
  return m_startSync;
}

void qml_scenario::setStartSync(qml_sync* s)
{
  if(s != m_startSync)
  {
    m_startSync = s;
    m_startSync->setParent(this);
    m_startSync->setParentItem(this);
    startSyncChanged(s);
  }
}

void qml_scenario::reset_impl()
{
}

}
}
