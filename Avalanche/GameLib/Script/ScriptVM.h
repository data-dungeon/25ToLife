//===========================================================================
//=
//= ScriptVM.h
//=
//===========================================================================

#ifndef _SCRIPT_VM_H
#define _SCRIPT_VM_H

#include "Script/ScriptTypes.h"
#include "Script/ScriptExecutable.h"
#include "Script/ScriptVarList.h"

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
#include "Math/Vector.h"
#define HAVE_VECTOR3
#include "GameHelper/Handle.h"
handledef(CActor) ActorHandle;
class CActor;
#endif

class CMessageOwner;
class ScriptMessageHandler;

#define SCRIPT_PC_STACK_SIZE	16
#define SCRIPT_STACK_SIZE		512

class ScriptVM
{
private:
	friend class ScriptSystem;
	ScriptVM( ScriptExecutable* executable, ScriptVarList*& stack, int& stackDepth );
	~ScriptVM();

public:
	void Run( float elapsedTime, int stackDelta = 0 );

	// variable access routines
	void	SetVar( int varIndex, int value );
	void	SetVar( int varIndex, float value );
	void	SetVar( int varIndex, char* value );
	void	SetVar( int varIndex, int type, ScriptValue value );
	void	SetVar( const char* name, int value );
	void	SetVar( const char* name, float value );
	void	SetVar( const char* name, char* value );
	void	SetVar( const char* name, int type, ScriptValue value );
	int	GetInt( const char* name );
	float	GetFloat( const char* name );
	char*	GetString( const char* name );
	ScriptVar::VarType GetType( const char* name );

	// stack access routines
	void	Push( int value );
	void	Push( float value );
	void	Push( char* value );
	void	Push( void* value, ScriptVar::VarType type );
	int	PopInt();
	bool	PopBool();
	float	PopFloat();
	char*	PopString();
	void*	PopTypeless();
	ScriptVar PopVar();

	ScriptVar::VarType PeekType();

	#ifdef HAVE_VECTOR3
	Vector3	PopVector3();
	void		Push( Vector3CRef value );
	#endif

	#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
	CActor*	PopActor();
	ActorHandle	PopActorHandle();
	void		Push( CActor* actor );
	void		Push( ActorHandle handle );
	#endif

	void ClearStacks();
	void JumpToLabel( const char* labelName );
	void CallFunction( const char* functionName, int stackDelta = 0 );
	bool HasFunction( const char* functionName );

	inline int StackDepth()   const { return m_stackDepth;   }
	inline int PCStackDepth() const { return m_pcStackDepth; }

	inline void  SetOwner( void* owner )  { m_owner = owner;    }
	inline void  SetAction( int action )  { m_action = action;  }
	inline void* GetOwner() const         { return m_owner;     }
	inline int   GetAction()              { return m_action;    }

	void	RegisterMessageHandlers( CMessageOwner* messageOwner );
	void	SetMessageHandlerRules( const char* message, unsigned int rules );
	void	RemoveMessageHandler( const char* message );
	void	RemoveAllMessageHandlers();
	void	RouteMessageToFunction( const char* message, const char* functionName );

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
	int  GetMasterMetaDataSize();
	bool HasMasterMetaDataChangedSinceLastWrite();
	void WriteMasterMetaData( void* &io_buffer );
	int  GetMasterDataSize();
	bool HasMasterDataChangedSinceLastWrite();
	void WriteMasterData( void* &io_metaDataBuffer, void* &io_dataBuffer );
	void ReadMasterData( void* &io_metaDataBuffer, void* &io_dataBuffer );
#endif

	const char* GetScriptFileNameOfPC( void ) { return m_executable->m_debugInfo ? m_executable->m_debugInfo[ m_pc ].file : m_executable->m_sourceFilename; }
	const int GetScriptLineNumberOfPC( void ) { return m_executable->m_debugInfo ? m_executable->m_debugInfo[ m_pc ].line : 0; }

	static void GetDebugInfo( char* infoStr );

private:
	ScriptExecutable*	m_executable;
	ScriptVarList*		m_vars;
	ScriptVarList*&	m_stack;
	short*				m_pcStack;
	int					m_pc;
	int&					m_stackDepth;
	int					m_pcStackDepth;
	float					m_sleepTime;
	void*					m_owner;
	int					m_action;
	int					m_running;
	ScriptMessageHandler*	m_messageHandlers;
	CMessageOwner*				m_messageOwner;

	friend class ScriptMessageHandler;

	ScriptVar::VarType DetermineMessageHandlerDataType( int labelIndex );
};

#endif //_SCRIPT_VM_H
