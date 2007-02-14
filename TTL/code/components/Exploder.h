//============================================================================
//=
//= CExploder - A component that will manage an actor getting split into
//=             many pieces.
//=
//============================================================================

#ifndef _EXPLODER_H
#define _EXPLODER_H

#include "Components/Component.h"
#include "container/Darray.h"

//============================================================================

class CExploderPiece;

//============================================================================

class CExploder : public CActorComponent
{
public:
	CExploder::CExploder( CCompActor& i_actor );
	~CExploder();

	virtual const char *ComponentName()  { return "Exploder"; }
//	const char *ComponentClass() { return "CExploder"; }

	virtual void Initialize( void );
//	virtual void SetAlpha( float alpha );

	void Reset();
	void AttachPieces( const char* piecename );
	void Explode( Vector3 dir );
	void AttemptUpdate( float i_deltaSec );
	void MunitionImpact( const CMunitionImpact &hit );

	REF_SEND_MESSAGE_HANDLER(MunitionImpact, CExploder) munitionImpactHandler;
	REF_SEND_MESSAGE_HANDLER(CDie, CExploder) deathHandler;
	REF_SEND_MESSAGE_HANDLER(SlaveDie, CExploder) slaveDeathHandler;

private:
	DynamicArray<CExploderPiece*>	d_pieces;
	float			d_timer;
	bool			d_dead;

	// vars to set from Maya object attributes
	const char*	d_sound;
	const char*	d_surfaceType;
	float			d_mass;
	float			d_size;
	float			d_lifetimer;
	const char* d_messageDeath;
	const char* d_messageDie;
	float			d_delayDeath;
};

//============================================================================

#endif //_EXPLODER_H
