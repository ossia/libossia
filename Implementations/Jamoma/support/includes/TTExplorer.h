/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Namespace Explorer Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_EXPLORER_H__
#define __TT_EXPLORER_H__

#include "TTModularIncludes.h"


/**	TTExplorer ... TODO : an explanation

 
 */

class TTMODULAR_EXPORT TTExplorer : public TTObjectBase
{
	TTCLASS_SETUP(TTExplorer)
	
private:
	
	TTSymbol			mNamespace;						///< ATTRIBUTE : the name of the namespace selection to manage
	TTAddress           mAddress;						///< ATTRIBUTE : the node where to start the exploration (default : the local root)
	TTSymbol			mOutput;						///< ATTRIBUTE : what returns the exploration from the node :
														///<	- descendants : any address below the node at any level (default)
														///<	- children : all children of the node
														///<	- brothers : all instances of the node
														///<	- attributes : all attributes of the node
	
	TTBoolean			mUpdate;						///< ATTRIBUTE : is the explorer listen to the namespace to update the result ?
	
	TTSymbol			mSort;							///< ATTRIBUTE : to sort the result : none, alphabetic, priority (default : alphabetic)
	
	TTUInt8				mDepth;							///< ATTRIBUTE : a depth limit for the exploration (only usefull for descendants mode)
	
	TTNodeDirectoryPtr	mDirectory;						///< an explorer depends on a directory
	
	TTObject            mAddressObserver;				///< an address life cycle observer
	TTObject            mApplicationObserver;			///< an application life cycle observer
	TTObject            mReturnValueCallback;			///< a way to return back value to the owner of this explorer
	TTObject            mReturnSelectionCallback;		///< a way to return back selection to the owner of this explorer
	TTHashPtr			mFilterBank;					///< a hash table containing TTDictionaryBasePtr to store filters
	TTListPtr			mFilterList;					///< a list containing TTSymbol to retreive filters in the filter bank
	
	TTNodePtr			mTempNode;						///< remember the node on which the exploration have been done	(Children cases)
	TTHashPtr			mResult;						///< hash table containing all elements found by the explorer
	TTValue				mLastResult;					///< keep the last result to filter unchanged list
		
	/** */
	TTErr Explore();
	
	/** Set the selection state of a namespace item using his name and a new selection state
		if no selection state : switch the state */
	TTErr Select(const TTValue& inputValue, TTValue& outputValue);
	
	/** Set all selection state to YES */
	TTErr SelectAll();
	
	/** Set all selection state to NO */
	TTErr SelectNone();
	
	/** Return selection back to the owner of this explorer */
	TTErr SelectionRefresh();
	
	/** Create a filter, add it to the bank and append it to the list
		< filterName filterKey1 filterValue1 filterKey2 filterValue2 ... >
		note : the schema of the filter is automatically detected by the keys */
	TTErr FilterSet(const TTValue& inputValue, TTValue& outputValue);
	
	/** Delete a filter from the bank using his name and remove it from the filter list */
	TTErr FilterRemove(const TTValue& inputValue, TTValue& outputValue);
	
	/** Get info about a filter or a filterKey
	 < filterName filterKey1 filterValue1 filterKey2 filterValue2 ... > */
	TTErr FilterInfo(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	// TODO : TTErr Dump();
	
	/** */
	TTErr setNamespace(const TTValue& value);
	
	/** */
	TTErr getFilterList(TTValue& value);
	
	/** */
	TTErr setFilterList(const TTValue& value);
	
	/** */
	TTErr setOutput(const TTValue& value);
	
	/** */
	TTErr setAddress(const TTValue& value);
	
	/** */
	TTErr setUpdate(const TTValue& value);
	
	/** */
	TTErr setSort(const TTValue& value);
	
	/** */
	TTErr setDepth(const TTValue& value);
	
	/** */
	TTErr getObjectsByType(TTValue& value);
	
	/** */
	TTErr bindAddress();
	
	/**  */
	TTErr unbindAddress();
	
	/** */
	TTErr bindApplication();
	
	/** */
	TTErr unbindApplication();
	
	/** */
	TTErr returnResultBack();
	
	/** */
	TTErr returnSelectionBack();
	
	friend TTErr TTMODULAR_EXPORT TTExplorerDirectoryCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTExplorerApplicationManagerCallback(const TTValue& baton, const TTValue& data);
};

typedef TTExplorer* TTExplorerPtr;

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTExplorerDirectoryCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTExplorerApplicationManagerCallback(const TTValue& baton, const TTValue& data);

/** compare priority attribute of object's node
 @param	v1							< relativeAddress, a pointer to a value containing a pointer to a TTNode >
 @param	v2							< relativeAddress, a pointer to a value containing a pointer to a TTNode >
 @return							is the priority of v1 is smaller than v2 (except if equal 0) ? */ 
TTBoolean TTMODULAR_EXPORT TTExplorerCompareNodePriority(TTValue& v1, TTValue& v2);

#endif // __TT_EXPLORER_H__
