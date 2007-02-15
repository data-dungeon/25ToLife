/*
**
**  File:   Message.h
**  Date:   February 18, 2004
**  By:     Bryant Collard
**  Desc:   A system for sending, posting, and receiving messages.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Message.h $
**  $Revision: #32 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

//Engine includes
#include "Math/Vector.h"
#include "Math/Matrix.h"

// Gamelib includes
#include "container/LinearHash.h"
#include "EngineHelper/Singleton.h"
#include "GameHelper/Handle.h"

// Message defines also used in scripts.
#include "GameHelper/MessageDefines.h"

// Forward declarations
class CPostedMessage;
class CMessageOwner;
class CMessageGroup;
class CMessagePair;
class CMessageDispatcher;
class CHost;
handledef(CActor) ActorHandle;

#ifndef CDROM
#define DEBUG_MESSAGE
#endif

#ifdef DEBUG_MESSAGE

#include "GameHelper/LogUtil.h"

#define MAX_OWNER_NAME_LENGTH  64

#define MESSAGE_LOG_CONFIG       0x01
#define MESSAGE_LOG_MEMORY       0x02
#define MESSAGE_LOG_SEND_LOCAL   0x04
#define MESSAGE_LOG_SEND_NETWORK 0x08

#endif

// Typedef for functions installed as translators
typedef uint32 (*TTranslatorSize)(void* i_data);
typedef void (*TTranslatorPack)(void* i_dataIn, void* i_dataOut,
		CMessageOwner &i_owner);
typedef void* (*TTranslatorUnpack)(void* i_data, CMessageOwner &i_owner);

// Macros for no-frills message handlers. These are here simply to reduce typing
// in commonly occurring situations.

//////////////////////////// FULL_MESSAGE_HANDLER ///////////////////////////

// Define a message handler derived class that can handle both sent and posted
// messages. Both PostData() and HandleMessage() need to be implemented. Also,
// the user must install a translator if needed. This macro can be used to
// define both stand-alone and nested classes.
#define FULL_MESSAGE_HANDLER(classType)                                     \
	class classType : public CMessageHandler                                 \
	{                                                                        \
	  public:                                                                \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner)                                         \
			{                                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage); \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	}

/////////////////////////// BASIC_MESSAGE_HANDLER ///////////////////////////

// Define a message handler derived class that can handle both sent and posted
// messages whose data will be of the specified basic type. HandleMessage()
// needs to be implemented. The macro implments PostData() and a translator is
// installed in Initialize(). This macro can be used to define both stand-alone
// and nested classes.
#define BASIC_MESSAGE_HANDLER(classType, dataType)                          \
	class classType : public C_##dataType##_MessageHandler                   \
	{                                                                        \
	  public:                                                                \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner)                                         \
			{                                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.InstallBasicDataTypeTranslator(i_name,      \
						dataType);                                                \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	}

//////////////////////////// SEND_MESSAGE_HANDLER ///////////////////////////

// Define a message handler derived class that can handle only sent
// messages. HandleMessage() needs to be implemented. An attempt to post a
// message to the handler will result in an assertion. Also, the user must
// install a translator if needed. This macro can be used to define both
// stand-alone and nested classes.
#define SEND_MESSAGE_HANDLER(classType)                                     \
	class classType : public CMessageHandler                                 \
	{                                                                        \
	  public:                                                                \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner)                                         \
			{                                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	}

////////////////////////// REF_FULL_MESSAGE_HANDLER /////////////////////////

// Define a message handler derived class with the same characteristics of one
// defined by FULL_MESSAGE_HANDLER augmented by a reference to the containing
// class. This macro is used to define a nested class. The resulting nested
// class is a friend of the containing class and the member variable, d_ref,
// points to the containing class. In this way, the handler has full access to
// the contianing class and acts as much like a member of the containing class
// as possible.
#define REF_FULL_MESSAGE_HANDLER(classType, refType)                        \
	class classType;                                                         \
	friend class classType;                                                  \
	class classType : public CMessageHandler                                 \
	{                                                                        \
	  public:                                                                \
		classType() {d_ref = NULL;}                                           \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner, refType* i_ref)                         \
			{                                                                  \
				d_ref = i_ref;                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage); \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	  private:                                                               \
		refType* d_ref;                                                       \
	}

///////////////////////// REF_BASIC_MESSAGE_HANDLER /////////////////////////

// Define a message handler derived class with the same characteristics of one
// defined by BASIC_MESSAGE_HANDLER augmented by a reference to the containing
// class. This macro is used to define a nested class. The resulting nested
// class is a friend of the containing class and the member variable, d_ref,
// points to the containing class. In this way, the handler has full access to
// the contianing class and acts as much like a member of the containing class
// as possible.
#define REF_BASIC_MESSAGE_HANDLER(classType, dataType, refType)             \
	class classType;                                                         \
	friend class classType;                                                  \
	class classType : public C_##dataType##_MessageHandler                   \
	{                                                                        \
	  public:                                                                \
		classType() {d_ref = NULL;}                                           \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner, refType* i_ref)                         \
			{                                                                  \
				d_ref = i_ref;                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.InstallBasicDataTypeTranslator(i_name,      \
						dataType);                                                \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	  private:                                                               \
		refType* d_ref;                                                       \
	}

////////////////////////// REF_SEND_MESSAGE_HANDLER /////////////////////////

// Define a message handler derived class with the same characteristics of one
// defined by SEND_MESSAGE_HANDLER augmented by a reference to the containing
// class. This macro is used to define a nested class. The resulting nested
// class is a friend of the containing class and the member variable, d_ref,
// points to the containing class. In this way, the handler has full access to
// the contianing class and acts as much like a member of the containing class
// as possible.
#define REF_SEND_MESSAGE_HANDLER(classType, refType)                        \
	class classType;                                                         \
	friend class classType;                                                  \
	class classType : public CMessageHandler                                 \
	{                                                                        \
	  public:                                                                \
		classType() {d_ref = NULL;}                                           \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner, refType* i_ref)                         \
			{                                                                  \
				d_ref = i_ref;                                                  \
				SET_MESSAGE_DISPATCHER_CALL_LOCATION();                         \
				SetRules(i_rules);                                              \
				g_messageDispatcher.Register(this, i_name, i_owner);            \
			}                                                                  \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted);                                                 \
	  private:                                                               \
		refType* d_ref;                                                       \
	}

////////////////////////////// CMessageHandler //////////////////////////////

class CMessageHandler
{
  public:
	CMessageHandler();
	virtual ~CMessageHandler();

	// Override to copy transient data into more persistant memory.
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage)
		{return(false);}

	// Override to actually handle a message.
	virtual void HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted) {}

	// Post a message to the owner's queue.
	void QueueMessage(void* i_data, ActorHandle i_sender);

	// Handling and routing rules.
	void SetRules(uint32 i_rules);
	uint32 GetRules(void) {return(d_rules);}

	// Make the handler get freed automatically when it's not owned.
	void DeleteWhenDefunct(bool i_deleteWhenDefunct);

	// Name of the message we handle
	const char* GetName(void);

  private:
	void Register(CMessageOwner &i_owner, CMessageGroup &i_group);
	void Unregister(void);

	CMessagePair* d_pair;
	CMessageHandler* d_next;

	uint32 d_rules;

	bool d_deleteWhenDefunct;

	friend class CPostedMessage;
	friend class CMessageOwner;
	friend class CMessageGroup;
	friend class CMessagePair;
	friend class CMessageDispatcher;
};

/////////////////////////////// CPostedMessage //////////////////////////////

class CPostedMessage
{
  public:
	CPostedMessage(CMessageHandler &i_handler, ActorHandle i_sender);
	~CPostedMessage();
	void SetData(void* i_data) {d_data = i_data;}
	void* AllocateData(uint32 i_size);

  private:
	CPostedMessage();
	void HandleMessage(void);

	CMessageHandler &d_handler;
	CPostedMessage* d_prevPost;
	CPostedMessage* d_nextPost;
	bool d_ownData;
	void* d_data;
	ActorHandle d_sender;

	friend class CMessageHandler;
	friend class CMessageOwner;
};

/////////////////////////////// CMessageOwner ///////////////////////////////

class CMessageOwner
{
  public:
	CMessageOwner();
	CMessageOwner(const char* i_ownerName);
	~CMessageOwner();
	void SetOwnerName(const char* i_ownerName);
	void SetOwnerName(const char* i_prefix, const char* i_suffix);

	void HandlePostedMessages(void);
	void HandlePostedMessages(const char* i_name);

	void UnregisterAll(void);

	void SetMaster(bool i_isMaster);
	bool IsMaster(void) {return(d_isMaster);}

	void SetSessionID(uint32 i_sessionID);
	uint32 GetSessionID(void) {return(d_sessionID);}

	void SetCoordinateSystem(Vector3 *i_ownerInWorld,
			DirCos3x3 *i_ownerToWorld)
		{d_ownerInWorld = i_ownerInWorld; d_ownerToWorld = i_ownerToWorld;}
	const Vector3 &OwnerInWorld(void) {return(*d_ownerInWorld);}
	const DirCos3x3 &OwnerToWorld(void) {return(*d_ownerToWorld);}

  private:
	void RemoveFromQueue(CMessageHandler* i_handler);
	CMessagePair* d_pairHead;
	CPostedMessage* d_queueHead;
	CPostedMessage* d_queueTail;

	bool d_isMaster;
	uint32 d_sessionID;

	const Vector3* d_ownerInWorld;
	const DirCos3x3* d_ownerToWorld;

	friend class CMessageHandler;
	friend class CPostedMessage;
	friend class CMessageGroup;
	friend class CMessagePair;
	friend class CMessageDispatcher;

#ifdef DEBUG_MESSAGE
	char d_ownerName[MAX_OWNER_NAME_LENGTH];
#endif
};

/////////////////////////////// CMessageGroup ///////////////////////////////

class CMessageGroup
{
  public:
	CMessageGroup(const char* i_name);
	~CMessageGroup();

	uint32 GetPackedSize(void* i_data);
	bool Pack(void* i_dataIn, void* i_dataOut, CMessageOwner &i_owner);
	void* Unpack(void* i_data, CMessageOwner &i_owner);

  private:
	CMessageGroup();
	bool InstallTranslator(TTranslatorPack i_packFunc,
			TTranslatorUnpack i_unpackFunc, TTranslatorSize i_sizeFunc,
			uint32 i_packedSize);

	CMessagePair* d_pairHead;
	uint32 d_packedSize;
	TTranslatorSize d_sizeFunc;
	TTranslatorPack d_packFunc;
	TTranslatorUnpack d_unpackFunc;

	friend class CPostedMessage;
	friend class CMessageHandler;
	friend class CMessageOwner;
	friend class CMessagePair;
	friend class CMessageDispatcher;

#ifdef DEBUG_MESSAGE
	void LogRegister(void);
	void LogSend(void);
	bool d_registered;
	bool d_sent;
	uint32 d_registerBeforeSend;
	uint32 d_registerAfterSend;
	uint32 d_sendBeforeRegister;
	uint32 d_sendAfterRegister;
#endif

	// Must be at the end.
	char d_name[1];
};

//////////////////////////////// CMessagePair ///////////////////////////////

class CMessagePair
{
  public:
	CMessagePair(CMessageOwner &i_owner, CMessageGroup &i_group);
	~CMessagePair();

	uint32 SendMessage(void* i_data, ActorHandle i_sender, uint32 i_handleRules);
	uint32 PostMessage(void* i_data, ActorHandle i_sender, uint32 i_handleRules);

	CMessageOwner &GetOwner(void) {return(d_owner);}
	const char* GetMessageName(void) {return(d_group.d_name);}
	uint32 GetSessionID(void) {return(d_owner.d_sessionID);}
	uint32 GetPackedSize(void* i_data);
	bool Pack(void* i_dataIn, void* i_dataOut);
	void* Unpack(void* i_data);

  private:
	CMessageOwner &d_owner;
	CMessageGroup &d_group;
	CMessageHandler *d_handlerHead;
	CMessagePair* d_ownerPrev;
	CMessagePair* d_ownerNext;
	CMessagePair* d_groupPrev;
	CMessagePair* d_groupNext;
	bool d_active;

	friend class CMessageHandler;
	friend class CPostedMessage;
	friend class CMessageOwner;
	friend class CMessageGroup;
	friend class CMessageDispatcher;
};

///////////////////////////// CMessageDispatcher ////////////////////////////

class CMessageDispatcher
{
  public:
	CMessageDispatcher();
	virtual ~CMessageDispatcher();

	void Purge(void);

	void Register(CMessageHandler* i_handler, const char* i_name,
			CMessageOwner &i_owner);
	void Unregister(CMessageHandler* i_handler);

	bool InstallFixedSizeTranslator(const char* i_name,
			TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
			uint32 i_packedSize);
	bool InstallVariableSizeTranslator(const char* i_name,
			TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
			TTranslatorSize i_sizeFunc);
	virtual bool InstallBasicDataTypeTranslator(const char* i_name,
			EMessageBasicDataType i_type) {return(false);}

	CMessagePair* GetMessagePair(const char* i_name, CMessageOwner &i_owner);
	CMessagePair* GetMessagePair(const char* i_name, CMessageOwner* i_owner);
	CMessagePair* GetMessagePair(const char* i_name, ActorHandle i_actor);

	CMessagePair* GetMessagePair(uint32 i_hash, CMessageOwner &i_owner);
	CMessagePair* GetMessagePair(uint32 i_hash, CMessageOwner* i_owner);
	CMessagePair* GetMessagePair(uint32 i_hash, ActorHandle i_actor);

	CMessageGroup* GetMessageGroup(const char* i_name);
	CMessageGroup* GetMessageGroup(uint32 i_hash);

	void SendMessageToAll(const char* i_name, void* i_data,
			ActorHandle i_sender);
	bool SendMessageToOwner(const char* i_name, void* i_data,
			ActorHandle i_sender, CMessageOwner &i_owner);
	bool SendMessageToOwner(const char* i_name, void* i_data,
			ActorHandle i_sender, CMessageOwner* i_owner);
	bool SendMessageToActor(const char* i_name, void* i_data,
			ActorHandle i_sender, ActorHandle i_actor);
	void SendMessageToSession(const char* i_name, void* i_data,
			ActorHandle i_sender);
	void SendMessageToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host);
	void SendMessageToLocalHost(const char* i_name, void* i_data,
			ActorHandle i_sender);
	void SendMessageToLocalHost(uint32 i_hash, void* i_data,
			ActorHandle i_sender);

	void PostMessageToAll(const char* i_name, void* i_data,
			ActorHandle i_sender);
	bool PostMessageToOwner(const char* i_name, void* i_data,
			ActorHandle i_sender, CMessageOwner &i_owner);
	bool PostMessageToOwner(const char* i_name, void* i_data,
			ActorHandle i_sender, CMessageOwner* i_owner);
	bool PostMessageToActor(const char* i_name, void* i_data,
			ActorHandle i_sender, ActorHandle i_actor);
	void PostMessageToSession(const char* i_name, void* i_data,
			ActorHandle i_sender);
	void PostMessageToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host);
	void PostMessageToLocalHost(const char* i_name, void* i_data,
			ActorHandle i_sender);
	void PostMessageToLocalHost(uint32 i_hash, void* i_data,
			ActorHandle i_sender);

	bool DoesOwnerHandleMessage(const char* i_name, CMessageOwner &i_owner)
		{return(GetMessagePair(i_name, i_owner) != NULL);}
	bool DoesOwnerHandleMessage(const char* i_name, CMessageOwner* i_owner)
		{return(GetMessagePair(i_name, i_owner) != NULL);}
	bool DoesActorHandleMessage(const char* i_name, ActorHandle i_actor)
		{return(GetMessagePair(i_name, i_actor) != NULL);}

	// Some utilities to help munge floats into simple messages
	static void *FloatToData(float f)
		{ return ((void *)*((uint32 *)&f)); }
	static float DataToFloat(void *i_data)
		{ return *((float *)((uint32 *)&i_data)); }
	static void *IntToData(int i)
		{ return (void *)i; }
	static int DataToInt(void *i_data)
		{ return (int)i_data; }
	static void *HandleToData(uint32 i)
		{ return (void *)i; }
	static uint32 DataToHandle(void *i_data)
		{ return (uint32)i_data; }
	static void *CharToData(const char *s)
		{ return (void *)s; }
	static const char *DataToChar(void *i_data)
		{ return (const char *)i_data; }

	// HACK!!! provide mechanism to block outgoing messages.
	void BlockOutgingMessages(void) {d_blockOutgoing = true;}
	void UnblockOutgingMessages(void) {d_blockOutgoing = false;}

	// Get at the currently active group and owner for debugging purposes.
	static const char* s_activeGroup;
	static const char* s_activeOwner;

	void DumpDebug(void);
	void EnableLog(bool i_writeLog);
	void ToggleLog(void);
	void SetLogFilter(uint8 i_logFilter);
#ifdef DEBUG_MESSAGE
	void WriteLogEntry(uint8 i_category, const char* i_action,
			CMessageHandler &i_handler, bool i_writeLocation, const char* i_notes);
	void WriteLogEntry(uint8 i_category, const char* i_action,
			CMessagePair &i_pair, CMessageHandler* i_handler, bool i_writeLocation,
			const char* i_notes);
	void WriteLogEntry(uint8 i_category, const char* i_action,
			CMessageGroup* i_group, CMessageOwner* i_owner, CMessagePair* i_pair,
			CMessageHandler* i_handler, bool i_writeLocation, const char* i_notes);
	static const char* s_debugFile;
	static int s_debugLine;
	int d_sendDepth;
#endif

  protected:
	virtual void SendToMaster(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void SendToDuplicas(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void SendToNeighbors(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void SendToSession(const char* i_name, void* i_data,
			ActorHandle i_sender) {}
	virtual void SendToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host) {}
	virtual void PostToMaster(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void PostToDuplicas(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void PostToNeighbors(void* i_data, ActorHandle i_sender,
			CMessagePair* i_pair) {}
	virtual void PostToSession(const char* i_name, void* i_data,
			ActorHandle i_sender) {}
	virtual void PostToHost(const char* i_name, void* i_data,
			ActorHandle i_sender, CHost* i_host) {}
	CMessageGroup* InstallTranslatorInGroup(const char* i_name,
			TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
			TTranslatorSize i_sizeFunc, uint32 i_packedSize);

  private:
	void SendMessage(void* i_data, ActorHandle i_sender, CMessagePair* i_pair);
	void PostMessage(void* i_data, ActorHandle i_sender, CMessagePair* i_pair);
	CMessageGroup* GetGroup(const char* i_name);

	// HACK!!! provide mechanism to block outgoing messages.
	bool d_blockOutgoing;

	CLinearHash<CMessageGroup*> d_groupTable;

#ifdef DEBUG_MESSAGE
	bool d_writeLog;
	CLogFile d_logFile;
	uint8 d_logFilter;
#endif
};

//////////////////////// Basic Data Message Handlers ////////////////////////

class C_MESSAGE_VALUE_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_ACTOR_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_ACTOR_HANDLE_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_STRING_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_VECTOR3_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_OWNED_VECTOR3_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_OWNED_POSITION_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_MATRIX3X3_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_OWNED_ORIENTATION_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_MATRIX4X4_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

class C_MESSAGE_OWNED_TRANSFORM_PTR_MessageHandler : public CMessageHandler
{
  public:
	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage);
};

DefineSingleton(CMessageDispatcher);

#ifdef DEBUG_MESSAGE

#define ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION(file, line)                   \
	do                                                                         \
	{                                                                          \
		CMessageDispatcher::s_debugFile = file;                                 \
		CMessageDispatcher::s_debugLine = line;                                 \
	} while(0)

#define SET_MESSAGE_DISPATCHER_CALL_LOCATION()                                \
	do                                                                         \
	{                                                                          \
		CMessageDispatcher::s_debugFile = __FILE__;                             \
		CMessageDispatcher::s_debugLine = __LINE__;                             \
	} while(0)

inline CMessageDispatcher &GetMessageDispatcher(const char* i_file, int i_line)
{
	CMessageDispatcher::s_debugFile = i_file;
	CMessageDispatcher::s_debugLine = i_line;
	return(GetSingleton(CMessageDispatcher));
}

#define g_messageDispatcher GetMessageDispatcher(__FILE__, __LINE__)

#else

#define ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION(file, line) ((void)0)
#define SET_MESSAGE_DISPATCHER_CALL_LOCATION() ((void)0)
#define g_messageDispatcher GetSingleton(CMessageDispatcher)

#endif

////////////////////////// CCallbackMessageHandler //////////////////////////

// Define a templated class that derives from the message handler and that
// uses a member function of a class to handle messages and. For example:
//
// class CFoo
// {
//   public:
//    void Initialize(void);
//
//   private:
//    void HandlerFunction(void* i_data, ActorHandle i_sender, bool i_posted);
//    CCallbackMessageHandler<CFoo> d_handler;
// };
//
// void CFoo::Initialize(void)
// {
//    d_handler.Initialize("Message", RULES, owner, this,
//          &CFoo::HandlerFunction);
// }
//
// void CFoo::FooMessageHandler(void* i_data, ActorHandle i_sender,
//       bool i_posted)
// {
//    ...
// }
//
// A PostData() callback function can be specified as well.

template<class Type>
class CCallbackMessageHandler : public CMessageHandler
{
  public:
	typedef bool (Type::*Poster)(void* i_data, CPostedMessage &i_postedMessage);
	typedef void (Type::*Handler)(void* i_data, ActorHandle i_sender,
			bool i_posted);

	CCallbackMessageHandler()
	{
		d_object = (Type*)NULL;
		d_poster = (Poster)NULL;
		d_handler = (Handler)NULL;
	}

	void Initialize(const char* i_name, uint32 i_rules, CMessageOwner &i_owner,
			Type* i_object, Handler i_handler, Poster i_poster = NULL)
	{
		d_object = i_object;
		d_poster = i_poster;
		d_handler = i_handler;

		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		SetRules(i_rules);
		g_messageDispatcher.Register(this, i_name, i_owner);
	}

	virtual bool PostData(void *i_data, CPostedMessage &i_postedMessage)
	{
		if ((d_object != NULL) && (d_poster != (Poster)NULL))
			return((d_object->*d_poster)(i_data, i_postedMessage));
		return(false);
	}

	virtual void HandleMessage(void* i_data, ActorHandle i_sender,
			bool i_posted)
	{
		if ((d_object != NULL) && (d_handler != (Handler)NULL))
			(d_object->*d_handler)(i_data, i_sender, i_posted);
	}

  private:
	Type* d_object;
	Poster d_poster;
	Handler d_handler;
};

/////////////////// Basic Data Callback Message Handlers ////////////////////

// Define a class template for each of the basic data types. Such a class can
// be declared as (for example):
//
//     MESSAGE_VALUE_CallbackMessageHandler<Type> handler;

#define BASIC_CALLBACK_MESSAGE_HANDLER(dataType)                            \
	template<class objType>                                                  \
	class dataType##_CallbackMessageHandler :                                \
			public C_##dataType##_MessageHandler                               \
	{                                                                        \
	  public:                                                                \
		typedef void (objType::*Handler)(void* i_data, ActorHandle i_sender,  \
				bool i_posted);                                                 \
		dataType##_CallbackMessageHandler()                                   \
		{                                                                     \
			d_object = (objType*)NULL;                                         \
			d_handler = (Handler)NULL;                                         \
		}                                                                     \
		void Initialize(const char* i_name, uint32 i_rules,                   \
				CMessageOwner &i_owner, objType* i_object, Handler i_handler)   \
		{                                                                     \
			d_object = i_object;                                               \
			d_handler = i_handler;                                             \
			SET_MESSAGE_DISPATCHER_CALL_LOCATION();                            \
			SetRules(i_rules);                                                 \
			g_messageDispatcher.InstallBasicDataTypeTranslator(i_name,         \
					dataType);                                                   \
			g_messageDispatcher.Register(this, i_name, i_owner);               \
		}                                                                     \
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,        \
				bool i_posted)                                                  \
		{                                                                     \
			if ((d_object != NULL) && (d_handler != (Handler)NULL))            \
				(d_object->*d_handler)(i_data, i_sender, i_posted);             \
		}                                                                     \
	  private:                                                               \
		objType* d_object;                                                    \
		Handler d_handler;                                                    \
	}

BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_VALUE);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_ACTOR_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_ACTOR_HANDLE);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_STRING);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_VECTOR3_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_OWNED_VECTOR3_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_OWNED_POSITION_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_MATRIX3X3_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_OWNED_ORIENTATION_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_MATRIX4X4_PTR);
BASIC_CALLBACK_MESSAGE_HANDLER(MESSAGE_OWNED_TRANSFORM_PTR);

#endif // MESSAGE_H
