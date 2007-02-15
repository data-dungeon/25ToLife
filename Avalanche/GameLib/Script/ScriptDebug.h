//===========================================================================
//=
//= ScriptDebug.h
//=
//===========================================================================

#ifndef _SCRIPT_DEBUG_H
#define _SCRIPT_DEBUG_H

#ifdef SCRIPT_COMPILER

struct FileNode;

class ScriptDebug
{
public:
	ScriptDebug();
	~ScriptDebug();
	const char* GetLine( const char* filename, int line );

private:
	FileNode* GetFile( const char* filename );

	FileNode*	m_files;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_DEBUG_H
