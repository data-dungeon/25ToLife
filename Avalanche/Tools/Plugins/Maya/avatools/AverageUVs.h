//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _AverageUVs
#define _AverageUVs

typedef std::vector<int> IntArray;


class AverageUVs : public MPxCommand
{
	class PolyInfo
	{
	public:
		TupArray<int> m_uvids;
		TupArray<int> m_vertexIds;
	};

	class SelectedVertexInfo
	{
	public:
		int m_vertexIndex;
		TupArray<int> m_faceIds;
		TupArray<int> m_facePointIds;
	};
	
public:
	AverageUVs ();
	~AverageUVs ();

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif
