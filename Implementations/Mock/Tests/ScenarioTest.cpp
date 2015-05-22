#include "../Sources/Editor/Scenario.cpp"
#include "../Sources/Editor/TimeConstraint.cpp"
#include "../Sources/Editor/TimeNode.cpp"
#include "../Sources/Editor/TimeEvent.cpp"
#include "../Sources/Editor/TimeValue.cpp"

#include <QTest>
#include <memory>
//#include <QDebug>

class ScenarioTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void scenarioTest()
    {
        auto scenar = Scenario::create();
        TimeValue t;
        auto cstr = TimeConstraint::create(t,t,t);

        auto startNode = TimeNode::create();
        scenar->setStartNode(startNode);
        QCOMPARE( scenar->getStartNode(), startNode ) ;

        scenar->addConstraint(*cstr, *(scenar->getStartNode()));
        //QCOMPARE( cstr->getStartNode(), startNode );

        auto endNode = TimeNode::create();

        scenar->addConstraint(*cstr, *scenar->getStartNode(), *endNode);

        //QCOMPARE( cstr->getStartNode(), scenar->getStartNode() );
        //QCOMPARE( cstr->getEndNode(), endNode ) ;

        scenar->setEndNode(endNode);
        QCOMPARE( scenar->getEndNode(), endNode ) ;

        scenar->setKiller(true);
        QCOMPARE( scenar->isKiller(), true );
        scenar->play();

    }
};
//*/
QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
