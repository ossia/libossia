#pragma once
#include <Editor/TimeConstraint.h>
#include <Editor/TimeNode.h>
#include <Editor/TimeEvent.h>
#include <Editor/Scenario.h>
#include <Editor/Curve.h>
#include <Editor/CurveSegment.h>
#include <Editor/Automation.h>

#include <iostream>
namespace OSSIA
{
namespace Toolkit
{
class DebugVisitor
{
        int depth = 0;
    public:
        std::string space() const
        {
            return std::string(depth, ' ');
        }

        void visit(std::shared_ptr<OSSIA::TimeConstraint> cst)
        {
            std::cerr << space() << "TimeConstraint " << double(cst->getDuration()) << std::endl;
            depth++;
            for(const auto& proc : cst->timeProcesses())
            {
                if(auto scenar = std::dynamic_pointer_cast<OSSIA::Scenario>(proc))
                    visit(scenar);
                else if(auto autom = std::dynamic_pointer_cast<OSSIA::Automation>(proc))
                    visit(autom);
            }

            visit(cst->getEndEvent());
            depth--;
        }


        void visit(std::shared_ptr<OSSIA::TimeEvent> ev)
        {
            std::cerr << space() << "TimeEvent" << std::endl;
            depth++;
            for(const auto& cst : ev->nextTimeConstraints())
            {
                visit(cst);
            }
            depth--;
        }

        void visit(std::shared_ptr<OSSIA::TimeNode> tn)
        {
            std::cerr << space() << "TimeNode" << std::endl;
            depth++;
            for(const auto& event : tn->timeEvents())
            {
                visit(event);
            }
            depth--;
        }
        void visit(std::shared_ptr<OSSIA::Scenario> scenar)
        {
            std::cerr << space() << "Scenario" << std::endl;
            depth++;
            visit(scenar->getStartTimeNode());
            depth--;
        }

        void visit(std::shared_ptr<OSSIA::Automation> autom)
        {
            std::cerr << space() << "Automation" << std::endl;
            auto d = autom->getDriving();
            if(auto b = dynamic_cast<const OSSIA::Behavior*>(d))
            {
                depth++;
                visit(b->value);
                depth--;
            }
        }

        void visit(std::shared_ptr<OSSIA::CurveAbstract> curve)
        {
            /*
            if(auto c = dynamic_cast<OSSIA::Curve<float>*>(curve.get()))
            {
                std::cerr << space() << "Curve " << c->getPointsMap().size() <<  std::endl;
                depth++;
                for(const auto& seg : c->getPointsMap())
                {
                    visit(seg.second.second);
                }
                depth--;
            }
            */
        }

        void visit(std::shared_ptr<OSSIA::CurveSegment<float>> curve)
        {
            std::cerr << space() << "CurveSegment" << std::endl;
        }
};

}
}
