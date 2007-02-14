//============================================================================
//=
//= CExploder - A component that will manage an actor getting split into
//=             many pieces.
//=
//============================================================================

#include "TTLPCH.h"
#include "components/Exploder.h"
#include "Components/ComponentCreator.h"
#include "Components/RigidBody.h"
#include "stage/CategoryBit.h"
#include "CollideHelper/ObbNode.h"

#include "EngineHelper/timer.h"
//#include "geomUtil/BoxExtents.h"

//#define GRAVITY -12.0f	// -9.8f
#define GRAVITY -9.8f

// Define a creator and add it to the creator list.
CUSTOM_COMPONENT_CREATOR(CExploder, Exploder, COMPONENT_NEUTRAL_PRIORITY
		)::Create( CCompActor &i_actor )
{
	CExploder* exploder = new CExploder( i_actor );

	const char* piecename	= i_actor.Parm().GetString( "exp/piece" );
	ASSERT( piecename && piecename[0] );
	exploder->AttachPieces( piecename );

	return exploder;
}

//============================================================================

class CExploderPiece
{
public:
	CExploderPiece( CInstance* instance )
	{
		// mark original instance as hidden
		instance->Hide( true );

		// remember our instance so we can create the actor later
		d_instance = instance;
		d_actor = NULL;
		d_rigid = NULL;
	}

	void Reset()
	{
		if ( d_actor )
		{
			d_actor->MarkForDeath( 0.0f );
			d_actor = NULL;
			d_rigid = NULL;
		}
	}

	void Activate( CActor* parent, const char* d_surfaceType, float d_mass  )
	{
		// create our actor
		d_actor = new CCompActor();

		// set up the geometry
		ts_Geometry geom;
		d_instance->GetGeometry( &geom );
		d_actor->SetGeometry( 0, &geom );

		// place in world
		d_origin = parent->GetBodyInWorld() + ( d_instance->Matrix().GetTranslation() * parent->GetBodyToWorld() );
		d_actor->Place( d_origin, parent->GetBodyToWorld() );

		// set up collision object
		CObbNode* node = new CObbNode;
		ASSERT_PTR(node);
		d_actor->SetCollisionNode(node, CStageEntity::SIMPLE, true);
		node->SetSurfaceName( d_surfaceType );
		node->FitObbToActor();

		// if we have any zero depth collisions, fix them.
		if ( Math::Zero( node->GetXSize() ) )
			node->SetXSize( 0.05f );
		if ( Math::Zero( node->GetYSize() ) )
			node->SetYSize( 0.05f );
		if ( Math::Zero( node->GetZSize() ) )
			node->SetZSize( 0.05f );

		d_actor->Initialize();
		d_actor->SetPositiveInclude( TERRAIN_BIT | STATIC_BIT );

		// make it a rigid body
		d_rigid = (CRigidBody*) d_actor->AttachComponent( "RigidBody" );
		ASSERT_PTR( d_rigid );
		d_rigid->SetMass( d_mass );
		d_rigid->SetInertia(10.0f, 10.0f, 10.f);
		d_rigid->SetBodyAcceleration(Vector3(0.0f, GRAVITY, 0.0f), true);
		d_rigid->SetAngVelocityInCG( Vector3( g_random.UpTo( 10.0f ), g_random.UpTo( 10.0f ), g_random.UpTo( 10.0f ) ) );
		d_actor->InitializeComponents();

		// put into the world
		g_stage.AddProtagonist( d_actor );
		d_actor->SetDeleteWhenNoCast( true );
	}

	void SetVel( Vector3CRef vel )
	{
		d_rigid->SetCGVelocityInWorld( vel );
	}

	Vector3 GetPos()
	{
		return d_origin;
	}

	CInstance* Instance() { return d_instance; }

private:
	CInstance*		d_instance;
	CCompActor*		d_actor;
	CRigidBody*		d_rigid;
	Vector3			d_origin;
};

//============================================================================

CExploder::CExploder( CCompActor& i_actor ) : CActorComponent( i_actor )
{
	Reset();
}

//============================================================================

CExploder::~CExploder()
{
	CExploderPiece** piece;
	while ((piece = d_pieces.Get(-1)) != NULL)
	{
		delete *piece;
		d_pieces.Delete(-1);
	}
}

//============================================================================
// - also setup all vars from the Maya Object attributes
//============================================================================

void CExploder::Initialize( void )
{
	d_sound			= actor.Parm().GetString( "ai/sound" );
	d_surfaceType	= actor.Parm().GetString( "SurfaceType" );
	d_mass			= actor.Parm().GetFloat( "ai/mass" );
	d_size			= actor.Parm().GetFloat( "ai/size" );
//	d_health			= actor.Parm().GetInt( "ai/health" );
	d_messageDeath	= actor.Parm().GetString( "ai/messageDeath" );	// send message at death
	d_messageDie	= actor.Parm().GetString( "ai/messageDie" );		// recieve message to die
	d_delayDeath	= actor.Parm().GetFloat( "ai/delayDeath" );
	d_lifetimer		= actor.Parm().GetFloat( "ai/delay" );

	if ( !d_messageDeath )
		d_messageDeath = "NULL";

	munitionImpactHandler.Initialize(MUNITION_IMPACT_ACTOR, 
			MESSAGE_HANDLE_IF_MASTER, actor.GetMessageOwner(), this);
	if ( d_messageDie && strlen( d_messageDie ) > 0 )
	{
		deathHandler.Initialize( d_messageDie, MESSAGE_HANDLE_IF_MASTER |
				MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this );
	}
	slaveDeathHandler.Initialize( "ExploderDestroy", MESSAGE_HANDLE_IF_DUPLICA | 
			MESSAGE_ROUTE_TO_DUPLICAS, actor.GetMessageOwner(), this );
}

//============================================================================
// Attach all pieces of the exploder
//============================================================================

void CExploder::AttachPieces( const char* piecename )
{
	int missed = 0;
	d_dead = false;

	for ( int i = 1 ; missed < 2 ; i++ )
	{
		char name[ 256 ];
		sprintf( name, "%s%d", piecename, i );

		CInstance* instance = g_loadSOM.FindInstanceByName( name );
		if ( instance )
		{
			missed = 0;
			CExploderPiece* piece = new CExploderPiece( instance );
			d_pieces.Add( piece );
		}
		else
			missed++;
	}
}

//============================================================================
// if the timer has been set - it means object has been signaled to die
// - the delay timer is counting down - at 0 - complete death
//============================================================================

void CExploder::AttemptUpdate( float i_deltaSec )
{
	if ( d_dead && d_delayDeath > 0.0f )
	{
		d_delayDeath -= i_deltaSec;
		if ( d_delayDeath <= 0.0f )
		{
			Explode( Vector3(0.0f, 0.0f, 0.0f) );
		}
	}
	else if ( d_timer > 0.0f )
	{
		d_timer -= i_deltaSec;

		if ( d_timer <= 0.0f )
		{
			// shut off all the pieces
			for ( int i = 0 ; i < d_pieces.Count() ; i++ )
			{
				d_pieces[ i ]->Reset();
//				float fade = 1.0f;
//				d_pieces[ i ]->Instance()->SetAlpha( fade );
			}
			// reset timer
			d_timer = 0.0f;
		}
	}
}

//============================================================================

void CExploder::Explode( Vector3 dir )
{
	// hide our body
	actor.Instance( 0 )->Hide( true );
	actor.SetPositiveInclude( 0 );
	d_dead = true;
	// tell anyone chained to this death - to die
	g_messageDispatcher.SendMessageToAll( d_messageDeath, NULL, INVALID_OBJECT_HANDLE );
	// tell slaves to explode
	g_messageDispatcher.SendMessageToActor( "ExploderDestroy", NULL, INVALID_OBJECT_HANDLE, actor.GetHandle() );

	Vector3 parent = actor.GetBodyInWorld();			// get parent object position in world
	Sfx::PlayStationary( d_sound, parent );			// play explosion sound

	d_timer = d_lifetimer;
	//actor.Initialize();

	// activate all the pieces
	for ( int i = 0 ; i < d_pieces.Count() ; i++ )
	{
		d_pieces[ i ]->Activate( &actor, d_surfaceType, d_mass );
		Vector3 vel;
		vel = d_pieces[ i ]->Instance()->Matrix().GetTranslation();

		Vector3 pos;
		pos = d_pieces[ i ]->GetPos();
		vel.x( ( dir.x() * 2 ) + ( d_size * (pos.x() - parent.x()) ) );
		vel.y( d_size * (pos.y() - parent.y()) );
		vel.z( ( dir.z() * 2 ) + ( d_size * (pos.z() - parent.z()) ) );

		d_pieces[ i ]->SetVel( vel );
	}
}

//============================================================================

void CExploder::Reset()
{
	// show our body
//	actor.Instance( 0 )->Hide( false );
//
//	// shut off all the pieces
//	for ( int i = 0 ; i < d_pieces.Count() ; i++ )
//	{
//		d_pieces[ i ]->Reset();
//	}
//
//	// reset timer
//	d_timer = 0.0f;
}


//============================================================================
void CExploder::MunitionImpact( const CMunitionImpact &hit )
{
	// An Exploder object has been hit by something, determine shot/hit direction
	Vector3 dir = ( hit.d_intersection - hit.d_shotOrigin ).Normalized();

	if ( !d_dead )
	{
		d_delayDeath = 0.0f;
		Explode( dir );
	}
}

//============================================================================
//EVENTS
//============================================================================

void CExploder::MunitionImpact::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	CMunitionImpact* impact = (CMunitionImpact *) i_data;
	d_ref->MunitionImpact( *impact );
}

void CExploder::CDie::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if (!d_ref->d_dead)
	{
		if ( d_ref->d_delayDeath != 0.0f )
		{
			d_ref->d_dead = true;
		}
		else
		{
			d_ref->Explode( Vector3(0.0f, 0.0f, 0.0f) );
		}
	}
}

void CExploder::SlaveDie::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
{
	if (!d_ref->d_dead)
	{
		if ( d_ref->d_delayDeath != 0.0f )
		{
			d_ref->d_dead = true;
		}
		else
		{
			d_ref->Explode( Vector3(0.0f, 0.0f, 0.0f) );
		}
	}
}

//=================================eof========================================
