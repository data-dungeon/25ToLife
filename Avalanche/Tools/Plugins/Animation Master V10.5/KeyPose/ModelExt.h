//

#ifndef __HASHFILE
#include "HashFile.h"
#endif
#include <AFXTEMPL.h>
#ifndef __HPROPERT_H
#include "sdk\HPropert.h"
#endif

class HModel;
class CPropertyWnd;
class PropertyExt
{
public:
   union {
      ULONG m_flags;
      struct {
         BITFIELDS32   m_isexpanded : 1,
                       m_unused1    : 1;
      };
      CPropertyWnd *m_wnd;
   };
   PropertyExt()
   {
      m_flags = 0;
      m_wnd = NULL;
   }
   ~PropertyExt();

   BOOL IsExpanded() { return m_isexpanded;}
   void SetExpanded(BOOL state) { m_isexpanded = state; }

   CPropertyWnd *GetPropertyWnd() { return m_wnd; }
//   void SetPropertyWnd(CPropertyWnd *wnd) {m_wnd = wnd;}

   int GetDefaultHeight();

   BOOL Serialize(HashStream &hs);
};

//************************************************************
#define MODEL_EXT_VERSION 1

class ModelExt
{
public:
   int m_posebarvertscrollpos;
   int m_posebaractivetab;
   CTypedPtrMap<CMapPtrToPtr, HProperty *, PropertyExt *> m_propertytoext;

   ModelExt() {
      m_posebarvertscrollpos = 0;
      m_posebaractivetab = 0;
   }

   ~ModelExt() {
      FreeAllPropertyExtensions();
   }


   void FreeAllPropertyExtensions();
   BOOL FreePropertyExt(HProperty *property);

   void BuildPropertyExtBranch(HTreeObject *obj);
   void BuildPropertyExt(HModel *model);

   virtual BOOL Serialize(HashStream &hs);

   PropertyExt *GetPropertyExt(HProperty *);
};

ModelExt *GetModelExt(HModel *model);
PropertyExt *GetPropertyExt(HProperty *property);
