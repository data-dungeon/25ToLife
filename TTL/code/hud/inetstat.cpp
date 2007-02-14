///////////////////////////////////////////////////////////////////////////////
// Our network status
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "sim/referee.h"
#include "network/connection.h"
#include "math/sfilter.h"
#include "hud/inetstat.h"
#include "main/NetMonitor.h"

#define FORCE_SIZE		20.0f
#define DEFAULT_ALPHA 	0.8f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NetStatInstrument::NetStatInstrument() :
SpriteInstrument("talkie")
{
	t_Handle handle = dblLoad("newhud/netstat.dbl");
	ASSERT(handle != INVALID_HANDLE);

	m_sprite.AddChild(&m_host);
	m_sprite.AddChild(&m_problem);
	m_sprite.SetPriority(45);

	m_host.SetTextureSet(handle, 0);
	m_host.Reset();
	m_host.SetSize(FORCE_SIZE, FORCE_SIZE);
	m_host.SetAlpha(0.0f);

	m_problem.SetTextureSet(handle, 1);
	m_problem.Reset();
	m_problem.SetSize(FORCE_SIZE, FORCE_SIZE);
	m_problem.SetAlpha(0.0f);
	m_problem.SetPosition(-FORCE_SIZE, 0.0f);
	m_problemAlpha = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NetStatInstrument::~NetStatInstrument()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 NetStatInstrument::Extents()
{
	return Vector2(FORCE_SIZE, FORCE_SIZE);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void NetStatInstrument::Update(float dt)
{
	if (!m_hidden && m_active)
	{
		// Are we the host?
		m_host.SetAlpha(g_referee.GetLocalPlayerAgent()->IsSessionHost() ? DEFAULT_ALPHA : 0.0f);

		// DO we have a connection problem?
		float targetAlpha = 0.0f;
		
		// Check our connection, have we been silent for at least a second?
		if (g_CNetMonitor.GetConnectionSilenceTime() >= 1000 )
			targetAlpha = Math::Clamp((Math::Cos(g_timer.GetEndSec() * 8.0f) + 1.0f) * 0.5f) * DEFAULT_ALPHA;

		m_problemAlpha = FOFilterImplicit(m_problemAlpha, targetAlpha, dt, 0.0f);
		m_problem.SetAlpha(m_problemAlpha);
	}
}

//#undef so the SN pch won't take a shit
#undef FORCE_SIZE
#undef DEFAULT_ALPHA

