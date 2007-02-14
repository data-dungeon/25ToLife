/***************************************************************************/
// The #defines for weapons for use in the weapon library and scripts
/***************************************************************************/

#ifndef VOICEOVERDEFS_H
#define VOICEOVERDEFS_H

/*** NOTHING BUT DEFINES! ***/

#define VOICEOVER_INTERRUPT					0x000000001
#define VOICEOVER_QUEUE							0x000000002
#define VOICEOVER_PLAY_IF_NOT_BUSY			0x000000004
#define VOICEOVER_ONESHOT_GAME				0x000000008
#define VOICEOVER_ONESHOT_LEVEL				0x000000010
#define VOICEOVER_ONESHOT_LOAD				0x000000020
#define VOICEOVER_MULTISHOT_GAME				0x000000040
#define VOICEOVER_MULTISHOT_LEVEL			0x000000080
#define VOICEOVER_MULTISHOT_LOAD				0x000000100
#define VOICEOVER_INTERRUPTABLE				0x000000200
#define VOICEOVER_WET							0x000000400
#define VOICEOVER_NO_MUSIC_VOLUME_CHANGE	0x000000800

#define VOICEOVER_TRACKABLE_BITS				( VOICEOVER_ONESHOT_GAME    | \
														  VOICEOVER_ONESHOT_LEVEL   | \
														  VOICEOVER_ONESHOT_LOAD    | \
														  VOICEOVER_MULTISHOT_GAME  | \
														  VOICEOVER_MULTISHOT_LEVEL | \
														  VOICEOVER_MULTISHOT_LOAD  )
 
#define VOICEOVER_TYPE_GAME					0x0100
#define VOICEOVER_TYPE_LEVEL					0x0200
#define VOICEOVER_TYPE_LOAD					0x0300

/*** NOTHING BUT DEFINES! ***/

#endif

