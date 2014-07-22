/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Script Object
 *
 * @details TODO: Add more info
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_SCRIPT_H__
#define __TT_SCRIPT_H__

#include "TTModularIncludes.h"


/** About line schemas :
 
	kTTSym_command : 
		address : a TTNodeAddress
		value : the value to send
		unit : the unit to use
		ramp : a ramp time
 
	kTTSym_comment : 
		value : the comment
 
	kTTSym_flag : 
		name : the name of the flag
		value : a TTvalue containing any extra info relative to this flag
 
 */

class TTMODULAR_EXPORT TTScript : public TTObjectBase
{
	TTCLASS_SETUP(TTScript)
	
private:
	
    TTBoolean           mFlattened;             ///< ATTRIBUTE : is the script ready for quick operation process or not ?
    // TODO JamomaCore #305 : TTList : allow to register as none pointer members
	TTListPtr           mLines;					///< a linked list containing all lines of the script
    // TODO JamomaCore #305 : TTList : allow to register as none pointer members
    TTListPtr           mFlattenedLines;		///< a linked list containing all lines of the script and the subscripts flattened for quick access
	
	TTObject            mSubScript;				///< the current sub script to manage
	TTObject            mParentScript;			///< the current parent script to manage (usefull for ReadFrom method)
	
	TTObject            mReturnLineCallback;	///< a callback to return back lines to the owner of this script
	
    
    // get the lines list
    TTErr   getLines(TTValue& value);
    TTErr   getFlattenedLines(TTValue& value);
    
	/**	Clear all lines of the script	*/
	TTErr	Clear();
    
    /**	Process all command lines of the script and the subscripts to cache all lines into a Flattened quick access link list
        Each line binds also to the node in the directory in order to increase the recall, interpolation, mix.
        However this is dangerous because if the node dispeared, their will be a BAD_ACCESS.
        This is usefull to increase the speed of any operation */
	TTErr	Flatten(const TTValue& inputValue, TTValue& outputValue);
    TTErr	Unflatten();
	
	/**	Run all command lines of the script (or the Flattened lines if ready) */
	TTErr	Run(const TTValue& inputValue, TTValue& outputValue);
    TTErr   RunFlattened();
    
    /**	Run one command of the script */
	TTErr	RunCommand(const TTValue& inputValue, TTValue& outputValue);
    
    /**	Remove one command of the script */
	TTErr	RemoveCommand(const TTValue& inputValue, TTValue& outputValue);
	
	/**	Dump all lines of the script using mReturnLineCallback (or the Flattened lines if ready) */
	TTErr	Dump(const TTValue& inputValue, TTValue& outputValue);
    TTErr	DumpFlattened();
    
    /**	Dump one line of the script using mReturnLineCallback */
	TTErr	DumpLine(const TTValue& inputValue, TTValue& outputValue);
	
	/**	Append any line to the script (a parsing will find which kind of line it is)	*/
	TTErr	Append(const TTValue& newLine, TTValue& outputValue);
	
	/**	Append a command line to the script and output the dictionnary associated to the parsed line
		<address, value, unit, ramp, ...>	*/
	TTErr	AppendCommand(const TTValue& newCommand, TTValue& outputValue);
	
	/**	Append a comment line to the script and output the dictionnary associated to the parsed line
		<comment>	*/
	TTErr	AppendComment(const TTValue& newComment, TTValue& outputValue);
	
	/**	Append a flag line to the script and output the dictionnary associated to the parsed line
		<name, arg1, arg2, ...>	*/
	TTErr	AppendFlag(const TTValue& newflagAndArguments, TTValue& outputValue);
	
	/**	Append a sub script */
	TTErr	AppendScript(const TTValue& newScript, TTValue& outputValue);
	
	/**  needed to be handled by a TTXmlHandler	*/
	TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
	
	/**  needed to be handled by a TTTextHandler	*/
	TTErr	WriteAsText(const TTValue& inputValue, TTValue& outputValue);
	TTErr	ReadFromText(const TTValue& inputValue, TTValue& outputValue);
	
	friend	TTErr TTMODULAR_EXPORT TTScriptInterpolate(TTObject script1, TTObject script2, TTFloat64 position);
	friend	TTErr TTMODULAR_EXPORT TTScriptMix(const TTValue& scripts, const TTValue& factors);
	friend	TTErr TTMODULAR_EXPORT TTScriptMerge(TTObject scriptToMerge, TTObject mergedScript);
	friend	TTErr TTMODULAR_EXPORT TTScriptOptimize(TTObject aScriptToOptimize, TTObject aScript, TTObject optimizedScript);
	friend	TTErr TTMODULAR_EXPORT TTScriptCopy(TTObject scriptTocopy, TTObject aScriptCopy);
};

typedef TTScript* TTScriptPtr;


/* Parse a value into a comment, a flag or a command line.
   Returns NULL in case of error */
TTDictionaryBasePtr TTMODULAR_EXPORT TTScriptParseLine(const TTValue& newLine);

/* Parse a value into a comment line 
   Returns NULL in case of error */
TTDictionaryBasePtr TTMODULAR_EXPORT TTScriptParseComment(const TTValue& newComment);

/* Parse a value into a flag line 
   Returns NULL in case of error */
TTDictionaryBasePtr TTMODULAR_EXPORT TTScriptParseFlag(const TTValue& newflagAndArguments);

/* Parse a value into a command line. 
   Returns NULL in case of error */
TTDictionaryBasePtr TTMODULAR_EXPORT TTScriptParseCommand(const TTValue& newCommand);

/* Parse a value into a sub script line and optionnaly pass a callback to output
   Returns NULL in case of error */
TTDictionaryBasePtr TTMODULAR_EXPORT TTScriptParseScript(const TTValue& newScript);

/* Interpolate between two scripts
   note : we assume that the Bind method have been called before on the two scripts */
TTErr			TTMODULAR_EXPORT TTScriptInterpolate(TTObject script1, TTObject script2, TTFloat64 position);

/* Mix several scripts together
   note : we assume that the Bind method have been called before on each scripts */
TTErr			TTMODULAR_EXPORT TTScriptMix(const TTValue& scripts, const TTValue& factors);
TTFloat64		TTMODULAR_EXPORT TTScriptMixLine(TTDictionaryBasePtr lineToMix, TTSymbol dataType, TTUInt32 mixSize, TTFloat64 factor, TTValue& mixedValue, TTBoolean init=NO);

/* Merge a script into another without redundant command lines */
TTErr			TTMODULAR_EXPORT TTScriptMerge(TTObject scriptToMerge, TTObject mergedScript);

/* Optimize a script comparing to another to remove redundant command lines */
TTErr			TTMODULAR_EXPORT TTScriptOptimize(TTObject aScriptToOptimize, TTObject aScript, TTObject optimizedScript);

/* Copy a script */
TTErr			TTMODULAR_EXPORT TTScriptCopy(TTObject scriptTocopy, TTObject aScriptCopy);

/* a TTFunctionMatch to find a line in the script depending on the address */
void			TTMODULAR_EXPORT TTScriptFindAddress(const TTValue& lineValue, TTPtr addressPtrToMatch, TTBoolean& found);

/* a TTFunctionMatch to find a line in the script depending on the target address (in mFlattenedLines only) */
void            TTMODULAR_EXPORT TTScriptFindTarget(const TTValue& lineValue, TTPtr addressPtrToMatch, TTBoolean& found);

/* Copy a line */
TTDictionaryBasePtr	TTMODULAR_EXPORT TTScriptCopyLine(TTDictionaryBasePtr lineTocopy);


#endif // __TT_SCRIPT_H__
