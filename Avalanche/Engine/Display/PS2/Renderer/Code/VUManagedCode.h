#ifndef __VU_MANAGED_CODE_H
#define __VU_MANAGED_CODE_H

#include "VUCodeBlock.h"
#include "VUDynamicMPG.h"

class VUManagedSlot;

class VUManagedCode : public VUCodeBlock
{

	VUDynamicMPG *				p_MPG;
	VUManagedSlot *			p_LockSlot;

	const unsigned int		d_UniqueID;	
	unsigned int				d_GroupID;
	
public:
	VUManagedCode(
		void *					_CodeBegin,
		void *					_CodeEnd
	) :
	VUCodeBlock( _CodeBegin, _CodeEnd),
	p_LockSlot( 0),
	d_UniqueID( (unsigned int) this),
	d_GroupID( 0)
	{
	
		p_MPG = VUDynamicMPG::create( _CodeBegin, _CodeEnd);
	
	}
	
	~VUManagedCode( void )
	{
		delete p_MPG;
	}
	
	VUManagedSlot *		lockSlot( void ) const				{ return p_LockSlot; }
	void						lockSlot( VUManagedSlot * _Slot)	{ p_LockSlot = _Slot; }
	
	VUDynamicMPG &			getMPG( void ) const 				{ return *p_MPG; }

	unsigned int			uniqueID( void ) const				{ return d_UniqueID; }
	
	unsigned int			groupID( void ) const				{ return d_GroupID; }
	void						groupID( unsigned int _GroupID) 	{ d_GroupID = _GroupID; }			
	
} ;

	
#endif
