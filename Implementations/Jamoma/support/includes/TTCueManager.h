/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A CueManager Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_CUE_MANAGER_H__
#define __TT_CUE_MANAGER_H__

#include "TTModularIncludes.h"

/**	TTCueManager ... TODO : an explanation
 
 
 */

class TTCue;
typedef TTCue* TTCuePtr;

class TTXmlHandler;
typedef TTXmlHandler* TTXmlHandlerPtr;

// a namespace is a TTList
typedef	TTList* NamespacePtr;

class TTMODULAR_EXPORT TTCueManager : public TTObjectBase
{
	TTCLASS_SETUP(TTCueManager)
	
private:
    
	TTValue				mNames;							///< ATTRIBUTE : cues are ordered by name
	TTSymbol			mCurrent;						///< ATTRIBUTE : the current cue name
	TTInt32				mCurrentPosition;				///< ATTRIBUTE : the current cue position
	TTSymbol			mNamespace;						///< ATTRIBUTE : the name of the namespace selection to use
    TTAddress           mAddress;						///< ATTRIBUTE : an address to make the mangment relative to
	TTHash              mCues;							///< ATTRIBUTE : a hash table containing <name, #TTCue>
	
	TTObject			mCurrentCue;					///< the current cue
	TTAddressItemPtr    mDefaultNamespace;				///< an internal default namespace
	
	TTObject            mReturnLineCallback;			///< Callback to return back cue lines to the owner of this cuemanager
    
    TTSymbol			mLastCurrent;                   ///< ATTRIBUTE : remember the current cue when parsing a file
	
	/** */
	TTErr	getCurrentDescription(TTValue& value);
	TTErr	setCurrentDescription(const TTValue& value);
	
	/** */
	TTErr	getCurrentRamp(TTValue& value);
	TTErr	setCurrentRamp(const TTValue& value);
    
    /** */
    TTErr   setAddress(const TTValue& value);
	
	/** Select a set of absolute addresses
	 @param inputValue      a set of absolute addresses
	 @param outputValue     nothing
     @return                kTTErrNone */
	TTErr	NamespaceSelect(const TTValue& inputValue, TTValue& outputValue);
	
	/** Unselect a set of absolute addresses
	 @param inputValue      a set of absolute addresses
	 @param outputValue     nothing
     @return                kTTErrNone */
	TTErr	NamespaceUnselect(const TTValue& inputValue, TTValue& outputValue);
	
	/** Select only the addresses which are in a cue
	 @param inputValue      name of a cue or nothing (use the current cue)
	 @param outputValue     nothing
     @return                kTTErrNone */
	TTErr	NamespaceGrab(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	Clear();
	
	/** New a cue :
		name/id : create a new cue.
		nothing : store into the current cue */
	TTErr	New(const TTValue& inputValue, TTValue& outputValue);
    
    /** Update a cue :
     name/id : update an existing cue.
     nothing : update the current cue */
	TTErr	Update(const TTValue& inputValue, TTValue& outputValue);
    
    /** Append a line to a cue :
        name/id line : append line to the cue */
    TTErr   Append(const TTValue& inputValue, TTValue& outputValue);

	/** Recall a cue : 
		name/id : recall the cue.
		nothing : recall the current cue */
	TTErr	Recall(const TTValue& inputValue, TTValue& outputValue);
	
	/** Output a cue using the mReturnLineCallback : 
	 name/id : output the cue.
	 nothing : output the current cue */
	TTErr	Output(const TTValue& inputValue, TTValue& outputValue);
	
	/** Interpolate 2 cues : 
		name1, name2, position : interpolate between the 2 given cues
		TODO : name1, position : interpolate between the current cue and the given cue */
	TTErr	Interpolate(const TTValue& inputValue, TTValue& outputValue);
	
	/** Mix several cues : 
		name1, factor1, name2, factor2, ... : mix each given cues */
	TTErr	Mix(const TTValue& inputValue, TTValue& outputValue);
	
	/** Move a cue : 
	 name + position : move the cue to the given position. */
	TTErr	Move(const TTValue& inputValue, TTValue& outputValue);
	
	/** Delete a cue :
		name : remove the cue.
		nothing : remove the current cue */
	TTErr	Delete(const TTValue& inputValue, TTValue& outputValue);
    
    /** Reorder the list */
	TTErr	Order(const TTValue& inputValue, TTValue& outputValue);
	
	/** Rename a cue : 
	 name + newName: rename the cue with the newName */
	TTErr	Rename(const TTValue& inputValue, TTValue& outputValue);
	
	/** Copy a cue : 
	 name : copy the cue (and optionally give a new name + a position)  */
	TTErr	Copy(const TTValue& inputValue, TTValue& outputValue);
	
	/** Optimize a sub set of cues clearing redundant command lines :
		name1, name2, name3, ... : make the optimization between all given cues. */
	TTErr	Optimize(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTXmlHandler */
	TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTTextHandler */
	TTErr	WriteAsText(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromText(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	notifyNamesObservers();
    
    /** */
	TTAddressItemPtr getNamespace();
};

typedef TTCueManager* TTCueManagerPtr;

#endif // __TT_CUE_MANAGER_H__
