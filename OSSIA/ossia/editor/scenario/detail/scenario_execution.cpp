#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia/editor/exceptions.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <iostream>
#include <hopscotch_map.h>
#include <cassert>
#include <set>
#include <map>

namespace ossia
{
void scenario::make_happen(
    time_event& event,
    constraint_set& started,
    constraint_set& stopped)
{
  event.m_status = time_event::status::HAPPENED;

  // stop previous TimeConstraints
  for (auto& timeConstraint : event.previous_time_constraints())
  {
    timeConstraint->stop();
    stopped.insert(timeConstraint.get());
  }

  // setup next TimeConstraints
  for (auto& timeConstraint : event.next_time_constraints())
  {
    timeConstraint->start();
    started.insert(timeConstraint.get());
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}



void scenario::make_dispose(
    time_event& event,
    constraint_set& stopped)
{
  if (event.m_status == time_event::status::HAPPENED)
  {
    throw execution_error("time_event::dispose: "
                          "HAPPENED event cannot be disposed");
    return;
  }

  event.m_status = time_event::status::DISPOSED;

  // stop previous TimeConstraints
  for (auto& timeConstraint : event.previous_time_constraints())
  {
    timeConstraint->stop();
    stopped.insert(timeConstraint.get());
  }

  // dispose next TimeConstraints end event if everything is disposed before
  for (auto& nextTimeConstraint : event.next_time_constraints())
  {
    bool dispose = true;

    for (auto& previousTimeConstraint : nextTimeConstraint->get_end_event().previous_time_constraints())
    {
      if (previousTimeConstraint->get_start_event().get_status()
          != time_event::status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if (dispose)
      nextTimeConstraint->get_end_event().dispose();
  }

  if (event.m_callback)
    (event.m_callback)(event.m_status);
}

void scenario::process_this(
    time_node& node,
    std::vector<time_event*>& statusChangedEvents,
    constraint_set& started,
    constraint_set& stopped)
{
  // prepare to remember which event changed its status to PENDING
  // because it is needed in time_node::trigger
  node.m_pending.clear();

  bool maximalDurationReached = false;

  for (const std::shared_ptr<time_event>& timeEvent : node.get_time_events())
  {
    switch (timeEvent->get_status())
    {
      // check if NONE TimeEvent is ready to become PENDING
      case time_event::status::NONE:
      {
        bool minimalDurationReached = true;

        for (const std::shared_ptr<ossia::time_constraint>& timeConstraint : timeEvent->previous_time_constraints())
        {
          const auto& ev = timeConstraint->get_start_event();
          // previous TimeConstraints with a DISPOSED start event are ignored
          if (ev.get_status() == time_event::status::DISPOSED)
          {
            continue;
          }

          // previous TimeConstraint with a none HAPPENED start event
          // can't have reached its minimal duration
          if (ev.get_status() != time_event::status::HAPPENED)
          {
            minimalDurationReached = false;
            break;
          }

          // previous TimeConstraint which doesn't reached its minimal duration
          // force to quit
          if (timeConstraint->get_date() < timeConstraint->get_min_duration())
          {
            minimalDurationReached = false;
            break;
          }
        }

        // access to PENDING status once all previous TimeConstraints allow it
        if (minimalDurationReached)
          timeEvent->set_status(time_event::status::PENDING);
        else
          break;
      }

        // PENDING TimeEvent is ready for evaluation
      case time_event::status::PENDING:
      {
        node.m_pending.push_back(timeEvent.get());

        for (const std::shared_ptr<ossia::time_constraint>& timeConstraint : timeEvent->previous_time_constraints())
        {
          if (timeConstraint->get_date() >= timeConstraint->get_max_duration())
          {
            maximalDurationReached = true;
            break;
          }
        }

        break;
      }

      case time_event::status::HAPPENED:
      case time_event::status::DISPOSED:
      {
        break;
      }
    }
  }

  // if all TimeEvents are not PENDING
  if (node.m_pending.size() != node.m_timeEvents.size())
  {
    if(node.m_evaluating)
    {
      node.m_evaluating = false;
      node.left_evaluation.send();
    }

    return;
  }

  if(!node.m_evaluating)
  {
    node.m_evaluating = true;
    node.entered_evaluation.send();
  }

  //! \todo force triggering if at leat one TimeEvent has
  // at least one TimeConstraint over its maximal duration

  // update the expression one time
  // then observe and evaluate TimeNode's expression before to trig
  // only if no maximal duration have been reached
  if (*node.m_expression != expressions::expression_true() && !maximalDurationReached)
  {
    if (!node.is_observing_expression())
      expressions::update(*node.m_expression);

    node.observe_expression(true);

    if (!expressions::evaluate(*node.m_expression))
      return;
  }

  // trigger the time node

  // now TimeEvents will happen or be disposed
  for (auto& timeEvent : node.m_pending)
  {
    time_event& ev = *timeEvent;
    auto& expr = ev.get_expression();
    // update any Destination value into the expression
    expressions::update(expr);

    if (expressions::evaluate(expr))
      make_happen(ev, started, stopped);
    else
      make_dispose(ev, stopped);
  }

  // stop expression observation now the TimeNode has been processed
  node.observe_expression(false);

  // notify observers
  node.triggered.send();


  // former PENDING TimeEvents are now HAPPENED or DISPOSED
  statusChangedEvents.insert(
        statusChangedEvents.end(),
        node.m_pending.begin(),
        node.m_pending.end());

  node.m_pending.clear();

  node.m_evaluating = false;
  node.finished_evaluation.send(maximalDurationReached);
  if(maximalDurationReached)
    node.m_status = time_node::DONE_MAX_REACHED;
  else
    node.m_status = time_node::DONE_TRIGGERED;
}

enum progress_mode { PROGRESS_MIN, PROGRESS_MAX } mode{PROGRESS_MAX};

void update_overtick(time_constraint& constraint, time_node* end_node, ossia::time_value tick_us, ossia::time_value cst_old_date, overtick_map& node_tick_dur)
{
  // Store the over-ticking min / max, scaled to speed = 1.
  // That is : tick - (max_dur - tick_start_dur) / speed
  auto cst_speed = constraint.get_speed();
  auto ot = tick_us - (constraint.get_max_duration() * 1000. - cst_old_date) / cst_speed;

  auto node_it = node_tick_dur.lower_bound(end_node);
  if(node_it != node_tick_dur.end() && (end_node == node_it->first))
  {
    auto& cur = node_it->second;

    if(ot < cur.min) cur.min = ot;
    if(ot > cur.max) cur.max = ot;
  }
  else
  {
    node_tick_dur.insert(node_it, std::make_pair(end_node, overtick{ot, ot}));
  }
}

void scenario::tick_constraint(time_constraint& constraint, time_value tick)
{
  // Tick without going over the max
  // so that the state is not 1.01*automation for instance.
  auto cst_old_date = constraint.get_date() * 1000.;
  auto cst_max_dur = constraint.get_max_duration();
  if(!cst_max_dur.infinite())
  {
    auto this_tick = std::min(tick, cst_max_dur * 1000. - cst_old_date + 1.);
    constraint.tick(this_tick);
  }
  else
  {
    constraint.tick(tick);
  }
}

state_element scenario::state(ossia::time_value date, double pos)
{
  //ossia::logger().info("scenario::state starts");
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    // Duration of this tick.
    time_value tick_us = (prev_last_date == Infinite)
                         ? date * 1000.
                         : (date - prev_last_date) * 1000.;

    ossia::state cur_state;
    m_overticks.clear();
    m_endNodes.clear();
    constraints_started.clear();
    constraints_stopped.clear();

    m_endNodes.reserve(m_nodes.size());
    m_overticks.reserve(m_nodes.size());
    // First we should find, for each running constraint, the actual maximum tick length
    // that they can be ticked. If it is < tick_us, then they won't execute.

    // Three categories of constraints:
    // * the ones currently we're starting from
    // * intermediary ones (e.g. fitting entirely in one tick) : we take their state at 0.5
    // * the ones we're finishing in : we take their state where we finish

    std::vector<time_event*> statusChangedEvents;
    for(time_node* n : m_waitingNodes)
    {
      process_this(*n, statusChangedEvents, m_runningConstraints, m_runningConstraints);
      if(!statusChangedEvents.empty())
      {
        // TODO won't work if there are multiple waiting nodes
        m_waitingNodes.clear();
        statusChangedEvents.clear();
      }
    }

    for(time_constraint* constraint : m_runningConstraints)
    {
      auto cst_old_date = constraint->get_date() * 1000.;
      tick_constraint(*constraint, tick_us);

      // ossia::logger().info("scenario::state tick {}: {}", (void*)constraint, tick_us);

      auto end_node = &constraint->get_end_event().get_time_node();
      m_endNodes.insert(end_node);

      update_overtick(*constraint, end_node, tick_us, cst_old_date, m_overticks);
    }

    // Handle time nodes / events... if they are not finished, constraints in running_constraint are in cur_cst
    // else, add the next constraints

    for(time_node* node : m_endNodes)
    {
      process_this(*node, statusChangedEvents, constraints_started, constraints_stopped);
    }

    for(auto c : constraints_stopped)
      m_runningConstraints.erase(c);
    m_runningConstraints.insert(constraints_started.begin(), constraints_started.end());

    m_endNodes.clear();
    do
    {
      const bool is_unmuted = unmuted();
      for (const auto& timeEvent : statusChangedEvents)
      {
        time_event& ev = *timeEvent;
        switch(ev.get_status())
        {
          case time_event::status::HAPPENED:
          {
            if(is_unmuted)
            {
              flatten_and_filter(cur_state, ev.get_state());
            }

            auto& tn = ev.get_time_node();
            if(tn.get_status() == time_node::status::DONE_MAX_REACHED)
            {
              // Propagate the remaining tick to the next constraints
              auto it = m_overticks.find(&tn);
              if(it == m_overticks.end())
              {
                //ossia::logger().info("scenario::state tick_dur not found");
                continue;
              }

              time_value remaining_tick = (mode == PROGRESS_MAX)
                                          ? it->second.max
                                          : it->second.min;

              const auto& next = ev.next_time_constraints();
              //overticks.reserve(overticks.size() + next.size());
              for(const auto& constraint : next)
              {
                // ossia::logger().info("scenario::state tick {}: {}", (void*)constraint, tick_dur);
                tick_constraint(*constraint, remaining_tick);

                auto end_node = &constraint->get_end_event().get_time_node();
                m_endNodes.insert(end_node);

                update_overtick(*constraint, end_node, remaining_tick, 0._tv, m_overticks);
              }
            }
            break;
          }

          case time_event::status::DISPOSED:
          default:
            break;
        }
      }

      // For constraints that did finish, we take their last state :
      for(auto constraint : constraints_stopped)
      {
        flatten_and_filter(cur_state, constraint->state());
      }

      statusChangedEvents.clear();
      constraints_started.clear();
      constraints_stopped.clear();

      for(auto node : m_endNodes)
      {
        process_this(*node, statusChangedEvents, constraints_started, constraints_stopped);
      }

      for(auto c : constraints_stopped)
        m_runningConstraints.erase(c);
      m_runningConstraints.insert(constraints_started.begin(), constraints_started.end());
      m_endNodes.clear();

    } while(!statusChangedEvents.empty());

    // Finally, take the state of the constraints that are still running.
    for(auto& constraint : m_runningConstraints)
    {
      flatten_and_filter(cur_state, constraint->state());
    }

    m_lastState = cur_state;
  }

  // ossia::logger().info("scenario::state ends");
  return m_lastState;

}

/// Old execution algorithm ///

/*

state_element scenario::state()
{
  auto& par = *parent();
  if (!par.running())
  {
    throw execution_error("scenario_impl::state: "
                          "parent time constraint is not running");
    return {};
  }

  // if date hasn't been processed already
  time_value date = par.get_date();
  if (date != m_lastDate)
  {
    auto prev_last_date = m_lastDate;
    m_lastDate = date;

    ossia::state cur_state;
    // reset internal mCurrentState

    // process the scenario from the first TimeNode to the running constraints
    ptr_container<time_event> statusChangedEvents;
    time_node& n = *m_nodes[0];
    n.process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    if(unmuted())
    {
      for (const auto& timeEvent : statusChangedEvents)
      {
        time_event& ev = *timeEvent;
        if (ev.get_status() == time_event::status::HAPPENED)
          flatten_and_filter(cur_state, ev.get_state());
      }
    }

    // make the time of each running TimeConstraint flows and add their state
    // note : this means TimeConstraint's state can overwrite TimeEvent's state
    for (const auto& timeConstraint : m_constraints)
    {
      time_constraint& cst = *timeConstraint;
      if (cst.get_drive_mode() != clock::drive_mode::EXTERNAL)
      {
        throw execution_error("scenario_impl::state: "
            "the pattern constraint clock is supposed to "
            "be in EXTERNAL drive mode");
        return {};
      }

      if (cst.running())
      {
        // don't tick if the TimeConstraint is starting to avoid double ticks
        auto& startEvent = cst.get_start_event();
        bool not_starting = none_of(
            statusChangedEvents, [&](const std::shared_ptr<time_event>& ev) {
              return ev->get_status() == time_event::status::HAPPENED
                     && ev.get() == &startEvent;
            });

        if (not_starting)
        {
          // no such event found : not starting
          if (prev_last_date == Infinite)
            cst.tick(date * 1000.);
          else
            cst.tick(ossia::time_value{(date - prev_last_date) * 1000.});
        }
        else
        {

          // We advance the constraint so that we don't loose time
          // TODO getDate is worst-case linear, maybe we should cache it to
          // have the executedDate in constant time ?
//          if(prev_last_date == Infinite)
//              cst.tick();
//          else
//              cst.tick(((date - cst.get_start_event().get_time_node()->get_date())*
//          1000.));

        }
      }

      // if the time constraint is still running after the tick
      if (cst.running())
      {
        flatten_and_filter(cur_state, cst.state());
      }
    }
    m_lastState = cur_state;

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = !any_of(m_nodes, [](const std::shared_ptr<time_node>& tn) {
      return any_of(
          tn->get_time_events(), [](const std::shared_ptr<time_event>& ev) {
            return ev->get_status() == time_event::status::NONE;
          });
    });

    // if the Scenario is done : stop the parent TimeConstraint
    if (done)
    {
      if (date > par.get_min_duration())
      {
        ; //! \todo mParent->stop(); // if the parent TimeConstraint's Clock is
          //! in EXTERNAL drive mode, it creates a deadlock.
      }
    }
  }

  return m_lastState;
}

*/

}
