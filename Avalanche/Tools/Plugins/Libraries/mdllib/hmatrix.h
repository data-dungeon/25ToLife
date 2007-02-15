//////////////////////////////////////////////////////////////////////////////////////////
//
// HMATRIX CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HMATRIX_H
#define __HMATRIX_H

#ifndef __HVECTOR_H
#include "hvector.h"
#endif

class HMatrix
{
public:
	enum mType { INTEGER,VECTOR } type;
	
	HMatrix(unsigned cols,unsigned rows,mType matrix_type);
	HMatrix(unsigned cols,unsigned rows,mType matrix_type,const int value);
	HMatrix(const HMatrix &other);
	~HMatrix();

	void				clear();

//////////////////////////////////////////////////////////////////////////////////////////
//	INTEGER METHODS
//////////////////////////////////////////////////////////////////////////////////////////

	void				set(const unsigned col, const unsigned row,const int value);
	int					getInt(const unsigned col, const unsigned row) const;

	void				set(const unsigned index, const int value);
	int					getInt(const unsigned index) const;

	void				fill(const int value);

	void				fillColumn(const unsigned col,const std::vector<int> &list,bool reverse);
	void				fillRow(const unsigned row,const std::vector<int> &list,bool reverse);

	void				fillColumn(const unsigned col,const int value);
	void				fillRow(const unsigned row,const int value);

	void				getColumn(const unsigned col,std::vector<int> &list);
	void				getRow(const unsigned row,std::vector<int> &list);
	
//////////////////////////////////////////////////////////////////////////////////////////
//	VECTOR METHODS
//////////////////////////////////////////////////////////////////////////////////////////
	void				set(const unsigned col, const unsigned row,const HVector pos);
	HVector				getVector(const unsigned col, const unsigned row) const;

	void				set(const unsigned index, const HVector pos);
	HVector				getVector(const unsigned index) const;

	void				fill(const HVector pos);

	void				fillColumn(const unsigned col,const HVectorArray &list,bool reverse);
	void				fillRow(const unsigned row,const HVectorArray &list,bool reverse);

	void				fillColumn(const unsigned col,const HVector pos);
	void				fillRow(const unsigned row,const HVector pos);

	void				getColumn(const unsigned col,HVectorArray &list);
	void				getRow(const unsigned row,HVectorArray &list);

	HMatrix				&operator = ( const HMatrix &other );
	friend std::ostream		&operator << (std::ostream &os,const HMatrix &other);

	////////////////////////////////////////////////////////////////////////////////////////
	// CLASS VARIABLES
	////////////////////////////////////////////////////////////////////////////////////////

	unsigned			maxcols,maxrows;

private:
	std::vector<int>	ivalues;
	HVectorArray		vvalues;

};

#endif
