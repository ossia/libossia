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
  if(m_timer)
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
  /* TODO
  // TODO simulation tool that allows to play by ticking manually and reset like uppaal
  if(!itvl)
    return;

  if(m_timer && itvl == m_cur)
  {
    m_timer = this->startTimer(8, Qt::PreciseTimer);
    return;
  }
  else if(m_timer && itvl != m_cur)
  {
    this->killTimer(*m_timer);
    return;
  }

  m_cur = itvl;
  auto sn = std::make_shared<ossia::time_sync>();
  auto en = std::make_shared<ossia::time_sync>();
  auto se = std::make_shared<ossia::time_event>([] (const ossia::time_event::status) {}, *sn, ossia::expressions::make_expression_true());
  auto ee = std::make_shared<ossia::time_event>([] (const ossia::time_event::status) {}, *en, ossia::expressions::make_expression_true());
  auto itv = std::make_shared<ossia::time_interval>(
               [this](
               double position,
               ossia::time_value date,
               const ossia::state_element& state) {
    ossia::launch(state);
    m_cur->setPlayDuration(reverseTime(date));

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
  this->startTimer(16, Qt::PreciseTimer);
  m_cur->m_interval = itv;
  m_cur->setup();
  ossia::state st;
  itv->start(st);
  ossia::launch(st);
  m_cur_t = std::chrono::high_resolution_clock::now();
  */
}

void qml_exec::pause(qml_interval* itvl)
{
  if(m_timer && itvl == m_cur)
  {
    this->killTimer(*m_timer);
    return;
  }
}

void qml_exec::stop(qml_interval* itvl)
{
  if(m_timer)
  {
    this->killTimer(*m_timer);
  }
  m_timer = ossia::none;
}

qml_exec::qml_exec()
{

}

qml_exec::~qml_exec()
{

}

void qml_exec::timerEvent(QTimerEvent* event)
{
  auto new_t = std::chrono::high_resolution_clock::now();
  auto last = m_cur_t;
  m_cur_t = new_t;

  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(m_cur_t - last).count();

  m_cur->m_interval->tick(time_value{elapsed});
}

}
}
