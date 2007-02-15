/*************************************/
/*                                   */
/*   PigpenPCH.cpp                   */
/*   new engine  05/01/03            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   pre-compiled header             */
/*                                   */
/*************************************/

/* pre-compiled headers by doing up front work on a number of .h files, then having the results available when those
.h files are actually included. typically, there is a .cpp and .h files used to generate the pre-compiled header for a
project. in settings, the .cpp file is marked to "create precompiled header file." all other .cpp files are marked as
"use precompiled header file." choosing which .h files to include in a pre-compiled header is a balancing act between:

  wanting to include as many as possible because it improves performance
  not including rarely-include files since touching them will re-compile every file in the project

there is also a question about whether to include both .h files from inside and outside the project. for example, the
game project contains files like DBFile.h, and also depends on Layers include files. should the game pre-compiled header
include Layers include files and/or Game include files. i think this is also a balancing act, one we'll resolve through
trial and error */

/******************* includes ****************************/

/* system includes */

/* engine includes */

#ifdef CW
#define CW_PCH	// enable contents of .h file, see comments in .h file
#endif //CW
#include "PigpenPCH.h"

