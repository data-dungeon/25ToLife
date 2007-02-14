#ifndef __WALLMOVES_H_INCLUDED__
#define __WALLMOVES_H_INCLUDED__

#include "Components/Component.h"
#include "Components/InteractionMonitor.h"

class WallVault : public CActorComponent
{
public:
	WallVault( CCompActor &i_actor );
	~WallVault();

	const char *ComponentName() { return "WallVault"; }
	const char *ComponentClass() { return "KinematicDriver"; }

	virtual bool DefaultActive( void ) { return false; }

	void Initialize( void );
	void Activate( void );
	void Deactivate( void );
	void BeginFrame( void );

	void SetVault( OBB *vault ); 
	void SetVault( CActor *vaultActor ); 
	Vector3 GetPointInBox( Vector3CRef center, Vector3CRef x, Vector3CRef y, Vector3CRef z, Vector3CRef target );
	void LockToVault( void );

	void AbortVault( void );

	bool FarSideFall( void ) { return farFall; }

private:
	enum
	{
		MOVING_TO_VAULT = 0,
		LOCK_TO_VAULT,
		VAULTING
	};
	enum
	{
		VAULT = 0,
		CLIMBUP,
		CLIMBOVER,
	};

	OBB *currentVault;
	int animIndex;
	int vaultType;
	float baseHeight;
	int state;
	float speed;
	float nearHeight; // height offset from the ground to the top of the wall on the near side
	float farHeight; // height offset from the ground to the top of the wall on the far side
	bool farFall;

	float facingTravel;
	float upTravel;

	CActorComponent *reactivateDriver;
	CActorComponent *reactivateController;

	REF_SEND_MESSAGE_HANDLER(ManDown, WallVault) d_manDownHandler;
};

#endif //__WALLMOVES_H_INCLUDED__
