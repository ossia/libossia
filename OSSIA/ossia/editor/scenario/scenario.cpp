#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>

#include <ossia/editor/exceptions.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>
#include <hopscotch_map.h>
#include <cassert>

namespace ossia
{

scenario::scenario()
{
  // create the start TimeNode
  m_nodes.push_back(std::make_shared<time_node>());
}

scenario::~scenario()
{
  for(auto& timenode : m_nodes)
  {
    timenode->cleanup();
  }
}

using DateMap = tsl::hopscotch_map<time_node*, time_value>;
using EventPtr = std::shared_ptr<ossia::time_event>;
using ConstraintPtr = std::shared_ptr<ossia::time_constraint>;
static void process_timenode_dates(time_node& t, DateMap& map)
{
  map.insert(std::make_pair(&t, t.get_date()));

  for (EventPtr& ev : t.get_time_events())
  {
    for (ConstraintPtr& cst : ev->next_time_constraints())
    {
      process_timenode_dates(cst->get_end_event().get_time_node(), map);
    }
  }
}

state_element scenario::offset(ossia::time_value offset)
{
  auto& p = *parent();
  if (p.running())
  {
    throw execution_error("scenario_impl::offset: "
                          "parent time constraint is running");
    return {};
  }

  // reset internal offset list and state
  m_pastEvents.clear();
  ossia::state cur_state;

  // Precompute the default date of every timenode.
  tsl::hopscotch_map<time_node*, time_value> time_map;
  process_timenode_dates(*m_nodes[0], time_map);

  // Set *every* time constraint prior to this one to be rigid
  // note : this change the semantics of the score and should not be done like
  // this;
  // it's only a temporary (1 year later: haha) bugfix for
  // https://github.com/OSSIA/i-score/issues/253 .
  for (auto& elt : time_map)
  {
    if (elt.second < offset)
    {
      for (EventPtr& ev : elt.first->get_time_events())
      {
        for (ConstraintPtr& cst_ptr : ev->previous_time_constraints())
        {
          time_constraint& cst = *cst_ptr;
          auto dur = cst.get_nominal_duration();
          cst.set_min_duration(dur);
          cst.set_max_duration(dur);
        }
      }
    }
    else
    {
      for (EventPtr& ev_ptr : elt.first->get_time_events())
      {
        for (ConstraintPtr& cst_ptr : ev_ptr->previous_time_constraints())
        {
          time_constraint& cst = *cst_ptr;
          auto& start_tn = cst.get_start_event().get_time_node();
          auto start_date_it = time_map.find(&start_tn);
          if(start_date_it != time_map.end())
          {
            auto start_date = start_date_it->second;
            if (start_date < offset)
            {
              auto dur = cst.get_nominal_duration();
              auto dur_min = cst.get_min_duration();
              if (dur_min < dur)
                cst.set_min_duration(offset - start_date);
            }
          }
        }
      }
    }
  }

  // propagate offset from the first TimeNode
  process_offset(*m_nodes[0], offset);

  // build offset state from all ordered past events
  if(unmuted())
  {
    for (const auto& p : m_pastEvents)
    {
      flatten_and_filter(cur_state, p.second->get_state());
    }
  }

  // offset all TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    // offset TimeConstraint's Clock
    time_value constraintOffset
        = offset - cst.get_start_event().get_time_node().get_date();

    if (constraintOffset >= Zero && constraintOffset <= cst.get_max_duration())
    {
      flatten_and_filter(cur_state, cst.offset(constraintOffset));
    }
  }

  m_lastState = cur_state;

  return m_lastState;
}

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
            cst.tick();
          else
            cst.tick(ossia::time_value{(date - prev_last_date) * 1000.});
        }
        else
        {
          /*
          // We advance the constraint so that we don't loose time
          // TODO getDate is worst-case linear, maybe we should cache it to
          // have the executedDate in constant time ?
          if(prev_last_date == Infinite)
              cst.tick();
          else
              cst.tick(((date - cst.get_start_event().get_time_node()->get_date())*
          1000.));
          */
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

void scenario::start()
{
  // start each TimeConstraint if possible
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    time_event::status startStatus = cst.get_start_event().get_status();
    time_event::status endStatus = cst.get_end_event().get_status();

    // the constraint is in the past
    if (startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::HAPPENED)
    {
    }
    // the start of the constraint is pending
    else if (
        startStatus == time_event::status::PENDING
        && endStatus == time_event::status::NONE)
    {
    }
    // the constraint is supposed to be running
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::NONE)
    {
      cst.start();
    }
    // the end of the constraint is pending
    else if (
        startStatus == time_event::status::HAPPENED
        && endStatus == time_event::status::PENDING)
    {
      cst.start();
    }
    // the constraint is in the future
    else if (
        startStatus == time_event::status::NONE
        && endStatus == time_event::status::NONE)
    {
    }
    // error
    else
    {
      throw execution_error("scenario_impl::start: "
          "TimeEvent's status configuration of the "
          "TimeConstraint is not handled");
    }
  }
}

void scenario::stop()
{
  // stop each running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    if (cst.running())
    {
      cst.stop();
    }
  }

  for(const auto& node : m_nodes)
  {
      node->reset();
  }
}

void scenario::pause()
{
  // pause all running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    if (cst.running())
    {
      cst.pause();
    }
  }
}

void scenario::resume()
{
  // resume all running TimeConstraints
  for (const auto& timeConstraint : m_constraints)
  {
    auto& cst = *timeConstraint;
    if (cst.running())
    {
      cst.resume();
    }
  }
}

void scenario::add_time_constraint(
    std::shared_ptr<time_constraint> timeConstraint)
{
  time_constraint& cst = *timeConstraint;

  // store the TimeConstraint if it is not already stored
  if (!contains(m_constraints, timeConstraint))
  {
    m_constraints.push_back(std::move(timeConstraint));
  }

  // store TimeConstraint's start node if it is not already stored
  add_time_node(cst.get_start_event().get_time_node().shared_from_this());

  // store TimeConstraint's end node if it is not already stored
  add_time_node(cst.get_end_event().get_time_node().shared_from_this());

  // set TimeConstraint's clock in external mode
  cst.set_drive_mode(clock::drive_mode::EXTERNAL);
}

void scenario::remove_time_constraint(
    const std::shared_ptr<time_constraint>& timeConstraint)
{
  remove_one(m_constraints, timeConstraint);

  // set the TimeConstraint's clock in none external mode
  timeConstraint->set_drive_mode(clock::drive_mode::INTERNAL);
}

void scenario::add_time_node(std::shared_ptr<time_node> timeNode)
{
  // store a TimeNode if it is not already stored
  if (!contains(m_nodes, timeNode))
  {
    m_nodes.push_back(std::move(timeNode));
  }
}

void scenario::remove_time_node(const std::shared_ptr<time_node>& timeNode)
{
  remove_one(m_nodes, timeNode);
}

const std::shared_ptr<time_node>& scenario::get_start_time_node() const
{
  return m_nodes[0];
}

const ptr_container<time_node>& scenario::get_time_nodes() const
{
  return m_nodes;
}

const ptr_container<time_constraint>& scenario::get_time_constraints() const
{
  return m_constraints;
}

void scenario::process_offset(
    time_node& timenode, time_value offset)
{
  time_value date = timenode.get_date();
  auto get_event_status = [] (const time_event& event)
  {
    switch(event.get_offset_behavior())
    {
      case time_event::offset_behavior::EXPRESSION_TRUE:
        return time_event::status::HAPPENED;
      case time_event::offset_behavior::EXPRESSION_FALSE:
        return time_event::status::DISPOSED;
      case time_event::offset_behavior::EXPRESSION:
        return expressions::evaluate(event.get_expression())
                          ? time_event::status::HAPPENED
                          : time_event::status::DISPOSED;
      default:
        return time_event::status::NONE;
    }
  };

  for (auto& ev_ptr : timenode.get_time_events())
  {
    auto& event = *ev_ptr;
    time_event::status eventStatus;

    // evaluate event status considering its time node date
    if (date < offset)
    {
      eventStatus = get_event_status(event);
    }
    else if (date == offset)
    {
      eventStatus = time_event::status::PENDING;
    }
    else
    {
      eventStatus = time_event::status::NONE;
    }

    // evaluate event status considering previous time constraints
    for (const auto& timeConstraint : event.previous_time_constraints())
    {
      time_value constraintOffset
          = offset - timeConstraint->get_start_event().get_time_node().get_date();

      if (constraintOffset < Zero)
      {
        eventStatus = time_event::status::NONE;
      }
      else if (constraintOffset >= Zero
               && constraintOffset <= timeConstraint->get_max_duration())
      {
        eventStatus = constraintOffset > timeConstraint->get_min_duration()
                          ? time_event::status::PENDING
                          : time_event::status::NONE;
      }
      else if (constraintOffset > timeConstraint->get_max_duration())
      {
        eventStatus = get_event_status(event);
      }
    }

    // setup event status
    event.set_status(eventStatus);

    // add HAPPENED event to offset event list
    if (eventStatus == time_event::status::HAPPENED)
      m_pastEvents.insert(std::make_pair(date, ev_ptr));

    // propagate offset processing to setup all TimeEvents
    for (const auto& timeConstraint : event.next_time_constraints())
    {
      process_offset(timeConstraint->get_end_event().get_time_node(), offset);
    }
  }
}

}
