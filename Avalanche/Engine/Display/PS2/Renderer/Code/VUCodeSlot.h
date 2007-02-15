#ifndef __VU_CODE_SLOT_H
#define __VU_CODE_SLOT_H

class VUCodeSlot
{
	bool							b_InUse;
	bool							b_Locked;

	unsigned int				d_VUAddr;
	unsigned int				d_VUSize;

public:
	VUCodeSlot( 
		unsigned int			_VUAddr, 
		unsigned int			_VUSize, 
		bool						_Locked = false,
		bool						_InUse = false
		) ;

	bool						isLocked( void )	{ return b_Locked; }
	void						Lock( void )		{ b_Locked = true; }
	void						unLock( void )		{ b_Locked = false; }

	bool						inUse( void )		{ return b_InUse; }
	void						Use( void )			{ b_InUse = true; }
	void						unUse( void )		{ b_InUse = false; }

	unsigned int			vuAddr( void )		{ return d_VUAddr; }
	unsigned int			vuSize( void )		{ return d_VUSize; }
} ;

#endif
