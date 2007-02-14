/***************************************************************************/
// The #defines for weapons for use in the weapon library and scripts
/***************************************************************************/

#ifndef WEAPONDEFS_H
#define WEAPONDEFS_H

/*** NOTHING BUT DEFINES! ***/

#define WEAPON_NONE                     -1

#define WEAPON_PEA								0
#define WEAPON_ENUM_COUNT						1

/* responses to NotifyWeaponHit-- this is a bitfield. used by missactor.cpp */

#define WEAPON_RESPONSE_NO_DAMAGE	0x00000001	// weapon should do no damage
#define WEAPON_RESPONSE_NO_DIE		0x00000002	// weapon should not die as result of collision
#define WEAPON_RESPONSE_NO_COLLIDE	0x00000004	// weapon should pretend there was no collision (e.g. don't bounce)

/* weapon specific defines, shared here so scripts can cause things to happen that match the weapon missile stuff */

/*** NOTHING BUT DEFINES! ***/

#endif

