/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief #TTBuffer manages the check-in/out of #TTSampleMatrix pointers.
 *
 * @details #TTBuffer acts as a librarian for checking-in and checking-out audio within a chunk of memory. Any object that wishes to read or write samples managed by #TTBuffer must first check-out a pointer to the current #TTSampleMatrix. Once the pointer has been obtained, the object can read and write audio samples by working directly with the corresponding #TTSampleMatrix functions. Upon completion of its work, the object is then responsible for checking-in the pointer back to the #TTBuffer.@n@n
 * This extra layer of protection prevents problems that can occur during such operations as changing the length in samples, changing number of channels, filling the buffer or loading a sound file. Whenever these key changes are made through #TTBuffer, it first creates a new #TTSampleMatrix and applies changes there. The new #TTSampleMatrix only becomes available for check-out after the changes are completed.@n@n
 * The advantage of this approach is that during such changes, objects that have checked-out a #TTSampleMatrix pointer can continue using these samples without fear of them being changed before being ready. Only once the object checks-in the current pointer and performs another check-out will it begin using the new set of samples. This can be advantageous in applications such as sampling, wavetables or granular processing.
 * 
 * @see TTSampleMatrix, TTMatrix, TTAudioSignal
 *  
 * @authors Timothy Place & Nathan Wolek
 *
 * @copyright Copyright © 2012, Timothy Place & Nathan Wolek @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_BUFFER_H__
#define __TT_BUFFER_H__

#include "TTDSP.h"
#include "TTSampleMatrix.h"


extern TTHashPtr gTTBufferNameMap;	// maps names to TTSampleMatrix instances for TTBuffer

/**	TTBuffer manages the check-in/out of #TTSampleMatrix pointers.

#TTBuffer acts as a librarian for checking-in and checking-out audio within a chunk of memory. Any object that wishes to read or write samples managed by #TTBuffer must first check-out a pointer to the current #TTSampleMatrix. Once the pointer has been obtained, the object can read and write audio samples by working directly with the corresponding #TTSampleMatrix functions. Upon completion of its work, the object is then responsible for checking-in the pointer back to the #TTBuffer.
 
	@see TTAudioSignal, TTSampleMatrix
*/
class TTDSP_EXPORT TTBuffer : public TTAudioObjectBase {
	TTCLASS_SETUP(TTBuffer)
	
protected:
	
	TTSymbol				mName;					///< name associated with this buffer
	TTSampleMatrixPtr		mActiveMatrix;			///< pointer to TTSampleMatrix that will be returned via checkOutMatrix()
	TTSampleMatrixPtr		mBecomingActiveMatrix;		///< pointer to TTSampleMatrix that is being prepared internally for a future checkOutMatrix()
	
	// internal method used for initializing the TTBuffer and mActiveMatrix for use
	TTErr init(TTChannelCount	channelCount, TTSymbol name);

	// internal method used for disposing of a no-longer used matrix
	TTErr chuckMatrix(TTSampleMatrixPtr oldMatrix);
	
	// internal methods used for prepping mBecomingActiveMatrix, then swapping it out with mActiveMatrix
	TTErr prepareBecomingActiveMatrix();
	TTErr promoteBecomingActiveMatrix();
	
public:
	
	// public method to check out the mActiveMatrix
	TTErr checkOutMatrix(TTSampleMatrixPtr& startUsingThisMatrix);
    TTErr checkOutMatrixValues(const TTValueRef unusedInput, TTValueRef output);
	
	// public method to check in TTSampleMatrix. if it is no longer mActiveMatrix, action is taken.
	TTErr checkInMatrix(TTSampleMatrixPtr& doneUsingThisMatrix);
    TTErr checkInMatrixValues(const TTValueRef input, TTValueRef unusedOutput);
	
	TTErr getNames(const TTValueRef unusedInput, TTValueRef returnedNames)
	{
		return gTTBufferNameMap->getKeys(returnedNames);
	}
	
		
	TTErr setName(const TTValueRef newName)
	{
		TTSymbol			name = kTTSymEmpty;
		TTSymbol			oldName = mName;
		TTValue				returnedValue;
		TTErr				err = kTTErrNone;
		
		name = newName[0];
		
		// if the name is the same, then do nothing
		if (name == mName)
			return kTTErrNone;
		
		// if the name was left off, then generate a random value
		if (name == kTTSymEmpty)
			name = TTSymbol::random();
		
		// see if the name is already in the global buffer name map
		err = gTTBufferNameMap->lookup(name, returnedValue);
		
		// if it is already in use by another TTBuffer
		if (err == kTTErrNone)
		{
			return kTTErrInvalidValue;
		} else { // if it is not, then we can add it 
			gTTBufferNameMap->append(name, TTPtr(this));
			mName = name;
			return kTTErrNone;
		}
	}
	
	
	/****************************************************************************************************/
	// TODO: Some will need to be rewritten as BufferPool implementation is fleshed out
	
	/*
	NOTE: Set methods could follow this pattern
		1) TTObjectInstantiate("samplematrix", (TTObjectPtr*)&mBecomingActiveMatrix, kTTValNONE)
		2) mBecomingActiveMatrix.adaptTo(mActiveMatrix)
		3) mBecomingActiveMatrix->setTheWhatever(TTValue arg1)
		4) if no error...
			mBecomingIdle = mActiveMatrix
			mActiveMatrix = mBecomingActiveMatrix
		5) if mBecomingIdle->getUserCount() = 0 then delete
			else mBecomingIdle->setBufferPoolStage(kSM_BecomingIdle)
     
     In order to ensure that this happens consistently, we use the following macros to wrap methods from TTSampleMatrix as our own
	*/
	
	#define TTBUFFER_WRAP_1ARG(methodname) \
			TTErr methodname (TTValue& arg1) { return mActiveMatrix -> methodname (arg1); }
	#define TTBUFFER_WRAP_k1ARG(methodname) \
			TTErr methodname (const TTValue& arg1) { return mActiveMatrix -> methodname (arg1); }
	#define TTBUFFER_WRAP_WITHSPAWN_k1ARG(methodname) \
			TTErr methodname (const TTValue& arg1) \
				{ \
					TTErr err = prepareBecomingActiveMatrix(); \
					if (!err) \
						err = mBecomingActiveMatrix -> methodname (arg1); \
					if (!err) \
						err = promoteBecomingActiveMatrix(); \
					return err; \
				}

	// Methods of the hosted TTSampleMatrix object

	TTBUFFER_WRAP_WITHSPAWN_k1ARG( setNumChannels )
	TTBUFFER_WRAP_1ARG( getNumChannels )
	
	TTBUFFER_WRAP_WITHSPAWN_k1ARG( setLengthInSeconds )
	TTBUFFER_WRAP_1ARG(  getLengthInSeconds )
	
	TTBUFFER_WRAP_WITHSPAWN_k1ARG( setLengthInSamples )
	TTBUFFER_WRAP_1ARG(  getLengthInSamples )

	/* NOTE: We do not wrap getValueAtIndex, peek, setValueAtIndex, poke and simliar methods.  
	Objects should work directly with the TTSampleMatrixPtr that they check out for these types of operations.
	*/
    
    /**	Attribute updater: whenever the sample rate for this buffer is changed, apply to the active TTSampleMatrix immediately. This change is not deferred to next checkout.
     @return Returns a TTErr error code.	*/
	TTErr updateSampleRate(const TTValue& oldSampleRate, TTValue&)
    {
        return mActiveMatrix->setAttributeValue(kTTSym_sampleRate, sr);
    }
	
	TTErr	fill(const TTValue& value, TTValue& unusedOutput)								
	{ 
		TTErr err = prepareBecomingActiveMatrix();
		if (!err)
			err = mBecomingActiveMatrix -> fill (value,unusedOutput);
		if (!err)
			err = promoteBecomingActiveMatrix();
		return err;
	}
    
    TTErr	load(const TTValue& value, TTValue& unusedOutput)
	{
		TTErr err = prepareBecomingActiveMatrix();
		if (!err)
			err = mBecomingActiveMatrix -> load (value,unusedOutput);
		if (!err)
			err = promoteBecomingActiveMatrix();
		return err;
	}
    
    TTErr   resizeThenLoad(const TTValue& value, TTValue& unusedOutput)
    {
        TTErr err = prepareBecomingActiveMatrix();
        if (!err)
            err = mBecomingActiveMatrix -> resizeThenLoad(value, unusedOutput);
        if (!err)
            err = promoteBecomingActiveMatrix();
        return err;
    }

	TTBUFFER_WRAP_k1ARG( normalize )
	
	/****************************************************************************************************/

	
	/**	Unit testing */
	virtual TTErr test(TTValue& returnedTestInfo);	

};

typedef TTBuffer* TTBufferPtr;

/** Wrap TTBuffer instances for convenience. */
class TTAudioBuffer : public TTObject {
public:
	TTAudioBuffer(const TTValue& channelCount, const TTValue& sampleCount):
	TTObject(kTTSym_buffer, channelCount)
	{
        instance()->setLengthInSamples(sampleCount);
    }
    
    TTBufferPtr instance()
	{
		return (TTBufferPtr)mObjectInstance;
	}
    
    TTErr fill(const TTValue &value)
    {
        TTValue unusedOuput;
        return instance()->fill(value, unusedOuput);
    }
    
    TTErr load(const TTValue &value)
    {
        TTValue unusedOuput;
        return instance()->load(value, unusedOuput);
    }
    
    TTErr resizeThenLoad(const TTValue &value)
    {
        TTValue unusedOutput;
        return instance()->resizeThenLoad(value, unusedOutput);
    }
    
    TTErr checkOutMatrix(TTSampleMatrixPtr& startUsingThisMatrix)
    {
        return instance()->checkOutMatrix(startUsingThisMatrix);
    }
    
    TTErr checkInMatrix(TTSampleMatrixPtr& doneUsingThisMatrix)
    {
        return instance()->checkInMatrix(doneUsingThisMatrix);
    }
    
};

#endif // __TT_BUFFER_H__
