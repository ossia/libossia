/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief Handles any signal input
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_INPUT_H__
#define __TT_INPUT_H__

#include "TTModularIncludes.h"
#include "TTOutput.h"

/** TTInput ... TODO : an explanation
 
 
 */

class TTOutput;
typedef TTOutput* TTOutputPtr;

class TTMODULAR_EXPORT TTInput : public TTObjectBase
{
	TTCLASS_SETUP(TTInput)
	
public:	// use public for quick acces during signal processing
	
	TTSymbol			mType;						///< ATTRIBUTE : the type of the input signal (like audio, video, ...)
	TTAddress           mOutputAddress;				///< ATTRIBUTE : address of the output to bind
	
	TTBoolean			mMute;						///< ATTRIBUTE : to mute signal
	TTBoolean			mBypass;					///< ATTRIBUTE : to pass signal directly to a TTOutput object
	
	// In the below a "signal" is any kind item in a stream of data that is flowing through this object
	// Sometimes we just use TTValue (for simple numbers)
	// Sometimes we need more complex structures (like audio signal)s
	// When it is just a TTValue we don't use these below -- they are used for e.g. the audio signal case
	
	TTObject            mSignalIn;					///< any data structure to receive complex signal
	
	TTList              mSignalCache;				///< a list of any data structure to deal with others complex signals (like mixing, dubbing, ...)
													///< this is used to receive audio e.g. from many jcom.send~ objects pointing to our object
													///< note that the senders also have object caches that point back to us if appropriate
	
	TTObject            mSignalOut;					///< any data structure to send complex signal
													///< mirrors the input to the model
	
	TTObject            mSignalZero;				///< a zero signal -- used when e.g. muted
	
	TTObject            mOutputObject;				///< TTOutput object to which to forward is we are linked to a TTOutput object
	
private:

	TTObject            mReturnSignalCallback;		///< a way to return back signal to the owner of this input
	TTObject            mAddressObserver;			///< to observe mOutputAddress creation/destruction
    
    TTValue				mSignal;					///< ATTRIBUTE : dummy variable -- unused by the attribute but here because the macro expects it
	TTAttributePtr		mSignalAttr;				///< optimization: cache the pointer to the above attribute for direct access to the
													///< signal preview attribute to notify observers

	
	/** Send signal. The mIndex have to be choosen before 
	 
		If you have a link (see below) then this forwards the received message to the linked output object.
		(not used in the case of the TTInputAudio subclass)
	 
		@param inputValue	message coming into the input, e.g. a "jit_matrix foo" message
		@param outputValue	unused
	 */
	TTErr Send(const TTValue& inputValue, TTValue& outputValue);
	
	/** Set outputObject that is to be linked with this input to create a pair (e.g. for mixing, bypassing, etc.)
		There can only zero or one links from an input/output.
		If you try to create a link without first unlinking the previous link is overwritten.
	 */
	TTErr Link(const TTValue& inputValue, TTValue& outputValue);
	
	/** Remove outputObject */
	TTErr Unlink();
	
	
	/** Set the outputAddress attribute 
		When we set this we observe the namespace to know when an output object is registered or unregistered at this address.
		When this address exists it will automatically link us by calling the Link method (above).
	 */
	TTErr setOutputAddress(const TTValue& value);
    
	
    /** Notify signal observer 
		Messages go to TTReceiver (e.g. j.receive)
	 */
	TTErr notifySignalObserver(const TTValue& value);
	
	friend TTErr TTMODULAR_EXPORT TTInputDirectoryCallback(const TTValue& baton, const TTValue& data);

};

typedef TTInput* TTInputPtr;

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTInputDirectoryCallback(const TTValue& baton, const TTValue& data);


#endif // __TT_INPUT_H__
