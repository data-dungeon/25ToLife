#ifndef __UNDO_REDO_STATE_H__
#define __UNDO_REDO_STATE_H__

class HSnipeObjectList;
class HSnipeObject;

class UndoRedoState
{
public:
	HSnipeObjectList *m_pPreviousSelectedObjects;
	HSnipeObjectList *m_pNewSelectedObjects;
	HSnipeObject *m_pNewFocusObject, *m_pPreviousFocusObject;
	HSnipeObject *m_pNewNearestFocusObject, *m_pPreviousNearestFocusObject;

	bool m_bSelectionChanged;
	TupArray<HSnipeObject *> m_updatedObjects;
	TupArray<TupString> m_previousObjectAttributes;
	TupArray<TupString> m_newObjectAttributes;

	UndoRedoState() : 
		m_bSelectionChanged(false),
		m_pPreviousSelectedObjects(NULL),
		m_pNewSelectedObjects(NULL),
		m_pNewFocusObject(NULL),
		m_pPreviousFocusObject(NULL),
		m_pNewNearestFocusObject(NULL),
		m_pPreviousNearestFocusObject(NULL)
	{
	}

	~UndoRedoState()
	{
		if (m_pPreviousSelectedObjects)
			delete m_pPreviousSelectedObjects;
		if (m_pNewSelectedObjects)
			delete m_pNewSelectedObjects;
	}
};

#endif	// __UNDO_REDO_STATE_H__