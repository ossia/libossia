/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Cue Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_CUE_H__
#define __TT_CUE_H__

#include "TTModularIncludes.h"

class TTScript;
typedef TTScript* TTScriptPtr;

/**	TTCue ... TODO : an explanation
 
 
 */

class TTMODULAR_EXPORT TTCue : public TTObjectBase
{
	TTCLASS_SETUP(TTCue)
	
	private :
	
	TTSymbol					mName;							///< ATTRIBUTE : the name of the cue
	TTSymbol					mDescription;					///< ATTRIBUTE : a description for the cue
    TTValue                     mTags;							///< ATTRIBUTE : tag list for the preset
	TTUInt32					mRamp;							///< ATTRIBUTE : a time ramping value for the cue
    TTAddress                   mAddress;						///< ATTRIBUTE : the address from where to manage the script
	TTObject					mScript;						///< a script containing relativeAddress and value
	
	/** */
	TTErr	getName(TTValue& value);
	TTErr	setName(const TTValue& value);
	
	/** */
	TTErr	getDescription(TTValue& value);
	TTErr	setDescription(const TTValue& value);
	
	/** */
	TTErr	getRamp(TTValue& value);
	TTErr	setRamp(const TTValue& value);
    
    /** */
    TTErr   setAddress(const TTValue& value);
	
	/** */
	TTErr	Clear();
	
	/** Fill the cue on a namespace using namespace pointer or name */
	TTErr	Store(const TTValue& inputValue, TTValue& outputValue);
    
    /** Update the cue from an address (default : kAdrsRoot to update all the cue) */
    TTErr   Update(const TTValue& inputValue, TTValue& outputValue);
    
    /** Fill the cue line by line */
	TTErr	Append(const TTValue& inputValue, TTValue& outputValue);
	
	/** Run the cue from an address (default : kAdrsRoot to recall all the cue) */
	TTErr	Recall(const TTValue& inputValue, TTValue& outputValue);
	
	/** Output the cue from an address (default : kAdrsRoot to output all the cue) */
	TTErr	Output(const TTValue& inputValue, TTValue& outputValue);
	
	/** Edit selection on a namespace using namespace pointer or name */
	TTErr	Select(const TTValue& inputValue, TTValue& outputValue);
    
	/**  needed to be handled by a TTXmlHandler */
	TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTTextHandler */
	TTErr	WriteAsText(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromText(const TTValue& inputValue, TTValue& outputValue);
	
	/** a recursive method to store a namespace into a script object */
	TTErr	processStore(TTObject aScript, const TTAddressItemPtr aSelection, TTNodePtr nodeToProcess);
    
    /** a method to update each Data value */
    TTErr   processUpdate(TTObject aScript);
	
	/** a recursive method to process a namespace selection from a script object */
	TTErr	processSelect(TTObject aScript, TTAddressItemPtr aSelection);
	
	/** a method to change each ramping value into a script */
	TTErr	processRamp(TTObject aScript, TTUInt32 ramp);
	
	/** a recursive method to read the ramp value from a script */
	TTErr	searchRamp(TTObject aScript, TTUInt32& ramp);
	
	friend TTErr TTMODULAR_EXPORT TTCueInterpolate(TTObject cue1, TTObject cue2, TTFloat64 position);
	friend TTErr TTMODULAR_EXPORT TTCueMix(const TTValue& cues, const TTValue& factors);
	friend TTErr TTMODULAR_EXPORT TTCueMerge(TTObject aCueToMerge, TTObject mergedCue);
	friend TTErr TTMODULAR_EXPORT TTCueOptimize(TTObject aCueToOptimize, TTObject aCue, TTObject optimizedCue);
	friend TTErr TTMODULAR_EXPORT TTCueCopy(TTObject aCueToCopy, TTObject aCueCopy);
};

typedef TTCue* TTCuePtr;

TTErr       TTMODULAR_EXPORT TTCueInterpolate(TTObject cue1, TTObject cue2, TTFloat64 position);

TTErr		TTMODULAR_EXPORT TTCueMix(const TTValue& cues, const TTValue& factors);

TTErr		TTMODULAR_EXPORT TTCueMerge(TTObject aCueToMerge, TTObject mergedCue);

TTErr		TTMODULAR_EXPORT TTCueOptimize(TTObject aCueToOptimize, TTObject aCue, TTObject optimizedCue);

TTErr		TTMODULAR_EXPORT TTCueCopy(TTObject aCueToCopy, TTObject aCueCopy);

#endif // __TT_CUE_H__
