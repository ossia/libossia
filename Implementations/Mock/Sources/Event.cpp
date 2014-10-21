#include <Event.h>
#include <TimeBox.h>
#include <TimeValue.h>
#include <vector>
#include <algorithm>
#include "later.h"
using namespace OSSIA;
Event::Event()
{

}

Event::~Event()
{

}

std::set<TimeBox*> Event::getPreviousTimeBoxes() const
{
	return {};
}

std::set<TimeBox*> Event::getNextTimeBoxes() const
{
	return {};
}

Event::Event(const Event&)
{

}

void Event::play(bool log, std::string name) const
{
	// log et name sont là pour quoi ?
	std::vector<std::pair<Event*, TimeValue>> nextevents;
	for(TimeBox* box : getNextTimeBoxes())
	{
		// On suppose que endEvent des suivants sont les startEvent des suivants d'après
		if(box->isRigid())
		{
			nextevents.emplace_back(&box->getEndEvent(), box->getLength());
		}

		// Faut quand même bien les démarrer à un moment ou un autre
		box->play();
	}

	// Supprimer doublons. Ne pas faire si on désire mettre un event dépendant de la fin
	// de deux TimeBoxes. -> synchronisation
	std::sort(nextevents.begin(), nextevents.end(), [] (auto& lh, auto& rh)
		{ return lh.first < rh.first; });
	std::unique(nextevents.begin(), nextevents.end(),[] (auto& lh, auto& rh)
		{ return lh.first == rh.first; });

	for(auto& evts : nextevents)
	{
		Later<> do_later(evts.second,
						 true,
						 &Event::play, evts.first, false, std::string());
	}
}
