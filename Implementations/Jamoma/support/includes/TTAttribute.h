/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief The TTAttribute class represents a single attribute, as used by the TTObjectBase class.
 *
 * @details 
 *
 * @authors Timothy Place, Theo de la Hogue, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_ATTRIBUTE_H__
#define __TT_ATTRIBUTE_H__

#include "TTObjectBase.h"

// for now, we support the old macros...
#define SUPPORT_OLD_ATTRIBUTE_REGISTRATION

/**	A convenience macro to be used by subclasses for registering attributes with a custom getter.
	@param	name	The name of the attribute, which is also the name of the classes' member holding the value, and used for the getter method name.
	@param	type	The type of the value.
*/
#define addAttribute(name, type)							TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); registerAttribute(_attrname_##name, type, &m##name)
#ifdef SUPPORT_OLD_ATTRIBUTE_REGISTRATION
#define registerAttributeSimple(name, type)					registerAttribute(#name, type, &name)
#endif

/**	A convenience macro to be used by subclasses for registering attributes with a custom getter.
	@param	name	The name of the attribute, which is also the name of the classes' member holding the value, and used for the getter method name.
	@param	type	The type of the value.
*/
#define addAttributeWithGetter(name, type)					TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); registerAttribute(_attrname_##name, type, &m##name, (TTGetterMethod)& thisTTClass ::get##name )
#ifdef SUPPORT_OLD_ATTRIBUTE_REGISTRATION
#define registerAttributeWithGetter(name, type)				registerAttribute(#name, type, &name,    (TTGetterMethod)& thisTTClass ::get##name )
#endif

/**	A convenience macro to be used by subclasses for registering attributes with a custom setter.
	@param	name	The name of the attribute, which is also the name of the classes' member holding the value, and used for the setter method name.
	@param	type	The type of the value.
*/
#define addAttributeWithSetter(name, type)					TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); registerAttribute(_attrname_##name, type, &m##name, (TTSetterMethod)& thisTTClass ::set##name )
#ifdef SUPPORT_OLD_ATTRIBUTE_REGISTRATION
#define registerAttributeWithSetter(name, type)				registerAttribute(#name, type, &name,    (TTSetterMethod)& thisTTClass ::set##name )
#endif

/**	A convenience macro to be used by subclasses for registering attributes with a custom getter and setter.
	Note that we don't bother passing the address of the value in this macro, because the default setter/getter is not used to access it.
	@param	name	The name of the attribute, which is also the name of the classes' member holding the value, and used for the getter/setter method names.
	@param	type	The type of the value.
*/
#define addAttributeWithGetterAndSetter(name, type)			TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); registerAttribute(_attrname_##name, type, NULL, 	 (TTGetterMethod)& thisTTClass ::get##name, (TTSetterMethod)& thisTTClass ::set##name )
#ifdef SUPPORT_OLD_ATTRIBUTE_REGISTRATION
#define registerAttributeWithSetterAndGetter(name, type)    registerAttribute(#name, type, NULL, 	 (TTGetterMethod)& thisTTClass ::get##name, (TTSetterMethod)& thisTTClass ::set##name )
#endif


/** A convenience macro to be used for registering properties of attributes.
	This assumes that the property is one that has been explicitly supported by TTAttribute through the definition of accessor methods.
	If you are adding a custom property then you must define your own accessor methods and register the property by calling the
	TTObjectBase::registerAttributeProperty() method directly.
*/
#define addAttributeProperty(attributeName, propertyName, initialValue)	registerAttributeProperty(_attrname_##attributeName, #propertyName, initialValue, (TTGetterMethod)& TTAttribute::get##propertyName , (TTSetterMethod)& TTAttribute::set##propertyName )

/****************************************************************************************************/
// Class Specifications


/**
	This class represents a single attribute, as used by the TTObjectBase class.
	At the moment we define it in the same file because we are sharing the typedef
	for TTMethod.
*/
class TTFOUNDATION_EXPORT TTAttribute : public TTObjectBase {
private:
public:
	// Should make this group private, but to get things working initially, we're leaving them public...
	const TTSymbol		name;			///< The name of the attribute.
	TTDataType			type;			///< The data type of the attribute value.
	void*				address;		///< Pointer to the memory holding the attribute value.
	TTGetterMethod		getter;			///< Method to fetch the attribute value.
	TTSetterMethod		setter;			///< Method to set the attribute value.
	const TTObjectBasePtr	getterObject;	///< TTObjectBasePtr to fetch the attribute value.
	const TTObjectBasePtr	setterObject;	///< TTObjectBasePtr to set the attribute value.
	TTAttributeFlags	getterFlags;	///< Define the behavior of the attribute getter method.
	TTAttributeFlags	setterFlags;	///< Define the behavior of the attribute setter method.
	TTValue				internalValue;	///< Attributes that maintain their own data use this member to store it.
										///< This should typically only be used by attribute properties that are not performance critical.
	TTBoolean			readOnly;		///< The readonly property, if defined, means an attribute cannot be set.
	TTFloat64			rangeLowBound;	///< If the range property is defined, this is the bottom of a value's range.
	TTFloat64			rangeHighBound;	///< If the range property is defined, this is the top of a value's range.
	TTSymbol			rangeChecking;	///< If the rangeChecking property is defined, the value should be checked for range and modified accordingly.
	TTBoolean			hidden;			///< Property: this attribute is private/invisible to the outside world
	TTSymbol			description;	///< Property: description of this attribute
	TTValue				mDefaultValue;		///< Property: the default value for this attribute
	
	/** Object constructor.
	 */
	TTAttribute(const TTSymbol newName, TTDataType newType, void* newAddress);
	TTAttribute(const TTSymbol newName, TTDataType newType, void* newAddress, TTGetterMethod newGetter);
	TTAttribute(const TTSymbol newName, TTDataType newType, void* newAddress, TTSetterMethod newSetter);
	TTAttribute(const TTSymbol newName, TTDataType newType, void* newAddress, TTGetterMethod newGetter, TTSetterMethod newSetter);
	TTAttribute(const TTSymbol newName, const TTObjectBasePtr newGetterObject, const TTObjectBasePtr newSetterObject);
	TTAttribute(TTAttributePtr extendedAttribute, const TTObjectBasePtr extendedObject);
	
	/** Object destructor.
	 */
	virtual ~TTAttribute();
	
	/** Set the getterFlag property of the attribute, defining the behavior of the attribute getter method.
	 @param newFlags				The new value for the getterFlag property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	void setGetterFlags(TTAttributeFlags newFlags);
	
	/** Get the current getterFlag property of the attribute, describing the behavior of the attribute getter method.
	 @param currentFlags			Pointer to a #TTValue used to return the current value of the getterFlag property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	void getGetterFlags(TTAttributeFlags& currentFlags);

	/** Set the setterFlag property of the attribute, defining the behavior of the attribute setter method.
	 @param newFlags				The new value for the setterFlag property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	void setSetterFlags(TTAttributeFlags newFlags);
	
	/** Get the current setterFlag property of the attribute, describing the behavior of the attribute setter method.
	 @param currentFlags			Pointer to a #TTValue used to return the current value of the setterFlag property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	void getSetterFlags(TTAttributeFlags& currentFlags);

	/** The default method for getting the current attribute value.
	 @param TTAttribute			The attribute
	 @param TTValue				Pointer to a #TTValue used to return the current value of the attribute.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr defaultGetter(const TTAttribute& attribute, TTValue& value);
	
	/** The default method for setting the attribute value.
	 @param TTAttribute			The attribute
	 @param TTValue				The new value of the attribute.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr defaultSetter(const TTAttribute& attribute, const TTValue& value);
	
	/** TODO: This needs to be documented.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr callbackGetter(const TTAttribute& attribute, TTValue& value);
	
	/** TODO: This needs to be documented.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr callbackSetter(const TTAttribute& attribute, TTValue& value);
	
	/** TODO: This needs to be documented.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr extendedGetter(const TTAttribute& attribute, TTValue& value);
	
	/** TODO: This needs to be documented.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr extendedSetter(const TTAttribute& attribute, TTValue& value);
	
	
	/** Set the readOnly property of the attribute, controlling if the attribute value can be changed or not.
	 @param newReadOnlyValue		The new value for the readOnly property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setreadOnly(const TTValue& newReadOnlyValue);
	
	
	/** Get the readOnly property of the attribute, controlling if the attribute value can be changed or not.
	 @param currentReadOnlyValue	Pointer to a #TTValue used to return the current value of the readOnly property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getreadOnly(TTValue& currentReadOnlyValue);
	
	
	/** Set the range of possible values for the attribute.
	 @param newRange				The new value for the range property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setrange(const TTValue& newRange);
	
	
	/** Get the range of possible values for the attribute.
	 @param currentRange			Pointer to a #TTValue used to return the current value for the range property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getrange(TTValue& currentRange);
	
	
	/** Set the range boundary checking mode property for the attribute.
	 @param newRangeCheckingMode	The new range range boundary checking mode property for the attribute.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setrangeChecking(const TTValue& newRangeCheckingMode);
	
	
	/** Get the range boundary checking mode property for the attribute.
	 @param currentRangeCheckingMode	Pointer to a #TTValue used to return the current value for the range boundary checking mode property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getrangeChecking(TTValue& currentRangeCheckingMode);
	
	/** Set the hidden flag for the attribute, determining if this attribute is private/invisible to the outside world.
	 @param newHiddenFlag			The new value for the hidden flag.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr sethidden(const TTValue& newHiddenFlag);
	
	/** Get the hidden flag for the attribute, indicating if this attribute is private/invisible to the outside world.
	 @param currentHiddenFlag		Pointer to a #TTValue used to return the current value for the hidden flag.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr gethidden(TTValue& currentHiddenFlag);
	
	/** Set the description property of the attribute, used for documentation purposes.
	 @param newDescription			The new value for the documentation property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setdescription(const TTValue& newDescription);
	
	/** Get the description property of the attribute, used for documentation purposes.
	 @param returnedDescription		Pointer to a #TTValue used to return the current value for the documentation property.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getdescription(TTValue& returnedDescription);
	
	TTErr setdefaultValue(const TTValue& aDefaultValue);
	TTErr getdefaultValue(TTValue& aReturnedDefaultValue);
	
};


#endif // __TT_ATTRIBUTE_H__

