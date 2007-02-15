//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		1) bool - shadow on/off
//		2) bool - shadow reuse on/off
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _vertexLight
#define _vertexLight

class vertexLight : public MPxCommand
{
public:
	vertexLight ();
	~vertexLight (); 
	
	MStatus		doIt( const MArgList& args );
	MStatus     redoIt();
	MStatus     undoIt();
	bool        isUndoable() const;
	
	static void*	creator();
	
private:
	bool m_shadowmaps,m_reuseshadowmaps;
};

#endif