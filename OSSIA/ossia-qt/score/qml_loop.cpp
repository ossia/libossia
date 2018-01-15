#include "qml_loop.hpp"

namespace ossia
{
namespace qt
{

qml_loop::qml_loop(QQuickItem* parent)
  : qml_process{parent}
{
  m_interval = new qml_interval{this};
  reset();
}

qml_loop::~qml_loop()
{

}

qml_interval* qml_loop::interval() const
{
  return m_interval;
}

void qml_loop::setup()
{
  /* TODO
  m_impl = std::make_shared<ossia::loop>(
             0_tv,
             time_interval::exec_callback{},
             time_event::exec_callback{},
             time_event::exec_callback{});
  m_interval->m_interval = &m_impl->get_time_interval();
  m_interval->m_interval->set_callback(
        [=] (double pos, ossia::time_value tv, const ossia::state_element&) {
    m_interval->setPlayDuration(reverseTime(tv));
  });

  m_impl->get_start_timesync().set_expression(make_expression(m_startTrig, this, ossia::expressions::make_expression_true()));
  m_impl->get_end_timesync().set_expression(make_expression(m_endTrig, this, ossia::expressions::make_expression_true()));
  m_impl->get_start_timesync().get_time_events()[0]->set_expression(make_expression(m_startCond, this, ossia::expressions::make_expression_true()));
  m_impl->get_end_timesync().get_time_events()[0]->set_expression(make_expression(m_endCond, this, ossia::expressions::make_expression_true()));
  m_interval->setup();
  */
}

std::shared_ptr<time_process> qml_loop::process() const
{
  return m_impl;
}

QQmlScriptString qml_loop::startCond() const { return m_startCond; }

QQmlScriptString qml_loop::endCond() const { return m_endCond; }

QQmlScriptString qml_loop::startTrig() const { return m_startTrig; }

QQmlScriptString qml_loop::endTrig() const { return m_endTrig; }

void qml_loop::setInterval(qml_interval* interval)
{
  if (m_interval == interval)
    return;

  m_interval = interval;
  emit intervalChanged(m_interval);
}

void qml_loop::setStartCond(QQmlScriptString s) {
  if(s != m_startCond) {
    m_startCond = s;
    emit startCondChanged(s);
  }
}

void qml_loop::setEndCond(QQmlScriptString s) {
  if(s != m_endCond) {
    m_endCond = s;
    emit endCondChanged(s);
  }
}

void qml_loop::setStartTrig(QQmlScriptString s) {
  if(s != m_startTrig) {
    m_startTrig = s;
    emit startTrigChanged(s);
  }
}

void qml_loop::setEndTrig(QQmlScriptString s) {
  if(s != m_endTrig) {
    m_endTrig = s;
    emit endTrigChanged(s);
  }
}


void qml_loop::reset_impl() { }

}
}
