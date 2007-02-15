/////////////////////////////////////////////////////////////////////////////
// Controller focus manager
/////////////////////////////////////////////////////////////////////////////
#include "platform/PlatformPCH.h"

// The global
ControllerFocusManager g_controlFocus;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ControllerFocusManager::ControllerFocusManager()
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ControllerFocusManager::~ControllerFocusManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// Add/remove a control mapper (we manage the memory for you after this)
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::Add(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);
	if (!mapper)
		return;

	// Make sure it doesn't alrady exist
	ASSERT(Find(mapper->Name(), player) == NULL);

	// Aquire a devce for this badboy
	ControllerInputDriver *device = NULL;
	if (g_input.ControllerLocked(player))
		device = g_input.GetController(player);
	mapper->BindDevice(device);
	mapper->ClearFocus();

	// Put it in the list
	m_list[player].AddTail(mapper);

	Debug(player, "Add", mapper);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::Remove(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);
	if (!mapper)
		return;

	Debug(player, "Remove", mapper);

	// Removed this guy from the stack
	while (OnStack(mapper, player))
		Pop(mapper, player);

	// Get rid of it
	m_list[player].Remove(mapper);
}

/////////////////////////////////////////////////////////////////////////////
// Find the mapper
/////////////////////////////////////////////////////////////////////////////
ControlMapper *ControllerFocusManager::Find(
const char *name,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);

	ControlMapper *mapper = m_list[player].Head();
	while (mapper)
	{
		if (strcmpi(name, mapper->Name()) == 0)
			return mapper;

		mapper = mapper->next;
	}

	// Did not find it
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Push the mapper
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::Push(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);
	if (!mapper)
		return;

	// You shouldn't have your mapper on the stack more than once
	ASSERT(!OnStack(mapper, player));

	// Eat pending button presses on focus change
	if (g_input.ControllerLocked(player))
		g_input.GetController(player)->EatButtonPresses();

	// Deactivate the controllers focus
	if (!m_stack[player].Empty())
		m_stack[player].Peek()->ClearFocus();

	// Put this on the stack top and activate
	m_stack[player].Push(mapper);
	mapper->SetFocus();

	Debug(player, "Push", mapper);
}

/////////////////////////////////////////////////////////////////////////////
// Pop the current mapper
/////////////////////////////////////////////////////////////////////////////
ControlMapper *ControllerFocusManager::Pop(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);

	// Nothing to see here
	if (m_stack[player].Empty())
		return NULL;

	// Eat pending button presses on focus change
	if (g_input.ControllerLocked(player))
		g_input.GetController(player)->EatButtonPresses();

	// Pop
	bool onTop = (!m_stack[player].Empty() && m_stack[player].Peek() == mapper);
	if (onTop)
		mapper->ClearFocus();

	// We have to find it
	int count = m_stack[player].Items();
	for (int i = count - 1; i >= 0; i--)
	{
		if (m_stack[player].Peek(i) == mapper)
			m_stack[player].Remove(i);
	}

	// Set focus to the new guy
	if (!m_stack[player].Empty())
		m_stack[player].Peek()->SetFocus();

	Debug(player, "Pop", mapper);

	return onTop ? mapper : NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Get the current mapper without popping
/////////////////////////////////////////////////////////////////////////////
ControlMapper *ControllerFocusManager::Current(
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);

	if (m_stack[player].Empty())
		return NULL;
	else
		return m_stack[player].Peek();
}

/////////////////////////////////////////////////////////////////////////////
// is this mapper already on the stack?
/////////////////////////////////////////////////////////////////////////////
bool ControllerFocusManager::OnStack(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);
	if (!mapper)
		return false;

	int count = m_stack[player].Items();
	for (int i = 0; i < count; i++)
	{
		if (m_stack[player].Peek(i) == mapper)
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool ControllerFocusManager::OnTop(
ControlMapper *mapper,
int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);
	if (!mapper)
		return false;

	if (m_stack[player].Empty())
		return false;

	return (m_stack[player].Peek() == mapper);
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::Update()
{
	// Update all the current mappers
	for (int p = 0; p < MAX_CONTROLLERS; p++)
	{
		ControlMapper *mapper = Current(p);
		if (mapper)
		{
			// We should have focus
			ASSERT(mapper->HasFocus());

			// We better have a device
			if (!mapper->HasDevice() && g_input.ControllerLocked(p))
				mapper->BindDevice(g_input.GetController(p));

			// Update mapper
			mapper->Update();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Clear the stacks
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::ClearStack(int player)
{
	ASSERT(player >= 0 && player < MAX_CONTROLLERS);

	// Anything to do?
	m_list[player].RemoveAll();
	m_stack[player].Clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::ClearStacks()
{
	for (int p = 0; p < MAX_CONTROLLERS; p++)
		ClearStack(p);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ControllerFocusManager::Debug(
int player,
const char *operation,
ControlMapper *mapper)
{
#ifdef _DEBUG
	if (mapper)
		TRACE("ControlMapper %d %s - %s\n", player, operation, mapper->Name());
	else
		TRACE("ControlMapper %d %s\n", player, operation);

	int count = m_stack[player].Items();
	if (count == 0)
		TRACE("Stack empty");
	else
	{
		TRACE ("Stack: \n");
		for (int i = count - 1; i >= 0; i--)
			TRACE("%d - %s\n", i, m_stack[player].Peek(i)->Name());
	}

	TRACE("\n");
#endif
}

