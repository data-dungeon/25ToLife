/***************************************************************************/
// track secondary objectives
/***************************************************************************/
#include "ttlpch.h"
#include "avatar/sobj.h"
#include "EngineHelper/sfx.h"
#include "Environ/Munition.h"
#include "Components/KineControl/BipedKineControl.h"
#include "Components/KineControl/BipedKineControl_s.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "Components/ComponentCreator.h"
#include "frontend/savegame.h"
#include "Environ/ConfigMgr.h"
#include "weapons/armory.h"
#include "weapons/ttlweapon.h"
#include "components/Ai/AiBehavior.h"
#include "GameHelper/Translator.h"

DynList<CActor>							hostageList( 20 , 20 , false );


enum LevelObjectiveIDs
{
	O_LEVEL1_O1 = 1,
	O_LEVEL1_O2, 
	O_LEVEL1_O3, 
	O_LEVEL1_O4, 
	O_LEVEL1_O5, 
	O_LEVEL1_O6, 

	O_LEVEL2_O1, 
	O_LEVEL2_O2, 
	O_LEVEL2_O3, 
	O_LEVEL2_O4, 

	O_LEVEL3_O1, 
	O_LEVEL3_O2, 
	O_LEVEL3_O3, 
	O_LEVEL3_O4, 
	O_LEVEL3_O5, 
	O_LEVEL3_O6, 

	O_LEVEL4_O1, 
	O_LEVEL4_O2, 
	O_LEVEL4_O3, 
	O_LEVEL4_O4, 
	O_LEVEL4_O5, 
	
	O_LEVEL5_O1, 
	O_LEVEL5_O2,
	O_LEVEL5_O3, 
	O_LEVEL5_O4, 
	O_LEVEL5_O5, 
	O_LEVEL5_O6, 
	O_LEVEL5_O7, 

	O_LEVEL6_O1, 
	O_LEVEL6_O2, 
	O_LEVEL6_O3, 
	O_LEVEL6_O4, 

	O_LEVEL7_O1, 
	O_LEVEL7_O2, 
	O_LEVEL7_O3, 

	O_LEVEL8_O1, 
	O_LEVEL8_O2, 
	O_LEVEL8_O3, 
	O_LEVEL8_O4, 
	O_LEVEL8_O5, 
	O_LEVEL8_O6, 

	O_LEVEL9_O1, 
	O_LEVEL9_O2, 
	O_LEVEL9_O3, 
	O_LEVEL9_O4, 

	O_LEVEL10_O1,
	O_LEVEL10_O2,
	O_LEVEL10_O3,
	O_LEVEL10_O4,
	O_LEVEL10_O5,
	O_LEVEL10_O6,

	O_LEVEL11_O1,
	O_LEVEL11_O2,
	O_LEVEL11_O3,
	O_LEVEL11_O4,

	O_LEVEL12_O1,
	O_LEVEL12_O2,
	O_LEVEL12_O3,
	
	O_LEVEL13_O1
};


#define O_12_MIN ( 12.0 * 60.0 )
#define O_5_MIN ( 5.0 * 60.0 )
#define O_1_MIN ( 15.0f )



#define NUM_STRINGS 55

ObjectiveDataSave objectivesSave[ NUM_LEVELS ][ MAX_OBJECTIVES_PER_LEVEL ];

ObjectiveData objectives[ NUM_LEVELS ][ MAX_OBJECTIVES_PER_LEVEL ] = {
	{ // SP_LEVEL1 Warehouse
		{ O_LEVEL1_O1 , "Fight your way to the Warehouse"												, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL1_O2  , O_BOOL    , "Warehouse1"	, true  , O_NOP , O_PRI1			, O_NO_UPDATE	, O_NO_UPDATE , false  , 0  , 0 },
		{ O_LEVEL1_O2 , "Power off the electrical fence"												, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL1_O3  , O_BOOL    , "Warehouse2"	, false , O_NOP , O_PRI2			, O_NO_UPDATE	, O_NO_UPDATE , false  , 0  , FF_DONT_RESET },
		{ O_LEVEL1_O3 , "Get to your car."	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Warehouse3"	, false , O_NOP , O_PRI3			, O_NO_UPDATE	, O_NO_UPDATE , false  , 0  , FF_NO_COMPLETE_MSG },
		{ O_LEVEL1_O4 , "Take %d Human Shields"															, 2.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Warehouse4"	, true  , O_GTE , O_HUMAN_SHIELD	, O_UPDATE		, O_NO_UPDATE , false  , 0  , 0 },
		{ O_LEVEL1_O5 , "Defeat %d enemies"																	, 20.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Warehouse5"	, true  , O_GTE , O_KILLS			, O_NO_UPDATE	, O_NO_UPDATE , false  , 0  , 0 },
		{ O_LEVEL1_O6 , "Achieve %d%% accuracy"															, 15.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Warehouse6"	, true  , O_GTE , O_ACCURACY		, O_WAIT			, O_NO_UPDATE , false  , 0  , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																				, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF			, O_NO_UPDATE	, O_NO_UPDATE , false  , 0  , 0 },
	},
	{ // SP_LEVEL2 Apartments
		{ O_LEVEL2_O1 , "Get to apartment (building 4, apt 25)"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL2_O2  , O_BOOL    , "Apartments1" , true  , O_NOP , O_PRI1   , O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_DONT_RESET | FF_NO_COMPLETE_MSG },
		{ O_LEVEL2_O2 , "Return to your car."	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Apartments2" , false , O_NOP , O_MARK_AT_END   , O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL2_O3 , "Cover up %d tags"										, 4.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Apartments3" , true  , O_GTE  , O_TAGS	, O_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL2_O4 , "Defeat %d enemies"										, 25.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Apartments4" , true  , O_GTE , O_KILLS  , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"													, 0.0f	  , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF  , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"													, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF  , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"													, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF  , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL3 Downtown
		{ O_LEVEL3_O1 , "Rob the bank"							, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL3_O2  , O_BOOL    , "Downtown1"	, true  , O_GTE , O_LOOT		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL3_O2 , "Sneak out the back exit"				, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL3_O3  , O_BOOL    , "Downtown2"	, false , O_NOP , O_PRI2		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL3_O3 , "Escape to your getaway car"			, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Downtown3"	, false , O_NOP , O_PRI3		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL3_O4 , "Break into %d ATMs"					, 4.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Downtown4"	, true  , O_GTE , O_CASH		, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL3_O5 , "Defeat %d enemies"						, 25.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Downtown5"	, true  , O_GTE , O_KILLS		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL3_O6 , "Complete level without dying"	, 1.0f  , 0.0f  , 0.0f , O_BONUS    , -1           , O_BOOL    , "Downtown6"	, true  , O_LT  , O_DEATH		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"									, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL4 Culdesac
		{ O_LEVEL4_O1 , "Locate the missing evidence"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL4_O2  , O_BOOL    , "Culdesac1"	, true  , O_GTE , O_LOOT		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL4_O2 , "Get the evidence to Mendoza’s car!"								, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Culdesac2"	, false , O_NOP , O_PRI2		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL4_O3 , "Keep your rookie partner alive"									, 1.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_BOOL    , "Culdesac3"	, true  , O_NOP , O_BUDDYDEAD	, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL4_O4 , "Apprehend %d felons"													, 5.0f     , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Culdesac4"	, true  , O_EQ  , O_APPREHEND	, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL4_O5 , "Achieve %d%% accuracy"												, 20.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Culdesac5"	, true  , O_GTE , O_ACCURACY	, O_WAIT			, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																	, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																	, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL5 Club
		{ O_LEVEL5_O1 , "Find Shaun Calderon"												, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL5_O2  , O_BOOL		, "Club1"		, true	, O_NOP , O_PRI1			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL5_O2 , "Follow Shaun to the rooftops!"				, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL5_O3  , O_BOOL		, "Club5"		, false  , O_NOP , O_PRI2			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL5_O3 , "Get to the Subway!"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1			  , O_BOOL		, "Club6"		, false  , O_NOP , O_PRI3			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL5_O4 , "Apprehend %d drug dealers"										, 4.0f     , 0.0f  , 0.0f , O_BONUS	  , -1			  , O_INT		, "Club2"		, true	, O_GTE , O_STONER		, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL5_O5 , "Achieve %d%% accuracy"											, 25.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT		, "Club3"		, true	, O_GTE , O_ACCURACY		, O_WAIT			, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL5_O6 , "Register %d head shots"											, 8.0f     , 0.0f  , 0.0f , O_STAT    , -1           , O_INT		, "Club4"		, true	, O_GTE , O_HEADSHOTS	, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE	, "O_NO_OBJ"   , false	, O_NOP , O_UNDEF			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL6 Subway
		{ O_LEVEL6_O1 , "Trap Shaun in Subway Train"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1	, O_BOOL    , "Subway1"		, true  , O_NOP , O_PRI1		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL6_O2 , "Free a cop."						, 1.0f     , 0.0f  , 0.0f , O_BONUS	  , -1	, O_INT	   , "Subway4"		, false , O_GTE , O_FREEACOP	, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL6_O3 , "Achieve %d%% accuracy"		, 30.0f    , 0.0f  , 0.0f , O_STAT    , -1	, O_INT     , "Subway2"		, true  , O_GTE , O_ACCURACY	, O_WAIT			, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL6_O4 , "Defeat %d enemies"				, 24.0f    , 0.0f  , 0.0f , O_STAT    , -1	, O_INT     , "Subway3"		, true  , O_GTE , O_KILLS 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1	, O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1	, O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1	, O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL7 Tijuana
		{ O_LEVEL7_O1 , "Find Riggs at the 'Curtains Club.'"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Tijuana1"		, true  , O_NOP , O_PRI1  , O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL7_O2 , "Tag over %d gang tags"																						, 4.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Tijuana2"		, true  , O_GTE , O_TAGS  , O_UPDATE	 , O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL7_O3 , "Defeat %d enemies"																								, 60.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Tijuana3"		, true  , O_GTE , O_KILLS , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																											, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																											, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																											, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																											, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"    , false , O_NOP , O_UNDEF , O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 }, 
	},
	{ // SP_LEVEL8 Casino
		{ O_LEVEL8_O1 , "Find the Security Room."	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL8_O2  , O_BOOL    , "Casino1"	, true  , O_NOP , O_PRI1 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL8_O2 , "Collect the cash"													, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL8_O3  , O_BOOL    , "Casino2"	, true  , O_GTE , O_LOOT 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 ,  FF_NO_COMPLETE_MSG },
		{ O_LEVEL8_O3 , "Escape with the loot"												, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Casino3"	, true  , O_NOP , O_PRI1 		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL8_O4 , "Rob %d cash carts"													, 5.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Casino4"	, true  , O_GTE , O_CASH		, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL8_O5 , "Defeat %d enemies"													, 45.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Casino5"	, true  , O_GTE , O_KILLS		, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL8_O6 , "Register %d head shots"											, 12.0f     , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Casino6"	, true  , O_GTE , O_HEADSHOTS	, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"	, false , O_NOP , O_UNDEF		, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL9 Penthouse
		{ O_LEVEL9_O1 , "Find Saragosa and kill him."	, 1.0f   , 0.0f	, 0.0f , O_PRIMARY , -1	, O_BOOL    , "Penthouse1" , true  , O_NOP , O_SARAGOSADEAD	, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL9_O2 , "Register %d head shots"													, 16.0f  , 0.0f	, 0.0f , O_STAT    , -1 , O_INT     , "Penthouse3" , false , O_GTE , O_HEADSHOTS		, O_UPDATE		, O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL9_O3 , "Defeat %d enemies"															, 32.0f  , 0.0f	, 0.0f , O_STAT    , -1 , O_INT     , "Penthouse2" , false , O_GTE , O_KILLS			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																		, 0.0f	, 0.0f	, 0.0f , O_NO_OBJ  , -1 , O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF			, O_NO_UPDATE	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																		, 0.0f	, 0.0f	, 0.0f , O_NO_OBJ  , -1 , O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF			, O_NO_UPDATE 	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																		, 0.0f	, 0.0f	, 0.0f , O_NO_OBJ  , -1 , O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF			, O_NO_UPDATE 	, O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ    , "NO_OBJ"																		, 0.0f	, 0.0f	, 0.0f , O_NO_OBJ  , -1 , O_NO_TYPE , "O_NO_OBJ"   , false , O_NOP , O_UNDEF			, O_NO_UPDATE 	, O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL10 Prison
		{ O_LEVEL10_O1 , "Open the door to Cell Block A"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL10_O2 , O_BOOL    , "Prison1"	, true  , O_NOP , O_PRI1 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL10_O2 , "Open door to Courtyard section"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL10_O3 , O_BOOL    , "Prison2"	, false , O_NOP , O_PRI1 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL10_O3 , "Open the door to the Warehouse"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL10_O4 , O_BOOL    , "Prison3"	, false , O_NOP , O_PRI1 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL10_O4 , "Get out of the Prison"				, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1			  , O_BOOL    , "Prison4"	, false , O_NOP , O_MARK_AT_END 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL10_O5 , "Protect Lazarus"						, 1.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_BOOL    , "Prison5"	, true  , O_NOP , O_LAZARUSDEAD	, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL10_O6 , "Defeat %d enemies"						, 30.0f    , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Prison6"	, true  , O_GTE , O_KILLS 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"									, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"	, false , O_NOP , O_UNDEF 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL11 Mall
		{ O_LEVEL11_O1 , "Find the DRJays.com clothing store."	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , O_LEVEL11_O2	, O_BOOL    , "Mall1"		, true  , O_NOP , O_PRI1			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL11_O2 , "It's payback time, kill Mendoza."												, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1				, O_BOOL    , "Mall2"		, false , O_NOP , O_MENDOZADEAD	, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_DONT_RESET | FF_NO_COMPLETE_MSG },
		{ O_LEVEL11_O4 , "Break into %d ATMs"																	, 5.0f     , 0.0f  , 0.0f , O_BONUS   , -1				, O_INT     , "Mall3"		, true  , O_GTE , O_CASH			, O_UPDATE	  , O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL11_O3 , "Complete level without dying"																, 1.0f , 0.0f  , 0.0f , O_BONUS    , -1				, O_BOOL    , "Mall4"		, true  , O_LT  , O_DEATH		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"																					, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1				, O_NO_TYPE , "O_NO_OBJ"	, false , O_NOP , O_UNDEF 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"																					, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1				, O_NO_TYPE , "O_NO_OBJ"	, false , O_NOP , O_UNDEF 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"																					, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1				, O_NO_TYPE , "O_NO_OBJ"	, false , O_NOP , O_UNDEF 			, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
	},																																																																																	
	{ // SP_LEVEL12 Hacienda
		{ O_LEVEL12_O1 , "Serve Shaun some justice"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "Hacienda1"	 , true  , O_NOP , O_MARK_AT_END		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_LEVEL12_O2 , "Find %d Secret Stashes"		, 5.0f     , 0.0f  , 0.0f , O_BONUS   , -1           , O_INT     , "Hacienda2"	 , true  , O_GTE , O_CASH 	   , O_UPDATE	  , O_NO_UPDATE , false , 0 , 0 },
		{ O_LEVEL12_O3 , "Defeat %d enemies"				, 30.0     , 0.0f  , 0.0f , O_STAT    , -1           , O_INT     , "Hacienda3"	 , true  , O_GTE , O_KILLS		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
	},
	{ // SP_LEVEL13 beatdown!
		{ O_LEVEL13_O1 , "Kill or be killed!"	, 1.0f     , 0.0f  , 0.0f , O_PRIMARY , -1           , O_BOOL    , "beatdown1"	 , true  , O_NOP , O_MARK_AT_END		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , FF_NO_COMPLETE_MSG },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
		{ O_NO_OBJ     , "NO_OBJ"							, 0.0f     , 0.0f  , 0.0f , O_NO_OBJ  , -1           , O_NO_TYPE , "O_NO_OBJ"     , false , O_NOP , O_UNDEF		, O_NO_UPDATE , O_NO_UPDATE , false , 0 , 0 },
	},
};


// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR( PlayerStatsGrabber , PlayerStatsGrabber , COMPONENT_NEUTRAL_PRIORITY );

/***************************************************************************/
// Globals
/***************************************************************************/
PlayerStatsGrabber::PlayerStatsGrabber( CCompActor &actor ):CActorComponent( actor )
{
	//Initialize();
}

/***************************************************************************/
PlayerStatsGrabber::~PlayerStatsGrabber( void )
{
	//Initialize();
}

/***************************************************************************/
// Reset to defaults
/***************************************************************************/
void PlayerStatsGrabber::Initialize( void )
{
	objectTriggerHandler.Initialize( "ObjectiveComplete" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	shaunDeadHandler.Initialize( "shaundead" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	buddyDeadHandler.Initialize( "BuddyDied" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	buddySafeHandler.Initialize( "buddysafe" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	lazarusDeadHandler.Initialize( "LazarusDied" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	lazarusSafeHandler.Initialize( "LazarusSafe" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	saragosaDeadHandler.Initialize( "Boss_A3_IsDead" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	mendozaDeadHandler.Initialize( "mendozadied" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	jdubDeadHandler.Initialize( "jdub" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );


	d_copTakerDeadHandler.Initialize( "CopTaker_IsDead" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_copDeadHandler.Initialize( "HostageCop_IsDead" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	//d_hostageGrabbedHandler.Initialize( "HostageGrabbed", MESSAGE_HANDLE_IF_MASTER, g_referee.GetMessageOwner() , this );
	d_hostageGrabbedHandler.Initialize( "HostageTaker", MESSAGE_HANDLE_IF_MASTER , g_players.GetPlayer( 0 )->GetMessageOwner() , this );
	// watch bullets
	munitionFiredHandler.Initialize( "MunitionFired" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	munitionImpactHandler.Initialize( "MunitionImpact" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	d_killsHandler.Initialize( "RecordKill" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	d_sneakStartHandler.Initialize( "Go_Objective_2" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_sneakStopHandler.Initialize( "End_Objective_2" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	d_apprehendHandler.Initialize( "RecordApprehend" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_hostageHandler.Initialize( "Hostage" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_tagsHandler.Initialize( "RecordTagOwnage" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_gotCashHandler.Initialize( "GotCash" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_lootPickupHandler.Initialize( "HaveLoot" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	Reset();
}

/***************************************************************************/
// Reset to defaults
/***************************************************************************/
void PlayerStatsGrabber::Reset( void )
{
	hostageList.Clear();
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::BeginFrame()
{
}


//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::ObjectTrigger::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	l , o;
	char	*s = (char *)i_data;

	ASSERT_PTR( d_ref );

	// hack this for now -- if I have time I'll have this changed on the art end.
	if( !stricmp( s , "LazarusSafe" ) )
	{
		o = g_secondaryObjective.GetObjectiveByType( O_LAZARUSDEAD );
		if( o!=-1 )
		{
			if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
			    objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )	
			{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_COMPLETE );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_COMPLETE_MESSAGE;
			}
		}
		return;
	}

	for( l=0 ; l<O_MAX_LEVELS ; l++ )
	{
		for( o=0 ; o<MAX_OBJECTIVES_PER_LEVEL ; o++ )
		{
			if( !stricmp( s , objectives[ l ][ o ].getUnlockable() ) )
			{
				if( objectives[ l ][ o ].getDisplayType() == O_BOOL )
				{
					objectives[ l ][ o ].setNumItemsObtained( 1.0 );
				}
				else
				{
					// handle other types here
				}
			}
		}
	}
}


//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CCopDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_FREEACOP );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
		    objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_FAIL );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_FAIL_MESSAGE;
		}
	}

	g_secondaryObjective.setCopDead( true );
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CjdubDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int o;
	CActor *actor = g_stage.MasterCast().FindActorByInstanceName( "dispatcher" );
	
	ASSERT_PTR( d_ref );

	// did the avatar die at the same time that the rocket guy died?  if so then don't end the level. respawn will happen
	CActor *player = g_players.GetPlayer(0);
	HealthIF *healthIF = (HealthIF *)player->GetInterface(HealthIF::GetID());
	bool isDead = (healthIF && !healthIF->IsAlive());
	if( isDead )
		return;

	// have we finished all the primary objectives?
	o = g_secondaryObjective.GetObjectiveByType( O_PRI2 );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE )
		{
			g_secondaryObjective.markFinished(); // the objective system will now check to see if this level needs to be ended.
			return;
		}
	}

	if( actor )
		g_messageDispatcher.SendMessageToActor( "fin" , NULL , INVALID_OBJECT_HANDLE , actor->GetHandle() );
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CCopTakerDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int o;

	ASSERT_PTR( d_ref );

	// cop is dead so objective has already been marked as FAIL
	if( g_secondaryObjective.getCopDead() )
		return;

	o = g_secondaryObjective.GetObjectiveByType( O_FREEACOP );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
		    objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_COMPLETE );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_COMPLETE_MESSAGE;
		}
	}
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::CPauseObjTimer::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	ASSERT_PTR( d_ref );

	d_ref->timerPaused = true;
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::CUnpauseObjTimer::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	ASSERT_PTR( d_ref );

	d_ref->timerPaused = false;
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CShaunDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_SHAUNDEAD );
	if( o!=-1 )
	{
		objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
		objectives[ g_secondaryObjective.GetLevel() ][ o ].forceUpdateMessage = true;		
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CBuddyDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_BUDDYDEAD );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
			 objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_FAIL );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_FAIL_MESSAGE;
		}
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CLazarusDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_LAZARUSDEAD );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
			 objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_FAIL );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_FAIL_MESSAGE;
		}
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CLazarusSafe::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_LAZARUSDEAD );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_COMPLETE );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_COMPLETE_MESSAGE;
		}
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CBuddySafe::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_BUDDYDEAD );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&
			 objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_COMPLETE );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_COMPLETE_MESSAGE;
		}
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CSaragosaDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_SARAGOSADEAD );
	if( o!=-1 )
	{
		objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CMendozaDead::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_MENDOZADEAD );
	if( o!=-1 )
	{
		objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CSneakStart::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	ASSERT_PTR( d_ref );

	g_secondaryObjective.SetWatchSneak( true );
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::CSneakStop::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int	o;

	ASSERT_PTR( d_ref );

	g_secondaryObjective.SetWatchSneak( false );

	o = g_secondaryObjective.GetObjectiveByType( O_SNEAK );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
		{
			objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
		}
	}
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::MunitionFired::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	CMunitionImpact* hit = (CMunitionImpact *)i_data;

	ASSERT_PTR( d_ref );
	uint8 a = CParticipantIF::GetParticipantIdFromActor(i_sender);
	if (a == INVALID_PARTICIPANT)
	{
		return;
	}
	CActor* actor = g_referee.GetActor( a );

	// check for sneaking.  we are discovered if anybody fires a bullet
	if( g_secondaryObjective.isWatchingSneak() )
	{
		int o;

		o = g_secondaryObjective.GetObjectiveByType( O_SNEAK );
		if( o!=-1 )
		{
			if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )
			{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_FAIL );
				objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_FAIL_MESSAGE;
			}
		}
	}

	if( ((CCombatant*)actor)->GetIsNpc() )
	{
		return;
	}
	g_secondaryObjective.IncShotsFired();
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::Apprehend::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int o;

	ASSERT_PTR( d_ref );
	uint8 a = CParticipantIF::GetParticipantIdFromActor(i_sender);
	if (a == INVALID_PARTICIPANT)
	{
		return;
	}
	CActor* actor = g_referee.GetActor( a );
	const char *name = actor->Instance()->Name();

	// this can only be hackish right now -- very ugly indeed.
	o = g_secondaryObjective.GetObjectiveByType( O_STONER );
	if( o!=-1 )
	{
		if( !strcmp( name , "NPC_StonerA1" ) || 
			!strcmp( name , "NPC_StonerA2" ) ||
			!strcmp( name , "NPC_StonerA3" ) ||
			!strcmp( name , "NPC_StonerA4" ) ||
			!strcmp( name , "NPC_StonerA5" ) )
		{
			objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
			objectives[ g_secondaryObjective.GetLevel() ][ o ].forceUpdateMessage = true;
		}
	}

	// don't count friendly npc as apprehended
	int npc_team = actor->Parm().GetInt( "npc_team" );
	if( npc_team==AiBehavior::OBJNPCTEAM_FRIENDLY || 
		npc_team==AiBehavior::OBJNPCTEAM_AMBIENT || 
		npc_team==AiBehavior::OBJNPCTEAM_NEUTRAL )
	{
		return;
	}

	g_secondaryObjective.IncNumApprehended();
}

//***************************************************************************
//***************************************************************************
void PlayerStatsGrabber::Hostage::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	ASSERT_PTR( d_ref );

	g_secondaryObjective.IncNumHostages();
}

//******************************************************************************
//******************************************************************************
void PlayerStatsGrabber::CTag::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{

	g_secondaryObjective.IncNumTags();
}

//******************************************************************************
//******************************************************************************
void PlayerStatsGrabber::CLootPickup::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int			o;

	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_LOOT );
	if( o!=-1 )
	{
		objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
	}
}

/***************************************************************************/
void PlayerStatsGrabber::MunitionImpact::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	ASSERT_PTR( d_ref );
	bool hit = (bool)( i_data != 0 );
	if( !hit )
	{
		return;
	}
}

//***************************************************************************
// mark kills
//***************************************************************************
void PlayerStatsGrabber::CKill::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	const char *s;

	uint8 victim = (uint8)(0xFF & (uint32)i_data);
	if (victim == INVALID_PARTICIPANT)
	{
		return;
	}
	CActor* victimActor = g_referee.GetActor( victim );
	s = ((CCombatant*)victimActor)->GetClassName();

	uint8 attacker = (uint8)(0xFF & ((uint32)i_data >> 8));
   if (attacker == INVALID_PARTICIPANT)
	{
		return;
	}
	CActor* attackerActor = g_referee.GetActor( attacker );

	if( attacker != g_players.GetParticipantId( 0 ) )
	{
		return;
	}

	int npc_team = victimActor->Parm().GetInt( "npc_team" );
	switch( npc_team )
	{
		case AiBehavior::OBJNPCTEAM_ENEMY:
		case AiBehavior::OBJNPCTEAM_ENEMY2:
			g_secondaryObjective.IncNumKills();
		break;

		case AiBehavior::OBJNPCTEAM_FRIENDLY:
			g_secondaryObjective.IncNumFriendlyKills();
		break;
	
		case AiBehavior::OBJNPCTEAM_AMBIENT:
			g_secondaryObjective.IncNumFriendlyKills();
		case AiBehavior::OBJNPCTEAM_NEUTRAL:
		break;
		
		default:
		break;
	}
}

//============================================================================
void PlayerStatsGrabber::HostageGrabbed::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int			o;
	CCompActor	&a = d_ref->actor;
	
	ASSERT_PTR( d_ref );


	// is this the avatar?
	if( &a == g_players.GetPlayer( 0 ) )
	{
		uint8 aa = CParticipantIF::GetParticipantIdFromActor(i_data);
		if (aa == INVALID_PARTICIPANT)
		{
			return;
		}
		CActor* actor = g_referee.GetActor( aa );

//		if( actor == &a )
//			return;

		if( hostageList.Find( actor )==-1 )
		{
			hostageList.Add( actor );

			o = g_secondaryObjective.GetObjectiveByType( O_HUMAN_SHIELD );
			if( o!=-1 )
			{
				objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
				objectives[ g_secondaryObjective.GetLevel() ][ o ].forceUpdateMessage = true;
			}
			g_secondaryObjective.IncNumHostages();
		}
	}
}

//============================================================================
void PlayerStatsGrabber::CGotCash::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int			o;
	//CCompActor	&a = d_ref->actor;
	
	ASSERT_PTR( d_ref );

	o = g_secondaryObjective.GetObjectiveByType( O_CASH );
	if( o!=-1 )
	{
		objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained++;
		objectives[ g_secondaryObjective.GetLevel() ][ o ].forceUpdateMessage = true;	
	}
}

CInterface *PlayerStatsGrabber::GetInterface( uint i_id )
{
	return NULL;
}








// singleton.
DeclareSingleton( SecondaryObjective );

/***************************************************************************/
// Globals
/***************************************************************************/
//SecondaryObjective::SecondaryObjective( CCompActor &actor ):CActorComponent( actor )
SecondaryObjective::SecondaryObjective()
{
	Initialize();
}

//***************************************************************************
//***************************************************************************
SecondaryObjective::~SecondaryObjective( void )
{
	//Initialize();
}

void SecondaryObjective::GetObjectiveString( int o , char *s )
{
	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	int	displayType   = objectives[ currentLevel ][ o ].getDisplayType();
	int	numItems   = (int)objectives[ currentLevel ][ o ].getNumItems();
	char s1[ 40 ];

	s1[ 0 ] = 0;
	GetObjectiveValue( o , s1 );
	switch( displayType )
	{
		case O_INT:
			sprintf( s , g_translator.Translate( objectives[ currentLevel ][ o ].getObjectiveString() ) , numItems );
		break;

		case O_TIME:
			sprintf( s , g_translator.Translate( objectives[ currentLevel ][ o ].getObjectiveString() ) , (int)((float)numItems/60.0f) );
		break;

		case O_BOOL:
			strcpy( s , g_translator.Translate( objectives[ currentLevel ][ o ].getObjectiveString() ) );
		break;
	}
	//strcat( s , s1 );
   return;
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::GetRewardString( int o , char *s )
{
	char *s1;

	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	s[ 0 ] = 0;
	s1 = GameLocks::KeyDescription( objectives[ currentLevel ][ o ].getUnlockable() );
	if( s1 != NULL )
		strcpy( s , s1 );
   return;
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::GetUnlockableString( int o , char *s )
{
	char *s1;

	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	s[ 0 ] = 0;
	s1 = objectives[ currentLevel ][ o ].getUnlockable();
	if( s1 != NULL )
		strcpy( s , s1 );
   return;
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::GetObjectiveValue( int o , char *s )
{
	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	int	objectiveType = objectives[ currentLevel ][ o ].getObjectiveType();
	int	displayType   = objectives[ currentLevel ][ o ].getDisplayType();
	int	n;

	switch( objectiveType )
	{
		case O_PRI1:
			return;
		break;

		case O_PRI2:
			return;
		break;

		case O_PRI3:
			return;
		break;

		case O_PRI4:
			return;
		break;

		case O_BUDDYDEAD:
		case O_LAZARUSDEAD:
		case O_SARAGOSADEAD:
		case O_MENDOZADEAD:
			sprintf( s , "" );
			return;
		break;

		case O_BONUS1:
			if( displayType == O_INT )
			{
				n = (int)objectives[ currentLevel ][ o ].getNumItemsObtained();
			}
			else
			{
				return;
			}
		break;

		case O_HUMAN_SHIELD:
		case O_CASH:
		case O_LOOT:
		case O_STONER:
			n = (int)objectives[ currentLevel ][ o ].getNumItemsObtained();
		break;

		case O_TAGS:
			n = g_secondaryObjective.GetNumTags();
		break;

		case O_HEADSHOTS:
			n = g_secondaryObjective.GetNumHeadshots();
		break;

		case O_KILLS:
			n = g_secondaryObjective.GetNumKills();
		break;

		case O_APPREHEND:
			n = g_secondaryObjective.GetNumApprehended();
 		break;

		case O_ACCURACY:
			if( g_secondaryObjective.GetShotsFired()>0 )
			{
				n = (int)(( 100*g_secondaryObjective.GetNumHits() )/g_secondaryObjective.GetShotsFired());
			}
			else
			{
				n = 0;
			}
			sprintf( s , "%i%%" , n );
			return;
		break;

		case O_GAMETIME:
			//t = g_secondaryObjective.GetTimePlayed();
			//sprintf( s , "%02d:%02d:%02d" , (long)(t/(3600.0f))  , ((long)(t/(60.0f)))%60 , ((long)t)%60 );
			sprintf( s , "-" );
			return;
		break;

		default:
			sprintf( s , "-" );
			return;
		break;
	}
	sprintf( s , "%i" , n );

	return;
}

//***************************************************************************
//***************************************************************************
void SecondaryObjective::GetStatValue( int o , char *s )
{
	float	t = 0.0f;
	int	n = 0;

	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	switch( o )
	{
		case O_HOSTAGE:
			n = g_secondaryObjective.GetNumHostages();
		break;

		case O_HEADSHOTS:
			n = g_secondaryObjective.GetNumHeadshots();
		break;

		case O_KILLS:
			n = g_secondaryObjective.GetNumKills();
		break;

		case O_APPREHEND:
			n = g_secondaryObjective.GetNumApprehended();
 		break;

		case O_CIVILIANS:
			n = g_secondaryObjective.GetNumFriendlyKills();
 		break;

		case O_ACCURACY:
			if( g_secondaryObjective.GetShotsFired()>0 )
			{
				n = (int)(( 100*g_secondaryObjective.GetNumHits() )/g_secondaryObjective.GetShotsFired());
			}
			else
			{
				n = 0;
			}
			sprintf( s , "%i%%" , n );
			return;
 		break;

		case O_GAMETIME:
			t = g_secondaryObjective.GetTimePlayed();
			sprintf( s , "%02d:%02d:%02d" , (int)(t/(3600.0f))  , (int)(t/(60.0f))%60 , ((int)t)%60 );
			return;
		break;
	}
	sprintf( s , "%i" , n );

	return;
}

void SecondaryObjective::GetTimeString( char *s )
{
	float t;
	t = g_secondaryObjective.GetTimePlayed();
	sprintf( s , "%02d:%02d:%02d" , (int)(t/(3600.0f))  , (int)(t/(60.0f))%60 , ((int)t)%60 );
}


//***************************************************************************
//***************************************************************************
void SecondaryObjective::GetStatString( int o , char *s )
{
	char *s1 = NULL;

	if( currentLevel >= NUM_LEVELS )
	{
		s[ 0 ] = 0;
		return;
	}

	switch( o )
	{
		case O_HEADSHOTS:
			s1 = "Head Shots";
		break;

		case O_KILLS:
			s1 = "Enemies Killed";
		break;

		case O_APPREHEND:
			s1 = "Felons Apprehended";
 		break;

		case O_HOSTAGE:
			s1 = "Hostages Taken";
 		break;

		case O_CIVILIANS:
			s1 = "Civilians Killed";
 		break;

		case O_ACCURACY:
			s1 = "Shooting Accuracy";
 		break;

		case O_GAMETIME:
			s1 = "Time";
		break;
	}
	strcpy( s , s1 );

	return;
}

//***************************************************************************
//***************************************************************************
bool SecondaryObjective::IsObjectiveMet( int o )
{
	if ( currentLevel >= NUM_LEVELS )
		return false;

	int	op			= objectives[ currentLevel ][ o ].getOperator(); 
	float	have		= objectives[ currentLevel ][ o ].getNumItemsObtained();
	float	needed	= objectives[ currentLevel ][ o ].getNumItems();
	
	if( objectives[ currentLevel ][ o ].getCurrentObjState() == O_FAIL )
	{
		return false;
	}

	// exclude some loss conditions
	int	objectiveType = objectives[ currentLevel ][ o ].getObjectiveType();
	switch( objectiveType )
	{
		case O_SARAGOSADEAD:
		case O_MENDOZADEAD:
			if( objectives[ g_secondaryObjective.GetLevel() ][ o ].numItemsObtained == 1 )
			{
				return true;
			}
		break;
	}

	if( objectives[ currentLevel ][ o ].getCurrentObjState() == O_COMPLETE )
	{
		return true;
	}

	if( objectives[ currentLevel ][ o ].getDisplayType() == O_BOOL )
	{
		if( have==1  && needed==1 )
			return 1;
	}

	switch( op )
	{
		case O_EQ:
			if( needed == have )
			{
				return true;
			}
		break;

		case O_GT:
			if( have > needed )
			{
				return true;
			}
		break;

		case O_GTE:
			if( have >= needed )
			{
				return true;
			}
		break;

		case O_LT:
			if( have < needed  )
			{
				return true;
			}
		break;

		case O_LTE:
			if( have <= needed  )
			{
				return true;
			}
		break;
	}
	
	return false;
}

/***************************************************************************/
// Reset to defaults
/***************************************************************************/
void SecondaryObjective::Initialize( void )
{
	d_deathHandler.Initialize( "ManDown" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_setCheckpointHandler.Initialize( "SetCheckpoint" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	
	d_pauseObjTimerHandler.Initialize( "PauseObjTimer" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	d_unpauseObjTimerHandler.Initialize( "UnpauseObjTimer" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	d_spawnAvatarHandler.Initialize( "PlayerSpawning" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );
	
	d_dropHostageHandler.Initialize( "DropHostage" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	//d_checkpointReset.Initialize( "KillAllAtCheckpoint" , MESSAGE_HANDLE_IF_MASTER , g_referee.GetMessageOwner() , this );

	Reset();
}

// hack -- it works but this is not really the place to put a message bridge from the cinematic code to the avatar.
//void SecondaryObjective::CheckpointReset::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
//{
//	ASSERT_PTR( d_ref );
//
//	// only look at culdesac for this hack
//	if( d_ref->currentLevel != 3 )
//		return;
//
//	int checkpoint = (int)i_data;
//
//	// and only look for checkpoint 4 -- the last checkpoint
//	if( checkpoint != 4 )
//		return;
//
//	// now, if all the above have been met then activate the last primary objective for the level.
//	// this objective is activated by a message and if the avatar dies on checkpoint 4 then this
//	// trigger is not activated and the level cannot be finished.
//	CActor *actor = g_stage.MasterCast().FindActorByInstanceName( "Objective" );
//	if( actor )
//		g_messageDispatcher.SendMessageToActor( "HaveLoot" , NULL , INVALID_OBJECT_HANDLE , actor->GetHandle() );
//}

// hack -- it works but this is not really the place to put a message bridge from the cinematic code to the avatar.
void SecondaryObjective::CDropHostage::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	CActor *actor = g_players.GetPlayer( 0 );

	if( actor!=NULL )
	{
		InteractionMonitor	*monitor = (InteractionMonitor*)((CCompActor*)actor)->GetComponentByName( "InteractionMonitor" );
		if( monitor != NULL )
		{
			if( monitor->GetInteractionType() == InteractionMonitor::INTERACTION_RELEASEHOSTAGE )
			{
				CProtagonist *target = (CProtagonist *)monitor->GetInteractionTargetActor();
				g_messageDispatcher.SendMessageToActor( "HostageTaker", CMessageDispatcher::HandleToData(INVALID_OBJECT_HANDLE), actor->GetHandle(), actor->GetHandle() );	
				g_messageDispatcher.SendMessageToActor( "Hostage", 0, INVALID_OBJECT_HANDLE, target->GetHandle() );
			}
		}
	}
}

/***************************************************************************/
// Reset to defaults
/***************************************************************************/
void SecondaryObjective::Reset( void )
{
	int	l;
	int	o;
	bool	firstPri;

	numKills = 0;
	shotsFired = 0;
	numTimesHit = 0;
	numHits = 0;
	numHeadshots = 0;
	startTime = g_timer.GetStartSec();

	numTags = 0;
	numFriendlyKills = 0;
	numFriendlyHits = 0;
	numApprehended = 0;
	numHostages = 0;
	currentLevel = 0;
	currPrimary = 0;
	currBonus = 0;
	currStat = 0;
	timer = 0;
	lastTime	= 0;
	timerPaused = false;

	watchSneak = false;

	finished = false;


	// checkpoint save stuff
	tnumKills = 0;
	tshotsFired = 0;
	tnumTimesHit = 0;
	tnumHits = 0;
	tnumHeadshots = 0;
	tstartTime = g_timer.GetStartSec();

	tnumTags = 0;
	tnumFriendlyKills = 0;
	tnumFriendlyHits = 0;
	tnumApprehended = 0;
	tnumHostages = 0;
	ttimer = 0;
	tlastTime	= 0;

	copDead = false;
	tcopDead = false;

	// set visibility status of primary objectives
	for( l = 0 ; l < O_MAX_LEVELS ; l ++ )
	{
		firstPri = true;
		for( o = 0; o < MAX_OBJECTIVES_PER_LEVEL; o++ )
		{	
			// reset the visibility state
			if( objectives[ l ][ o ].getObjectiveClass() == O_PRIMARY )
			{
				objectives[ l ][ o ].setVisible( false );
				objectives[ l ][ o ].forceUpdateMessage = false;
				if( firstPri )
				{
					objectives[ l ][ o ].setVisible( true );
					firstPri =  false;
				}
			}
			else
				objectives[ l ][ o ].visible = true;

			objectives[ l ][ o ].setCurrentObjState( objectives[ l ][ o ].getDefaultObjState() ); 

			// reset count values
			objectives[ l ][ o ].setNumItemsObtained( 0 );

			objectivesSave[ l ][ o ].numItemsObtained = objectives[ l ][ o ].numItemsObtained;
			objectivesSave[ l ][ o ].currentObjState = objectives[ l ][ o ].currentObjState;
			objectivesSave[ l ][ o ].visible = objectives[ l ][ o ].visible;
		}
	}
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::GetFirstPrimaryObjective( int &i )
{
	for( i = 0; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_PRIMARY )
		{
			break;
		}
	}

	currPrimary = i + 1;
//	GetObjectiveString( i , s );
	
	return;
}

//***************************************************************************
//***************************************************************************
bool	SecondaryObjective::GetNextPrimaryObjective( int &i )
{
	for( i = currPrimary; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		// notify end of objectives
		if( i >= MAX_OBJECTIVES_PER_LEVEL )
		{
			return true;
		}	

		// only show visible objectives
		if( !objectives[ currentLevel ][ i ].isVisible() )
		{
			continue;
		}

		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_PRIMARY )
		{
			break;
		}
	}
	// notify end of objectives
	if( i >= MAX_OBJECTIVES_PER_LEVEL )
	{
		return true;
	}	

	currPrimary = i + 1;
	
	return false;
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::GetFirstBonusObjective( int &i )
{
	for( i = 0; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_BONUS )
		{
			break;
		}
	}
	
	currBonus = i + 1;

	if( i >= MAX_OBJECTIVES_PER_LEVEL )
	{
		i = - 1;
	}
	
	return;
}

//***************************************************************************
//***************************************************************************
bool	SecondaryObjective::GetNextBonusObjective( int &i )
{
	for( i = currBonus; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		// notify end of objectives
		if( i >= MAX_OBJECTIVES_PER_LEVEL )
		{
			return true;
		}

		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_BONUS )
		{
			break;
		}
	}
	// notify end of objectives
	if( i >= MAX_OBJECTIVES_PER_LEVEL )
	{
		return true;
	}

	currBonus = i + 1;
	
	return false;
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::GetFirstStatObjective( int &i )
{
	for( i = 0; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_STAT )
		{
			break;
		}
	}

	// couldn't find a stat objective
	if( i==7 )
		i = -1;

	currStat = i + 1;
	
	return;
}

//***************************************************************************
//***************************************************************************
bool	SecondaryObjective::GetNextStatObjective( int &i )
{

	for( i = currStat; i < MAX_OBJECTIVES_PER_LEVEL; i++ )
	{
		// notify end of objectives
		if( i >= MAX_OBJECTIVES_PER_LEVEL )
		{
			return true;
		}

		if( objectives[ currentLevel ][ i ].getObjectiveClass() == O_STAT )
		{
			break;
		}
	}
	// notify end of objectives
	if( i >= MAX_OBJECTIVES_PER_LEVEL )
	{
		return true;
	}

	currStat = i + 1;
	
	return false;
}

bool SecondaryObjective::IsObjectiveFailed( int o )
{
	char os = objectives[ currentLevel ][ o ].getCurrentObjState();
	if( os==O_FAIL )
		return true;

	// timer objectives...
	if( objectives[ currentLevel ][ o ].getOperator()==O_LT || 
		objectives[ currentLevel ][ o ].getOperator()==O_LTE ) // only fail conditions ... right now.
	{
		if( !g_secondaryObjective.IsObjectiveMet( o ) )
		{
			return true;
		}
	}

	return false;
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::updateTimer( float dt )
{
	if( !timerPaused )
		timer += dt;
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::Update( void )
{
	int	o;
	int	objectiveType;
	int	displayType ;
	bool	displayUpdateInfo = false;
	bool	displayCompleteInfo = false;
	bool	displayFailInfo = false;
	bool	forceNoDisplay;
	bool	forceUpdateMessage;
	float	n = 0;
	float	l = 0;
	char	fs[ 100 ];
	char	cs[ 100 ];
	char	us[ 100 ];
	char	s[ 100 ];
	float	dt;
	float nowTime;


	if ( currentLevel >= NUM_LEVELS )
		return;

	// timer update stuff...
	nowTime = g_timer.GetStartSec();
	dt = nowTime - lastTime;
	lastTime = nowTime;
	updateTimer( dt );

	fs[ 0 ] = 0;
	cs[ 0 ] = 0;
	us[ 0 ] = 0;

	for( o = 0; o < MAX_OBJECTIVES_PER_LEVEL; o++ )
	{
		objectiveType = objectives[ currentLevel ][ o ].getObjectiveType();
		displayType   = objectives[ currentLevel ][ o ].getDisplayType();
		forceUpdateMessage   = objectives[ currentLevel ][ o ].forceUpdateMessage;
		forceNoDisplay = false; 
		displayUpdateInfo = false;
		displayCompleteInfo = false;
		displayFailInfo = false;

		// nothing here, move along.
		if( objectiveType == O_NO_TYPE )
		{
			continue;
		}

		char os = objectives[ currentLevel ][ o ].getCurrentObjState();
		if( ( os==O_FAIL || os==O_COMPLETE ) && 
			!( (objectives[ currentLevel ][ o ].flags&FORCE_FAIL_MESSAGE)==FORCE_FAIL_MESSAGE || 
			  (objectives[ currentLevel ][ o ].flags&FORCE_COMPLETE_MESSAGE)==FORCE_COMPLETE_MESSAGE ) )
		{
			continue;
		}

		// see if we need to show the player some objective info
		if( g_secondaryObjective.IsObjectiveMet( o ) && 
			objectives[ currentLevel ][ o ].getOperator()!=O_LT && 
			objectives[ currentLevel ][ o ].getOperator()!=O_LTE)
		{
			displayCompleteInfo = true;
			g_secondaryObjective.GetObjectiveString( o , cs );
			strcat( cs , " Completed" );
		}

		if( g_secondaryObjective.IsObjectiveFailed( o ) )
		{
			displayFailInfo = true;
			g_secondaryObjective.GetObjectiveString( o , fs );
			strcat( fs , " Failed" );
		}

		switch( objectiveType )
		{
			case O_PRI1:
			case O_PRI2:
			case O_PRI3:
			case O_PRI4:
				if( displayCompleteInfo )
				{
					int j;
					int nid = objectives[ currentLevel ][ o ].getNextObjectiveID();

					for( j=0 ; j<MAX_OBJECTIVES_PER_LEVEL ; j++ )
					{
						if( nid == objectives[ currentLevel ][ j ].getID() )
						{
							break;
						}
					}
					objectives[ currentLevel ][ j ].setVisible( true );
				}
			break;
			
			case O_LOOT:
				if( objectives[ currentLevel ][ o ].getObjectiveClass() == O_PRIMARY && displayCompleteInfo )
				{
					int j;
					int nid = objectives[ currentLevel ][ o ].getNextObjectiveID();

					for( j=0 ; j<MAX_OBJECTIVES_PER_LEVEL ; j++ )
					{
						if( nid == objectives[ currentLevel ][ j ].getID() )
						{
							break;
						}
					}
					objectives[ currentLevel ][ j ].setVisible( true );
				}
			break;

			case O_BONUS1:
				if( displayType == O_INT )
				{
					n = 0; // not implemented yet
				}
				else
				{

				}
			break;

			case O_STONER:
				n = objectives[ currentLevel ][ o ].getNumItemsObtained();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
			break;

			case O_HEADSHOTS:
				n = (float)g_secondaryObjective.GetNumHeadshots();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
			break;

			case O_KILLS:
				n = (float)g_secondaryObjective.GetNumKills();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
			break;

			case O_APPREHEND:
				n = (float)g_secondaryObjective.GetNumApprehended();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
			break;

			case O_TAGS:
				n = (float)g_secondaryObjective.GetNumTags();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
			break;

			case O_ACCURACY:
				if( g_secondaryObjective.GetShotsFired()>0 )
				{
					n = (float)((int)(( 100*g_secondaryObjective.GetNumHits() )/g_secondaryObjective.GetShotsFired()));
					l = objectives[ currentLevel ][ o ].getNumItemsObtained();
					objectives[ currentLevel ][ o ].setNumItemsObtained( n );
				}
				else
				{
					objectives[ currentLevel ][ o ].setNumItemsObtained( 0 );
				}
				forceNoDisplay = true;
 			break;

			case O_GAMETIME:
				n = g_secondaryObjective.GetTimePlayed();
				l = objectives[ currentLevel ][ o ].getNumItemsObtained();
				objectives[ currentLevel ][ o ].setNumItemsObtained( n );
				forceNoDisplay = true;
			break;
		}
		
		if( os==O_WAIT )
		{
			continue;
		}
	
		// show info if something has changed
		char *ds = NULL;
		bool doDisplay = false;

		if( l!=n || forceUpdateMessage )
		{
			displayUpdateInfo = true;
			objectives[ currentLevel ][ o ].forceUpdateMessage = false;
		}

		if( displayCompleteInfo )
		{
			ds = cs;
			objectives[ currentLevel ][ o ].setCurrentObjState( O_COMPLETE ); // stop showing messages for this objective
			if( strcmp( "" , objectives[ currentLevel ][ o ].getUnlockable() ) ) 
				GameLocks::Unlock( objectives[ currentLevel ][ o ].getUnlockable() );
			if( (objectives[ currentLevel ][ o ].getFixedFlags()&FF_NO_COMPLETE_MSG)!=FF_NO_COMPLETE_MSG )
				doDisplay = true;

			if( objectives[ currentLevel ][ o ].getNextObjectiveID()==-1 && 
				 objectives[ currentLevel ][ o ].getObjectiveClass()==O_PRIMARY &&
				 finished )
			{
				CActor *actor = g_stage.MasterCast().FindActorByInstanceName( "dispatcher" );
	
				if( actor )
					g_messageDispatcher.SendMessageToActor( "fin" , NULL , INVALID_OBJECT_HANDLE , actor->GetHandle() );			
			}
		}
		else if( displayFailInfo )
		{
			ds = fs;
			objectives[ currentLevel ][ o ].setCurrentObjState( O_FAIL ); // stop showing messages for this objective
			doDisplay = true;
		}
		else if( displayUpdateInfo )
		{
			g_secondaryObjective.GetObjectiveString( o , us );
			sprintf( s , " ( %d of %d )" , (int)objectives[ currentLevel ][ o ].getNumItemsObtained() , (int)objectives[ currentLevel ][ o ].getNumItems() );
			strcat( us , s );
			ds = us;
			if( os!=O_NO_UPDATE )
			{
				doDisplay = true;
			}
		}

		if( doDisplay )
		{
			GameHUD_ShowMessage msg = { ds, 4.0f, false, false };
			GameHUD::SendMessage( GAMEHUD_SHOWMESSAGE, &msg );
		}
		
		// clear flags
		objectives[ currentLevel ][ o ].flags = 0;
	}
}

//***************************************************************************
//***************************************************************************
void	SecondaryObjective::End( void ) // THIS NEEDS TO BE CLEANED UP AFTER BUILD
{
	int	o;
	int	objectiveType;

	if ( currentLevel >= NUM_LEVELS )
		return;

	for( o = 0; o < MAX_OBJECTIVES_PER_LEVEL; o++ )
	{
		objectiveType = objectives[ currentLevel ][ o ].getObjectiveType();

		// nothing here, move along.
		if( objectiveType == O_NO_TYPE )
		{
			continue;
		}

		// goofy objectives that are completed by just finishing the level.
		if( ( objectiveType == O_MARK_AT_END || objectiveType==O_DEATH ) && objectives[ currentLevel ][ o ].getCurrentObjState()!=O_FAIL )
		{
			objectives[ currentLevel ][ o ].setCurrentObjState( O_COMPLETE ); 
			if( strcmp( "" , objectives[ currentLevel ][ o ].getUnlockable() ) )
         	GameLocks::Unlock( objectives[ currentLevel ][ o ].getUnlockable() );
		}

		// check to see if culdesack partner is still alive.
		//if( objectiveType == O_BUDDYDEAD && objectives[ currentLevel ][ o ].getNumItemsObtained()==0 )
		//{
		//	objectives[ currentLevel ][ o ].setCurrentObjState( O_COMPLETE ); 
		//	GameLocks::Unlock( objectives[ currentLevel ][ o ].getUnlockable() );
		//}

		// check to see if lazarus is still alive.
		//if( objectiveType == O_LAZARUSDEAD && objectives[ currentLevel ][ o ].getNumItemsObtained()==0 )
		//{
		//	objectives[ currentLevel ][ o ].setCurrentObjState( O_COMPLETE ); 
		//	GameLocks::Unlock( objectives[ currentLevel ][ o ].getUnlockable() );
		//}

		// have we completed an objective?
		if( g_secondaryObjective.IsObjectiveMet( o ) )
      {
			objectives[ currentLevel ][ o ].setCurrentObjState( O_COMPLETE ); 
			if( strcmp( "" , objectives[ currentLevel ][ o ].getUnlockable() ) )
				GameLocks::Unlock( objectives[ currentLevel ][ o ].getUnlockable() );
		}
	}
}


//***************************************************************************
//***************************************************************************
void	SecondaryObjective::Init( void )
{
	int	l;
	int	o;
	bool	firstPri;

	for( l = 0 ; l < O_MAX_LEVELS ; l ++ )
	{
		firstPri = true;
		for( o = 0; o < MAX_OBJECTIVES_PER_LEVEL; o++ )
		{	
			// reset the visible state
			if( objectives[ l ][ o ].getObjectiveClass() == O_PRIMARY )
			{
				objectives[ l ][ o ].setVisible( false );
				objectives[ l ][ o ].forceUpdateMessage = false;
				if( firstPri )
				{
					objectives[ l ][ o ].setVisible( true );
					firstPri =  false;
				}
			}
		}
	}

	for( l = 0 ; l < O_MAX_LEVELS ; l ++ )
	{
		for( o = 0; o < MAX_OBJECTIVES_PER_LEVEL; o++ )
		{	
			objectives[ l ][ o ].setCurrentObjState( objectives[ l ][ o ].getDefaultObjState() ); 
			if( strcmp( "" , objectives[ l ][ o ].getUnlockable() ) )
			{
				if( GameLocks::IsUnlocked( objectives[ l ][ o ].getUnlockable() ) )
				{
					objectives[ l ][ o ].setCurrentObjState( O_COMPLETE ); 
					if( o<=6 )
						if( objectives[ l ][ o + 1 ].getObjectiveClass() == O_PRIMARY )
							objectives[ l ][ o + 1 ].setVisible( true );
				}
			}

			// reset count values
			objectives[ l ][ o ].setNumItemsObtained( 0 );

			objectivesSave[ l ][ o ].numItemsObtained = objectives[ l ][ o ].numItemsObtained;
			objectivesSave[ l ][ o ].currentObjState = objectives[ l ][ o ].currentObjState;
			objectivesSave[ l ][ o ].visible = objectives[ l ][ o ].visible;
		}
	}

}

//***************************************************************************
//***************************************************************************
char	SecondaryObjective::GetObjState( int o )
{
	return objectives[ currentLevel ][ o ].getCurrentObjState();
}


//***************************************************************************
//***************************************************************************
char	SecondaryObjective::IsObjVisible( int o )
{
	return objectives[ currentLevel ][ o ].isVisible();
}

//***************************************************************************
//***************************************************************************
char *worldNameToIndex[ O_MAX_LEVELS ] = {
"sp_warehouse",
"sp_apartments",
"sp_downtown",
"sp_culdesac",
"sp_club",
"sp_subway",
"sp_tijuana",
"sp_casino",
"sp_penthouse",
"sp_prison",
"sp_mall",
"sp_hacienda",
"wavebattle"
};

void	SecondaryObjective::SetLevel( const char *worldName )
{
	int i;
	for( i=0 ; i<O_MAX_LEVELS ; i++ )
	{
		if( !stricmp( (char *)worldName , worldNameToIndex[ i ] ) )
		{
			break;
		}
	}
	currentLevel = i;
}

//***************************************************************************
//***************************************************************************
int SecondaryObjective::GetObjectiveByType( int t )
{
	int	o;

	for( o=0 ; o<MAX_OBJECTIVES_PER_LEVEL ; o++ )
	{
		if( objectives[ currentLevel ][ o ].getObjectiveType() == t )
		{
			break;
		}
	}

	if( o<MAX_OBJECTIVES_PER_LEVEL )
		return o;

	return -1;
}

/******************************************************************************/
/******************************************************************************/
void SecondaryObjective::CSetCheckpoint::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int i;
	int j;

	TRACE( "Death Obj Save\n" );

	d_ref->tnumKills = d_ref->numKills;
	d_ref->tshotsFired = d_ref->shotsFired;
	d_ref->tnumTimesHit = d_ref->numTimesHit;
	d_ref->tnumHits = d_ref->numHits;
	d_ref->tnumHeadshots = d_ref->numHeadshots;
	d_ref->ttimer = d_ref->timer;
	d_ref->tlastTime = d_ref->lastTime;

	d_ref->tnumTags = d_ref->numTags;
	d_ref->tnumFriendlyKills = d_ref->numFriendlyKills;
	d_ref->tnumFriendlyHits = d_ref->numFriendlyHits;
	d_ref->tnumApprehended = d_ref->numApprehended;
	d_ref->tnumHostages = d_ref->numHostages;

	d_ref->tcopDead = d_ref->copDead;

	for( i=0 ; i<NUM_LEVELS ; i++ )
	{
		for( j=0; j<MAX_OBJECTIVES_PER_LEVEL ; j++ )
		{
			objectivesSave[ i ][ j ].numItemsObtained = objectives[ i ][ j ].numItemsObtained;
			objectivesSave[ i ][ j ].currentObjState = objectives[ i ][ j ].currentObjState;
			objectivesSave[ i ][ j ].visible = objectives[ i ][ j ].visible;
		}
	}
}

void SecondaryObjective::CSpawnAvatar::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	g_messageDispatcher.SendMessageToAll( "UnpauseObjTimer" , NULL , INVALID_OBJECT_HANDLE );
}

/******************************************************************************/
/******************************************************************************/
void SecondaryObjective::CDeath::HandleMessage( void* i_data , ActorHandle i_sender , bool i_posted )
{
	int i;
	int j;
	int o;

	if ( g_players.GetPlayer( 0 ) != CActor::FromHandle( i_sender ) )
	{
		return;
	}

	TRACE( "Death Obj Reset\n" );

	o = g_secondaryObjective.GetObjectiveByType( O_DEATH );
	if( o!=-1 )
	{
		if( objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_COMPLETE &&	
			 objectives[ g_secondaryObjective.GetLevel() ][ o ].getCurrentObjState() != O_FAIL )	
		{
			objectives[ g_secondaryObjective.GetLevel() ][ o ].setCurrentObjState( O_FAIL );
			objectives[ g_secondaryObjective.GetLevel() ][ o ].flags |= FORCE_FAIL_MESSAGE;
		}
	}

	g_messageDispatcher.SendMessageToAll( "PauseObjTimer" , NULL , INVALID_OBJECT_HANDLE );

	d_ref->numKills = d_ref->tnumKills;
	d_ref->shotsFired = d_ref->tshotsFired;
	d_ref->numTimesHit = d_ref->tnumTimesHit;
	d_ref->numHits = d_ref->tnumHits;
	d_ref->numHeadshots = d_ref->tnumHeadshots;
	d_ref->timer = d_ref->ttimer;
	d_ref->lastTime = d_ref->lastTime;

//	tstartTime = tstartTime;

	//d_ref->numTags = d_ref->tnumTags;
	d_ref->numFriendlyKills = d_ref->tnumFriendlyKills;
	d_ref->numFriendlyHits = d_ref->tnumFriendlyHits;
	d_ref->numApprehended = d_ref->tnumApprehended;
	d_ref->numHostages = d_ref->tnumHostages;

	d_ref->copDead = d_ref->tcopDead;

	for( i=0 ; i<NUM_LEVELS ; i++ )
	{
		for( j=0; j<MAX_OBJECTIVES_PER_LEVEL ; j++ )
		{
			// a general 'don't reset'
			if( ( objectives[ i ][ j ].fixedFlags & FF_DONT_RESET ) == FF_DONT_RESET ) 
			{
				continue;
			}

			// don't reset tags.
			if( objectives[ i ][ j ].getObjectiveType()==O_TAGS /*&& objectives[ i ][ j ].currentObjState!=O_COMPLETE*/ ) 
			{
				continue;
			}

			// don't reset tags.
			if( objectives[ i ][ j ].getObjectiveType()==O_CASH /*&& objectives[ i ][ j ].currentObjState!=O_COMPLETE*/ ) 
			{
				continue;
			}

			// don't reset tags.
			if( objectives[ i ][ j ].getObjectiveType()==O_DEATH /*&& objectives[ i ][ j ].currentObjState!=O_COMPLETE*/ ) 
			{
				continue;
			}

			objectives[ i ][ j ].numItemsObtained = objectivesSave[ i ][ j ].numItemsObtained;
			objectives[ i ][ j ].currentObjState = objectivesSave[ i ][ j ].currentObjState;
			objectives[ i ][ j ].visible = objectivesSave[ i ][ j ].visible;
		}
	}
}


