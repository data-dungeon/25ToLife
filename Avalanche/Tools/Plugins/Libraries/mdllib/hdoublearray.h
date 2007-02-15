#ifndef __HDOUBLEARRAY_H
#define __HDOUBLEARRAY_H

typedef std::vector<double> DoubleVector;
typedef DoubleVector::iterator DoubleVectorIt;

class HDoubleArray;

typedef std::vector<HDoubleArray>::iterator DoubleVectorArrayIt;

class HDoubleArray
{
public:
	HDoubleArray ();
	HDoubleArray(const unsigned size);
	~HDoubleArray ();

	void					clear();
	unsigned				length() const;

	void					append(const double value);
	void					append(const HDoubleArray &other);

	void					resize(const unsigned size);
	void					reserve(const unsigned size);
	
	void					remove(const unsigned index);

	double					begin() const;
	double					end() const;

	double					get(const unsigned index) const;
	void					set(const unsigned index,const double value);

	void					sort();
	bool					valueExists(const double value,unsigned &index);
	double					maxValue();
	HDoubleArray			&unify(const HDoubleArray &a,const HDoubleArray &b);

	double					operator[]( unsigned index ) const;
 	double&	 				operator[]( unsigned index );
	HDoubleArray			&operator = ( const HDoubleArray &other );
	friend std::ostream			&operator << (std::ostream &os,HDoubleArray &other);

	std::vector<double> classList;
};

class HDoubleIterator
{
public:
	HDoubleIterator();
	HDoubleIterator(HDoubleArray &iarray);
	~HDoubleIterator();

	void			reset(void);
	void			reset(HDoubleArray &iarray);

	bool			isDone(void);

	void			insert(const double value);
	unsigned		index();

	HDoubleIterator	operator  ++(int);
	HDoubleIterator	operator  --(int);
	double				*operator * ();

private:
	HDoubleArray *pointTo;
	std::vector<double>::iterator cl_iterator;
};

#include "hdoublearray.hpp"

#endif

