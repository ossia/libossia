#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/flicks.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <atomic>
#include <memory>

/**
 * \file time_sync.hpp
 */
namespace ossia
{
class expression_base;
class state;
class time_event;
class scenario;
struct OSSIA_EXPORT time_sync_callback
{
  virtual ~time_sync_callback();

  virtual void triggered();

  //! Called when the time_sync starts evaluating
  virtual void entered_evaluation();

  //! Called when the time_sync has started triggering (e.g. was clicked)
  virtual void entered_triggering();

  //! Called when we know at which date a trigger must execute due to
  //! quantification
  virtual void trigger_date_fixed(ossia::time_value);

  //! Called if the time_sync stops evaluating due to a changing duration
  virtual void left_evaluation();

  //! Boolean : true if the evaluation was finished due to the max bound
  virtual void finished_evaluation(bool);
};

struct OSSIA_EXPORT time_sync_callbacks
{
  ossia::small_vector<time_sync_callback*, 2> callbacks;

  void clear()
  {
    for(auto cb : callbacks)
      delete cb;
    callbacks.clear();
  }

  void triggered()
  {
    for(auto v : callbacks)
      v->triggered();
  }

  void entered_evaluation()
  {
    for(auto v : callbacks)
      v->entered_evaluation();
  }

  void entered_triggering()
  {
    for(auto v : callbacks)
      v->entered_triggering();
  }

  void trigger_date_fixed(ossia::time_value t)
  {
    for(auto v : callbacks)
      v->trigger_date_fixed(t);
  }

  void left_evaluation()
  {
    for(auto v : callbacks)
      v->left_evaluation();
  }

  void finished_evaluation(bool b)
  {
    for(auto v : callbacks)
      v->finished_evaluation(b);
  }
};

/**
 * \brief #time_sync is use to describe temporal structure to synchronize each
 * attached #time_event evaluation.
 *
 * \details #time_sync is also a #time_event container.
 * \todo the shared_from_this is used at a single point, maybe it should be
 * removed ?
 * or replaced with intrusive_ptr ?
 */
class OSSIA_EXPORT time_sync final
{
  friend class ossia::scenario;

public:
  using container = ossia::small_vector<std::shared_ptr<time_event>, 2>;
  using iterator = container::iterator;
  using const_iterator = container::const_iterator;

  time_sync();
  ~time_sync();

  /*! get the date
 \details the date is the sum of its previous #time_interval durations
 \details a #time_sync with na previous #time_intervals have a date equals to
 0.
 \return #TimeValue the date */
  [[nodiscard]] time_value get_date() const noexcept;

  /*! get the expression of the #time_sync */
  [[nodiscard]] const expression& get_expression() const noexcept;

  /*! set the expression of the #time_sync
 \details setting the expression to ExpressionTrue will defer the evaluation
 on #time_event's expression
 \details setting the expression to ExpressionFalse will mute TimeSync
 execution
 \param expression_ptr
 \return #time_sync the time_sync */
  time_sync& set_expression(expression_ptr) noexcept;

  /*! create and store a #time_event
 \param #Container<#time_event>::const_iterator where to store the #time_event
 \param #time_event::ExecutionCallback to get #time_event's status back
 \param expression_ptr an std::optional expression to apply to the
 #time_event
 \return std::shared_ptr<#time_event> */
  iterator emplace(
      const_iterator, time_event::exec_callback,
      expression_ptr = expressions::make_expression_true());
  iterator insert(const_iterator, std::shared_ptr<time_event>);
  void remove(const std::shared_ptr<time_event>&);

  /*! get the #time_events of the #time_sync
 \return #Container<#time_event> */
  auto& get_time_events() noexcept { return m_timeEvents; }

  /*! get the #time_events of the #time_sync
 \return #Container<#time_event> */
  [[nodiscard]] const auto& get_time_events() const noexcept { return m_timeEvents; }

  // Interface to be used for set-up by other time processes
  [[nodiscard]] bool is_observing_expression() const noexcept;
  [[nodiscard]] bool is_evaluating() const noexcept;

  /*! evaluate all #time_event's to make them to happen or to dispose them
 \return boolean true if the operation succeeded */
  void start_trigger_request() noexcept;
  void end_trigger_request() noexcept;

  /**
   * Auto-trigger timesyncs are timesyncs which will
   * directly restart their following graph upon triggering.
   * Else, triggering the timesync will stop the following subgraph.
   * This is only relevant for subgraphs not connected
   * to the root of a ossia::scenario
   */
  [[nodiscard]] bool is_autotrigger() const noexcept;
  void set_autotrigger(bool) noexcept;

  [[nodiscard]] bool is_start() const noexcept;
  void set_start(bool) noexcept;

  //! enable observation of the ossia::expression
  void observe_expression(bool);
  void observe_expression(bool, ossia::expressions::expression_result_callback cb);

  //! Resets the internal state. Necessary when restarting an execution.
  void reset();

  /* To be called before deletion, to break the shared_ptr cycle */
  void cleanup();

  void mute(bool b);
  [[nodiscard]] bool muted() const noexcept { return m_muted; }

  /*! Execution callbacks
   *
   * Used to be notified when the #time_sync is triggered.
   */
  time_sync_callbacks callbacks;

  enum class status : uint8_t
  {
    NOT_DONE,
    DONE_TRIGGERED,
    DONE_MAX_REACHED
  };
  [[nodiscard]] status get_status() const noexcept { return m_status; }

  void set_sync_rate(double syncRatio) noexcept { m_sync_rate = syncRatio; }
  [[nodiscard]] double get_sync_rate() const noexcept { return m_sync_rate; }
  [[nodiscard]] bool has_sync_rate() const noexcept { return m_sync_rate > 0; }

  void set_trigger_date(time_value v) noexcept
  {
    m_trigger_date = v;
    callbacks.trigger_date_fixed(v);
  }
  [[nodiscard]] time_value get_trigger_date() const noexcept { return m_trigger_date; }
  [[nodiscard]] bool has_trigger_date() const noexcept
  {
    return !m_trigger_date.infinite();
  }

  void set_is_being_triggered(bool v) noexcept;
  [[nodiscard]] bool is_being_triggered() const noexcept { return m_is_being_triggered; }

private:
  ossia::expression_ptr m_expression;
  container m_timeEvents;

  std::optional<expressions::expression_callback_iterator> m_callback;

  double m_sync_rate = 0.;

  std::atomic_bool trigger_request{};
  time_value m_trigger_date = Infinite;
  status m_status : 2;
  bool m_start : 1;
  bool m_observe : 1;
  bool m_evaluating : 1;
  bool m_muted : 1;
  bool m_autotrigger : 1;
  bool m_is_being_triggered : 1;
};

}
