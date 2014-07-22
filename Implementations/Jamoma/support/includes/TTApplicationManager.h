/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief Handles #TTApplication and #Protocol instances
 *
 * @details the #TTApplicationManager class is a kind of tower control between our application (called "local") and all others (called "distant").
 * This coordination between applications is based on protocol units which one applications have to register. @n
 * Then, once applications are registered to protocols, any distant applications can : @n
 *      - discover the local application namespace. @n
 *      - get the value an attribute of an object registered into the local application. @n
 *      - set the value an attribute of an object registered into the local application. @n
 *      - listen the value an attribute of an object registered into the local application. @n
 * First of all, notice those features are not available for all protocols @n
 * Then, notice the features above can also be done in the opposite way (the local application to the distant applications). @n
 * The #TTApplicationManager class provides method to instanciate and release #TTApplication and #Protocol instances. @n
 *
 * @see TTApplication, ProtocolLib
 *
 * @authors Théo de la Hogue
 *
 * @copyright Copyright © 2010-2014, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_APPLICATION_MANAGER_H__
#define __TT_APPLICATION_MANAGER_H__

#include "TTModularIncludes.h"

class TTApplication;
typedef TTApplication* TTApplicationPtr;

class Protocol;
typedef Protocol* ProtocolPtr;

class TTXmlHandler;
typedef TTXmlHandler* TTXmlHandlerPtr;

/** @enum Application notification flags
 *  @brief Defines notification flags to notify observers of #TTApplicationManager
 */
enum TTApplicationNotificationFlag {
	kApplicationReleased = 0,				///< an application have been released by the application manager
	kApplicationInstantiated= 1,			///< an application have been intantiated by the application manager
	kApplicationProtocolStarted = 2,		///< application's protocol have been started
	kApplicationProtocolStopped = 3			///< application's protocol will be stopped
};

/**	Handles #TTApplication and #Protocol instances
 
 the #TTApplicationManager class is a kind of tower control between our application (called "local") and all others (called "distant").
 This coordination between applications is based on protocol units which one applications have to register. @n
 Then, once applications are registered to protocols, any distant applications can : @n
        - discover the local application namespace. @n
        - get the value an attribute of an object registered into the local application. @n
        - set the value an attribute of an object registered into the local application. @n
        - listen the value an attribute of an object registered into the local application. @n
 First of all, notice those features are not available for all protocols @n
 Then, notice the features above can also be done in the opposite way (the local application to the distant applications). @n
 The #TTApplicationManager class provides method to instanciate and release #TTApplication and #Protocol instances. @n
 
 @see TTApplication, ProtocolLib
 */
class TTMODULAR_EXPORT TTApplicationManager : public TTObjectBase
{
	TTCLASS_SETUP(TTApplicationManager)
	
private:

	TTHash              mApplications;						///< hash table containing <#TTSymbol applicationName, #TTApplication anApplication>
	TTHash              mProtocols;							///< hash table containing <#TTSymbol protocolName, #Protocol aProtocol>
		
	TTObject            mApplicationLocal;					///< the local application
	TTObject            mApplicationCurrent;				///< used for ReadFromXml mechanism
	
	TTHash              mApplicationObservers;				///< a pointer to a hashtab which register all application life cycle observers
	TTMutexPtr			mApplicationObserversMutex;			///< a Mutex to protect the mObservers hash table.
    
    TTObject            mCurrentProtocol;                   ///< used for ReadFromXml mechanism

#if 0
#pragma mark -
#pragma mark Application accesors
#endif
    
	/** Attribute accessor: get all application names
     @return #TTErr error code 
     @seealso ApplicationIntantiateLocal, ApplicationIntantiateDistant, ApplicationRelease, ApplicationGet */
	TTErr getApplicationNames(TTValue& returnedApplicationNames);
    
    /** Attribute accessor: get local application name
     @return #TTErr error code 
     @seealso ApplicationIntantiateLocal */
	TTErr getApplicationLocalName(TTValue& returnedApplicationLocalName);
    
    /** Attribute accessor: get local application
     @return #TTErr error code 
     @seealso ApplicationIntantiateLocal */
	TTErr getApplicationLocal(TTValue& returnedApplicationLocal);
    
#if 0
#pragma mark -
#pragma mark Protocol accesors
#endif
	
	/** Attribute accessor: get all protocol names
     @param value
     @return #TTErr error code 
     @seealso ProtocolInstantiate, ProtocolRelease, ProtocolGet */
	TTErr getProtocolNames(TTValue& value);
    
#if 0
#pragma mark -
#pragma mark Application factory
#endif
    
	/** Instantiate the local application passing a name
     @param inputValue      application name
     @param outputValue     application object
     @return #TTErr error code */
	TTErr ApplicationInstantiateLocal(const TTValue& inputValue, TTValue& outputValue);
    
    /** Instantiate a distante application passing a name
     @param inputValue      application name
     @param outputValue     application object
     @return #TTErr error code */
	TTErr ApplicationInstantiateDistant(const TTValue& inputValue, TTValue& outputValue);
    
    /** Release any application passing a name
     @param inputValue      application name
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationRelease(const TTValue& inputValue, TTValue& outputValue);
    
    /** Rename an application passing its old name and a new name
     @details : this method is hidden because it is only used internally by #TTApplication::setName
     @param inputValue      application old name and a new name
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationRename(const TTValue& inputValue, TTValue& outputValue);
    
    /** Get any application passing a name
     @param inputValue      application name
     @param outputValue     application object
     @return #TTErr error code */
	TTErr ApplicationFind(const TTValue& inputValue, TTValue& outputValue);
    
	/** Convenient method to notify application observers
     @param applicationName application name
     @param anApplication   application object
     @param flag            a #TTApplicationNotificationFlag
     @return #TTErr error code 
     @seealso ApplicationIntantiateLocal, ApplicationIntantiateDistant, ApplicationRelease */
	TTErr notifyApplicationObservers(TTSymbol applicationName, TTObject anApplication, TTApplicationNotificationFlag flag);
    
#if 0
#pragma mark -
#pragma mark Protocol factory
#endif
    
    /** Instantiate a protocol passing a name
     @param inputValue      protocol name
     @param outputValue     protocol object
     @return #TTErr error code */
	TTErr ProtocolInstantiate(const TTValue& inputValue, TTValue& outputValue);
    
    /** Release a protocol passing a name
     @param inputValue      protocol name
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ProtocolRelease(const TTValue& inputValue, TTValue& outputValue);
    
    /** Get any protocol passing a name
     @param inputValue      protocol name
     @param outputValue     protocol object
     @return #TTErr error code */
	TTErr ProtocolFind(const TTValue& inputValue, TTValue& outputValue);
    
#if 0
#pragma mark -
#pragma mark Application features
#endif
	
	/** Discover the namespace of an application under an address
     @param inputValue      <TTSymbol whereToDiscover>
     @param outputValue     <TTSymbol *returnedType, TTValuePtr returnedChildren,  TTValuePtr returnedAttributes>
     @return #TTErr error code */
	TTErr ApplicationDiscover(const TTValue& inputValue, TTValue& outputValue);
    
    /** Discover all the namespace of an application under an address
     @param inputValue      <TTSymbol whereToDiscover>
     @param outputValue     <TTNodePtr>
     @return #TTErr error code */
	TTErr ApplicationDiscoverAll(const TTValue& inputValue, TTValue& outputValue);
	
	/** Get a value from an attribute of an object at an address in an application
     @param inputValue      <TTSymbol whereToGet, TTSymbol attributeToGet, TTValuePtr returnedValue>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationGet(const TTValue& inputValue, TTValue& outputValue);
	
	/** Set a value from an attribute of an object at an address in an application
     @param inputValue      <TTSymbol whereToSet, TTSymbol attributeToSet, TTValuePtr newValue>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationSet(const TTValue& inputValue, TTValue& outputValue);
	
	/** Listen for value changes from an attribute of an object at an address in an application or for creation/destruction under an address.
     @param inputValue      <TTSymbol protocolName, TTSymbol applicationToNotify, TTSymbol whereToListen, TTBoolean enableListening>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationListen(const TTValue& inputValue, TTValue& outputValue);
	
	/** Update value changes of an attribute of an object at an address in an application or for creation/destruction under an address.
	 @details this is usually the answer of distant namespace or #TTMirror attribute listening
     @param inputValue      <TTSymbol appAnswering, TTSymbol whereComesFrom, TTValuePtr newValue>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ApplicationListenAnswer(const TTValue& inputValue, TTValue& outputValue);

#if 0
#pragma mark -
#pragma mark Protocol features
#endif
    
	/** Scan a protocol network in order to intantiate distant applications automatically
     @param inputValue      <TTSymbol protocolName>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ProtocolScan(const TTValue& inputValue, TTValue& outputValue);

	/** Run the reception thread mecanism of a Protocol
     @param inputValue      <TTSymbol protocolName>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ProtocolRun(const TTValue& inputValue, TTValue& outputValue);
	
	/** Stop the reception thread mecanism of a Protocol
     @param inputValue      <TTSymbol protocolName>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ProtocolStop(const TTValue& inputValue, TTValue& outputValue);

#if 0
#pragma mark -
#pragma mark Backup
#endif
    
	/**  Write all applications and protocols setup
     @param inputValue      <TTXmlHandlerPtr aXmlHandler>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
    
    /**  Convenient method used to write a protocol setup
     @param aXmlHandler     a xml handler object
     @param aProtocol       a protocol object
     @return #TTErr error code 
     @seealso WriteAsXml */
    TTErr writeProtocolAsXml(TTXmlHandlerPtr aXmlHandler, TTObject aProtocol);
    
    /**  Read an applications and protocols setup
     @param inputValue      <TTXmlHandlerPtr aXmlHandler>
     @param outputValue     nothing
     @return #TTErr error code */
	TTErr ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
    
#if 0
#pragma mark -
#pragma mark Public methods useful to ease access to application and protocol
#endif
    
public:
    
    /**  Get an application relative to a name
     @param applicationName #TTSymbol
     @return #TTApplicationPtr or NULL if the application doesn't exist */
    TTApplicationPtr findApplication(TTSymbol applicationName);
    
    /**  Get an application directory relative to a name
     @param applicationName #TTSymbol
     @return #TTNodeDirectoryPtr or NULL if the application doesn't exist */
    TTNodeDirectoryPtr findApplicationDirectory(TTSymbol applicationName);
    
    /**  Get local application
     @return #TTApplicationPtr or NULL if the application doesn't exist */
    TTApplicationPtr getApplicationLocal();
    
    /**  Get an application relative to an address
     @param anAddress       #TTAddress
     @return #TTApplicationPtr : mApplicationLocal for address without directory part or NULL if the directory part is not an application name */
    TTApplicationPtr findApplicationFrom(TTAddress anAddress);
    
    /**  Get an application directory relative to an address
     @param anAddress       #TTAddress
     @return #TTNodeDirectoryPtr : mApplicationLocal directory for address without directory part or NULL if the directory part is not an application name */
    TTNodeDirectoryPtr findApplicationDirectoryFrom(TTAddress anAddress);
    
    /**  Get application protocol names relative to a name
     @param applicationName #TTSymbol
     @return a #TTValue containing all protocol names */
    TTValue getApplicationProtocolNames(TTSymbol applicationName);
    
    /**  Get a protocol relative to a name
     @param protocolName    #TTSymbol
     @return #ProtocolPtr or NULL if the protocol doesn't exist */
    ProtocolPtr findProtocol(TTSymbol protocolName);
    
#if 0
#pragma mark -
#pragma mark Friendship
#endif
    
	friend TTBoolean TTMODULAR_EXPORT TTApplicationManagerGetLocalApplicationDebug();
	friend TTApplicationPtr TTMODULAR_EXPORT TTApplicationManagerGetApplicationFrom(TTAddress anAddress);
	
	friend TTValue TTMODULAR_EXPORT TTApplicationManagerGetApplicationProtocols(TTSymbol applicationName);
	
	friend TTErr TTMODULAR_EXPORT TTApplicationManagerAddApplicationObserver(TTSymbol anApplicationName, const TTObject anObserver);
	friend TTErr TTMODULAR_EXPORT TTApplicationManagerRemoveApplicationObserver(TTSymbol anApplicationName, const TTObject anObserver);
	
	friend TTErr TTMODULAR_EXPORT TTApplicationManagerProtocolActivityInCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTApplicationManagerProtocolActivityOutCallback(const TTValue& baton, const TTValue& data);
};

typedef TTApplicationManager* TTApplicationManagerPtr;

#if 0
#pragma mark -
#pragma mark Some Functions
#endif

/** Add a TTCallback as observer of application creation/destruction
 note : it uses the extern TTModularApplications variable
 @param anApplicationName		an application to observe
 @param observer				a TTCallbackPtr to add
 @return						an error code */
TTErr TTMODULAR_EXPORT TTApplicationManagerAddApplicationObserver(TTSymbol anApplicationName, const TTObject anObserver);

/** Remove a TTCallback as observer of application creation/destruction
 note : it uses the extern TTModularApplications variable
 @param anApplicationName		an application
 @param observer				a TTCallbackPtr to remove
 @return						a kTTErrGeneric if there isn't observer */
TTErr TTMODULAR_EXPORT TTApplicationManagerRemoveApplicationObserver(TTSymbol anApplicationName, const TTObject anObserver);

/** To get back raw incoming messages from any protocol
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTApplicationManagerProtocolActivityInCallback(const TTValue& baton, const TTValue& data);

/** To get back raw outputing messages from any protocol
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTApplicationManagerProtocolActivityOutCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_APPLICATION_MANAGER_H__
