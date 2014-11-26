/*!
 * \file State_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "State_Impl.h"

namespace OSSIA
{
    State* State::create()
    {
        // todo : we shouldn't init each time we create an object ...
        TTFoundationInit("/usr/local/jamoma/");
        TTModularInit("/usr/local/jamoma/");
        TTScoreInit("/usr/local/jamoma/");
        
        return new State_Impl();
    }
    
    State_Impl::State_Impl() :
    mScript("Script"),
    mRunning(NO)
    {}
    
    State_Impl::~State_Impl()
    {}
    
    void State_Impl::launch() const
    {
        if (!mRunning)
        {
            mRunning = YES;
            
            mScript.send(kTTSym_Run);
            
            mRunning = NO;
        }
    }
    
    class State::const_iterator
    {};
    
    State::const_iterator State_Impl::begin() const
    {}
    
    State::const_iterator State_Impl::end() const
    {}
    
    State::const_iterator State_Impl::find(const StateElement&) const
    {}
    
    // Managing StateElements
    void State_Impl::addStateElement(const StateElement& stateElement)
    {
        if (stateElement.getType() != StateElement::StateElementType::MESSAGE_TYPE)
            return;
        
        Message     message = stateElement;
        TTAddress   address = message.getAddress();
        TTValuePtr  value = message.getValue();
        
        // get the lines of the state
        TTValue v;
        mScript.get("flattenedLines", v);
        TTListPtr flattenedLines = TTListPtr(TTPtr(v[0]));
        
        // find the line at address
        TTErr err = flattenedLines->find(&TTScriptFindAddress, (TTPtr)&address, v);
        
        // the line doesn't exist : append it to the state
        if (err)
        {
            TTValue command = *value;
            command.prepend(address);
            
            mScript.send("AppendCommand", command);
        }
        // the line exists : replace the value
        else
        {
            TTDictionaryBasePtr aLine = TTDictionaryBasePtr((TTPtr)v[0]);
            aLine->setValue(*value);
        }
    }
    
    bool State_Impl::removeStateElement(const StateElement&)
    {
        return false;
    }
}