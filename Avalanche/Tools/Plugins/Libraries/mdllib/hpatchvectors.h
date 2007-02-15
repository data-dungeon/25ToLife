#ifndef __HPATCHVECTORS_H
#define __HPATCHVECTORS_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif
#ifndef __HMACROS_H
#include "hmacros.h"
#endif
#ifndef __HTESSEDGE_H
#include "htessedge.h"
#endif


class HPatchArray;

class HPatchVectors
{
public:
	struct pvFlags {
		unsigned long	kIs3point						: 1,
						k5ptSubPatch					: 1;
	} flags;

	HPatchVectors();
	HPatchVectors(const HPatchVectors &other);
	~HPatchVectors();

	void					clear();
	void					append(HVector v);

	int						patchIndex;
	int						subPatchIndex;
	HVectorArray			pv;
	unsigned				tessedges[4];

	HPatchVectors &operator = ( const HPatchVectors &other );
	HPatchVectors &operator = ( HVector *other );
	friend std::ostream &operator << (std::ostream &os,const HPatchVectors &other);
};

class HPatchVectorsArray
{
public:
	HPatchVectorsArray();
	~HPatchVectorsArray();

	void					clear();
	unsigned				length() const;
	unsigned				append(const HPatchVectors &pVectors);

	HPatchVectors			*get(unsigned index);
	HPatchVectors			*begin();
	HPatchVectors			*end();

	HPatchVectors			operator[](unsigned index) const;
	HPatchVectors			&operator[](unsigned index);
	HPatchVectorsArray		&operator = ( const HPatchVectorsArray &other );

	friend std::ostream &operator << (std::ostream &os,const HPatchVectorsArray &other);

	std::vector<HPatchVectors> classList;
};

#endif