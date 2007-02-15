#ifndef __UNDO_REDO_STATE_H__
#define __UNDO_REDO_STATE_H__

class UndoRedoState
{
public:
	MSelectionList m_previousSelection;
	MSelectionList m_newSelection;
	bool m_bSelectionChanged;
	MDagPathArray m_updatedNodes;
	MStringArray m_previousNodeAttributes;
	MStringArray m_newNodeAttributes;

	UndoRedoState() : m_bSelectionChanged(false) 
	{
	}
};

#endif	// __UNDO_REDO_STATE_H__