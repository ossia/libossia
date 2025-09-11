#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <cstdint>
#include <functional>
#include <memory>

/**
 * \file time_event.hpp
 */

namespace ossia
{
class state;
class time_interval;
class time_sync;
class time_process;
class scenario;
/**
 * @brief The time_event class
 *
 * \brief #time_event is use to describe temporal structure to launch the start
 * or the end of each attached #time_interval.
 *
 * \details #time_event has a #State and can also be submitted to an
 * expression.
 *
 */
class OSSIA_EXPORT time_event
{
  friend class ossia::scenario;

public:
  /*! event status */
  enum class status : uint8_t
  {
    NONE = 0b00000000,
    PENDING = 0b00000001,
    HAPPENED = 0b00000010,
    DISPOSED = 0b00000011,
    FINISHED = 0b10000000
  };

  /**
   * @brief The OffsetBehavior enum
   * Describes what happens when a parent scenario
   * does an offset beyond this event. This is useful to
   * make default cases for the scenario.
   */
  enum class offset_behavior : uint8_t
  {
    EXPRESSION_TRUE,  //! The condition is considered True
    EXPRESSION_FALSE, //! The condition is considered False
    EXPRESSION        //! The condition will be evaluated
  };

  /*! to get the event status back
     \param #Status new status */
  using exec_callback = std::function<void(status)>;

public:
  time_event(
      time_event::exec_callback, time_sync& aTimeSync, expression_ptr anExpression);

  /*! destructor */
  ~time_event();

  /*! changes the callback in the event
   \param #time_event::ExecutionCallback to get #time_event's status back
   \details this may be unsafe to do during execution */
  void set_callback(time_event::exec_callback);

  void add_time_process(std::shared_ptr<time_process>);
  void remove_time_process(time_process*);
  [[nodiscard]] const auto& get_time_processes() const { return m_processes; }

  void tick(ossia::time_value date, ossia::time_value offset, double speed);

  /*! get the #time_sync where the event is
   \return std::shared_ptr<#time_sync> */
  [[nodiscard]] time_sync& get_time_sync() const;

  void set_time_sync(time_sync&);

  /*! get the expression of the event
  \return std::shared_ptr<expression> */
  [[nodiscard]] const expression& get_expression() const;

  /*! set the expression of the event
   \param std::shared_ptr<expression>
   \return #time_event the event */
  time_event& set_expression(expression_ptr);

  /*! get the status of the event
   \return #Status */
  [[nodiscard]] status get_status() const;

  /**
   * @brief getOffsetValue Returns the value of the condition if
   * we are offsetting past this time event.
   */
  [[nodiscard]] offset_behavior get_offset_behavior() const;

  /**
   * @brief setOffsetValue Sets the value of the condition if we are offsetting
   * past this time event.
   */
  time_event& set_offset_behavior(offset_behavior);

  /*! get previous time constraints attached to the event
   \return #Container<#time_interval> */
  auto& previous_time_intervals() { return m_previous_time_intervals; }

  /*! get previous time constraints attached to the event
   \return #Container<#TimeProcess> */
  [[nodiscard]] const auto& previous_time_intervals() const
  {
    return m_previous_time_intervals;
  }

  /*! get next time constraints attached to the event
   \return #Container<#time_interval> */
  auto& next_time_intervals() { return m_next_time_intervals; }

  /*! get next time constraints attached to the event
   \return #Container<#TimeProcess> */
  [[nodiscard]] const auto& next_time_intervals() const { return m_next_time_intervals; }

  void set_status(status s);

  void reset();

  /* To be called before deletion, to break the shared_ptr cycle */
  void cleanup();

  void mute(bool m);

private:
  time_event::exec_callback m_callback;

  time_sync* m_timesync{};
  ossia::small_vector<std::shared_ptr<time_process>, 1> m_processes;
  status m_status;
  offset_behavior m_offset{offset_behavior::EXPRESSION_TRUE};

  expression_ptr m_expression;

  small_ptr_container<time_interval, 1> m_previous_time_intervals;
  small_ptr_container<time_interval, 1> m_next_time_intervals;
};

inline constexpr auto
operator&(ossia::time_event::status lhs, ossia::time_event::status rhs) noexcept
{
  using t = std::underlying_type_t<ossia::time_event::status>;
  return static_cast<t>(lhs) & static_cast<t>(rhs);
}
}
