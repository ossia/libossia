/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief TTNodeInfo provides priority, tag and description for a node.
 *
 * @details ...
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2014, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_NODEINFO_H__
#define __TT_NODEINFO_H__

#include "TTModular.h"

/**	TTNodeInfo provides priority, tag and description for a node.
 @details ...
 */
class TTMODULAR_EXPORT TTNodeInfo : public TTObjectBase
{
	TTCLASS_SETUP(TTNodeInfo)
	
private:
	
	TTSymbol		mDescription;				///< ATTRIBUTE: text to describe the role of this node
    TTInt32			mPriority;					///< ATTRIBUTE: does this node have a priority over other nodes ?
    TTValue			mTags;						///< ATTRIBUTE: tag list for this node
    
    /**	Setter for mDescription attribute
     @param value       new description value */
	TTErr       setDescription(const TTValue& value);
    
    /**	Setter for mPriority attribute
     @param value       new priority value */
	TTErr       setPriority(const TTValue& value);
    
    /**	Setter for mTags attribute
     @param value       new tag value */
	TTErr       setTags(const TTValue& value);
    
    /** Notify observers of an attribute 
     @param attrName    the name of the attribute to notify
     @param value       the value to send to observers */
    TTErr       notifyObservers(TTSymbol attrName, const TTValue& value);
};
typedef TTNodeInfo* TTNodeInfoPtr;


#endif // __TT_NODEINFO_H__
