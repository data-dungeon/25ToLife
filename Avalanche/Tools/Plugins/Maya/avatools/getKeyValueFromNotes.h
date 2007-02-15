//////////////////////////////////////////////////////////////////////////////////////
//	getKeyValueFromNotes
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _getKeyValueFromNotes
#define _getKeyValueFromNotes

class getKeyValueFromNotes : public MPxCommand {
public:
	getKeyValueFromNotes ();
	~getKeyValueFromNotes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

protected:
	MString m_objectname;
	MString m_key,m_value;
};

#endif