/**********************************************/
/*														     */
/* PathFollowerCam.h							        */
/* 25 To Life										     */
/* visual c++ 6.0                             */
/*                                            */
/**********************************************/

#ifndef _PATHFOLLOWERCAM_H
#define _PATHFOLLOWERCAM_H

#include "Components/Component.h"
#include "Math/Vector.h"
#include "Math/Quatern.h"
#include "linearAlgebra/cquat.h"
#include "path/PathCollection.h"
#include "hud/text.h"


/* forward declarations */

class CCompActor;
class	PathCamera;
class CameraScene;
class CameraGroup;

class CPathFollowerCam : public CActorComponent
{
public:
	CPathFollowerCam( CCompActor& i_actor );
	virtual		~CPathFollowerCam();

	const char *ComponentName() { return "PathFollowerCam"; }
	const char *ComponentClass() { return "KinematicDriver"; }

	void			Setup();

	SplinePath	*GetPath() { return d_path; }
	bool			SetPath( SplinePath *i_path );

	void			SetTarget( CActor *actor ) { d_target = actor; };
	CActor		*GetTarget() { return d_target; }

	void			SetTargetPoint( Vector3 *v ) { d_targetPoint = *v; }
	Vector3		*GetTargetPoint() { return &d_targetPoint; }

	void			SetSpeed( float speed ) { d_speed = speed; }
	float			GetSpeed() { return d_speed; }

	void			SetEndDelay( float t ) { d_endDelay = t; }
	float			GetEndDelay() { return d_endDelay; }

	void			SetTargetSpeed( float speed ) { d_targetSpeed = speed; }
	float			GetTargetSpeed() { return d_targetSpeed; }

	void			SetAccel( float accel ) { d_accel = accel; }
	float			GetAccel() { return d_accel; }

	void			SetDecel( float decel ) { d_decel = decel; }
	float			GetDecel() { return d_decel; }

	void			SetTimeActive( float time ) { d_timeActive = time; }
	float			GetTimeActive() { return d_timeActive; }

	void			SetTimeRemaining( float time ) { d_timeRemaining = time; }
	float			GetTimeRemaining() { return d_timeRemaining; }

	void			SetTargetingBehavior( int targetingBehavior ) { d_targetingBehavior = targetingBehavior; }
	int			GetTargetingBehavior() { return d_targetingBehavior; } 

	void			SetCameraType( int cameraType ) { d_cameraType = cameraType; }
	int			GetCameraType() { return d_cameraType; } 

	void			SetFadeIn( bool f ) { m_fadeIn = f; }
	bool			GetFadeIn() { return m_fadeIn; } 

	void			SetFadeOut( bool f ) { m_fadeOut = f; }
	bool			GetFadeOut() { return m_fadeOut; } 

	void			SetAddCinemaBars( bool f ) { m_addCinemaBars = f; }
	bool			GetAddCinemaBars() { return m_addCinemaBars; } 

	void			SetTextMessage( char *s ) { textMessage = s; }
	char			*GetTextMessage( void ) { return textMessage; }

// move along path
	void			Go( float i_deltaSec );

	// stop at current position
	void			Stop( float i_deltaSec );

	void			RemoveBlackCinemaBars();
	void			AddBlackCinemaBars();

	// hooks to update the actor
	virtual void			Activate( void );
	virtual void			Deactivate( void );
	virtual void			BeginUpdate( void );
	virtual void			AttemptUpdate( float i_deltaSec );
	virtual bool			DefaultActive( void ) { return false; }

#ifndef CONSUMER_BUILD
	// debug
	inline void	Draw3D()	{ /*d_pPathCamera->GetPath()->Draw();*/ }
#endif //CONSUMER_BUILD

protected:

	// count down to restarting path
	float						d_timeToReset;
	float						d_speed;
	float						d_targetSpeed;
	float						d_accel;
	float						d_decel;
	float						d_endDelay;
	float						d_timeActive;
	float						d_timeRemaining;
	int						d_targetingBehavior;
	int						d_cameraType;
	CActor					*d_target;
	Vector3					d_targetPoint;

	Camera					*d_pCamera;
	char						*d_oldCamName;

	Vector3					d_vPosition;				// current position				meters

	SplinePath				*d_path;

	CameraGroup				*d_group;

	bool						m_suspend;
	bool						d_pathCameraActivated;

	bool						m_fadeIn;
	bool						m_fadeOut;

	bool						m_addCinemaBars;

	t_Handle					d_TopSpriteHandle;
	t_Handle					d_BottomSpriteHandle;

	Text						d_SubTitleText;
	char						*textMessage;

	float						volume;

private:

	REF_SEND_MESSAGE_HANDLER( SuspendMessage , CPathFollowerCam) d_suspendMessageHandler;
	REF_SEND_MESSAGE_HANDLER( ResumeMessage , CPathFollowerCam) d_resumeMessageHandler;
	REF_SEND_MESSAGE_HANDLER( StartCinematic , CPathFollowerCam) d_startCinematicMessageHandler;
	REF_SEND_MESSAGE_HANDLER( EndCinematic , CPathFollowerCam) d_endCinematicMessageHandler;

public:

	/* overload = operator to make copying easy */

	CPathFollowerCam &operator=( CPathFollowerCam & );
};

#endif
