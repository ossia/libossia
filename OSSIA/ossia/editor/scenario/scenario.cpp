#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>

#include <ossia/editor/exceptions.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>
#include <unordered_map>

namespace ossia
{

scenario::scenario()
{
  // create the start TimeNode
  mTimeNodes.push_back(std::make_shared<time_node>());
}

scenario::~scenario()
{
}

using DateMap = std::unordered_map<time_node*, time_value>;
using EventPtr = std::shared_ptr<ossia::time_event>;
using ConstraintPtr = std::shared_ptr<ossia::time_constraint>;
static void process_timenode_dates(time_node& t, DateMap& map)
{
  map.insert(std::make_pair(&t, t.getDate()));

  for (EventPtr& ev : t.timeEvents())
  {
    for (ConstraintPtr& cst : ev->nextTimeConstraints())
    {
      process_timenode_dates(cst->getEndEvent().getTimeNode(), map);
    }
  }
}

state_element scenario::offset(time_value offset)
{
  if (parent->getRunning())
  {
    throw execution_error("scenario_impl::offset: "
                          "parent time constraint is running");
    return {};
  }

  // reset internal offset list and state
  mPastEventList.clear();
  ossia::state cur_state;

  // Precompute the default date of every timenode.
  std::unordered_map<time_node*, time_value> time_map;
  process_timenode_dates(*mTimeNodes[0], time_map);

  // Set *every* time constraint prior to this one to be rigid
  // note : this change the semantics of the score and should not be done like
  // this;
  // it's only a temporary bugfix for
  // https://github.com/OSSIA/i-score/issues/253 .
  for (auto& elt : time_map)
  {
    if (elt.second < offset)
    {
      for (EventPtr& ev : elt.first->timeEvents())
      {
        for (ConstraintPtr& cst_ptr : ev->previousTimeConstraints())
        {
          auto& cst = *cst_ptr;
          auto dur = cst.getDurationNominal();
          cst.setDurationMin(dur);
          cst.setDurationMax(dur);
        }
      }
    }
    else
    {
      for (EventPtr& ev_ptr : elt.first->timeEvents())
      {
        for (ConstraintPtr& cst_ptr : ev_ptr->previousTimeConstraints())
        {
          auto& cst = *cst_ptr;
          auto& start_tn = cst.getStartEvent().getTimeNode();
          auto start_date = time_map.at(&start_tn);
          if (start_date < offset)
          {
            auto dur = cst.getDurationNominal();
            auto dur_min = cst.getDurationMin();
            if (dur_min < dur)
              cst.setDurationMin(offset - start_date);
          }
        }
      }
    }
  }

  // propagate offset from the first TimeNode
  process_offset(*mTimeNodes[0], offset);

  // sort mPastEventList by date
  mPastEventList.sort();

  // build offset state from all ordered past events
  for (const auto& p : mPastEventList)
  {
    flatten_and_filter(cur_state, p.second->getState());
  }

  // offset all TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    // offset TimeConstraint's Clock
    time_value constraintOffset
        = offset - cst.getStartEvent().getTimeNode().getDate();

    if (constraintOffset >= Zero && constraintOffset <= cst.getDurationMax())
    {
      flatten_and_filter(cur_state, cst.offset(constraintOffset));
    }
  }

  return cur_state;
}

state_element scenario::state()
{
  auto& par = *parent;
  if (!par.getRunning())
  {
    throw execution_error("scenario_impl::state: "
                          "parent time constraint is not running");
    return {};
  }

  // if date hasn't been processed already
  time_value date = par.getDate();
  if (date != mLastDate)
  {
    auto prev_last_date = mLastDate;
    mLastDate = date;

    ossia::state cur_state;
    // reset internal mCurrentState

    // process the scenario from the first TimeNode to the running constraints
    ptr_container<time_event> statusChangedEvents;
    auto& n = *mTimeNodes[0];
    n.process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
    {
      auto& ev = *timeEvent;
      if (ev.getStatus() == time_event::Status::HAPPENED)
        flatten_and_filter(cur_state, ev.getState());
    }
    // make the time of each running TimeConstraint flows and add their state
    // note : this means TimeConstraint's state can overwrite TimeEvent's state
    for (const auto& timeConstraint : mTimeContraints)
    {
      auto& cst = *timeConstraint;
      if (cst.getDriveMode() != clock::DriveMode::EXTERNAL)
      {
        throw execution_error("scenario_impl::state: "
            "the pattern constraint clock is supposed to "
            "be in EXTERNAL drive mode");
        return {};
      }

      if (cst.getRunning())
      {
        // don't tick if the TimeConstraint is starting to avoid double ticks
        auto& startEvent = cst.getStartEvent();
        bool not_starting = none_of(
            statusChangedEvents, [&](const std::shared_ptr<time_event>& ev) {
              return ev->getStatus() == time_event::Status::HAPPENED
                     && ev.get() == &startEvent;
            });

        if (not_starting)
        {
          // no such event found : not starting
          if (prev_last_date == Infinite)
            cst.tick();
          else
            cst.tick((date - prev_last_date) * 1000.);
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
              cst.tick(((date - cst.getStartEvent().getTimeNode()->getDate())*
          1000.));
          */
        }
      }

      // if the time constraint is still running after the tick
      if (cst.getRunning())
      {
        flatten_and_filter(cur_state, cst.state());
      }
    }

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = !any_of(mTimeNodes, [](const std::shared_ptr<time_node>& tn) {
      return any_of(
          tn->timeEvents(), [](const std::shared_ptr<time_event>& ev) {
            return ev->getStatus() == time_event::Status::NONE;
          });
    });

    // if the Scenario is done : stop the parent TimeConstraint
    if (done)
    {
      if (date > par.getDurationMin())
      {
        ; //! \todo mParent->stop(); // if the parent TimeConstraint's Clock is
          //! in EXTERNAL drive mode, it creates a deadlock.
      }
    }
    return cur_state;
  }

  return {};
}

void scenario::start()
{
  // start each TimeConstraint if possible
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    time_event::Status startStatus = cst.getStartEvent().getStatus();
    time_event::Status endStatus = cst.getEndEvent().getStatus();

    // the constraint is in the past
    if (startStatus == time_event::Status::HAPPENED
        && endStatus == time_event::Status::HAPPENED)
    {
    }
    // the start of the constraint is pending
    else if (
        startStatus == time_event::Status::PENDING
        && endStatus == time_event::Status::NONE)
    {
    }
    // the constraint is supposed to be running
    else if (
        startStatus == time_event::Status::HAPPENED
        && endStatus == time_event::Status::NONE)
    {
      cst.start();
    }
    // the end of the constraint is pending
    else if (
        startStatus == time_event::Status::HAPPENED
        && endStatus == time_event::Status::PENDING)
    {
      cst.start();
    }
    // the constraint is in the future
    else if (
        startStatus == time_event::Status::NONE
        && endStatus == time_event::Status::NONE)
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
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    if (cst.getRunning())
    {
      cst.stop();
    }
  }
}

void scenario::pause()
{
  // pause all running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    if (cst.getRunning())
    {
      cst.pause();
    }
  }
}

void scenario::resume()
{
  // resume all running TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    if (cst.getRunning())
    {
      cst.resume();
    }
  }
}

void scenario::addTimeConstraint(
    std::shared_ptr<time_constraint> timeConstraint)
{
  auto& cst = *timeConstraint;

  // store the TimeConstraint if it is not already stored
  if (!contains(mTimeContraints, timeConstraint))
  {
    mTimeContraints.push_back(std::move(timeConstraint));
  }

  // store TimeConstraint's start node if it is not already stored
  addTimeNode(cst.getStartEvent().getTimeNode().shared_from_this());

  // store TimeConstraint's end node if it is not already stored
  addTimeNode(cst.getEndEvent().getTimeNode().shared_from_this());

  // set TimeConstraint's clock in external mode
  cst.setDriveMode(clock::DriveMode::EXTERNAL);
}

void scenario::removeTimeConstraint(
    const std::shared_ptr<time_constraint>& timeConstraint)
{
  remove_one(mTimeContraints, timeConstraint);

  // set the TimeConstraint's clock in none external mode
  timeConstraint->setDriveMode(clock::DriveMode::INTERNAL);
}

void scenario::addTimeNode(std::shared_ptr<time_node> timeNode)
{
  // store a TimeNode if it is not already stored
  if (!contains(mTimeNodes, timeNode))
  {
    mTimeNodes.push_back(std::move(timeNode));
  }
}

void scenario::removeTimeNode(const std::shared_ptr<time_node>& timeNode)
{
  remove_one(mTimeNodes, timeNode);
}

const std::shared_ptr<time_node>& scenario::getStartTimeNode() const
{
  return mTimeNodes[0];
}

const ptr_container<time_node>& scenario::timeNodes() const
{
  return mTimeNodes;
}

const ptr_container<time_constraint>& scenario::timeConstraints() const
{
  return mTimeContraints;
}

void scenario::process_offset(
    time_node& timenode, time_value offset)
{
  time_value date = timenode.getDate();

  for (const auto& event : timenode.timeEvents())
  {
    time_event::Status eventStatus;

    // evaluate event status considering its time node date
    if (date < offset)
      eventStatus = expressions::evaluate(event->getExpression())
                        ? time_event::Status::HAPPENED
                        : time_event::Status::DISPOSED;
    else if (date == offset)
      eventStatus = time_event::Status::PENDING;
    else
      eventStatus = time_event::Status::NONE;

    // evaluate event status considering previous time constraints
    for (const auto& timeConstraint : event->previousTimeConstraints())
    {
      time_value constraintOffset
          = offset - timeConstraint->getStartEvent().getTimeNode().getDate();

      if (constraintOffset < Zero)
      {
        eventStatus = time_event::Status::NONE;
      }
      else if (
          constraintOffset >= Zero
          && constraintOffset <= timeConstraint->getDurationMax())
      {
        eventStatus = constraintOffset > timeConstraint->getDurationMin()
                          ? time_event::Status::PENDING
                          : time_event::Status::NONE;
      }
      else if (constraintOffset > timeConstraint->getDurationMax())
      {
        eventStatus = expressions::evaluate(event->getExpression())
                          ? time_event::Status::HAPPENED
                          : time_event::Status::DISPOSED;
      }
    }

    // setup event status
    event->setStatus(eventStatus);

    // add HAPPENED event to offset event list
    if (eventStatus == time_event::Status::HAPPENED)
      mPastEventList.push_back(std::make_pair(date, event));

    // propagate offset processing to setup all TimeEvents
    for (const auto& timeConstraint : event->nextTimeConstraints())
    {
      process_offset(timeConstraint->getEndEvent().getTimeNode(), offset);
    }
  }
}

}
