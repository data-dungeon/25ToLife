/**********************************************/
/*                                            */
/* Plant.h                                    */
/* big juju   3/26/02                         */
/* ned martin  avalanche software             */
/* originally by bryant collard,              */
/* i just fit it into derived CSimple scheme  */
/* visual c++ 6.0                             */
/* plants                                     */
/*                                            */
/**********************************************/

/* plants are currently derived from CSimple-- so all they can do is sit there and move, and maybe react to protagonists */

#ifndef __PLANT_H
#define __PLANT_H

class TupperwareAggregate;	// in Tupperware/Aggregate.h
class TupConfigRoot;			// in TupperAttrib/ConfigRoot.h
class CPlantParam;			// in TTL/Environ/PlantParam.h

/******************* defines *****************************/

/* temporary, for speed tests */

#define NO_OBB_CHECKS

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

class CPlant : public CProtagonist
{
public:

	/* Constructor/destructor. xml node provides name of instance/model for actor to use */

	CPlant();
	~CPlant();

	// Class
	static const CEntityType &ClassType(void) {return(s_plantClass);}

	/* initialize */

	virtual bool Initialize(void);
	virtual void Update(float i_deltaSec) {}
	void SetParams();

	void SetHidden(bool i_hidden) {d_hidden = i_hidden;}
	void SetScale(float i_scale) {d_scale = i_scale;}
	float GetScale(void) {return(d_scale);}
	CProtagonist* GetLastTouched(void) {return(d_touched);}

	virtual void Draw(CCollisionEvent* i_event);
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);

public:

	/* simple routines */

	/* routines defined in .cpp */

protected:

	/* private stuff */

public:

	/********************** CActor overrides/implementations *******************************************/

	virtual void LoadDynamicLinkageParams(void);

	/********************** CProtagonist overrides/implementations *********************************/

	// no BeginFrame-- use parent's
	// no BeginUpdate-- use parent's
	// no AttemptUpdate-- use parent's
	// no EndUpdate-- use parent's
	// no EndFrame-- use parent's

// Implementation-- data

protected:

	CPlantParam *d_pParam;
	DirCos3x3 d_obbToWorld;
	ts_Bone* d_boxParent;
	bool d_hidden;
	float d_scale;
	bool d_disabled;
	float d_enableTime;
	CProtagonist* d_touched;
	float d_sfxTimer;
	bool d_hasTrunk;
	bool d_bPlantCheck;

#ifdef NO_OBB_CHECKS
	/* support for turning off collisions */
	
	virtual void RunAnimation(void);	// override CActor::RunAnimation
	virtual void ScaleActor(void);
	bool d_firstFrame;
#endif


// operations

public:

private:
	// Class
	static const CEntityType s_plantClass;
};

extern float g_featherTimer;

#endif // __PLANT_H
