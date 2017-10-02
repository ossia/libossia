#include "qml_exec.hpp"
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/expression/expression.hpp>
namespace ossia
{
namespace qt
{

qml_exec&qml_exec::instance()
{
  static qml_exec e;
  return e;
}

qml_exec* qml_exec::get(QObject* obj)
{
  return &instance();
}

void qml_exec::submitCommand(std::function<void ()> v)
{
  if(m_clock)
  {
    m_queue.enqueue(std::move(v));
  }
  else
  {
    v();
  }
}

void qml_exec::play(qml_interval* itvl)
{
  // TODO simulation tool that allows to play by ticking manually and reset like uppaal
  if(!itvl)
    return;

  if(m_clock && itvl == m_cur)
  {
    m_clock->resume();
    return;
  }
  else if(m_clock && itvl != m_cur)
  {
    m_clock->stop();
  }

  m_cur = itvl;
  auto sn = std::make_shared<ossia::time_sync>();
  auto en = std::make_shared<ossia::time_sync>();
  auto se = std::make_shared<ossia::time_event>([] (const ossia::time_event::status) {}, *sn, ossia::expressions::make_expression_true());
  auto ee = std::make_shared<ossia::time_event>([] (const ossia::time_event::status) {}, *en, ossia::expressions::make_expression_true());
  auto itv = std::make_shared<ossia::time_interval>(
        [this,  &score_cst = m_cur](
        double position,
        ossia::time_value date,
        const ossia::state_element& state)
  {
    ossia::launch(state);

    score_cst->setPlayDuration(reverseTime(date));

    std::function<void()> c;
    while(m_queue.try_dequeue(c))
    {
      c();
    }
  }, *se, *ee,
  defaultTime(itvl->nominalDuration()),
  defaultTime(itvl->minDuration()),
  defaultTime(itvl->maxDuration())
  );
  m_cur->m_interval = itv;
  m_cur->setup();
  m_clock = std::make_unique<ossia::clock>(*m_cur->interval(), 1.0);
  m_clock->set_granularity(std::chrono::microseconds(8000));
  m_clock->start();
}

void qml_exec::pause(qml_interval* itvl)
{
  if(m_clock && itvl == m_cur)
  {
    m_clock->pause();
    return;
  }
}

void qml_exec::stop(qml_interval* itvl)
{
  if(m_clock)
  {
    m_clock->stop();
  }
  m_clock.reset();
}

qml_exec::qml_exec()
{

}

qml_exec::~qml_exec()
{

}

}
}
