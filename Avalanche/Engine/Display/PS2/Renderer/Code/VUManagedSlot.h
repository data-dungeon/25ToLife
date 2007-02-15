#ifndef __VU_MANAGED_SLOT_H
#define __VU_MANAGED_SLOT_H

#include "VUCodeSlot.h"

class	VUManagedSlot : public VUCodeSlot
{
	
public:
	unsigned int				d_UniqueID;
	unsigned int				d_GroupID;

	bool							b_Loaded;
	bool							b_Active;
	bool							b_InChain;
	bool							b_ActivateOnSwap;
	
	VUManagedSlot *			p_Next;

	VUManagedSlot(
		unsigned int				_VUAddr,
		unsigned int				_VUSize,
		unsigned int				_GroupID = 0,
		unsigned int				_UniqueID = 0,
		bool							_Loaded = false,
		bool							_Locked = false,
		bool							_Active = false
	);

	inline bool					inChain( void ) const 	{ return b_InChain; }
	inline void					Chain( bool bActivateMode ) { b_InChain = true; b_ActivateOnSwap = bActivateMode; }
	inline void					unChain( void )			{ b_InChain = false; }
	
	inline void					swapChain( void )
	{
		if( b_InChain)
		{
			b_InChain = false; b_Active = b_ActivateOnSwap;
		}
	}
	
	inline bool					isLoaded( void ) const 	{ return b_Loaded; }
	inline void					Load( void )				{ b_Loaded = true; }
	inline void 				unLoad( void )				{ b_Loaded = false; }
	
	inline bool					isActive( void ) const	{ return b_Active; }
	inline void					Activate( void )			{ b_Active = true; }
	inline void					unActivate( void )		{ b_Active = false; }
	
	inline unsigned int 		groupID( void ) const	{ return d_GroupID; }
	inline void					groupID( unsigned int _GroupID )			
		{ d_GroupID = _GroupID; }
	
	inline unsigned int		uniqueID( void ) const	{ return d_UniqueID; }
	inline void					uniqueID( unsigned int _UniqueID )
		{ d_UniqueID = _UniqueID; (_UniqueID == 0) ? Use( ) : unUse( ); }
	
	inline VUManagedSlot *	next( void ) const 				{ return p_Next; }
	inline void					next( VUManagedSlot *_next) 	{ p_Next = _next; }


} ;

#endif
