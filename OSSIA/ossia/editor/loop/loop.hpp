#pragma once

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <ossia_export.h>

/**
 * \file loop.hpp
 */
namespace ossia
{
class graph;
/**
 * @brief The loop class
 *
 * A time process that allows looping around a time_interval.
 * First the start time_sync is checked.
 * Then the main interval executes.
 * Then the end time_sync is checked.
 */
class OSSIA_EXPORT loop final : public time_process
{
public:
  /*! factory
 \param const #TimeValue& duration of the pattern #time_interval
 \param #time_interval::ExecutionCallback to be notified at each step of
 the
 loop
 \param #time_event::ExecutionCallback to get start pattern #time_event's
 status
 back
 \param #time_event::ExecutionCallback to get end pattern #time_event's
 status
 back
 \return a new loop */
  loop(
      time_value, time_interval::exec_callback, time_event::exec_callback,
      time_event::exec_callback);

  /*! destructor */
  ~loop() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  /*! get the pattern #time_interval
 \return std::shared_ptr<TimeInterval> */
  time_interval& get_time_interval();
  const time_interval& get_time_interval() const;

  /*! get the pattern start #time_sync
 \return std::shared_ptr<TimeSync> */
  const time_sync& get_start_timesync() const;
  time_sync& get_start_timesync();

  /*! get the pattern end #time_sync
 \return std::shared_ptr<TimeSync> */
  const time_sync& get_end_timesync() const;
  time_sync& get_end_timesync();

  void transport(ossia::time_value offset, double pos) override;
  void offset(ossia::time_value, double pos) override;
  void state(
      ossia::time_value from, ossia::time_value to, double pos,
      ossia::time_value tick_offset, double gspeed) override;

private:
  bool process_sync(
      ossia::time_sync& node, ossia::time_event& event, bool pending,
      bool maxReached);
  void make_happen(time_event& event);
  void make_dispose(time_event& event);

  time_sync m_startNode;
  time_sync m_endNode;
  time_event& m_startEvent;
  time_event& m_endEvent;
  time_interval m_interval;

  ossia::time_value m_lastDate{ossia::Infinite};
};
}
