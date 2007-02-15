//

class Matrix34;
class Vector2;
class Vector;
class Vector4;

CArchive &operator >> ( CArchive &ar, Matrix34 &m );
CArchive &operator >> ( CArchive &ar, Vector2 &v );
CArchive &operator >> ( CArchive &ar, Vector &v );
CArchive &operator >> ( CArchive &ar, Vector4 &v );
CString ReadString( CArchive &ar );
