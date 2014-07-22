/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Preset Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_PRESET_H__
#define __TT_PRESET_H__

#include "TTModularIncludes.h"

class TTScript;
typedef TTScript* TTScriptPtr;

/**	TTPreset ... TODO : an explanation
 
 
 */

class TTMODULAR_EXPORT TTPreset : public TTObjectBase
{
	TTCLASS_SETUP(TTPreset)
	
private :
	
	TTSymbol					mName;							///< ATTRIBUTE : the name of the preset
    TTSymbol					mDescription;                   ///< ATTRIBUTE : the description of the preset
	TTAddress                   mAddress;						///< ATTRIBUTE : the parent address from where to search object to store
	
	TTNodeDirectoryPtr			mDirectory;						///< a preset depends on a directory
	TTObject					mScript;						///< a script containing relativeAddress and value
	
	/** */
	TTErr	setAddress(const TTValue& value);
	
	/** */
	TTErr	Clear();
	
	/** */
	TTErr	Store();
    
    /** Update the whole preset */
    TTErr   Update();
    
	/** */
	TTErr	Recall(const TTValue& inputValue, TTValue& outputValue);
    
    /** Output the preset from an address (default : kAdrsRoot to output all the preset) */
	TTErr	Output(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTXmlHandler */
	TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTTextHandler */
	TTErr	WriteAsText(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromText(const TTValue& inputValue, TTValue& outputValue);
    
    /** a method to update each Data value */
    TTErr   processUpdate(TTObject& aScript);
	
	friend TTErr TTMODULAR_EXPORT TTPresetInterpolate(TTObject preset1, TTObject preset2, TTFloat64 position);
	friend TTErr TTMODULAR_EXPORT TTPresetMix(const TTValue& presets, const TTValue& factors);
	friend TTErr TTMODULAR_EXPORT TTPresetCopy(TTObject aPresetToCopy, TTObject aPresetCopy);
};

typedef TTPreset* TTPresetPtr;

/** Return Yes if the node have to be part of the preset */
TTBoolean	TTMODULAR_EXPORT TTPresetTestObject(TTNodePtr node, TTPtr args);

TTErr		TTMODULAR_EXPORT TTPresetInterpolate(TTObject preset1, TTObject preset2, TTFloat64 position);

TTErr		TTMODULAR_EXPORT TTPresetMix(const TTValue& presets, const TTValue& factors);

TTErr		TTMODULAR_EXPORT TTPresetCopy(TTObject aPresetToCopy, TTObject aPresetCopy);

#endif // __TT_PRESET_H__
