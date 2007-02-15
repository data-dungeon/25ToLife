//////////////////////////////////////////////////////////////////////////////////////////
//
// HEDGE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HEDGE_H
#define __HEDGE_H

class mdlObject;

class HEdge
{
public:
	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS FLAGS
	////////////////////////////////////////////////////////////////////////////////////////

	struct eflags {
		unsigned long	kVertex0IsPatchCorner			: 1,
						kVertex1IsPatchCorner			: 1,
						kIsHookShortEdge				: 1,								  
						kIsHookLongEdge					: 1,
						kIsBorderEdge					: 1,
						kOnPatchEdge					: 1,
						kHardEdge						: 1;
	} flags;

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS METHODS
	////////////////////////////////////////////////////////////////////////////////////////

	HEdge();
	HEdge(const HEdge &other);
	HEdge(unsigned vidx0, unsigned vidx1);
	HEdge(const unsigned vidx0,const unsigned vidx1,
			const unsigned nidx0,const unsigned nidx1);
	~HEdge();

	void				clear();
	void				invert();
	void				addVertexIndicies(unsigned vi0,unsigned vi1);
	void				addNormalIndicies(unsigned ni0,unsigned ni1);

	HEdge &operator = ( const HEdge &other );
	bool operator == ( const HEdge &other ) const;
	bool operator != ( const HEdge &other ) const;
	friend std::ostream		&operator << (std::ostream &os,HEdge &e);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	unsigned			vertexIndex0,vertexIndex1;
	unsigned			normalIndex0,normalIndex1;
	unsigned			patchidx;
	unsigned			adjpatchidx;
	mdlObject			*model;
};

class HEdgeArray
{
public:
	HEdgeArray ();
	HEdgeArray(const HEdgeArray &other);
	HEdgeArray(const unsigned s);

	~HEdgeArray ();

	void				clear(void);
	unsigned			length(void) const;
	void				resize(const unsigned s);
	unsigned			append(const HEdge &other);

	bool				findEdge(HEdge &other,unsigned &index);

	HEdge				*get(unsigned index);
	HEdge				*begin();
	HEdge				*end();

	HEdge				operator[]( unsigned index ) const;
 	HEdge&				operator[]( unsigned index );
	HEdgeArray			&operator = ( const HEdgeArray &other );
	friend std::ostream		&operator << (std::ostream &os,const HEdgeArray &other);

	std::vector<HEdge> classList;
};

#endif
