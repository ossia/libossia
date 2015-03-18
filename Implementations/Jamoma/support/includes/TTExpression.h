/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief a TTExpression is a parsed symbol containing an address a logical operator and a value
 *
 * @details The TTExpression class allows to retreive easily each part of a logical expression symbol @n@n
 *
 * @see TTimeCondition
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_EXPRESSION_H__
#define __TT_EXPRESSION_H__

#include "TTScoreIncludes.h"

/**	The TTExpression class allows to retreive easily each part of a logical expression symbol
 
 @see TTimeCondition
 */
class TTExpression : public TTSymbol
{
private:
    
    TTAddress                           mAddress;                       ///< the address
    TTSymbol                            mOperator;                      ///< logical operator (Ø, >, ≥, <, ≤, ==, !=)
    TTValue                             mValue;                         ///< a value to compare

public:
    
    /** TTExpression Constructor */
    TTExpression() :
    mAddress(kTTAdrsEmpty),
    mOperator(kTTSymEmpty)
    {
        mSymbolPointer = gTTAddressTable.lookup("");
    }
    
    TTExpression(const char *cstr)
    {
        mSymbolPointer = gTTAddressTable.lookup(cstr);
        
        TTValue toParse = TTString(cstr);
        toParse.fromString();
        parse(toParse);
    }
    
    TTExpression(const TTString& aString)
    {
        mSymbolPointer = gTTSymbolTable.lookup(aString);
        
        TTValue toParse = aString;
        toParse.fromString();
        parse(toParse);
    }
    
    /** TTExpression Destructor */
    virtual ~TTExpression()
    {;}
    
    /** Get the expression address
     @return                expression address */
    const TTAddress&    getAddress() const;
    
    /** Get the expression operator
     @return                expression operator */
    const TTSymbol&     getOperator() const;
    
    /** Get the expression value
     @return                expression value */
    const TTValue&      getValue() const;
    
    /** evaluate the logical expression 
     @param value           the value to evaluate
     @return                return true is the test passes */
    TTBoolean           evaluate(const TTValue& value);
    
private:
    
    /** Parse the expression
     @return                a value to parse */
    void                parse(TTValue& toParse);
};

typedef TTExpression* TTExpressionPtr;

/** The case receiver callback return back the value of observed address
 @param	toParse             a value : <address operator value>
 @param	anExpression        the parsed expression
 @return					an error code */
void TTSCORE_EXPORT TTExpressionParseFromValue(const TTValue& toParse, TTExpression& anExpression);


#endif // __TT_EXPRESSION_H__
