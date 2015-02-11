#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Scenario.h"
using namespace OSSIA;
/*
class TimeNode::Impl {
	public:
		TimeValue _date;
};
*/
TimeNode::TimeNode()//:
//	pimpl(new Impl)
{

}

TimeNode::~TimeNode()
{

}

Scenario& TimeNode::getParentScenario() const
{
	Scenario* s = new Scenario;
	return *s;
}

TimeValue TimeNode::getDate() const
{
	TimeValue v;
	return v;
//	return pimpl->_date;
}
/*
void TimeNode::setDate(TimeValue v)
{
//	pimpl->_date = v;
}
*/
