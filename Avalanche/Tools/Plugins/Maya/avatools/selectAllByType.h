//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		novc - select all that don't have vertex colors
//		vc - select all that have have vertex colors
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _selectAllByType
#define _selectAllByType

class selectAllByType : public MPxCommand 
{
protected:
	enum SelectionTypes
	{
		VC							= 1<<0,
		NO_VC						= 2<<0,
	};
	
	int m_selectionFlags;
	
public:
	selectAllByType ();
	~selectAllByType ();
	
	MStatus		doIt( const MArgList& args );
	MStatus     redoIt();
	MStatus     undoIt();
	bool        isUndoable() const;
	
	static void*	creator();
	
private:
	void CheckNode(MDagPath &dagPath,MDagPathArray &dagPathArray);
	bool ValidNodeName(const char *pName);
	bool ValidNode(MDagPath &mDagPath);
	bool ValidMesh(MDagPath &mDagPath);
	bool CheckMesh(MDagPath &mDagPath);
};

#endif