/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A PresetManager Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_PRESET_MANAGER_H__
#define __TT_PRESET_MANAGER_H__

#include "TTModularIncludes.h"

/**	TTPresetManager ... TODO : an explanation
 
 
 */

class TTPreset;
typedef TTPreset* TTPresetPtr;

class TTXmlHandler;
typedef TTXmlHandler* TTXmlHandlerPtr;

class TTMODULAR_EXPORT TTPresetManager : public TTObjectBase
{
	TTCLASS_SETUP(TTPresetManager)
	
private:
	
	TTAddress           mAddress;						///< ATTRIBUTE : the container address to manage
	TTValue				mNames;							///< ATTRIBUTE : presets are ordered by name
	TTSymbol			mCurrent;						///< ATTRIBUTE : the current preset name
	TTUInt32			mCurrentPosition;				///< ATTRIBUTE : the current cue position

	TTHash              mPresets;						///< a hash table containing <name, TTPresetPtr>
	TTObject			mCurrentPreset;					///< the current preset
    
    TTObject            mReturnLineCallback;			///< Callback to return back cue lines to the owner of this presetmanager
	
	/** */
	TTErr	setAddress(const TTValue& value);
    
    /** */
    TTErr   getValue(TTValue& value);
    
    /** */
    TTErr   setValue(const TTValue& value);

	/** */
	TTErr	Clear();
	
	/** */
	TTErr	New(const TTValue& inputValue, TTValue& outputValue);
    
    /** */
    TTErr   Update(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	Recall(const TTValue& inputValue, TTValue& outputValue);
    
	/** Output a preset using the mReturnLineCallback :
	 name/id : output the preset.
	 nothing : output the current preset */
	TTErr	Output(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	Interpolate(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	Mix(const TTValue& inputValue, TTValue& outputValue);
	
	/** Move a preset : 
	 name + position : move the preset to the given position. */
	TTErr	Move(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr	Delete(const TTValue& inputValue, TTValue& outputValue);
    
    /** Reorder the preset list*/
	TTErr	Order(const TTValue& inputValue, TTValue& outputValue);
	
	/** Rename a preset : 
	 name + newName: rename the preset with the newName */
	TTErr	Rename(const TTValue& inputValue, TTValue& outputValue);
	
	/** Copy a preset : 
	 name : copy the preset (and optionally give a new name + a position) */
	TTErr	Copy(const TTValue& inputValue, TTValue& outputValue);	
	
	/**  needed to be handled by a TTXmlHandler */
	TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTTextHandler */
	TTErr	WriteAsText(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromText(const TTValue& inputValue, TTValue& outputValue);
	
    /** */
	TTErr	notifyValueObservers();
    
	/** */
	TTErr	notifyNamesObservers();
};

typedef TTPresetManager* TTPresetManagerPtr;

#endif // __TT_PRESET_MANAGER_H__
