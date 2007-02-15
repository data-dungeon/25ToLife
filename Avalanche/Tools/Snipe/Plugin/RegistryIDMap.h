#pragma once

#include "../Utility/MemoryStream.h"

class RegistryIDMap
{
public:
   // CmdLookup
   class CmdLookup 
   {
   public:
      String m_strPluginName;
      String m_strClassName;

      CmdLookup() {}
      CmdLookup(const String &strPluginName, const String &strClassName) :
       m_strPluginName(strPluginName),
       m_strClassName(strClassName)
      {
      }

      CmdLookup & operator = ( const CmdLookup &other )
      {
         m_strPluginName = other.m_strPluginName;
         m_strClassName = other.m_strClassName;
         return *this;
      }

      BOOL operator == ( const CmdLookup &other ) const
      {
         return m_strPluginName == other.m_strPluginName &&
            m_strClassName == other.m_strClassName;
      }

      String GetString() const { return m_strPluginName + ":" + m_strClassName; }

      void Serialize( BaseStream &bs )
      {
         if (bs.IsInputStream()) {
            bs.ReadLine(m_strPluginName);
            bs.ReadLine(m_strClassName);
         }
         else {
            bs << m_strPluginName << nl; 
            bs << m_strClassName << nl;
         }
      }
   };

// RegistryIDMap
   CMap< CmdLookup, const CmdLookup &, UINT, UINT> m_mapCmdID;
   CArray<BOOL> m_usedcmdids;

   RegistryIDMap() 
   {
      Load();
   }

   ~RegistryIDMap() 
   {
      Save();
      Free();
   }
   void Load();
   void Save();
   void Free();

   UINT GetCmdID(const String &strPluginName, const String &strClassName);


   //CmdID///////////////////////////////
   class CmdID
   {
   public:
      UINT m_nID;
      CmdLookup m_lookup;

      CmdID() :
       m_nID((UINT)-1)
      {
      }

      CmdID(UINT nID, const String &strPluginName, const String &strClassName) :
       m_nID((UINT)-1),
       m_lookup(strPluginName,strClassName)
      {
      }

      void Serialize( BaseStream &bs )
      {
         if (bs.IsInputStream()) {
            bs >> m_nID;
         }
         else {
            bs << m_nID << nl;
         }
         m_lookup.Serialize( bs );
      }
   };
};

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const RegistryIDMap::CmdLookup &>( const RegistryIDMap::CmdLookup &key )
{
   return HashKey<const String &> (key.GetString());
}
#endif
