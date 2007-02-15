/***********************************/
/*                                 */
/* Envelope.h                      */
/* Pigpen particle tool  12/20/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* behavior envelope class         */
/*                                 */
/***********************************/

#ifndef __ENVELOPE_H
#define __ENVELOPE_H

/******************* includes ****************************/

/* ImportExport includes */

/* game engine includes */

#include <Game/Managers/ParticleDB.h>

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in envelope serialization */

#define ENVELOPE_VERSION		0

/* control points that user can move around */

enum	{	SPLINE_POINT0 = 0,		// left endpoint
			SPLINE_POINT1,				// control point 1
			SPLINE_POINT2,				// control point 2
			SPLINE_POINT3,				// right endpoint
			FLOOR_POINT,
			CEILING_POINT,

			MAX_CONTROL_POINTS		// must be last
		};

/******************* structures **************************/

/******************* class *******************************/

class CEnvelope : public CObject
{
public:

// Constructor/destructor

	CEnvelope();
	~CEnvelope();

// Implementation

protected:

	/* keep serialization up-to-date! */

	unsigned long m_ulVersion;

	/* not serialized */

	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

public:

	Vector2 m_ControlPoints[MAX_CONTROL_POINTS];
	unsigned long m_ulBitfields;

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	int GetType(void) { return(m_ulBitfields & ENVELOPE_TYPE_MASK); }
	unsigned long GetLoadVersion(void) { return(m_ulLoadVersion); }

	/* routines defined in .cpp */

	bool IsValidControlPoint(int nIndex);
	float AdjustSplinePoint(float fY);
	void LoadFixup(void);
	void Straight100To0(void);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

// operations

public:

};

#endif // __ENVELOPE_H
