//============================================================================
//=
//= ModeScript.h - stuff to load and run the "GameMode" script
//=
//============================================================================

#ifndef _MODE_SCRIPT_H
#define _MODE_SCRIPT_H

#include "GameHelper/Message.h"

class ScriptVM;

class CModeScript
{
public:
	CModeScript();
	~CModeScript();

	void			SetScript( const char* filename );
	void			SetMessageOwner( CMessageOwner* owner );
	void			ClearScript();
	void			Run();

	const char* Filename() const { return d_filename; }
	ScriptVM*	Script()   const { return d_script; }
	bool			IsValid()  const { return d_script != NULL; }

private:
	ScriptVM*		d_script;
	char				d_filename[ 32 ];
	CMessageOwner*	d_messageOwner;
	CMessageOwner	d_tempMessageOwner;
};

#endif //_MODE_SCRIPT_H
