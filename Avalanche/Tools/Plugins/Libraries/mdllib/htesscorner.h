//////////////////////////////////////////////////////////////////////////////////////////
//
// HTESSCORNER CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __HTESSCORNER_H
#define __HTESSCORNER_H

class HTessCorner
{
public:
	struct tcflags {
		unsigned long	kHasVertexIndex				: 1;
	} flags;

	HTessCorner();
	HTessCorner(const HTessCorner &other);
	~HTessCorner();

	void			clear();
	void			setVertexIndex(unsigned vi);
	void			setNormalIndex(unsigned ni);

	HTessCorner &operator = ( const HTessCorner &other );

	unsigned vertexIndex;
	unsigned normalIndex;

	friend std::ostream		&operator << (std::ostream &os,HTessCorner &other);
};

class HTessCornerArray
{
public:
	HTessCornerArray();
	HTessCornerArray(const unsigned s);
	HTessCornerArray(const HTessCornerArray &other);
	~HTessCornerArray();

	void					clear();
	unsigned				length() const;
	void					resize(const unsigned size);
	unsigned				append(const HTessCorner &other);

	HTessCorner				*get(unsigned index);
	HTessCorner				*begin();
	HTessCorner				*end();

	HTessCorner				operator[]( unsigned index ) const;
 	HTessCorner&			operator[]( unsigned index );
	HTessCornerArray		&operator = ( const HTessCornerArray &other );

	friend std::ostream			&operator << (std::ostream &os,const HTessCornerArray &other);

	std::vector<HTessCorner> classList;
};

#endif
