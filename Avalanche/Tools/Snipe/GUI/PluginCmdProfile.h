#pragma once
#include "ExtCmdManager.h"

class CommandUIEntry;

class CExtPluginCmdItem : public CExtCmdItem
{
public:
   CString m_strPluginName;
   CString m_strClassName;
   CommandUIEntry *m_pEntry;
   union {
      ULONG m_nCmdFlags;
      struct {
         ULONG   m_bPluginCmd  : 1,
                 m_bValidCmdID : 1,
                 m_bNewCommand : 1;
      };
   };

   CExtPluginCmdItem() :
    m_pEntry(NULL),
    m_nCmdFlags(0)
   {
      m_bNewCommand = TRUE;
   }

   CExtPluginCmdItem( const CExtCmdItem & _cmd ) : 
    CExtCmdItem ( _cmd ),
    m_pEntry(NULL),
    m_nCmdFlags(0)
   {
   }
   virtual ~CExtPluginCmdItem() {}
};

class CExtPluginCmdProfile : public CExtCmdProfile
{
protected:
   CExtCmdItem *OnCreateCmdItem( const CExtCmdItem & _cmd );
public:
   virtual bool SerializeState( CArchive & ar );
};