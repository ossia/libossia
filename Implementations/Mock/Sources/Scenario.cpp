#include "Editor/Scenario.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeBox.h"
#include <thread>
#include <list>
#include <atomic>
#include <vector>
using namespace OSSIA;
class Scenario::Impl
{/*
	public:
		Impl(Scenario* parent):
			_parent{nullptr},
			_startTimeNode{new TimeNode},
			_endTimeNode{new TimeNode}
		{

		}

		// Cas récursif : mauvaise idée, pascal pourrait faire exploser la pile d'appels
		// Et pas assez d'interruptions.
		void getListOfFollowingRigidStarts(std::list<TimeBox*>& lst,
										   TimeNode& startTimeNodeRecurse)
		{
			for(TimeBox* box : startTimeNodeRecurse.getNextTimeBoxes())
			{
				if(box->isRigid())
				{
					// Ajouter à la liste (opti: mettre direct pt. d'interr ici?)
					lst.push_back(box);

					// Appel récursif sur son évt de fin.
					getListOfFollowingRigidStarts(lst, box->getEndTimeNode());
				}
			}
		}

		void play()
		{
			// Deuxième idée :
			//_parent->getStartTimeNode().play();
			// cf. TimeNode::play()

			// Première idée (bof)
			//0. Exécuter ce qu'il y a dans l'TimeNode ?
			// Algorithme:
			// 1) Faire le graphe des dépendences rigides
			//		(séquence des timebox non souples).
			// std::list<TimeBox*> lst;
			// getListOfFollowingRigidStarts(lst, _parent->getStartTimeNode().getNextTimeBoxes());

			// 2) Ajouter une alarme pour chaque début de timebox non-souple.
			//		(ou un peu avant?)
			// 3) A chaque fois qu'une timebox souple est déclenchée:
			//     recalculer le graphe et mettre en place de nouvelles alarmes

		}


		Scenario* _parent{nullptr};

		std::thread _runThread{};

		std::vector<TimeBox*> _timeBoxes{};
		std::vector<TimeNode*> _timeNodes{};

		// Utiliser shared_ptr
		TimeNode* _startTimeNode{nullptr};
		TimeNode* _endTimeNode{nullptr};

		std::atomic<int> _msTimer{0};*/
};

Scenario::Scenario()//:
//	pimpl(new Impl(this))
{

}

Scenario::Scenario(const Scenario&)
{

}

Scenario::~Scenario()
{

}

Scenario&Scenario::operator=(const Scenario&)
{

}

void Scenario::play() const
{
//	getStartTimeNode().play();
	// Passer en vecteur de threads pour permettre multiples lectures? Plutôt dans TimeBox?
	// pimpl->_runThread = std::thread(&Scenario::Impl::play, pimpl);
}

std::set<TimeBox*> Scenario::getTimeBoxes() const
{/*
	std::set<TimeBox*> st;
	for(TimeBox* bp : pimpl->_timeBoxes) st.insert(bp);
	return st;*/
}

std::set<TimeNode*> Scenario::getTimeNodes() const
{/*
	std::set<TimeNode*> st;
	for(TimeNode* bp : pimpl->_timeNodes) st.insert(bp);
	return st;*/
}

void Scenario::addTimeBox(const TimeBox& t, const TimeNode& startTimeNode)
{/*
	pimpl->_timeBoxes.push_back(&const_cast<TimeBox&>(t));
*/
}

void Scenario::addTimeBox(const TimeBox& t, const TimeNode& startTimeNode, const TimeNode& endTimeNode)
{/*
	pimpl->_timeBoxes.push_back(&const_cast<TimeBox&>(t));
	TimeNode* sev = &const_cast<TimeNode&>(startTimeNode);
	TimeNode* eev = &const_cast<TimeNode&>(endTimeNode);

	if(std::none_of(pimpl->_timeNodes.begin(),
					pimpl->_timeNodes.end(),
					[&] (TimeNode* node) { return node == sev; }))
	{
		pimpl->_timeNodes.push_back(sev);
	}

	if(std::none_of(pimpl->_timeNodes.begin(),
					pimpl->_timeNodes.end(),
					[&] (TimeNode* node) { return node == eev; }))
	{
		pimpl->_timeNodes.push_back(eev);
	}*/
}

TimeNode& Scenario::getStartNode() const
{
//	return *pimpl->_startTimeNode;
}


void Scenario::setStartNode(const TimeNode& ev)
{
	//BAD BAD BAD
//	pimpl->_startTimeNode = &const_cast<TimeNode&>(ev);
}

TimeNode&Scenario::getEndNode() const
{
//	return *pimpl->_endTimeNode;
}

void Scenario::setEndNode(const TimeNode& ev)
{
//	pimpl->_endTimeNode = &const_cast<TimeNode&>(ev);
}
