#pragma once

#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/dataflow/token_request.hpp>

#include <ossia_export.h>

#include <memory>
#include <string>

/**
 * \file time_process.hpp
 */
namespace ossia
{
class graph_node;
class time_interval;
/**
 * @brief The time_process class
 *
 * \brief #TimeProcess is the parent class used to describe temporal content.
 */
class OSSIA_EXPORT time_process
{
public:
  std::shared_ptr<ossia::graph_node> node;
  /*! destructor */
  virtual ~time_process();

  /*! get a #StateElement from the process on its parent #time_interval
   offset
   \details don't call offset when the parent #time_interval is running
   \param date offset date
   \param pos offset position (in [0;1] relative to parent nominal duration) */
  void offset(ossia::time_value date);

  void transport(ossia::time_value date);

  /*! get a #StateElement from the process depending on its parent
   #time_interval date
   \details don't call state when the parent #time_interval is not running
   */
  virtual void state(const ossia::token_request&) = 0;

  /**
   * @brief start
   *
   * Will be called when
   * the parent time interval is started.
   */
  virtual void start();

  /**
   * @brief stop
   *
   * Will be called when
   * the parent time interval is stopped.
   */
  virtual void stop();

  /**
   * @brief stop
   *
   * Will be called when
   * the parent time interval is paused.
   */
  virtual void pause();

  /**
   * @brief resume
   *
   * Will be called when
   * the parent time interval is resumed.
   */
  virtual void resume();

  /**
   * @brief mute Mutes or unmute the process.
   *
   * `mute(true)` mutes the process.
   * A muted process executes itself but does not send its state.
   */
  void mute(bool m);

  /**
   * @brief True if the process is not currently muted.
   */
  bool unmuted() const;

  /**
   * @brief Enables or disable the process.
   *
   * Unlike a muted process, a disabled process does not execute at all.
   */
  void enable(bool m);

  /**
   * @brief True if the process is enabled.
   */
  bool enabled() const;

  /**
   * @brief Enables looping of the process after every loop_duration
   */
  void set_loops(bool b);

  /**
   * @brief Make the process's data be displaced of v units.
   */
  void set_start_offset(time_value v);

  /**
   * @brief Set how long a single loop lasts.
   *
   * Only relevant if loops have been enabled.
   */
  void set_loop_duration(time_value v);

protected:
  //! Reimplement this to have a special behaviour on mute
  virtual void mute_impl(bool);
  virtual void offset_impl(ossia::time_value date) = 0;

  virtual void transport_impl(ossia::time_value date) = 0;

  time_value m_loop_duration{};
  time_value m_start_offset{};
  bool m_loops = false; // TODO bitfields ?
  bool m_unmuted = true;
  bool m_enabled = true;
};


template<typename T>
class looping_process : public time_process
{
public:
  using time_process::time_process;
  void state(const ossia::token_request& tok) override
  {
    if(!this->m_loops)
    {
      if(this->m_start_offset == 0_tv)
        static_cast<T*>(this)->state_impl(tok);
      else
        static_cast<T*>(this)->state_impl(tok.add_offset(this->m_start_offset));
    }
    else
    {
      tok.loop(this->m_start_offset,
               this->m_loop_duration,
               [this] (const token_request& tr) { static_cast<T*>(this)->state_impl(tr); },
               [this] (const time_value& t) { static_cast<T*>(this)->transport_impl(t); }
      );
    }
  }
};

enum class sync_status
{
  NOT_READY,
  RETRY,
  DONE
};

}
