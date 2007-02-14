//============================================================================
//== TTLConstants.h -
//==   TTL constant defines that are also used in non-C files, e.g. NetZ .ddl
//==   files or scripts.
//============================================================================

#ifndef _TTL_CONSTANTS
#define _TTL_CONSTANTS

// Participant dimensions
#define MAX_PLAYERS				16
#define MAX_PARTICIPANTS		56

// Reserved participant IDs.
#define INVALID_PARTICIPANT	0x7F
#define PARTICIPANT_DENIED		INVALID_PARTICIPANT - 1
#define ILLEGAL_PARTICIPANT	INVALID_PARTICIPANT - 2
#define SPAWN_TICKET_MASK     0x80

#define MAX_TEAMS             8
#define INVALID_TEAM          0xFF

#define ACCOUNT_ID_SIZE       16	// actually only 12 bytes, but the the compiler rounds sizeof(OSIAccountID) up to 16
#define ROUND_ID_SIZE         8
#define TAG_PARAMS_SIZE       32 // 20 -> 16 byte aligned

#define NUM_REFEREE_PHASES    6

#endif //_TTL_CONSTANTS

//============================================================================
