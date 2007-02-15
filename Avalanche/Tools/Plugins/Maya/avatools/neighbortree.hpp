inline NeighborTree::NeighborTree()
{
	m_vertices.clear();
	m_vertexnormals.clear();
	m_uvs.clear();
	m_edges.clear();
	m_polyinfolist.clear();
	m_neighbors.clear();
	m_vtopology.clear();
	m_ptopology.clear();
	m_ulist.clear();
	m_vlist.clear();
	m_visited.clear();
	m_polyindicies.clear();
}

inline int NeighborTree::AddEdgeToList(int v0,int v1,int polyidx)
{
	Edge edge(v0,v1);
	edge.borderedge = 1;
	edge.polyidx = polyidx;

	for (unsigned i = 0; i < m_edges.size(); i++)
	{
		if (edge == m_edges[i]) // check to see if these match up v0=v1 v1=v0
		{
			// found edge is no longer a border and now links to other edge
			m_edges[i].borderedge = 0;
			m_edges[i].adjpolyidx = polyidx;
			// link our edge to this one
			edge.adjpolyidx = m_edges[i].polyidx;
			edge.borderedge = 0;
			Edge *debug = &m_edges[i];
			break;
		}
	}
	m_edges.push_back(edge);

	return m_edges.size()-1;
}

inline void NeighborTree::GetIntOnBothSides(int index,int &indexL,int &indexR,int len,bool edges)
{
	int reversed = 0;

	if ((index-1)<0)
	{
		indexL = len-1;
		reversed = 1;
	}
	else
		indexL = index-1;

	if ((index+1)>=len)
	{
		if (!edges)
		{
			indexR = 0;
			reversed = 1;
		}
		else
			indexR = index;
	}
	else
	{
		if (!edges)
			indexR = index+1;
		else
			indexR = index;
	}
}

inline void NeighborTree::GetIndiciesStartingAt(int index,std::vector<int> indicies, std::vector<int> &result,int flag)
{
	for (unsigned i = index+1; i < indicies.size(); i++)
	{
		result.push_back(indicies[i]);
		int debug = indicies[i];
	}

	for (i = 0; i < (unsigned)index; i++)
	{
		result.push_back(indicies[i]);
		int debug = indicies[i];
	}
	if (flag)
		result.push_back(indicies[i]);
}

inline bool NeighborTree::GenerateNeighborInfo(int selectionIndex)
{
	MStatus status;
	int numVerts;
	std::vector<int> vids,nids,uvids,eids;
	std::vector<int> countarray;
	int vid,nid,uvid;

	MSelectionList selection;
	status = MGlobal::getActiveSelectionList(selection);

	MDagPath dagpath;
	MObject component;

	selection.getDagPath(selectionIndex,dagpath,component);

	MObject shape = dagpath.node();

	if (dagpath.hasFn(MFn::kTransform))
		shape = dagpath.child(0);

	MFnMesh fnMesh(shape);

	MPointArray vertextable;
	MFloatVectorArray normaltable;
	MFloatArray uarray,varray;

	fnMesh.getPoints(vertextable,MSpace::kObject);
//	fnMesh.getNormals(normaltable,space);
	fnMesh.getUVs(uarray,varray);

	m_vertices.resize(vertextable.length());
	for (int i = 0; i < (int)vertextable.length(); i++)
		m_vertices[i].Set(vertextable[i].x,vertextable[i].y,vertextable[i].z);

//	for (i = 0; i < m_normals.length(); i++)
//		m_normals.append(Vector(normaltable[i].x,normaltable[i].y,normaltable[i].z));

	m_uvs.resize(uarray.length());
	for (i = 0; i < (int)uarray.length(); i++)
		m_uvs[i].Set(uarray[i],varray[i]);

	int vtablelen = vertextable.length();

	m_polyindicies.clear();
	m_polyinfolist.clear();

	m_neighbors.resize(vtablelen);	
	countarray.resize(vtablelen);
	m_polyinfolist.resize(fnMesh.numPolygons());

//	Loop through whole geometry to collect some minimal data
	int index;

	IntMap imap;

	
	// go through and get the selected faces
	for ( MItSelectionList faceListIter( selection ); !faceListIter.isDone(); faceListIter.next() )
	{
		MDagPath faceDagPath;
		faceListIter.getDagPath( faceDagPath, component );
		MFn::Type componentType = component.apiType();
		// if pointing to faces or no components
		if (componentType != MFn::kMeshPolygonComponent &&
			componentType != MFn::kInvalid)
			continue;

		MObject faceShape = faceDagPath.node();
		
		if (faceDagPath.hasFn(MFn::kTransform))
			faceShape = faceDagPath.child(0);

		if (faceShape!=shape)
			continue;

		MItMeshPolygon polyIter(faceDagPath,component);

		for (;!polyIter.isDone(); polyIter.next())
		{
			index = polyIter.index();

			imap.SetValue(index,1);

			m_polyindicies.push_back(index);

			numVerts = polyIter.polygonVertexCount(&status);
			if (!status)
				return 0;

			vids.clear();
			nids.clear();
			uvids.clear();
			eids.clear();

			MIntArray meids;
			polyIter.getEdges(meids);

			for (i = 0; i < numVerts; i++)
			{
				vid = polyIter.vertexIndex(i,&status);
				if (status)
				{
					nid = polyIter.normalIndex(i,&status);
					polyIter.getUVIndex(i,uvid);

					vids.push_back(vid);
					nids.push_back(nid);
					uvids.push_back(uvid);
					eids.push_back(meids[i]);
				}	
			}

			PolyInfo *polyinfo = &m_polyinfolist[polyIter.index()];
			polyinfo->m_vids.resize(numVerts);
			polyinfo->m_eids.resize(numVerts);
			polyinfo->m_uvids.resize(numVerts);
			for (i = 0; i < numVerts; i++)
			{
				countarray[vids[i]]++;

	//			Add polyinfo data for edge traversal
				polyinfo->m_vids[i] = vids[i];
				if ((i+1) < (int)vids.size())
					polyinfo->m_eids[i] = AddEdgeToList(vids[i],vids[i+1],polyIter.index());
				else
					polyinfo->m_eids[i] = AddEdgeToList(vids[i],vids[0],polyIter.index());
			}

				
			////////////////////////////////////////////////////////////////////////////////

			int indexL,indexR;
			for (i = 0; i < numVerts; i++)
			{
				GetIntOnBothSides(i,indexL,indexR,numVerts);

				m_neighbors[vids[i]].sv.push_back(vids[indexL]);
				m_neighbors[vids[i]].sv.push_back(vids[indexR]);

				m_neighbors[vids[i]].sf.push_back(polyIter.index());
				m_neighbors[vids[i]].sf.push_back(polyIter.index());

				m_neighbors[vids[i]].uvs.push_back(uvids[i]);
				m_neighbors[vids[i]].uvs.push_back(uvids[i]);
				
				GetIntOnBothSides(i,indexL,indexR,numVerts,true);

				m_neighbors[vids[i]].se.push_back(eids[indexL]);
				m_neighbors[vids[i]].se.push_back(eids[indexR]);

	//			GetIndiciesStartingAt(i,vids,m_neighbors[vids[i]].next);
	//			GetIndiciesStartingAt(i,nids,(m_neighbors[vids[i]].nnids,true);
	//			GetIndiciesStartingAt(i,uvids,m_neighbors[vids[i]].nuvids,true);
	//			GetIndiciesStartingAt(i,eids,m_neighbors[vids[i]].neids,true);
			}

		}
	}

	// go through and do the other faces
	MItMeshPolygon polyIter(dagpath);
	for (;!polyIter.isDone(); polyIter.next())
	{
		index = polyIter.index();

		if (!imap.HasKey(index))
		{
			PolyInfo *poly = &m_polyinfolist[index];

			numVerts = polyIter.polygonVertexCount(&status);
			if (!status)
				continue;

			vids.clear();
			uvids.clear();

			for (i = 0; i < numVerts; i++)
			{
				vid = polyIter.vertexIndex(i,&status);
				if (status)
				{
					polyIter.getUVIndex(i,uvid);
					poly->m_vids.push_back(vid);
					poly->m_uvids.push_back(uvid);
				}	
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// ASSIGN BORDER AND CORNER FLAGS
	////////////////////////////////////////////////////////////////////////////////

	m_cornercount = 0;
	for (i = 0; i < (int)countarray.size(); i++)
	{
		if (countarray[i] == 1)
		{
			m_neighbors[i].corner = 1;
			m_neighbors[i].border = 1;
			m_cornercount++;
		}
		else if (countarray[i] == 2)
			m_neighbors[i].border = 1;
	}

	m_selectedVertexIndex = -1;
	
	// do the vertices that are selected
	for ( MItSelectionList vertListIter( selection ); !vertListIter.isDone(); vertListIter.next() )
	{
		if (m_selectedVertexIndex!=-1)
			break;
		
		MDagPath vertDagPath;
		vertListIter.getDagPath( vertDagPath, component );
		MFn::Type componentType = component.apiType();
		if (componentType != MFn::kMeshVertComponent)
			continue;
		
		MObject vertShape = vertDagPath.node();
		
		if (vertDagPath.hasFn(MFn::kTransform))
			vertShape = vertDagPath.child(0);
		
		if (vertShape!=shape)
			continue;
		
		MItMeshVertex vertexIter(dagpath,component);
		for (;!vertexIter.isDone(); vertexIter.next())
		{
			int vertexIndex = vertexIter.index();
			if (m_neighbors[vertexIndex].border) // if point is on a border edge
			{
				m_selectedVertexIndex = vertexIndex;
				break;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	return true;
}

inline bool NeighborTree::IsSelectionCylindrical()
{
	if (m_cornercount != 4)
		return true;
	return false;
}

inline int NeighborTree::GetFirstCorner()
{
	if (m_selectedVertexIndex!=-1 && m_neighbors[m_selectedVertexIndex].corner)
	{
		return m_selectedVertexIndex;
	}
	for (int i = 0; i < (int)m_neighbors.size(); i++)
	{
		if (m_neighbors[i].corner)
		{
			return i;
		}
	}
	return -1;
}

inline int NeighborTree::GetCornerPoly()
{
	int vid = GetFirstCorner();
	if (vid < 0)
		return -1;
	return m_neighbors[vid].sf.front();
}

inline int NeighborTree::GetFirstBorderEdge()
{
	int i;
	// if we have a selected vertex then grab the edge that is a border and has the selected vertex as v1
	if (m_selectedVertexIndex!=-1) 
	{
		for (i = 0; i < (int)m_edges.size(); i++)
		{
			if (m_edges[i].borderedge && m_edges[i].v1==m_selectedVertexIndex)
				return i;
		}
	}
	// here we just find a border edge
	for (i = 0; i < (int)m_edges.size(); i++)
	{
		if (m_edges[i].borderedge)
			return i;
	}
	return -1;
}

inline int NeighborTree::GetStartEdge()
{
	if (IsSelectionCylindrical())
	{
		int eidx = GetFirstBorderEdge();
		return GetMatchingEdgePlus1(eidx);		
	}
	else if (m_polyindicies.size() == 1) // only one face?
	{
		return GetFirstBorderEdge();
	}
	else
	{
		int eidx;
		int pidx = GetCornerPoly();
		int corneridx = GetFirstCorner();
		for (int i = 0;i<(int)m_polyinfolist[pidx].m_eids.size();i++)
		{
			eidx = m_polyinfolist[pidx].m_eids[i];
			if (m_edges[eidx].v0 == corneridx) // if edge starts with the corner then great
				return eidx;
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdge(int eidx)
{
	// get the poly that this edge belongs to
	int pidx = m_edges[eidx].polyidx;
	// loop through the edges on this poly
	for (int i = 0; i < (int)m_polyinfolist[pidx].m_eids.size();i++)
	{
		if (m_edges[eidx] == m_edges[m_polyinfolist[pidx].m_eids[i]])
		{
			return m_polyinfolist[pidx].m_eids[i];
		}
	}
	return -1;
}

// given a poly and an edge (which may be the owner of the edge or an adjacent neighboor)
// find the edge on the poly that matches the edge
inline int NeighborTree::GetMatchingEdge(int pidx,int eidx)
{
	for (int i = 0; i < (int)m_polyinfolist[pidx].m_eids.size();i++)
	{
		if (m_edges[eidx] == m_edges[m_polyinfolist[pidx].m_eids[i]])
		{
			return m_polyinfolist[pidx].m_eids[i];
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus1(int eidx)
{
	// get the poly that this edge belongs to
	int pidx = m_edges[eidx].polyidx;
	// loop through the edges on this poly
	for (int i = 0; i < (int)m_polyinfolist[pidx].m_eids.size();i++) 
	{
		if (m_edges[eidx] == m_edges[m_polyinfolist[pidx].m_eids[i]]) 
		{
			if ((i+1) >= (int)m_polyinfolist[pidx].m_eids.size())
			{
				return m_polyinfolist[pidx].m_eids[(i+1)-m_polyinfolist[pidx].m_eids.size()];
			}
			return m_polyinfolist[pidx].m_eids[i+1];
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus2(int eidx)
{
	int pidx = m_edges[eidx].polyidx;
	for (int i = 0; i < (int)m_polyinfolist[pidx].m_eids.size();i++) 
	{
		if (m_edges[eidx] == m_edges[m_polyinfolist[pidx].m_eids[i]]) 
		{
			if ((i+2) >= (int)m_polyinfolist[pidx].m_eids.size())
			{
				return m_polyinfolist[pidx].m_eids[(i+2)-m_polyinfolist[pidx].m_eids.size()];
			}
			return m_polyinfolist[pidx].m_eids[i+2];
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus3(int eidx)
{
	int pidx = m_edges[eidx].polyidx;
	for (int i = 0; i < (int)m_polyinfolist[pidx].m_eids.size();i++) 
	{
		if (m_edges[eidx] == m_edges[m_polyinfolist[pidx].m_eids[i]]) 
		{
			if ((i+3) >= (int)m_polyinfolist[pidx].m_eids.size())
			{
				return m_polyinfolist[pidx].m_eids[(i+3)-m_polyinfolist[pidx].m_eids.size()];
			}
			return m_polyinfolist[pidx].m_eids[i+3];
		}
	}
	return -1;
}

inline int NeighborTree::GetNextEdge(int eidx,bool &atborder)
{
	atborder = false;
	int next_eidx = GetMatchingEdgePlus2(eidx);
	if (next_eidx<0)
		return -1;
	if (!m_edges[next_eidx].borderedge) 
	{
		int adjpolyidx = m_edges[next_eidx].adjpolyidx;

		if (HasBeenVisited(adjpolyidx)) 
		{
			atborder = true;
			return next_eidx;
		}

		return GetMatchingEdge(adjpolyidx,next_eidx);
	}
	atborder = true;
	return next_eidx;
}

inline void NeighborTree::InitVisitedLookup() 
{
	m_visited.clear();
	m_visited.resize(m_polyinfolist.size());
}

inline void NeighborTree::SetPolyVisited(int pid)
{
	m_visited[pid] = true;
}

inline int NeighborTree::HasBeenVisited(int pid)
{
	if (m_visited[pid])
		return true;
	return false;
}

inline bool NeighborTree::GenerateTopologyInfo()
{
	bool atborder0 = false,atborder1=false;
	IntArray temp,temp2;
	IntArray polylist;

	InitVisitedLookup();

	int nextedge_x = GetStartEdge();
	if (nextedge_x < 0)
		return false;
	int nextedge_y = GetMatchingEdgePlus3(nextedge_x);
	bool finished = false;
	
	do
	{
		temp2.clear();
		do
		{
			temp.push_back(m_edges[nextedge_x].v0);
			temp2.push_back(m_edges[nextedge_x].v1);
			polylist.push_back(m_edges[nextedge_x].polyidx);
			SetPolyVisited(m_edges[nextedge_x].polyidx);
			nextedge_x = GetNextEdge(nextedge_x,atborder0);
			if (atborder0) 
			{
				temp.push_back(m_edges[nextedge_x].v1);
				temp2.push_back(m_edges[nextedge_x].v0);
			}
		} while (!atborder0);
		m_vtopology.push_back(temp);
		temp.clear();
		m_ptopology.push_back(polylist);
		polylist.clear();

		nextedge_y = GetNextEdge(nextedge_y,atborder1);
		nextedge_x = GetMatchingEdgePlus1(nextedge_y);

	} while (!atborder1);
	m_vtopology.push_back(temp2);
	
	return true;
}

inline Vector NeighborTree::GetVertexPosition(int index)
{
	return m_vertices[index];
}

inline double NeighborTree::DistanceTo(Vector p0,Vector p1)
{
	return (p1-p0).NormalizeLength();
}

inline bool NeighborTree::GenerateDistanceInfo() 
{
	unsigned x,y;
	Vector p0,p1;

	int maxx = m_vtopology[0].size();
	int maxy = m_vtopology.size();

	m_ulist.clear();
	m_vlist.clear();

	// collect sums
	
	DoubleArray xsums,ysums;

	double xsum,ysum;

	// sum of edges along x direction
	for (y = 0; y < (unsigned)maxy; y++) 
	{
		xsum = 0;
		for (x = 0; x < (unsigned)(maxx-1); x++) 
		{
			p0 = GetVertexPosition(m_vtopology[y][x]);
			p1 = GetVertexPosition(m_vtopology[y][x+1]);
			xsum += DistanceTo(p1,p0);
		}
		xsums.push_back(xsum);
	}

	// sum of edges along y direction
	for (x = 0; x < (unsigned)maxx; x++) 
	{
	ysum = 0;
		for (y = 0; y < (unsigned)(maxy-1); y++) 
		{
			p0 = GetVertexPosition(m_vtopology[y][x]);
			p1 = GetVertexPosition(m_vtopology[y+1][x]);
			ysum += DistanceTo(p1,p0);
		}
		ysums.push_back(ysum);
	}	


	// collect ratios
	DoubleArray xlist,ylist;
	DoubleMultiArray xratios,yratios;
	double dist;

	for (y = 0; y < (unsigned)maxy; y++) 
	{
		xlist.clear();
		for (x = 0; x < (unsigned)(maxx-1); x++) 
		{
			p0 = GetVertexPosition(m_vtopology[y][x]);
			p1 = GetVertexPosition(m_vtopology[y][x+1]);
			dist = DistanceTo(p1,p0);
			xlist.push_back(dist/xsums[y]);
		}
		xratios.push_back(xlist); // sematic reasons for yratios instead of xratios
	}

	for (x = 0; x < (unsigned)maxx; x++) 
	{
		ylist.clear();
		for (y = 0; y < (unsigned)(maxy-1); y++) 
		{
			p0 = GetVertexPosition(m_vtopology[y][x]);
			p1 = GetVertexPosition(m_vtopology[y+1][x]);
			dist = DistanceTo(p1,p0);
			ylist.push_back(dist/ysums[x]);
		}
		yratios.push_back(ylist); // sematic reasons for xratios instead of yratios
	}	
	
	unsigned xlen = xratios[0].size(), ylen = xratios.size();
	float val;

	for (y = 0; y < ylen; y++) 
	{
		xlist.clear();
		val = 0;
		xlist.push_back(val);
		for (x = 0; x < xlen; x++) 
		{
			val += (float)xratios[y][x];
			xlist.push_back(val);
		}
		m_ulist.push_back(xlist);
	}

	xlen = yratios.size(), ylen = yratios[0].size();

	for (x = 0; x < xlen; x++) 
	{
		ylist.clear();
		val = 0;
		ylist.push_back(val);
		for (y = 0; y < ylen; y++) 
		{
			val += (float)yratios[x][y];
			ylist.push_back(val);
		}
		m_vlist.push_back(ylist);
	}			
	return true;		
}

inline Vector2 NeighborTree::CalcPolyUVStretch(unsigned vert,unsigned col,unsigned row,double scaletofit)
{
	Vector2 uv_topleft; uv_topleft.Set(0,0);
	Vector2 uv_lowerleft;	uv_lowerleft.Set(0,1);
	Vector2 uv_lowerright; uv_lowerright.Set(1,1);
	Vector2 uv_topright; uv_topright.Set(1,0);
	
	Vector2 temp;
	if (vert == 0)
		temp = uv_topleft;
	else if (vert == 1)
		temp = uv_lowerleft;
	else if (vert == 2)
		temp = uv_lowerright;
	else if (vert == 3)
		temp = uv_topright;

	float maxcol = (float)m_ptopology[0].size();
	float maxrow = (float)m_ptopology.size();

	float ratioU,ratioV;
	
	if (scaletofit==0.0)
	{
		ratioU = (1.0f/maxcol);
		ratioV = (1.0f/maxrow);
	}
	else
	{
		ratioU = (1.0f/maxcol);
		ratioV = (1.0f/maxrow);

		if (maxcol > maxrow)
		{
			ratioU = (float)scaletofit;
		}
		else if (maxrow > maxcol)
		{
			ratioV = (float)scaletofit;
		}
	}

	float u = (float)((temp.x * ratioU) + (col * ratioU));
	float v = (float)(1.0f - ((temp.y * ratioV) + (row * ratioV)));
		
	return Vector2(u,v);
}

inline Vector2 NeighborTree::CalcPolyUV(unsigned vert,unsigned col,unsigned row)
{
	Vector2 uv_topleft(0,0);
	Vector2 uv_lowerleft(0,1);
	Vector2 uv_lowerright(1,1);
	Vector2 uv_topright(1,0);

	Vector2 temp;
	if (vert == 0)
		temp = uv_topleft;
	else if (vert == 1)
		temp = uv_lowerleft;
	else if (vert == 2)
		temp = uv_lowerright;
	else if (vert == 3)
		temp = uv_topright;

	float u = 0, v = 0;

	u = (float)(m_ulist[row+(int)temp.y][col+(int)temp.x]);
	v = (float)(1.0 - (m_vlist[col+(int)temp.x][row+(int)temp.y]));

	return Vector2(u,v);
}

inline int NeighborTree::GetUVAtVertexAndFace(int vidx,int pidx)
{
	for (int i = 0; i < (int)m_neighbors[vidx].sf.size(); i++)
		if (m_neighbors[vidx].sf[i] == pidx)
			return m_neighbors[vidx].uvs[i];
	return -1;
}

inline int NeighborTree::GetEdgesAtVertexAndFace(int vidx,int pidx,int &eidx0,int &eidx1)
{
	int debug =  m_neighbors[vidx].sf.size();

	for (int i = 0; i < (int)m_neighbors[vidx].sf.size(); i++)
	{
		if (m_neighbors[vidx].sf[i] == pidx)
		{
			eidx0 = m_neighbors[vidx].se[i];
			eidx1 = m_neighbors[vidx].se[i+1];
			return 1;
		}
	}
	return 0;
}

inline int NeighborTree::GetUVAtColAndRow(int vert,int col,int row)
{
	int vidx;
	int pidx = m_ptopology[row][col];

	if (vert == 0)
		vidx = m_vtopology[row][col];
	else if (vert == 1)
		vidx = m_vtopology[row+1][col];
	else if (vert == 2)
		vidx = m_vtopology[row+1][col+1];
	else if (vert == 3)
		vidx = m_vtopology[row][col+1];

	return GetUVAtVertexAndFace(vidx,pidx);
}

inline int NeighborTree::DoesValueExist(const int value,const IntArray &array)
{
	for (int i = 0; i < (int)array.size(); i++)
		if (value == array[i])
			return i;
	return -1;
}

inline bool NeighborTree::GetPolyColAndRow(int pidx,int &col,int &row)
{
	IntArray *array;
	int x,y;
	for (y = 0; y < (int)m_ptopology.size(); y++)
	{
		array = &m_ptopology[y];
		x = DoesValueExist(pidx,*array);
		if (x >= 0) {
			col = x; row = y;
			return true;
		}
	}
	return false;
}

inline int NeighborTree::GetVertexOffset(int polyidx,int col,int row)
{
	int vidx = m_vtopology[row][col];
	for (int i = 0; i < (int)m_polyinfolist[polyidx].m_vids.size(); i++)
	{
		int vidx2 = m_polyinfolist[polyidx].m_vids[i];
		if (m_polyinfolist[polyidx].m_vids[i] == vidx)
			return m_polyinfolist[polyidx].m_vids.size()-i;
//			return i;
	}
	return -1;
}

inline void NeighborTree::ShiftIntArray(const int offset,std::vector<int> &array)
{
	std::vector<int> newarray;
	newarray.resize(array.size());
	for (int i = 0; i < (int)array.size(); i++) {
		if ((i+offset)<(int)array.size())
			newarray[i] = array[i+offset];
		else
			newarray[i] = array[(i+offset)-array.size()];
	}
	array = newarray;
}

inline void NeighborTree::ShiftVector2Array(const int offset,std::vector<Vector2> &array)
{
	std::vector<Vector2> newarray;
	newarray.resize(array.size());
	for (int i = 0; i < (int)array.size(); i++) {
		if ((i+offset)<(int)array.size())
			newarray[i] = array[i+offset];
		else
			newarray[i] = array[(i+offset)-array.size()];
	}
	array = newarray;
}

inline void NeighborTree::CalcNormals(bool supersmooth)
{
	m_vertexnormals.clear();
	m_vertexnormals.resize(m_vertices.size());

	int vidx;
	for (int j = 0; j < (int)m_polyindicies.size(); j++)
	{
		Vector normal;
		for (int i = 0; i < (int)m_polyinfolist[m_polyindicies[j]].m_vids.size(); i++)
		{
			vidx = m_polyinfolist[m_polyindicies[j]].m_vids[i];
			normal = Vector(0,0,0);

			if (supersmooth)
				for (int k = 0; k < (int)m_neighbors[vidx].sv.size(); k+=2)
					normal += CalcNormal(m_neighbors[vidx].sv[k]);

			normal += CalcNormal(vidx);
			normal = normal.Normalized();

			m_vertexnormals[vidx] = normal;
		}
	}
}

inline Vector NeighborTree::CalcNormal(int vidx)
{
	Vector a,b,n;
	unsigned neighborlength = m_neighbors[vidx].sv.size();

	for (int i = 0; i < (int)neighborlength; i+=2)
	{
		a = m_vertices[m_neighbors[vidx].sv[i]] - m_vertices[vidx];
		if ( (i+1) < (int)neighborlength )
		{
			b = m_vertices[m_neighbors[vidx].sv[i+1]] - m_vertices[vidx];
			a.Normalize();
			b.Normalize();
			n += b^a;
		}
	}
	n.Normalize();
	return n;
}

inline std::ostream &operator << (std::ostream &os,NeighborTree &other)
{
	for (int i = 0; i < (int)other.m_edges.size(); i++)
		os << "Edge (" << i << ") " << other.m_edges[i];
	os << std::endl;

	for (i = 0; i < (int)other.m_polyinfolist.size(); i++)
	{
		os << "Poly: " << i << std::endl;
		for (int j = 0; j < (int)other.m_polyinfolist[i].m_vids.size(); j++)
			os << other.m_polyinfolist[i].m_vids[j] << ",";
		os << std::endl;
		
		for (j = 0; j < (int)other.m_polyinfolist[i].m_eids.size(); j++)
			os << other.m_edges[other.m_polyinfolist[i].m_eids[j]];
	}
	os << std::endl;

	os << "Poly Topology" << std::endl;
	for (int y = 0; y < (int)other.m_ptopology.size(); y++)
	{
		for (int x = 0; x < (int)other.m_ptopology[0].size(); x++)
		{
			os << other.m_ptopology[y][x] << " ";
		}
		os << std::endl;
	}
	os << std::endl;
	os << "Vertex Topology" << std::endl;

	for (y = 0; y < (int)other.m_vtopology.size(); y++)
	{
		for (int x = 0; x < (int)other.m_vtopology[0].size(); x++)
		{
			os << other.m_vtopology[y][x] << " ";
		}
		os << std::endl;
	}
	os << "UV Topology" << std::endl;

	for (y = 0; y < (int)other.m_ptopology.size(); y++)
	{
		for (int x = 0; x < (int)other.m_ptopology[0].size(); x++)
		{
			int pidx = other.m_ptopology[y][x];

			int vidx = other.m_vtopology[y][x];
			int index = other.GetUVAtVertexAndFace(vidx,pidx);
			os << index << " ";

			vidx = other.m_vtopology[y][x+1];
			index = other.GetUVAtVertexAndFace(vidx,pidx);
			os << index << " ";
		}
		os << std::endl;

		for (x = 0; x < (int)other.m_ptopology[0].size(); x++)
		{
			int pidx = other.m_ptopology[y][x];

			int vidx = other.m_vtopology[y+1][x];
			int index = other.GetUVAtVertexAndFace(vidx,pidx);
			os << index << " ";

			vidx = other.m_vtopology[y+1][x+1];
			index = other.GetUVAtVertexAndFace(vidx,pidx);
			os << index << " ";
		}
		os << std::endl;
	}

	return os;
}

inline void NeighborTree::GetInteriorEdges(std::vector<int> &edges)
{
	float maxcol = (float)m_ptopology[0].size();
	float maxrow = (float)m_ptopology.size();

	for (int y = 0; y < maxrow; y++)
	{
		for (int x = 0; x < maxcol; x++)
		{
			int pidx = m_ptopology[y][x];
			
			int vidx = m_vtopology[y][x];
	
			int eidx0,eidx1;
			GetEdgesAtVertexAndFace(vidx,pidx,eidx0,eidx1);

			if ((y>0) && (y <maxrow))
			{
				edges.push_back(eidx0);
			}

			if ((x>0) && (x <maxcol))
			{
				edges.push_back(eidx1);
			}
		}
	}
}

inline void NeighborTree::GetPolyCounts(MIntArray &polycounts,bool wholemesh)
{
	polycounts.clear();

	if (!wholemesh)
	{
		for (int j = 0; j < (int)m_polyindicies.size(); j++)
			polycounts.append(m_polyinfolist[m_polyindicies[j]].m_vids.size());
	}
	else
	{
		for (int j = 0; j < (int)m_polyinfolist.size(); j++)
			polycounts.append(m_polyinfolist[j].m_vids.size());
	}
}

inline void NeighborTree::GetPolyConnects(MIntArray &polyconnects)
{
	polyconnects.clear();

	for (int j = 0; j < (int)m_polyindicies.size(); j++)
		for (int i = 0; i < (int)m_polyinfolist[m_polyindicies[j]].m_vids.size(); i++)
			polyconnects.append(m_polyinfolist[m_polyindicies[j]].m_vids[i]);
}

inline void NeighborTree::GetUVConnects(MIntArray &uvconnects)
{
	uvconnects.clear();

	for (int j = 0; j < (int)m_polyindicies.size(); j++)
		for (int i = 0; i < (int)m_polyinfolist[m_polyindicies[j]].m_uvids.size(); i++)
			uvconnects.append(m_polyinfolist[m_polyindicies[j]].m_uvids[i]);
}

inline void NeighborTree::BuildTextureClampData(MFloatArray &uarray,MFloatArray &varray,MIntArray &uvconnects,int nostretch,double scaletofit)
{
	int col,row;

	std::vector<Vector2> uvs;
	uvs.resize(4);

	int uvidx[4];

	uarray.clear();
	varray.clear();
	uvconnects.clear();

	int uvid;

	VectorMap vmap;
	
	for (int j = 0; j < (int)m_polyindicies.size(); j++)
	{
		int index = m_polyindicies[j];

		GetPolyColAndRow(index,col,row);

		if (nostretch)
		{
			uvs[0] = CalcPolyUV(0,col,row);
			uvs[1] = CalcPolyUV(1,col,row);
			uvs[2] = CalcPolyUV(2,col,row);
			uvs[3] = CalcPolyUV(3,col,row);
		}
		else
		{
			uvs[0] = CalcPolyUVStretch(0,col,row,scaletofit);
			uvs[1] = CalcPolyUVStretch(1,col,row,scaletofit);
			uvs[2] = CalcPolyUVStretch(2,col,row,scaletofit);
			uvs[3] = CalcPolyUVStretch(3,col,row,scaletofit);
		}
		uvidx[0] = GetUVAtColAndRow(0,col,row);
		uvidx[1] = GetUVAtColAndRow(1,col,row);
		uvidx[2] = GetUVAtColAndRow(2,col,row);
		uvidx[3] = GetUVAtColAndRow(3,col,row);

		int offset = GetVertexOffset(index,col,row);
		ShiftVector2Array(offset,uvs);

		PolyInfo *poly = &m_polyinfolist[m_polyindicies[j]];

		for (int i = 0; i < (int)uvs.size(); i++)
		{
			Vector key = Vector(uvs[i].x,uvs[i].y,0);
			if (!vmap.HasKey(key))
			{
				m_uvs.push_back(uvs[i]);
				uvid = m_uvs.size()-1;
				poly->m_uvids[i] = uvid;
				vmap.CreateKey(key,uvid);
			}
			else
			{
				vmap.GetKeyValue(key,uvid);
				poly->m_uvids[i] = uvid;
			}
		}
	}

	IntMap imap;

	int newuvid;

	for (j = 0; j < (int)m_polyinfolist.size(); j++)
	{
		PolyInfo *poly = &m_polyinfolist[j];

		for (int i = 0; i < (int)poly->m_uvids.size();i++)
		{
			uvid = poly->m_uvids[i];
			if (!imap.HasKey(uvid))
			{
				newuvid = uarray.length();
				uarray.append((float)m_uvs[uvid].x);
				varray.append((float)m_uvs[uvid].y);
				imap.SetValue(uvid,newuvid);
			}
			else
			{
				imap.GetValue(uvid,newuvid);
			}
			uvconnects.append(newuvid);
		}
	}
}