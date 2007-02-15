////////////////////////////////////////////////////////////////////////////
//
// ExportInstance
//
// Export Instances into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportInstance.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:29a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/16/03    Time: 2:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * consolidate vertex colors on meshes that use one colorset
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/20/02    Time: 8:44a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Instance specific GCN code for vertex colors
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First pass of instance flags accessing node attribute data
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 2:38p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Vertex Colors working with vertex paint and applyvc
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#ifndef EXPORT_INSTANCE_H
#define EXPORT_INSTANCE_H

class InstanceHolder;
class FilterOptions;
class FilterDataManager;
class TupperwareAggregate;
class TupNodePool;
class TupObjectPool;
class TupConfigRoot;

class ExportInstance
{
#ifdef EXPORT_GCN
	class RGBAColor
	{
	public:
		u8				R;
		u8				G;
		u8				B;
		u8				A;
		
		operator ts_bRGBA() const { ts_bRGBA temp; temp.R = R; temp.G = G; temp.B = B; temp.A = A; return temp; } 
		bool operator==(const RGBAColor &s) { return (R==s.R && G==s.G && B==s.B && A==s.A); }
		
	};
#endif

	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportInstance(void); // this is not defined

public:

	ExportInstance(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot);

	void WriteInstance(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool);
	void WriteInstanceVertexColors(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool);
#ifdef EXPORT_GCN
	void WriteGCNInstanceVertexColors(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool);
	void GetConsolidatedColorPool(Keeper<RGBAColor> &colorPool,TupObjectPool &tupObjectPool,TupTriMesh &tupTriMesh,int colorRef,int alphaRef);
	void GetConsolidatedColorPool(Keeper<u16> &colorPool,TupObjectPool &tupObjectPool,TupTriMesh &tupTriMesh,int colorRef,int alphaRef);

#endif			
	void LinkInstancePasses(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool);
};

class InstanceHolder
{
	u32 m_instanceFilePos;
	u32 m_instanceVertexColorFilePos;
	s32 m_vertexColorInstanceIndex;
	CInstance *m_pInstanceBuffer;
	TupperwareAggregate *m_pNode;

public:

	InstanceHolder(void);
	~InstanceHolder();
	void SetInstanceFilePos(u32 instanceFilePos);
	void SetInstanceVertexColorFilePos(u32 instanceVertexColorFilePos);
	void SetNodeAgg(TupperwareAggregate *pNode);
	void SetVertexColorInstanceIndex(s32 index);

	u32 GetInstanceFilePos(void);
	u32 GetInstanceVertexColorFilePos(void);
	s32 GetVertexColorInstanceIndex(void);
	TupperwareAggregate *GetNodeAgg(void);
	CInstance &GetInstanceBuffer(void);
};

#endif
