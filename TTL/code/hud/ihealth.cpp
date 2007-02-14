///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "hud/ihealth.h"
#include "interfacesTTL/healthIF.h"

#define ACTUAL_WIDTH		55.0f
#define ACTUAL_HEIGHT	96.0f

#define BASE_ALPHA		0.15f
#define BAR_ALPHA			0.2f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HealthInstrument::HealthInstrument() :
SpriteInstrument("health")
{
	m_currentPlayerType = NONE;

	// Setup the sprites (no textures yet)
	for (unsigned int l = 0; l < BAR_LAYERS; l++)
		m_sprite.AddChild(&m_layer.GetSprite(l));

	m_pulse = 0.0f;
	m_pulseRate = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 HealthInstrument::Extents()
{
	return Vector2(ACTUAL_WIDTH, ACTUAL_HEIGHT);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void HealthInstrument::Update(float dt)
{
	// Always update this
	Math::MoveToZero(&m_pulse, m_pulseRate * dt);

	// are we hidden?
	if (m_hidden || !m_active)
	{
		// Lets do nothing!
		SpriteInstrument::Update(dt);
		return;
	}

	// Get the basics
	CCompActor *actor = g_hud.GetSubject();
	float health;
	bool alive;
	bool valid = GetHealth(actor, health, alive);

	// Are we valid?
	if (!valid || !UpdateType(actor))
		m_sprite.HideAll();
	else
	{
		// Update the indicator!
		UpdateIndicator(health, alive);
		m_sprite.ShowAll();
	}

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
// Get at an actors health! returns false
// if this actor does not have health
///////////////////////////////////////////////////////////////////////////////
bool HealthInstrument::GetHealth(
CCompActor *who,
float &health,
bool &alive)
{
	// Who's on first
	if (!who)
		return false;

	// Get the health interface
	HealthIF *healthIF = (HealthIF *)who->GetInterface(HealthIF::GetID());
	if (!healthIF)
		return false;

	health = healthIF->GetHealthPercentage();
	alive = healthIF->IsAlive();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Our player type
///////////////////////////////////////////////////////////////////////////////
HealthInstrument::PlayerType HealthInstrument::GetPlayerType(
CCompActor *who)
{
#ifdef THISSHOULDHAVEWORKED
	// Who is this?
	if (who)
	{
		// Get the player id
		CParticipantIF *participant= (CParticipantIF*)who->GetInterface(CParticipantIF::GetID());
		if (participant)
		{
			uint8 id = participant->GetParticipantId();

			// Figure it out!
			CPlayerProfile::EVariant variant = g_referee.GetTeamingModel().GetPlayerProfileVariant(id);
			if (variant == CPlayerProfile::COP)
				return COP;
			else if (variant == CPlayerProfile::GANG)
				return GANGSTER;
		}
	}
#else
	if (who)
	{
		char *role = who->Parm().GetString( "role" );
		if (role)
		{
			if (strcmp(role, "gang") == 0)
				return GANGSTER;
			else if(strcmp(role, "cop") == 0)
				return COP;
		}
	}
#endif

	return NONE;
}

///////////////////////////////////////////////////////////////////////////////
// Update type
///////////////////////////////////////////////////////////////////////////////
bool HealthInstrument::UpdateType(
CCompActor *who)
{
	// Have we changed?
	PlayerType type = GetPlayerType(who);
	if (type != m_currentPlayerType)
	{
		m_currentPlayerType = type;
		m_layer.SetTextureDB(TextureDBName(m_currentPlayerType));
	}

	// Allows this to process
	if (m_layer.Process() == SSS_INITIALIZE)
		SetupSprites();

	return (m_currentPlayerType != NONE);
}

///////////////////////////////////////////////////////////////////////////////
// Update the indicator!
///////////////////////////////////////////////////////////////////////////////
void HealthInstrument::UpdateIndicator(
float health,
bool alive)
{
	// Figure out the color
	const Vector3 red(1.0f, 0.0f, 0.0f);
	const Vector3 green(0.0f, 1.0f, 0.0f);
	const Vector3 yellow(1.0f, 1.0f, 0.0f);

	Vector3 color;
	health = Math::Clamp(health);
	if (health <= 0.5f)
		color = red + (yellow - red) * (health * 2.0f);
	else
		color = yellow + (green - yellow) * ((health - 0.5f) * 2.0f);
	m_layer.GetSprite(BAR_FILL).SetColor(color.X(), color.Y(), color.Z(), BAR_ALPHA);

	// Setup the clip
	float vStart = (128.0f - ACTUAL_HEIGHT) / 128.0f * 0.5f;
	float vEnd = 1.0f - vStart;
	float v = vStart + (vEnd - vStart) * (1.0f - health);
	m_layer.GetSprite(BAR_FILL).CropRect(0.0f, v, 1.0f, vEnd);

	for (unsigned int l = 0; l < BAR_LAYERS; l++)
		m_layer.GetSprite(l).SetScale(1.0f + m_pulse, 1.0f + m_pulse);
}

///////////////////////////////////////////////////////////////////////////////
// Load the right sprite up
///////////////////////////////////////////////////////////////////////////////
void HealthInstrument::SetupSprites()
{
	// Setup the layers
	for (unsigned int l = 0; l < BAR_LAYERS; l++)
	{
		float width, height;
		Sprite &sprite = m_layer.GetSprite(l);
		sprite.GetSize(width, height);
		sprite.SetSize(height, width);
		sprite.SetOrigin(0.5f, 0.5f);
		sprite.SetPosition(ACTUAL_WIDTH * 0.5f, ACTUAL_HEIGHT * 0.5f);

		sprite.SetTextureCoordinates(0, 1.0, 0.0f);
		sprite.SetTextureCoordinates(1, 1.0, 1.0f);
		sprite.SetTextureCoordinates(2, 0.0, 1.0f);
		sprite.SetTextureCoordinates(3, 0.0, 0.0f);

		sprite.SetPriority(50 + l);
	}

	m_layer.GetSprite(BAR_BASECOLOR).SetAlpha(BASE_ALPHA);
}

///////////////////////////////////////////////////////////////////////////////
// Get the right dbl name based on player type
///////////////////////////////////////////////////////////////////////////////
const char *HealthInstrument::TextureDBName(
PlayerType playerType)
{
	switch (playerType)
	{
		case COP:
			return "newhud/healthc.dbl";
			break;
		case GANGSTER:
			return "newhud/healthg.dbl";
			break;
		default:
		case NONE:
			return NULL;
			break;
	}
}

// we're done with these; #undef so the SN pch won't take a shit
#undef BASE_ALPHA
#undef BAR_ALPHA

