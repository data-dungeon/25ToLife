/////////////////////////////////////////////////////////////////////////////
// Fog based on position along a spline.
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





// There can be only one!
SplineFog *g_splineFogPtr = new SplineFog;

/////////////////////////////////////////////////////////////////////////////

SplineFog::SplineFog(void)
{
	m_Path = NULL;
	m_Fogs = NULL;
}

/////////////////////////////////////////////////////////////////////////////

SplineFog::~SplineFog(void)
{
	Terminate();
}

/////////////////////////////////////////////////////////////////////////////

bool SplineFog::Initialize(SplinePath* path, const char* world_name)
{
	m_Path = path;
	m_Start = 0.1f;
	m_End = 0.5f;
	
	//ASSERT( m_Path );
	if ( m_Path )
	{
		// Read in the fog table, or use defaults if one doesn't exist.
		s32 size;
		t_Error err;
		char filename[256];
		strcpy( filename, world_name );
		strcat( filename, ".fog" );
		m_Fogs = (Fog*)Media::Load(filename, &size, &err);
		
		if (size == 0 || size != m_Path->NumPoints() * (int)sizeof(Fog))
		{
			TRACE( "*** Fog file (%s) doesn't match path!  Using defaults.\n", filename );

			// Use defaults.
			float start = Display__GetNearFog() / Display__GetFarClip();
			float end = Display__GetFarFog() / Display__GetFarClip();
			size = m_Path->NumPoints() * sizeof(Fog);
			m_Fogs = (Fog*)memAlloc(size);
			ASSERT(m_Fogs);
			for (int i = 0; i < m_Path->NumPoints(); i++)
			{
				m_Fogs[i].start = start;
				m_Fogs[i].end = end;
				m_Fogs[i].red = 1.0f;
				m_Fogs[i].green = 1.0f;
				m_Fogs[i].blue = 1.0f;
				m_Fogs[i].flare = 1.0f;
				m_Fogs[i].intensity = 1.0f;
			}
		}
#ifdef GCN
		else
		{
			for ( int i = 0; i < m_Path->NumPoints(); i++ )
			{
				m_Fogs[i].start = ByteSwap(m_Fogs[i].start);
				m_Fogs[i].end = ByteSwap(m_Fogs[i].end);
				m_Fogs[i].red = ByteSwap(m_Fogs[i].red);
				m_Fogs[i].green = ByteSwap(m_Fogs[i].green);
				m_Fogs[i].blue = ByteSwap(m_Fogs[i].blue);
				m_Fogs[i].flare = ByteSwap(m_Fogs[i].flare);
				m_Fogs[i].intensity = ByteSwap(m_Fogs[i].intensity);
			}
		}
#endif

	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////

void SplineFog::Terminate(void)
{
	if (m_Fogs)
	{
		memFree(m_Fogs);
	}
}

/////////////////////////////////////////////////////////////////////////////

void SplineFog::Update(const Vector3& position)
{
	if (!m_Path)
	{
		//ASSERT(m_Path);
		return;
	}

	int a, b;
	
	float distance = m_Path->NearestDistance(position);
	if (distance < 0)
	{
		distance = 0;
	}
	else if (distance > m_Path->Length())
	{
		distance = m_Path->Length();
	}
	
	m_Path->BetweenEndPoints(distance, a, b);
	
	// Get the two data points & interpolate.
	float start0 = m_Fogs[a].start;
	float start1 = m_Fogs[b].start;
	float end0 = m_Fogs[a].end;
	float end1 = m_Fogs[b].end;
	
	float d0 = m_Path->DistanceAtEndPoint(a);
	float d1 = m_Path->DistanceAtEndPoint(b);
	float t = (distance - d0) / (d1 - d0);

	if ( t < 0.0f )
	{
		t = 0.0f;
	}
	if ( t > 1.0f )
	{
		t = 1.0f;
	}

	float it = 1.0f - t;
	
	m_Start = start0 * it + start1 * t;
	m_End = end0 * it + end1 * t;

	float flare0 = m_Fogs[a].flare;
	float flare1 = m_Fogs[b].flare;
	m_Flare = flare0 * it + flare1 * t;
	
	float r0 = m_Fogs[a].red;
	float r1 = m_Fogs[b].red;
	float g0 = m_Fogs[a].green;
	float g1 = m_Fogs[b].green;
	float b0 = m_Fogs[a].blue;
	float b1 = m_Fogs[b].blue;

	float i0 = m_Fogs[a].intensity;
	float i1 = m_Fogs[b].intensity;

	m_Intensity = i0 * it + i1 * t;

	m_Red = ( r0 * it + r1 * t ) * m_Intensity;
	m_Green = ( g0 * it + g1 * t ) * m_Intensity;
	m_Blue = ( b0 * it + b1 * t ) * m_Intensity;

	ts_bRGBA color;
	color.R = ( unsigned char )( m_Red * 255 );
	color.G = ( unsigned char )( m_Green * 255 );
	color.B = ( unsigned char )( m_Blue * 255 );
	color.A = 255;

	// N8: !!! - kludge to fix fog for microsoft demo.
	if (distance > 1780.0f)
	{
		m_Start = m_End = 0.0f;
	}
	
	CSceneView* sv = &( SceneMgr::GetFirstScene()->d_sceneView );
	
	if ( m_Start == m_End )
		sv->SetFog(false);
	else
	{
		sv->SetFog(true);
		sv->SetNearFog(sv->GetFarClip() * m_Start);
		sv->SetFarFog(sv->GetFarClip() * m_End);
		sv->SetFogColor(color);
		RenderState__Request(RS_FOGMODE,  RS_FOGMODE_LINEAR);
	}
}

/////////////////////////////////////////////////////////////////////////////

float SplineFog::GetStart(void)
{
	return m_Start;
}

/////////////////////////////////////////////////////////////////////////////

float SplineFog::GetEnd(void)
{
	return m_End;
}

/////////////////////////////////////////////////////////////////////////////
