/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Protocol interface
 *
 * @details
 *
 * @author Laurent Garnier, Théo de la Hogue
 *
 * @copyright © 2011, Laurent Garnier, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "TTFoundationAPI.h"

/** Declares instantiation and registration methods to add the protocol class as any #TTObject class */
#define PROTOCOL_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : Protocol(arguments)

/** Declares all members needed by any protocol class */
#define PROTOCOL_INITIALIZE \
mName = TTSymbol(thisTTClassName); \
mVersion = TTSymbol(thisProtocolVersion); \
mAuthor = TTSymbol(thisProtocolAuthor); \
mGet = TTBoolean(thisProtocolGet); \
mSet = TTBoolean(thisProtocolSet); \
mListen = TTBoolean(thisProtocolListen); \
mDiscover = TTBoolean(thisProtocolDiscover); \
mDiscoverAll = TTBoolean(thisProtocolDiscoverAll); \
registerAttribute(TTSymbol("parameterNames"), kTypeLocalValue, NULL, (TTGetterMethod)& thisTTClass::getParameterNames); \

/** Declares specific accessors methods to manage the parameter value in order to have one value per registered applications */
#define PROTOCOL_PARAMETER(name) \
TTErr get##name(TTValue& value) {TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); return getApplicationParameters(TTSymbol(_attrname_##name),value);}; \
TTErr set##name(const TTValue& value) {TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); return setApplicationParameters(TTSymbol(_attrname_##name),value);}; \

/** Add a protocol parameter as an attribute of the class */
#define addAttributeAsProtocolParameter(name, type) \
TTString _attrname_##name(#name); _attrname_##name.at(0)=tolower(_attrname_##name.at(0)); registerAttribute(_attrname_##name, type, NULL, (TTGetterMethod)& thisTTClass ::get##name, (TTSetterMethod)& thisTTClass ::set##name ); mParameterNames.append(TTSymbol(_attrname_##name)) \

/** Macro to update and get the local application name (to use only inside the protocol class) */
#define accessProtocolLocalApplicationName ProtocolGetLocalApplicationName(TTObjectBasePtr(this))

/****************************************************************************************************/
// Class Specification

/**	Protocol is the base class for all protocol protocol.
 It still has knowledge and support for ...
 */
class Protocol : public TTObjectBase {
	
protected:																																	
	TTObject                    mApplicationManager;				///< the application manager of the Modular framework.					
																	///< protocol programmers should not have to deal with this member.
    
    TTSymbol                    mLocalApplicationName;              ///< cache local application name
	
	TTObject                    mActivityInCallback;				///< a callback to trace raw incoming messages.
	TTObject                    mActivityOutCallback;				///< a callback to trace raw outputing messages.

    TTHash                      mApplicationParameters;             ///< ATTRIBUTE : hash table containing hash table of parameters
																	///< for each application registered for communication with this protocol
																	///< <TTSymbol applicationName, <TTSymbol parameterName, TTValue value>>
    
public:																															
	TTSymbol					mName;								///< ATTRIBUTE : the name of the protocol to use							
	TTSymbol					mVersion;							///< ATTRIBUTE : the version of the protocol								
	TTSymbol					mAuthor;							///< ATTRIBUTE : the author of the protocol
    TTBoolean					mGet;                               ///< ATTRIBUTE : is the Protocol allows to send get request ?
    TTBoolean					mSet;                               ///< ATTRIBUTE : is the Protocol allows to send set request ?
	TTBoolean					mListen;                            ///< ATTRIBUTE : is the Protocol allows to send listen request ?
	TTBoolean					mDiscover;                          ///< ATTRIBUTE : is the Protocol allows to send discover request ?
    TTBoolean					mDiscoverAll;                       ///< ATTRIBUTE : is the Protocol allows to send discover all request ?
	TTBoolean					mRunning;							///< ATTRIBUTE : is the Protocol reception thread enable ?
	TTBoolean					mActivity;							///< ATTRIBUTE : is the Protocol activity thread enable ?

    TTSymbol                    mSelectedApplication;               ///< internal symbol to select an application to access to its own parameters
                                                                    ///< note : a protocol developper have to use PROTOCOL_PARAMETERto declare specific accessors for a protocol parameter
                                                                    ///< this allows to manage one parameter value per registered application
    
    TTValue                     mParameterNames;                    ///< store the name of the protocol parameters

	/** Constructor 
     @param arguments           #TTApplicationManager object, #TTCallback for activity in, #TTCallback for activity out */
	Protocol(const TTValue& arguments);
	
	/** Destructor */
	virtual ~Protocol();

	/** Set application manager 
     @param[in] value           #TTApplicationManager object
     @return #TTErr error code */
	TTErr setApplicationManager(const TTValue& value);
	
	/** Register an application as a client of the protocol 
     @details This method allocate a TTHash to store parameters
     @param[in] inputValue      #TTSymbol application name
     @param[out] outputValue    nothing
     @return #TTErr error code */
	TTErr ApplicationRegister(const TTValue& inputValue, TTValue& outputValue);
    
    /** Rename an application
     @param[in] inputValue      #TTSymbol old application name, #TTSymbol new application name
     @param[out] outputValue    nothing
     @return #TTErr error code */
    TTErr ApplicationRename(const TTValue& inputValue, TTValue& outputValue);
	
	/** Unregister an application as a client of the protocol 
     @details This method deallocate a TTHash used to store parameters
     @param[in] inputValue      #TTSymbol application name
     @param[out] outputValue    nothing
     @return #TTErr error code */
	TTErr ApplicationUnregister(const TTValue& inputValue, TTValue& outputValue);
    
    /** Select an application to access to its own parameters value
     @param[in] inputValue      #TTSymbol application name
     @param[out] outputValue    nothing
     @return #TTErr error code
     @seealso getApplicationParameters, setApplicationParameters, PROTOCOL_PAREMETER_ACCESSORS */
	TTErr ApplicationSelect(const TTValue& inputValue, TTValue& outputValue);
    
    /** Select the local application to access to its own parameters value
     @return #TTErr error code
     @seealso getApplicationParameters, setApplicationParameters, PROTOCOL_PAREMETER_ACCESSORS */
    TTErr ApplicationSelectLocal();
    
    /** Get parameters needed by this protocol
     @param[out] value      returned parameter names */
	TTErr getParameterNames(TTValue& value) {value = mParameterNames; return kTTErrNone;};
	
	/** Internal accessor to get the parameter of an application
     @param[in] parameterName   the name of the parameter to get
     @param[out] value          returned the selected parameter for the selected application */
	TTErr getApplicationParameters(TTSymbol parameterName, TTValue& value);
	
	/** Internal accessor to set the parameter of an application
     @param[in] parameterName   the name of the parameter to set
     @param[in] value           value for selected parameter for the selected application */
	TTErr setApplicationParameters(TTSymbol parameterName, const TTValue& value);

    /** Get the names of the registered applications */
	TTErr getApplicationNames(TTValue& value);
    
    /** Is an application registered for this protocol ? */
	TTErr isRegistered(const TTValue& inputValue, TTValue& outputValue);

    
    /** Scan to find remote applications and add them to the application manager
     @param inputValue			: anything needed for scanning
	 @param outputValue			: all remote device informations
     @return errorcode			: return a kTTErrGeneric if the protocol fails to start or if it was running already
     */
	virtual TTErr Scan(const TTValue& inputValue, TTValue& outputValue)=0;
	
	/*!
     * Run reception thread mechanism for the local application only
     @param inputValue			: nothing to run all registered applications or a #TTSymbol application name
	 @param outputValue			: any informations relative to a failure when running the protocol
     @return errorcode			: return a kTTErrGeneric if the protocol fails to start or if it was running already
     */
	virtual TTErr Run(const TTValue& inputValue, TTValue& outputValue)=0;
	
	/*!
     * Stop the reception thread mechanism for the local application only
     @param inputValue			: nothing to stop all registered applications or a #TTSymbol application name
	 @param outputValue			: any informations relative to a failure when stopping the protocol
     @return errorcode			: return a kTTErrGeneric if the protocol fails to stop or if it was already stopped
     */
	virtual TTErr Stop(const TTValue& inputValue, TTValue& outputValue)=0;
	
	/**************************************************************************************************************************
	 *
	 *	SEND REQUEST METHODS
	 *
	 **************************************************************************************************************************/
	
	/*!
	 * Send a discover request to an application to get a part of the namespace at the given address
	 *
 	 @param to					: the application where to discover
	 @param address				: the address to discover
     @param returnedType          : the type of the node at the address (default is none which means no type)
	 @param returnedChildren      : all names of nodes below the address
	 @param returnedAttributes	: all attributes of the node at the address
     @param tryCount              : number of try for this request
	 @return errorcode			: kTTErrNone means the answer has been received, kTTErrValueNotFound means something is bad in the request
	 else it returns kTTErrGeneric if no answer or timeout
	 */
	virtual TTErr SendDiscoverRequest(TTSymbol to, TTAddress address,
                                      TTSymbol& returnedType,
									  TTValue& returnedChildren,
									  TTValue& returnedAttributes,
                                      TTUInt8 tryCount=0)=0;
    
    /*!
	 * Send a discover all request to an application to fill all the directory under this address
	 *
 	 @param to					: the application where to discover
	 @param address				: the address to discover
     @param node                  : the node for this address
     @param tryCount              : number of try for this request
	 @return errorcode			: kTTErrNone means the answer has been received, kTTErrValueNotFound means something is bad in the request
	 else it returns kTTErrGeneric if no answer or timeout
	 */
	virtual TTErr SendDiscoverAllRequest(TTSymbol to, TTAddress address,
                                         TTNodePtr node,
                                         TTUInt8 tryCount=0)=0;
	
	/*!
	 * Send a get request to an application to get a value at the given address
	 *
 	 @param to					: the application where to get
	 @param address				: the address to get
	 @param returnedValue			: the value which is going to be filled
     @param tryCount              : number of try for this request
	 @return errorcode			: kTTErrNone means the answer has been received, kTTErrValueNotFound means something is bad in the request
	 else it returns kTTErrGeneric if no answer or timeout
	 */
	virtual TTErr SendGetRequest(TTSymbol to, TTAddress address, 
								 TTValue& returnedValue,
                                 TTUInt8 tryCount=0)=0;
	
	/*!
	 * Send a set request to set a value of a specific application
	 *
	 @param to					: the application where to set
	 @param address				: the address to set
	 @param value					: anything to send
     @param tryCount              : number of try for this request
	 @return errorcode			: kTTErrNone means the answer has been received, kTTErrValueNotFound means something is bad in the request
	 */
	virtual TTErr SendSetRequest(TTSymbol to, TTAddress address, 
								 TTValue& value,
                                 TTUInt8 tryCount=0)=0;
	
	/*!
	 * Send a listen request to a specific application
	 *
	 @param to					: the application where to listen
	 @param address				: the address to listen
	 @param attribute				: the attribute to listen
	 @param enable				: enable/disable the listening
     @param tryCount              : number of try for this request
	 @return errorcode			: kTTErrNone means the answer has been received, kTTErrValueNotFound means something is bad in the request
	 */
	virtual TTErr SendListenRequest(TTSymbol to, TTAddress address, 
									TTBoolean enable,
                                    TTUInt8 tryCount=0)=0;
	
	
	/**************************************************************************************************************************
	 *
	 *	SEND ANSWER METHODS
	 *
	 **************************************************************************************************************************/
	
	/*!
	 * Send a disover answer to a application which ask for.
	 *
	 @param to					: the application where to send answer
	 @param address				: the address where comes from the description
     @param returnedType          : the type of the node at the address (default is none which means no type)
	 @param returnedChildren      : all names of nodes below the address
	 @param returnedAttributes	: all attributes the node at the address
	 */
	virtual TTErr SendDiscoverAnswer(TTSymbol to, TTAddress address,
                                     TTSymbol& returnedType,
									 TTValue& returnedChildren,
									 TTValue& returnedAttributes,
									 TTErr err=kTTErrNone)=0;
    
    /*!
	 * Send a discover answer to a application which ask for.
	 *
	 @param to					: the application where to send answer
	 @param address				: the address where comes from the description
     @param node                  : the node for this address
	 */
	virtual TTErr SendDiscoverAllAnswer(TTSymbol to, TTAddress address,
                                        TTNodePtr node,
                                        TTErr err=kTTErrNone)=0;
	
	/*!
	 * Send a get answer to a application which ask for.
	 *
	 @param to					: the application where to send answer
	 @param address				: the address where comes from the value
	 @param returnedValue			: the value of the attribute at the address
	 */
	virtual TTErr SendGetAnswer(TTSymbol to, TTAddress address, 
								const TTValue& returnedValue,
								TTErr err=kTTErrNone)=0;
	
	/*!
	 * Send a listen answer to a application which ask for.
	 *
	 @param to					: the application where to send answer
	 @param address				: the address where comes from the value
	 @param returnedValue			: the value of the attribute at the address
	 */
	virtual TTErr SendListenAnswer(TTSymbol to, TTAddress address, 
								   const TTValue& returnedValue,
								   TTErr err=kTTErrNone)=0;
	
	/**************************************************************************************************************************
	 *
	 *	RECEIVE REQUEST METHODS (BUILT-IN METHODS)
	 *
	 **************************************************************************************************************************/

	/*!
	 * Notify the protocol that an application ask for a namespace description
	 *
	 * !!! This a built-in protocol method which sends automatically the answer (or a notification if error)
	 *
	 @param from					: the application where comes from the request
	 @param address				: the address the application wants to discover
	 */
	TTErr ReceiveDiscoverRequest(TTSymbol from, TTAddress address);
    
    /*!
	 * Notify the protocol that an application ask for a namespace description
	 *
	 * !!! This a built-in protocol method which sends automatically the answer (or a notification if error)
	 *
	 @param from					: the application where comes from the request
	 @param address				: the address the application wants to discover
	 */
	TTErr ReceiveDiscoverAllRequest(TTSymbol from, TTAddress address);
	
	/*!
	 * Notify the protocol that an application ask for value
	 *
	 * !!! This a built-in protocol method which sends automatically the answer (or a notification if error)
	 *
	 @param from					: the application where comes from the request
	 @param address				: the address the application wants to get
	 */
	TTErr ReceiveGetRequest(TTSymbol from, TTAddress address);
	
	/*!
	 * Notify the protocol that an application wants to set value
	 *
	 * !!! This a built-in protocol method which set automatically the value (or send a notification if error)
	 *
	 @param from					: the application where comes from the request
	 @param address				: the address the application wants to get
	 @param newValue				: the incoming value
	 */
	TTErr ReceiveSetRequest(TTSymbol from, TTAddress address, const TTValue& newValue);
	
	/*!
	 * Notify the protocol that an application wants to listen (or not) the namespace
	 *
	 * !!! This a built-in protocol method which create/remove automatically the listener (or send a notification if error)
	 *
	 @param from					: the application where comes from the request
	 @param address				: the address the application wants to listen
	 @param enable				: enable/disable the listening
	 */
	TTErr ReceiveListenRequest(TTSymbol from, TTAddress address, TTBoolean enable);
	
	/**************************************************************************************************************************
	 *
	 *	RECEIVE ANSWER METHODS : each protocol deals with answers when it send a request except for listening mechanism
	 *							note : maybe we could add a listen answer manager because each protocol have to remember 
	 *							which listen requests has been done (?)
	 *
	 **************************************************************************************************************************/
	
	/*!
	 * Notify the protocol that an application answers to a listen request
	 *
	 * !!! This a built-in protocol method notify automatically the listener (or send a notification if error)
	 *
	 @param from					: the application where comes from the answer
	 @param address				: the address where comes from the answer
	 @param newValue				: the answered value
	 */
	TTErr ReceiveListenAnswer(TTSymbol from, TTAddress address, const TTValue& newValue);
	
	/**************************************************************************************************************************
	 *
	 *	ACTIVITY METHOD : a set of built-in methods to report raw incoming or outputing messages to the manager
	 *
	 **************************************************************************************************************************/
	
	/*!
	 * Notify the protocol that a message is arriving
	 *
	 * !!! This a built-in protocol method
	 *
	 @param message				: an incoming message
	 */
	TTErr ActivityInMessage(const TTValue& message);
	
	/*!
	 * Notify the protocol that a message is outputing
	 *
	 * !!! This a built-in protocol method
	 *
	 @param message				: an outputing message
	 */
	TTErr ActivityOutMessage(const TTValue& message);
	
	
	
	friend TTErr TT_EXTENSION_EXPORT ProtocolDirectoryCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TT_EXTENSION_EXPORT ProtocolAttributeCallback(const TTValue& baton, const TTValue& data);
	
	friend TTErr TT_EXTENSION_EXPORT ProtocolGetAttributeCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TT_EXTENSION_EXPORT ProtocolSetAttributeCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TT_EXTENSION_EXPORT ProtocolSendMessageCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TT_EXTENSION_EXPORT ProtocolListenAttributeCallback(const TTValue& baton, const TTValue& data);
    
};
typedef Protocol* ProtocolPtr;

/**	Called when an application directory send a 
 notification to registered application observers
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolDirectoryCallback(const TTValue& baton, const TTValue& data);

/**	Called when an application object attribute send a 
 notification to registered application observers
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolAttributeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolGetAttributeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolSetAttributeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolSendMessageCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TT_EXTENSION_EXPORT ProtocolListenAttributeCallback(const TTValue& baton, const TTValue& data);

#endif	//__PROTOCOL_H__

#ifndef __PROTOCOLLIB_H__
#define __PROTOCOLLIB_H__

class TT_EXTENSION_EXPORT ProtocolLib {
public:
	
	/**	Return a list of all available protocols. */
	static void getProtocolNames(TTValue& protocolNames);
};

#endif	//__PROTOCOLLIB_H__
