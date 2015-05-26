#include "Editor/State.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaState : public State
{
  
private:
  
  // Implementation specific
  shared_ptr<StateElement> element;
  //TTList mLines; // code relative to old state managment
  
public:
  // Constructors, destructor, cloning
  JamomaState()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaState(const JamomaState * other)
  {}
  
  virtual ~JamomaState()
  {}
  
  virtual shared_ptr<State> clone() const override
  {
    return shared_ptr<State>(new JamomaState(this));
  }

  // Lecture
  virtual void launch() const override
  {
/* code relative to old state managment
 
    for (mLines.begin(); mLines.end(); mLines.next())
    {
      TTDictionary line = mLines.current()[0];
      
      // depending on the schema
      if (line.getSchema() == kTTSym_command)
      {
        // retreive the object under the address
        TTAddress address;
        TTValue   v;
        
        line.lookup(kTTSym_address, v);
        address = v[0];
        
        TTNodeDirectoryPtr directory = accessApplicationDirectoryFrom(address);
        if (directory == NULL)
          continue;
        
        TTNodePtr node;
        TTErr err = directory->getTTNode(address, &node);
        
        if (!err)
        {
          TTObject object = node->getObject();
          
          if (object.valid())
          {
            // for data object
            if (object.name() == kTTSym_Data)
            {
              // send the line using the command message
              if (address.getAttribute() == kTTSym_value)
              {
                object.send(kTTSym_Command, line); // TODO : TTData::Command should handles TTDictionary
                continue;
              }
            }
            
            // any other case : set attribute
            line.getValue(v);
            object.set(address.getAttribute(), v);
          }
        }
      }
      else if (line.getSchema() == kTTSym_state)
      {
        State*  state;
        TTValue v;
        
        line.lookup(kTTSym_state, v);
        state = TTPtr(v[0]);
        
        state.launch();
      }
    }
 */
  }
    
/* code relative to old state managment
 
  virtual iterator insert(const_iterator where, const shared_ptr<StateElement> & what) override
  {

 
    if (stateElement.getType() == StateElement::StateElementType::MESSAGE_TYPE)
    {
      Message       message = stateElement;
      TTAddress     address = message.getAddress();
      TTValue       value = message.getValue();
      TTDictionary  line;
      
      line.setSchema(kTTSym_command);
      line.setValue(value);
      
      // default attribute is value attribute
      if (address.getAttribute() == kTTSymEmpty)
        address.appendAttribute(kTTSym_value);
      
      line.append(kTTSym_address, address);
      
      mLines.append(line);
    }
    else if (stateElement.getType() == StateElement::StateElementType::STATE_TYPE)
    {
      State* state = State::create();
      
      for (State::const_iterator it = stateElement.begin(); it != stateElement.end(); it++)
        state.addStateElement(*it);
      
      TTDictionary line;
      line.setSchema(kTTSym_state);
      line.setValue((TTPtr)state);
      
      mLines.append(line);
    }

    return iterator();
  }
*/
};

shared_ptr<State> State::create()
{
  return shared_ptr<State>(new JamomaState);
}

/* code relative to old state managment
 
void State_ImplFindAddress(const TTValue& lineValue, TTPtr addressPtrToMatch, TTBoolean& found)
{
  TTDictionary  line = lineValue[0];
  TTAddress     address;
  TTValue       v;
  
  line.lookup(kTTSym_address, v);
  address = v[0];
  
  found = address == *((TTAddress*)addressPtrToMatch);
}
*/