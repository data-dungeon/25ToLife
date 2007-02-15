#include "VUManagedSlot.h"

VUManagedSlot::VUManagedSlot( unsigned int _VUAddr, unsigned int _VUSize, unsigned int _UniqueID, unsigned int _GroupID, bool _Loaded, bool _Locked, bool _Active) :
	VUCodeSlot( _VUAddr, _VUSize, _Locked, _UniqueID ? true : false),
	d_UniqueID( _UniqueID),
	d_GroupID( _GroupID),
	b_Loaded( _Loaded),
	b_Active( _Active),
	b_InChain( false),
	p_Next( 0)
{

}

