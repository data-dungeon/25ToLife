/*************************************/
/*                                   */
/*   DummyCActor.h              */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   fix-up for dbl viewer           */
/*                                   */
/*************************************/

/* the dbl viewer uses the Game project, which calls CActor::Reset. currently the dbl viewer is very simple, and
separate from the Gamelib projects. including the CActor project means including lots of other Gamelib projects,
so I'm going to just cobble up a dummy class to satisfy the CActor reference at link time */

#ifndef __DUMMYCActor_H
#define __DUMMYCActor_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

/******************* forward declarations ****************/

struct ts_Bone;				// in Game/Animation/Bone.h

/******************* defines *****************************/

/******************* compile control *********************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/
class GameEventHandler
{
	public:
								 GameEventHandler() {}
		virtual				~GameEventHandler() {}


	public:
		virtual void		HandleEvent(unsigned int i_iEvent, void *io_pData, void *pSender)
									{ }
	friend class EventDispatcher;	// provides for better encapsulation.
};

// ===========================================================================
//	CLASS: EventDispatcher

class EventDispatcher
{
	public:
		 EventDispatcher() {}	// object not valid until Initialize()'d.
		~EventDispatcher() {}



	public:
		void				TriggerEvent(unsigned int i_iEvent, void *io_pData, void *pSender = NULL, void *pTo = NULL);
};

//extern EventDispatcher g_eventDispatcher;



class CActor
{
public:
	CActor() {}
	~CActor() {}

	/* to satisfy link */

	void AdvanceLightColorMult();
};

class CProtagonist
{
public:
	CProtagonist() {}
	~CProtagonist() {}

	/* to satisfy link */

	ts_Bone *GetOBBChildBone(unsigned int index);
	void UpdateOBBChildList();
};

class Ray
{
	Ray() {}
	~Ray() {}
};

class OBBChildList
{
	OBBChildList() {}
	~OBBChildList() {}

	/* to satisfy link */

	bool Test(Ray &r, Vector3 &hitPoint, Vector3 &hitNormal, float *d = NULL, unsigned int *index = NULL);

};

class WindowsApp
{
public:
	// The constructor and destructors
	WindowsApp(void) {}
	virtual ~WindowsApp(void) {}

protected:
	// Subclass must set the protected me
	static WindowsApp *me;

};

class Resource
{
	public:
		static void ReleaseAll( void );

};

class Stage
{
public:

private:
};

extern Stage *g_stagePtr;
inline Stage &SafePtr(Stage *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_stage	SafePtr(g_stagePtr)



#endif __DUMMYCActor_H
