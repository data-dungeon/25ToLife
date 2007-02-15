/////////////////////////////////////////////////////////////////////////////
// Screen Space Effect Manger class
//
// Manages a list of screen space effects
/////////////////////////////////////////////////////////////////////////////
#ifndef SSEMGR_H
#define SSEMGR_H

#include "container/dllist.h"

// Forwards
class ScreenSpaceEffect;

/////////////////////////////////////////////////////////////////////////////
// The screen space effect manager
/////////////////////////////////////////////////////////////////////////////
class SSEffectsMgr
{
public:
	SSEffectsMgr(void);
	~SSEffectsMgr(void);

	// Initialize/terminate the system
	static void Initialize();
	static void Terminate();

	// Add an effect to the start of the list
	bool AddHead(ScreenSpaceEffect *effect);

	// Add an effect to the end of the list
	bool AddTail(ScreenSpaceEffect *effect);

	// Remove the effect from the list (doesn't delete it)
	bool Remove(ScreenSpaceEffect *effect);

	// Call all of the updates
 	void Update(CameraScene *scene = NULL);

private:
	// The list of managed effects
	DoublyLinkedList<ScreenSpaceEffect>	list;
};

// There can be only one!
extern SSEffectsMgr *g_ssEffectsPtr;
inline SSEffectsMgr &SafePtr(SSEffectsMgr *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_ssEffects SafePtr(g_ssEffectsPtr)

#endif
