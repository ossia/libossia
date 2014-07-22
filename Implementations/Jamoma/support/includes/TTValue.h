/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief Provides a common way of representing composite values.
 *
 * @details #TTValue is the primary interface used to pass values to and from methods in Jamoma Core. Methods for a given #TTObject should be passed both an input and output value to complete its operation, while the return is reserved for a #TTErr.
 @n@n Each #TTValue may be composed of a single element or many elements because it has been defined as a subclass of the C++ standard library's <a href="http://www.cplusplus.com/reference/vector/vector/">vector class</a>. This also enables our class to inherit familiar functions such as size() and from its parent class.
 @n@n Individual items within the #TTValue are defined by the #TTElement class. These individual elements may be one of the defined types in the #TTDataType enumeration.
 *
 * @authors Tim Place, Théo de la Hogue, Nathan Wolek, Julien Rabin, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_VALUE_H__
#define __TT_VALUE_H__

#include "TTElement.h"
#include <functional>


/**	@copybrief TTValue.h
	@copydetails TTValue.h
*/
class TTValue : public TTElementVector {
    
public:

	/** @brief Constructor for an empty value */
	TTValue()
	{
		reserve(1);
	}
	
	/** @brief Constructor with a single initial element. */
	template<class T>
	TTValue(const T& anInitialValue)
	{
		resize(1);
		at(0) = anInitialValue;
	}
		
	/** @brief Constructor with two initial elements. */
	template <class T, class U>
	TTValue(const T& aFirstElementInitialValue, const U& aSecondElementInitialValue)
	{
		resize(2);
		at(0) = aFirstElementInitialValue;
		at(1) = aSecondElementInitialValue;
	}
	
	/** @brief Constructor with three initial elements. */
	template <class T, class U, class V>
	TTValue(const T& aFirstElementInitialValue, const U& aSecondElementInitialValue, const V& aThirdElementInitialValue)
	{
		resize(3);
		at(0) = aFirstElementInitialValue;
		at(1) = aSecondElementInitialValue;
		at(2) = aThirdElementInitialValue;
	}

	/** @brief Constructor with four initial elements. */
	template <class T, class U, class V, class W>
	TTValue(const T& aFirstElementInitialValue, const U& aSecondElementInitialValue, const V& aThirdElementInitialValue, const W& aFourthElementInitialValue)
	{
		resize(4);
		at(0) = aFirstElementInitialValue;
		at(1) = aSecondElementInitialValue;
		at(2) = aThirdElementInitialValue;
		at(3) = aFourthElementInitialValue;
	}

	// force the destructor to be non-virtual
	// we don't want subclasses of TTValue so it won't be a problem, and this solves linking snafus in some edge cases
    /** @brief Destructor */
	~TTValue()
	{;}
	
    
private:
	/** @brief Internal method used by the constructors. */
	void init();
	
	/** @brief Performs a deep copy of the object */
	inline void copy(const TTValue& obj);


public:

    /** @brief Clear all values from the vector, leaving with size of 0 */
	void clear() {
		TTElementVector::clear();
	}

    
	/** @brief Copy a value starting from an index until another index */
	void copyRange(const TTValue& obj, TTUInt16 startIndex, TTUInt16 endIndex)
	{
		resize(endIndex - startIndex);
		for (size_t i=0; i<size(); i++)
			at(i) = obj[startIndex+i];
	}
	
	
	/** @brief Copy a value starting from an index until the last element */
	void copyFrom(const TTValue& obj, TTUInt16 index)
	{
		copyRange(obj, index, obj.size());
	}
	
	
	/** @brief Insert another TTValue before the first element.
     @details
     The following example code would result in TTValue b having elements ordered <1, 2, 3, ga, bu, zo, meu>:
     @code{.cpp}
     TTValue a(1, 2, 3); 
     TTValue b(ga, bu, zo, meu);
     b.prepend(a);
     @endcode
	 */
	void prepend(const TTValue& aValueToPrepend)
	{
		TTValue v = aValueToPrepend;

		v.append(*this);
		*this = v;
	}

	
	/**	@brief Assign a value to TTValue.
     @details Overwrites current elements.
     */
	template<class T>
	TTValue& operator = (T value)
	{
		resize(1);
		at(0) = value;
		return *this;
	}
	
	/**	@brief Test equality of two values */
	friend bool operator == (const TTValue& a, const TTValue& b)
	{
		if (a.size() == b.size()) {
			for (size_t i=0; i<a.size(); i++) {
				if (a.at(i) != b.at(i)) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

#ifndef TT_PLATFORM_WIN
    /** @overload
     */
	template<class T>
	friend bool operator == (const TTValue& a, const T b)
	{
		if (a.size() == 1 && a[0] == b)
			return true;
		else
			return false;
	}
#endif

	/** @brief Get a value from TTValue */
	template<class T>
	operator T() const
	{
		if (size())
			return T(at(0));
		else
			return T(0);
	}

    // TTSymbol needs to be manually wrapped to avoid ambiguity as interpretted by the clang compiler
    /** @overload
     */
	operator TTSymbol() const
	{
		if (size())
			return at(0);
		else
			return kTTSymEmpty;
	}
    
    // TTObject needs to be manually wrapped to avoid ambiguity as interpretted by the clang compiler
    /** @overload
     */
	operator TTObject() const
	{
		if (size())
			return at(0);
		else
			return TTObject();
	}
	
    /** @brief Insert a single TTElement at the end */
	template<class T>
	void append(const T& anElementValueToAppend)
	{
		TTElement e(anElementValueToAppend);
		
		push_back(e);
	}
	
    /** @brief Insert another TTValue after the last element.
     @details
     The following example code would result in TTValue b having elements ordered <ga, bu, zo, meu, 1, 2, 3>:
     @code{.cpp}
     TTValue a(1, 2, 3);
     TTValue b(ga, bu, zo, meu);
     b.append(a);
     @endcode
	 */
	void append(const TTValue& aValueToAppend)
	{
		TTUInt32 appendingElementCount = aValueToAppend.size();
		TTUInt32 oldElementCount = size();
		TTUInt32 newElementCount = oldElementCount + appendingElementCount;
		
		resize(newElementCount);
		
		for (TTUInt32 i=0; i<appendingElementCount; i++) {
			TTElement e = aValueToAppend[i];
			
			at(oldElementCount+i) = e;
		}
	}

	/** @brief Clip numerical values between low and high boundaries
     @param[in] aLowBound   Lowest value that should be preserved. Anything lower will be set to this value.
     @param[in] aHighBound  Highest value that should be preserved. Anything higher will be set to this value.
     @return    none
     */
	void clip(const TTFloat64& aLowBound, const TTFloat64& aHighBound)
	{
		for (TTElementIter i = this->begin(); i != this->end(); i++)
			i->clip(aLowBound, aHighBound);
	}
	
	/** @brief Clip numerical values below a specified boundary
     @param[in] aLowBound   Lowest value that should be preserved. Anything lower will be set to this value.
     @return    none
     */
	void cliplow(const TTFloat64& aLowBound)
	{
		for (TTElementIter i = this->begin(); i != this->end(); i++)
			i->cliplow(aLowBound);
	}
	
	/** @brief Clip numerical values above a specified boundary
     @param[in] aHighBound  Highest value that should be preserved. Anything higher will be set to this value.
     @return    none
     */
	void cliphigh(const TTFloat64& aHighBound)
	{
		for (TTElementIter i = this->begin(); i != this->end(); i++)
			i->cliphigh(aHighBound);
	}
	
	/** @brief Round float & double elements to the nearest whole number */
	void round()
	{
		for_each(this->begin(), this->end(), std::mem_fun_ref(&TTElement::round));
	}

	/** @brief Truncate float & double elements so that only whole number remains */
	void truncate()
	{
		for_each(this->begin(), this->end(), std::mem_fun_ref(&TTElement::truncate));
	}
	
	/** @brief Booleanize numerical elements
     @details Sets all non-zero numerical elements to true, while those that are zero will be set to false. Changes the #TTDataType of these elements to kTypeBoolean.
     */
	void booleanize()
	{
		for_each(this->begin(), this->end(), std::mem_fun_ref(&TTElement::booleanize));
	}
	
	/** @brief Return the content as a single string with spaces between elements
     @param     none
     @return    #TTString that contains the content of all elements in the #TTValue
     */
	TTString toString(TTBoolean quotes = YES) const
	{
		TTString temp;
	
		for (size_t i=0; i<size(); i++) {
			at(i).string(temp, quotes);		// get a string for each item
			if (i < (size()-1))				// add a space between each item, but no space at the end
				temp.append(" ");
		}
		
        return temp;
	}
    
    // TODO: Could this be DRYer?
    /** @overload
     */
	void toString(TTBoolean quotes = YES)
	{
		TTString temp;
        
		for (size_t i=0; i<size(); i++) {
			at(i).string(temp, quotes);		// get a string for each item
			if (i < (size()-1))             // add a space between each item, but no space at the end
				temp.append(" ");
		}
		
		// now set the value to the new string
        clear();
        append(temp);
	}
	
    /** @breif Convert a single string into individual elements using space to divide items
     @param     numberAsSymbol  optional #TTBoolean determines whether method leaves numbers as symbols, default is NO
     @return    none
     */
	void fromString(TTBoolean numberAsSymbol = NO)
	{
		if (at(0).type() != kTypeString) { // if the first element isn't a string
			clear(); // clear the contents of the value
			return; // and do nothing else
		}
					
		TTUInt32					n = 0;
		TTInt32						convertedInt;
        TTUInt32					convertedUInt;
		TTFloat32					convertedFloat;
		std::vector<std::string>	strList;
		std::string					str(TTString(at(0)));
		std::istringstream			iss(str);
		
		std::copy(
				  std::istream_iterator<std::string>( iss ),
				  std::istream_iterator<std::string>(),
				  back_inserter( strList ) );
		
		if (strList.size() < 1) {
			clear();
			return;
		}

		resize(strList.size());
		
		for (unsigned int i = 0; i < strList.size(); ++i) {
			TTString	currentString = strList.at(i).c_str();
			
			if (currentString.toTTInt32(convertedInt) && !numberAsSymbol) {
                
				at(n) = int(convertedInt);
				n++;
			}
            else if (currentString.toTTUInt32(convertedUInt) && !numberAsSymbol) {
                
				at(n) = TTUInt32(convertedUInt);
				n++;
			}
			else if (currentString.toTTFloat32(convertedFloat) && !numberAsSymbol) {
                
				at(n) = TTFloat64(convertedFloat);  // cast float32 into float64
				n++;
			}
			else {
				if (currentString.c_str()[0] == '"') {
					TTString	editString = currentString.substr(1, currentString.size());	// don't keep the leading "
					
					while (currentString.c_str()[currentString.size()-1] != '"' && (i != (strList.size() - 1))) {
						i++;
						currentString = strList.at(i);
						
						editString += " ";
						editString += currentString;
					}

					at(n) = TTSymbol(editString.substr(0, editString.size()-1));
					n++;

				}
				else {
					
					at(n) = TTSymbol(currentString.c_str());
					n++;
				}
			}
		}
		
		// resize value
		resize(n);
	}
		

	/**	@breif Convert a comma-separated-value string into an array of TTSymbols.
     @return    kTTErrInvalidType if first item is not kTypeString, else kTTErrNone
	 */
	TTErr transformCSVStringToSymbolArray()
	{
		if (at(0).type() != kTypeString)
			return kTTErrInvalidType;
		
		const TTString&	str = at(0);
		char*			cStr;
		char*			current;

		clear();
		
		cStr = new char[str.size()+1];
		strncpy(cStr, str.c_str(), str.size()+1);
		
		current = strrchr(cStr, ',');
		while (current) {
			*current = 0;
			current++;
			
			// Do some basic whitespace stripping from the ends
			while (*current == ' ')
				current++;
			while (current[strlen(current)-1] == ' ')
				current[strlen(current)-1] = 0;
			
			append(TTSymbol(current));
			current = strrchr(cStr, ',');
		}
		append(TTSymbol(cStr));
		delete[] cStr;
		return kTTErrNone;
	}
    
// inherited functions from Vector class
    
#ifdef _DOXY_
    
    /** @breif Return the number of elements
     @details Inherited from the C++ standard library's <a href="http://www.cplusplus.com/reference/vector/vector/">vector class</a>
     @param     none
     @return    number of elements currently in #TTValue
     */
    size_type size() const noexcept;
    
    /** @breif Change the number of elements
     @details Inherited from the C++ standard library's <a href="http://www.cplusplus.com/reference/vector/vector/">vector class</a>
     @param     n   number of elements for resulting #TTValue
     @return    void
     */
    void resize (size_type n);
    
#endif
	
// deprecated functions
    
    /** @brief DEPRECATED
     @deprecated instead, please call the size() method */
	TT_DEPRECATED( TTUInt16 getSize() const )
	{
		return size();
	}
	
	/** @brief DEPRECATED
     @deprecated instead, please call the resize() method */
	TT_DEPRECATED( void setSize(const TTUInt16 arg) )
	{
		resize(arg);
	}
	
	
	/** @brief DEPRECATED
     @deprecated instead, please call TTElement::type() on the element itself. 
     @details
     Old syntax:
     @code{.cpp}
     TTValue v(1,2,3);
     TTDataType thetype = v.getType(1);
     @endcode
     New syntax:
     @code{.cpp}
     TTValue v(1,2,3);
     TTDataType thetype = v[1].type(); 
     @endcode
     */
	TT_DEPRECATED( TTDataType getType(const TTUInt16 index=0) const )
	{
		return at(index).type();
	}
    
    
    /** @brief DEPRECATED
     @deprecated instead, please make an assignment using standard C array syntax.
     @details
     Old syntax:
     @code{.cpp}
     TTValue v;
     v.set(0, 3.14);
     @endcode
     New syntax:
     @code{.cpp}
     TTValue v;
     v[0] = 3.14;
     @endcode
	 */
	template<class T>
	TT_DEPRECATED ( void set(const TTUInt16 index, const T& anElementValue) )
	{
		at(index) = anElementValue;
	}
    
	/** @brief DEPRECATED
	 @deprecated instead, please fetch the value of an element using standard C array syntax.
     @details
     Old syntax:
     @code{.cpp}
	 TTValue	v(3.14);
	 TTFloat64	mypi;
	 v.get(0, mypi);
	 @endcode
     New syntax:
     @code{.cpp}
	 TTValue v(3.14);
	 TTFloat64	mypi;
	 mypi = v[0];
     @endcode
	 */
	template<class T>
	TT_DEPRECATED ( void get(const TTUInt16 index, T& returnedElementValue) const )
	{
		returnedElementValue = at(index);
	}
    
	/*
     TT_DEPRECATED ( void get(const TTUInt16 index, TTObjectBase** value) const )
     {
     if (at(index).type() == kTypeObject)
     *value = at(index);
     }
	 */
    
	TT_DEPRECATED ( void get(const TTUInt16 index, TTPtr* value) const )
	{
		if (at(index).type() == kTypePointer)
			*value = at(index);
	}
    
	TT_DEPRECATED ( void get(const TTUInt16 index, TTString& value) const )
	{
		value = (TTString)at(index);
	}
    
	
    // inlined for speed (e.g. for use in the matrix)
    /** @brief DEPRECATED
	 @deprecated inlined function formerly used by #TTMatrix or @ref foundationDataspaceLib
     */
    TT_DEPRECATED( TTFloat64 getUInt8(TTUInt16 index = 0) const )
	{
		return TTUInt8(at(index));
	}
	
	// inlined for speed (e.g. for use in the matrix)
    /** @copydoc getUInt8()
     */
	TT_DEPRECATED( TTFloat64 getInt32(TTUInt16 index = 0) const )
	{
		return TTInt32(at(index));
	}
	
	/** @copydoc getUInt8()
     */
	TT_DEPRECATED( TTFloat64 getFloat32(TTUInt16 index = 0) const )
	{
		return TTFloat32(at(index));
	}
	
	/** @copydoc getUInt8()
     */
	TT_DEPRECATED( TTFloat64 getFloat64(TTUInt16 index = 0) const )
	{
		return TTFloat64(at(index));
	}
    
	/** @copydoc getUInt8()
     */
	TT_DEPRECATED( void getArray(TTUInt8* arrayToFill, TTUInt16 maxSize) const )
	{
		for (size_t i=0; i<size(); i++) {
			if (i == maxSize)
				break;
			*(arrayToFill+i) = TTUInt8(at(i));
		}
	}
	
	
	/** @overload
     */
	TT_DEPRECATED( void getArray(TTInt32* arrayToFill, TTUInt16 maxSize) const )
	{
		for (size_t i=0; i<size(); i++) {
			if (i == maxSize)
				break;
			*(arrayToFill+i) = TTInt32(at(i));
		}
	}
	
	
	/** @overload
     */
	TT_DEPRECATED( void getArray(TTFloat32* arrayToFill, TTUInt16 maxSize) const )
	{
		for (size_t i=0; i<size(); i++) {
			if (i == maxSize)
				break;
			*(arrayToFill+i) = TTFloat32(at(i));
		}
	}
	
	
	/** @overload
     */
	TT_DEPRECATED( void getArray(TTFloat64* arrayToFill, TTUInt16 maxSize) const )
	{
		for (size_t i=0; i<size(); i++) {
			if (i == maxSize)
				break;
			*(arrayToFill+i) = TTFloat64(at(i));
		}
	}
	


};


typedef TTValue* TTValuePtr;
typedef TTValue& TTValueRef;
typedef const TTValue& TTValueConstRef;

// dumb global which is an empty / uninitialized symbol -- you shouldn't use it.
// it's only here for backwards compatibility reasons.
typedef void* TTNoValue;
#define kTTValNONE (TTNoValue(0))

#endif // __TT_VALUE_H__

