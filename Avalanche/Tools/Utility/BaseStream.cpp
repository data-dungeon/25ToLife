#include "StdAfx.h"
#include "BaseStream.h"
#include "Vector.h"
#include "Vector2.h"
#include "RGBColor.h"
#include "Quaternion.h"
#include "ATime.h"

BaseStream::BaseStream()
{
   m_tokenarray = NULL;
   SetTokenSize(MAXSTRING);
   m_fProductVersion = 0.0f;
   m_nMargin = 0;
}

BaseStream::~BaseStream()
{
   if (m_tokenarray)
      free(m_tokenarray);
}

void BaseStream::WriteMargin()
{
   if (IsAsciiStream())
      for (int i=0; i < m_nMargin; i++)
         *this << ' ';
}

void BaseStream::ReadMargin()
{
   if (IsAsciiStream()) 
   {
      char c;
      while (Read(&c, sizeof(char))) 
      {
         if (c != 0x20) 
         {
            Seek(-1, ST_OFFSET);
            break;
         }
      }
   }
}

void BaseStream::SetTokenSize(UINT nSize)
{
   if (m_tokenarray)
      free(m_tokenarray);

   m_tokenarray = (char *)calloc(nSize, sizeof(char));
   m_nTokenSize = nSize;
}

// Insertion/Extraction Operators
BaseStream &operator << (BaseStream &bs, SBYTE value)
{
   static char buf[5];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%hd", ( SWORD ) value );
      bs << buf;
   } 
   else 
      bs.Write( &value, sizeof( SBYTE ));
   
   return bs;
}

BaseStream &operator >> (BaseStream &bs, SBYTE &value)
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (SBYTE)atoi( bs.m_tokenarray );
   } else 
      bs.Read( &value, sizeof( SBYTE ) );
   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, UBYTE value )
{
   static char buf[5];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%hu", ( UWORD ) value );
      bs << buf;
   } 
   else 
      bs.Write( &value, sizeof( UBYTE ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, UBYTE &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (UBYTE)atoi( bs.m_tokenarray );
   } else 
      bs.Read( &value, sizeof( UBYTE ) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, SWORD value )
{
   static char buf[7];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%hd", value );
      bs << buf;
   } 
   else 
      bs.Write( &value, sizeof( SWORD ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, SWORD &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (SWORD)atoi( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( SWORD ));

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, UWORD value )
{
   static char buf[7];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%hu", value );
      bs << buf;
   } 
   else 
      bs.Write( &value, sizeof( UWORD ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, UWORD &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (UWORD)atoi( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( UWORD ));

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, LONG value )
{
   static char buf[12];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%ld", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( LONG ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, LONG &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (LONG)atol( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( LONG ) );
   
   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, ULONG value )
{
   static char buf[12];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%lu", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( ULONG ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, ULONG &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (ULONG)atol( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( ULONG ));

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, float value )
{
   static char buf[20];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%g", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( float ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, float &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (float)atof( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( float ) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, double value )
{
   static char buf[20];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%.16g", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( double ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, double &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (double)atof( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( double ) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, int value )
{
   static char buf[12];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%d", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( int ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, int &value)
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (int)atoi( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( int ) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, UINT value )
{
   static char buf[12];
   
   if ( bs.IsAsciiStream() ) {
      sprintf( buf, "%d", value );
      bs << buf;
   } 
   else
      bs.Write( &value, sizeof( UINT ));

   return bs;
}

BaseStream &operator>>( BaseStream &bs, UINT &value )
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseTokenWhiteSpace();
      value = (UINT)atoi( bs.m_tokenarray );
   } 
   else
      bs.Read( &value, sizeof( UINT ) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, char value )
{
   bs.Write( &value, sizeof( char ));
   return bs;
}

BaseStream &operator>>( BaseStream &bs, char &value )
{
   bs.Read( &value, sizeof( char ) );
   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, char *value )
{
   if (bs.IsBinaryStream())
      bs << (UWORD) strlen( value );

   bs.Write( value, (UINT)strlen(value) );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator << ( BaseStream &bs, const Time &time )
{
   static char ctime[64], ctemp[8];

   long ticks = time.GetTicks();
   int minutes = 0;

   if (ticks < 0) {
      strcpy(ctime, "-");
      ticks = -ticks;
   }
   else
      ctime[0] = '\0';

   if (ticks >= Time::TICKSPERSECOND * 60)
   {
      minutes = ticks / (Time::TICKSPERSECOND * 60);
      sprintf( ctemp, "%d:", minutes);
      strcat( ctime, ctemp );
      ticks -= minutes * Time::TICKSPERSECOND * 60;
   }

   if (ticks >= Time::TICKSPERSECOND || minutes)
   {
      int seconds = ticks / (Time::TICKSPERSECOND);
      sprintf( ctemp, "%d:", seconds);
      strcat( ctime, ctemp );
      ticks -= seconds * Time::TICKSPERSECOND;
   }

   int frames = ticks / (Time::TICKSPERSECOND/30);
   sprintf( ctemp, "%d", frames);
   strcat( ctime, ctemp );
   ticks -= frames * (Time::TICKSPERSECOND/30);

   if (ticks)
   {
      sprintf( ctemp, ".%d", ticks);
      strcat( ctime, ctemp );
   }

   bs << ctime;

   return bs;
}

BaseStream &operator >> ( BaseStream &bs, Time &time )
{
   bs.ParseTokenWhiteSpace();

   int minutes=0, seconds=0, frames=0, ticks=0, sign=1;
   size_t len = strlen( bs.m_tokenarray );
   char *text = bs.m_tokenarray;
   char *start = text;
   BOOL periodfound = FALSE;

   if (*start == '-')
   {
      sign = -1;
      start++;
   }

   for (char *c=start; c<=text+len; c++)
   {
      if (c==text+len || *c==':' || *c=='.')
      {
         if (*c == '.')
            periodfound = TRUE;

         *c = '\0';

         minutes = seconds;
         seconds = frames;
         frames  = ticks;
         ticks   = atoi( start );

         start = c+1;
      }
   }

   if (!periodfound && ticks<30)
   {
      minutes = seconds;
      seconds = frames;
      frames  = ticks;
      ticks   = 0;
   }

   time.SetTicks( sign *
      minutes*60*Time::TICKSPERSECOND +
      seconds*Time::TICKSPERSECOND +
      frames*(Time::TICKSPERSECOND/30) +
      ticks );

   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const Vector2 &v )
{
   return bs << v.x << sp << v.y;
}

BaseStream &operator>>( BaseStream &bs, Vector2 &v )
{
   return bs >> v.x >> v.y;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const Vector &v )
{
   return bs << v.x << sp << v.y << sp << v.z;
}

BaseStream &operator>>( BaseStream &bs, Vector& v )
{
   return bs >> v.x >> v.y >> v.z;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const Quaternion &q )
{
   return bs << q[0] << sp << q[1] << sp << q[2] << sp << q[3];
}

BaseStream &operator>>( BaseStream &bs, Quaternion &q )
{
   return bs >> q[0] >> q[1] >> q[2] >> q[3];
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const Label &lb )
{
   if ( bs.IsAsciiStream() )
      return bs << lb.string << '=';
   else
      return bs << lb.id;
}

BaseStream &operator>>( BaseStream &bs, Label& lb)
{
   if ( bs.IsAsciiStream() ) {
      bs.ParseToken( '=' );
      strcpy( lb.string, bs.m_tokenarray );
      lb.id = GetLabelIDFromString( lb.string );
   } else {
      bs.Read( &lb.id, sizeof(LONG));
#ifdef _MAC
      FlipLong68k( (long *)&lb.id );
#endif
      GetLabelStringFromID( lb.id, lb.string );
   }
   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const Header &hd )
{
   if (bs.IsAsciiStream())
      return bs << (String)"[" + hd.string + "]";
   else
      return bs << hd.id;
}

BaseStream &operator>>( BaseStream &bs, Header &hd)
{
   if (bs.IsAsciiStream()) {
      char temp;
      bs.Read( &temp, 1 );
      bs.ParseToken( ']' );
      strcpy( hd.string, bs.m_tokenarray );
      hd.id = GetHeaderIDFromString( hd.string );
      if (hd.id==-1)
         hd.id = GetLabelIDFromString( hd.string );
   } else {
      bs.Read( &hd.id, sizeof(LONG));
#ifdef _MAC
      FlipLong68k( (long *)&hd.id );
#endif
      GetHeaderStringFromID( hd.id, hd.string );
   }
   return bs;
}

////////////////////////////////////////////////////////////

BaseStream &operator<<( BaseStream &bs, const StreamBOOL &sb )
{
   if (bs.IsAsciiStream()) {
      if (sb.m_bool)
         return bs << "TRUE";
      else
         return bs << "FALSE";
   } else
      return bs << (BYTE)sb.m_bool;
}


BaseStream &operator>>( BaseStream &bs, StreamBOOL &sb )
{
   if (bs.IsAsciiStream()) {
      bs.ParseTokenWhiteSpace();
      if (stricmp(bs.m_tokenarray, "ON")==0 || stricmp(bs.m_tokenarray, "TRUE")==0 || stricmp(bs.m_tokenarray, "1")==0)
         sb.m_bool = TRUE;
      else
         sb.m_bool = FALSE;
   } else {
      BYTE byte;
      bs >> byte;
      sb.m_bool = byte;
   }
   return bs;
}

///////////////////////////////////////////////////////////////////////////

BaseStream &mar( BaseStream& bs )
{
   bs.WriteMargin();
   return bs;
}

BaseStream &sp( BaseStream& bs )
{
   if ( bs.IsAsciiStream() )
      return bs << ' ';
   else
      return bs;
}

BaseStream &spb( BaseStream& bs )
{
   return bs << ' ';
}

BaseStream &nl( BaseStream& bs )
{
   if ( bs.IsAsciiStream() )
      return bs << '\x0A';
   else
      return bs;
}

BaseStream &tab( BaseStream& bs )
{
   if ( bs.IsAsciiStream())
      return bs << '\x09';
   else
      return bs;
}

BaseStream &tabb( BaseStream& bs )
{
   return bs << '\x09';
}

/////////// Labels/Headers

static LabelArray headerarray[] = // Keep'm Alphabetical
{
0,                         "ACTION",
};

static LabelArray labelarray[] = // IMPORTANT: Keep'm Alphabetical
{
0,      "ActionObjectTarget",
};

#if _MSC_VER >= 1100
template<> UINT AFXAPI HashKey<CharPtr &> (CharPtr &skey)
#else
UINT AFXAPI HashKey(CharPtr &skey)
#endif
{
   char *key = skey.m_buf;
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CLabelMap : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
   CLabelMap( LabelArray array[], UINT tablesize, UINT hashsize )
   {
      SetLabelArray( array, tablesize, hashsize );
   }

   void SetLabelArray( LabelArray array[], UINT tablesize, UINT hashsize )
   {
      RemoveAll();

      InitHashTable( hashsize );

      for (UINT i=0; i<tablesize; i++)
         SetAt( array[i].text, array[i].id );
//      UINT count = HighestCollision();
   }

   UINT HighestCollision()
   {
      int hi = 0, hits = 0, sum = 0;

   	for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
   	{
   		struct CLabelMap::Assoc* pAssoc;
   		int count = 0;
   		for (pAssoc = m_pHashTable[nHash]; pAssoc; pAssoc = pAssoc->pNext)
   		   count++;
         if (count) {
            sum += count;
            hits++;
         }
   	   if (count > hi)
   		   hi = count;
   	}
      float avg = (float)sum / hits;
   	return hi;
   }
};

static CLabelMap<CharPtr, CharPtr &, UINT, UINT> f_labelmap( labelarray, NUMELEMENTS(labelarray), 503 ); // prime number roughly 20% larger than table
static CLabelMap<CharPtr, CharPtr &, UINT, UINT> f_headermap( headerarray, NUMELEMENTS(headerarray), 751 ); // prime number roughly 20% larger than table

void SetLabelArray( LabelArray array[], UINT tablesize, UINT hashsize )
{
   f_labelmap.SetLabelArray( array, tablesize, hashsize );
}

void SetHeaderArray( LabelArray array[], UINT tablesize, UINT hashsize )
{
   f_labelmap.SetLabelArray( array, tablesize, hashsize );
}

LONG GetLabelIDFromString( char *string )
{
   UINT id;
   if (f_labelmap.Lookup( (CharPtr &)string, id ))
      return id;
   else
      return -1;
}

LONG GetHeaderIDFromString( char *string )
{
   UINT id;
   if (f_headermap.Lookup( (CharPtr &)string, id ))
      return id;
   else
      return -1;
}

static inline int GetHeaderIndexFromID( LONG pid )
{
   int i;
   for (i = 0; i < sizeof(headerarray)/sizeof(LabelArray)
    && (headerarray[i].id != pid); i++);
   if (i < sizeof(headerarray)/sizeof(LabelArray))
      return i;
   else
      return -1;
}

void GetHeaderStringFromID( LONG pid, char *string )
{
   int i = GetHeaderIndexFromID( pid );

   if (i == -1)
      sprintf( string, "HeaderNotFound: %d", pid );
   else
      strcpy( string, headerarray[i].text.m_buf );
}

static inline int GetLabelIndexFromID( LONG pid )
{
   int i;
   for (i = 0; i < sizeof(labelarray)/sizeof(LabelArray)
    && (labelarray[i].id != pid); i++);
   if (i < sizeof(labelarray)/sizeof(LabelArray))
      return i;
   else
      return -1;
}

BOOL GetLabelStringFromID( LONG pid, char *string )
{
   int i = GetLabelIndexFromID( pid );

   if (i == -1)
   {
      sprintf( string, "LabelNotFound: %d", pid );
      return FALSE;
   }
   else
   {
      strcpy( string, labelarray[i].text.m_buf );
      return TRUE;
   }
}
