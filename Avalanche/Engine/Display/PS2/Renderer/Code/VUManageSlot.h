#ifndef __VU_MANAGED_SLOT_H
#define __VU_MANAGED_SLOT_H

#include "VUCodeSlot.h"

class	VUManagedSlot : public VUCodeSlot
{
	
public:
	unsigned int				d_CodeID;
	VUManagedSlot *			p_Next;

	VUManagedSlot(
		unsigned int				_VUAddr,
		unsigned int				_VUSize,
		bool							_Locked = false,
		bool							_InUse = false
	);


	unsigned int &				codeID( void ) { return d_CodeID; }
	unsigned int				codeID( void ) const { return d_CodeID; }

	void							useByID( unsigned int codeID) { d_CodeID = codeID; Use( ); }

	void							next( VUManagedSlot *_next) { p_Next = _next; }
	VUMangagedSlot *			next( void ) { return p_Next; }


}
