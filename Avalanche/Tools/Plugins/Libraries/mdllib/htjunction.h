//////////////////////////////////////////////////////////////////////////////////////////
//
// HTJUNCTION CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HTJUNCTION_H
#define __HTJUNCTION_H

#ifndef __HEDGE_H
#include "hedge.h"
#endif

class HEdge;

class HTJunction
{
public:
	HTJunction();
	HTJunction(const HTJunction &other);
	~HTJunction();

	void					clear();

	HTJunction				&operator = (const HTJunction &other);
	friend std::ostream			&operator << (std::ostream &os,HTJunction &other);

	HEdge					long_edge;
	HEdgeArray				short_edges;
};

class HTJunctionArray
{
public:
	HTJunctionArray ();
	~HTJunctionArray ();

	void					clear();
	unsigned				length() const;
	void					resize(const unsigned size);
	unsigned				append(HTJunction &other);

	HTJunction				*get(unsigned index);
	HTJunction				*begin();
	HTJunction				*end();

	HTJunction				operator[]( unsigned index ) const;
 	HTJunction&				operator[]( unsigned index );
	HTJunctionArray			&operator = (const HTJunctionArray &other);
	friend std::ostream			&operator << (std::ostream &os,HTJunctionArray &other);

	std::vector<HTJunction> classList;
};

#endif
