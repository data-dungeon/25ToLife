inline int NeighborTree::addEdgeToList(int v0,int v1,int patchidx)
{
	Edge edge(v0,v1);
	edge.flags.kIsBorderEdge = 1;
	edge.patchidx = patchidx;

	for (int i = 0; i < _edges.size(); i++) {
		if (edge == _edges[i]) {
			_edges[i].flags.kIsBorderEdge = 0;
			_edges[i].adjpatchidx = patchidx;
			edge.adjpatchidx = _edges[i].patchidx;
			edge.flags.kIsBorderEdge = 0;
			break;
		}
	}
	_edges.push_back(edge);
	return _edges.size()-1;
}

inline int NeighborTree::addVertexToList(Vector *vp)
{
	if (vp) {
		for (int i = 0; i < _vpointers.size(); i++) {
			if (vp == _vpointers[i])
				return i;
		}
		_vpointers.push_back(vp);
		return _vpointers.size()-1;
	}
	return -1;
}

inline bool NeighborTree::GenerateNeighborInfo(HashPatches &patches)
{
	int v0,v1,v2,v3;
	HashCPs cps;
	for (int i = 0; i < patches.size(); i++) {
		HashTools::GetPatchCPs(patches[i],cps);
		v0 = addVertexToList((Vector*)cps[0]->GetHead()->GetPosition());
		v1 = addVertexToList((Vector*)cps[1]->GetHead()->GetPosition());
		v2 = addVertexToList((Vector*)cps[2]->GetHead()->GetPosition());
		v3 = addVertexToList((Vector*)cps[3]->GetHead()->GetPosition());

		PatchInfo info;
		if ( v0 >= 0 )
			info._vids.push_back(v0);
		if ( v1 >= 0 )
			info._vids.push_back(v1);
		if ( v2 >= 0 )
			info._vids.push_back(v2);
		if ( v3 >= 0 )
			info._vids.push_back(v3);

		if (info._vids.size() == 3) {
			info._eids.resize(3);
			info._eids[0] = addEdgeToList(v0,v1,i);
			info._eids[1] = addEdgeToList(v1,v2,i);
			info._eids[2] = addEdgeToList(v2,v0,i);
		}
		else if (info._vids.size() == 4) {
			info._eids.resize(4);
			info._eids[0] = addEdgeToList(v0,v1,i);
			info._eids[1] = addEdgeToList(v1,v2,i);
			info._eids[2] = addEdgeToList(v2,v3,i);
			info._eids[3] = addEdgeToList(v3,v0,i);
		}

		_patchinfolist.push_back(info);
	}

	_neighbors.resize(_vpointers.size());

	std::vector<int> countarray;
	countarray.resize(_vpointers.size());

	for (i = 0; i < _patchinfolist.size(); i++) {
		SetNeighborsForPatch(_patchinfolist[i],i);
		for (int j = 0; j < _patchinfolist[i]._vids.size(); j++)
			countarray[_patchinfolist[i]._vids[j]]++;
	}
	_cornercount = 0;
	for (i = 0; i < countarray.size(); i++) {
		if (countarray[i] == 1) {
			_neighbors[i].flags.kIsBorderCorner = 1;
			_cornercount++;
		}
	}

	return true;
}

inline bool NeighborTree::IsSelectionCylindrical() {
	if (_cornercount != 4)
		return true;
	return false;
}

inline int NeighborTree::GetFirstCorner()
{
	for (int i = 0; i < _neighbors.size(); i++) {
		if (_neighbors[i].flags.kIsBorderCorner) {
			return i;
		}
	}
	return -1;
}

inline int NeighborTree::GetCornerPatch()
{
	int vid = GetFirstCorner();
	if (vid < 0)
		return -1;
	return _neighbors[vid]._nsp.front();
}

inline int NeighborTree::GetFirstBorderEdge() {
	for (int i = 0; i < _edges.size(); i++)
		if (_edges[i].flags.kIsBorderEdge)
			return i;
	return -1;
}

inline int NeighborTree::GetStartEdge()
{
	if (IsSelectionCylindrical()) {
		int eidx = GetFirstBorderEdge();
		return GetMatchingEdgePlus1(eidx);		
	}
	else if (_patchinfolist.size() == 1) {
		return GetFirstBorderEdge();
	}
	else {
		int eidx;
		int patch = GetCornerPatch();
		bool finished = false;
		bool trigger = false;
		int i = 0,count = 1;;
		while(!finished) {
			eidx = _patchinfolist[patch]._eids[i];
			if (!_edges[eidx].flags.kIsBorderEdge)
				trigger = true;
			if (trigger && _edges[eidx].flags.kIsBorderEdge) {
				if (count == 2)
					return _patchinfolist[patch]._eids[i];
				count++;
			}
			if (++i >= _patchinfolist[patch]._eids.size())
				i = 0;
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdge(int eidx)
{
	int pidx = _edges[eidx].patchidx;
	for (int i = 0; i < _patchinfolist[pidx]._eids.size();i++)
		if (_edges[eidx] == _edges[_patchinfolist[pidx]._eids[i]])
			return _patchinfolist[pidx]._eids[i];
	return -1;
}

inline int NeighborTree::GetMatchingEdge(int pidx,int eidx)
{
	for (int i = 0; i < _patchinfolist[pidx]._eids.size();i++)
		if (_edges[eidx] == _edges[_patchinfolist[pidx]._eids[i]])
			return _patchinfolist[pidx]._eids[i];
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus1(int eidx)
{
	int pidx = _edges[eidx].patchidx;
	for (int i = 0; i < _patchinfolist[pidx]._eids.size();i++) {
		if (_edges[eidx] == _edges[_patchinfolist[pidx]._eids[i]]) {
			if ((i+1) >= _patchinfolist[pidx]._eids.size())
				return _patchinfolist[pidx]._eids[(i+1)-_patchinfolist[pidx]._eids.size()];
			return _patchinfolist[pidx]._eids[i+1];
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus2(int eidx)
{
	int pidx = _edges[eidx].patchidx;
	for (int i = 0; i < _patchinfolist[pidx]._eids.size();i++) {
		if (_edges[eidx] == _edges[_patchinfolist[pidx]._eids[i]]) {
			if ((i+2) >= _patchinfolist[pidx]._eids.size())
				return _patchinfolist[pidx]._eids[(i+2)-_patchinfolist[pidx]._eids.size()];
			return _patchinfolist[pidx]._eids[i+2];
		}
	}
	return -1;
}

inline int NeighborTree::GetMatchingEdgePlus3(int eidx)
{
	int pidx = _edges[eidx].patchidx;
	for (int i = 0; i < _patchinfolist[pidx]._eids.size();i++) {
		if (_edges[eidx] == _edges[_patchinfolist[pidx]._eids[i]]) {
			if ((i+3) >= _patchinfolist[pidx]._eids.size())
				return _patchinfolist[pidx]._eids[(i+3)-_patchinfolist[pidx]._eids.size()];
			return _patchinfolist[pidx]._eids[i+3];
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
	if (!_edges[next_eidx].flags.kIsBorderEdge) {
		int adjpatchidx = _edges[next_eidx].adjpatchidx;

		if (HasBeenVisited(adjpatchidx)) {
			atborder = true;
			return next_eidx;
		}

		return GetMatchingEdge(adjpatchidx,next_eidx);
	}
	atborder = true;
	return next_eidx;
}

inline void NeighborTree::InitVisitedLookup() {
	_visited.clear();
	_visited.resize(_patchinfolist.size());
}

inline void NeighborTree::SetPatchVisited(int pid)
{
	_visited[pid] = true;
}

inline int NeighborTree::HasBeenVisited(int pid)
{
	if (_visited[pid])
		return true;
	return false;
}

inline bool NeighborTree::GenerateTopologyInfo() {

	bool atborder0 = false,atborder1=false;
	IntArray temp,temp2;
	IntArray polylist;

	InitVisitedLookup();

	int nextedge_x = GetStartEdge();
	int nextedge_y = GetMatchingEdgePlus3(nextedge_x);
	bool finished = false;
	
	do
	{
		temp2.clear();
		do
		{
			temp.push_back(_edges[nextedge_x]._v0);
			temp2.push_back(_edges[nextedge_x]._v1);
			polylist.push_back(_edges[nextedge_x].patchidx);
			SetPatchVisited(_edges[nextedge_x].patchidx);
			nextedge_x = GetNextEdge(nextedge_x,atborder0);
			if (atborder0) {
				temp.push_back(_edges[nextedge_x]._v1);
				temp2.push_back(_edges[nextedge_x]._v0);
			}
		} while (!atborder0);
		_vtopology.push_back(temp);
		temp.clear();
		_ptopology.push_back(polylist);
		polylist.clear();

		nextedge_y = GetNextEdge(nextedge_y,atborder1);
		nextedge_x = GetMatchingEdgePlus1(nextedge_y);

	} while (!atborder1);
	_vtopology.push_back(temp2);
	
	return true;
}

inline Vector NeighborTree::GetVertexPosition(const int index) {
	return Vector(_vpointers[index]->x,_vpointers[index]->y,_vpointers[index]->z);
}

inline double NeighborTree::DistanceTo(const Vector p0,const Vector p1) {
	return (Vector(p1.x-p0.x,p1.y-p0.y,p1.z-p0.z)).Norm();
}

inline bool NeighborTree::GenerateDistanceInfo() {
	unsigned x,y;
	Vector p0,p1;

	int maxx = _vtopology[0].size();
	int maxy = _vtopology.size();

	_ulist.clear();
	_vlist.clear();

	// collect sums
	
	DoubleArray xsums,ysums;

	double xsum,ysum;

	// sum of edges along x direction
	for (y = 0; y < maxy; y++) {
		xsum = 0;
		for (x = 0; x < (maxx-1); x++) {
			p0 = GetVertexPosition(_vtopology[y][x]);
			p1 = GetVertexPosition(_vtopology[y][x+1]);
			xsum += DistanceTo(p1,p0);
		}
		xsums.push_back(xsum);
	}

	// sum of edges along y direction
	for (x = 0; x < maxx; x++) {
		ysum = 0;
		for (y = 0; y < (maxy-1); y++) {
			p0 = GetVertexPosition(_vtopology[y][x]);
			p1 = GetVertexPosition(_vtopology[y+1][x]);
			ysum += DistanceTo(p1,p0);
		}
		ysums.push_back(ysum);
	}	


	// collect ratios
	DoubleArray xlist,ylist;
	DoubleMultiArray xratios,yratios;
	double dist;

	for (y = 0; y < maxy; y++) {
		xlist.clear();
		for (x = 0; x < (maxx-1); x++) {
			p0 = GetVertexPosition(_vtopology[y][x]);
			p1 = GetVertexPosition(_vtopology[y][x+1]);
			dist = DistanceTo(p1,p0);
			xlist.push_back(dist/xsums[y]);
		}
		xratios.push_back(xlist); // sematic reasons for yratios instead of xratios
	}

	for (x = 0; x < maxx; x++) {
		ylist.clear();
		for (y = 0; y < (maxy-1); y++) {
			p0 = GetVertexPosition(_vtopology[y][x]);
			p1 = GetVertexPosition(_vtopology[y+1][x]);
			dist = DistanceTo(p1,p0);
			ylist.push_back(dist/ysums[x]);
		}
		yratios.push_back(ylist); // sematic reasons for xratios instead of yratios
	}	
	
	unsigned xlen = xratios[0].size(), ylen = xratios.size();
	float val;

	for (y = 0; y < ylen; y++) {
		xlist.clear();
		val = 0;
		xlist.push_back(val);
		for (x = 0; x < xlen; x++) {
			val += xratios[y][x];
			xlist.push_back(val);
		}
		_ulist.push_back(xlist);
	}

	xlen = yratios.size(), ylen = yratios[0].size();

	for (x = 0; x < xlen; x++) {
		ylist.clear();
		val = 0;
		ylist.push_back(val);
		for (y = 0; y < ylen; y++) {
			val += yratios[x][y];
			ylist.push_back(val);
		}
		_vlist.push_back(ylist);
	}			
	return true;		
}


inline void NeighborTree::SetNeighborsForPatch(PatchInfo &pinfo,int patchidx)
{
	for (int idx = 0; idx < pinfo._vids.size(); idx++) {
		Neighbor *neighbor = &_neighbors[pinfo._vids[idx]];
		if (pinfo._vids.size() == 4) {
			if (idx == 0) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[3]);
				neighbor->_nse.push_back(pinfo._eids[0]);
			}
			else if (idx == 1) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[0]);
				neighbor->_nse.push_back(pinfo._eids[1]);
			}
			else if (idx == 2) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[1]);
				neighbor->_nse.push_back(pinfo._eids[2]);
			}
			else if (idx == 3) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[2]);
				neighbor->_nse.push_back(pinfo._eids[3]);
			}
		}
		else if (pinfo._vids.size() == 3) {	
			if (idx == 0) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[2]);
				neighbor->_nse.push_back(pinfo._eids[0]);
			}
			else if (idx == 1) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[0]);
				neighbor->_nse.push_back(pinfo._eids[1]);
			}
			else if (idx == 2) {
				neighbor->_nsp.push_back(patchidx);
				neighbor->_nse.push_back(pinfo._eids[1]);
				neighbor->_nse.push_back(pinfo._eids[2]);
			}
		}
	}
}

inline Vector2 NeighborTree::CalcPatchUVStretch(unsigned vert,unsigned col,unsigned row) {
	Vector2 uv_topleft(0,0);
	Vector2 uv_lowerleft(0,1);
	Vector2 uv_lowerright(1,1);
	Vector2 uv_topright(1,0);

	Vector2 temp;
	if (vert == 0)
		temp = uv_topleft;
	else if (vert == 1)
		temp = uv_topright;
	else if (vert == 2)
		temp = uv_lowerright;
	else if (vert == 3)
		temp = uv_lowerleft;

	float maxcol = (float)_ptopology[0].size();
	float maxrow = (float)_ptopology.size();

	float ratioU = (1.0/maxcol);
	float ratioV = (1.0/maxrow);

	float u = (temp.x * ratioU) + (col * ratioU);
	float v = (temp.y * ratioV) + (row * ratioV);
		
	return Vector2(u,v);
}

inline Vector2 NeighborTree::CalcPatchUV(unsigned vert,unsigned col,unsigned row) {
	Vector2 uv_topleft(0,0);
	Vector2 uv_lowerleft(0,1);
	Vector2 uv_lowerright(1,1);
	Vector2 uv_topright(1,0);

	Vector2 temp;
	if (vert == 0)
		temp = uv_topleft;
	else if (vert == 1)
		temp = uv_topright;
	else if (vert == 2)
		temp = uv_lowerright;
	else if (vert == 3)
		temp = uv_lowerleft;

	float u = 0, v = 0;

	u = _ulist[row+temp.y][col+temp.x];
	v = _vlist[col+temp.x][row+temp.y];

	return Vector2(u,v);
}

inline int NeighborTree::DoesValueExist(const int value,const IntArray &array) {
	for (int i = 0; i < array.size(); i++)
		if (value == array[i])
			return i;
	return -1;
}

inline bool NeighborTree::GetPatchColAndRow(int pidx,int &col,int &row) {
	IntArray *array;
	int x,y;
	for (y = 0; y < _ptopology.size(); y++) {
		array = &_ptopology[y];
		x = DoesValueExist(pidx,*array);
		if (x >= 0) {
			col = x; row = y;
			return true;
		}
	}
	return false;
}

inline int NeighborTree::GetVertexOffset(const int patch,const int col, const int row) {
	int vid = _vtopology[row][col];
	for (int i = 0; i < _patchinfolist[patch]._vids.size(); i++)
		if (_patchinfolist[patch]._vids[i] == vid)
			return i;
	return -1;
}

inline void NeighborTree::ShiftIntArray(const int offset,std::vector<Vector2> &array){
	std::vector<Vector2> newarray;
	newarray.resize(array.size());
	for (int i = 0; i < array.size(); i++) {
		if ((i+offset)<array.size())
			newarray[i] = array[i+offset];
		else
			newarray[i] = array[(i+offset)-array.size()];
	}
	array = newarray;
}

inline ostream &operator << (ostream &os,NeighborTree &other)
{
	for (int i = 0; i < other._edges.size(); i++)
		os << "Edge (" << i << ") " << other._edges[i] << endl;

	for (i = 0; i < other._patchinfolist.size(); i++)
	{
		os << "Patch: " << i << endl;
		for (int j = 0; j < other._patchinfolist[i]._vids.size(); j++)
		{
			os << other._vpointers[other._patchinfolist[i]._vids[j]]->x << ",";
			os << other._vpointers[other._patchinfolist[i]._vids[j]]->y << endl;

		}
		for (j = 0; j < other._patchinfolist[i]._eids.size(); j++)
			os << other._edges[other._patchinfolist[i]._eids[j]];
	}
	os << endl;

	os << "Patch Topology" << endl;
	for (int y = 0; y < other._ptopology.size(); y++)
	{
		for (int x = 0; x < other._ptopology[0].size(); x++)
		{
			os << other._ptopology[y][x] << " ";
		}
		os << endl;
	}
	os << endl;
	os << "Vertex Topology" << endl;

	for (y = 0; y < other._vtopology.size(); y++)
	{
		for (int x = 0; x < other._vtopology[0].size(); x++)
		{
			os << other._vtopology[y][x] << " ";
		}
		os << endl;
	}

	return os;
}
