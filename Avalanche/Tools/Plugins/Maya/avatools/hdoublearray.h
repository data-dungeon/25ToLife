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
	HDoubleArray(unsigned s);

	~HDoubleArray ();

	void			clear(void);
	unsigned		length(void) const;
	void			append(double value);
	void			append_NR(double value);
	void			resize(unsigned s);

	const double		get(unsigned index);
	void			set(unsigned index, double value);

	double				begin();
	double				end();

	unsigned		beginIndex();
	unsigned		endIndex();

	void			Sort(void);

	bool			value_exists(double value,unsigned &index);
	double				max_value(void);

	double					operator[]( unsigned index ) const;
 	double&	 			operator[]( unsigned index );

	HDoubleArray			&operator = ( const HDoubleArray other );

	friend std::ostream		&operator << (std::ostream &os,HDoubleArray &iarray);

	std::vector<double> classList;
};

class HDoubleIterator
{
public:
	HDoubleIterator();
	HDoubleIterator(HDoubleArray &iarray);
	~HDoubleIterator();

	void reset(void);
	void reset(HDoubleArray &iarray);

	bool isDone(void);

	void insert(const double value);
	unsigned		index();

	HDoubleIterator	operator  ++(int);
	HDoubleIterator	operator  --(int);
	double				*operator * ();

private:
	HDoubleArray *pointTo;
	std::vector<double>::iterator cl_iterator;
};

class HDoubleArrayList
{
public:
	HDoubleArrayList();

	~HDoubleArrayList();

	void				clear(void);
	unsigned			length(void) const;
	void				append(HDoubleArray value);
	bool				value_exists(double value);
	void				resize(unsigned s);

	HDoubleArray			*get(unsigned index);

	HDoubleArray			operator[]( unsigned index ) const;
 	HDoubleArray&	 		operator[]( unsigned index );

	HDoubleArrayList		&operator = ( const HDoubleArrayList other );
	
	friend std::ostream		&operator << (std::ostream &os,HDoubleArrayList &ilist);

private:
	std::vector<HDoubleArray> classList;
};



#endif

