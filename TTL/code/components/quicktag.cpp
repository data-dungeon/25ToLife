/////////////////////////////////////////////////////////////////////////////
// Quick tagging class
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/ComponentCreator.h"
#include "environ/ConfigMgr.h"
#include "components/quicktag.h"
#include "effects/customtag.h"

STANDARD_COMPONENT_CREATOR(QuickTag, QuickTag, COMPONENT_NEUTRAL_PRIORITY);

#define QUICKTAG_WIDTH	 	2.0f
#define QUICKTAG_HEIGHT		1.5
#define QUICKTAG_DELAY		2000

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
QuickTag::QuickTag(
	CCompActor &owner) :
	CActorComponent(owner)
{
	for (int i = 0; i < MAX_QUICKTAGS; i++)
		m_tag[i] = NULL;
	m_index = 0;
 	m_nextTagTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
QuickTag::~QuickTag()
{
	for (int i = MAX_QUICKTAGS - 1; i >= 0; i--)
	{
		if (m_tag[i])
		{
			delete m_tag[i];
			m_tag[i] = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void QuickTag::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	m_applyTagHandler.Initialize("QuickTag",
		MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_DUPLICAS,
		actor.GetMessageOwner(), this);

	m_checkpointReset.Initialize("KillAllAtCheckpoint",
		MESSAGE_HANDLE_IF_MASTER |	MESSAGE_HANDLE_IF_DUPLICA,
		actor.GetMessageOwner(), this);

 	m_nextTagTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Tag!
/////////////////////////////////////////////////////////////////////////////
bool QuickTag::Tag(
	const Vector3 &eye,
	const Vector3 &dir)
{
	// Time to tag yet
	if (g_timer.GetEndMS() < m_nextTagTime)
		return false;

	// Where should it go?
	CLineEntity test;
	test.Line().SetOriginBasis(eye, dir);
	test.Line().SetMaxBoundary(3.0f);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
	test.SetIgnoreEntity(&actor);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(test) || query.actor != NULL)
		return false;

	// Make sure this is a legal material
	if (!LegalMaterial(query.material))
		return false;

	// Make a good up
	Vector3 up(0.0f, 1.0f, 0.0f);
	if (Math::Zero(1.0f - Vector3::Dot(up, query.normal)))
		up = dir;

	// Is this valid?
	if (!WillFit(query.intersection, query.normal, up))
		return false;

	// Fill this out like a message
	CustomTagInfo info;
	info.pos = query.intersection + (query.normal * 0.025f);
	info.normal = query.normal;
	info.up = up;
	info.color = query.color;

	// Apply the tag
	if (!Apply(info, true))
		return false;

	// Propagate the message
	g_messageDispatcher.SendMessageToActor("QuickTag", &info, actor.GetHandle(), actor.GetHandle());

	m_nextTagTime = g_timer.GetEndMS() + QUICKTAG_DELAY;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool QuickTag::Apply(
	const CustomTagInfo &info,
	bool master)
{
	// Garbage cleanup
	if (m_tag[m_index])
	{
		delete m_tag[m_index];
		m_tag[m_index] = NULL;
	}

	// Create our decal
	int index = CParticipantIF::GetParticipantIdFromActor(CActor::ToHandle(&actor));
	if (ValidSingleton(CustomTag))
		m_tag[m_index] = g_customTag.SetupEmitter(index);

	// Make sure we have a tag
	if (!m_tag[m_index])
		return false;

	// Some handy stuff
	const float finalAlpha = 0.4f;
	const float fadeInTime = 1.5f;

	// Do it
	if (!m_tag[m_index]->Setup(info.pos, info.normal, info.up, QUICKTAG_WIDTH, QUICKTAG_HEIGHT, fadeInTime, finalAlpha))
		return false;

	// Play the particle
	const float offset = 0.1f;
	Vector3Padded particlePos = info.pos + info.normal * offset;
	Vector3Padded velUnitVector = -info.normal;
	const float scale = 1.0f;
	ParticleMgr__CreateEffect("Tag_Spray_Short", &particlePos, &velUnitVector,
			NULL, NULL, 0.0f, scale);
	Sfx::PlayStationary("spray_start", info.pos);
	Sfx::QueueStationary("spray_burst", 1.0f, info.pos);

	// Success!
	m_index++;
	if (m_index >= MAX_QUICKTAGS)
		m_index = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Make sure this is a legal material
/////////////////////////////////////////////////////////////////////////////
bool QuickTag::LegalMaterial(
u8 materal)
{
	const char *invalidMaterial[] =
	{
		"glass",
		"opaqueglass",
		"shallowwater",
		"deepwater",
		"water",
		"cloth",
		"chainlink",
		"vegitation",
		"grass",
		"dirt",
		"sand",
		"car",
		"wooddoor",
		"metaldoor",
		"metalgate",
		"softanddusty",
		"electronic",
		"ndconcrete",
		"ndpavement",
		"ndwood",
		"ndheavywood",
		"ndsheetrock",
		"ndmetal",
		"ndthinmetal",
		"ndtile",
		"ndcarpet",
		"ndcardboard",
		"fence",
		"ladder",
		"vaulttrigger",
		"vaultable",
		"climbable",
		"forcefield",
		"nothing"
	};
	const int invalidMaterials = sizeof(invalidMaterial) / sizeof(const char *);

	for (int i = 0; i < invalidMaterials; i++)
	{
		int surfaceType = g_surfaceType.GetIndex(invalidMaterial[i]);
		if (surfaceType >= 0 && surfaceType == (int)materal)
		{
			TRACE("Tried to tag on %s, BWWWWWWWAAAAAAAAANNNNNNNNNTTTTTTTTT!!!!!!!\n", invalidMaterial[i]);
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool QuickTag::WillFit(
const Vector3 &pos,
const Vector3 &normal,
const Vector3 &up)
{
	// Compute a valid right
	Vector3 right = Vector3::Cross(up, normal);
	right.SafeNormalize();
	Vector3 orthoUp = Vector3::Cross(right, normal);

	// Weee!
	Vector3 corner[4];
	const float halfWidth = QUICKTAG_WIDTH * 0.5f;
	const float halfHeight = QUICKTAG_HEIGHT * 0.5f;
	corner[0] = pos + orthoUp * halfHeight + right * halfWidth;
	corner[1] = pos - orthoUp * halfHeight + right * halfWidth;
	corner[2] = pos + orthoUp * halfHeight - right * halfWidth;
	corner[3] = pos - orthoUp * halfHeight - right * halfWidth;

	// Test each corner
	const float scanDepth = 0.02f;

	// Intersect 4 tiny lines
	Vector3 intersection[4];
	for (int i = 0; i < 4; i++)
	{
		// Setup the test object
		CLineEntity test;
		test.Line().SetOriginBasis(corner[i], normal);
		test.Line().SetMinBoundary(-scanDepth);
		test.Line().SetMaxBoundary(scanDepth);
		test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_BOTH_SIDES);
		test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
		test.SetPositiveInclude(WORLD_CATEGORY);

		// Test!
		CLineQuery query;
		if (!query.Test(test))
			return false;
		if (!LegalMaterial(query.material))
			return false;
		intersection[i] = query.intersection;
	}

	// Thisis safe
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void QuickTag::ApplyTag::HandleMessage(
	void *i_data,
	ActorHandle i_sender,
	bool i_posted)
{
	CustomTagInfo *info = (CustomTagInfo *)i_data;
	ASSERT(info);
	ASSERT(!d_ref->actor.IsNetMaster());
	d_ref->Apply(*info);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void QuickTag::CheckpointReset::HandleMessage(
	void *i_data,
	ActorHandle i_sender,
	bool i_posted)
{
	if (d_ref->m_tag)
	{
		for (int i = 0; i < MAX_QUICKTAGS; i++)
		{
			delete d_ref->m_tag[i];
			d_ref->m_tag[i] = NULL;
		}
		d_ref->m_index = 0;
	}
}

