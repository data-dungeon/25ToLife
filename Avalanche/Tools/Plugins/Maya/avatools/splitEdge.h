//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _splitEdge
#define _splitEdge

class myEdge
{
public:
	myEdge()
	{
		v0 = 0;
		v1 = 0;
		p0 = MPoint(0.0,0.0,0.0);
		p1 = MPoint(0.0,0.0,0.0);
		flag = 0;
	}
	int v0,v1;
	MPoint p0,p1;
	int flag;
};

class splitEdge : public MPxCommand
{
public:
	splitEdge ();
	~splitEdge (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

	int EdgesAreEqual(myEdge a,myEdge b);
	int DoesEdgeExist(myEdge e,std::vector<myEdge> &edges);

	MStatus GetEdges(std::vector<myEdge> &edges,MPointArray &positions);

};

#endif