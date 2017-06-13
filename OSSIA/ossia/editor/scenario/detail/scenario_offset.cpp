#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>

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
using past_events_map = boost::container::flat_map<time_value, std::shared_ptr<time_event>>;
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

void process_offset(
    time_node& timenode, time_value offset, past_events_map& pastEvents)
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
      pastEvents.insert(std::make_pair(date, ev_ptr));

    // propagate offset processing to setup all TimeEvents
    for (const auto& timeConstraint : event.next_time_constraints())
    {
      process_offset(timeConstraint->get_end_event().get_time_node(), offset, pastEvents);
    }
  }
}


state_element scenario::offset(ossia::time_value offset, double pos)
{
  // reset internal offset list and state

  // a temporary list to order all past events to build the
  // offset state
  past_events_map pastEvents;

  m_runningConstraints.clear();
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
  process_offset(*m_nodes[0], offset, pastEvents);

  // build offset state from all ordered past events
  if(unmuted())
  {
    for (const auto& p : pastEvents)
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
      m_runningConstraints.insert(&cst);
    }
  }

  m_lastState = cur_state;

  return m_lastState;
}
}
