/**********************************************/
/*                                            */
/* NameLibrary.h                              */
/* big juju  2/24/02                          */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* register stuff by name                     */
/*                                            */
/**********************************************/

#ifndef __NAMELIBRARY_H
#define __NAMELIBRARY_H

#include "Game/Database/DataTypes.h"

class FileStream;

namespace NameLibrary
{
	// FileStream
	bool RegisterFileStream(const FileStream *pData, const char *pName);
	FileStream *FindFileStream(const char *pName);
	void ReleaseFileStreams( bool removePermanent );
};

#endif // __NAMELIBRARY_H
