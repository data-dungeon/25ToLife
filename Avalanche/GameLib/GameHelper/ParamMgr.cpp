/*
**
**  File:   ParamMgr.cpp
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
**   $Archive: /Avalanche/gamelibNew/GameHelper/ParamMgr.cpp $
**  $Revision: 16 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#include "GameHelper/GameHelperPCH.h"

// My header (always first)
#include "GameHelper/ParamMgr.h"

// Gamelib includes
#include "cmdcon/cmdcon.h"

DeclareSingleton(CParamMgr);

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////  CParamMgr  ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/***************************************************************************/
/***************************************************************************/
CParamMgr::CParamMgr()
{
	d_contextHead = NULL;
}

/***************************************************************************/
/***************************************************************************/
CParamMgr::~CParamMgr()
{
	Clear();
}

/***************************************************************************/
/***************************************************************************/
void CParamMgr::Clear()
{
	CParamContext* currContext = d_contextHead;
	while (currContext)
	{
		CParamContext* context = currContext;
		currContext = currContext->d_nextContext;
		delete context;
	}
	d_contextHead = NULL;
}

/***************************************************************************/
/***************************************************************************/
CParamContext* CParamMgr::FindContext(const char* i_name, size_t i_typeSize)
{
	if (i_name && i_name[0])
	{
		CParamContext* currContext = d_contextHead;
		while (currContext)
		{
			if (currContext->d_name && currContext->d_name[0] &&
					!strcmp(i_name, currContext->d_name))
			{
				ASSERT(i_typeSize == currContext->GetSize());
				if (i_typeSize == currContext->GetSize())
					return(currContext);
			}
			currContext = currContext->d_nextContext;
		}
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
void CParamMgr::AddContext(CParamContext* i_context)
{
	i_context->d_nextContext = d_contextHead;
	d_contextHead = i_context;
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////  CParamContext  //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/***************************************************************************/
/***************************************************************************/
CParamContext::CParamContext()
{
/*
 *	Init list pointers.
 */
	d_nextContext = NULL;
	d_name = NULL;
}

/***************************************************************************/
/***************************************************************************/
CParamContext::~CParamContext()
{
	delete[] d_name;
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::Init(const char* i_name, const char* i_path)
{
/*
 *	Set up the console if a name is provided.
 */
	if (i_name && i_name[0])
	{
/*
 *		Save context name.
 */
		MEM_SET_ONE_SHOT_NEW_NAME("CParamContext::Init");
		d_name = new char[strlen(i_name) + 1];
		ASSERT(d_name);
		strcpy(d_name, i_name);
/*
 *		Find the .var file path.
 */
		char var[128];
		if ((i_path != NULL) && (i_path[0] != '\0'))
		{
			strcpy (var, i_path);
			strcat (var, "\\");
			strcat (var, d_name);
		}
		else
			strcpy (var, d_name);
/*
 *		Make sure things are clean and bind the context to the .var file.
 */
		g_console.DeleteContext(d_name);
		g_console.BindScript(d_name, var);
	}
	else
		d_name = NULL;
/*
 *	Register the sub-class specific variables.
 */
	RegisterVariables();
/*
 *	Complete console setup and update watch.
 */
	if (d_name)
	{
		g_console.ExecuteContext(d_name);
#ifndef INTERACTIVE_CONSOLE
		g_console.DeleteContext(d_name);
#endif //INTERACTIVE_CONSOLE
#ifdef ENABLE_WATCHCLIENT
		WatchClient::Instance()->Refresh(d_name);
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(float &i_variable, char* i_title, 
		float i_default, float i_min, float i_max, void* i_messageData)
{
	i_variable = i_default;

	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
		{
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_REAL,
					CONVAR_DEFAULT);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable,
					i_variable, i_min, i_max, i_title);
#endif
		}
		else
		{
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_REAL,
					CONVAR_DEFAULT, &d_messageOwner, i_messageData);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable,
					i_variable, i_min, i_max, i_title, &d_messageOwner,
					i_messageData);
#endif
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(int &i_variable, char* i_title,
		int i_default, int i_min, int i_max, void* i_messageData)
{
	i_variable = i_default;

	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
		{
			g_console.CreateVar(d_name, i_title, i_variable, false,
					CONVAR_DEFAULT);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable,
					i_variable, (float)i_min, (float)i_max, i_title);
#endif
		}
		else
		{
			g_console.CreateVar(d_name, i_title, i_variable, false, CONVAR_DEFAULT,
					&d_messageOwner, i_messageData);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable,
					i_variable, (float)i_min, (float)i_max, i_title,
					&d_messageOwner, i_messageData);
#endif
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(bool &i_variable, char* i_title,
		bool i_default, void* i_messageData)
{
	i_variable = i_default;

	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_DEFAULT);
		else
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_DEFAULT,
					&d_messageOwner, i_messageData);
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(char* i_variable, char* i_title,
		const char* i_default, int i_maxLength, void* i_messageData)
{
	strncpy(i_variable, i_default, i_maxLength);
	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
		{
			g_console.CreateVar(d_name, i_title, i_variable, i_maxLength,
					CONVAR_DEFAULT);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, i_variable,
					i_variable, i_title);
#endif
		}
		else
		{
			g_console.CreateVar(d_name, i_title, i_variable, i_maxLength,
					CONVAR_DEFAULT, &d_messageOwner, i_messageData);

#ifdef ENABLE_WATCHCLIENT
			WatchClient::Instance()->RegisterVariable(d_name, i_variable,
					i_variable, i_title,
					&d_messageOwner, i_messageData);
#endif
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(Vector3Packed *i_variable, char* i_title,
		Vector3CRef i_default, Vector3CRef i_min, Vector3CRef i_max,
		void* i_messageData)
{
	RegisterVariable(i_variable, i_title, i_default.x(), i_default.y(),
			i_default.z(), i_min.x(), i_min.y(), i_min.z(), i_max.x(), i_max.y(),
			i_max.z(), i_messageData);
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariable(Vector3Packed *i_variable, char* i_title,
		float i_defaultX, float i_defaultY, float i_defaultZ,
		float i_minX, float i_minY, float i_minZ,
		float i_maxX, float i_maxY, float i_maxZ, void* i_messageData)
{
	i_variable->Set(i_defaultX, i_defaultY, i_defaultZ);

	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
		{
			g_console.CreateVar(d_name, i_title, i_variable, false, CONVAR_DEFAULT);

#ifdef ENABLE_WATCHCLIENT
			char label[256];

			strcpy (label, i_title);
			strcat (label, " X");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_x,
					i_variable->x(), i_minX, i_maxX, label);

			strcpy (label, i_title);
			strcat (label, " Y");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_y,
					i_variable->y(), i_minY, i_maxY, label);

			strcpy (label, i_title);
			strcat (label, " Z");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_z,
					i_variable->z(), i_minZ, i_maxZ, label);
#endif
		}
		else
		{
			g_console.CreateVar(d_name, i_title, i_variable, false, CONVAR_DEFAULT,
					&d_messageOwner, i_messageData);

#ifdef ENABLE_WATCHCLIENT
			char label[256];

			strcpy (label, i_title);
			strcat (label, " X");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_x,
					i_variable->x(), i_minX, i_maxX, label, &d_messageOwner,
					i_messageData);

			strcpy (label, i_title);
			strcat (label, " Y");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_y,
					i_variable->y(), i_minY, i_maxY, label, &d_messageOwner,
					i_messageData);

			strcpy (label, i_title);
			strcat (label, " Z");
			WatchClient::Instance()->RegisterVariable(d_name, &i_variable->_z,
					i_variable->z(), i_minZ, i_maxZ, label, &d_messageOwner,
					i_messageData);
#endif
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CParamContext::RegisterVariableInv(float &i_variable, char* i_title, 
		float i_default, float i_min, float i_max, void* i_messageData)
{
	i_variable = i_default;

	if (d_name)
	{
		if (i_messageData == PARAM_NO_MESSAGE)
		{
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_INVREAL,
					CONVAR_DEFAULT);

#ifdef ENABLE_WATCHCLIENT
			char label[256];
			strcpy (label, "Inv");
			strcat (label, i_title);
			VAR_VAL val;
			val.rval = i_variable;
			WatchClient::Instance()->RegisterVariable(d_name, VARTYPE_LOG,
					(void*)&i_variable, val, i_min, i_max, label);
#endif
		}
		else
		{
			g_console.CreateVar(d_name, i_title, i_variable, CONVAR_INVREAL,
					CONVAR_DEFAULT, &d_messageOwner, i_messageData);

#ifdef ENABLE_WATCHCLIENT
			char label[256];
			strcpy (label, "Inv");
			strcat (label, i_title);
			VAR_VAL val;
			val.rval = i_variable;
			WatchClient::Instance()->RegisterVariable(d_name, VARTYPE_LOG,
					(void*)&i_variable, val, i_min, i_max, label, &d_messageOwner,
					i_messageData);
#endif
		}
	}
}
