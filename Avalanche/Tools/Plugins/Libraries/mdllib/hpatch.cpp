//////////////////////////////////////////////////////////////////////////////////////////
//
// HPATCH CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hsplinearray.h"
#include "hpatch.h"
#include "hmacros.h"
#include "hstatus.h"
#include "hmatrix44.h"
#include "hvector2d.h"
#include "hpatchvectors2d.h"
#include "hintmap.h"
#include "hpolyarray.h"
#include "mdlobject.h"

#define ROW 0
#define COLUMN 1

HPatch::HPatch(){
	memset(&flags,0,sizeof(HPatch::pflags));
	attrIndex = 0;
	patchIndex = 0;
	patchArray = NULL;
}

HPatch::HPatch(const HPatch &other) {
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	cpid1 = other.cpid1;
	cpid2 = other.cpid2;
	patchIndex = other.patchIndex;
	attrIndex = other.attrIndex;
	patchArray = other.patchArray;
	patchVectors = other.patchVectors;
	decalIndicies = other.decalIndicies;
	groupIndicies = other.groupIndicies;
	cpvIndicies = other.cpvIndicies;
}

HPatch::~HPatch(){}

void HPatch::setFlagsFromOldFlags(unsigned long flags) {
	struct {
		unsigned long       m_is5point           : 1,
							m_ischeckednormal    : 1,
							m_isflipnormal       : 1,
							m_isflipp1           : 1,
							m_isflipp2           : 1,
							m_isflipp3           : 1,
							m_isflipp4           : 1,
							m_isdisplace         : 1,
							m_groupimagerotation : 2,
							m_isflipp5           : 1,
							m_isfractal          : 1,
							m_ishook1            : 1,
							m_ishook2            : 1,
							m_ishook3            : 1,
							m_ishook4            : 1,
							m_isusehook1         : 1,
							m_isusehook2         : 1,
							m_isusehook3         : 1,
							m_isusehook4         : 1,
							m_isendhook1         : 1,
							m_isendhook2         : 1,
							m_isendhook3         : 1,
							m_isendhook4         : 1,
							m_ishookdata1        : 1,
							m_ishookdata2        : 1,
							m_ishookdata3        : 1,
							m_ishookdata4        : 1,
							m_isdependent        : 1,
							m_isshaded           : 1,
							m_isbound            : 1,
							m_isvector           : 1; // 32
	} oldflags;

	HPatch::pflags n;
	memset(&n,0,sizeof(HPatch::pflags));
	memcpy(&oldflags,&flags,sizeof(unsigned long));
	
	if (oldflags.m_is5point)
		n.kIs5point = 1;

	n.kGroupimagerotation = oldflags.m_groupimagerotation;
	setFlags(n);
}

void HPatch::setFlags(unsigned long flags) {
	memcpy(&this->flags,&flags,sizeof(unsigned long));
}

void HPatch::setFlags(HPatch::pflags flags) {
	memcpy(&this->flags,&flags,sizeof(unsigned long));
}

unsigned long HPatch::getFlags() {
	unsigned long flags;
	memcpy(&flags,&this->flags,sizeof(unsigned long));
	return flags;
}

//	SEGMENTS
//	   0		  D          3
//	    +-------------------+
//	    |					| 
//	 	|					| 
//		|					| 
//		|					| 
//	  A	|					| B
//		|					|
//		|					|
//		|					|
//		|					|
//		|					|		
//		+-------------------+
//	   1	 	  C          2

// PATCHVECTORS
//	   0	  1       2      3	            
//	    +-------------------+
//		|					|
//	  	|					|
//	  4	|	  5       6     | 7
//		|					|
//	  	|					| 
//		|					|
//	  8	|	  9		  10	| 11
//		|					|
//		|					|
//		|					|		
//		+-------------------+
//	  12     13       14     15


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MAIN METHOD FOR CALCULATING TANGENTS AND ASSIGNING PATCH VECTORS
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void HPatch::assignAdjacentPatch(std::vector<int> &patchIndicies,HSplineSegment &seg) {
	for (unsigned i = 0; i < patchIndicies.size(); i++) {
		if (patchIndicies[i] != this->patchIndex)
			seg.adj_patchIndicies.push_back(patchIndicies[i]);
	}
	if (seg.adj_patchIndicies.size() == 0) {
		if ((!seg.flags.k5ptHookShortSegment) && (!seg.flags.k5ptHookLongSegment) &&
			(!seg.flags.kHookShortSegment) && (!seg.flags.kHookLongSegment)) {
			seg.flags.kBorderSegment = 1;
		}
	}
	return;
}

HSplineSegment HPatch::getSplineSegment(unsigned side) {
	unsigned cpindex0,cpindex1,cpvindex0,cpvindex1;
	int order5pt[] = {0,1,2,3,4,0};
	int order4pt[] = {0,1,2,3,0};
	std::vector<int> patchIndicies;

	cpindex0 = this->cpid1[side];
	cpindex1 = this->cpid2[side];

	if (this->flags.kIs5point) {
		cpvindex0 = this->cpvIndicies[order5pt[side]];
		cpvindex1 = this->cpvIndicies[order5pt[side+1]];
	}
	else {
		cpvindex0 = this->cpvIndicies[order4pt[side]];
		cpvindex1 = this->cpvIndicies[order4pt[side+1]];
	}

	HSplineSegment seg;
	HSplineArray *splines = &patchArray->model->splineArray;
	seg = splines->getSplineSegment(cpindex0,cpindex1,patchIndicies);
	assignAdjacentPatch(patchIndicies,seg);

	bool found = 0;
	for (unsigned i = 0; i < patchIndicies.size(); i++) {
		if (patchIndicies[i] != this->patchIndex) {
			found = true;
			break;
		}
	}
	if (found) {
		HPatch *test = this->patchArray->get(patchIndicies[i]);
		if (test->flags.kIs5point)
			seg.flags.k5ptHookLongSegment=1;
	}

	return seg;
}

void HPatch::calculateTangents() {
	bool foundPatchHook = false;
	UIFlags *ui_flags = &patchArray->model->ui_flags;

	HSplineSegment segA = getSplineSegment(0);
	HSplineSegment segB = getSplineSegment(1);
	HSplineSegment segC = getSplineSegment(2);
	HSplineSegment segD = getSplineSegment(3);
	HSplineSegment segE;
	if (this->flags.kIs5point)
		segE = getSplineSegment(4);

	if (this->flags.kIs5point) {
		calc5PointPatchVectors(segA,segB,segC,segD,segE);
	}
	else {
		segA.finalize();
		segB.finalize();
		segC.finalize();
		segD.finalize();
		setPatchVectors(false,-1,segA,segB,segC,segD);
	}
}

bool HPatch::findAdjacentEdgeOnPatch(HPatch *adj_patch,unsigned &index)
{
	HPatchVectors *pvect = adj_patch->patchVectors.get(0);
	for (unsigned j = 0; j < 4; j++)
	{
		HTessEdge *tessedge = patchArray->tessedges.get(pvect->tessedges[j]);

		if ((tessedge->flags.kHasEdgeVertices)&&(!tessedge->flags.kIsBorder))
		{
			for (unsigned k = 0; k < tessedge->adj_patchIndicies.size(); k++)
			{
				if (tessedge->adj_patchIndicies[k] == this->patchIndex)
				{
					index = j;
					return true;
				}
			}
		}
	}
	return false;
}

void HPatch::findAdjacentEdgesOnPatch(HPatch *adj_patch,std::vector<int> &pvindicies,std::vector<int> &eindicies) {
	eindicies.clear();
	pvindicies.clear();
	for (unsigned subpatch = 0; subpatch < adj_patch->patchVectors.length(); subpatch++) {
		HPatchVectors *pvect = adj_patch->patchVectors.get(subpatch);
		for (unsigned j = 0; j < 4; j++) {
			if ((patchArray->tessedges[pvect->tessedges[j]].flags.kHasEdgeVertices)&&(!patchArray->tessedges[pvect->tessedges[j]].flags.kIsBorder)) {
				for (unsigned k = 0; k < patchArray->tessedges[pvect->tessedges[j]].adj_patchIndicies.size(); k++) {
					if (patchArray->tessedges[pvect->tessedges[j]].adj_patchIndicies[k] == this->patchIndex) {
						eindicies.push_back(j);
						pvindicies.push_back(subpatch);
						break;
					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// derives corner cps of patch
///////////////////////////////////////////////////////////////////////////////////////////////////

void HPatch::derivePatchSegments() {
	bool found_connection = false;
	unsigned cpindex0,cpindex1;
	unsigned order[] = {0,1,2,3,0};
	unsigned order2[] = {0,1,2,3,4,0};
	HIntMap *cpstack = NULL,*cpstack0 = NULL,*cpstack1 = NULL;
	HIntMap **cpstacks = NULL;
	unsigned vi0,vi1;
	unsigned numpoints = cpid1.size();
	HCP *cp0,*cp1;

	HHook *hook;
	bool foundhook0,foundhook1,foundhooksegment = false;

	bool is_hookbase,is_hookbaseneighbor;
	unsigned hbi,hbni;

	HSplineArray *splines = &patchArray->model->splineArray;

	unsigned debug[4];
	debug[0] = cpid1[0];
	debug[1] = cpid1[1];
	debug[2] = cpid1[2];
	debug[3] = cpid1[3];

	if (this->flags.kIs3point)
		numpoints = 3;
	else if (this->flags.kIs5point)
		numpoints = 5;
	else
		numpoints = 4;

	// GET CP STACKS FOR POINTS OF PATCH
	/////////////////////////////////////////////////////////////////////////

	cpstacks = new HIntMap*[cpid1.size()];
	for (unsigned n = 0; n < cpid1.size(); n++)
		cpstacks[n] = splines->getCPStack(cpid1[n]);

	/////////////////////////////////////////////////////////////////////////

	for (unsigned i = 0; i < numpoints; i++) {
		if (numpoints != 5) {
			cpstack0 = cpstacks[order[i]];
			cpstack1 = cpstacks[order[i+1]];
			cpindex0 = cpid1[order[i]];
			cpindex1 = cpid1[order[i+1]];
		}
		else {
			cpstack0 = cpstacks[order2[i]];
			cpstack1 = cpstacks[order2[i+1]];
			cpindex0 = cpid1[order2[i]];
			cpindex1 = cpid1[order2[i+1]];
		}

		cp0 = splines->getCP(cpindex0);
		cp1 = splines->getCP(cpindex1);

		foundhook0 = false;
		foundhook1 = false;
		if (cp0->flags.kIsHook) {
			hook = splines->hookList.get(cp0->hookIndex);
			foundhook0 = true;
		}
		else if (cp1->flags.kIsHook) {
			hook = splines->hookList.get(cp1->hookIndex);
			foundhook1 = true;
		}
		foundhooksegment = false;
		if ((foundhook0)||(foundhook1)) {
			if (foundhook0) {
				is_hookbase = splines->drillForHookbase(*cpstack1,hbi,hook);
				is_hookbaseneighbor = splines->drillForHookbaseNeighbor(*cpstack1,hbni,hook);
				if (is_hookbase) {
					cp1 = splines->getCP(hbi);
					foundhooksegment = true;
				}
				if (is_hookbaseneighbor) {
					cp1 = splines->getCP(hbni);
					foundhooksegment = true;
				}
			}
			if (foundhook1) {
				is_hookbase = splines->drillForHookbase(*cpstack0,hbi,hook);
				is_hookbaseneighbor = splines->drillForHookbaseNeighbor(*cpstack0,hbni,hook);
				if (is_hookbase) {
					cp0 = splines->getCP(hbi);
					foundhooksegment = true;
				}
				if (is_hookbaseneighbor) {
					cp0 = splines->getCP(hbni);
					foundhooksegment = true;
				}
			}
		}
		if (!foundhooksegment) {
			HCP *cpA[1],*cpB[1];
			found_connection = findConnection(cpstack0,cpstack1,cpA,cpB);
			if (!found_connection) {
				THROW_FATAL_ERROR("Can't find connection.");
			}
			cp0 = *cpA;
			cp1 = *cpB;
		}
		this->cpid1[i] = cp0->cpindex;
		this->cpid2[i] = cp1->cpindex;

		if (numpoints == 5) {
			cp0->flags.kIs5PointPatchVertex = 1;
			cp1->flags.kIs5PointPatchVertex = 1;
		}

		vi0 = splines->drillForVertexIndex(cp0);
		if (cp1->flags.kIsLoop)
			vi1 = splines->drillForVertexIndex(cp1->head());
		else
			vi1 = splines->drillForVertexIndex(cp1);

		this->cpvIndicies.push_back(vi0);

		splines->neighbors[vi0].cpids.append(this->cpid1[i]);
		splines->neighbors[vi1].cpids.append(this->cpid2[i]);
	}
	// if 3 point patch fold 4 point patch
	if (numpoints == 3) {
		this->cpid1[3] = this->cpid1[0];
		this->cpid2[3] = this->cpid1[0];
		this->cpvIndicies.push_back(this->cpvIndicies[0]);
	}

	HNeighbor *np;
	for (unsigned j = 0; j < numpoints; j++) {
		np = splines->neighbors.get(cpvIndicies[j]);
		np->calcNeighbor(j,*this);
	}
	delete[] cpstacks;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// finds connection between two cp stacks
///////////////////////////////////////////////////////////////////////////////////////////////////

bool HPatch::findConnection(HIntMap *cpstack0,HIntMap *cpstack1,HCP **cp0,HCP **cp1) {
	HCP *cpnext, *cpprevious;
	HCP *cp;
	bool exists = false;
	unsigned cpidx;
	HIntMap *stacknext = NULL,*stackprevious = NULL;
	unsigned j;
	HSplineArray *splines = &patchArray->model->splineArray;

	for (unsigned i = 0; i < cpstack0->length(); i++) {	
		cp = splines->getCP(cpstack0->getKey(i));
//		if tail,loop
		if (cp->flags.kIsTail) {
			if (cp->flags.kIsLoop)
				cpnext = cp->head();
			else
				cpnext = cp;
		}
		else {
			if (cp->flags.kIsHashLoop)
				cpnext = cp->head();
			else
				cpnext = cp->next();
		}

		stacknext = splines->getCPStack(cpnext->cpindex);
		for (j = 0; j < cpstack1->length(); j++) {
			cpidx = cpstack1->getKey(j);
			exists = stacknext->hasKey(cpidx);
			if (exists) {
				*cp0 = cp;
				*cp1 = cpnext;
				return true;
			}
		}
		//If at head, loop
		if (cp->flags.kIsHead) {
			if ((cp->tail())->flags.kIsLoop)
				cpprevious = (cp->tail())->previous();
			else
				cpprevious = cp;
		}
		else
			cpprevious = cp->previous();

		stackprevious = splines->getCPStack(cpprevious->cpindex);
		for (j = 0; j < cpstack1->length(); j++) {
			cpidx = cpstack1->getKey(j);
			exists = stackprevious->hasKey(cpidx);
			if (exists) {
				*cp0 = cp;
				*cp1 = cpprevious;
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	METHOD FOR CALCULATING PATCH VECTORS
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// STAR METHOD - BEST RESULTS
void HPatch::calc5PointPatchVectors(HSplineSegment &segA,HSplineSegment &segB,HSplineSegment &segC,
											  HSplineSegment &segD,HSplineSegment &segE) {
	segA.finalize();
	segB.finalize();
	segC.finalize();
	segD.finalize();
	segE.finalize();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Get midpoints of sides
	///////////////////////////////////////////////////////////////////////////////////////////////////

	HSplineSegment segA1,segA2,segB1,segB2,segC1,segC2,segD1,segD2,segE1,segE2;

	segA.splitSplineSegment(segA1,segA2);
	segB.splitSplineSegment(segB1,segB2);
	segC.splitSplineSegment(segC1,segC2);
	segD.splitSplineSegment(segD1,segD2);
	segE.splitSplineSegment(segE1,segE2);

	HSplineArray *splines = &this->patchArray->model->splineArray;
	std::vector<int> patchIndicies;

	segA1.flags.k5ptHookShortSegment = 1;
	segA2.flags.k5ptHookShortSegment = 1;
	splines->getSegmentPatchAssociations(cpvIndicies[0],cpvIndicies[1],patchIndicies);
	assignAdjacentPatch(patchIndicies,segA1);
	assignAdjacentPatch(patchIndicies,segA2);
	
	segB1.flags.k5ptHookShortSegment = 1;
	segB2.flags.k5ptHookShortSegment = 1;
	patchIndicies.clear();
	splines->getSegmentPatchAssociations(cpvIndicies[1],cpvIndicies[2],patchIndicies);
	assignAdjacentPatch(patchIndicies,segB1);
	assignAdjacentPatch(patchIndicies,segB2);

	segC1.flags.k5ptHookShortSegment = 1;
	segC2.flags.k5ptHookShortSegment = 1;
	patchIndicies.clear();
	splines->getSegmentPatchAssociations(cpvIndicies[2],cpvIndicies[3],patchIndicies);
	assignAdjacentPatch(patchIndicies,segC1);
	assignAdjacentPatch(patchIndicies,segC2);

	segD1.flags.k5ptHookShortSegment = 1;
	segD2.flags.k5ptHookShortSegment = 1;
	patchIndicies.clear();
	splines->getSegmentPatchAssociations(cpvIndicies[3],cpvIndicies[4],patchIndicies);
	assignAdjacentPatch(patchIndicies,segD1);
	assignAdjacentPatch(patchIndicies,segD2);

	segE1.flags.k5ptHookShortSegment = 1;
	segE2.flags.k5ptHookShortSegment = 1;
	patchIndicies.clear();
	splines->getSegmentPatchAssociations(cpvIndicies[4],cpvIndicies[0],patchIndicies);
	assignAdjacentPatch(patchIndicies,segE1);
	assignAdjacentPatch(patchIndicies,segE2);

	segA1.finalize(); segA2.finalize();
	segB1.finalize(); segB2.finalize();
	segC1.finalize(); segC2.finalize();
	segD1.finalize(); segD2.finalize();
	segE1.finalize(); segE2.finalize();

	HVector midA = segA1.v1;
	HVector midB = segB1.v1;
	HVector midC = segC1.v1;
	HVector midD = segD1.v1;
	HVector midE = segE1.v1;

	HVector plane;
	HVector midpoint = get5PointPatchMidPoint(segA1,segB1,segC1,segD1,segE1,plane);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate interior edge vectors
	///////////////////////////////////////////////////////////////////////////////////////////////////

	HVector iedge0 = (midA - midpoint).negate();
	HVector iedge1 = (midB - midpoint).negate();
	HVector iedge2 = (midC - midpoint).negate();
	HVector iedge3 = (midD - midpoint).negate();
	HVector iedge4 = (midE - midpoint).negate();

	HVector oedge0 = (midpoint - midA);
	HVector oedge1 = (midpoint - midB);
	HVector oedge2 = (midpoint - midC);
	HVector oedge3 = (midpoint - midD);
	HVector oedge4 = (midpoint - midE);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate 5 point patch interior edges, interior tangents
	///////////////////////////////////////////////////////////////////////////////////////////////////

	double ledge0 = (iedge0.normalizeLength())/3;
	double ledge1 = (iedge1.normalizeLength())/3;
	double ledge2 = (iedge2.normalizeLength())/3;
	double ledge3 = (iedge3.normalizeLength())/3;
	double ledge4 = (iedge4.normalizeLength())/3;

	HVector it_edge0 = (plane^(iedge0^plane)).normalize() * ledge0;
	HVector it_edge1 = (plane^(iedge1^plane)).normalize() * ledge1;
	HVector it_edge2 = (plane^(iedge2^plane)).normalize() * ledge2;
	HVector it_edge3 = (plane^(iedge3^plane)).normalize() * ledge3;
	HVector it_edge4 = (plane^(iedge4^plane)).normalize() * ledge4;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate 5 point patch iterior edges, exterior tangents
	///////////////////////////////////////////////////////////////////////////////////////////////////

	// Average side vectors to get corner normals
	///////////////////////////////////////////////////////////////////////////////////////////////////

	HVector cnEA = (segE2.cv1-segE2.v1) ^ (segA1.cv0 - segA1.v0);
	HVector cnAB = (segA2.cv1-segA2.v1) ^ (segB1.cv0 - segB1.v0);
	HVector cnBC = (segB2.cv1-segB2.v1) ^ (segC1.cv0 - segC1.v0);
	HVector cnCD = (segC2.cv1-segC2.v1) ^ (segD1.cv0 - segD1.v0);
	HVector cnDE = (segD2.cv1-segD2.v1) ^ (segE1.cv0 - segE1.v0);

	// translate side segment midpoints outgoing tangents to plane based on corner normals
	///////////////////////////////////////////////////////////////////////////////////////////////////

	HVector cnEA_AB = (cnEA + cnAB) * 0.5;
	HVector cnAB_BC = (cnAB + cnBC) * 0.5;
	HVector cnBC_CD = (cnBC + cnCD) * 0.5;
	HVector cnCD_DE = (cnCD + cnDE) * 0.5;
	HVector cnDE_EA = (cnDE + cnEA) * 0.5;

	double cnEA_ABl = (oedge0.normalizeLength())/3;
	double cnAB_BCl = (oedge1.normalizeLength())/3;
	double cnBC_CDl = (oedge2.normalizeLength())/3;
	double cnCD_DEl = (oedge3.normalizeLength())/3;
	double cnDE_EAl = (oedge4.normalizeLength())/3;
	
	HVector ot_edge0 = (cnEA_AB^(oedge0^cnEA_AB)).normalize() * cnEA_ABl;
	HVector ot_edge1 = (cnAB_BC^(oedge1^cnAB_BC)).normalize() * cnAB_BCl;
	HVector ot_edge2 = (cnBC_CD^(oedge2^cnBC_CD)).normalize() * cnBC_CDl;
	HVector ot_edge3 = (cnCD_DE^(oedge3^cnCD_DE)).normalize() * cnCD_DEl;
	HVector ot_edge4 = (cnDE_EA^(oedge4^cnDE_EA)).normalize() * cnDE_EAl;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// create segments of calculated data
	///////////////////////////////////////////////////////////////////////////////////////////////////
	HSplineSegment temp(midA,ot_edge0,it_edge0,midpoint);

	temp.reverse();

	HSplineSegment ssegA0 = temp;
	HSplineSegment ssegA1 = segA2;
	HSplineSegment ssegA2 = segB1;
	HSplineSegment ssegA3(midB,ot_edge1,it_edge1,midpoint);

	temp = ssegA3;
	temp.reverse();

	HSplineSegment ssegB0 = temp;
	HSplineSegment ssegB1 = segB2;
	HSplineSegment ssegB2 = segC1;
	HSplineSegment ssegB3(midC,ot_edge2,it_edge2,midpoint);

	temp = ssegB3;
	temp.reverse();

	HSplineSegment ssegC0 = temp;
	HSplineSegment ssegC1 = segC2;
	HSplineSegment ssegC2 = segD1;
	HSplineSegment ssegC3(midD,ot_edge3,it_edge3,midpoint);

	temp = ssegC3;
	temp.reverse();

	HSplineSegment ssegD0 = temp;
	HSplineSegment ssegD1 = segD2;
	HSplineSegment ssegD2 = segE1;
	HSplineSegment ssegD3(midE,ot_edge4,it_edge4,midpoint);

	temp = ssegD3;
	temp.reverse();

	HSplineSegment ssegE0 = temp;
	HSplineSegment ssegE1 = segE2;
	HSplineSegment ssegE2 = segA1;
	HSplineSegment ssegE3(midA,ot_edge0,it_edge0,midpoint);

	ssegA0.finalize(); ssegA3.finalize();
	ssegB0.finalize(); ssegB3.finalize();
	ssegC0.finalize(); ssegC3.finalize();
	ssegD0.finalize(); ssegD3.finalize();
	ssegE0.finalize(); ssegE3.finalize();

	setPatchVectors(false,0,ssegA0,ssegA1,ssegA2,ssegA3);
	setPatchVectors(false,1,ssegB0,ssegB1,ssegB2,ssegB3);
	setPatchVectors(false,2,ssegC0,ssegC1,ssegC2,ssegC3);
	setPatchVectors(false,3,ssegD0,ssegD1,ssegD2,ssegD3);
	setPatchVectors(false,4,ssegE0,ssegE1,ssegE2,ssegE3);
}


HVector HPatch::calcNormalFrom5Vertices(HVector &v0, HVector &v1, HVector &v2, HVector &v3, HVector &v4) {
	HVector pseudonormA = (v0 ^ v1).normalize();
	HVector pseudonormB = (v1 ^ v2).normalize();
	HVector pseudonormC = (v2 ^ v3).normalize();
	HVector pseudonormD = (v3 ^ v4).normalize();
	HVector pseudonormE = (v4 ^ v0).normalize();
	return (pseudonormA + pseudonormB + pseudonormC + pseudonormD + pseudonormE).normalize();
}

HVector HPatch::calcNormalFrom4Vertices(HVector &v0, HVector &v1, HVector &v2, HVector &v3) {
	HVector pseudonormA = (v0 ^ v1).normalize();
	HVector pseudonormB = (v1 ^ v2).normalize();
	HVector pseudonormC = (v2 ^ v3).normalize();
	HVector pseudonormD = (v3 ^ v0).normalize();
	return (pseudonormA + pseudonormB + pseudonormC + pseudonormD).normalize();
}

void HPatch::setPatchVectors(unsigned is3point,
							 int subpatchid,
							 const HSplineSegment &segA,
							 const HSplineSegment &segB,
							 const HSplineSegment &segC,
							 const HSplineSegment &segD)
{

	HPatchVectors *pVects =	&this->patchVectors[this->patchVectors.append(HPatchVectors())];

	pVects->tessedges[0] = patchArray->tessedges.append(HTessEdge());
	pVects->tessedges[1] = patchArray->tessedges.append(HTessEdge());
	pVects->tessedges[2] = patchArray->tessedges.append(HTessEdge());
	pVects->tessedges[3] = patchArray->tessedges.append(HTessEdge());

	HVector pVectors[16];
	setEdgePatchVectors(0,segA,pVectors);
	setEdgePatchVectors(1,segB,pVectors);
	setEdgePatchVectors(2,segC,pVectors);
	setEdgePatchVectors(3,segD,pVectors);
	calculateInteriorTangents(pVectors);

	*pVects = pVectors;

	pVects->patchIndex = this->patchIndex;

	if (is3point)
		pVects->flags.kIs3point = 1;

	if (subpatchid >= 0)
	{
		pVects->flags.k5ptSubPatch = 1;
		pVects->subPatchIndex = subpatchid;
	}
	
	patchArray->tessedges[pVects->tessedges[0]].flags.kIsBorder = segA.flags.kBorderSegment;
	patchArray->tessedges[pVects->tessedges[1]].flags.kIsBorder = segB.flags.kBorderSegment;
	patchArray->tessedges[pVects->tessedges[2]].flags.kIsBorder = segC.flags.kBorderSegment;
	patchArray->tessedges[pVects->tessedges[3]].flags.kIsBorder = segD.flags.kBorderSegment;

	patchArray->tessedges[pVects->tessedges[0]].adj_patchIndicies = segA.adj_patchIndicies;
	patchArray->tessedges[pVects->tessedges[1]].adj_patchIndicies = segB.adj_patchIndicies;
	patchArray->tessedges[pVects->tessedges[2]].adj_patchIndicies = segC.adj_patchIndicies;
	patchArray->tessedges[pVects->tessedges[3]].adj_patchIndicies = segD.adj_patchIndicies;

	if (segA.flags.kHookShortSegment)
		patchArray->tessedges[pVects->tessedges[0]].flags.kIsHookShortSegment = 1;
	if (segA.flags.kHookLongSegment)
		patchArray->tessedges[pVects->tessedges[0]].flags.kIsHookLongSegment = 1;
	if (segA.flags.k5ptHookShortSegment)
		patchArray->tessedges[pVects->tessedges[0]].flags.kIsHookShortSegment = 1;
	if (segA.flags.k5ptHookLongSegment)
		patchArray->tessedges[pVects->tessedges[0]].flags.kIsHookLongSegment = 1;

	if (segB.flags.kHookShortSegment)
		patchArray->tessedges[pVects->tessedges[1]].flags.kIsHookShortSegment = 1;
	if (segB.flags.kHookLongSegment)
		patchArray->tessedges[pVects->tessedges[1]].flags.kIsHookLongSegment = 1;
	if (segB.flags.k5ptHookShortSegment)
		patchArray->tessedges[pVects->tessedges[1]].flags.kIsHookShortSegment = 1;
	if (segB.flags.k5ptHookLongSegment)
		patchArray->tessedges[pVects->tessedges[1]].flags.kIsHookLongSegment = 1;

	if (segC.flags.kHookShortSegment)
		patchArray->tessedges[pVects->tessedges[2]].flags.kIsHookShortSegment = 1;
	if (segC.flags.kHookLongSegment)
		patchArray->tessedges[pVects->tessedges[2]].flags.kIsHookLongSegment = 1;
	if (segC.flags.k5ptHookShortSegment)
		patchArray->tessedges[pVects->tessedges[2]].flags.kIsHookShortSegment = 1;
	if (segC.flags.k5ptHookLongSegment)
		patchArray->tessedges[pVects->tessedges[2]].flags.kIsHookLongSegment = 1;

	if (segD.flags.kHookShortSegment)
		patchArray->tessedges[pVects->tessedges[3]].flags.kIsHookShortSegment = 1;
	if (segD.flags.kHookLongSegment)
		patchArray->tessedges[pVects->tessedges[3]].flags.kIsHookLongSegment = 1;
	if (segD.flags.k5ptHookShortSegment)
		patchArray->tessedges[pVects->tessedges[3]].flags.kIsHookShortSegment = 1;
	if (segD.flags.k5ptHookLongSegment)
		patchArray->tessedges[pVects->tessedges[3]].flags.kIsHookLongSegment = 1;

	patchArray->tessedges[pVects->tessedges[0]].flags.kHardEdge = segA.flags.kHardEdge;
	patchArray->tessedges[pVects->tessedges[1]].flags.kHardEdge = segB.flags.kHardEdge;
	patchArray->tessedges[pVects->tessedges[2]].flags.kHardEdge = segC.flags.kHardEdge;
	patchArray->tessedges[pVects->tessedges[3]].flags.kHardEdge = segD.flags.kHardEdge;
}

HVector HPatch::get5PointPatchMidPoint(const HSplineSegment &seg,const HSplineSegment &seg1,const HSplineSegment &seg2,
									   const HSplineSegment &seg3,const HSplineSegment &seg4,HVector &planenormal) {
	HVector midpoint = (seg.v1 + seg1.v1 + seg2.v1 + seg3.v1 + seg4.v1) * 0.2f;
	planenormal = calcNormalFrom5Vertices(seg.v1-midpoint,seg1.v1-midpoint,
		seg2.v1-midpoint,seg3.v1-midpoint,seg4.v1-midpoint);
	raiseMidPoint(seg.v0,seg1.v0,seg2.v0,seg3.v0,seg4.v0,midpoint,planenormal);
	return midpoint;
}

void HPatch::raiseMidPoint( const HVector &p1, const HVector &p2, const HVector &p3, const HVector &p4, const HVector &p5,
				    HVector &midpoint, HVector n ) {  

   HVector t1 = p2;    
   t1 -= p1;
   HVector t2 = p3;
   t2 -= p2;
   HVector t3 = p4;
   t3 -= p3;
   HVector t4 = p5;
   t4 -= p4;
   HVector t5 = p1;
   t5 -= p5;

   HVector temp = t1 ^ t2;
   temp.normalize();
   HVector h = midpoint;
   h -= p2;
   HVector totaln = n;
   float length = (float)h.normalizeLength();
   length *= (float)h.dot( temp );
   totaln *= length;

   temp = t2 ^ t3;
   temp.normalize();
   h = midpoint;
   h -= p3;
   HVector tempn = n;
   length = (float)h.normalizeLength();
   length *= (float)h.dot( temp );
   tempn *= length;
   totaln += tempn;

   temp = t3 ^ t4;
   temp.normalize();
   h = midpoint;
   h -= p4;
   tempn = n;
   length = (float)h.normalizeLength();
   length *= (float)h.dot( temp );
   tempn *= length;
   totaln += tempn;

   temp = t4 ^ t5;
   temp.normalize();
   h = midpoint;
   h -= p5;
   tempn = n;
   length = (float)h.normalizeLength();
   length *= (float)h.dot( temp );
   tempn *= length;
   totaln += tempn;

   temp = t5 ^ t1;
   temp.normalize();
   h = midpoint;
   h -= p1;
   tempn = n;
   length = (float)h.normalizeLength();
   length *= (float)h.dot( temp );
   tempn *= length;
   totaln += tempn;

   totaln /= 5.;
   midpoint += totaln;
}

//	SIDE 0 = A, 1 = B, 2 = C, 3 = D, 4 = E
void HPatch::setEdgePatchVectors(unsigned side, const HSplineSegment &seg, HVector *pVectors) {
	std::vector<int> indicies;

	indicies = get4SidedPatchVectorIndicies(side);
	pVectors[indicies[0]] = seg.v0;
	pVectors[indicies[1]] = seg.cv0;
	pVectors[indicies[2]] = seg.cv1;
	pVectors[indicies[3]] = seg.v1;
}


std::vector<int> HPatch::get4SidedPatchVectorIndicies(unsigned side) {
	std::vector<int> indicies;

	if (side == 0) {
		indicies.push_back(0);
		indicies.push_back(4);
		indicies.push_back(8);
		indicies.push_back(12);
	}
	else if (side == 1) {
		indicies.push_back(12);
		indicies.push_back(13);
		indicies.push_back(14);
		indicies.push_back(15);
	}
	else if (side == 2) {
		indicies.push_back(15);
		indicies.push_back(11);
		indicies.push_back(7);
		indicies.push_back(3);
	}
	else if (side == 3) {
		indicies.push_back(3);
		indicies.push_back(2);
		indicies.push_back(1);
		indicies.push_back(0);
	}
	return indicies;
}

//	SIDE 0 = A, 1 = B, 2 = C, 3 = D
HSplineSegment HPatch::getSegmentFromSide(unsigned side) {
	HSplineSegment seg;
	std::vector<int> indicies = get4SidedPatchVectorIndicies(side);
	return HSplineSegment(indicies[0],indicies[1],indicies[2],indicies[3]);
}

HVector2D HPatch::calcUVOnPatch(double s, double t, unsigned nPatch) {
	HVector2D repeat,uv;
	HVector2D a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
	HPatchVectors2D patchVectors;
	UIFlags *ui_flags = &patchArray->model->ui_flags;

	patchVectors.pv.resize(16);

	patchVectors.pv[0] = HVector2D(0,1);
	patchVectors.pv[3] = HVector2D(1,1);
	patchVectors.pv[12] = HVector2D(0,0);
	patchVectors.pv[15] = HVector2D(1,0);

	repeat.x = 1; repeat.y = 1;

	HDecal *decal;
	HStamp *stamp;
	HPatchVectors *pvect;

	patchVectors.rotateElementsClockwise(this->flags.kGroupimagerotation);

	bool exists; unsigned idx;

	HDecalArray *decals = &this->patchArray->decalArray;
	HStampArray *stamps = &this->patchArray->stampArray;

	if (this->hasDecals()) {
		decal = decals->get(this->decalIndicies.back());
		if (decal->hasStamps()) {
			for (unsigned i = 0; i < decal->stampIndicies.size(); i++) {
				stamp = stamps->get(decal->stampIndicies[i]);
				exists = stamp->patchIndicies.hasKey(this->patchIndex);
				if (exists) {
					idx = stamp->patchIndicies.getValue(this->patchIndex);
					if (this->flags.kIs5point) {
						pvect = stamp->uvs.get(idx);
						uv = calcUVOn5Point(s,t,nPatch,repeat,pvect);
					}
					else {
						pvect = stamp->uvs.get(idx);
						patchVectors.pv[0] = HVector2D(pvect->pv[0].x,-pvect->pv[0].y);
						patchVectors.pv[1] = HVector2D(pvect->pv[1].x,-pvect->pv[1].y);
						patchVectors.pv[2] = HVector2D(pvect->pv[2].x,-pvect->pv[2].y);
						patchVectors.pv[3] = HVector2D(pvect->pv[3].x,-pvect->pv[3].y);
						patchVectors.pv[4] = HVector2D(pvect->pv[11].x,-pvect->pv[11].y);
						patchVectors.pv[7] = HVector2D(pvect->pv[4].x,-pvect->pv[4].y);
						patchVectors.pv[8] = HVector2D(pvect->pv[10].x,-pvect->pv[10].y);
						patchVectors.pv[11] = HVector2D(pvect->pv[5].x,-pvect->pv[5].y);
						patchVectors.pv[12] = HVector2D(pvect->pv[9].x,-pvect->pv[9].y);
						patchVectors.pv[13] = HVector2D(pvect->pv[8].x,-pvect->pv[8].y);
						patchVectors.pv[14] = HVector2D(pvect->pv[7].x,-pvect->pv[7].y);
						patchVectors.pv[15] = HVector2D(pvect->pv[6].x,-pvect->pv[6].y);
						uv = calcUVOn4Point(s,t,repeat,patchVectors);
					}	
				}
			}
		}
	}
	else {
		uv = calcUVOn4Point(s,t,repeat,patchVectors);
	}
	return uv;
}

bool HPatch::isVStraight(double tol) {
	HVector v0   = patchVectors[0].pv[0];
	HVector v1   = patchVectors[0].pv[12];
	HVector v2   = patchVectors[0].pv[15];
	HVector v3   = patchVectors[0].pv[3];
	HVector cv0  = patchVectors[0].pv[4];
	HVector cv1  = patchVectors[0].pv[8];
	HVector cv10 = patchVectors[0].pv[7];
	HVector cv11 = patchVectors[0].pv[11];
	HSplineSegment segA(v0,cv0,cv1,v1),segB(v2,cv11,cv10,v3);
	if (segA.isStraight(tol))
		if (segB.isStraight(tol))
			return true;
	return false;
}

bool HPatch::isUStraight(double tol) {
	HVector v0   = patchVectors[0].pv[0];
	HVector v1   = patchVectors[0].pv[12];
	HVector v2   = patchVectors[0].pv[15];
	HVector v3   = patchVectors[0].pv[3];
	HVector cv2  = patchVectors[0].pv[1];
	HVector cv5  = patchVectors[0].pv[13];
	HVector cv6  = patchVectors[0].pv[2];
	HVector cv9	 = patchVectors[0].pv[14];
	HSplineSegment segA(v3,cv6,cv2,v0),segB(v1,cv5,cv9,v2);
	if (segA.isStraight(tol))
		if (segB.isStraight(tol))
			return true;
	return false;
}

void HPatch::getUSubdivideInfo(double tol,HDoubleArray &percent) {
	HVector v0   = patchVectors[0].pv[0];
	HVector v1   = patchVectors[0].pv[12];
	HVector v2   = patchVectors[0].pv[3];
	HVector v3   = patchVectors[0].pv[15];
	HVector cv0  = patchVectors[0].pv[4];
	HVector cv1  = patchVectors[0].pv[8];
	HVector cv10 = patchVectors[0].pv[11];
	HVector cv11 = patchVectors[0].pv[7];
	
	HSplineSegment segA(v0,cv0,cv1,v1,true),segB(v2,cv11,cv10,v3,true);
	HDoubleArray A,B;
	segA.doSubdivide(tol,A);
	segB.doSubdivide(tol,B);
	percent.unify(A,B);
}

void HPatch::getVSubdivideInfo(double tol,HDoubleArray &percent) {
	HVector v0   = patchVectors[0].pv[0];
	HVector v1   = patchVectors[0].pv[12];
	HVector v2   = patchVectors[0].pv[15];
	HVector v3   = patchVectors[0].pv[3];
	HVector cv2  = patchVectors[0].pv[1];
	HVector cv5  = patchVectors[0].pv[13];
	HVector cv6  = patchVectors[0].pv[2];
	HVector cv9	 = patchVectors[0].pv[14];

	HSplineSegment segA(v0,cv2,cv6,v3,true),segB(v1,cv5,cv9,v2,true);
	HDoubleArray A,B;
	segA.doSubdivide(tol,A);
	segB.doSubdivide(tol,B);
	percent.unify(A,B);
}

void HPatch::getSubdivideInfo(double tol,HDoubleArray &upercent,HDoubleArray &vpercent) {
	getUSubdivideInfo(tol,upercent);
	getVSubdivideInfo(tol,vpercent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	patch tessellation methods
///////////////////////////////////////////////////////////////////////////////////////////////////

void HPatch::calcPolysMidpointSubdivision(HPolyArray &polygons,std::vector<int> &shaderIndicies,unsigned uspans, unsigned vspans) {
	HVector normal;
	unsigned usubdivide = uspans, vsubdivide = vspans;
	bool is3point;
	unsigned nspans = this->patchArray->model->ui_normalsamples;
	UIFlags *ui_flags = &patchArray->model->ui_flags;

	if (this->patchVectors.length() == 0) //tangents weren't calculated
		return;

//	if (ui_flags->planarcheck) {
//		if (this->isUStraight(this->patchArray->model->ui_straightnesstolerance))
//			usubdivide = 1;
//		if (this->isVStraight(this->patchArray->model->ui_straightnesstolerance))
//			vsubdivide = 1;
//	}

	unsigned numPatchVectors = this->patchVectors.length();

	HVector vtx0,vtx3;

	unsigned corner0,corner1,corner2,corner3;

	HMatrix vmat(uspans+1,vspans+1,HMatrix::INTEGER);
	HMatrix nmat(uspans+1,vspans+1,HMatrix::INTEGER);
	HMatrix uvmat(uspans+1,vspans+1,HMatrix::INTEGER);
	HMatrix nvectmat(uspans+1,vspans+1,HMatrix::VECTOR);
	
	HPolyArray *polys = patchArray->model->polygonArray;
	double nspank = getDivisionConstant((float)nspans);

	for (unsigned pvectidx = 0; pvectidx < numPatchVectors; pvectidx++) {

		std::vector<int> edgeAlist,edgeBlist,edgeClist,edgeDlist;
		edgeAlist.reserve(vsubdivide+1);
		edgeBlist.reserve(usubdivide+1);
		edgeClist.reserve(vsubdivide+1);
		edgeDlist.reserve(usubdivide+1);

		createNormalMatrix(pvectidx,nvectmat);

		createPatchMatrix(usubdivide,vsubdivide,nspans,pvectidx,nvectmat,vmat,nmat,uvmat);

		for (unsigned v = 0; v < vsubdivide; v++) {
			for (unsigned u = 0; u < usubdivide; u++) {

				corner0 = (v * (uspans+1)) + u;
				corner1 = ((v+1) * (uspans+1)) + u;
				corner2 = ((v+1) * (uspans+1)) + (u+1);
				corner3 = (v * (uspans+1)) + (u+1);

				vtx0 = polys->vertices[vmat.getInt(corner0)];
				vtx3 = polys->vertices[vmat.getInt(corner3)];

				is3point = false;
				if ((this->flags.kIs3point)&&(v == 0))
					is3point = true;

				HPoly poly;
				if (!is3point) {
					poly.vertexIndicies.resize(4);
					poly.edgeIndicies.resize(4);
					poly.uvIndicies.resize(4);
					poly.normalIndicies.resize(4);
				}
				else {
					poly.vertexIndicies.resize(3);
					poly.edgeIndicies.resize(3);
					poly.uvIndicies.resize(3);
					poly.normalIndicies.resize(3);
				}

				poly.uvIndicies[0] = uvmat.getInt(corner0);
				poly.uvIndicies[1] = uvmat.getInt(corner1);
				poly.uvIndicies[2] = uvmat.getInt(corner2);
				if (!is3point)
					poly.uvIndicies[3] = uvmat.getInt(corner3);

				poly.normalIndicies[0] = nmat.getInt(corner0);
				poly.normalIndicies[1] = nmat.getInt(corner1);
				poly.normalIndicies[2] = nmat.getInt(corner2);
				if (!is3point)
					poly.normalIndicies[3] = nmat.getInt(corner3);

				poly.vertexIndicies[0] = vmat.getInt(corner0);
				poly.vertexIndicies[1] = vmat.getInt(corner1);
				poly.vertexIndicies[2] = vmat.getInt(corner2);
				if (!is3point)
					poly.vertexIndicies[3] = vmat.getInt(corner3);

				// SETS CORNER AND EDGE FLAGS
				////////////////////////////////////////////////////////////////////////////////

				createEdgeList(u,v,usubdivide-1,vsubdivide-1,pvectidx,is3point,poly,
								edgeAlist,edgeBlist,edgeClist,edgeDlist);

				////////////////////////////////////////////////////////////////////////////////
				
				poly.patchIndex = this->patchIndex;
				poly.shaderIndicies = shaderIndicies;

				polygons.append(poly,true);
			}
		}

		mdlObject *mdl = this->patchArray->model;
		patchArray->tessedges[this->patchVectors[pvectidx].tessedges[0]].setEdgeVertices(false,edgeAlist,mdl);
		patchArray->tessedges[this->patchVectors[pvectidx].tessedges[1]].setEdgeVertices(false,edgeBlist,mdl);
		patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].setEdgeVertices(true,edgeClist,mdl);
		if (!this->flags.kIs3point)
			patchArray->tessedges[this->patchVectors[pvectidx].tessedges[3]].setEdgeVertices(true,edgeDlist,mdl);

		if (this->flags.kIs3point) {
			std::vector<int> *temp = &patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].vertexIndicies;
			(*temp)[temp->size()-1] = patchArray->tessedges[this->patchVectors[pvectidx].tessedges[0]].vertexIndicies.front();
			std::vector<int> *temp0 = &patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].normalIndicies;
			(*temp)[temp0->size()-1] = patchArray->tessedges[this->patchVectors[pvectidx].tessedges[0]].normalIndicies.front();
		}
		if (!this->flags.kIs5point) {
			HTessCorner *corner0 = this->patchArray->tesscorners->get(this->cpvIndicies[0]);
			HTessCorner *corner1 = this->patchArray->tesscorners->get(this->cpvIndicies[1]);
			HTessCorner *corner2 = this->patchArray->tesscorners->get(this->cpvIndicies[2]);
			HTessCorner	*corner3 = this->patchArray->tesscorners->get(this->cpvIndicies[3]);
			corner0->setVertexIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[0]].vertexIndicies.front());
			corner1->setVertexIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[1]].vertexIndicies.front());
			corner2->setVertexIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].vertexIndicies.front());
			if (!this->flags.kIs3point)
				corner3->setVertexIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].vertexIndicies.back());

			corner0->setNormalIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[0]].normalIndicies.front());
			corner1->setNormalIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[1]].normalIndicies.front());
			corner2->setNormalIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].normalIndicies.front());
			if (!this->flags.kIs3point)
				corner3->setNormalIndex(patchArray->tessedges[this->patchVectors[pvectidx].tessedges[2]].normalIndicies.back());
		}
	}
}

void HPatch::createEdgeList(unsigned u, unsigned v, unsigned umax, unsigned vmax, unsigned pvectidx,
							bool is3point, HPoly &poly,
							std::vector<int> &edgeAlist,std::vector<int> &edgeBlist,std::vector<int> &edgeClist,std::vector<int> &edgeDlist) {
	UIFlags *ui_flags = &patchArray->model->ui_flags;
	HPolyArray *polygons = this->patchArray->model->polygonArray;

	
	poly.edgeIndicies[0] = polygons->edges.append(HEdge(poly.vertexIndicies[0],poly.vertexIndicies[1],
														poly.normalIndicies[0],poly.normalIndicies[1]));
	poly.edgeIndicies[1] = polygons->edges.append(HEdge(poly.vertexIndicies[1],poly.vertexIndicies[2],
														poly.normalIndicies[1],poly.normalIndicies[2]));
	if (!is3point) {
		poly.edgeIndicies[2] = polygons->edges.append(HEdge(poly.vertexIndicies[2],poly.vertexIndicies[3],
														poly.normalIndicies[2],poly.normalIndicies[3]));
		poly.edgeIndicies[3] = polygons->edges.append(HEdge(poly.vertexIndicies[3],poly.vertexIndicies[0],
														poly.normalIndicies[3],poly.normalIndicies[0]));
	}
	else
		poly.edgeIndicies[2] = polygons->edges.append(HEdge(poly.vertexIndicies[2],poly.vertexIndicies[0],
														poly.normalIndicies[2],poly.normalIndicies[0]));
	
	HEdge *edge0 = &polygons->edges[poly.edgeIndicies[0]];
	HEdge *edge1 = &polygons->edges[poly.edgeIndicies[1]];
	HEdge *edge2 = &polygons->edges[poly.edgeIndicies[2]];
	HEdge *edge3;
	if (!is3point)
		edge3 = &polygons->edges[poly.edgeIndicies[3]];
/*
	if ((u == 0) && (v == 0)) {
		edge0->flags.kVertex0IsPatchCorner = 1;
		if (!is3point)
			edge3->flags.kVertex1IsPatchCorner = 1;
		else
			edge2->flags.kVertex1IsPatchCorner = 1;
	}
	else if ((u == 0) && (v == vmax)) {
		edge0->flags.kVertex1IsPatchCorner = 1;
		edge1->flags.kVertex0IsPatchCorner = 1;
	}
	else if ((u == umax) && (v == vmax)) {
		edge1->flags.kVertex1IsPatchCorner = 1;
		edge2->flags.kVertex0IsPatchCorner = 1;
	}
	else if ((u == umax) && (v == 0)) {
		edge2->flags.kVertex1IsPatchCorner = 1;
		if (!is3point)
			edge3->flags.kVertex0IsPatchCorner = 1;
	}
*/
	HPatchVectors *pvect = this->patchVectors.get(pvectidx);
	if (u == 0) {
		edge0->flags.kOnPatchEdge = 1;
//		edge0->cpvIndex0 = this->cpvIndicies[0];
//		edge0->cpvIndex1 = this->cpvIndicies[1];
		edge0->patchidx = this->patchIndex;

		if (patchArray->tessedges[pvect->tessedges[0]].flags.kIsHookShortSegment)
			edge0->flags.kIsHookShortEdge = 1;
		if (patchArray->tessedges[pvect->tessedges[0]].flags.kIsHookLongSegment)
			edge0->flags.kIsHookLongEdge = 1;

		if (patchArray->tessedges[pvect->tessedges[0]].flags.kIsBorder)
			edge0->flags.kIsBorderEdge = 1;
		else
			if (patchArray->tessedges[pvect->tessedges[0]].adj_patchIndicies.size() > 0)
				edge0->adjpatchidx = patchArray->tessedges[pvect->tessedges[0]].adj_patchIndicies.front();

		if (patchArray->tessedges[pvect->tessedges[0]].flags.kHardEdge)
			edge0->flags.kHardEdge = 1;

		edgeAlist.push_back(poly.edgeIndicies[0]);
	}
	if (!is3point) {
		if (v == 0) {
			edge3->flags.kOnPatchEdge = 1;
//			edge3->cpvIndex0 = this->cpvIndicies[3];
//			edge3->cpvIndex1 = this->cpvIndicies[0];
			edge3->patchidx = this->patchIndex;

			if (patchArray->tessedges[pvect->tessedges[3]].flags.kIsHookShortSegment)
				edge3->flags.kIsHookShortEdge = 1;
			if (patchArray->tessedges[pvect->tessedges[3]].flags.kIsHookLongSegment)
				edge3->flags.kIsHookLongEdge = 1;

			if (patchArray->tessedges[pvect->tessedges[3]].flags.kIsBorder)
				edge3->flags.kIsBorderEdge = 1;
			else
				if (patchArray->tessedges[pvect->tessedges[3]].adj_patchIndicies.size() > 0)
					edge3->adjpatchidx = patchArray->tessedges[pvect->tessedges[3]].adj_patchIndicies.front();

		if (patchArray->tessedges[pvect->tessedges[3]].flags.kHardEdge)
			edge3->flags.kHardEdge = 1;

			edgeDlist.push_back(poly.edgeIndicies[3]);
		}
	}
	if (u == umax) {
		edge2->flags.kOnPatchEdge = 1;
//		edge2->cpvIndex0 = this->cpvIndicies[2];
//		edge2->cpvIndex1 = this->cpvIndicies[3];
		edge2->patchidx = this->patchIndex;

		if (patchArray->tessedges[pvect->tessedges[2]].flags.kIsHookShortSegment)
			edge2->flags.kIsHookShortEdge = 1;
		if (patchArray->tessedges[pvect->tessedges[2]].flags.kIsHookLongSegment)
			edge2->flags.kIsHookLongEdge = 1;

		if (patchArray->tessedges[pvect->tessedges[2]].flags.kIsBorder)
			edge2->flags.kIsBorderEdge = 1;
		else
			if (patchArray->tessedges[pvect->tessedges[2]].adj_patchIndicies.size() > 0)
				edge2->adjpatchidx = patchArray->tessedges[pvect->tessedges[2]].adj_patchIndicies.front();

		if (patchArray->tessedges[pvect->tessedges[2]].flags.kHardEdge)
			edge2->flags.kHardEdge = 1;

		edgeClist.push_back(poly.edgeIndicies[2]);
	}
	if (v == vmax) {
		edge1->flags.kOnPatchEdge = 1;
//		edge1->cpvIndex0 = this->cpvIndicies[1];
//		edge1->cpvIndex1 = this->cpvIndicies[2];
		edge1->patchidx = this->patchIndex;
		if (patchArray->tessedges[pvect->tessedges[1]].flags.kIsHookShortSegment)
			edge1->flags.kIsHookShortEdge = 1;
		if (patchArray->tessedges[pvect->tessedges[1]].flags.kIsHookLongSegment)
			edge1->flags.kIsHookLongEdge = 1;

		if (patchArray->tessedges[pvect->tessedges[1]].flags.kIsBorder)
			edge1->flags.kIsBorderEdge = 1;
		else
			if (patchArray->tessedges[pvect->tessedges[1]].adj_patchIndicies.size() > 0)
				edge1->adjpatchidx = patchArray->tessedges[pvect->tessedges[1]].adj_patchIndicies.front();

		if (patchArray->tessedges[pvect->tessedges[1]].flags.kHardEdge)
			edge1->flags.kHardEdge = 1;

		edgeBlist.push_back(poly.edgeIndicies[1]);
	}
}

void ReverseInts(std::vector<int> source,std::vector<int> &dest)
{
	dest.clear();
	int sourcelen = source.size();
	dest.resize(sourcelen);
	for (unsigned i = 0; i < source.size(); i++)
	{
		dest[sourcelen-i-1] = source[i];	
	}
}

void HPatch::createPatchMatrix(unsigned uspans,unsigned vspans,unsigned nspans,unsigned pvectidx,
							HMatrix &nvectmat,HMatrix &vmat,HMatrix &nmat,HMatrix &uvmat) {
	double u0,v0;
	UIFlags *ui_flags = &patchArray->model->ui_flags;
	double uk = getDivisionConstant((float)uspans),vk = getDivisionConstant((float)vspans),nspank = getDivisionConstant((float)nspans);
	HPolyArray *polygons = patchArray->model->polygonArray;
	HPatchVectors *pvect = &this->patchVectors[pvectidx];
	HVector vect;
	unsigned vidx,nidx;

	HMatrix mflags(uspans+1,vspans+1,HMatrix::INTEGER,1);

	unsigned i = 0;
	unsigned u,v,cellpos;

	if (!this->flags.kIs3point)
		this->cpid1[3];

	if (!this->flags.kIs5point) {
		HTessCorner *corner0 = &(*this->patchArray->tesscorners)[this->cpvIndicies[0]];
		HTessCorner *corner1 = &(*this->patchArray->tesscorners)[this->cpvIndicies[1]];
		HTessCorner *corner2 = &(*this->patchArray->tesscorners)[this->cpvIndicies[2]];
		HTessCorner *corner3 = &(*this->patchArray->tesscorners)[this->cpvIndicies[3]];

		applyEdgeVerticesToMatrix(0,COLUMN,0,pvectidx,vmat,nmat,mflags);	
		applyEdgeVerticesToMatrix(1,ROW,vspans,pvectidx,vmat,nmat,mflags);		
		applyEdgeVerticesToMatrix(2,COLUMN,uspans,pvectidx,vmat,nmat,mflags);
		applyEdgeVerticesToMatrix(3,ROW,0,pvectidx,vmat,nmat,mflags);		

	//	handle 3 sided patches
		if (this->flags.kIs3point) {
			if (corner0->flags.kHasVertexIndex) {
				vmat.fillRow(0,corner0->vertexIndex);
				nmat.fillRow(0,corner0->normalIndex);
				mflags.fillRow(0,0);
			}
			else {
				vidx = polygons->vertices.append(calcPointOnPatch(0,0,pvectidx));
				nidx = polygons->normals.append(getNormalFromMatrix(0,0,nspans,nspank,nvectmat));
				polygons->neighbors.append(HNeighbor());
				vmat.fillRow(0,vidx);
				nmat.fillRow(0,nidx);
				mflags.fillRow(0,0);
				corner0->setVertexIndex(vidx);
				corner0->setNormalIndex(nidx);
			}
		}
					
	//	handle corner vertex indicies to matrix
		if (corner0->flags.kHasVertexIndex) {
			if (mflags.getInt(0,0)) {
				vmat.set(0,0,corner0->vertexIndex);
				nmat.set(0,0,corner0->normalIndex);
				mflags.set(0,0,0);
			}
		}
		if (corner1->flags.kHasVertexIndex) {
			if (mflags.getInt(0,vspans)) {
				vmat.set(0,vspans,corner1->vertexIndex);
				nmat.set(0,vspans,corner1->normalIndex);
				mflags.set(0,vspans,0);
			}
		}
		if (corner2->flags.kHasVertexIndex) {
			if (mflags.getInt(uspans,vspans)) {
				vmat.set(uspans,vspans,corner2->vertexIndex);
				nmat.set(uspans,vspans,corner2->normalIndex);
				mflags.set(uspans,vspans,0);
			}
		}
		if (!this->flags.kIs3point) {
			if (corner3->flags.kHasVertexIndex) {
				if (mflags.getInt(uspans,0)) {
					vmat.set(uspans,0,corner3->vertexIndex);
					nmat.set(uspans,0,corner3->normalIndex);
					mflags.set(uspans,0,0);
				}
			}
		}		
	}
	else {
		unsigned cornerorder[] = {1,2,3,4,0};
		unsigned prev_patchorder[] = {4,0,1,2,3};
		HPatchVectors *prev_pvect = this->patchVectors.get(prev_patchorder[pvect->subPatchIndex]);
		if (patchArray->tessedges[prev_pvect->tessedges[3]].flags.kHasEdgeVertices) {
			vmat.fillColumn(0,patchArray->tessedges[prev_pvect->tessedges[3]].vertexIndicies,true);
			nmat.fillColumn(0,patchArray->tessedges[prev_pvect->tessedges[3]].normalIndicies,true);
			mflags.fillColumn(0,0);
		}
		if (pvect->subPatchIndex == 4) {
			if (patchArray->tessedges[this->patchVectors.begin()->tessedges[0]].flags.kHasEdgeVertices) {
				vmat.fillRow(0,patchArray->tessedges[this->patchVectors.begin()->tessedges[0]].vertexIndicies,false);
				nmat.fillRow(0,patchArray->tessedges[this->patchVectors.begin()->tessedges[0]].normalIndicies,false);
				mflags.fillRow(0,0);
			}
		}	
		bool found0=false,found1=false;
		unsigned adj_side0,adj_side1;
		HPatch *patch0,*patch1;
		HPatchVectors *adj_pvect0,*adj_pvect1;
		std::vector<int> subpatchIndicies,edgeIndicies;
		unsigned order[] = {0,1,2,3,4,0};
		
		if (patchArray->tessedges[pvect->tessedges[1]].adj_patchIndicies.size() > 0) {
			patch0 = this->patchArray->get(patchArray->tessedges[pvect->tessedges[1]].adj_patchIndicies.front());
			if (!patch0->flags.kIs5point) {
				adj_pvect0 = patch0->patchVectors.begin();
				found0 = findAdjacentEdgeOnPatch(patch0,adj_side0);
			}
			else {
				findAdjacentEdgesOnPatch(patch0,subpatchIndicies,edgeIndicies);
				if (subpatchIndicies.size() > 0) {
					if ((subpatchIndicies[0] == 0) && (subpatchIndicies[1] == 4))
					{
						ReverseInts(subpatchIndicies,subpatchIndicies);
						ReverseInts(edgeIndicies,edgeIndicies);
					}
					adj_pvect0 = patch0->patchVectors.get(subpatchIndicies[0]);
					vmat.fillRow(vspans,patchArray->tessedges[adj_pvect0->tessedges[edgeIndicies[0]]].vertexIndicies,true);
					nmat.fillRow(vspans,patchArray->tessedges[adj_pvect0->tessedges[edgeIndicies[0]]].normalIndicies,true);
					mflags.fillRow(vspans,0);
				}
			}
		}
		if (patchArray->tessedges[pvect->tessedges[2]].adj_patchIndicies.size() > 0) {
			patch1 = this->patchArray->get(patchArray->tessedges[pvect->tessedges[2]].adj_patchIndicies.front());
			if (!patch1->flags.kIs5point) {
				adj_pvect1 = patch1->patchVectors.begin();
				found1 = findAdjacentEdgeOnPatch(patch1,adj_side1);
			}
			else {
				findAdjacentEdgesOnPatch(patch1,subpatchIndicies,edgeIndicies);
				if (subpatchIndicies.size() > 0) {
					if ((subpatchIndicies[0] == 0) && (subpatchIndicies[1] == 4))
					{
						ReverseInts(subpatchIndicies,subpatchIndicies);
						ReverseInts(edgeIndicies,edgeIndicies);
					}
					adj_pvect1 = patch1->patchVectors.get(subpatchIndicies[1]);
					vmat.fillColumn(uspans,patchArray->tessedges[adj_pvect1->tessedges[edgeIndicies[1]]].vertexIndicies,false);
					nmat.fillColumn(uspans,patchArray->tessedges[adj_pvect1->tessedges[edgeIndicies[1]]].normalIndicies,false);
					mflags.fillColumn(uspans,0);
				}
			}
		}

		unsigned doreverse,vlen;

		if (found0) {
			std::vector<int> *vindicies = &patchArray->tessedges[adj_pvect0->tessedges[adj_side0]].vertexIndicies;
			std::vector<int> *nindicies = &patchArray->tessedges[adj_pvect0->tessedges[adj_side0]].normalIndicies;

			vlen = vindicies->size();
			doreverse = vlen-1;
			for (unsigned i = 0; (i*2) < vlen; i++) {
				vmat.set(doreverse-i*2,vspans,(*vindicies)[i]);
				nmat.set(doreverse-i*2,vspans,(*nindicies)[i]);
				mflags.set(doreverse-i*2,vspans,0);
			}
		}
		if (found1) {
			std::vector<int> *vindicies = &patchArray->tessedges[adj_pvect1->tessedges[adj_side1]].vertexIndicies;
			std::vector<int> *nindicies = &patchArray->tessedges[adj_pvect1->tessedges[adj_side1]].normalIndicies;

			vlen = vindicies->size();
			doreverse = vlen-1;	
			for (unsigned i = 0; (i*2) < vlen; i++) {
				vmat.set(uspans,doreverse-i*2,(*vindicies)[doreverse-i]);
				nmat.set(uspans,doreverse-i*2,(*nindicies)[doreverse-i]);
				mflags.set(uspans,doreverse-i*2,0);
			}
		}

		cellpos = (vspans * (uspans+1)) + uspans;
		HTessCorner *corner = this->patchArray->tesscorners->get(this->cpvIndicies[cornerorder[pvect->subPatchIndex]]);
		if (corner->flags.kHasVertexIndex) {
			vmat.set(cellpos,corner->vertexIndex);
			nmat.set(cellpos,corner->normalIndex);
			mflags.set(cellpos,0);
		}
		else {
			vidx = polygons->vertices.append(calcPointOnPatch(1,1,pvectidx));
			nidx = polygons->normals.append(getNormalFromMatrix(1,1,nspans,nspank,nvectmat));
			vmat.set(cellpos,vidx);
			nmat.set(cellpos,nidx);
			mflags.set(cellpos,0);
			polygons->neighbors.append(HNeighbor());
			corner->setVertexIndex(vidx);
			corner->setNormalIndex(nidx);
		}

	}
	unsigned idx;
	HVector temp0,temp1;
//	apply vertex indicies flagged in mflags; otherwise calculate new ones
	for (v = 0; v < vspans+1; v++) {
		for (u = 0; u < uspans+1; u++) {
			u0 = uk*u; v0 = vk*v;
			cellpos = (v*(uspans+1))+u;
			if (mflags.getInt(cellpos)){
				vmat.set(cellpos,polygons->vertices.append(calcPointOnPatch(u0,v0,pvectidx)));
				polygons->neighbors.append(HNeighbor());
			}
			if (mflags.getInt(cellpos)){
				temp0 = getNormalFromMatrix(u0,v0,nspans,nspank,nvectmat);
				nmat.set(cellpos,polygons->normals.append(temp0));
			}
			else {
				idx = nmat.getInt(cellpos);
				temp0 = polygons->normals[idx];
				temp1 = getNormalFromMatrix(u0,v0,nspans,nspank,nvectmat);
				temp0 += temp1;
				temp0 *= 0.5;
				polygons->normals[idx] = temp0;
				nmat.set(cellpos,idx);
			}
			uvmat.set(cellpos,polygons->uvs.append(calcUVOnPatch(u0,v0,pvectidx)));
		}
	}

	if (pvect->flags.k5ptSubPatch)
	{
		int fiveedgehookvid = vmat.getInt(0,vspans);
		int subpatchindex = pvectidx+1;
		if (subpatchindex>4)
			subpatchindex = 0;
		int cpindex = this->cpid1[subpatchindex];
		int cpindex2 = this->cpid2[pvectidx];
		patchArray->fivepointintmap.setValue(cpindex,fiveedgehookvid);
		patchArray->fivepointintmap.setValue(cpindex2,fiveedgehookvid);
	}
}

bool HPatch::applyEdgeVerticesToMatrix(unsigned side,unsigned ftype,unsigned cellidx,unsigned pvectidx,
									HMatrix &vmat,HMatrix &nmat,HMatrix &mflags) {

	HPatchVectors *curr_pvect = &this->patchVectors[pvectidx];
	HTessEdge *tessedge = &patchArray->tessedges[curr_pvect->tessedges[side]];	
	bool reverse = false;
	unsigned adj_side;
	UIFlags *ui_flags = &patchArray->model->ui_flags;

	if (tessedge->flags.kIsBorder) {
		return false;
	}
	else {
		HTessCorner *corner0 = &(*this->patchArray->tesscorners)[this->cpvIndicies[0]];
		HTessCorner *corner1 = &(*this->patchArray->tesscorners)[this->cpvIndicies[1]];
		HTessCorner *corner2 = &(*this->patchArray->tesscorners)[this->cpvIndicies[2]];
		HTessCorner *corner3 = &(*this->patchArray->tesscorners)[this->cpvIndicies[3]];

		if (tessedge->adj_patchIndicies.size()==0)
			return false;

		unsigned patchCount = tessedge->adj_patchIndicies.size();
//		this loop is mostly for long hook segments
		for (unsigned l = 0; l < patchCount; l++) {
			HPatch *patch = &(*this->patchArray)[tessedge->adj_patchIndicies[l]];
			HPatchVectors *pvect = &patch->patchVectors[0];
	
			bool found = findAdjacentEdgeOnPatch(patch,adj_side);
			std::vector<int> *vindicies;
			std::vector<int> *nindicies;

			if (found) {
				vindicies = &patchArray->tessedges[pvect->tessedges[adj_side]].vertexIndicies;
				nindicies = &patchArray->tessedges[pvect->tessedges[adj_side]].normalIndicies;

				int order[] = {0,1,2,3,0};
				unsigned vlen = vindicies->size();
				unsigned doreverse = 0;
				unsigned doreverseB = 0;
				if (tessedge->flags.kIsHookShortSegment) {
					unsigned short_right_corner = this->cpvIndicies[order[side]];
					unsigned short_left_corner = this->cpvIndicies[order[side+1]];
					unsigned long_right_corner = patch->cpvIndicies[order[adj_side]];
					unsigned long_left_corner = patch->cpvIndicies[order[adj_side+1]];
					bool shortedge_right = (long_left_corner == short_right_corner);
					bool shortedge_left = (short_left_corner == long_right_corner);

					if (shortedge_left) {
						doreverse = 0;
						if ((side == 0)||(side == 1))
							doreverse = vlen-1;

						for (unsigned i = 0; (i*2) < vlen; i++) {
							if (ftype == ROW) {
								vmat.set(abs((int)(doreverse-i*2)),cellidx,(*vindicies)[i]);
								nmat.set(abs((int)(doreverse-i*2)),cellidx,(*nindicies)[i]);
								mflags.set(abs((int)(doreverse-i*2)),cellidx,0);
							}
							else if (ftype == COLUMN) {
								vmat.set(cellidx,abs((int)(doreverse-i*2)),(*vindicies)[i]);
								nmat.set(cellidx,abs((int)(doreverse-i*2)),(*nindicies)[i]);
								mflags.set(cellidx,abs((int)(doreverse-i*2)),0);
							}
						}
					}
					if (shortedge_right) {
						doreverseB = vlen-1;
						doreverse = vlen-1;
						if (((side == 0)||(side == 1)))
							doreverse = 0;

						for (unsigned i = 0; (i*2) < vlen; i++) {
							if (ftype == ROW) {						
								vmat.set(abs((int)(doreverse-i*2)),cellidx,(*vindicies)[abs((int)(doreverseB-i))]);
								nmat.set(abs((int)(doreverse-i*2)),cellidx,(*nindicies)[abs((int)(doreverseB-i))]);
								mflags.set(abs((int)(doreverse-i*2)),cellidx,0);
							}
							else if (ftype == COLUMN) {
								vmat.set(cellidx,abs((int)(doreverse-i*2)),(*vindicies)[abs((int)(doreverseB-i))]);
								nmat.set(cellidx,abs((int)(doreverse-i*2)),(*nindicies)[abs((int)(doreverseB-i))]);
								mflags.set(cellidx,abs((int)(doreverse-i*2)),0);
							}
						}
					}
					return true;
				}
				else if (tessedge->flags.kIsHookLongSegment) {
					unsigned short_right_corner = patch->cpvIndicies[order[adj_side]];
					unsigned short_left_corner = patch->cpvIndicies[order[adj_side+1]];
					unsigned long_right_corner = this->cpvIndicies[order[side]];
					unsigned long_left_corner = this->cpvIndicies[order[side+1]];
					bool shortedge_right = (long_left_corner == short_right_corner);
					bool shortedge_left = (short_left_corner == long_right_corner);
					
					if (shortedge_left) {
						doreverseB = vlen-1;
						doreverse = vlen-1;
						if ((side == 0)||(side == 1))
							doreverse = 0;
						for (unsigned i = 0; (i*2) < vlen; i++) {
							if (ftype == ROW) {
								vmat.set(abs((int)(doreverse-i)),cellidx,(*vindicies)[abs((int)(doreverseB-(i*2)))]);
								nmat.set(abs((int)(doreverse-i)),cellidx,(*nindicies)[abs((int)(doreverseB-(i*2)))]);
								mflags.set(abs((int)(doreverse-i)),cellidx,0);
							}
							else if (ftype == COLUMN) {
								vmat.set(cellidx,abs((int)(doreverse-i)),(*vindicies)[abs((int)(doreverseB-(i*2)))]);
								nmat.set(cellidx,abs((int)(doreverse-i)),(*nindicies)[abs((int)(doreverseB-(i*2)))]);
								mflags.set(cellidx,abs((int)(doreverse-i)),0);
							}
						}
					}
					else if (shortedge_right) {
						doreverse = 0;
						if ((side == 0)||(side == 1))
							doreverse = vlen-1;
						for (unsigned i = 0; (i*2) < vlen; i++) {
							if (ftype == ROW) {
								vmat.set(abs((int)(doreverse-i)),cellidx,(*vindicies)[i*2]);
								nmat.set(abs((int)(doreverse-i)),cellidx,(*nindicies)[i*2]);
								mflags.set(abs((int)(doreverse-i)),cellidx,0);
							}
							else if (ftype == COLUMN) {
								vmat.set(cellidx,abs((int)(doreverse-i)),(*vindicies)[i*2]);
								nmat.set(cellidx,abs((int)(doreverse-i)),(*nindicies)[i*2]);
								mflags.set(cellidx,abs((int)(doreverse-i)),0);
							}
						}
					}
				}
				else {
					if ((ftype==COLUMN)&&(cellidx == 0))
						if (vindicies->front() != corner0->vertexIndex)
							reverse = true;
					if ((ftype==ROW)&&(cellidx == vmat.maxrows-1))
						if (vindicies->front() != corner1->vertexIndex)
							reverse = true;
					if ((ftype==COLUMN)&&( cellidx == vmat.maxcols-1))
						if (vindicies->front() != corner2->vertexIndex)
							reverse = false;
					if ((ftype==ROW)&&(cellidx == 0))
						if (vindicies->front() != corner3->vertexIndex)
							reverse = false;
					if (ftype == ROW) {
						vmat.fillRow(cellidx,*vindicies,reverse);
						nmat.fillRow(cellidx,*nindicies,reverse);
						mflags.fillRow(cellidx,0);
					}
					if (ftype == COLUMN) {
						vmat.fillColumn(cellidx,*vindicies,reverse);
						nmat.fillColumn(cellidx,*nindicies,reverse);
						mflags.fillColumn(cellidx,0);
					}
					return true;
				}
			}
		}
	}
	return false;
}

void HPatch::createNormalMatrix(unsigned pvectidx,HMatrix &nvectmat) {
	double u0,v0;
	unsigned cols = nvectmat.maxcols,rows = nvectmat.maxrows;
	double uk = getDivisionConstant((float)nvectmat.maxcols-1),vk = getDivisionConstant((float)nvectmat.maxrows-1);

	for (unsigned v = 0; v < rows; v++) {
		for (unsigned u = 0; u < cols; u++) {
			u0 = uk*u; v0 = vk*v;
			nvectmat.set(u,v,calcNormalOnPatch(u0,v0,pvectidx));
		}
	}
}

void HPatch::createPositionMatrix(unsigned pvectidx, HMatrix &pvectmat)
{
	double u0,v0;
	double uk = getDivisionConstant((float)pvectmat.maxcols-1),vk = getDivisionConstant((float)pvectmat.maxrows-1);
	for (unsigned v = 0; v < pvectmat.maxrows; v++)
	{
		for (unsigned u = 0; u < pvectmat.maxcols; u++)
		{
			u0 = uk*u; v0 = vk*v;
			pvectmat.set(u,v,calcPointOnPatch(u0,v0,pvectidx));
		}
	}
}

/*
void HPatch::createNormalMatrix(unsigned pvectidx,HMatrix &nvectmat) {
	HNeighborArray *neighbors = &this->patchArray->model->splineArray.neighbors;
	HVector corner0,corner1,corner2,corner3;
	unsigned cols = nvectmat.maxcols,rows = nvectmat.maxrows;
	HMatrix mflags(cols,rows,HMatrix::INTEGER,1);
	double uk = getDivisionConstant(nvectmat.maxcols-1),vk = getDivisionConstant(nvectmat.maxrows-1);
	double u0,v0;

	if (!this->flags.kIs5point) {
		corner0 = neighbors->calcNormal(this->cpvIndicies[0],true);
		corner1 = neighbors->calcNormal(this->cpvIndicies[1],true);
		corner2 = neighbors->calcNormal(this->cpvIndicies[2],true);
		corner3 = neighbors->calcNormal(this->cpvIndicies[3],true);
		
		nvectmat.set(0,0,corner0);
		mflags.set(0,0,0);

		nvectmat.set(cols-1,0,corner3);
		mflags.set(cols-1,0,0);

		nvectmat.set(0,rows-1,corner1);
		mflags.set(0,rows-1,0);

		nvectmat.set(cols-1,rows-1,corner2);
		mflags.set(cols-1,rows-1,0);
	}

	unsigned cellpos;
	for (unsigned v = 0; v < rows; v++) {
		for (unsigned u = 0; u < cols; u++) {
			u0 = uk*u; v0 = vk*v;
			cellpos = (v*(cols))+u;
			if (mflags.getInt(cellpos))
				nvectmat.set(cellpos,calcNormalOnPatch(u0,v0,pvectidx));
		}
	}

}
*/

HVector HPatch::getNormalFromMatrix(double u,double v,unsigned nspans,double nspank,HMatrix &nvectmat) {
	HVector n;
	unsigned col = (unsigned)(u*nspans);
	unsigned row = (unsigned)(v*nspans);

	HVector2D nuv(u,v);

	if (row == nspans)
		row = nspans-1;

	if (col == nspans)
		col = nspans-1;

	HVector corner0 = nvectmat.getVector(col,row);
	HVector corner1 = nvectmat.getVector(col+1,row);
	HVector corner2 = nvectmat.getVector(col+1,row+1);
	HVector corner3 = nvectmat.getVector(col,row+1);

	double col0nuvspace = double(col)/nspans;
	double row0nuvspace = double(row)/nspans;
	double col1nuvspace = double((col+1))/nspans;
	double row1nuvspace = double((row+1))/nspans;

	HVector2D nv0(col0nuvspace,row0nuvspace);
	HVector2D nv1(col1nuvspace,row0nuvspace);
	HVector2D nv2(col1nuvspace,row1nuvspace);
	HVector2D nv3(col0nuvspace,row1nuvspace);

	double dnv0 = (nspank - (nv0 - nuv).length()) / nspank;
	double dnv1 = (nspank - (nv1 - nuv).length()) / nspank;
	double dnv2 = (nspank - (nv2 - nuv).length()) / nspank;
	double dnv3 = (nspank - (nv3 - nuv).length()) / nspank;

	if (dnv0 < 0)
		dnv0 = 0.0;
	if (dnv1 < 0)
		dnv1 = 0.0;
	if (dnv2 < 0)
		dnv2 = 0.0;
	if (dnv3 < 0)
		dnv3 = 0.0;

	double total = dnv0+dnv1+dnv2+dnv3;
	n = (corner0*dnv0 + corner1*dnv1 + corner2*dnv2 + corner3*dnv3) / total;
	return n;
}

/*
void HPatch::calcPolysVariableSubdivision(HPolyArray &polygons,std::vector<int> &shaderIndicies) {
	HVector normal;
	double u2,u3,v2,v3;
	unsigned usubdivide, vsubdivide;
	bool is3point;

	UIFlags *ui_flags = &patchArray->model->ui_flags;

	if (this->patchVectors.length() == 0) //tangents weren't calculated
		return;

	HDoubleArray upercent,vpercent;
	getSubdivideInfo(patchArray->model->ui_straightnesstolerance,vpercent,upercent);

	usubdivide = upercent.length()-1;
	vsubdivide = vpercent.length()-1;

	if ((usubdivide>10) && (vsubdivide > 10)) {
		THROW_FATAL_ERROR("Subdivisions exceed 10.");
	}

	unsigned numPatchVectors = this->patchVectors.length();

	HProxyPoly poly;

	HVector vtx0,vtx1,vtx2,vtx3;
	HVector nml0,nml1,nml2,nml3;

	for (unsigned pvectidx = 0; pvectidx < numPatchVectors; pvectidx++) {
		for (unsigned v = 0; v < vsubdivide; v++) {
			for (unsigned u = 0; u < usubdivide; u++) {

				poly.clear();

				u2 = upercent[u];
				v2 = vpercent[v];
				v3 = vpercent[v+1];
				u3 = upercent[u+1];

				vtx0 = calcPointOnPatch(u2,v2,pvectidx);
				vtx1 = calcPointOnPatch(u2,v3,pvectidx);
				vtx2 = calcPointOnPatch(u3,v3,pvectidx);
				vtx3 = calcPointOnPatch(u3,v2,pvectidx);

				is3point = false;
				if (vtx0.DistanceTo(vtx3) < 0.01f)
					is3point = true;

				if (ui_flags->calculateuv) {
					poly.uvs.append(calcUVOnPatch(u2,v2,pvectidx));
					poly.uvs.append(calcUVOnPatch(u2,v3,pvectidx));
					poly.uvs.append(calcUVOnPatch(u3,v3,pvectidx));
					if (!is3point)
						poly.uvs.append(calcUVOnPatch(u3,v2,pvectidx));
				}

				if (ui_flags->derivenormalsfrompatches) {
					nml0 = calcNormalOnPatch(u2,v2,pvectidx);
					nml1 = calcNormalOnPatch(u2,v3,pvectidx);
					nml2 = calcNormalOnPatch(u3,v3,pvectidx);
					nml3 = calcNormalOnPatch(u3,v2,pvectidx);

					poly.normals.append(nml0);
					poly.normals.append(nml1);
					poly.normals.append(nml2);
					if (!is3point)
						poly.normals.append(nml3);
				}
				poly.vertices.append(vtx0);
				poly.vertices.append(vtx1);
				poly.vertices.append(vtx2);
				if (!is3point)
					poly.vertices.append(vtx3);

				// SETS CORNER AND EDGE FLAGS
				////////////////////////////////////////////////////////////////////////////////

				createEdgeList(u,v,usubdivide-1,vsubdivide-1,pvectidx,is3point,poly.edges);

				////////////////////////////////////////////////////////////////////////////////
				
				poly.patchIndex = this->patchIndex;
				poly.shaderIndicies = shaderIndicies;

				if (ui_flags->weldvertices)
					polygons.add_weld_vertices(poly,true);
				else
					polygons.add(poly);
			}
		}
	}
}
*/

bool HPatch::hasGroups() {
	if (this->groupIndicies.size() > 0)
		return true;
	return false;
}

bool HPatch::hasDecals() {
	if (this->decalIndicies.size() > 0)
		return true;
	return false;
}

void HPatch::tessellate(HPolyArray &polygons,std::vector<int> &shaderIndicies,unsigned numsubdivisions) {
	UIFlags *ui_flags = &patchArray->model->ui_flags;

//	if (ui_flags->variabletessellation)
//		calcPolysVariableSubdivision(polygons,shaderIndicies);
//	else
		calcPolysMidpointSubdivision(polygons,shaderIndicies,numsubdivisions, numsubdivisions);
}

HPatch *HPatch::next() {
	if (this == patchArray->end())
		return NULL;
	return this+1;
}

HPatch *HPatch::previous() {
	if (this == patchArray->begin())
		return NULL;
	return this-1;
}

HPatch *HPatch::begin() {
	return patchArray->begin();
}

HPatch *HPatch::end() {
	return patchArray->end();
}

HPatch &HPatch::operator = ( const HPatch &other ){
	memcpy(&this->flags,&other.flags,sizeof(unsigned long));
	cpid1 = other.cpid1;
	cpid2 = other.cpid2;
	patchIndex = other.patchIndex;
	attrIndex = other.attrIndex;
	patchArray = other.patchArray;
	patchVectors = other.patchVectors;
	decalIndicies = other.decalIndicies;
	groupIndicies = other.groupIndicies;
	cpvIndicies = other.cpvIndicies;
	return *this;
}

std::ostream &operator << (std::ostream &os, HPatch &patch){
	unsigned i;
	os << "Patch (" << patch.patchIndex << ") ";

	if (patch.flags.kIs3point)
		os << "[kIs3point] ";
	if (patch.flags.kIs5point)
		os << "[kIs5point] ";
	os << std::endl;

	os << "SideA: " << patch.cpid1[0] << "," << patch.cpid2[0] << std::endl;
	os << "SideB: " << patch.cpid1[1] << "," << patch.cpid2[1] << std::endl;
	os << "SideC: " << patch.cpid1[2] << "," << patch.cpid2[2] << std::endl;
	os << "SideD: " << patch.cpid1[3] << "," << patch.cpid2[3] << std::endl;
	if (patch.flags.kIs5point)
		os << "SideE: " << patch.cpid1[4] << "," << patch.cpid2[4] << std::endl;

//	os << "cpvIndicies: " << patch.cpvIndicies << std::endl;

	for (i = 0; i < patch.patchVectors.length(); i++){
		if (patch.patchVectors.length()>1)
			os << "SubPatch:" << patch.patchVectors[i].subPatchIndex << std::endl;
		os << patch.patchVectors[i];
	}
		
	for (i = 0; i < patch.groupIndicies.size(); i++)
		os << patch.patchArray->groupArray[patch.groupIndicies[i]] << std::endl;

	for (i = 0; i < patch.decalIndicies.size(); i++)
		os << patch.patchArray->decalArray[patch.decalIndicies[i]] << std::endl;

	return os;
}