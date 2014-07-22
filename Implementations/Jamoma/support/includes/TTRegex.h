/* 
 * TTRegex
 * Copyright © 2008, Théo de la Hogue & Tim Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_REGEX_H__
#define __TT_REGEX_H__

#ifdef WIN_VERSION
 #pragma warning(disable:4083) //warning C4083: expected 'newline'; found identifier 's'
#endif // WIN_VERSION

#include "TTFoundationAPI.h"

//typedef TTString::const_iterator	TTRegexStringPosition;
//typedef TTString::iterator			TTRegexStringIterator;



/****************************************************************************************************/
// Class Specifications

/**
 
 */

class TTFOUNDATION_EXPORT TTRegex {
	
private:

	TTPtr			mExpression;				///< the pointer to the boost::regex expression
	TTPtr			mResult;					///< the pointer to the boost::match_results <std::string::const_iterator>
		
public:
	
	/** Constructor */
	TTRegex(const char*  anExpression);
	
	/** Destructor */
	virtual ~TTRegex();

	/** Parse a string using the expression
		@param	begin					the beginning of the string to parse
		@param	end						the end of the string to parse
		@return							a error code	*/
//	TTErr parse(TTRegexStringPosition& begin, TTRegexStringPosition& end);
	TTErr parse(TTStringIter& begin, TTStringIter& end);
	
//	TTErr parse(TTRegexStringIterator& begin, TTRegexStringIterator& end)
//	{
//		return parse((TTRegexStringPosition&)begin, (TTRegexStringPosition&)end);
//	}

	/** Get where start the result */
	//TTRegexStringPosition	begin();
	TTStringIter	begin();
	
	/** Get where end the result */
	//TTRegexStringPosition	end();
	TTStringIter	end();
};

	
#endif // __TT_REGEX_H__
