#include "Scenario_impl.hpp"
#include <Misc/Util.h>
#include <unordered_map>
#include <iostream>
namespace impl
{
JamomaScenario::JamomaScenario() :
JamomaTimeProcess()
{
  // create the start TimeNode
  mTimeNodes.push_back(TimeNode::create());
}

JamomaScenario::JamomaScenario(const JamomaScenario * other) :
JamomaTimeProcess()
{}

shared_ptr<Scenario> JamomaScenario::clone() const
{
  return make_shared<JamomaScenario>(this);
}

JamomaScenario::~JamomaScenario()
{}


# pragma mark -
# pragma mark Execution
using DateMap = std::unordered_map<TimeNode*, TimeValue>;
using EventPtr = std::shared_ptr<OSSIA::TimeEvent>;
using ConstraintPtr = std::shared_ptr<OSSIA::TimeConstraint>;
static void process_timenode_dates(TimeNode& t, DateMap& map)
{
    map.insert(std::make_pair(&t, t.getDate()));

    for(EventPtr& ev : t.timeEvents())
    {
        for(ConstraintPtr& cst : ev->nextTimeConstraints())
        {
            process_timenode_dates(*cst->getEndEvent()->getTimeNode(), map);
        }
    }
}

StateElement JamomaScenario::offset(TimeValue offset)
{
  if (parent->getRunning())
    throw runtime_error("parent time constraint is running");

  // reset internal offset list and state
  mPastEventList.clear();
  OSSIA::State cur_state;

  // Precompute the default date of every timenode.
  std::unordered_map<TimeNode*, TimeValue> time_map;
  process_timenode_dates(*mTimeNodes[0], time_map);

  // Set *every* time constraint prior to this one to be rigid
  // note : this change the semantics of the score and should not be done like this;
  // it's only a temporary bugfix for https://github.com/OSSIA/i-score/issues/253 .
  for(auto& elt : time_map)
  {
      if(elt.second < offset)
      {
          for(EventPtr& ev : elt.first->timeEvents())
          {
              for(ConstraintPtr& cst_ptr : ev->previousTimeConstraints())
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
          for(EventPtr& ev_ptr : elt.first->timeEvents())
          {
              for(ConstraintPtr& cst_ptr : ev_ptr->previousTimeConstraints())
              {
                  auto& cst = *cst_ptr;
                  auto& start_tn = cst.getStartEvent()->getTimeNode();
                  auto start_date = time_map.at(start_tn.get());
                  if(start_date < offset)
                  {
                      auto dur = cst.getDurationNominal();
                      auto dur_min = cst.getDurationMin();
                      if(dur_min < dur)
                          cst.setDurationMin(offset - start_date);
                  }
              }
          }
      }
  }


  // propagate offset from the first TimeNode
  process_offset(mTimeNodes[0], offset);

  // sort mPastEventList by date
  mPastEventList.sort();

  // build offset state from all ordered past events
  for (const auto& p : mPastEventList)
  {
    flattenAndFilter(cur_state, p.second->getState());
  }

  // offset all TimeConstraints
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    // offset TimeConstraint's Clock
    TimeValue constraintOffset = offset - cst.getStartEvent()->getTimeNode()->getDate();

    if (constraintOffset >= Zero && constraintOffset <= cst.getDurationMax())
    {
      flattenAndFilter(cur_state, cst.offset(constraintOffset));
    }
  }

  return cur_state;
}

StateElement JamomaScenario::state()
{
  auto& par = *parent;
  if (!par.getRunning())
    throw runtime_error("parent time constraint is not running");

  // if date hasn't been processed already
  TimeValue date = par.getDate();
  if (date != mLastDate)
  {
    auto prev_last_date = mLastDate;
    mLastDate = date;

    OSSIA::State cur_state;
    // reset internal mCurrentState

    // process the scenario from the first TimeNode to the running constraints
    Container<TimeEvent> statusChangedEvents;
    shared_ptr<JamomaTimeNode> n = dynamic_pointer_cast<JamomaTimeNode>(mTimeNodes[0]);
    n->process(statusChangedEvents);

    // add the state of each newly HAPPENED TimeEvent
    for (const auto& timeEvent : statusChangedEvents)
    {
      auto& ev = *timeEvent;
      if (ev.getStatus() == TimeEvent::Status::HAPPENED)
        flattenAndFilter(cur_state, ev.getState());
    }
    // make the time of each running TimeConstraint flows and add their state
    // note : this means TimeConstraint's state can overwrite TimeEvent's state
    for (const auto& timeConstraint : mTimeContraints)
    {
      auto& cst = *timeConstraint;
      if (cst.getDriveMode() != Clock::DriveMode::EXTERNAL)
        throw runtime_error("the pattern constraint clock is supposed to be in EXTERNAL drive mode");

      if (cst.getRunning())
      {
        // don't tick if the TimeConstraint is starting to avoid double ticks
        auto& startEvent = cst.getStartEvent();
        bool not_starting = none_of(statusChangedEvents,
                                    [&] (const std::shared_ptr<TimeEvent>& ev)
                                    {
                                      return ev->getStatus() == TimeEvent::Status::HAPPENED && ev == startEvent;
                                    });

        if (not_starting)
        {
          // no such event found : not starting
          if(prev_last_date == Infinite)
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
                cst.tick(((date - cst.getStartEvent()->getTimeNode()->getDate())* 1000.));
            */
        }
      }

      // if the time constraint is still running after the tick
      if (cst.getRunning())
      {
          flattenAndFilter(cur_state, cst.state());
      }
    }

    // if all the TimeEvents are not NONE : the Scenario is done
    bool done = !any_of(mTimeNodes,
           [] (const std::shared_ptr<TimeNode>& tn)
    {
        return any_of(tn->timeEvents(),
            [] (const std::shared_ptr<TimeEvent>& ev)
        {
            return ev->getStatus() == TimeEvent::Status::NONE;
        });
    });

    // if the Scenario is done : stop the parent TimeConstraint
    if (done)
    {
      if (date > par.getDurationMin())
        ;//! \todo mParent->stop(); // if the parent TimeConstraint's Clock is in EXTERNAL drive mode, it creates a deadlock.
    }
    return cur_state;
  }

  return {};
}

# pragma mark -
# pragma mark Execution - Implementation specific

void JamomaScenario::start()
{
  // start each TimeConstraint if possible
  for (const auto& timeConstraint : mTimeContraints)
  {
    auto& cst = *timeConstraint;
    TimeEvent::Status startStatus = cst.getStartEvent()->getStatus();
    TimeEvent::Status endStatus = cst.getEndEvent()->getStatus();

    // the constraint is in the past
    if (startStatus == TimeEvent::Status::HAPPENED &&
        endStatus == TimeEvent::Status::HAPPENED)
    {}
    // the start of the constraint is pending
    else if (startStatus == TimeEvent::Status::PENDING &&
             endStatus == TimeEvent::Status::NONE)
    {}
    // the constraint is supposed to be running
    else if (startStatus == TimeEvent::Status::HAPPENED &&
             endStatus == TimeEvent::Status::NONE)
    {
      cst.start();
    }
    // the end of the constraint is pending
    else if (startStatus == TimeEvent::Status::HAPPENED &&
             endStatus == TimeEvent::Status::PENDING)
    {
      cst.start();
    }
    // the constraint is in the future
    else if (startStatus == TimeEvent::Status::NONE &&
             endStatus == TimeEvent::Status::NONE)
    {}
    // error
    else
      throw runtime_error("TimeEvent's status configuration of the TimeConstraint is not handled");
  }
}

void JamomaScenario::stop()
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

void JamomaScenario::pause()
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

void JamomaScenario::resume()
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

# pragma mark -
# pragma mark Edition

void JamomaScenario::addTimeConstraint(shared_ptr<TimeConstraint> timeConstraint)
{
  auto& cst = *timeConstraint;

  // store the TimeConstraint if it is not already stored
  if (!contains(mTimeContraints, timeConstraint))
  {
    mTimeContraints.push_back(std::move(timeConstraint));
  }

  // store TimeConstraint's start node if it is not already stored
  addTimeNode(cst.getStartEvent()->getTimeNode());

  // store TimeConstraint's end node if it is not already stored
  addTimeNode(cst.getEndEvent()->getTimeNode());

  // set TimeConstraint's clock in external mode
  cst.setDriveMode(Clock::DriveMode::EXTERNAL);
}

void JamomaScenario::removeTimeConstraint(const shared_ptr<TimeConstraint>& timeConstraint)
{
  remove_one(mTimeContraints, timeConstraint);

  // set the TimeConstraint's clock in none external mode
  timeConstraint->setDriveMode(Clock::DriveMode::INTERNAL);
}

void JamomaScenario::addTimeNode(shared_ptr<TimeNode> timeNode)
{
  // store a TimeNode if it is not already stored
  if (!contains(mTimeNodes, timeNode))
  {
    mTimeNodes.push_back(std::move(timeNode));
  }
}

void JamomaScenario::removeTimeNode(const shared_ptr<TimeNode>& timeNode)
{
  remove_one(mTimeNodes, timeNode);
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaScenario::getStartTimeNode() const
{
  return mTimeNodes[0];
}

# pragma mark -
# pragma mark TimeNodes and TimeConstraints

const Container<TimeNode>& JamomaScenario::timeNodes() const
{
  return mTimeNodes;
}

const Container<TimeConstraint>& JamomaScenario::timeConstraints() const
{
  return mTimeContraints;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaScenario::process_offset(shared_ptr<TimeNode> timenode, TimeValue offset)
{
  TimeValue date = timenode->getDate();

  for (const auto& event : timenode->timeEvents())
  {
    TimeEvent::Status eventStatus;

    // evaluate event status considering its time node date
    if (date < offset)
      eventStatus = event->getExpression()->evaluate() ? TimeEvent::Status::HAPPENED : TimeEvent::Status::DISPOSED;
    else if (date == offset)
      eventStatus = TimeEvent::Status::PENDING;
    else
      eventStatus = TimeEvent::Status::NONE;

    // evaluate event status considering previous time constraints
    for (const auto& timeConstraint : event->previousTimeConstraints())
    {
      TimeValue constraintOffset = offset - timeConstraint->getStartEvent()->getTimeNode()->getDate();

      if (constraintOffset < Zero)
      {
        eventStatus = TimeEvent::Status::NONE;
      }
      else if (constraintOffset >= Zero && constraintOffset <= timeConstraint->getDurationMax())
      {
        eventStatus = constraintOffset > timeConstraint->getDurationMin() ? TimeEvent::Status::PENDING : TimeEvent::Status::NONE;
      }
      else if (constraintOffset > timeConstraint->getDurationMax())
      {
        eventStatus = event->getExpression()->evaluate() ? TimeEvent::Status::HAPPENED : TimeEvent::Status::DISPOSED;
      }
    }

    // setup event status
    shared_ptr<JamomaTimeEvent> e = dynamic_pointer_cast<JamomaTimeEvent>(event);
    e->setStatus(eventStatus);

    // add HAPPENED event to offset event list
    if (eventStatus == TimeEvent::Status::HAPPENED)
      mPastEventList.push_back(std::make_pair(date, event));

    // propagate offset processing to setup all TimeEvents
    for (const auto& timeConstraint : event->nextTimeConstraints())
    {
      process_offset(timeConstraint->getEndEvent()->getTimeNode(), offset);
    }
  }
}
}

