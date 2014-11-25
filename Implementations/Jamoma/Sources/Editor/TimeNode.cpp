/*!
 * \file TimeNode.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/TimeNode.h"

#include "TTScore.h"

namespace OSSIA
{
  class TimeNode::Impl
  {
    
  public:
    
    Impl()
    {
      // todo : move this else where ...
      TTFoundationInit("/usr/local/jamoma/");
      TTModularInit("/usr/local/jamoma/");
      TTScoreInit("/usr/local/jamoma/");
    };
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
  };
  
  TimeNode::TimeNode() :
  pimpl(new Impl)
  {}
  
  TimeNode::TimeNode(const TimeNode & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  TimeNode::~TimeNode()
  {
    delete pimpl;
  }
  
  TimeNode& TimeNode::operator= (const TimeNode & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}