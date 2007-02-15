//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: timer.h                                                          //
//    Author: Daniel Silver                                                    //
//   Created: 04/05/2001                                                       //
//=============================================================================//

#ifndef __TIMER_H__
#define __TIMER_H__

double Timer( void );
void StartTimer( void );
double EndTimer( void );
double CpuTimer( void );
__int64 CpuTimeStamp( void );

#endif __TIMER_H__