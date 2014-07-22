/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief an Expression is a parsed symbol containing an address a logical operator and a value
 *
 * @details The Expression class allows to retreive easily each part of a logical expression symbol @n@n
 *
 * @see TTimeCondition
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "TTScoreIncludes.h"

/**	The Expression class allows to retreive easily each part of a logical expression symbol
 
 @see TTimeCondition
 */
class Expression : public TTSymbol
{
private:
    
    TTAddress                           mAddress;                       ///< the address
    TTSymbol                            mOperator;                      ///< logical operator (Ø, >, ≥, <, ≤, ==, !=)
    TTValue                             mValue;                         ///< a value to compare

public:
    
    /** Expression Constructor */
    Expression() :
    mAddress(kTTAdrsEmpty),
    mOperator(kTTSymEmpty)
    {
        mSymbolPointer = gTTAddressTable.lookup("");
    }
    
    Expression(const char *cstr)
    {
        mSymbolPointer = gTTAddressTable.lookup(cstr);
        
        TTValue toParse = TTString(cstr);
        toParse.fromString();
        parse(toParse);
    }
    
    Expression(const TTString& aString)
    {
        mSymbolPointer = gTTSymbolTable.lookup(aString);
        
        TTValue toParse = aString;
        toParse.fromString();
        parse(toParse);
    }
    
    /** Expression Destructor */
    virtual ~Expression()
    {;}
    
    /** Get the expression address
     @return                expression address */
    const TTAddress&      getAddress() const;
    
    /** Get the expression operator
     @return                expression operator */
    const TTSymbol&       getOperator() const;
    
    /** Get the expression value
     @return                expression value */
    const TTValue&        getValue() const;
    
    /** evaluate the logical expression 
     @param value           the value to evaluate
     @return                return true is the test passes */
    TTBoolean       evaluate(const TTValue& value);
    
private:
    
    /** Parse the expression
     @return                a value to parse */
    void            parse(TTValue& toParse);
};

typedef Expression* ExpressionPtr;

/** The case receiver callback return back the value of observed address
 @param	toParse             a value : <address operator value>
 @param	anExpression        the parsed expression
 @return					an error code */
void TTSCORE_EXPORT ExpressionParseFromValue(const TTValue& toParse, Expression& anExpression);


#endif // __EXPRESSION_H__
