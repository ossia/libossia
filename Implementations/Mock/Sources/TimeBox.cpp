#include <TimeBox.h>
#include <TimeValue.h>
#include <Event.h>
#include <vector>
using namespace OSSIA;

class TimeBox::Impl
{
	public:
		Impl():
			_startEvent{new Event},
			_endEvent{new Event}
		{

		}
		std::vector<TimeProcess*> _processes;
		Event* _startEvent{nullptr};
		Event* _endEvent{nullptr};
};
class TimeBox::const_iterator
{

};

TimeBox::TimeBox():
	pimpl{new Impl}
{

}

TimeBox::~TimeBox()
{

}

TimeBox::const_iterator TimeBox::begin() const
{
//	return pimpl->_processes.cbegin();
}

TimeBox::const_iterator TimeBox::end() const
{
//	return pimpl->_processes.cend();
}

void TimeBox::play(bool log, std::string name) const
{
//	for(TimeProcess* p : *this)
	{

	}
}

Event&TimeBox::getStartEvent() const
{
	return *pimpl->_startEvent;
}

Event&TimeBox::getEndEvent() const
{
	return *pimpl->_endEvent;
}

void TimeBox::setStartEvent(Event&& ev)
{
	pimpl->_startEvent = new Event(ev);
}

void TimeBox::setEndEvent(Event&& ev)
{
	pimpl->_endEvent = new Event(ev);
}

void TimeBox::setStartEvent(const Event& ev)
{
	pimpl->_startEvent = &const_cast<Event&>(ev);
}

void TimeBox::setEndEvent(const Event&ev)
{
	pimpl->_endEvent = &const_cast<Event&>(ev);
}

TimeValue TimeBox::getLength() const
{
	return pimpl->_endEvent->getDate() - pimpl->_startEvent->getDate();
}

bool TimeBox::isRigid()
{
	return true;
}

