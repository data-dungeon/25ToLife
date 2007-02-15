#ifndef __HINTARRAY_H
#define __HINTARRAY_H


typedef std::vector<int> IntVector;
typedef IntVector::iterator IntVectorIt;


class HIntArray;

typedef std::vector<HIntArray>::iterator IntVectorArrayIt;

class HIntArray
{
public:

	HIntArray ();
	HIntArray(unsigned s);

	~HIntArray ();

	void			clear(void);
	unsigned		length(void) const;
	void			append(int value);
	void			append_NR(int value);
	void			resize(unsigned s);
	void			zero();

	const int		get(unsigned index);
	void			set(unsigned index, int value);

	int				begin();
	int				end();

	unsigned		beginIndex();
	unsigned		endIndex();

	void			Sort(void);

	bool			value_exists(int value,unsigned &index);
	int				max_value(void);

	int					operator[]( unsigned index ) const;
 	int&	 			operator[]( unsigned index );

	HIntArray			&operator = ( const HIntArray other );

	friend std::ostream		&operator << (std::ostream &os,HIntArray &iarray);

	std::vector<int> classList;
};

class HIntIterator
{
public:
	HIntIterator();
	HIntIterator(HIntArray &iarray);
	~HIntIterator();

	void	reset(void);
	void  reset(HIntArray &iarray);

	bool			isDone(void);

	void 			insert(const int value);
	unsigned		index();

	HIntIterator	operator  ++(int);
	HIntIterator	operator  --(int);
	int				*operator * ();

private:
	HIntArray *pointTo;
	std::vector<int>::iterator cl_iterator;
};

class HIntArrayList
{
public:
	HIntArrayList();

	~HIntArrayList();

	void				clear(void);
	unsigned			length(void) const;
	void				append(HIntArray value);
	bool				value_exists(int value);
	void				resize(unsigned s);

	HIntArray			*get(unsigned index);

	HIntArray			operator[]( unsigned index ) const;
 	HIntArray&	 		operator[]( unsigned index );

	HIntArrayList		&operator = ( const HIntArrayList other );
	
	friend std::ostream		&operator << (std::ostream &os,HIntArrayList &ilist);

private:
	std::vector<HIntArray> classList;
};



#endif

