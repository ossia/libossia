/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief TTObjectBase to handle xml file reading and writing to be able to store / recall state of an object into/from xml files.
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_XML_HANDLER_H__
#define __TT_XML_HANDLER_H__

#include "TTModularIncludes.h"
#include <stdio.h>

#define TTMODULAR_XML_ENCODING "ISO-8859-1"

/** Write / Read mecanism
 
	writeAs<Format> / readFrom<Format> methods are not directly called using the classic message system.
	We should prefer use one of the exported TT<Format>Reader / TT<Format>Writer method which have :
		- an Object attribute : the TTObjectBase you want it reads / writes a file
	or
		- the data structure to pass in order to read / write depending on the selected <Format>
 
	This allow us to use the same method to start reading / writing and even to ask to other objects to
	read / write recursively on the same data stucture.
 
 
	Exemple :
 
	If you want to read in Xml format you set the Object attribute as myTopObject then you call the Read message with 
	aValueContainingFullPathToaFile. Then this method (as a friend of your TTTopObject class) will automatically create 
	an XmlReader data structure and call ReadFromXml(aValueContainingAnXmlReader) on your myTopObject.
 
	Because your TTTopObject class used TTLowerObject to describe himself (and have to extract their xml description 
	from the xml file to set them up) the ReadFromXml method sets recursively the Object Attribute with aLowerObject 
	and then calls the Read message with an empty value : this would calls the ReadFromXml(aValueContainingAnXmlReader)
	on your TTLowerObject.
 
 */

class TTMODULAR_EXPORT TTXmlHandler : public TTObjectBase
{
	TTCLASS_SETUP(TTXmlHandler)
	
public:	// use public for recursive access
	
	TTValue             mObject;						///< the last handled object (it is possible to have an array of objects)
	TTSymbol			mFilePath;						///< the path to the last writen/read file

	TTSymbol			mHeaderNodeName;				///< the name of the header node in the xml file
	TTSymbol			mVersion;						///< the version number 
	TTSymbol			mXmlSchemaInstance;				///< the URL of the schema instance location
	TTSymbol			mXmlSchemaLocation;				///< the URL of the xml schema location

	TTPtr				mWriter;						// xmlTextWriterPtr
	TTPtr				mReader;						// xmlTextReaderPtr
	
	TTBoolean			mXmlNodeStart;					///< true if the Reader starts to read a Node
    TTBoolean			mXmlNodeIsEmpty;				///< true if the Node is empty
	TTSymbol			mXmlNodeName;					///< the Node name being read by the Reader
	TTValue				mXmlNodeValue;					///< the Node value being read by the Reader
	
    /**	Setter for mObject attribute. */
	TTErr setObject(const TTValue& value);

	/** TTXmlWriter could takes absolute file path or nothing.
		In the path case, TTXmlWriter starts xml file writting and then calls the WriteAsXml 
		method of mObject attribute
		In the second case, it directly calls the WriteAsXml method */
	TTErr Write(const TTValue& args, TTValue& outputValue);
	TTErr WriteAgain();
	
	/** TTXmlReader could takes absolute file path or nothing.
		In the path case, TTXmlReader starts xml file reading and then calls the ReadFromXml 
		method of mObject attribute
		In the second case, it directly calls the ReadFromXml method */
	TTErr Read(const TTValue& args, TTValue& outputValue);
	TTErr ReadAgain();
	
	/** TTXmlReader make a TTValue from an xmlChar* using the fromString method (see in TTValue.h) */
//	TTErr fromXmlChar(const xmlChar* xCh, TTValue& v, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	TTErr fromXmlChar(const void* xCh, TTValue& v, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	
	/** Get the value of an xml element attribute */
	TTErr getXmlAttribute(TTSymbol attributeName, TTValue& returnedValue, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	
	/** Get the value of the next xml element attribute */
	TTErr getXmlNextAttribute(TTSymbol& returnedAttributeName, TTValue& returnedValue, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	
private :
	
	TTBoolean				mIsWriting;				///< a flag to know if it is writing a file
	TTBoolean				mIsReading;				///< a flag to know if it is reading a file
	
};

typedef TTXmlHandler* TTXmlHandlerPtr;


#endif // __TT_XML_HANDLER_H__
