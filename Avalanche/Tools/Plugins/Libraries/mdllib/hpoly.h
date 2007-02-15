//////////////////////////////////////////////////////////////////////////////////////////
//
// HPOLY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HPOLY_H
#define __HPOLY_H

class HPolyArray;

class HPoly
{
public:
	HPoly();
	HPoly(const HPoly &other);
	~HPoly();

	void					clear();
	bool					hasVertices();
	bool					hasNormals();
	bool					hasUVs();

	HPoly					*next();
	HPoly					*previous();
	HPoly					*begin();
	HPoly					*end();

	HPoly &operator = ( const HPoly &other );

	unsigned				patchIndex;
	unsigned				polyIndex;

	unsigned				smoothGrpID;
	unsigned				dontuse;

	std::vector<int>		vertexIndicies;
	std::vector<int>		normalIndicies;
	std::vector<int>		uvIndicies;
	std::vector<int>		edgeIndicies;
	std::vector<int>		shaderIndicies;

	HPolyArray				*polyArray;

//	friend std::ostream		&operator << (std::ostream &os,HPoly &poly);
};

#endif
