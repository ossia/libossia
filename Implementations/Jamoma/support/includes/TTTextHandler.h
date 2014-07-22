/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief TTObjectBase to handle text file reading and writing to be able to store / recall state of an object into/from text files.
 *
 * @details Write / Read mecanism
 
	writeAs<Format> / readFrom<Format> methods are not directly called using the classic message system. We should prefer use one of the exported TT<Format>Reader / TT<Format>Writer method which have :
	- an Object attribute : the TTObjectBase you want it reads / writes a file
	or
	- the data structure to pass in order to read / write depending on the selected <Format>
 
	This allow us to use the same method to start reading / writing and even to ask to other objects to / write recursively on the same data stucture.
 
	Example :
 
	If you want to read in Xml format you set the Object attribute as myTopObject then you call the Read message with aValueContainingFullPathToaFile. Then this method (as a friend of your TTTopObject class) will automatically create an XmlReader data structure and call ReadFromXml(aValueContainingAnXmlReader) on your myTopObject.
 
	Because your TTTopObject class used TTLowerObject to describe himself (and have to extract their xml description from the xml file to set them up) the ReadFromXml method sets recursively the Object Attribute with aLowerObject and then calls the Read message with an empty value : this would calls the ReadFromXml(aValueContainingAnXmlReader) on your TTLowerObject.
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_TEXT_HANDLER_H__
#define __TT_TEXT_HANDLER_H__

#include "TTModularIncludes.h"

/**  
 */

class TTMODULAR_EXPORT TTTextHandler : public TTObjectBase
{
	TTCLASS_SETUP(TTTextHandler)
	
public:	// use public for recursive access
	
	TTObject	mObject;						///< the last handled object
	TTSymbol	mFilePath;						///< the path to the last writen/read file
	
	TTString	*mWriter;						///< the text where to write
	TTString	*mReader;						///< the text to read
	
	TTValuePtr	mLine;							///< the line being read by the Reader
	TTUInt8		mTabCount;						///< count the number of tabulation at the beginning of the line
	TTUInt8		mSpaceNumberForTab;				///< ATTRIBUTE : depending on the text editor a tab can be equal to 4, 5, 8 white space
	
	TTBoolean	mFirstLine;						///< a flag to know if it is the first line
	TTBoolean	mLastLine;						///< a flag to know if it is the last line

	/** TTTextWriter could takes a file path or a string buffer pointer. */
	TTErr Write(const TTValue& args, TTValue& outputValue);
    TTErr WriteAgain();
	
	/** TTTextReader could takes a file path or a string buffer pointer. */
	TTErr Read(const TTValue& args, TTValue& outputValue);
    TTErr ReadAgain();
	
private :
	
	TTBoolean			mIsWriting;						///< a flag to know if it is writing
	TTBoolean			mIsReading;						///< a flag to know if it is reading
	
};

typedef TTTextHandler* TTTextHandlerPtr;


#endif // __TT_TEXT_HANDLER_H__
