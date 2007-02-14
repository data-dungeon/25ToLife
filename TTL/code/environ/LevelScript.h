//============================================================================
//=
//= LevelScript.h
//=    code to run a "master" script for a level/world
//=
//============================================================================

#ifndef __LEVELSCRIPT_H
#define __LEVELSCRIPT_H

#include "GameHelper/Message.h"

class ScriptVM;
class ScriptExecutable;

class CLevelScript
{
public:
	static CLevelScript* Load( const char* filename );
	CLevelScript( ScriptExecutable* exe );
	~CLevelScript();
	void Run();
	ScriptVM* Script() const { return d_script; }

private:
	ScriptVM*		d_script;
	CMessageOwner	d_messageOwner;
};

#endif // __LEVELSCRIPT_H
