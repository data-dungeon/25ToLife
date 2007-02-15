//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _highlightShader
#define _highlightShader

class highlightShader : public MPxCommand {
public:
	highlightShader ();
	~highlightShader (); 

	MStatus	doIt( const MArgList& args );
	MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

	void		getSelectionShadingGroups(MStringArray &selection, MStringArray &SG);
	MStatus		highlightMultilister(MStringArray &SG);
};

#endif