/***********************************/
/*                                 */
/* Surface.h                       */
/* Pigpen particle tool  4/4/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* collision surface type class    */
/*                                 */
/***********************************/

#ifndef __SURFACE_H
#define __SURFACE_H

/******************* includes ****************************/

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in image serialization */

#define SURFACE_VERSION		0

/* flags */

#define SURFACE_PREDEFINED				0x00000001	// surface is one of the pre-defined types
#define SURFACE_END_OF_LIFE			0x00000002	// end-of-life "surface type" enables spawn settings without "particle collides" being checked
#define SURFACE_DEFAULT_COLLISION	0x00000004	// default collision surface type-- used when spawn settings not made for a particluar surface

/******************* structures **************************/

/******************* forward declarations ****************/

/******************* class *******************************/

class CSurface : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* array records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CSurface)

// Constructor/destructor

	CSurface();
	~CSurface();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;						// user-defined surface name (e.g. wood)
	unsigned long m_ulFlags;
	unsigned long m_ulVersion;

	/* not serialized */
	
	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetName()							{ return(m_Name); }
	void SetName(char *pName)					{ m_Name = pName; }
	void SetFlags(unsigned long ulFlags)	{ m_ulFlags |= ulFlags; }
	bool Predefined()								{ return((m_ulFlags & SURFACE_PREDEFINED) != 0); }
	bool EndOfLife()								{ return((m_ulFlags & SURFACE_END_OF_LIFE) != 0); }

	/* routines defined in .cpp */

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

// operations

public:

};

#endif // __SURFACE_H
