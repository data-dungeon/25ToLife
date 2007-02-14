///////////////////////////////////////////////////////////////////////////////
// Radar!
///////////////////////////////////////////////////////////////////////////////
#ifndef IRADAR_H
#define IRADAR_H

#include "hud\ipanel\isprite.h"
#include "stage\cactor.h"
#include "container\dllist.h"
#include "container\pool.h"

#define MAX_FIRED_INDICATORS	3
#define MAX_BLIPS_PER_TYPE		8

#define SHOW_BADDIES

///////////////////////////////////////
///////////////////////////////////////
class RadarInstrument : public SpriteInstrument
{
public:
	// Constructor
	RadarInstrument();
	virtual ~RadarInstrument();

	// Get our size
	Vector2 Extents();

	// The parent calls this to set your position
	void Place(float x, float y);

	// The update (called once per frame)
	virtual void Update(float dt);

	// Register a hit
	void NotifyFired(CActor *shooter, const Vector3 &pos);

protected:
	// Our texture DB
	t_Handle				m_textureDB;

	// The background
	Sprite				m_background[4];
	Sprite				m_overlay;

	// Range in meters the radar can see
	float					m_maxRange;

	// Overall radius of radar in pixels
	float					m_radius;

	// Fired indicators
	Sprite				m_fired[MAX_FIRED_INDICATORS];
	Vector3				m_firedPos[MAX_FIRED_INDICATORS];
	float					m_firedAlpha[MAX_FIRED_INDICATORS];
	int					m_oldestFired;
	void UpdateFired(const Vector3 &viewPos, const Vector3 &viewDir);

	// Get the info about the viewer
	void GetViewerInfo(Vector3 &pos, Vector3 &dir);

	// Transform a point into radar space
	static float GetAngle(const Vector3 &viewer, const Vector3 &viewDir, const Vector3 &pos);

	// return a vector in radar space that corresponds to the pos
	Vector2 ToRadar(const Vector3 &viewer, const Vector3 &viewDir, const Vector3 &pos, float *range = NULL);

	// The referee based blips
	class BlipArray
	{
	public:
		 BlipArray();
		~BlipArray();

		bool Create(Sprite &parent, int maxBlips, const Vector4 &color, t_Handle textureDB, int texture);

		bool Allocate(const Vector2 &pos, float scale = 1.0f);
		void Clear();

		void Update();

		void HideAll()
			{ Clear(); Update(); }

	private:
		Sprite			*m_blip;
		int				m_usedBlips, m_maxBlips;
	};

	class PersistentBlip
	{
	public:
		PersistentBlip(Sprite &parent, const Vector3 &pos, t_Handle textureDB, int texture, float maxRange);
		PersistentBlip(Sprite &parent, ActorHandle follow, t_Handle textureDB, int texture, float maxRange, bool isPickup = false, bool useTeamColor = false);
		~PersistentBlip() {}

		// Is this item valid?
		bool IsValid();

		// Get the position in world
		Vector3 PositionInWorld();

		float MaxRange()
			{ return m_maxRange; }

		// Set the screen position
		void SetPosition(const Vector2 &pos)
			{ m_sprite.SetPosition(pos.X(), pos.Y()); }

		void SetColor(float red, float green, float blue, float alpha = 1.0f)
			{ m_sprite.SetColor(red, green, blue, alpha); }

		bool ObjectHidden();

		void Show(bool show)
			{ if (show) m_sprite.Show(); else m_sprite.Hide(); }

		bool UseTeamColor()
			{ return(m_useTeamColor); }

		CActor* GetActor()
			{ return(CActor::FromHandle(m_actor)); }

	private:
		ActorHandle	m_actor;
		Vector3		m_pos;
		float			m_maxRange;
		Sprite		m_sprite;
	  	bool 			m_isPickup;
		bool			m_useTeamColor;
		CActor 		*GetCarrier(CActor *actor) const;

	public:
		PersistentBlip			*prev, *next;
	};
	DoublyLinkedList<PersistentBlip>	m_persistentBlips;
	Pool<PersistentBlip>					m_persistentBlipPool;

	int AttachToActors(const char *actorKey, bool couldMove,	int texture, float maxRange, bool isPickup = false, bool useTeamColor = false);
	void UpdatePersistentBlips(const Vector3 &viewerPos, const Vector3 &viewerDir);

	bool										m_persistentScanned;
	virtual bool CreatePersistentBlips() = 0;
};

///////////////////////////////////////
///////////////////////////////////////
class SPRadarInstrument : public RadarInstrument
{
public:
	// Constructor
	SPRadarInstrument();
	~SPRadarInstrument() {}

	// The update (called once per frame)
	void Update(float dt);

private:
	enum BlipTypes
	{
		BLIP_FRIENDLY,
		BLIP_MANDOWN,
		BLIP_NPC,
		BLIP_BADDIE,
		BLIP_DEADBADDIE,
		BLIPTYPES
	};
	BlipArray			m_blip[BLIPTYPES];
	void UpdateRefereeBlips(const Vector3 &viewerPos, const Vector3 &viewerDir);
	bool CreatePersistentBlips();
};

///////////////////////////////////////
///////////////////////////////////////
class MPRadarInstrument : public RadarInstrument
{
public:
	// Constructor
	MPRadarInstrument();
	~MPRadarInstrument() {}

	// The update (called once per frame)
	void Update(float dt);

private:
	enum BlipTypes
	{
		BLIP_FRIENDLY,
		BLIP_MANDOWN,
		BLIP_NPC,
		BLIP_TALKER,
		BLIPTYPES
	};
	BlipArray			m_blip[BLIPTYPES];
	void UpdateRefereeBlips(const Vector3 &viewerPos, const Vector3 &viewerDir);
	bool CreatePersistentBlips();
};

#endif
