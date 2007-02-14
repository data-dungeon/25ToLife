//============================================================================
//=
//= ActorCreator.h - handles creating actors specified in world database
//=
//============================================================================

#ifndef _ACTORCREATOR_H
#define _ACTORCREATOR_H

class SharedBody;

#define COMBATANT_LOD_HIGH		0
#define COMBATANT_LOD_MID		1
#define COMBATANT_LOD_LOW		2

//============================================================================

class ActorCreator
{
public:
	static bool InitGameSpecificGeometry( CActor* actor );
	static CActor* CreateFromParameters( ParameterSet& parms, Vector3CRef pos, float heading );
	static void  SetCombatantLOD( int lod );
	static void  SetUseLOD( bool flag );
	static int   GetCombatantLOD();
	static void  MakeCombatantPieceFilename( int piece, char* filename );
	static void  MakeCombatantTextureFilename( int texture, char* filename );

private:
	static bool InitCombatantGeometry( CActor* actor );
	static bool InitNPCGeometry( CActor* actor );
	static bool InitCombatantPieces( SharedBody* body, ParameterSet* parms );
	static bool InitCombatantTextures( SharedBody* body, ParameterSet* parms );

	static int s_combatantLOD;
	static bool s_useLOD;
};

//============================================================================

#endif //_ACTORCREATOR_H
