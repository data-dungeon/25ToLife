/////////////////////////////////////////////////////////////////////////////
// Controller focus manager
/////////////////////////////////////////////////////////////////////////////
#ifndef CFOCUS_H
#define CFOCUS_H

#include "platform/systemi.h"

#include "container/dllist.h"
#include "container/sstack.h"

class ControlMapper;

class ControllerFocusManager
{
public:
	// Initialize/terminate the mapper
	ControllerFocusManager();
	~ControllerFocusManager();

	// Add/remove a control mapper (we manage the memory for you after this)
	void Add(ControlMapper *mapper, int player = 0);
	void Remove(ControlMapper *mapper, int player = 0);

	// Find the mapper
	ControlMapper *Find(const char *name, int player = 0);

	// Admit it, we all like to be lazy
	void Remove(const char *name, int player = 0)
		{ Remove(Find(name, player), player); }
	void Push(const char *name, int player = 0)
		{ Push(Find(name, player), player); }
	void Pop(const char *name, int player = 0)
		{ Pop(Find(name, player), player); }

	// Push the mapper
	void Push(ControlMapper *mapper, int player = 0);

	// Pop the current mapper
	ControlMapper *Pop(ControlMapper *mapper, int player = 0);

	// Get the current mapper without popping
	ControlMapper *Current(int player = 0);

	// is this mapper already on the stack?
	bool OnStack(ControlMapper *mapper, int player = 0);
	bool OnTop(ControlMapper *mapper, int player = 0);

	// Call this once per frame
	void Update();

	// Clear the stacks
	void ClearStack(int player = 0);
	void ClearStacks();

private:
	// List of mappers (one per player)
	DoublyLinkedList<ControlMapper>     m_list[MAX_CONTROLLERS];

	// The stack of mappers (one per player)
	SimpleStack<ControlMapper *, 8> 		m_stack[MAX_CONTROLLERS];

	void Debug(int player, const char *operation, ControlMapper *mapper);
};

// Our singleton
extern ControllerFocusManager g_controlFocus;

#endif
