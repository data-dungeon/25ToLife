//////////////////////////////////////////////////////
// Security Cam System  - SecurityCam.cpp
//
// Paul Blagay  3/30/04
// Avalanche Software 2004
//////////////////////////////////////////////////////

#include "camera/CameraPCH.h"

#include "Game/Managers/RenderMgr.h" 
#include "camera/controlcam/controlcam.h"
#include "camera/StationaryCam/StationaryCamDbl.h"
#include "camera/SecurityCamera/SecurityCam.h"
#include "platform/cfocus.h"

//////////////////////////////////////////////////////

SecurityCam::SecurityCam()
{
	m_numCams   = 0;
	m_activeCam = 0;
	m_securityCam = NULL;
	m_fadeOnSwitch = false;

	ResetParams();
}

//////////////////////////////////////////////////////

SecurityCam::~SecurityCam()
{
	DeInit();
}

//////////////////////////////////////////////////////
bool SecurityCam::LoadSecurityCamsFromDBL( t_Handle worldHandle )
{
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_STATIONARYCAM, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (!recordList)
		return false;

	// setup array
	m_numCams = (int)recordList->usRecordCount;

	if (m_numCams == 0)
		return false;

	// create a list of cameras
	m_securityCam = (SecCam *)memAlloc(sizeof(SecCam) * recordList->usRecordCount);
	ASSERT(m_securityCam);
	if (!m_securityCam)
		return false;

	// Clear it!
	memset(m_securityCam, '\0', sizeof(SecCam) * recordList->usRecordCount);

	for (int i = 0; i < m_numCams; i++)
	{
		ts_EnvStationaryCam* source = (ts_EnvStationaryCam *)GetRecordAddress(recordList, i);
		ASSERT(source);

		// setup vars
		m_securityCam[i].pos = source->pos;
		m_securityCam[i].dir = source->dir;
		m_securityCam[i].fov = source->fov;
		m_securityCam[i].x = source->x;
		m_securityCam[i].y = source->y;
//pab		m_securityCam[i].speed = source->speed;
		m_securityCam[i].speed = 5.0f;
//pab		m_securityCam[i].time = source->time;
		m_securityCam[i].time = 2.0f;
		strcpy (m_securityCam[i].name, source->name);
		m_securityCam[i].rotation.Set (0.0f, 0.0f, 0.0f);
		m_securityCam[i].rotationToggle = 1;

		// calculate min and max angles
		m_securityCam[i].minAngleX = Math::Deg2Rad (-(m_securityCam[i].x / 2.0f));
		m_securityCam[i].maxAngleX = Math::Deg2Rad ((m_securityCam[i].x / 2.0f));

		m_securityCam[i].minAngleY = Math::Deg2Rad (-(m_securityCam[i].y / 2.0f));
		m_securityCam[i].maxAngleY = Math::Deg2Rad ((m_securityCam[i].y / 2.0f));

		// add the cameras
		m_camPtr[i] = new StationaryCamera(source->name);
		ASSERT(m_camPtr[i]);
		m_camGroup->AddTail(m_camPtr[i]);
	}

	// set default cam
	m_camGroup->Select(m_camPtr[0]);

	return(true);
}

//////////////////////////////////////////////////////
void SecurityCam::PushGroup ( CameraScene* camScene )
{
	ASSERT( camScene == m_camScene );
	camScene->PushGroup(m_camGroup);
}

//////////////////////////////////////////////////////
void SecurityCam::PopGroup ( CameraScene* camScene )
{
	ASSERT( camScene == m_camScene );
	CameraGroup *pGroup = camScene->PopGroup(m_camGroup);
	ASSERT( pGroup == m_camGroup );
}

//////////////////////////////////////////////////////
void SecurityCam::Init(t_Handle worldHandle, CameraScene* camScene)
{
	m_camScene = camScene;

	// setup camera system
	m_camGroup = new CameraGroup("SecurityCam");
	ASSERT(m_camGroup);
	m_camScene->AddGroup(m_camGroup);

	// load the cams
	bool result = LoadSecurityCamsFromDBL(worldHandle);
}

//////////////////////////////////////////////////////
void SecurityCam::DeInit()
{
	if ( m_securityCam != NULL )
	{
		memFree(m_securityCam);
		m_securityCam = NULL;
		m_numCams = 0;
	}
}

//////////////////////////////////////////////////////
bool SecurityCam::Update(float i_deltasec)
{
	if (m_timeElapsed < m_securityCam[m_activeCam].time)
	{
		// set fov
//		m_camPtr[m_activeCam]->

		//	update rotation of camera
		Matrix3x3 m;
		m.Identity();

		// x
		if (m_securityCam[m_activeCam].x != 0.0f)
		{
			// check direction
			if (m_securityCam[m_activeCam].rotationToggle > 0)
			{
				float delta = Math::AngleDelta( m_securityCam[m_activeCam].rotation.x(), m_securityCam[m_activeCam].maxAngleX );

				if (delta < 0.0f)
					m_securityCam[m_activeCam].rotationToggle = -m_securityCam[m_activeCam].rotationToggle;
			}
			else
			{
				float delta = Math::AngleDelta( m_securityCam[m_activeCam].rotation.x(), m_securityCam[m_activeCam].minAngleX );

				if (delta > 0.0f)
					m_securityCam[m_activeCam].rotationToggle = -m_securityCam[m_activeCam].rotationToggle;
			}
			m_securityCam[m_activeCam].rotation.x( m_securityCam[m_activeCam].rotation.x() + Math::Deg2Rad( (m_securityCam[m_activeCam].speed * m_securityCam[m_activeCam].rotationToggle) * i_deltasec ));
			m.SetXRotation ( m_securityCam[m_activeCam].rotation.x() );
		}

		// y
		if (m_securityCam[m_activeCam].y != 0.0f)
		{
				// check direction
			if (m_securityCam[m_activeCam].rotationToggle > 0)
			{
				float delta = Math::AngleDelta( m_securityCam[m_activeCam].rotation.y(), m_securityCam[m_activeCam].maxAngleY );

				if (delta < 0.0f)
					m_securityCam[m_activeCam].rotationToggle = -m_securityCam[m_activeCam].rotationToggle;
			}
			else
			{
				float delta = Math::AngleDelta( m_securityCam[m_activeCam].rotation.y(), m_securityCam[m_activeCam].minAngleY );

				if (delta > 0.0f)
					m_securityCam[m_activeCam].rotationToggle = -m_securityCam[m_activeCam].rotationToggle;
			}

			m_securityCam[m_activeCam].rotation.y( m_securityCam[m_activeCam].rotation.y() + Math::Deg2Rad((m_securityCam[m_activeCam].speed * m_securityCam[m_activeCam].rotationToggle) * i_deltasec));
			m.SetYRotation ( m_securityCam[m_activeCam].rotation.y() );
		}

		// calculate the target position for the camera
		// position + direction vector
		Vector3 dir = m_securityCam[m_activeCam].dir;
		dir.SafeNormalize();
		dir = dir * m;

		Vector3 Target( m_securityCam[m_activeCam].pos + dir );
		m_camPtr[m_activeCam]->Setup( m_securityCam[m_activeCam].pos, Target );

		// increment timer
		m_timeElapsed += i_deltasec;
		return false;
	}

	// fade out
	if (m_fadeOnSwitch)
		RenderMgr__FadeOut(0);

	return true;
}

//////////////////////////////////////////////////////
void SecurityCam::NextCamera()
{
	m_timeElapsed = 0.0f;

	m_activeCam++;
	if (m_activeCam >= m_numCams)
		m_activeCam = 0;

	// switch camera
	g_cameraDirector.SelectCamera("SecurityCam", m_securityCam[m_activeCam].name);

	if (m_fadeOnSwitch)
		RenderMgr__FadeIn( 0 );
}

//////////////////////////////////////////////////////
void SecurityCam::SetCameraByName( char *Name )
{
	int	i;

	// find the camera
	for (i = 0; i < m_numCams; i++)
	{
		if (!strcmp(m_securityCam[i].name, Name))
			break;
	}

	ResetParams();
	m_activeCam = i;
}

//////////////////////////////////////////////////////
void SecurityCam::SetCameraByIndex( int Index )
{
	ASSERTS (Index < m_numCams && Index >= 0, "Index out of range");

	ResetParams();
	m_activeCam = Index;
}

//////////////////////////////////////////////////////
char* SecurityCam::GetCameraNameByIndex( int Index )
{
	ASSERTS (Index < m_numCams && Index >= 0, "Index out of range");

	return (m_securityCam[Index].name);
}

//////////////////////////////////////////////////////
void SecurityCam::ResetParams()
{
	m_timeElapsed = 0.0f;
}

