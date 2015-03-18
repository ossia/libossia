/*
 Jamoma Class for representing strings
 Copyright © 2012, Timothy Place & Théo Delahogue

 License: This code is licensed under the terms of the "New BSD License"
 http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_STRING_H__
#define __TT_STRING_H__

#include "TTBase.h"


/****************************************************************************************************/
// Class Specification

/**	The TTString class is used to represent a string.
	We cannot safely pass std::string across lib boundaries, particularly on Windows.
	We also don't want to simply pass naked C-Strings for a whole host of reasons.

	In many cases we try to mimic the interface of std::string for ease of compatibility.


	FROM http://cboard.cprogramming.com/cplusplus-programming/86598-std-vector-char-vs-std-string.html
	1. std::string has a huge number of string-related functions which make it easy to manipulate strings.
	2. std::vector, on the other hand, is guaranteed to be contiguous in memory -- that is, &data[x + 1] = &data[x] + sizeof(data[x]).
		std::string has NO guarantee that it is contiguous in memory.
		So, for example, say you're using an API call that fills a character buffer.
		You'd need to use the vector, not the string.

	@seealso TTSymbol
*/
class TTString : public std::vector<char> {

public:

	/** Constructor */
	TTString(const char *aCString = "")
	{
		assign(aCString);
	}


	TTString(const std::string& aStdString)
	{
		assign(aStdString);
	}


	TTString(std::vector<char>::iterator begin, std::vector<char>::iterator end)
	{
		TTPtrSizedInt newsize = (end - begin);

		reserve(newsize+16);
		for (std::vector<char>::iterator c = begin; c != end; ++c) {
			push_back(*c);
		}

		// In some cases the range has NULL termination included, in other cases not.
		// In these cases we need to correct the size of the string to match the C-String.
		// If we don't then two strings with identical contents (e.g. "foo" and "foo") will hash to different values.
		// If then looked-up in the symbol table we corrupt the symbol table with two entries for the same string!
		TTBoolean resized = NO;

		for (TTPtrSizedInt i=newsize-1; i>0; i--) {
			if (at(i) != 0) {
				resize(i+1);
				resized = YES;
				break;
			}
		}
		if (!resized)
			resize(newsize); // ensure NULL termination
	}

	/** Destructor */
	~TTString()
	{}


	/** Return a pointer to the internal C-string */
	const char* c_str() const
	{
		return &this->at(0);
	}


	/** Cast to a C-string. */
	operator const char*() const
	{
		//return mData;
		return &this->at(0);
	}


	/** Assign from a C-string. */
	TTString& operator = (const char* aCString)
	{
		assign(aCString);
		return *this;
	}


	/** Assign from a std::string. */
	TTString& operator = (std::string& aStdString)
	{
		assign(aStdString);
		return *this;
	}


	/** Assign from a simple char. */
	TTString& operator = (const char aChar)
	{
		resize(1);
		at(0) = aChar;
		return *this;
	}


	/** Overload to assign from a C-string. */
	void assign(const char* aCString, size_t length=0)
	{
		if (length == 0) // no length defined, so check the c-string in the traditional way
			length = strlen(aCString);

		if ((length+1) >= capacity())
			reserve(length+16);

		resize(length);
		memcpy(&this->at(0), aCString, length);
	}


	/** Assign from a std::string. */
	void assign(const std::string& aStdString)
	{
		assign(aStdString.c_str(), aStdString.length());
	}


	/** Find out the length of a string.  */
	size_t size() const
	{
		return std::vector<char>::size() - 1;
	}


	/** Find out the length of a string.  */
	size_t length() const
	{
		return size();
	}


	// because when size() == 0 there is still a char (null terminator) in the vector, we have to override this
	bool empty() const
	{
		return size() == 0;
	}

	/** Allocate  memory for the string. */
	void resize(size_t newSize)
	{
		std::vector<char>::resize(newSize + 1);
		this->at(newSize) = 0; // NULL terminate for safety
	}


	template<class T>
	TTString& operator += (const T& arg)
	{
		append(arg);
		return (*this);
	}


	template<class T>
	TTString& operator += (const TTString& anotherString)
	{
		append(anotherString.c_str(), anotherString.length());
		return (*this);
	}


	template<class T>
	TTString& operator += (const std::string& aStdString)
	{
		append(aStdString.c_str(), aStdString.length());
		return (*this);
	}


	/** Append / Concatenate */
	void append(const char *str, size_t length=0)
	{
		if (!str)
			return;

		if (length == 0)
			length = strlen(str);

		size_t oldSize = size();
		size_t newSize = oldSize + length;

		if (newSize >= capacity())
			reserve(newSize + 256);

		resize(newSize);
		memcpy(&this->at(oldSize), str, length);
	}


	void append(const char aChar)
	{
		char s[2];

		snprintf(s, 2, "%c", aChar);
		append(s);
	}


	void append(int anInt)
	{
		char s[16];

		snprintf(s, 16, "%i", anInt);
		append(s);
	}


	void append(unsigned int aUInt)
	{
		char s[16];

		snprintf(s, 16, "%u", aUInt);
		append(s);
	}


	void append(float aFloat)
	{
		char s[16];

		snprintf(s, 16, "%f", aFloat);
		append(s);
	}


	void append(double aDouble)
	{
		char s[16];

		snprintf(s, 16, "%lf", aDouble);
		append(s);
	}


	template<class T>
	TTString operator + (const T& arg)
	{
		TTString result = *this;	// Make a copy of myself
		result += arg;				// Use += to add arg to the copy.
		return result;
	}


	bool operator == (const char *cString) const
	{
		const TTString s(cString);
		return (*this) == s;
	}

	bool operator == (const TTString &other) const
	{
		return *dynamic_cast<const std::vector<char>*>(this) == *dynamic_cast<const std::vector<char>*>(&other);
	}


	bool operator != (const char *cString) const
	{
		return !(*this == cString);
	}

	bool operator != (const TTString &other) const
	{
		return !(*this == other);
	}


	/** Return the index of the first instance of a specified char in the string.
		@param	aChar	The char for which to search
		@param	from	A position in the string from which to begin the search.  By default it starts at the beginning (0)
	 */
	size_t find_first_of(const char aChar, size_t from = 0)
	{
		TTBoolean	found = NO;
		size_t		i;

		for (i=from; i<size(); i++) {
			if (at(i) == aChar) {
				found = YES;
				break;
			}
		}
		if (found)
			return i;
		else
			return -1;
	}

    /** Return the index of the last instance of a specified char in the string.
     @param	aChar	The char for which to search
	 */
	size_t find_last_of(const char aChar)
	{
		TTBoolean	found = NO;
		size_t		i, j;

        j = size() - 1;

		for (i=0; i<size(); i++) {
			if (at(j) == aChar) {
				found = YES;
				break;
			} else {
                j--;
            }
		}
		if (found)
			return j;
		else
			return -1;
	}


	/** Returns a string object with its contents initialized to a substring of the current object.
		@param pos	Position of a character in the current string object to be used as starting character for the substring.
		@param n 	Length of the substring.
	 */
	TTString substr (size_t pos = 0, size_t n = 1) const
	{
		TTString	substring;
		size_t		i;

		if (pos+n > size())		// If the size is too large and thus the range extends past the end of the string...
			n = size() - pos;	// we limit the range to end at the end of the string.

		substring.reserve(n+16);
		substring.resize(n);
		for (i=0; i<n; i++) {
			substring[i] = (*this)[pos + i];
			if (pos+i >= size())
				break;
		}
		return substring;
	}


	// This avoids a warning in GCC 4.7 about ambiguity between using an int vs. a size_t where
	// the int could also be considered an index into a char array
	char& operator[] (const int index)
	{
		return this->at(index);
	}


	// NOTE: we do not export TTString because it is defined in a header as a subclass of a stl template
	// but we do want to export this method, which is not defined inline so that we don't pick up a direct
	// dependency on Mersenne Twister
	/** Replace contents with a pseudo-random string. */
	void TTFOUNDATION_EXPORT random();



//	TTBoolean toTTInt32( const TTString & str, TTInt32 & convertedInt )
	TTBoolean toTTInt32(TTInt32& convertedInt) const
	{
		char * pEnd;

		convertedInt = strtol(c_str(), &pEnd, 10);
		return *pEnd == 0;
	}

    //	TTBoolean toTTUInt32( const TTString & str, TTInt32 & convertedUInt )
	TTBoolean toTTUInt32(TTUInt32& convertedUInt) const
	{
		char * pEnd;

		convertedUInt = strtoul(c_str(), &pEnd, 10);

        // is the last letter is a 'u' ?
		return *pEnd == 'u' && pEnd == (c_str() + length() - 1);
	}

	/*	note : isTTFloat32 works only because the TTInt32 case is matched before
	 see in TTValue::fromString method
	 */
//	TTBoolean toTTFloat32( const TTString & str, TTFloat32 & convertedFloat )
	TTBoolean toTTFloat32(TTFloat32& convertedFloat) const
	{
		char * pEnd;

		convertedFloat = strtod(c_str(), &pEnd);
		return *pEnd == 0;
	}

};


/** Iterator for using STL algorithms with TTString. */
typedef TTString::iterator	TTStringIter;


/** Expose TTString for use in std output streams. */
template <class charT, class traits>
std::basic_ostream <charT, traits>& operator<< (std::basic_ostream <charT, traits>& stream, const TTString& aString)
{
	return stream << aString.c_str();
}


// Only providing this for use in the Foundation itself at the moment
// (on OS 10.8 with Xcode 4.4 and compiling with clang for the Ruby extension, this code cause compile problems)
#ifdef TTFOUNDATION_EXPORTS


/** Provide overload of std::hash so that TTString can be used the same as std::string for std::map et al. */

#if defined( __clang__ )

// GCC and Clang provide different (cryptic) ways of adding custom types to the c++ hashing classes
// The GCC version is based on code from StackOverflow
// The Clang version is based on the code for std::string from libc ( on the mac these are installed @ /usr/lib/c++/v1/ )

#include <functional>
namespace std
{
	template <>
	//struct hash<TTString> : public __hash_base<size_t, TTString>
	struct hash<TTString> //: public __hash_node<size_t, TTString>
	{
		public:
			size_t operator()(const TTString& __val) const noexcept
			{
				return __do_string_hash(__val.data(), __val.data() + __val.size());
			}
	};
}

#elif defined( TT_PLATFORM_WIN )
namespace std
{
//    namespace tr1
//    {
        template <>
        struct hash<TTString> : public unary_function<TTString, size_t>
        {
			public:
				size_t operator()(const TTString& self) const
				{
					size_t hashkey = _Hash_seq((const unsigned char*)self.data(), self.size()); //std::hash(self.data(), self.size());
					//cout << "HASH: " << self.data() << " with size: " << self.size() << " = " << hashkey << endl;
					return hashkey;
				}
			};
//    }
}

#else // gcc 4.7

namespace std
{
	template <>
	struct hash<TTString>
	{
		public:

			size_t operator()(const TTString& self) const
			{
				size_t hashkey = std::_Hash_impl::hash(self.data(), self.size());
				return hashkey;
			}

	};
}

#endif // ifdef clang etc...

#endif // TTFOUNDATION_EXPORTS


#endif // __TT_STRING_H__
