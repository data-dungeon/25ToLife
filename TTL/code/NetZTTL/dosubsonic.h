#pragma once

#ifndef DO_SUBSONIC_H
#define DO_SUBSONIC_H

#include <netz.h>
#include "NetZTTL/DOSubsonicDDL.h"
#include "GameHelper/Callback.h"

class DOSubsonic : public DOCLASS(DOSubsonic)
{
public:
	DOSubsonic();
	~DOSubsonic();

protected:
	virtual CActor* CreateProxy(const char* i_class);
	void InitMaster(void);
	void InitProxy(void);
	void PropagateMaster(float i_gameTime);
	void Terminate(void);

	CCallback_1<DSSixDOF, Vector3CRef> d_callback;
};

#endif
