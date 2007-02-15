////////////////////////////////////////////////////////////////////////////
//
// TupperwarePch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "Log.cpp"
#include "Aggregate.cpp"
#include "List.cpp"
#include "Scalar.cpp"
#include "Comment.cpp"
#include "Iterator.cpp"
#include "memorymanager.cpp"
#include "Path.cpp"
#include "pool.cpp"
#include "tupstring.cpp"
#include "Atom.cpp"
#include "binaryloadsave.cpp"
#include "stringmanager.cpp"
#include "Codec.cpp"
#include "Reader.cpp"
#include "Writer.cpp"
#include "loadsave.cpp"
#endif // defined(PS2) && defined(SN)
