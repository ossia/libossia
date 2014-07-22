/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief Handles opml file reading and writing to be able to store / recall state of an object into/from opml files.
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_OPML_HANDLER_H__
#define __TT_OPML_HANDLER_H__

#include "TTModularIncludes.h"
#include <stdio.h>

#define TTMODULAR_OPML_ENCODING "ISO-8859-1"

/** Write / Read mecanism
 
	writeAs<Format> / readFrom<Format> methods are not directly called using the classic message system.
	We should prefer use one of the exported TT<Format>Reader / TT<Format>Writer method which have :
		- an Object attribute : the #TTObject you want it reads / writes a file
	or
		- the data structure to pass in order to read / write depending on the selected <Format>
 
	This allow us to use the same method to start reading / writing and even to ask to other objects to
	read / write recursively on the same data stucture.
 
 
	Exemple :
 
	If you want to read in Opml format you set the Object attribute as myTopObject then you call the Read message with 
	aValueContainingFullPathToaFile. Then this method (as a friend of your TTTopObject class) will automatically create 
	an OpmlReader data structure and call ReadFromOpml(aValueContainingAnOpmlReader) on your myTopObject.
 
	Because your TTTopObject class used TTLowerObject to describe himself (and have to extract their opml description 
	from the opml file to set them up) the ReadFromOpml method sets recursively the Object Attribute with aLowerObject 
	and then calls the Read message with an empty value : this would calls the ReadFromOpml(aValueContainingAnOpmlReader)
	on your TTLowerObject.
 
 */

class TTMODULAR_EXPORT TTOpmlHandler : public TTObjectBase
{
	TTCLASS_SETUP(TTOpmlHandler)
	
public:	// use public for recursive access
	
	TTObject			mObject;						///< the last handled object
	TTSymbol			mFilePath;						///< the path to the last writen/read file

	TTSymbol			mHeaderNodeName;				///< the name of the header node in the opml file
	TTSymbol			mVersion;						///< the version number

	TTPtr				mWriter;						// xmlTextWriterPtr type, but using generic pointer in the shared header
	TTPtr				mReader;						// xmlTextReaderPtr type, but using generic pointer in the shared header
	
	TTSymbol			mXmlNodeName;					///< the Node being read by the Reader
	

	/** TTOpmlWriter could takes absolute file path or nothing.
		In the path case, TTOpmlWriter starts opml file writting and then calls the WriteAsOpml 
		method of mObject attribute
		In the second case, it directly calls the WriteAsOpml method */
	TTErr Write(const TTValue& args, TTValue& outputValue);
	TTErr WriteAgain();
	
	
	/** TTOpmlReader could takes absolute file path or nothing.
		In the path case, TTOpmlReader starts opml file reading and then calls the ReadFromOpml 
		method of mObject attribute
		In the second case, it directly calls the ReadFromOpml method */
	TTErr Read(const TTValue& args, TTValue& outputValue);
	TTErr ReadAgain();
	
	/** TTOpmlReader make a TTValue from an opmlChar* using the fromString method (see in TTValue.h) */
//	TTErr fromXmlChar(const xmlChar* xCh, TTValue& v, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	TTErr fromXmlChar(const void* xCh, TTValue& v, TTBoolean addQuote=NO, TTBoolean numberAsSymbol=NO);
	
private :
	
	TTBoolean				mIsWriting;				///< a flag to know if it is writing a file
	TTBoolean				mIsReading;				///< a flag to know if it is reading a file
	
};

typedef TTOpmlHandler* TTOpmlHandlerPtr;


#endif // __TT_OPML_HANDLER_H__
