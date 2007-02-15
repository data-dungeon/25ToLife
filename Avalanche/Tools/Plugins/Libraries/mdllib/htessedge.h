//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSEDGE CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HTESSEDGE_H
#define __HTESSEDGE_H

class mdlObject;

class HTessEdge
{
public:
	struct teflags {
		unsigned long	kHasEdgeVertices				: 1,
						kIsHookShortSegment				: 1,
						kIsHookLongSegment				: 1,
						kIsBorder						: 1,
						kHardEdge						: 1;
	} flags;

	HTessEdge();
	HTessEdge(const HTessEdge &other);
	~HTessEdge();

	void			clear();
	void			setEdgeVertices(bool reverse,std::vector<int> &edgeIndicies,mdlObject *mdl);

	HTessEdge &operator = ( const HTessEdge &other );

	std::vector<int> adj_patchIndicies;
	std::vector<int> vertexIndicies;
	std::vector<int> normalIndicies;

	mdlObject *model;

	friend std::ostream		&operator << (std::ostream &os,const HTessEdge &other);
};

class HTessEdgeArray
{
public:
	HTessEdgeArray();
	HTessEdgeArray(const unsigned s);
	HTessEdgeArray(const HTessEdgeArray &other);
	~HTessEdgeArray();

	void					clear();
	unsigned				length() const;
	void					resize(const unsigned size);
	unsigned				append(const HTessEdge &other);

	HTessEdge				*get(unsigned index);
	HTessEdge				*begin();
	HTessEdge				*end();

	HTessEdge				operator[]( unsigned index ) const;
 	HTessEdge&				operator[]( unsigned index );
	HTessEdgeArray			&operator = ( const HTessEdgeArray &other );

	std::vector<HTessEdge> classList;
};

#endif
