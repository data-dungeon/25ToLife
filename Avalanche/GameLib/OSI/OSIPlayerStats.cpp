#include "OSIPlayerStats.hpp"

const char * const OSIPlayerStats::personalStats[ OSIPlayerStats::NUM_PLAYERSTATS ] =  { 
	"stats_rank", "stats_copWins", "stats_copLosses", "stats_copTies", "stats_gangWins", "stats_gangLosses",
    "stats_gangTies", "stats_copKills", "stats_gangKills", "stats_copDeaths", "stats_gangDeaths", "stats_copSuicides",
    "stats_gangSuicides", "stats_copHeadshots", "stats_gangHeadshots", "stats_copTeamKills", "stats_gangTeamKills",
    "stats_copTeamDeaths", "stats_gangTeamDeaths", "stats_copNPCKills", "stats_gangNPCKills", "stats_apprehensions",
    "stats_apprehended", "stats_copShotsFired", "stats_gangShotsFired", "stats_copShotsHit", "stats_gangShotsHit",
    "stats_copTimePlayed", "stats_gangTimePlayed", "stats_lootScores", "stats_raidScores", "stats_tagScores",
    "stats_copMelee", "stats_copGrenade", "stats_copSpecial", "stats_copSubMachinegun", "stats_copHeavyMachinegun",
    "stats_copShotgun", "stats_gangMelee", "stats_gangGrenade", "stats_gangSpecial", "stats_gangSubMachinegun",
    "stats_gangHeavyMachinegun", "stats_gangShotgun", "stats_copDeathHoldingMelee", "stats_copDeathHoldingGrenade",
    "stats_copDeathHoldingSpecial", "stats_copDeathHoldingSubMachinegun", "stats_copDeathHoldingHeavyMachinegun",
    "stats_copDeathHoldingShotgun", "stats_gangDeathHoldingMelee", "stats_gangDeathHoldingGrenade",
    "stats_gangDeathHoldingSpecial", "stats_gangDeathHoldingSubMachinegun", "stats_gangDeathHoldingHeavyMachinegun",
    "stats_gangDeathHoldingShotgun", "stats_copDeathByMelee", "stats_copDeathByGrenade", "stats_copDeathBySpecial",
    "stats_copDeathBySubMachinegun", "stats_copDeathByHeavyMachinegun", "stats_copDeathByShotgun",
    "stats_gangDeathByMelee", "stats_gangDeathByGrenade", "stats_gangDeathBySpecial", "stats_gangDeathBySubMachinegun",
    "stats_gangDeathByHeavyMachinegun", "stats_gangDeathByShotgun", "stats_tagWins", "stats_tagGamesPlayed",
    "stats_tagTime", "stats_warWins", "stats_warGamesPlayed", "stats_warTime", "stats_robberyWins",
    "stats_robberyGamesPlayed", "stats_robberyTime", "stats_raidWins", "stats_raidGamesPlayed", "stats_raidTime",
    "stats_mode5Wins", "stats_mode5GamesPlayed", "stats_mode5Time", "stats_mode6Wins", "stats_mode6GamesPlayed",
	"stats_mode6Time", "stats_incompletes" 
};


///////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Set correct initial values
///////////////////////////////////////////////////////////////////////////////////////
OSIPlayerStats::OSIPlayerStats()
{
	clearStats();
}


