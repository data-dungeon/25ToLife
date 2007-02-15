//==============================================================================
//
//  xCL - Cross Platform Compile and Link
//
//==============================================================================

#ifndef XCL_HPP
#define XCL_HPP

//==============================================================================
//   INCLUDES
//==============================================================================

#include "CmdLineData.hpp"

//==============================================================================
//   VARIABLE DECLARATIONS
//==============================================================================

extern bool					g_Verbose;
extern cmd_line_data		g_CmdLineData;

//==============================================================================
//  FUNCTION DECLARATIONS
//==============================================================================
                                        
void		OpenSection( char *pTitle );
void		DivideSection( void );
void		CloseSection( void );
                                        
void		DumpTextFileContent( char *pFileName );
void		DumpCommandLineAndResponseFile( char *pTitle, char *pCmdLine, char *pResponseFileName );
                                         
char *	UseOnlyBackSlashes( char *pPath );
char *	UseOnlySlashes( char *pPath );

//==============================================================================
#endif // XCL_HPP
//==============================================================================
