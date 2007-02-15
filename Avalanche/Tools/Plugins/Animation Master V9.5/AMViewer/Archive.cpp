//

#include "stdafx.h"
#include "Vector4.h"
#include "Matrix34.h"

CArchive &operator >> ( CArchive &ar, Vector2 &v )
{
    return ar >> v.x >> v.y;
}

CArchive &operator >> ( CArchive &ar, Vector &v )
{
    return ar >> v.x >> v.y >> v.z;
}

CArchive &operator >> ( CArchive &ar, Vector4 &v )
{
   float x,y,z,w;
   ar >> x >> y >> z >> w;
   v.Set( x,y,z,w );
   return ar;
}

CArchive &operator >> ( CArchive &ar, Matrix34 &m )
{
   Vector4 v0,v1,v2;
   ar >> v0 >> v1 >> v2;
   m.Set( v0,v1,v2 );
   return ar;
}

CString ReadString( CArchive &ar )
{
   char c;
   CString string;

   while (ar.Read(&c,1)) {
      if (c=='\0')
         break;
      string+=c;
   }
   return string;
}
