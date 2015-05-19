//*
#include "../Sources/Editor/Scenario.cpp"
#include "../Sources/Editor/TimeConstraint.cpp"
#include "../Sources/Editor/TimeNode.cpp"
#include "../Sources/Editor/TimeEvent.cpp"
#include "../Sources/Editor/TimeValue.cpp"
//*/
/*
#include "Scenario.h"
#include "TimeConstraint.h"
#include "TimeEvent.h"
#include "TimeNode.h"
#include "TimeValue.h"
//*/

#include <memory>
//#include <QDebug>

int main()
{
    auto scenar = Scenario::create();
    auto cstr = TimeConstraint::create(TimeValue(), TimeValue(), TimeValue());

    auto startNode = TimeNode::create();
    scenar->setStartNode(startNode);
    // COMPARE( scenar->getStartNode(), startNode ) ;

    scenar->addConstraint(*cstr, *(scenar->getStartNode()));
    // COMPARE( cstr->getStartNode(), startNode );

    auto endNode = TimeNode::create();

    scenar->addConstraint(*cstr, *scenar->getStartNode(), *endNode);

    // COMPARE( cstr->getStartNode(), scenar->getStartNode() );
    // COMPARE( cstr->getEndNode(), tn ) ;

    scenar->setEndNode(endNode);
    // COMPARE( scenar->getEndNode(), tn ) ;

    scenar->setKiller(true);
    // COMPARE( scenar->isKiller(), true );
    scenar->play();

    return 0;
}
//*/
