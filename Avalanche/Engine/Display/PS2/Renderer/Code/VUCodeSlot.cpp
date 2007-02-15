#include "VUCodeSlot.h"

VUCodeSlot::VUCodeSlot( unsigned int _VUAddr, unsigned int _VUSize, bool _Locked, bool _InUse) :
	b_InUse( _InUse),
	b_Locked( _Locked),
	d_VUAddr( _VUAddr),
	d_VUSize( _VUSize)
{

}

