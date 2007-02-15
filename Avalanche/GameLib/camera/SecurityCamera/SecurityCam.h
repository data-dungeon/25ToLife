//////////////////////////////////////////////////////
// Security Cam System  - SecurityCam.h
//
// Paul Blagay  3/30/04
// Avalanche Software 2004
//////////////////////////////////////////////////////
#ifndef _SECURITYCAM_H_
#define _SECURITYCAM_H_

#include "camera/StationaryCam/StationaryCam.h"
#include "camera/controlcam/controlcam.h"

#define MAX_SECURITY_CAMS		20

class SecurityCam
{
public:
	SecurityCam();
	~SecurityCam();

	// funcs
	void Init(t_Handle worldHandle , CameraScene* camScene);
	void DeInit();
	bool LoadSecurityCamsFromDBL( t_Handle worldHandle );

	bool Update(float i_deltasec);
	void NextCamera();
	void SetCameraByName( char *Name );
	void SetCameraByIndex( int Index );
	void SetFade(bool Fade) {	m_fadeOnSwitch = Fade; };
	char* GetCameraNameByIndex( int Index );
	int	GetNumCams() { return m_numCams; };
	void PushGroup( CameraScene* camScene );
	void PopGroup( CameraScene* camScene );
	void ResetParams();

	struct SecCam
	{
		Vector3	pos;							
		Vector3	dir;							
		float		fov;							
		float		x;								
		float		y;								
		float		speed;						
		float		time;		
		int		rotationToggle;
		Vector3	rotation;
		float		minAngleX;
		float		maxAngleX;
		float		minAngleY;
		float		maxAngleY;
		char		name[12];
	};

	SecCam		*m_securityCam;

private:
	int				m_numCams;
	int				m_activeCam;
	float				m_timeElapsed;
	bool				m_fadeOnSwitch;

	// camera stuff
	CameraScene*				m_camScene;
	StationaryCamera*			m_camPtr[MAX_SECURITY_CAMS];
	CameraGroup*				m_camGroup;
};

#endif // _SECURITYCAM_H_
