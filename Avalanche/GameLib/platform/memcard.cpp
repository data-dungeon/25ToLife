/***************************************************************************/
// Memory Card driver
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"





// Our singleton
MemCard *g_MemCardPtr = NULL;

MemCard::MemCard( void)
{
	m_bInitialized = false;
	m_VersionID = 0xbadbad;
	m_Update = NULL;
}
