///////////////////////////////////////////////////////////////////////////////
// 3D player names instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "interfacesTTL/healthIF.h"
#include "components/ai/aipsyche.h"
#include "components/ai/aiperception.h"
#include "components/ai/aiweaponcontrol.h"
#include "components/ai/ainavigation.h"
#include "hud\iplayernames.h"

#define NAME_SCALE	1.0f
#define NAME_YOFFSET	2.2f
#define NAME_ALPHA	0.5f
#define ENEMY_SHOW_D	2.0f
#define MIN_CAMERA_D 2.0f
#define AI_NAME_SCALE 0.75f

bool g_aiSpy = false;

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
PlayerNamesInstrument::PlayerNamesInstrument() :
SpriteInstrument("PlayerNames")
{
	for (int n = 0; n < MAX_PLAYER_NAMES; n++)
	{
		m_sprite.AddChild(&m_nameTag[n]);
		m_nameTag[n].SetPriority(0);
		m_nameTag[n].ShadowOff();
	}
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void PlayerNamesInstrument::Update(float dt)
{
	int usedNames = 0;

	if (!m_hidden && m_active && g_hud.GetSubject())
	{
		uint8 me = g_players.GetParticipantId(0);
		CActor *myActor = g_referee.GetActor(me);
		Vector3 myPos;
		if (myActor)
			myPos = myActor->GetBodyInWorld();
		else
			myPos = g_cameraDirector.CurrentCamera().Eye();

		uint8 player = INVALID_PARTICIPANT;
		const CTeamingModel &teamingModel = g_referee.GetTeamingModel();
		const Vector3 camera = g_cameraDirector.CurrentCamera().Eye();
		const float fov = g_cameraDirector.CurrentCamera().FOV();
		const float scale = NAME_SCALE * (fov / 65.0f);
		while ((player = g_referee.GetNextPlayer(player)) != INVALID_PARTICIPANT)
		{
			if (player == me || !g_referee.IsAlive(player))
				continue;

			CActor *playerActor = g_referee.GetActor(player);
			CPlayerAgent *playerAgent = g_referee.GetPlayerAgent(player);
			if (playerActor && playerAgent)
			{
				const char *avatarName = playerAgent->GetDisplayName();
				if (avatarName)
				{
					Vector3 pos = playerActor->GetBodyInWorld();
					float dSquaredToCamera = (camera - pos).LengthSquared(); 
					if (dSquaredToCamera > Math::Square(MIN_CAMERA_D) &&
							(teamingModel.AreFriends(player, me) || 
							(pos - myPos).LengthSquared() < Math::Square(ENEMY_SHOW_D)))
					{
						m_nameTag[usedNames].SetText(avatarName);
						if (teamingModel.IsPlayerBlue(player))
							m_nameTag[usedNames].SetColor(0.0f, 0.0f, 0.75f, NAME_ALPHA);
						else
							m_nameTag[usedNames].SetColor(0.75f, 0.0f, 0.0f, NAME_ALPHA);
						pos.Y(pos.Y() + NAME_YOFFSET);
						m_nameTag[usedNames].SetWorldPosition(pos);
						m_nameTag[usedNames].SetWorldScale(scale);
						m_nameTag[usedNames].ShowAll();
						++usedNames;
						ASSERT(usedNames <= MAX_PLAYER_NAMES);
					}
				}
			}
		}
	}

	// Make sure these are all hidden
	for (int n = usedNames; n < MAX_PLAYER_NAMES; n++)
		m_nameTag[n].HideAll();

	// Call the parent
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
AISpyInstrument::AISpyInstrument() :
SpriteInstrument("AISpy")
{
	for (int n = 0; n < MAX_AI_NAMES; n++)
	{
		m_sprite.AddChild(&m_nameTag[n]);
		m_nameTag[n].SetPriority(0);
		m_nameTag[n].ShadowOff();
	}
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void AISpyInstrument::Update(float dt)
{
	int usedNames = 0;

	if (!m_hidden && m_active && g_aiSpy)
	{
		uint8 me = g_players.GetParticipantId(0);
		uint8 ai = INVALID_PARTICIPANT;
		ASSERT(ValidSingleton(Referee));
 		const CTeamingModel &teamingModel = g_referee.GetTeamingModel();
		const Vector3 camera = g_cameraDirector.CurrentCamera().Eye();
		const float fov = g_cameraDirector.CurrentCamera().FOV();
		const float scale = AI_NAME_SCALE * (fov / 65.0f);
		while ((ai = g_referee.GetNextParticipant(ai)) != INVALID_PARTICIPANT)
		{
			if (ai == me)
				continue;

			CCompActor *aiActor = (CCompActor *)g_referee.GetActor(ai);
			if (aiActor)
			{
				Vector3 pos = aiActor->GetBodyInWorld();
				float dSquaredToCamera = (camera - pos).LengthSquared(); 
				if (dSquaredToCamera > Math::Square(MIN_CAMERA_D))
				{
					char name[64];
					MakeAIName(aiActor, name);
					if (*name)
					{
						m_nameTag[usedNames].SetText(name);
	
						if (teamingModel.AreFriends(ai, me))
							m_nameTag[usedNames].SetColor(0.0f, 0.75f, 0.0f, NAME_ALPHA);
						if (teamingModel.AreNeutral(ai, me))
							m_nameTag[usedNames].SetColor(0.0f, 0.0f, 0.75f, NAME_ALPHA);
						else
							m_nameTag[usedNames].SetColor(0.75f, 0.0f, 0.0f, NAME_ALPHA);

						Vector3 pos = aiActor->GetBodyInWorld();
						pos.Y(pos.Y() + NAME_YOFFSET);
						m_nameTag[usedNames].SetWorldPosition(pos);
						m_nameTag[usedNames].SetWorldScale(scale);
						m_nameTag[usedNames].ShowAll();
						if (++usedNames == MAX_AI_NAMES)
							break;
					}
				}
			}
		}
	}

	// Make sure these are all hidden
	for (int n = usedNames; n < MAX_AI_NAMES; n++)
		m_nameTag[n].HideAll();

	// Call the parent
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AISpyInstrument::MakeAIName(
CCompActor *aiActor,
char *name)
{
	ASSERT(aiActor);

	char psyche[64];
	GetPsyche(*aiActor, psyche);
	float hp = GetHP(*aiActor);
	if (HasContact(*aiActor))
		sprintf(name, "! %s %dhp", psyche, (int)hp);
	else
		sprintf(name, "%s %dhp", psyche, (int)hp);
}

///////////////////////////////////////////////////////////////////////////////
// Get at an actors health! returns false
// if this actor does not have health
///////////////////////////////////////////////////////////////////////////////
float AISpyInstrument::GetHP(
CCompActor &who)
{
	// Get the health interface
	HealthIF *healthIF = (HealthIF *)who.GetInterface(HealthIF::GetID());
	if (!healthIF)
		return 0.0f;

	return healthIF->GetHealth();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AISpyInstrument::HasContact(
CCompActor &who)
{
	AiPerception *perception = (AiPerception*)who.GetComponentByName("AiPerception");
	if (!perception)
		return false;
	return perception->GetRecentSighting(5.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AISpyInstrument::GetPsyche(
CCompActor &who,
char *psyche)
{
	AiPsyche *ai = (AiPsyche *)who.GetComponentByName("AiPsyche");
	if (!ai)
		strcpy(psyche, "NOAI");
	else
	{
		switch (ai->GetAiType())
		{
			case AiPsyche::AI_UNDEFINED:
				strcpy(psyche, "UNDEF");
				break;
			case AiPsyche::AI_WATCHER:
				strcpy(psyche, "WATCHER");
				break;
			case AiPsyche::AI_WALKER:
				strcpy(psyche, "WALKER");
				break;
			case AiPsyche::AI_RUNNER:
				strcpy(psyche, "RUNNER");
				break;
			case AiPsyche::AI_HANDSUP:
				strcpy(psyche, "HANDSUP");
				break;
			case AiPsyche::AI_HELDUP:
				strcpy(psyche, "HELDUP");
				break;
			case AiPsyche::AI_COWERER:
				strcpy(psyche, "COWERER");
				break;
			case AiPsyche::AI_DANCER:
				strcpy(psyche, "DANCER");
				break;
			case AiPsyche::AI_DJ:
				strcpy(psyche, "DJ");
				break;
			case AiPsyche::AI_GRUNT:
				strcpy(psyche, "GRUNT");
				break;
			case AiPsyche::AI_CHARGER:
				strcpy(psyche, "CHARGE");
				break;
			case AiPsyche::AI_DEFENDER:
				strcpy(psyche, "DEFEND");
				break;
			case AiPsyche::AI_SNIPER:
				strcpy(psyche, "SNIPER");
				break;
			case AiPsyche::AI_STATIONARY:
				strcpy(psyche, "STATION");
				break;
			case AiPsyche::AI_SUICIDAL:
				strcpy(psyche, "SUICIDE");
				break;
			case AiPsyche::AI_BUDDY:
				strcpy(psyche, "BUDDY");
				break;
			case AiPsyche::AI_CHICKEN:
				strcpy(psyche, "CHICK");
				break;
			case AiPsyche::AI_EVACUATOR:
				strcpy(psyche, "EVAC");
				break;
			case AiPsyche::AI_BAMBI:
				strcpy(psyche, "BAMBI");
				break;
			default:
				strcpy(psyche, "UNK");
				break;
		}

		AiNavigation *nav = (AiNavigation *)who.GetComponentByName("AiNavigation");
		if (nav)
		{
			switch (nav->GetMode())
			{
				case AiNavigation::NAVMODE_DISABLED:
					strcat(psyche, " |DISABLED|");
					break;
				case AiNavigation::NAVMODE_STATIONARY:
					strcat(psyche, " |STATIONARY|");
					break;
				case AiNavigation::NAVMODE_MOVE_TO_TARGET:
					strcat(psyche, " |MOVE_TO_TARGET|");
					break;
				case AiNavigation::NAVMODE_FLANK_TO_TARGET:
					strcat(psyche, " |FLANK_TO_TARGET|");
					break;
				case AiNavigation::NAVMODE_RETREAT:
					strcat(psyche, " |RETREAT|");
					break;
				case AiNavigation::NAVMODE_DODGY:
					strcat(psyche, " |DODGY|");
					break;
				case AiNavigation::NAVMODE_RETURN_TO_START:
					strcat(psyche, " |RETURN|");
					break;
				case AiNavigation::NAVMODE_GO_TO_BEACON:
					strcat(psyche, " |GO2BEACON|");
					break;
				case AiNavigation::NAVMODE_GO_TO_BEACON_LOCK:
					strcat(psyche, " |GO2BEACON_LOCK|");
					break;
				case AiNavigation::NAVMODE_WANDER:
					strcat(psyche, " |WANDER|");
					break;
				case AiNavigation::NAVMODE_RUN_HOME:
					strcat(psyche, " |HOME|");
					break;
				case AiNavigation::NAVMODE_ATTACK_FROM_COVER:
					strcat(psyche, " |ATTACKCOVER|");
					break;
				case AiNavigation::NAVMODE_MELEE:
					strcat(psyche, " |MELEE|");
					break;
				case AiNavigation::NAVMODE_FOLLOW_AVATAR:
					strcat(psyche, " |FOLLOW|");
					break;
				case AiNavigation::NAVMODE_RUN_FROM_NADE:
					strcat(psyche, " |RUN|");
					break;
				case AiNavigation::NAVMODE_PUSHED:
					strcat(psyche, " |PUSHED|");
					break;
				case AiNavigation::NAVMODE_SIDESTEP:
					strcat(psyche, " |SIDESTEP|");
					break;
			}
		}

		// What are they doing right now?
		AiWeaponControl *weapon = (AiWeaponControl *)who.GetComponentByName("AiWeaponControl");
		if (weapon)
		{


		}
		else // npc
		{
			// NPC
			switch (ai->GetHighestWant())
			{
				case AiPsyche::WANT_DO_JOB:
					strcat(psyche, " (job)");
					break;
				case AiPsyche::WANT_TO_WATCH:
					strcat(psyche, " (watch)");
					break;
				case AiPsyche::WANT_TO_FLEE:
					strcat(psyche, " (flee)");
					break;
				case AiPsyche::WANT_TO_COWER:
					strcat(psyche, " (cower)");
					break;
				case AiPsyche::WANT_TO_FIGHT:
					strcat(psyche, " (fight)");
					break;
			}
		}
	}
}


