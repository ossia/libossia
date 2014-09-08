/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief An MVC viewer object.
 *
 * @details In the Max implementation this is a j.remote object.
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_VIEWER_H__
#define __TT_VIEWER_H__

#include "TTModularIncludes.h"


class TTReceiver;
typedef TTReceiver* TTReceiverPtr;

class TTSender;
typedef TTSender* TTSenderPtr;

class TTApplicationManager;
typedef TTApplicationManager* TTApplicationManagerPtr;

class TTMODULAR_EXPORT TTViewer : public TTCallback
{
	TTCLASS_SETUP(TTViewer)
	
private:
	
	TTAddress                   mAddress;					///< ATTRIBUTE : data address to bind
	TTSymbol					mDescription;				///< ATTRIBUTE : text to describe the role of this data
	TTSymbol					mType;						///< ATTRIBUTE : type of the gui
	TTSymbol					mTags;						///< ATTRIBUTE : a tag for this viewer
	TTBoolean					mHighlight;					///< ATTRIBUTE : highlight state of the gui
	TTBoolean					mFreeze;					///< ATTRIBUTE : freeze data returning
	
	TTSymbol					mDataspace;					///< ATTRIBUTE : The dataspace that this view uses (default is 'none')
	TTSymbol					mDataspaceUnit;				///< ATTRIBUTE : The unit within the dataspace.
	TTObject                    mDataspaceConverter;		///< Performs conversions from data unit to the view unit
	TTObject                    mDataspaceObserver;			///< Observes the data's dataspace attribute
	TTObject                    mDataspaceUnitObserver;		///< Observes the data's unit attribute
	
	TTBoolean					mActive;					///< ATTRIBUTE : if false, received data won't be output
	TTValue						mReturnedValue;				///< ATTRIBUTE : a local value to allow observation of this viewer
	
	TTObject                    mReceiver;					///< Binds to our data
	TTObject                    mSender;					///< Binds to our data
	
	/** set the address */
	TTErr setAddress(const TTValue& value);
	
	/** set the attribute */
	TTErr setAttribute(const TTValue& value);
	
	/** set mActive */
	TTErr setActive(const TTValue& value);
    
    /** set the highlight */
	TTErr setHighlight(const TTValue& value);
	
	/** set the freeze */
	TTErr setFreeze(const TTValue& value);
	
	/**	Setter for mDataspaceUnit attribute. */
	TTErr setDataspaceUnit(const TTValue& value);
	
	/** set the returnedValue */
	TTErr setReturnedValue(const TTValue& value);
	
	/** */
	TTErr Send(const TTValue& valueToSend, TTValue& outputValue);
    
    /** Ask the value and use the outputValue to return it.
     This method is not relevant with address containing wildcards */
	TTErr Grab(const TTValue& inputValue, TTValue& outputValue);
	
	friend TTErr TTMODULAR_EXPORT TTViewerReceiveAddressCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTViewerReceiveValueCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTViewerDataspaceCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTViewerDataspaceUnitCallback(const TTValue& baton, const TTValue& data);
};

typedef TTViewer* TTViewerPtr;

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTViewerReceiveAddressCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTViewerReceiveValueCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTViewerDataspaceCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTViewerDataspaceUnitCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_VIEWER_H__
