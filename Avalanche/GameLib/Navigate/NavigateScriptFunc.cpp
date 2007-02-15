//============================================================================
//=
//= InterfacesScriptFunc.cpp - 
//=    Script Interface routines to module "Navigate"
//=
//============================================================================

#include "Navigate/NavigatePCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

static void Script_FindBestMesh( ScriptVM* vm )
{
	Vector3 vPosition = vm->PopVector3();
	int iMeshHandle = ( int )( gbl_NavManager.FindBestMesh( vPosition ) );
	vm->Push( iMeshHandle );
}

//============================================================================

static void Script_FindRandomPosition( ScriptVM* vm )
{
	Navigate::Mesh *pMesh = ( Navigate::Mesh* )( vm->PopInt() );
	Vector3 vPosition;
	vPosition = gbl_NavManager.FindRandomPosition( pMesh );
	vm->Push( vPosition );
}

//============================================================================
//extern int PathConnection_Create_3( int meshHandle, int pathType, float pointA[ 3 ], float pointB[ 3 ], float pointC[ 3 ] );
static void Script_PathConnection_Create_2( ScriptVM* vm )
{
	Vector3 endPoint = vm->PopVector3();
	Vector3 beginPoint = vm->PopVector3();
	int pathType = vm->PopInt();
	Navigate::Mesh *pMesh = ( Navigate::Mesh* )( vm->PopInt() );

	// create the path
	Navigate::Path *pPath = NULL;
	switch( pathType )
	{
	case 0:
		pPath = gbl_NavManager.CreateOneWayPath( pMesh, 2, &beginPoint, &endPoint );
		break;
	case 1:
		pPath = gbl_NavManager.CreateBackAndForthPath( pMesh, 2, &beginPoint, &endPoint );
		break;
	case 2:
		pPath = gbl_NavManager.CreateCircuitPath( pMesh, 2, &beginPoint, &endPoint );
		break;
	default:
		ASSERT( 0 );
		break;
	}

	ASSERT( pPath != NULL );
	pPath->Initialize();

	// create the path connection
	Navigate::PathConnection *pConnection = pPath->GetConnection();

	vm->Push( ( int )pConnection );
}

//============================================================================
//extern int PathConnection_Create_3( int meshHandle, int pathType, float pointA[ 3 ], float pointB[ 3 ], float pointC[ 3 ] );
static void Script_PathConnection_Create_3( ScriptVM* vm )
{
	Vector3 endPoint = vm->PopVector3();
	Vector3 midPoint = vm->PopVector3();
	Vector3 beginPoint = vm->PopVector3();
	int pathType = vm->PopInt();
	Navigate::Mesh *pMesh = ( Navigate::Mesh* )( vm->PopInt() );

	// create the path
	Navigate::Path *pPath = NULL;
	switch( pathType )
	{
	case 0:
		pPath = gbl_NavManager.CreateOneWayPath( pMesh, 3, &beginPoint, &midPoint, &endPoint );
		break;
	case 1:
		pPath = gbl_NavManager.CreateBackAndForthPath( pMesh, 3, &beginPoint, &midPoint, &endPoint );
		break;
	case 2:
		pPath = gbl_NavManager.CreateCircuitPath( pMesh, 3, &beginPoint, &midPoint, &endPoint );
		break;
	default:
		ASSERT( 0 );
		break;
	}

	ASSERT( pPath != NULL );
	pPath->Initialize();

	// create the path connection
	Navigate::PathConnection *pConnection = pPath->GetConnection();

	vm->Push( ( int )pConnection );
}

//============================================================================
//extern int PathConnection_CheckCreationDone( int connectionHandle );
static void Script_PathConnection_CheckCreationDone( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection * )( vm->PopInt() );
	if ( pConnection->GetStatus() == Navigate::PathConnection::PATHCONNECTION_VALID )
	{
		vm->Push( 1 );
	}
	else
	{
		vm->Push( 0 );
	}
}

//============================================================================
//extern int PathConnection_CheckEndOfPath( int connectionHandle );
static void Script_PathConnection_CheckEndOfPath( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	if ( pConnection->GetStatus() == Navigate::PathConnection::PATHCONNECTION_ENDOFPATH )
	{
		vm->Push( 1 );
	}
	else
	{
		vm->Push( 0 );
	}
}

//============================================================================
//extern ( float pos[ 3 ] ) PathConnection_GetNextWaypoint( int connectionHandle );
static void Script_PathConnection_GetNextWaypoint( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	Vector3 vNextWaypoint = pConnection->GetNextWaypoint();
	vm->Push( vNextWaypoint );
}

//============================================================================
//extern float PathConnection_GetDistanceToNextWaypoint( int connectionHandle );
static void Script_PathConnection_GetDistanceToNextWaypoint( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	Vector3 vPosition = pConnection->GetPosition();
	Vector3 vWaypoint = pConnection->GetNextWaypoint();
	float fLength = ( vWaypoint - vPosition ).Length();
	vm->Push( fLength );
}

//============================================================================
//extern void PathConnection_MoveToNextWaypoint( int connectionHandle );
static void Script_PathConnection_MoveToNextWaypoint( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	bool bMoved = pConnection->MoveToNextWaypoint();
	if ( bMoved )
	{
		vm->Push( 1 );
	}
	else
	{
		vm->Push( 0 );
	}
}

//============================================================================
//extern void PathConnection_Release( int connectionHandle );
static void Script_PathConnection_Release( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	pConnection->Release();
}

//============================================================================
//extern ( float pos[ 3 ] ) PathConnection_GetDirectionToNextWaypointNormalized( int connectionHandle );
static void Script_PathConnection_GetDirectionToNextWaypointNormalized( ScriptVM* vm )
{
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	Vector3 vDirection = pConnection->GetDirectionNormal();
	vm->Push( vDirection );
}

//============================================================================
//extern void PathConnection_ConnectToClosestPoint( int connectionHandle, float pos[ 3 ] );
static void Script_PathConnection_ConnectToClosestPoint( ScriptVM* vm )
{
	Vector3 vPosition = vm->PopVector3();
	Navigate::PathConnection *pConnection = ( Navigate::PathConnection* )( vm->PopInt() );
	ASSERT( pConnection != NULL );
	pConnection->ConnectNearest( vPosition );
}

//============================================================================
//extern int Cover_StartSearch( float targetPos[ 3 ], float volumeCenter[ 3 ], float volumeRadius );
static void Script_Cover_StartSearch( ScriptVM* vm )
{
	float fRadius = vm->PopFloat();
	Vector3 vVolumeCenter = vm->PopVector3();
	Vector3 vTargetPos = vm->PopVector3();
	Navigate::Mesh *pMesh = gbl_NavManager.FindBestMesh( vVolumeCenter );

	Navigate::CoverIterator *pIterator = gbl_NavManager.CreateCoverIterator( pMesh, vTargetPos, vVolumeCenter, fRadius );

	vm->Push( ( int )pIterator );
}

//============================================================================
//extern int Cover_CheckSearchCompleted( int coverHandle );
static void Script_Cover_CheckSearchCompleted( ScriptVM* vm )
{
	Navigate::CoverIterator *pIterator = ( Navigate::CoverIterator* )vm->PopInt();
	ASSERT( pIterator != NULL );

	if ( pIterator->HasWorkToDo() )
	{
		vm->Push( 0 );
	}
	else
	{
		vm->Push( 1 );
	}
}

//============================================================================
//extern void Cover_ReleaseSearch( int coverHandle );
static void Script_Cover_ReleaseSearch( ScriptVM* vm )
{
	Navigate::CoverIterator *pIterator = ( Navigate::CoverIterator* )vm->PopInt();
	ASSERT( pIterator != NULL );

	gbl_NavManager.DestroyCoverIterator( pIterator );
}

//============================================================================
//extern ( float pos[ 3 ] ) Cover_GetBestPosition( int coverHandle, float fPreferredDistanceFromTarget, float fDesiredOffsetFromCoverPoint );
static void Script_Cover_GetBestPosition( ScriptVM* vm )
{
	float fDesiredOffset = vm->PopFloat();
	float fPreferredDistance = vm->PopFloat();
	int iCoverHandle = vm->PopInt();
	ASSERT( iCoverHandle != 0 );
	Navigate::CoverIterator *pIterator = ( Navigate::CoverIterator* )( iCoverHandle );
	Vector3 vCoverPosition( 0.0f, 0.0f, 0.0f );

	Navigate::Cover cover;
	Segment seg;
	pIterator->IterateInit();
	while ( pIterator->Iterate( cover ) )
	{
		Vector3 vMidPoint = ( ( *( cover.m_pEnd ) - *( cover.m_pBegin ) * 0.5f ) + *( cover.m_pBegin ) );
		seg.start = vMidPoint;	// cover line mid-point
		seg.end = pIterator->GetTarget();						// target position
		if ( seg.LengthSquared() >= ( fPreferredDistance * fPreferredDistance ) )
		{	// returning the first cover node that is at least preferred distance from the target position
			vCoverPosition = vMidPoint;
			break;;
		}
	}

	vm->Push( vCoverPosition );
}

//============================================================================
void Navigate_RegisterScriptFunc()
{
	SCRIPT_REGISTER( FindBestMesh );
	SCRIPT_REGISTER( FindRandomPosition );
	SCRIPT_REGISTER( PathConnection_Create_2 );
	SCRIPT_REGISTER( PathConnection_Create_3 );
	SCRIPT_REGISTER( PathConnection_CheckCreationDone );
	SCRIPT_REGISTER( PathConnection_CheckEndOfPath );
	SCRIPT_REGISTER( PathConnection_GetNextWaypoint );
	SCRIPT_REGISTER( PathConnection_GetDistanceToNextWaypoint );
	SCRIPT_REGISTER( PathConnection_MoveToNextWaypoint );
	SCRIPT_REGISTER( PathConnection_Release );
	SCRIPT_REGISTER( PathConnection_GetDirectionToNextWaypointNormalized );
	SCRIPT_REGISTER( PathConnection_ConnectToClosestPoint );
	SCRIPT_REGISTER( Cover_StartSearch );
	SCRIPT_REGISTER( Cover_CheckSearchCompleted );
	SCRIPT_REGISTER( Cover_ReleaseSearch );
	SCRIPT_REGISTER( Cover_GetBestPosition );
}
