#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/transport.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/flicks.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/scenario/execution_log.hpp>
#include <ossia/editor/scenario/time_signature.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <smallfun.hpp>

#include <memory>

/**
 * \file time_interval.hpp
 */
namespace ossia
{
struct token_request;
class time_event;
class time_process;
class graph_node;

using time_signature_map = ossia::flat_map<ossia::time_value, time_signature>;
using tempo_curve = ossia::curve<int64_t, double>;
/**
 * @brief The time_interval class
 *
 * \brief #time_interval is use to describe temporal structure precising
 * which
 * amount of time (the duration) are between two #time_events.
 *
 * \details The duration can be fixed or between a minimal and a maximal
 * duration.
 *
 * #time_interval is a #TimeProcess container.
 */
class OSSIA_EXPORT time_interval
{
public:
  const std::shared_ptr<ossia::graph_node> node;

  auto get_date() const noexcept { return m_date; }
  auto get_offset() const noexcept { return m_offset; }
  double get_internal_speed() const noexcept { return m_speed; }

  double get_speed(time_value date) const noexcept;
  void set_offset(ossia::time_value g) noexcept { m_offset = g; }

  void set_speed(double g) noexcept { m_speed = g; }

  void set_parent_speed(double sp) noexcept { m_parentSpeed = sp; }
  tick_transport_info current_transport_info() const noexcept;

  void
  tick_current(ossia::time_value offset, const ossia::token_request& parent_request);

  void tick(
      ossia::time_value, const ossia::token_request& parent_request, double ratio = 1.0);
  void tick_offset(
      ossia::time_value, ossia::time_value offset,
      const ossia::token_request& parent_request);
  void tick_offset_speed_precomputed(
      ossia::time_value, ossia::time_value offset,
      const ossia::token_request& parent_request);

  /*! to get the interval execution back
   \param const #TimeValue position
   \param const #TimeValue date
   \param std::shared_ptr<#State> */
  using exec_callback
      = std::optional<smallfun::function<void(bool, ossia::time_value), 32>>;

  /*! constructor
   \details by default a #time_interval has an infinite duration with no
   minimal duration and an infinite maximal duration.
   \param #time_interval::ExecutionCallback to use to be notified at each
   step
   \param #std::shared_ptr<TimeEvent> event where the #time_interval starts
   \param #std::shared_ptr<TimeEvent> event where the #time_interval ends
   \param const #TimeValue& nominal duration of the #time_interval
   \param const #TimeValue& minimal duration of the #time_interval
   \param const #TimeValue& maximal duration of the #time_interval
   \return std::shared_ptr<#time_interval> */
  static std::shared_ptr<time_interval> create(
      time_interval::exec_callback, time_event&, time_event&, time_value = Infinite,
      ossia::time_value = Zero, ossia::time_value = Infinite);

  time_interval(
      time_interval::exec_callback, time_event&, time_event&, time_value = Infinite,
      ossia::time_value = Zero, ossia::time_value = Infinite);

  /*! destructor */
  ~time_interval();

  /*! start #time_interval */
  void start();
  void start_and_tick();

  /*! stop #time_interval */
  void stop();

  /*! start #time_interval */
  void pause();

  /*! stop #time_interval */
  void resume();

  /*! set #time_interval's offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_interval manages
   \details don't call offset when the #time_interval is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  void offset(ossia::time_value);

  void transport(ossia::time_value);

  /*! sets a new callback for the interval
    \param #time_interval::ExecutionCallback to use to be notified at each
    step
    */
  void set_callback(exec_callback);
  void set_callback(smallfun::function<void(bool, ossia::time_value), 32>);

  //! This callback won't compute the state.
  void set_stateless_callback(exec_callback);
  void set_stateless_callback(smallfun::function<void(bool, ossia::time_value), 32>);

  /*! get the #time_interval nominal duration
   \return const #TimeValue& nominal duration */
  const time_value& get_nominal_duration() const;

  /*! set the #time_interval duration
   \param const #TimeValue& duration
   \return #time_interval the interval */
  time_interval& set_nominal_duration(ossia::time_value);

  /*! get the #time_interval minimal duration
   \return const #TimeValue& minimal duration */
  const time_value& get_min_duration() const;

  /*! set the #time_interval minimal duration
   \param const #TimeValue& minimal duration
   \return #time_interval the interval */
  time_interval& set_min_duration(ossia::time_value);

  /*! get the #time_interval maximal duration
   \return const #TimeValue& maximal duration */
  const time_value& get_max_duration() const;

  /*! set the #time_interval maximal duration
   \param const #TimeValue& maximal duration
   \return #time_interval the interval */
  time_interval& set_max_duration(ossia::time_value);

  /*! get the event from where the #time_interval starts
   \return std::shared_ptr<#time_event> start event */
  time_event& get_start_event() const;

  /*! get the event from where the #time_interval starts
   \return std::shared_ptr<#time_event> start event */
  time_event& get_end_event() const;

  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the
   #time_interval's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void add_time_process(std::shared_ptr<time_process>);

  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the
   #time_interval's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void remove_time_process(time_process*);

  void reserve_processes(std::size_t sz);

  /*! get time processes attached to the #time_interval
   \return #Container<#TimeProcess> */
  const std::vector<std::shared_ptr<time_process>>& get_time_processes() const
  {
    return m_processes;
  }

  bool running() const noexcept { return m_running; }
  void cleanup();
  void mute(bool);

  void set_tempo_curve(std::optional<tempo_curve> curve);
  void set_time_signature_map(std::optional<time_signature_map> map);
  void set_quarter_duration(double tu);

#if defined(OSSIA_EXECUTION_LOG)
  std::string name;
#endif

  bool graphal{};

private:
  time_interval(const time_interval&) = delete;
  time_interval(time_interval&&) = delete;
  time_interval& operator=(const time_interval&) = delete;
  time_interval& operator=(time_interval&&) = delete;

  /*! get a #State from the interval depending on its date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_interval manages
   \details don't call state when the #time_interval is not running
   \return std::shared_ptr<#State> */
  void state(ossia::time_value from, ossia::time_value to);

  time_signature
  signature(time_value date, const ossia::token_request& parent_request) const noexcept;
  double
  tempo(time_value date, const ossia::token_request& parent_request) const noexcept;
  double tempo(time_value date) const noexcept;

  void tick_impl(
      ossia::time_value old_date, ossia::time_value new_date, ossia::time_value offset,
      const ossia::token_request& parent_request);

  std::vector<std::shared_ptr<time_process>> m_processes;
  time_interval::exec_callback m_callback;

  time_event& m_start;
  time_event& m_end;

  time_value m_nominal{};
  time_value m_min{};
  time_value m_max{};

  time_value m_date{};
  time_value m_offset{}; /// the date the clock will run from

  time_value m_tick_offset{}; /// offset in the current tick

  double m_current_tempo{};

  time_signature_map m_timeSignature{};
  tempo_curve m_tempoCurve{};

  ossia::quarter_note m_musical_start_last_signature{};

  ossia::quarter_note m_musical_start_last_bar{};
  ossia::quarter_note m_musical_start_position{};

  ossia::quarter_note m_musical_end_last_bar{};
  ossia::quarter_note m_musical_end_position{};

  double m_speed{1.}; /// tick length is multiplied by this
  double m_globalSpeed{1.};
  double m_parentSpeed{1.};
  time_signature m_current_signature{};
  double m_quarter_duration{ossia::quarter_duration<double>};
  bool m_running{false};
  bool m_hasTempo{false};
  bool m_hasSignature{false};
};
}
