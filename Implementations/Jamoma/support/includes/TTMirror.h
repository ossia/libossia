/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Mirror Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MIRROR_H__
#define __TT_MIRROR_H__

#include "TTModularIncludes.h"

/**	TTMirror ... TODO : an explanation
 
 
 */


/**	A convenience macro to be used for registering mirror attribute.
	Note that we don't lower the attribute name because we use the name of an existing attribute.
		@param	name	The TTSymbol name of the attribute.
		@param	type	The type of the value.
 */
#define addMirrorAttribute(name, type)		TTObjectBase::registerAttribute(name, type, NULL, (TTGetterMethod)& TTMirror::getMirrorAttribute, (TTSetterMethod)& TTMirror::setMirrorAttribute )

/**	A convenience macro to be used for registering mirror cached attribute.
 Note that we don't lower the attribute name because we use the name of an existing attribute.
 @param	name	The TTSymbol name of the attribute.
 @param	type	The type of the value.
 */
#define addMirrorCachedAttribute(name, type)		TTObjectBase::registerAttribute(name, type, NULL, (TTGetterMethod)& TTMirror::getMirrorCachedAttribute, (TTSetterMethod)& TTMirror::setMirrorCachedAttribute )

/** TODO : how to have TTGetterMethod and TTSetterMethod for Mirror attribute Property ?
 
	A convenience macro to be used for registering properties of mirror attributes.
	Note that we don't lower the attribute name because we use the name of an existing attribute.
		@param	name			The TTSymbol name of the attribute.
		@param	propertyName	The name of the property.
		@param	initialValue	The value of the property.
 */
//#define addMirrorAttributeProperty(name, propertyName, initialValue)		registerAttributeProperty(name, TTSymbol(#propertyName), initialValue, (TTGetterMethod)& TTAttribute::get##propertyName , (TTSetterMethod)& TTAttribute::set##propertyName )


/**	A convenience macro to be used for registering mirror message.
		@param	name	The name of the message.
		@param	flag	The flag of the message.
 */
#define addMirrorMessage(name, flag)		TTObjectBase::registerMessage(name, (TTMethod)& TTMirror::sendMirrorMessage , flag)

/** TODO : how to have TTGetterMethod and TTSetterMethod for Mirror message Property ?
	
	A convenience macro to be used for registering properties of messages.
	Note that we don't lower the message name because we use the name of an existing attribute.
		@param	name			The TTSymbol name of the attribute.
		@param	propertyName	The name of the property.
		@param	initialValue	The value of the property.
 */
//#define addMessageProperty(name, propertyName, initialValue)		registerMessageProperty(name, TTSymbol(#propertyName), initialValue, (TTGetterMethod)& TTMessage::get##propertyName , (TTSetterMethod)& TTMessage::set##propertyName )


class TTMODULAR_EXPORT TTMirror : public TTObjectBase
{
	TTCLASS_SETUP(TTMirror)
	
private:
	
	TTSymbol					mType;							///< the type of the object binded by the mirror
    TTBoolean                   mIsListening;                   ///< to not forget to disable listening when the Mirror will be destroyed
	
	TTObject                    mGetAttributeCallback;			///< a way to get the attribute value
	TTObject                    mSetAttributeCallback;			///< a way to set the attribute value
	TTObject                    mSendMessageCallback;			///< a way to send a message
	TTObject                    mListenAttributeCallback;		///< a way to listen the attribute value
    
    TTHash                      mAttributeValueCache;           ///< a hash table to cache attribute value (if no mGetAttributeCallback)
	
	TTErr						getMirrorAttribute(TTAttribute& anAttribute, TTValue& value);
	TTErr						setMirrorAttribute(TTAttribute& anAttribute, const TTValue& value);
    
    TTErr						getMirrorCachedAttribute(TTAttribute& anAttribute, TTValue& value);
	TTErr						setMirrorCachedAttribute(TTAttribute& anAttribute, const TTValue& value);
    
	TTErr						sendMirrorMessage(const TTSymbol* messageName, const TTValue& inputValue, TTValue& outputValue);
    
    /** When the type doesn't refer to a known class, use this method to instantiate the attributes to mirror
     @param inputValue      all attribute names to mirror
     @param outputValue     nothing
     @return #TTErr error code */
    TTErr						AttributesInstantiate(const TTValue& inputValue, TTValue& outputValue);
    
    /** When the type doesn't refer to a known class, use this method to instantiate the messages to mirror
     @param inputValue      all message names to mirror
     @param outputValue     nothing
     @return #TTErr error code */
    TTErr						MessagesInstantiate(const TTValue& inputValue, TTValue& outputValue);
    
    /** Add an attribute to cache its value and avoid the use of mGetAttributeCallback
     @param inputValue      the name of an attribute to add to the cache, a pointer to a value to cache
     @param outputValue     nothing
     @return #TTErr error code */
    TTErr						AttributeCache(const TTValue& inputValue, TTValue& outputValue);
    
    /** Remove an attribute to don't cache its and prefer to use mGetAttributeCallback if exist
     @param inputValue      the name of an attribute
     @param outputValue     nothing
     @return #TTErr error code */
    TTErr						AttributeUncache(const TTValue& inputValue, TTValue& outputValue);
    
public:
	TTErr						updateAttributeValue(const TTSymbol attributeName, TTValue& value);
    
    /** Enable the listening of an attribute value using protocol solution if exist.
     @details you have to unregister all observers for notification on the attribute before to disable the listening
     @param inputValue      a TTAttribute object, a TTBoolean value to enable/disable
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr						enableListening(const TTAttribute& anAttribute, TTBoolean enable);
    
    // over writting of the TTObjectBase::getName() method
    TTSymbol                    getName();
	
};

typedef TTMirror* TTMirrorPtr;

#endif // __TT_MIRROR_H__
