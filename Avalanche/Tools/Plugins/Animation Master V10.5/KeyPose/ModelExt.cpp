//

#include "Stdafx.h"
#include "SDK/HPropert.h"
#include "SDK/HModel.h"
#include "PropertyWnd.h"
#include "Hxt.h"
#include "ModelExt.h"

extern CHxtApp theApp;

ModelExt *GetModelExt()
{
   return theApp.m_wndKeyPose.m_modelext;
}

PropertyExt *GetPropertyExt(HProperty *property)
{
   ModelExt *modelext = GetModelExt();
   if (modelext)
      return modelext->GetPropertyExt(property);
   return NULL;
}

//****************************************************************
//****************************************************************
// PropertyExt
//****************************************************************
//****************************************************************
PropertyExt::~PropertyExt()
{
   if (m_wnd) {
      if (m_wnd->GetSafeHwnd())
         m_wnd->DestroyWindow();
      delete m_wnd;
   }
}

int PropertyExt::GetDefaultHeight()
{
   if (m_wnd)
      return m_wnd->GetDefaultHeight();//WHy this Crash
   else
      return 18;
}

BOOL PropertyExt::Serialize(HashStream &hs) 
{
   if (hs.IsStoring()) {
      hs << (BOOL)m_isexpanded;
   }
   else {
      BOOL bTemp;
      hs >> bTemp ;
      m_isexpanded = bTemp;
   }
   return TRUE;
}

//****************************************************************
//****************************************************************
// ModelExt
//****************************************************************
//****************************************************************

void ModelExt::FreeAllPropertyExtensions()
{
   for (POSITION pos = m_propertytoext.GetStartPosition(); pos; ) {
      HProperty *property;
      PropertyExt *propext;
      m_propertytoext.GetNextAssoc(pos, property, propext );
      m_propertytoext.RemoveKey(property);
      delete propext;
   }
}

BOOL ModelExt::FreePropertyExt(HProperty *property)
{
   PropertyExt *propext;
   if (m_propertytoext.Lookup(property, propext)) {
      delete propext;
      return m_propertytoext.RemoveKey(property);
   }
   return FALSE;
}

void ModelExt::BuildPropertyExtBranch(HTreeObject *obj)
{
   HProperty *property;
   for (int i=0; property = obj->GetPropertyAt(i); i++) {
      PropertyExt *propext = new PropertyExt;
      m_propertytoext.SetAt(property, propext);
      BuildPropertyExtBranch(property);
   }
}

void ModelExt::BuildPropertyExt(HModel *model)
{
   FreeAllPropertyExtensions();
   BuildPropertyExtBranch(model);
}

PropertyExt *ModelExt::GetPropertyExt(HProperty *property)
{
   PropertyExt *propext;
   if (m_propertytoext.Lookup(property, propext))
      return propext;

   return NULL;
}


BOOL ModelExt::Serialize(HashStream &hs) 
{
   if (hs.IsStoring()) {
      hs << MODEL_EXT_VERSION << m_posebarvertscrollpos << m_posebaractivetab;
      int count = m_propertytoext.GetCount();
      hs << count;
      for (POSITION pos = m_propertytoext.GetStartPosition(); pos; ) {
         HProperty *property;
         PropertyExt *propext;
         m_propertytoext.GetNextAssoc(pos, property, propext );
         hs << (int)property;
         propext->Serialize(hs);
      }
   }
   else {
      int version;
      hs >> version;
      if (version != MODEL_EXT_VERSION)
         return FALSE;
      hs >> m_posebarvertscrollpos >> m_posebaractivetab;
      int propextcount;
      hs >> propextcount;
      for (int i=0; i < propextcount; i++) {
         int address;
         HProperty *property;
         hs >> address;
         property = (HProperty *)address;

         PropertyExt *propext = new PropertyExt;
         if (!propext->Serialize(hs)) {
            delete propext;
            return FALSE;
         }
         m_propertytoext.SetAt(property, propext);
      }

   }
   return TRUE;
}

//****************************************************************
//****************************************************************
// CKeyPoseWnd
//****************************************************************
//****************************************************************
ModelExt *LoadModelExt(HModel *model)
{
   HDataProperty *dataproperty = theApp.GetDataProperty(model);
   
   DataValue datavalue = dataproperty->GetValue();

   if (datavalue.m_data==NULL)
      return NULL;

   MemoryStream ms;
   ms.Open((char *)datavalue.m_data, datavalue.m_length, HashStream::HS_LOAD | HashStream::HS_BINARY);

   ModelExt *modelext = new ModelExt;
   if (!modelext->Serialize(ms)) {
      delete modelext;
      return NULL;
   }
   return modelext;
}

ModelExt *CKeyPoseWnd::GetModelExt(HModel *model)
{
   ModelExt *ext = LoadModelExt(model);//Loads it from DataProperty, it is possible it hasn't been set yet
   if (ext)
      return ext;
   ext = new ModelExt;
   ext->BuildPropertyExt(model);
   return ext;
}

BOOL CKeyPoseWnd::SetModelExt(HModel *model, ModelExt *modelext)
{
   HDataProperty *dataproperty = theApp.GetDataProperty(model);

   MemoryStream ms;
   if (!ms.Open( HashStream::HS_BINARY | HashStream::HS_STORE ))
      return FALSE;

   if (!modelext->Serialize(ms))
      return FALSE;

   DataValue dv((BYTE *)ms.buf, ms.len);
   
   dataproperty->StoreValue(dv); 

   return TRUE;
}

