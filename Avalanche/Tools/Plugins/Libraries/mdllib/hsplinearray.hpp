//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SPLINEARRAY INLINE METHODS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// get cp pointer from absolute cp index
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HCP *HSplineArray::getCP(unsigned cpindex) {
	unsigned relCPIndex,splineIndex;
	findSplineRelativeCP(cpindex,relCPIndex,splineIndex);
	return this->getCP(relCPIndex,splineIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// get cp pointer from relative cp index and spline index
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HCP *HSplineArray::getCP(int relCPIndex,unsigned splineIndex) {
	HSpline *spln = this->get(splineIndex);
	if ((relCPIndex < 0)||(relCPIndex > (int)(spln->length()-1)))
		return NULL;
	return spln->get(relCPIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds relative cp index and spline index from absolute cp index
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::findSplineRelativeCP(unsigned absCPIndex, unsigned &relCPIndex, unsigned &splineIndex) {
	unsigned idx = absCpIdxMap->getValue(absCPIndex);
	relCPIndex = relCpiTable[idx];
	splineIndex = splineTable[idx];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// gets patch indicies associated with cp vertex indicies
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::getSegmentPatchAssociations(unsigned cpvindex0,unsigned cpvindex1,std::vector<int> &patchIndicies) {
	std::vector<int> temp;
	this->neighbors[cpvindex0].nsp.commonElements(this->neighbors[cpvindex1].nsp,temp);
	for (unsigned i = 0; i < temp.size(); i++)
		patchIndicies.push_back(temp[i]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	use this method to retrieve one span of a spline based on a start cp and stop cp.
///////////////////////////////////////////////////////////////////////////////////////////////////

inline HSplineSegment HSplineArray::getSplineSegment(unsigned cpindex0, unsigned cpindex1,std::vector<int> &patchIndicies) {
	HSplineSegment seg,seg1,seg2;
	HCP *cp, *cpnext;
	unsigned vi0,vi1;

	unsigned relative_cpindex0,relative_cpindex1;
	unsigned spline_index0,spline_index1;

	patchIndicies.clear();

	findSplineRelativeCP(cpindex0,relative_cpindex0,spline_index0);
	cp = getCP(relative_cpindex0,spline_index0);

	findSplineRelativeCP(cpindex1,relative_cpindex1,spline_index1);
	cpnext = getCP(relative_cpindex1,spline_index1);
	
	if (spline_index0 != spline_index1) {
		if ((cp->flags.kIsHook) && (cpnext->flags.kIsHookbase)) {
			if (doesCPHookbasePointToIndex(cp,cpnext->cpindex)) {
				seg.v0 = getHookPos(cp);
				seg.v1 = getCPPos(cpnext);
				splitHookedSegment(cp,seg1,seg2);
				seg.cv0 = seg1.cv1.negate();
				seg.cv1 = seg1.cv0.negate();
				seg.flags.kHookShortSegment = 1;
				vi0 = drillForVertexIndex(cpnext);
				if (cpnext->next() == NULL) {
					vi1 = drillForVertexIndex(cpnext->head());
				}
				else {
					if (cpnext->next()->flags.kIsLoop)
						vi1 = drillForVertexIndex(cpnext->head());
					else
						vi1 = drillForVertexIndex(cpnext->next());
				}
				getSegmentPatchAssociations(vi0,vi1,patchIndicies);
				return seg;
			}
		}
		if ((cp->flags.kIsHookbaseNeighbor) && (cpnext->flags.kIsHook)) {
			if (doesCPHookbaseNeighborPointToIndex(cpnext,cp->cpindex)) {
				seg.v0 = getCPPos(cp);
				seg.v1 = getHookPos(cpnext);
				splitHookedSegment(cpnext,seg1,seg2);
				seg.cv0 = seg2.cv1.negate();
				seg.cv1 = seg2.cv0.negate();
				seg.flags.kHookShortSegment = 1;
				if (cp->previous() == NULL)
					vi0 = drillForVertexIndex(cp->tail()->previous());
				else
					vi0 = drillForVertexIndex(cp->previous());
				vi1 = drillForVertexIndex(cp);
				getSegmentPatchAssociations(vi0,vi1,patchIndicies);
				return seg;
			}
		}
		if ((cp->flags.kIsHookbase) && (cpnext->flags.kIsHook)) {
			if (doesCPHookbasePointToIndex(cpnext,cp->cpindex)) {
				seg.v0 = getCPPos(cp);
				seg.v1 = getHookPos(cpnext);
				splitHookedSegment(cpnext,seg1,seg2);
				seg.cv0 = seg1.cv0;
				seg.cv1 = seg1.cv1;
				seg.flags.kHookShortSegment = 1;
				vi0 = drillForVertexIndex(cp);
				if (cp->next() == NULL) {
					vi1 = drillForVertexIndex(cp->head());
				}
				else {
					if (cp->next()->flags.kIsLoop)
						vi1 = drillForVertexIndex(cp->head());
					else
						vi1 = drillForVertexIndex(cp->next());
				}
				getSegmentPatchAssociations(vi0,vi1,patchIndicies);
				return seg;
			}
		}
		if ((cp->flags.kIsHook) && (cpnext->flags.kIsHookbaseNeighbor)) {
			if (doesCPHookbaseNeighborPointToIndex(cp,cpnext->cpindex)) {
				seg.v0 = getHookPos(cp);
				seg.v1 = getCPPos(cpnext);
				splitHookedSegment(cp,seg1,seg2);
				seg.cv0 = seg2.cv0;
				seg.cv1 = seg2.cv1;
				seg.flags.kHookShortSegment = 1;
				if (cpnext->previous() == NULL)
					vi0 = drillForVertexIndex(cpnext->tail()->previous());
				else
					vi0 = drillForVertexIndex(cpnext->previous());
				vi1 = drillForVertexIndex(cpnext);
				getSegmentPatchAssociations(vi0,vi1,patchIndicies);
				return seg;
			}
		}
	}

//	Get patch associations
	vi0 = drillForVertexIndex(cp);
	if (cpnext->flags.kIsLoop)
		vi1 = drillForVertexIndex(cpnext->head());
	else
		vi1 = drillForVertexIndex(cpnext);

	getSegmentPatchAssociations(vi0,vi1,patchIndicies);

	if (((cp->flags.kIsHookbase) && (cpnext->flags.kIsHookbaseNeighbor)) ||
		((cpnext->flags.kIsHookbase) && (cp->flags.kIsHookbaseNeighbor))) {
		HCP *hookcp;
		HHook *hook;
	
		if (cp->flags.kIsHookbaseNeighbor) {
			if (doesCPHookbasePointToIndex(cp,cpnext->cpindex)) {

				hookcp = getCP(hookList.get(cpnext->hookIndex)->hookidx);
				hook = hookList.get(hookcp->hookIndex);	
				
				seg.flags.kHookLongSegment = 1;
				getSegmentPatchAssociations(hook->vindex,vi0,patchIndicies);
				getSegmentPatchAssociations(hook->vindex,vi1,patchIndicies);
			}
		}
		if (cpnext->flags.kIsHookbaseNeighbor) {
			if (doesCPHookbasePointToIndex(cpnext,cp->cpindex)) {

				hookcp = getCP(hookList.get(cp->hookIndex)->hookidx);
				hook = hookList.get(hookcp->hookIndex);

				seg.flags.kHookLongSegment = 1;
				getSegmentPatchAssociations(hook->vindex,vi0,patchIndicies);
				getSegmentPatchAssociations(hook->vindex,vi1,patchIndicies);
			}
		}
	}

	int direction;

//	Get spline direction
	if (relative_cpindex1 > relative_cpindex0)
		direction = 1;
	else
		direction = -1;

	HSpline *spline;
	unsigned splinelength;

// Get spline
	if (!cp->flags.kIsHook) {
		spline = this->get(spline_index0);
		splinelength = spline->length();
	}
	else {
		spline = this->get(spline_index1);
		splinelength = spline->length();
	}
	
	unsigned index0 = relative_cpindex0;
	unsigned index1 = relative_cpindex1;

	HVector V0 = getCPPos(spline->get(relative_cpindex0));
	HVector V1 = getCPPos(spline->get(relative_cpindex1));

//	Get hook positions
	if ((spline->get(relative_cpindex0))->flags.kIsHook)
		V0 = getHookPos(spline->get(relative_cpindex0)->hookIndex);

	if ((spline->get(relative_cpindex1))->flags.kIsHook)
		V1 = getHookPos(spline->get(index1)->hookIndex);

//	Return a spline with no length, if points overlap

	if (relative_cpindex0 == relative_cpindex1) {
		seg.v0 = seg.cv0 = seg.cv1 = seg.v1 = V0;
		seg.flags.kHasNoLength = 1;
		return seg;
	}

	seg.v0 = V0;
	seg.v1 = V1;

	HVector it,ot;

	bool fliptangents = false;

	if (direction < 0)
		fliptangents = true;

	if ((relative_cpindex0 == 0) && (relative_cpindex1 == splinelength-2)) {
		if ((spline->tail())->flags.kIsLoop) {
			ot = ((spline->tail())->previous())->ot;
			it = (spline->tail())->it;
			seg.cv0 = it.negate();
			seg.cv1 = ot.negate();
			return seg;
		}
	}
	else if ((relative_cpindex0 == splinelength-2) && (relative_cpindex1 == 0)) {
		if ((spline->tail())->flags.kIsLoop) {
			seg.cv0 = ((spline->tail())->previous())->ot;
			seg.cv1 = (spline->tail())->it;
			return seg;
		}
	}

	if (!fliptangents) {
		ot = (spline->get(relative_cpindex0))->ot; it = (spline->get(relative_cpindex1))->it;
	}
	else {
		it = (spline->get(relative_cpindex1))->ot; ot = (spline->get(relative_cpindex0))->it;
		it.negate(); ot.negate();
	}

	seg.cv0 = ot; seg.cv1 = it;

	if ((!cp->flags.kIsSmooth) && (!cpnext->flags.kIsSmooth))
		seg.flags.kHardEdge = 1;

	return seg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// drill for find vertex index
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int HSplineArray::drillForVertexIndex(unsigned cpindex) {
	HCP *cp = getCP(cpindex);
	if (cp->flags.kIsHook) {
		HHook *hook = hookList.get(cp->hookIndex);
		return hook->vindex;
	}

	if (cp->flags.kIsLoop)
		cp = cp->head();

	while (cp->flags.kHasChild)
		cp = getChildCP(cp->cpindex);

	return cp->vindex;
}

inline int HSplineArray::drillForVertexIndex(HCP *cp) {
	if (cp->flags.kIsHook) {
		HHook *hook = hookList.get(cp->hookIndex);
		return hook->vindex;
	}

	if (cp->flags.kIsLoop)
		cp = cp->head();

	while (cp->flags.kHasChild)
		cp = getChildCP(cp->cpindex);

	return cp->vindex;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// method for adding a vertex position to the vertexlist
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline unsigned HSplineArray::addVertexToList( const HVector &pos ) {
	return this->vertices.append(pos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// method for getting a vertex position from the vertexlist
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HVector HSplineArray::getCPPos(unsigned cpindex) {
	HCP *cp = getCP(cpindex);
	if (cp->flags.kIsLoop)
		cp = cp->head();
	unsigned vi = drillForVertexIndex(cp);
	if (vi >= vertices.length()) {
		THROW_FATAL_ERROR("Index out of range. [getCPPos]\n");
	}
	return vertices[vi];
}

inline HVector HSplineArray::getCPPos(HCP *cp) {
	if (cp->flags.kIsLoop)
		cp = cp->head();
	unsigned vi = drillForVertexIndex(cp);
	if (vi >= vertices.length()) {
		THROW_FATAL_ERROR("Index out of range. [getCPPos]\n");
	}
	return vertices[vi];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// method for getting a hook position from the vertexlist
//////////////////////////////////////////////////////////////////////////////////////////////////////////
inline HVector HSplineArray::getHookPos(HCP *hookcp) {
	return vertices[hookList.get(hookcp->hookIndex)->vindex];
}

inline HVector HSplineArray::getHookPos(HHook *hook) {
	return vertices[hook->vindex];
}

inline HVector HSplineArray::getHookPos(unsigned hookindex) {
	return vertices[hookList[hookindex].vindex];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods for retrieving cp stacks
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds all cp's in stack
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::fillCPStack(unsigned cpindex, HIntMap &stackmap) {
	stackmap.clear();
	stackmap.append(cpindex);
	getParentCPs(cpindex,stackmap);
	getChildCPs(cpindex,stackmap);
}

inline HIntMap *HSplineArray::getCPStack(unsigned cpindex) {
	unsigned vi = drillForVertexIndex(cpindex);
	HNeighbor *n = stackList->get(vi);
	return &n->cpids;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds all parent instances of a cp
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::getParentCPs(unsigned cpindex, HIntMap &stackmap) {
	HCP *cp = getCP(cpindex);
	while (cp->flags.kHasParent) {
		cp = getParentCP(cp->cpindex);
		stackmap.append(cp->cpindex);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds all child instances of a cp
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::getChildCPs(unsigned cpindex, HIntMap &stackmap) {
	HCP *cp = getCP(cpindex);
	while (cp->flags.kHasChild) {
		cp = getChildCP(cp->cpindex);
		stackmap.append(cp->cpindex);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds first parent instance of a cp
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HCP *HSplineArray::getParentCP(unsigned cpidx) {
	HCP *cp = getCP(cpidx);
	if (cp->flags.kHasParent)
		return this->getCP(cp->parentIndex);
	else
		return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// finds first child instance of a cp
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HCP *HSplineArray::getChildCP(unsigned cpidx) {
	HCP *cp = getCP(cpidx);
	if (cp->flags.kHasChild)
		return this->getCP(cp->childIndex);
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// this doesn't use the instance table to traverse children cps
// this method is okay for traversing children, but not parents
// WARNING: this function was written to be used with setupCPInfo()
//////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void HSplineArray::getChildStack(unsigned cpindex,std::vector<int> &stack) {
	stack.clear();
	HCP *cp = getCP(cpindex);
	while (cp->flags.kIsInstance) {
		stack.push_back(cp->instanceIndex);
		cp = getCP(cp->instanceIndex);
	}
}