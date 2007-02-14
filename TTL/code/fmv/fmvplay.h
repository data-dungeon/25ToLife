//-----------------------------------------------------------
//
// fmvplay.h
//
// Plays an FMV.. wow, did I really need to explain that!
//
//-----------------------------------------------------------

#ifndef _FMVPLAY_H
#define _FMVPLAY_H

namespace CFmvPlay
{
	void	Init();
	void	Play(char* Filename, bool AllowInterruptions);
	void	DeInit();
};

#endif	// _FMVPLAY_H
