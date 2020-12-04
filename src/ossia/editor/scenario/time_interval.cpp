// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_process.hpp>

#include <algorithm>
#include <iostream>
#include <csignal>
#include <boost/config.hpp>
namespace ossia
{
double time_interval::get_speed(time_value date) const noexcept
{
  if(BOOST_LIKELY(!m_hasTempo))
  {
    return m_speed;
  }
  else
  {
    return m_speed * tempo(date) / ossia::root_tempo;
  }
}

void time_interval::tick_impl(
    ossia::time_value old_date, ossia::time_value new_date,
    ossia::time_value offset, const ossia::token_request& parent_request)
{
  m_tick_offset = offset;
  m_date = new_date;

#if defined(OSSIA_EXECUTION_LOG)
  auto log = g_exec_log.interval_start_run_tick(name, old_date, new_date, offset);
#endif

  if(old_date < 0_tv || new_date < 0_tv)
    return;

  m_current_signature = signature(old_date, parent_request);
  m_current_tempo = m_speed * tempo(old_date, parent_request);

  if(m_hasTempo)
  {
    m_globalSpeed = get_speed(old_date);
  }
  else
  {
    m_globalSpeed = m_parentSpeed * m_speed;
  }
  if(m_hasSignature)
  {
    // Compute the amount of bars for each signature change.
    // We need the exact amount of time that happens between two signature changes.

    // At interval t = 1s, date = sampling frequency. (result of time() function in score)
    // This is the same referential that the time of the bar changes.
    // -> date is already tempo-processed, we only need to care about the measure.
    // -> FS samples is always 0.5 measure (4/4) at ossia::root_tempo


    {
      const double num_quarters = old_date.impl / m_quarter_duration;

      auto [time, sig] = *ossia::last_before(m_timeSignature, old_date);

      auto quarters_since_last_measure_change = (old_date - time).impl / m_quarter_duration;
      auto quarters_in_bar = (4. * (double(sig.upper) / sig.lower));
      auto bars_since_last_measure_change = std::floor(quarters_since_last_measure_change / quarters_in_bar) * quarters_in_bar;

      m_musical_start_last_signature = time.impl / m_quarter_duration;
      m_musical_start_last_bar = (time.impl / m_quarter_duration + bars_since_last_measure_change);
      m_musical_start_position = num_quarters;
    }

    {
      const double num_quarters = new_date.impl / m_quarter_duration;

      auto [time, sig] = *ossia::last_before(m_timeSignature, new_date);

      auto quarters_since_last_measure_change = (new_date - time).impl / m_quarter_duration;
      auto quarters_in_bar = (4. * (double(sig.upper) / sig.lower));
      auto bars_since_last_measure_change = std::floor(quarters_since_last_measure_change / quarters_in_bar) * quarters_in_bar;

      m_musical_end_last_bar = (time.impl / m_quarter_duration + bars_since_last_measure_change);
      m_musical_end_position = num_quarters;
    }
  }
  else
  {
    m_musical_start_last_signature = parent_request.musical_start_last_signature;
    m_musical_start_last_bar = parent_request.musical_start_last_bar;
    m_musical_start_position = parent_request.musical_start_position;
    m_musical_end_last_bar = parent_request.musical_end_last_bar;
    m_musical_end_position = parent_request.musical_end_position;
  }


  state(old_date, new_date);
  if (m_callback)
    (*m_callback)(true, new_date);
}

void time_interval::tick_current(ossia::time_value offset, const ossia::token_request& parent_request)
{
  tick_impl(m_date, m_date, offset, parent_request);
}

void time_interval::tick(time_value date, const ossia::token_request& parent_request, double ratio)
{
  tick_impl(
      m_date, m_date + std::ceil(date.impl * get_speed(m_date) / ratio), m_tick_offset, parent_request);
}

void time_interval::tick_offset(time_value date, ossia::time_value offset, const ossia::token_request& parent_request)
{
  if(BOOST_UNLIKELY(m_hasTempo && parent_request.speed != 0))
  {
    // TODO : use this formula to compute the exact date at which we must end for this tick
    // t = beat / (t0/2 + ta/2)
    // we must find the equivalent constant tempo that would make us end at the exact same
    // date than a proper linear interpolation
    //auto beat = date - m_date;
    auto t0 = tempo(m_date); // TODO check what it means if the tempo comes from outside
    //auto ta = tempo(date);

    // absolute tempo given : we negate the speed of the parent
    // todo : this should be done outside for the scenario
    double speed = (m_speed * t0 / ossia::root_tempo) / parent_request.speed;

    tick_impl(m_date, m_date + std::ceil(date.impl * speed), offset, parent_request);
  }
  else
  {
    tick_impl(m_date, m_date + std::ceil(date.impl * m_speed), offset, parent_request);
  }
}

void time_interval::tick_offset_speed_precomputed(time_value date, ossia::time_value offset, const ossia::token_request& parent_request)
{
  tick_impl(m_date, m_date + date.impl, offset, parent_request);
}

time_signature time_interval::signature(time_value date, const ossia::token_request& parent_request) const noexcept
{
  if(m_hasSignature && !m_timeSignature.empty())
  {
    auto it = ossia::last_before(m_timeSignature, date);
    if(it != m_timeSignature.end())
    {
      return it->second;
    }
  }
  return parent_request.signature;
}

double time_interval::tempo(time_value date) const noexcept
{
  float t = static_cast<ossia::nodes::interval*>(node.get())->tempo;
  if (t != ossia::nodes::interval::no_tempo)
    return t;
  return m_tempoCurve.value_at(date.impl);
}

double time_interval::tempo(time_value date, const ossia::token_request& parent_request) const noexcept
{
  // TODO tempo should be hierarchic
  if(m_hasTempo)
  {
    return tempo(date);
  }
  return parent_request.tempo;
}

std::shared_ptr<time_interval> time_interval::create(
    time_interval::exec_callback callback, time_event& startEvent,
    time_event& endEvent, ossia::time_value nominal, ossia::time_value min,
    ossia::time_value max)
{
  auto timeInterval = std::make_shared<time_interval>(
      std::move(callback), startEvent, endEvent, nominal, min, max);

  startEvent.next_time_intervals().push_back(timeInterval);
  endEvent.previous_time_intervals().push_back(timeInterval);

  return timeInterval;
}

time_interval::time_interval(
    time_interval::exec_callback callback, time_event& startEvent,
    time_event& endEvent, ossia::time_value nominal, ossia::time_value min,
    ossia::time_value max)
    : node{std::make_shared<ossia::nodes::interval>()}
    , m_callback(std::move(callback))
    , m_start(startEvent)
    , m_end(endEvent)
    , m_nominal(nominal)
    , m_min(min)
    , m_max(max)
{
}

time_interval::~time_interval()
{
}

void time_interval::start_and_tick()
{
  start();
  tick_current(0_tv, {});
}

void time_interval::start()
{
  // launch the clock
  if (m_nominal <= m_offset)
    return stop();

  // start all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    if (timeProcess->enabled())
      timeProcess->start();
  }

  // set clock at a tick
  m_running = true;
  m_date = m_offset;

  if (m_callback)
    (*m_callback)(true, m_date);
}

void time_interval::stop()
{
  // stop all time processes
  for (const std::shared_ptr<ossia::time_process>& timeProcess :
       get_time_processes())
  {
    timeProcess->stop();
  }

  m_date = Zero;
  m_running = false;
  if (m_callback)
    (*m_callback)(false, m_date);
}

void time_interval::offset(ossia::time_value date)
{
  m_offset = date;
  m_date = date;

  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    // get the state of each TimeProcess at current clock position and date
    for (const auto& timeProcess : processes)
    {
      if (timeProcess->enabled())
      {
        timeProcess->offset(m_date);
      }
    }
  }
  if (m_callback)
    (*m_callback)(m_running, m_date);
}

void time_interval::transport(time_value date)
{
  m_offset = date;
  m_date = date;

  const auto& processes = get_time_processes();
  const auto N = processes.size();
  if (N > 0)
  {
    // get the state of each TimeProcess at current clock position and date
    for (const auto& timeProcess : processes)
    {
      if (timeProcess->enabled())
      {
        timeProcess->transport(m_date);
      }
    }
  }
  if (m_callback)
    (*m_callback)(m_running, m_date);
}

void time_interval::state(ossia::time_value from, ossia::time_value to)
{
  const auto& processes = get_time_processes();
  const auto N = processes.size();

  if (N > 0)
  {
    ossia::token_request tok{from, to, m_nominal, m_tick_offset, m_globalSpeed, m_current_signature, m_current_tempo};
    tok.musical_start_last_signature = this->m_musical_start_last_signature;
    tok.musical_start_last_bar = this->m_musical_start_last_bar;
    tok.musical_start_position = this->m_musical_start_position;
    tok.musical_end_last_bar = this->m_musical_end_last_bar;
    tok.musical_end_position = this->m_musical_end_position;
    node->request(tok);
    // get the state of each TimeProcess at current clock position and date
    for (const std::shared_ptr<ossia::time_process>& timeProcess : processes)
    {
      time_process& p = *timeProcess;
      if (p.enabled())
      {
#if defined(OSSIA_EXECUTION_LOG)
        auto log = g_exec_log.process_state(p.node->label());
        const auto mts = 48000. / ossia::flicks_per_second<double>;
        if(tok.physical_write_duration(mts) > 5000)
        {
          std::raise(SIGTRAP);

        }
#endif
        p.state(tok);
      }
    }
  }
}

void time_interval::pause()
{
  // pause all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->pause();
  }
}

void time_interval::resume()
{
  // resume all time processes
  for (const auto& timeProcess : get_time_processes())
  {
    timeProcess->resume();
  }
}

void time_interval::set_callback(time_interval::exec_callback cb)
{
  if (cb)
    m_callback = std::move(*cb);
  else
    m_callback = {};
}

void time_interval::set_callback(
    smallfun::function<void(bool, time_value), 32> cb)
{
  m_callback = std::move(cb);
}

void time_interval::set_stateless_callback(time_interval::exec_callback cb)
{
  if (cb)
    m_callback = std::move(*cb);
  else
    m_callback = {};
}

void time_interval::set_stateless_callback(
    smallfun::function<void(bool, time_value), 32> cb)
{
  m_callback = std::move(cb);
}

const time_value& time_interval::get_nominal_duration() const
{
  return m_nominal;
}

time_interval&
time_interval::set_nominal_duration(ossia::time_value durationNominal)
{
  m_nominal = durationNominal;

  if (m_nominal < m_min)
    set_min_duration(m_nominal);

  if (m_nominal > m_max)
    set_max_duration(m_nominal);

  if (!m_max.infinite() && m_date > m_max)
    m_date = m_max;

  return *this;
}

const time_value& time_interval::get_min_duration() const
{
  return m_min;
}

time_interval& time_interval::set_min_duration(ossia::time_value durationMin)
{
  m_min = durationMin;

  if (m_min > m_nominal)
    set_nominal_duration(m_min);

  return *this;
}

const time_value& time_interval::get_max_duration() const
{
  return m_max;
}

time_interval& time_interval::set_max_duration(ossia::time_value durationMax)
{
  m_max = durationMax;

  if (durationMax < m_nominal)
    set_nominal_duration(m_max);

  return *this;
}

time_event& time_interval::get_start_event() const
{
  return m_start;
}

time_event& time_interval::get_end_event() const
{
  return m_end;
}

void time_interval::add_time_process(std::shared_ptr<time_process> timeProcess)
{
  if (!timeProcess)
    return;

  // todo what if the interval started
  if (m_running)
    timeProcess->start();

  if(bool b = node->muted())
    timeProcess->mute(b);

  // store a TimeProcess if it is not already stored
  if (find(m_processes, timeProcess) == m_processes.end())
  {
    m_processes.push_back(std::move(timeProcess));
  }
}

void time_interval::remove_time_process(time_process* timeProcess)
{
  auto it = find_if(m_processes, [=](const auto& other) {
    return other.get() == timeProcess;
  });
  if (it != m_processes.end())
  {
    m_processes.erase(it);
  }
}

void time_interval::cleanup()
{
  m_processes.clear();
}

void time_interval::mute(bool m)
{
  node->set_mute(m);
  for(auto& p : get_time_processes())
  {
    p->mute(m);
  }
}

void time_interval::set_tempo_curve(std::optional<tempo_curve> curve)
{
  m_hasTempo = bool(curve);
  if(m_hasTempo)
  {
    m_tempoCurve = *std::move(curve);
    auto n = static_cast<ossia::nodes::interval*>(node.get());
    if(n->root_inputs().size() == 1)
    {
      n->root_inputs().push_back(new ossia::value_inlet{});
    }
  }
  else
  {
    m_tempoCurve.reset();
  }
}

void time_interval::set_time_signature_map(std::optional<time_signature_map> map)
{
  m_hasSignature = bool(map);
  if(map)
    m_timeSignature = *std::move(map);
}

void time_interval::set_quarter_duration(double tu)
{
  m_quarter_duration = tu;
}
}
