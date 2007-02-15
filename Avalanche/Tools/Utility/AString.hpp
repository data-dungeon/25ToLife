// String.hpp : implementation file

inline String::String( const String &init )
{
   if (init.m_pData && (m_pData = Alloc((init.Length()+1 ))))
      strcpy( m_pData, init.m_pData ); 
   else
      m_pData = NULL;    
}

inline String::String( const char *init )
{
   if (init && (m_pData = Alloc((strlen(init)+1 ))))
      strcpy( m_pData, init );
   else
      m_pData = NULL;    
}

inline String::String( char init )
{
   if (m_pData = Alloc( 2 )) {
      m_pData[0] = init;
      m_pData[1] = '\0';
   }   
}

inline String::String( char init, int count )
{
   if ( m_pData = Alloc(( count+1 ))) {
      for (int i = 0; i < count; ++i)
         m_pData[i] = init;
      m_pData[count] = '\0';
   }   
}

inline String::String( const char *init, int count )
{
   if ( m_pData = Alloc(( count+1 ))) {
      memcpy( m_pData, init, count );
      m_pData[count] = '\0';
   }   
}

inline String::String( short number )
{
   char init[20];
   
   sprintf( init, "%hd", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( LONG number )
{
   char init[20];
   
   sprintf( init, "%ld", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( ULONG number )
{
   char init[20];
   
   sprintf( init, "%lu", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( UINT number )
{
   char init[20];

   sprintf( init, "%lu", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( float number )
{
   char init[20];
   
   sprintf( init, "%g", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( double number )
{
   char init[20];
   
   sprintf( init, "%g", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( double number, double precision )
{
   char init[20];

   Round( number, precision );
   sprintf( init, "%g", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline String::String( int number )
{
   char init[20];
   
   sprintf( init, "%d", number );
   if (m_pData = Alloc(( strlen(init)+1 )))
      strcpy( m_pData, init ); 
}

inline UINT String::Length() const
{
   if (m_pData)
      return (UINT)strlen(m_pData); 
   else
      return 0;   
}

inline String operator + ( const String &left, const String &right )
{
   String result( left );
   result += right;
   return result;
}   

inline String &String::operator=(const char *other )
{
   Empty();
   if (other && (m_pData = Alloc((strlen(other)+1 ))))
      strcpy( m_pData, other );
   else
      m_pData = NULL;
   return *this;
}

inline void String::Set(const char *other, int len )
{
   Empty();
   if (other && len && (m_pData = Alloc(len+1 ))) {
      memcpy( m_pData, other, len );
      m_pData[len] = '\0';
   }
   else
      m_pData = NULL;
}

inline BOOL String::operator==(const String &other ) const 
{
   if (m_pData == NULL)
      return (other.m_pData == NULL);
   if (other.m_pData == NULL)
      return FALSE;
   return ( _stricmp( m_pData, other.m_pData ) == 0);
}

template<> AFX_INLINE UINT AFXAPI HashKey<const String &>( const String &key )
{
   UINT nHash = 0;
   const char *pData = key.Get();
   while (pData && *pData)
      nHash = (nHash<<5) + nHash + *pData++;
   return nHash;
}

inline String String::GetWithoutIntOnEnd() const
{
   int endint;
   String strTemp;

   StripIntOnEnd(endint, strTemp);
   return strTemp;
}

inline int String::GetIntOnEnd() const
{
   int endint;
   String strTemp;

   if (StripIntOnEnd(endint, strTemp))
      return endint;
   else
      return -1;
}

inline bool String::StripIntOnEnd(int &endint,String &substr) const
{
   String integerstr;
   substr = "";

   int len = Length();

   int i;
   for (i = 0; i < len; i++)
      if (isalpha((*this)[len-i-1]))
         break;

   if (i == len)
   {
      substr = *this;
      return false;
   }

   for (int j = 0; j < len-i;j++)
      substr+=(*this)[j];

   for (j = len-i; j < len; j++)
      integerstr+=(*this)[j];

   if (integerstr.IsEmpty())
      return false;

   endint = atoi(integerstr.m_pData);

   return true; 
}

inline String String::StripTabs()
{
   String temp = m_pData;
   temp.Replace("\t","");
   return temp;
}

inline String String::StripQuotes()
{
   String temp = m_pData;
   temp.Replace("\"","");
   return temp;
}

inline long String::AsInt() const
{
   return atol(m_pData);
}

inline unsigned long String::AsUnsigned() const
{
   unsigned long value;
   sscanf(m_pData,"%u",&value);
   return value;
}

inline float String::AsFloat() const
{
   float value;
   sscanf(m_pData,"%f",&value);
   return value;
}

inline double String::AsDouble() const
{
   float value;
   sscanf(m_pData,"%f",&value);
   return (double)value;
}

inline bool String::AsBool() const
{
   if (((*this) == "TRUE")||((*this) == "true"))
      return true;
   return false;
}

inline String StringArray::Combine(char c,int start,int stop)
{
   String combined = "";

   for (int i = start; i <= stop; i++)
   {
      combined += (*this)[i];
      if (i != stop)
         combined += c;
   }
   return combined;
}