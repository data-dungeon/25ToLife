#ifndef __OSTREAM_ALIGNER_H
#define __OSTREAM_ALIGNER_H

#include <ostream>

using namespace		std;

class OStreamAligner
{

protected:
	streampos			d_marker;
	ostream *			p_str;

public:
	OStreamAligner( ) : p_str( 0), d_marker( 0) { }

	streampos		marker( void ) const { return d_marker; }

	void 				mark( ostream &str) { p_str = &str; d_marker = str.tellp( );}

	ostream &		operator<< (ostream &str) { mark( str); return str; }

	virtual void 	align( ostream &str) = 0;

} ;

inline ostream & operator <<( ostream &str, OStreamAligner &aligner)
{
	aligner.align( str);

	return str;
}

#endif

