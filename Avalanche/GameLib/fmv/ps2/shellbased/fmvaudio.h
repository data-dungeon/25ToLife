//-----------------------------------------------------------------------------
//                               FMV Player
//
// $Workfile: fmvaudio.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

void FMVAudioInit( void );
void FMVAudioCreate( EShellMpegAudioType type, ShellMpegADSHeader *sshd );
u_int FMVAudioSend( u_char *b, u_int bs, u_int d, u_int ds, int param );
int FMVAudioIsStartable( void );
void FMVAudioStart( void );
void FMVAudioDestroy( void );
