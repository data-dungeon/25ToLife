//////////////////////////////////////////////////////
// Attract Mode - attractmode.h
//
// Paul Blagay  3/30/04
// Avalanche Software 2004
//////////////////////////////////////////////////////

#ifndef _ATTRACTMODE_H_
#define _ATTRACTMODE_H_


class AttractMode
{
public:
	static	void	Init( t_Handle worldHandle, CameraScene* camScene);
	static	void	DeInit();
	static	void	Start(bool Fade, float InteruptTime, float Timeout);		// Fade On/Off, NoSkipTime in seconds, Timeout in seconds
	static	void	Stop();
	static	void	Update( float i_deltasec );
	static	bool  IsActive();
};


#endif // _ATTRACTMODE_H_
