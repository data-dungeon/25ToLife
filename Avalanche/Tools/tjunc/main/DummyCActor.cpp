/*************************************/
/*                                   */
/*   DummyCActor.cpp                 */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   fix-up for dbl viewer           */
/*                                   */
/*************************************/

/* the dbl viewer uses the Game project, which calls CActor::Reset. currently the dbl viewer is very simple, and
separate from the Gamelib projects. including the CActor project means including lots of other Gamelib projects,
so I'm going to just cobble up a dummy class to satisfy the CActor reference at link time */

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include <Game/Animation/Bone.h>
#include <Audio/AudioSys.h>
#include <XML/XML.h>
#include "DummyCActor.h"

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/
Stage *g_stagePtr = NULL;
AudioSystem *g_audioPtr = NULL;
EventDispatcher g_eventDispatcher;

//WindowsApp *WindowsApp::me = NULL;

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

void CActor::AdvanceLightColorMult()
{
}


ts_Bone *CProtagonist::GetOBBChildBone(unsigned int index)
{
	return(NULL);
}
void CProtagonist::UpdateOBBChildList()
{
}


bool OBBChildList::Test(Ray &r, Vector3 &hitPoint, Vector3 &hitNormal, float *d, unsigned int *index)
{
	return(false);
}

void Resource::ReleaseAll( void )
{
}

bool AudioSystem::LoadSoundBankChunk( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	return(true);
}

xmlElement *xmlParseBuffer( unsigned char *buffer, unsigned int length, bool owns_data)
{
	return(NULL);
}
