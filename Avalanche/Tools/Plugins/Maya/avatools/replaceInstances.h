//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _replaceInstances
#define _replaceInstances

class replaceInstances : public MPxCommand {
public:
	replaceInstances ();
	~replaceInstances (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	bool m_copy,m_removeold,m_uniquenames;
};

#endif