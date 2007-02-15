/***************************************************************************/
// A cast member is a level of indirection added to an actor so that it can
// be seen in multiple casts
/***************************************************************************/
#ifndef CASTMEMBER_H
#define CASTMEMBER_H

class CActor;
class CCast;

class CCastMember
{
public:
	// Get the actor pointer (will never be null)
	inline CActor* GetActor ()
		{return (d_actor);};

	inline CActor* GetActor () const
		{return (d_actor);};

	// Get the next cast
	inline CCastMember* GetNextCast ()
		{return (d_nextCast);};

	// Get the cast
	inline CCast* GetCast ()
		{return (d_cast);};

	// Get the previous actor
	inline CCastMember* GetPrevActor ()
		{return (d_prevActor);};

	// Get the next actor
	inline CCastMember* GetNextActor ()
		{return (d_nextActor);};

	// Change to a different cast (saves memory alloc/free)
	void ChangeCast(CCast* a_cast, CCastMember* a_prevActor = 0);

	// Change order in cast
	void ChangeOrder(CCastMember* a_prevActor);

	// Call these and die
	CCastMember (CActor* a_actor, CCast* a_cast);
	virtual ~CCastMember ();

private:
	// You can't call these!!!!
	CCastMember ();

	CActor* d_actor;
	CCastMember* d_prevCast;
	CCastMember* d_nextCast;
	CCast* d_cast;
	CCastMember* d_prevActor;
	CCastMember* d_nextActor;

	friend class CCast;
};

#endif
