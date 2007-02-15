/*
**
**  File:   Stack.h
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   A simple stack for transient objects used in collision testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Stack.h $
**  $Revision: #26 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef STACK_H
#define STACK_H

// Engine includes //
#include "Layers/Assert/Assert.h"

// GameLib includes //
#include "platform/BaseType.h"
#include "EngineHelper/Singleton.h"

// Forward declarations //
class CCollisionStackItem;
class CCollisionStack;

// Debugging //
#ifndef DISABLE_ASSERTS
#define DEBUG_STACK
#endif
#ifndef DEBUG_STACK
#define SET_STACK_NAME(stack, name)
#define SET_LOCK_NAME(lock, name)
#define DUMP_STACK(stack)
#define PLOT_STACK(stack)
#else
#define STACK_NAME_LENGTH  128
#define SET_STACK_NAME(stack, name) (stack.SetName(name))
#define SET_LOCK_NAME(lock, name) (lock.SetName(name))
#define DUMP_STACK(stack) (stack.DumpStack())
#define PLOT_STACK(stack) (stack.PlotStack())
#endif

/**********************************************************************
 *
 *	CLASS CCollisionLock
 *
 *	An object used to "lock" the collision stack at some point.
 *
 **********************************************************************/
class CCollisionStackLock
{
  public:
	CCollisionStackLock();
	~CCollisionStackLock();
	bool Set(CCollisionStack &i_stack);
	void Release(void);
	
  private:
	CCollisionStackItem* d_item;
	CCollisionStackLock* d_prev;
	CCollisionStackLock* d_next;
	uint32 d_end;
	uint32 d_lastItem;

	friend class CCollisionStack;

#ifdef DEBUG_STACK
  public:
	const char* Name(void) {return(d_name);}
	void SetName(const char* i_name);

  private:
	char d_name[STACK_NAME_LENGTH];
#endif
};

/**********************************************************************
 *
 *	CLASS CCollisionStack
 *
 *	A container for memory used for collision detection and resolution
 *	and functions to manage it.
 *
 **********************************************************************/
class CCollisionStack
{
  public:
	CCollisionStack();
	CCollisionStack(uint32 i_size);
	CCollisionStack(byte* i_stack, uint32 i_size);
	~CCollisionStack();
	bool Initialize(uint32 i_size);
	bool Initialize(byte* i_stack, uint32 i_size);
	void Terminate(void);
	void Reset(void);
	CCollisionStackItem* Allocate(uint32 i_size);
	void Deallocate(CCollisionStackItem* i_item);
	void* Top(void);
	void* Expand(uint32 i_size, CCollisionStackItem* i_arrayHeader);
	bool Contract(uint32 i_size, CCollisionStackItem* i_arrayHeader);
	uint32 LockBottom(void);
	void UnlockBottom(uint32 i_key);
	bool IsBottomLocked(void);
	bool IsBottomKey(uint32 i_key);
	void ResetBottom(uint32 i_key);
	void* ExpandBottom(uint32 i_size, uint32 i_key);
	void* ContractBottom(uint32 i_size, uint32 i_key);
	bool Lock(CCollisionStackLock* i_lock);
	void Unlock(CCollisionStackLock* i_lock);
	uint32 TopSize(void);
	uint32 BottomSize(void);
	uint32 UsedSize(void);
	uint32 UnusedSize(void);
	uint32 NumLocks(void);
	bool IsEmpty(void);

  private:
	byte* d_stack;
	bool d_ownsStack;
	uint32 d_size;
	uint32 d_end;
	uint32 d_lastItem;
	uint32 d_bottom;
	uint32 d_bottomKey;
	CCollisionStackLock d_baseLock;
	CCollisionStackLock* d_currentLock;

#ifdef DEBUG_STACK
  public:
	void ValidateStack(void);
	void DumpStack(void);
	uint32 PeakSize(void) {return(d_peakSize);}
	const char* PeakName(void) {return(d_peakName);}
	bool PeakReset(void) {return(d_peakReset);}
	void SetName(const char* i_name);
	void PlotStack(void);

  private:
	void CheckPeak(void);

	uint32 d_firstItem;
	uint32 d_peakSize;
	char d_peakName[STACK_NAME_LENGTH];
	char d_currentName[STACK_NAME_LENGTH];
	bool d_peakReset : 1;
	bool d_currentReset : 1;
	bool d_dumpChanges : 1;
#endif
};

/**********************************************************************
 *
 *	CLASS CPlacementCollisionStack
 *
 *	Template for a stack that uses automatically allocated memory.
 *
 **********************************************************************/
template <int SIZE>
class CPlacementCollisionStack : public CCollisionStack
{
  public:
	CPlacementCollisionStack() : CCollisionStack(d_placementStack, SIZE) {}

  private:
	byte d_placementStack[SIZE];
};

/**********************************************************************
 *
 *	CLASS CCollisionStackItem
 *
 *	Base class for any object to be stored on the collision stack.
 *
 *	The new and delete operators are overridden so memory for all
 *	collision stack items is pulled from the collision stack. While
 *	a stack item may be destoyed by explicitly deleting it or by
 *	going out of scope, it is not guaranteed. When testing for a cluster
 *	is over, the stack is reset by simply setting the stack end pointer
 *	to zero, leaving the existing stack items in limbo. As long as
 *	any dynamic memory allocated by a stack item is also a stack item
 *	(memory comes off the stack), that is OK. So, do not depend on a
 *	stack item's destructor being called and only dynamically allocate
 *	other stack items.
 *
 *	Since normal memory management will be bypassed for these objects,
 *	this base class is declared to force any of the derived classes to
 *	have a size that is a multiple of any required alignment. That way,
 *	any item pulled off the stack will be aligned. Alignment is forced
 *	if using SIMD instructions on the PC and for for VU0 on the PS2.
 *
 **********************************************************************/
class
#ifdef WIN32
#if MMX_OK
__declspec(align(SIMD_ALIGN))
#endif //MMX_OK
#endif //WIN32
CCollisionStackItem
{
  public:
	virtual ~CCollisionStackItem() {}
	static void Delete(CCollisionStackItem* i_item);
	CCollisionStack* GetStack(void) {return(d_stack);}
#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

  protected:
	CCollisionStackItem();
	CCollisionStackItem(CCollisionStack &i_stack);

	CCollisionStack* d_stack;
#ifdef DEBUG_STACK
	uint32 d_itemStart;
	uint32 d_time;
	uint32 d_frame;
#endif

  private:
	uint32 d_previousItem;
	uint32 d_nextItem;
	uint32 d_itemEnd;

	friend class CCollisionStack;
	friend class CCollisionStackLock;
}
#ifdef PS2
__attribute__((aligned (VU_ALIGN)))
#endif //PS2
;

// A global stack
DefineSingleton(CCollisionStack)
#define g_collisionStack GetSingleton(CCollisionStack)

#define STACK_HPP
#include "collide/Stack.hpp"

#endif // STACK_H
