/*************************************/
/*                                   */
/*   TTLPCH_SN.cpp               */
/*   new engine  06/13/02            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   pre-compiled header             */
/*                                   */
/*************************************/

#include "TTLPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */

#include "avatar/avcontrolmap.cpp"
#include "avatar/pstat.cpp"
#include "avatar/pstatMP.cpp"
#include "avatar/pstatSP.cpp"
#include "avatar/taunt.cpp"

#include "camera/shootercam.cpp"

#include "components/Effects.cpp"
#include "components/Exploder.cpp"
#include "components/Participant.cpp"
#include "components/Pickup.cpp"
#include "components/Plant.cpp"
#include "components/PlantParam.cpp"
#include "components/TriggerBox.cpp"
#include "components/KineControl/BipedAnimScriptIF.cpp"
#include "components/KineControl/BipedKineControl.cpp"
#include "components/KineControl/BipedModel.cpp"
#include "components/Ai/AiPerception.cpp"
#include "components/Ai/AiBehavior.cpp"
#include "components/Ai/AiWeaponControl.cpp"
#include "components/Ai/AiPsyche.cpp"
//#include "components/Ai/AiVoice.cpp"
#include "components/Ai/AiBodyControl.cpp"
#include "components/Ai/AiNavigation.cpp"
#include "components/Ai/AiCombatStrategy.cpp"
#include "components/Vision.cpp"

#include "environ/ActorCreator.cpp"
#include "environ/EnvEffects.cpp"
#include "environ/EnvInitMgr.cpp"
#include "environ/glass.cpp"
#include "environ/LevelScript.cpp"
#include "environ/WorldActorLoader.cpp"

#include "fluff/attractmode.cpp"
#include "fluff/sprite3d.cpp"

#include "ActorsTTL/Combatant.cpp"

//#include "main/cardcheck.cpp"
#include "main/LoadBar.cpp"
#include "main/TTL.cpp"
#include "main/TtlScriptFuncs.cpp"
#include "main/GameEnv.cpp"
#include "main/Players.cpp"
#include "main/PlayerDB.cpp"
#include "main/TTLDebug.cpp"
#include "main/TTLLoop.cpp"

//#include "NetZTTL/DOCombatant.cpp"
//#include "NetZTTL/DOCombatantDDL.cpp"
//#include "netzttl/dscombatantpositionddl.cpp"
//#include "NetZTTL/DSCombatantStateDDL.cpp"
//#include "netzttl/dscombatanttransition.cpp"
//#include "netzttl/dscombatanttransitionddl.cpp"
//#include "NetZTTL/TTLDDF.cpp"

#include "sim/GameEntityCategory.cpp"
#include "sim/GameEntityType.cpp"
#include "sim/hud.cpp"
#include "sim/Referee.cpp"
#include "sim/DevLobby.cpp"

