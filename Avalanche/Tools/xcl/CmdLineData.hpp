//==============================================================================
//
//  CmdLineData.hpp
//
//==============================================================================

#ifndef CMDLINEDATA_HPP
#define CMDLINEDATA_HPP

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdlib.h>
#include "TokenList.hpp"

//==============================================================================
//  TYPES  -- MUST CHANGE NAMES IN CmdLineData.cpp IF THESE CHANGE!
//==============================================================================

enum {
	TOOL_UNKNOWN = 0,
	TOOL_COMPILER,
	TOOL_LINKER,
	TOOL_LIBRARIAN,
};

enum {
	TARGET_UNKNOWN = 0,
	TARGET_PC,
	TARGET_PS2,
	TARGET_XBOX,
	TARGET_GAMECUBE,
	TARGET_END_OF_LIST,
};

enum {
	VENDOR_UNKNOWN = 0,
	VENDOR_MICROSOFT,
	VENDOR_SN,
	VENDOR_METROWERKS,
	VENDOR_END_OF_LIST,
};

enum {
	SUBSYS_DEFAULT = 0,
	SUBSYS_WINDOWS,
	SUBSYS_WINCON,
	SUBSYS_END_OF_LIST,
};

enum {
	APP_COMPILER = 0,
	APP_LINKER,
	APP_DEBUGGER,
	APP_COMUTIL,
	APP_ASSEMBLER,
	APP_VUASSEMBLER,
	APP_END_OF_LIST
};

#define APP_ARRAY_SIZE APP_END_OF_LIST

extern const char *pApplicationNames[];
//==============================================================================

class cmd_line_data
{
	public:
							cmd_line_data( void );
							~cmd_line_data( );

		void				Activate( int argc, char** argv );
		void				Dump( void );
		void				setApplication(int iAppName, const char * strPath, const char * strApplication);
		void				setApplication(const char * strAppName, const char * strPath, const char * strApplication);

		const char *	getTargetName( );

		token_list		m_Define;
		token_list		m_IncludeDir;
		token_list		m_LibraryDir;
		token_list		m_SourceCode;
		token_list		m_SourceOther;
		token_list		m_ObjectFile;
		token_list		m_LibraryFile;
		token_list		m_CompileOptions;
		token_list		m_LinkOptions;
		token_list		m_UnknownOption;

		int				m_argc;
		char				**m_argv;

		int				m_Tool;
		int				m_Target;
		int				m_Vendor;
		bool				m_Symbols;
		bool				m_Preprocess;
		bool				m_ForceC;
		bool				m_ForceCPP;
		bool				m_CompileOnly;
		bool				m_RTTI;
		bool				m_DebugLibs;
		bool				m_StdInc;
		bool				m_CreatePrecompiled;
		bool				m_UsePrecompiled;
		char				m_Optimization;
		char				m_TargetString[128];
		char				m_VendorString[128];
		char				m_PCHFile[_MAX_PATH];
		char				m_OutputFile[_MAX_PATH];
		char				m_OutputPath[_MAX_PATH];
		char				m_SBRPath[_MAX_PATH];
		char				m_PDBPath[_MAX_PATH];
		char				m_PrefixFile[_MAX_PATH];

		char				*m_applications[APP_ARRAY_SIZE];
		char				*m_app_paths[APP_ARRAY_SIZE];

	private:
		void				ProcessOption( char* pToken );
		void				ProcessFile( char* pToken );
};

//==============================================================================
#endif // CMDLINEDATA_HPP
//==============================================================================
