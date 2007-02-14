#include "TTLPCH.h"

#include "Components/ComponentCreator.h"
#include "components/Effects.h"
#include "Audio/audiosys.h"
#include "Audio/cstreami.h"
#include "avatar/taunt.h"

STANDARD_COMPONENT_CREATOR(Taunt, Taunt, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
Taunt::Taunt(
CCompActor &owner) :
CActorComponent(owner),
m_taunt(0, 8)
{
	// Nothing played yet
	m_lastPlayed = -1;
	m_lastPlayedTime = 0.0f;
	m_handle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Taunt::~Taunt()
{
	// Wee!
	Stop();
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void Taunt::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	// Loop through our param set
	for (int i = 0; i < 12; i++)
	{
		const char *taunt = GetTaunt(i);
		if (taunt)
			m_taunt.Add(i);
	}

	// Our message handler
	m_sayHandler.Initialize("Taunt", MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS, actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
// The perframe update
/////////////////////////////////////////////////////////////////////////////
void Taunt::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);

	// Do we have something playing?
	CommonStreamInterface *stream = CSIHandle::GetStream(m_handle);
	if (!stream)
		return;

	// Did we die?
	Property *health = actor.GetProperty("health");
	if (health && health->GetFloat() <= 0.0f)
		Stop();
	else
	{
		// Update our position
		SoundStream3D *stream3D = stream->Stream3D();
		ASSERT(stream3D);
		Vector3 pos = actor.GetFocusInWorld();
		stream3D->SetPosition(pos);
		Vector3 vel = actor.GetVelocityInWorld();
		stream3D->SetVelocity(vel);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Is a taunt playing?
/////////////////////////////////////////////////////////////////////////////
bool Taunt::Playing()
{
	// Nothing to do here
	CommonStreamInterface *stream = CSIHandle::GetStream(m_handle);

	// Just ask it
	return (stream && stream->IsPlaying());
}

/////////////////////////////////////////////////////////////////////////////
// Stop taunts from playing (returns true if a taunt was playing)
/////////////////////////////////////////////////////////////////////////////
bool Taunt::Stop()
{
	// Nothing to do here
	CommonStreamInterface *stream = CSIHandle::GetStream(m_handle);
	if (!stream)
		return false;

	// Release it
	bool wasPlaying = stream->IsPlaying();
	CSIHandle::Kill(m_handle);
	return wasPlaying;
}

/////////////////////////////////////////////////////////////////////////////
// How many taunts are there?
/////////////////////////////////////////////////////////////////////////////
int Taunt::Count()
{
	return m_taunt.Count();
}

/////////////////////////////////////////////////////////////////////////////
// Play a specific taunt (returns false if already playing)
/////////////////////////////////////////////////////////////////////////////
bool Taunt::Say(
int index,
float volume)
{
	if (index < 0 || index >= Count() || Playing())
		return false;

	// Do it!
	bool ret = Play(GetTaunt(m_taunt[index]), volume);
	if (ret)
	{
		g_messageDispatcher.SendMessageToActor("Taunt", CMessageDispatcher::IntToData(index), actor.GetHandle(), actor.GetHandle());
		g_messageDispatcher.SendMessageToAll("Taunted", NULL, actor.GetHandle());
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Play a random taunt (returns false if already playing)
/////////////////////////////////////////////////////////////////////////////
bool Taunt::SayRandom(
float volume)
{
	if (Playing() || Count() == 0)
		return false;

   int playMe;
	if (Count() == 1)
		playMe = 0;
	else
	{
		do	{
			playMe = g_random.UpToExcludeHigh(Count());
		} while (m_lastPlayed == playMe);
	}

	// Do it!
	if (!Play(GetTaunt(m_taunt[playMe]), volume))
		return false;

	g_messageDispatcher.SendMessageToActor("Taunt", CMessageDispatcher::IntToData(playMe), actor.GetHandle(), actor.GetHandle());
	g_messageDispatcher.SendMessageToAll("Taunted", NULL, actor.GetHandle());
	m_lastPlayed = playMe;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get the last stream played
/////////////////////////////////////////////////////////////////////////////
const char *Taunt::GetLastPlayedName()
{
	if (m_lastPlayed < 0 || m_lastPlayed >= Count())
		return NULL;

	return GetTaunt(m_taunt[m_lastPlayed]);
}

/////////////////////////////////////////////////////////////////////////////
// Convert an index into a taunt
/////////////////////////////////////////////////////////////////////////////
const char *Taunt::GetTaunt(
int index)
{
	char name[32];
	sprintf(name, "taunt%d", index);

	ParameterSet &param = actor.Parm();
	return param.GetString(name);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Taunt::Play(
const char *taunt,
float volume)
{
	// Uh, duh
	if (!taunt || taunt[0] == '\0')
		return false;

	// Is there a taunt playing?
	CommonStreamInterface *stream = CSIHandle::GetStream(m_handle);
	if (stream)
		return false;

	// stop the stream, if it is there
	delete m_handle;

	// Start the new one
	stream = g_audio.CreateStream3D(taunt, false, true);
	if (!stream)
		return false;

	// We made it!
 	m_lastPlayedTime = g_timer.GetEndSec();

	SoundStream3D *stream3D = stream->Stream3D();
	ASSERT(stream3D);
	Vector3 pos = actor.GetFocusInWorld();
	stream3D->SetPosition(pos);
	Vector3 vel = actor.GetVelocityInWorld();
	stream3D->SetVelocity(vel);
	stream3D->SetVolume(volume);
	stream3D->Play();

	// Get a handle!
	m_handle = stream->CreateHandle();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Taunt::Say::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	int index = CMessageDispatcher::DataToInt(i_data);
	d_ref->Say(index);
}

