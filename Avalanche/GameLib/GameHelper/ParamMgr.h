/*
**
**  File:   ParamMgr.h
**  Date:   July 25, 2002
**  By:     Bryant Collard
**  Desc:   Share a single set of parameters among a number of actors. The
**          parameter set will either be a single variable or a collection
**          of variables in a structure or class. If you want to use this in
**          some class CFoo, for example, first, declare the parameter
**          structure and (probably) a pointer to it.
**
**          class CFoo
**          {
**           public:
**             ...
**             struct SFooParam
**             {
**                float d_var1;
**                int d_var2;
**                ...
**             };
**             SFooParam* d_params;
**          };
**
**          Also, override the RegisterVariables function to add the variable
**          to the command console and to watch. You can use the
**          RegisterVariable functions to make your life easier but you don't
**          have to:
**
**          void CParam<CFoo::SFooParam>::RegisterVariables(void)
**          {
**             RegisterVariable(d_data.d_var1, "Var1", 1.0f, 0.0f, 1.0f);
**             RegisterVariable(d_data.d_var1, "Var2", 1.0f, 0.0f, 1.0f);
**             ...
**          }
**
**          Finally, get a pointer to a parameter set during initialization:
**
**          d_params = CParam<SFooParam>::GetParam(this, "context", "path");
**
**          The first such call will instanciate a set, call RegisterVariables,
**          and return a pointer to the set. Subsequent calls will return the
**          address of the set. Note that if a NULL pointer is passed for the
**          context, a new set will be instanciated, no contexts in the
**          command console and watch will be created, and calls to
**          RegisterVariable will just set the default values.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibNew/GameHelper/ParamMgr.h $
**  $Revision: 10 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifndef PARAM_MGR_H
#define PARAM_MGR_H

// System includes //
#include <stdio.h>

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "EngineHelper/Singleton.h"
#include "GameHelper/Message.h"

// Forward declarations //
class CParamContext;

// Defines //
#define PARAM_NO_MESSAGE ((void*)(~0x0u - 0x1u))

/***************************************************************************/
/***************************************************************************/
class CParamMgr
{
  public:
	CParamMgr();
	~CParamMgr();
	void Clear();
	CParamContext* FindContext(const char* i_name, size_t i_typeSize);
	void AddContext(CParamContext* i_context);

  private:
	CParamContext* d_contextHead;
};

/***************************************************************************/
/***************************************************************************/
class CParamContext
{
  public:
	CParamContext();
	virtual ~CParamContext();
	void Init(const char* i_name, const char* i_path);
	virtual void RegisterVariables(void) {}
	void RegisterVariable(float &i_variable, char* i_title, 
			float i_default, float i_min, float i_max,
			void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariable(int &i_variable, char* i_title,
			int i_default, int i_min, int i_max,
			void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariable(bool &i_variable, char* i_title, bool i_default,
			void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariable(char* i_variable, char* i_title, const char* i_default,
			int i_maxLength, void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariable(Vector3Packed *i_variable, char* i_title,
			Vector3CRef i_default, Vector3CRef i_min, Vector3CRef i_max,
			void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariable(Vector3Packed *i_variable, char* i_title,
			float i_defaultX, float i_defaultY, float i_defaultZ,
			float i_minX, float i_minY, float i_minZ,
			float i_maxX, float i_maxY, float i_maxZ,
			void* i_messageData = PARAM_NO_MESSAGE);
	void RegisterVariableInv(float &i_variable, char* i_title, 
			float i_default, float i_min, float i_max,
			void* i_messageData = PARAM_NO_MESSAGE);

  protected:
	virtual size_t GetSize(void) const = 0;
	char* d_name;
	CMessageOwner d_messageOwner;

  private:
	CParamContext* d_nextContext;

	friend class CParamMgr;
};

DefineSingleton(CParamMgr)
#define g_paramMgr GetSingleton(CParamMgr)

/***************************************************************************/
/***************************************************************************/
template<class Type>
class CParam : public CParamContext
{
  public:
	static Type* GetParam(const char* i_name, char* i_path,
			CParamMgr &i_paramMgr = g_paramMgr);
	void InitMessageHandler(const char* i_name);

	// Override this
	virtual void RegisterVariables(void) {}

	Type& GetData(void) {return (d_data);}

  protected:
	virtual size_t GetSize(void) const {return(sizeof(Type));}

  private:
	Type d_data;

	class CParamMessageHandler : public CMessageHandler
	{
	  public:
		// Override this if necessary
		virtual void HandleMessage(void* i_data, ActorHandle i_sender,
				bool i_posted) {}
		Type* d_data;
	} d_messageHandler;
};

/***************************************************************************/
/***************************************************************************/
template<class Type>
inline Type* CParam<Type>::GetParam(const char* i_name, char* i_path,
		CParamMgr &i_paramMgr)
{
	CParam<Type>* context = (CParam<Type>*) i_paramMgr.FindContext(i_name,
			sizeof(Type));

	if (context == NULL)
	{
		context = new CParam<Type>;
		ASSERT(context);
		context->InitMessageHandler(i_name);
		context->Init(i_name, i_path);
		i_paramMgr.AddContext(context);
	}

	return(&(context->d_data));
}

/***************************************************************************/
/***************************************************************************/
template<class Type>
inline void CParam<Type>::InitMessageHandler(const char* i_name)
{
	SET_MESSAGE_DISPATCHER_CALL_LOCATION();
	d_messageOwner.SetOwnerName("Param", i_name);
	d_messageHandler.d_data = &d_data;
	d_messageHandler.SetRules(MESSAGE_HANDLE_IF_MASTER);
	g_messageDispatcher.Register(&d_messageHandler, "ConsoleVarModified",
			d_messageOwner);
}

#endif // PARAM_MGR_H
