#ifndef __DDMA_OBJECT_CLASS_IDS_H
#define __DDMA_OBJECT_CLASS_IDS_H

enum DMAObject_Flags
{
	OBJ_FLAG_VERTEX_COLORS 			= 0x00000001,
	OBJ_FLAG_TEXTURE_COORDS			= 0x00000002,
	OBJ_FLAG_VERTEX_NORMALS			= 0x00000004,

	// Object texture patching has occured
	OBJ_FLAG_TEXTURE_PATCHED		= 0x00000008,

	// has some Environment Mapping
	OBJ_FLAG_ENVIRONMENT_MAPPED	= 0x00000010,

		// this flag means that the object needs to be patched
		// before it can be used.  Patching results in a new
		// object that refers to data in the old object.
		// The patching routine is not genralized.  There is a
		// different patch routine for each handler (VUDataClass)
		// that supports it (VUPatchingDataClass).  You must first
		// find the correct handler, then use it to create the
		// patched object.  The PS2Renderer::PatchObject() calls
		// do this work for you.  Remember that the new patched
		// object refers to the old object's data.  So dont free
		// the old object until your done with all the new objects.

	OBJ_FLAG_NEED_PATCHING			= 0x00100000,

	// when the DDMAObject destructor runs it will free( p_DMAData);..
	OBJ_FLAG_OWNS_DMADATA			= 0x00200000,						 

	// Which matrix buffer (0 or 1) is currently in use.  
	// Only actually used in DDMAObject
	OBJ_FLAG_SECOND_MATRIX_BUFFER = 0x00400000,

	// needs to be textured dynamically (draw-time)
	OBJ_FLAG_DYNAMIC_TEXTURE 		= 0x00800000,
} ;

enum DDMAObject_DCID
{
	DATA_STATIC_CC 	= 0,				// static data - compressed colors
	DATA_STATIC_UC 	= 1,	 			// static data - uncompressed colors
	DATA_ANIMATED 		= 3,	 	 		// animated data

	DATA_DYNAMIC 		= 5	 			// dynamic
} ;


enum DDMAObject_FCID
{
	FIXUP_STATIC 		= 0,				// static fixup data
	FIXUP_PATCH_SETS	= 1,				// multiple color set data
	FIXUP_ANIMATED		= 2
}  ;

#endif
