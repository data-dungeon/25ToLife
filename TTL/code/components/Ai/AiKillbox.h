//============================================================================
//=
//= AiKillBox.h -
//=    This actor class watches for entry/exit of any other actor from its
//=    collision box, and then transmits those events to the handling
//=    script.
//=
//============================================================================

#ifndef _AIKILLBOX_H
#define _AIKILLBOX_H

//============================================================================

class AiKillBox : public CProtagonist
{
public:
	AiKillBox();
	~AiKillBox();
	virtual bool Initialize();
	virtual void BeginFrame();
	virtual void RunAnimation( bool forceUpdate );
private:
};

//============================================================================

#endif //_AIKILLBOX_H
