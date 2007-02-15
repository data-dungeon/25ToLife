#include <max.h>
#include <meshdlib.h>
#include <iparamb2.h> 
#include <modstack.h> 

class PerFaceDataMod;
class CPaintMode;

#include "resource.h"
#include "vNormals.h"
#include "FaceData.h"
#include "modData.h"
#include "perFaceDataMod.h"
#include "restore.h"
#include "paintMouseProc.h"
#include "paintMode.h"
#include "veroverrides.h"

// ---add a normal to the list if the smoothing group bits overlap, 
// ---otherwise create a new vertex normal in the list
void VNormal::AddNormal( Point3 &n, DWORD s ) 
{
	if (!(s&smooth) && init) 
	{
		if (next)	next->AddNormal( n, s );
		else		next = new VNormal( n, s );
	} 
	else 
	{
		norm   += n;
		smooth |= s;
		init    = TRUE;
	}
}

// ---retrieves a normal if the smoothing groups overlap or there is 
// ---only one in the list
Point3 &VNormal::GetNormal( DWORD s ) 
{
	if ( smooth & s || !next )	return norm;
	else						return next->GetNormal( s );	
}

// ---normalize each normal in the list
void VNormal::Normalize() 
{
	VNormal *ptr = next, *prev = this;

	while ( ptr ) 
	{
		if ( ptr->smooth&smooth ) 
		{
			norm += ptr->norm;
			prev->next = ptr->next;
			delete ptr;
			ptr = prev->next;
		} 
		else 
		{
			prev = ptr;
			ptr  = ptr->next;
		}
	}
	norm = ::Normalize( norm );
	if ( next ) next->Normalize();
}