#ifndef __VIF_CODE_H
#define __VIF_CODE_H

#if !defined(PS2) || !defined(SN)
#include <ostream>
#endif

using namespace std;

class VIFCode
{

	unsigned int	d_Code;
	
public:
	VIFCode( ) : d_Code( 0) { }
	VIFCode( unsigned int defCode) : d_Code( defCode) { }
	operator unsigned int ( ) { return d_Code; }		
#if !defined(PS2) || !defined(SN)
	void				write( ostream &str) const { str.write( (char *) &d_Code, 4); }
#endif
    
} ;

#if !defined(PS2) || !defined(SN)
inline ostream & operator<<( ostream &str, const VIFCode &Code)
{
	Code.write( str);
	
	return str;
	
}
#endif

#endif