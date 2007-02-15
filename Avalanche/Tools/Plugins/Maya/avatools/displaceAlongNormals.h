//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _displaceAlongNormals
#define _displaceAlongNormals

class displaceAlongNormals : public MPxCommand
{
public:
	displaceAlongNormals ();
	~displaceAlongNormals (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
public:
	double m_distance;
};

#endif