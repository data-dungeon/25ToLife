#ifndef __HINTARRAY_H
#define __HINTARRAY_H

#ifndef __HINTMAP_H
#include "hintmap.h"
#endif

typedef std::vector<int> IntVector;
typedef IntVector::iterator IntVectorIt;

class HIntMap;
class HIntArray;

typedef std::vector<HIntArray>::iterator IntVectorArrayIt;

class HIntArray
{
public:
	HIntArray ();
	~HIntArray ();

	void				clear();
	unsigned			length() const;

	int					front() const;
	int					back() const;

	void				append(const int key);
//	void				append(const HIntArray &keys);

//	void				resize(const unsigned s);

	void				remove(unsigned index);

	void				reverse();
	void				reverse(HIntArray &newlist);
	void				reorder(HIntArray &newarray, const HIntArray &order);

	void				sort();
	bool				valueExists(int key,unsigned &index);
	int					maxValue();

	void				commonElements(HIntArray &b,HIntArray &c);
	void				commonElements(HIntArray &b,HIntArray &c,HIntArray &idxs);

	void				commonElements(HIntArray &b,std::vector<int> &c);

	int					operator[]( unsigned index ) const;
 	int&	 			operator[]( unsigned index );

	HIntArray			&operator = ( HIntMap &map );
	HIntArray			&operator = ( const HIntArray &other );
	bool				operator == ( HIntArray &other ) const;
	bool				operator != ( HIntArray &other ) const;
	friend std::ostream		&operator << (std::ostream &os,const HIntArray &other);

	HIntMap imap;
	std::vector<int> classList;
};

class HIntIterator
{
public:
	HIntIterator();
	HIntIterator(HIntArray &other);
	~HIntIterator();

	void			reset();
	void			reset(HIntArray &other);

	bool			isDone();

	void			insert(const int value);
	unsigned		index() const;

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

	void				clear();
	void				resize(const unsigned size);
	unsigned			length() const;
	void				append(HIntArray &other);
	bool				valueExists(int value);

	HIntArray			*get(unsigned index);

	HIntArray			operator[]( unsigned index ) const;
 	HIntArray&	 		operator[]( unsigned index );

	HIntArrayList		&operator = ( const HIntArrayList &other );
	
	friend std::ostream		&operator << (std::ostream &os,HIntArrayList &other);

private:
	std::vector<HIntArray> classList;
};

#include "hintarray.hpp"

#endif

