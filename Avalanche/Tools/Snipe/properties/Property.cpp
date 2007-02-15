#include "stdafx.h"
#include "PropertyPointerProperty.h"
#include "PropertyArray.h"
#include "MotionCurveDrivers.h"
#include "resource.h"
#include "../Utility/FileDialog.h"

GLImageList Property::m_glImageListManipulatorKnob;

int BasicProperty<int>::m_prevvalue;
float BasicProperty<float>::m_prevvalue;
RGBFloat BasicProperty<RGBFloat>::m_prevvalue;
Vector BasicProperty<Vector>::m_prevvalue;
Vector2 BasicProperty<Vector2>::m_prevvalue;
Time BasicProperty<Time>::m_prevvalue;
String BasicProperty<String>::m_prevvalue;
SnipeObject *BasicProperty<SnipeObject *>::m_prevvalue;
Property *BasicProperty<Property *>::m_prevvalue;

///////////////////////////////////////////////////////////// 
// PropertyHeader

PropertyHeader::PropertyHeader( Property *pProperty, BOOL bWriteHeader, BaseStream &bs ) :
 SnipeObjectHeaderBase(pProperty, bWriteHeader && pProperty->IsSave(), bs)
{
   if (m_bWriteHeader) 
   {
      *m_pBS << mar << '<' << GetProperty()->GetMatchName() << '=';
      
      if (pProperty->IsDefaultValue() || !GetProperty()->WriteValue( *m_pBS ))
         m_pBS->Seek( -1, BaseStream::ST_OFFSET );

      *m_pBS << ">" << nl;
      m_nFilePos = m_pBS->GetPos();

      m_pBS->m_nMargin++;
   }
}

PropertyHeader::~PropertyHeader()
{
   if (m_bWriteHeader)
   {
      m_pBS->m_nMargin--;

      if (m_pBS->GetPos() != m_nFilePos) 
         *m_pBS << mar << "</" << GetProperty()->GetMatchName() << '>' << nl;
      else
      {
         m_pBS->Seek( -m_pBS->GetEOLSize()-1, BaseStream::ST_OFFSET );
         *m_pBS << "/>" << nl;
      }
   }
}

BOOL PropertyHeader::IsHeader( MemoryStream &ms )
{
   ms.ReadMargin();
   return (ms.GetBuffer()[ms.GetPos()] == '<');
}

BaseStream &operator >> ( BaseStream &bs, PropertyHeader &value )
{
   value.m_bBeginHeader = FALSE;
   value.m_bEndHeader = FALSE;

   bs.ReadMargin();

   char cTemp;
   bs >> cTemp; // '<'
   ASSERT( cTemp == '<' );

   int nCount = bs.ParseToken('>');

   if (bs.m_tokenarray[0] == '/') // end header </FOO>
   {
      value.m_bEndHeader = TRUE;
      value.m_strKey = &bs.m_tokenarray[1];
      value.m_strValue.Empty();
      return bs;
   }

   value.m_bBeginHeader = TRUE;

   if (bs.m_tokenarray[nCount-1] == '/') // begin and end header <FOO/> or <FOO=foo/>
   {
      bs.m_tokenarray[nCount-1] = '\0';
      value.m_bEndHeader = TRUE;
   }

   value.m_strKey = bs.m_tokenarray;
   int nEqualsPos = value.m_strKey.Find('=');
   if (nEqualsPos != -1)
   {
      value.m_strValue = value.m_strKey.Right( value.m_strKey.Length() - nEqualsPos - 1 );
      value.m_strKey = value.m_strKey.Left( nEqualsPos );
   }
   else
      value.m_strValue.Empty();

   return bs;
}

/////////////////////////////////////////
// Property::Info

Property::Info::Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags/*=PIF_DEFAULT*/ )
{
   m_strName = strName;
   m_strMatchName = strMatchName;

   // Default flag states
   m_nPropertyInfoFlags = 0;
   m_bVisible = TRUE;
   m_bUndoable = TRUE;

   // Set custom flags states
   if (nPropertyInfoFlags != PIF_DEFAULT)
   {
      if (nPropertyInfoFlags & PIF_HIDDEN)
         m_bVisible = FALSE;

      if (nPropertyInfoFlags & PIF_READONLY)
         m_bReadOnly = TRUE;

      if (nPropertyInfoFlags & PIF_NOUNDO)
         m_bUndoable = FALSE;

      if (nPropertyInfoFlags & PIF_SHOWVALUEWHENNOTSET)
         m_bShowValueWhenNotSet = TRUE;
   }
}

/////////////////////////////////////////
// Property

IMPLEMENT_SNIPEOBJECT_CREATE( Property )

Property::Property()
{
   m_pParent = NULL;
   m_pCache = NULL;
   m_pPropertyInfo = NULL;
   m_pPropertyDriverHead = NULL;
   m_nPropertyFlags = 0;
   m_bSet = TRUE;
   m_bCompositedValueDirty = TRUE;
}

Property::~Property()
{
}

BOOL Property::IsCompositedValueDirty( Time time ) const
{
   if (m_bCompositedValueDirty)
      return TRUE;

   for (PropertyDriver *pPropertyDriver=GetPropertyDriverHead(); pPropertyDriver; pPropertyDriver=pPropertyDriver->GetNextPropertyDriver())
   {
      if (pPropertyDriver->IsApplyDirty( time ))
         return TRUE;
   }

   return FALSE;
}

void Property::CompositeDrivers( Time time )
{
   if (IsInstance())
   {
      PropertyDriver *pPropertyDriver = GetPropertyDriverHead();
      if (pPropertyDriver)
      {
         for (; pPropertyDriver; pPropertyDriver=pPropertyDriver->GetNextPropertyDriver())
            pPropertyDriver->Apply( time );
      }
      else if (GetCache()->IsVisible())
         CopyCacheValue();
   }

   m_bCompositedValueDirty = FALSE;
}

void Property::PreDestruct()
{
   m_bIsPreDestructed = TRUE;

   PropertyObject::PreDestruct();
}

void Property::PostConstruct()
{
   PropertyObject::PostConstruct();

   m_bIsPostConstructed = TRUE;
}

void Property::Init( Property::Info *pInfo, PropertyObject *pParent )
{
   m_pPropertyInfo = pInfo;
   
   m_bVisible = pInfo->m_bVisible;

   if (pParent)
      pParent->AddProperty(this);
}

String Property::GetTitle() const
{
   if (m_pPropertyInfo)
      return m_pPropertyInfo->m_strName;
   else
      return "No Property Info!";
}

String Property::GetMatchName() const
{
   if (m_pPropertyInfo)
      return m_pPropertyInfo->m_strMatchName;
   else
      return "No Property Info!";
}

void Property::StoreValueIndexFromString( const String &strValue, int nComponentIndex/*=0*/, const Time &time/*=TAGGEDTIME*/)
{
   ASSERT(nComponentIndex==0);
   DoStoreValueCommand( this, strValue, time );
}

void Property::GetValueIndexString( String &strValue, int nComponentIndex/*=0*/ )
{
   ASSERT(nComponentIndex==0);
   GetValueString( strValue );
}

void Property::GetValueString( String &strOutValue, const Time &time/*=GetTime()*/ )
{
   //TODO, use time!!!!
   if (IsSet() || IsShowValueWhenNotSet())
   {
      MemoryStream ms;
      ms.Open(BaseStream::BS_OUTPUT);
      WriteValue(ms);
      ms << '\0';
      strOutValue = (char *)ms.GetBuffer();
   }
   else
   {
      static String strNotSet = String::GetResourceString( IDS_NOTSET );
      strOutValue = strNotSet;
   }
}

BOOL Property::IsDefaultValue( BOOL bIncludeChildren/*=FALSE*/ )
{
   if (bIncludeChildren)
   {
      for (int i=0; i<m_propertyarray.GetCount(); i++) 
      { 
         Property *pChildProperty = m_propertyarray[i];
         if (!pChildProperty->IsDefaultValue(TRUE))
            return FALSE;
      }
   }
   return TRUE;
}

void Property::SendNotificationToDependents( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   for (SnipeObject *pParent=m_pParent; pParent; pParent=pParent->GetParent())
   {
      pParent->OnObjectNotify( msg, wParam, lParam );
      if (!pParent->IsProperty())
         break;
   }
}

void Property::OnValueChanged( BOOL bStoreUndo/*=FALSE*/ )
{
   Validate();

   OnObjectNotify( NOTIFYOBJ_VALUECHANGED, bStoreUndo, (LPARAM)this );
   SendNotificationToDependents( NOTIFYOBJ_VALUECHANGED, bStoreUndo, (LPARAM)this );
}

void Property::OnValueStored( const Time &time, BOOL bStoreUndo )
{
   m_bSet = TRUE;

   OnValueChanged( bStoreUndo );

   PropertyDriver *pPropertyDriver = CreateStorageDriver();
   if (pPropertyDriver)
      pPropertyDriver->OnPropertyValueStored( time, bStoreUndo );
}

BOOL Property::IsSave()
{
   return !IsDefaultValue(TRUE);
}

void Property::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   PropertyHeader header( this, bWriteHeader, bs );

   PropertyObject::Save( bs );
}

SnipeObject *Property::GetObject()
{
   for (SnipeObject *pParent=GetParent(); pParent; pParent=pParent->GetParent())
   {
      if (!pParent->IsProperty())
         return pParent;
   }

   return NULL;
}

UINT Property::GetNumPropertyDrivers( SnipeObjectType nSOT/*=SOT_PropertyDriver*/ ) const
{
   UINT nNumPropertyDrivers = 0;

   for (PropertyDriver *pPropertyDriver=GetPropertyDriverHead(); pPropertyDriver; pPropertyDriver=pPropertyDriver->GetNextPropertyDriver())
   {
      if (pPropertyDriver->IsKindOf( nSOT ) && pPropertyDriver->IsInProject())
         nNumPropertyDrivers++;
   }

   return nNumPropertyDrivers;
}

PropertyDriver *Property::GetPropertyDriverAt( UINT nIndex, SnipeObjectType nSOT/*=SOT_PropertyDriver*/ ) const
{
   UINT nPropertyDriverIndex = 0;

   for (PropertyDriver *pPropertyDriver=GetPropertyDriverHead(); pPropertyDriver; pPropertyDriver=pPropertyDriver->GetNextPropertyDriver())
   {
      if (pPropertyDriver->IsKindOf( nSOT ) && pPropertyDriver->IsInProject())
      {
         if (nPropertyDriverIndex == nIndex)
            return pPropertyDriver;
         nPropertyDriverIndex++;
      }
   }

   return NULL;
}

PropertyDriver *Property::CreateStorageDriver()
{
   SnipeObject *pObject = GetObject();
   if (!pObject || pObject->IsCache())
      return NULL;

   SnipeObjectInfo *pInfo = GetStorageDriverInfo();
   if (!pInfo)
      return NULL;

   for (NotifyNode *pNotifyNode=m_pNotifyNodeHead; pNotifyNode; pNotifyNode=pNotifyNode->m_pNextNotifyNode)
   {
      SnipeObject *pNodeObject = pNotifyNode->GetSnipeObject();
      if (pNodeObject && pNodeObject->GetSOT()==SOT_PropertyPointerProperty)
      {
         PropertyPointerProperty *pPropertyPointerProperty = (PropertyPointerProperty *)pNodeObject;
         SnipeObject *pObject = pPropertyPointerProperty->GetObject();
         if (pObject->IsKindOf( pInfo->m_nSOT ))
         {
            PropertyDriver *pPropertyDriver = (PropertyDriver *)pObject;
            if (pPropertyDriver->m_bCurrent)
               return pPropertyDriver;
         }
      }
   }

   PropertyDriver *pPropertyDriver = (PropertyDriver *)pInfo->CreateObject();
   ASSERT( pPropertyDriver->IsKindOf(SOT_PropertyDriver) );

   ASSERT( pObject->IsKindOf(SOT_HierObject) );
   HierObject *pHierObject = (HierObject *)pObject;
   pHierObject->AddChildAtTail( pPropertyDriver );

   pPropertyDriver->m_pProperty.StoreValue( this );

   return pPropertyDriver;
}

void Property::AddPropertyDriver( PropertyDriver *pPropertyDriver )
{
   for (PropertyDriver **ppPropertyDriver=&m_pPropertyDriverHead; *ppPropertyDriver; ppPropertyDriver=&(*ppPropertyDriver)->m_pNextPropertyDriver)
      ;
   *ppPropertyDriver = pPropertyDriver;
}

BOOL Property::RemovePropertyDriver( PropertyDriver *pPropertyDriver )
{
   for (PropertyDriver **ppPropertyDriver=&m_pPropertyDriverHead; *ppPropertyDriver; ppPropertyDriver=&(*ppPropertyDriver)->m_pNextPropertyDriver)
   {
      if (*ppPropertyDriver == pPropertyDriver)
      {
         *ppPropertyDriver = pPropertyDriver->m_pNextPropertyDriver;
         return TRUE;
      }
   }

   return FALSE;
}

/////////////////////////////////////////
// IntProperty

IMPLEMENT_SNIPEOBJECT_CREATE( IntProperty )

void IntProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/, BOOL bStoreUndo /*=FALSE*/ )
{
   String strValue( GetValue() + nPixels );
   DoStoreValueCommand( this, strValue, TAGGEDTIME, !bStoreUndo );
}

/////////////////////////////////////////
// BoolProperty

IMPLEMENT_SNIPEOBJECT_CREATE( BoolProperty )

BOOL BoolProperty::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty==this && GetPropertyInfo()->m_bAutoHideChildren)
            ShowProperties(GetValue());
         break;
      }
      case NOTIFYOBJ_ADDPROPERTY:
      {
         Property *pProperty = (Property *)lParam;
         if (GetPropertyInfo()->m_bAutoHideChildren)
            pProperty->SetVisible(GetValue());
         break;
      }
   }
   return BasicProperty<BOOL>::OnObjectNotify( msg, wParam, lParam );
}

BOOL BoolProperty::WriteValue( BaseStream &bs ) 
{
   bs << StreamBOOL(GetValue()); 
   return TRUE;
}

void BoolProperty::ReadValue( BaseStream &bs ) 
{
   StreamBOOL value;
   bs >> value; 
   SetValue( value.m_bool );
}

/////////////////////////////////////////
// TypeProperty

IMPLEMENT_SNIPEOBJECT_CREATE( TypeProperty )

BOOL TypeProperty::WriteValue( BaseStream &bs )
{
   bs << GetPropertyInfo()->GetNodeArray()[ GetValue() ].GetMatchName(); 
   return TRUE;
}

static int GetValueFromStream( TypeProperty *pProperty, BaseStream &bs )
{
   String strValue;
   bs >> strValue;

   int nCount = pProperty->GetPropertyInfo()->GetNodeCount();
   for (int i=0; i < nCount; i++) 
   {
      if (pProperty->GetPropertyInfo()->GetNodeArray()[i].GetMatchName() == strValue) 
         return i;
   }
   return -1;
}

void TypeProperty::ReadValue( BaseStream &bs )
{
   String strValue;
   bs >> strValue;
   
   int nCount = GetPropertyInfo()->GetNodeCount();
   for (int i=0; i < nCount; i++) 
   {
      if (GetPropertyInfo()->GetNodeArray()[i].GetMatchName() == strValue) 
      {
         SetValue( i );
         return;
      }
   }
   String strMsg("Invalid value [%1] for property [%2].");
   strMsg.Replace("%1", strValue);
   strMsg.Replace("%2", GetMatchName());
   THROW_FATAL_ERROR( strMsg );
}

/////////////////////////////////////////
// TimeProperty

IMPLEMENT_SNIPEOBJECT_CREATE( TimeProperty )

void TimeProperty::Validate()
{
   if (GetValue() > GetPropertyInfo()->m_maxvalue)
      SetValue( GetPropertyInfo()->m_maxvalue );
   if (GetValue() < GetPropertyInfo()->m_minvalue)
      SetValue( GetPropertyInfo()->m_minvalue );
}

void TimeProperty::GetValueString( String &strOutValue, const Time &time/*=GetTime()*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
      strOutValue = GetValue().ToString( GetFPS(), GetTimeUnits() );
   else
      return BasicProperty<Time>::GetValueString( strOutValue, time );
}

void TimeProperty::StoreValueFromString( const String &strValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=TRUE*/)
{
   static String strNotSet = String::GetResourceString( IDS_NOTSET );

   if (strValue == strNotSet)
      BasicProperty<Time>::StoreValueFromString( strNotSet, time, bStoreUndo );
   else
      StoreValue( strValue.GetTime( GetFPS(), GetTimeUnits() ), time, bStoreUndo );
}

void TimeProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/, BOOL bStoreUndo /*= FALSE*/ )
{
   Time fNewTime;
   fNewTime.SetTicks( GetValue().GetTicks() + (double)nPixels*Time::TICKSPERSECOND / GetFPS() );

   if (IsUndoable())
   {
      String strValue( fNewTime.GetSeconds() );
      DoStoreValueCommand( this, strValue, TAGGEDTIME, !bStoreUndo );
   }
   else
      StoreValue( fNewTime.GetSeconds(), GetTime(), FALSE );
}

/////////////////////////////////////////
// FloatProperty

IMPLEMENT_SNIPEOBJECT_CREATE( FloatProperty )

SnipeObjectInfo *FloatProperty::GetStorageDriverInfo()
{
   static SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( SOT_FloatMotionCurveDriver );
   return pInfo;
}

void FloatProperty::Validate()
{
   if (GetValue() > GetPropertyInfo()->m_maxvalue)
      SetValue( GetPropertyInfo()->m_maxvalue );
   if (GetValue() < GetPropertyInfo()->m_minvalue)
      SetValue( GetPropertyInfo()->m_minvalue );
}

void FloatProperty::GetValueString( String &strOutValue, const Time &time/*=GetTime()*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
      strOutValue = UnitToString( GetFactoredValue(), GetMeasurementUnits() );
   else
      return FactoredProperty<float>::GetValueString( strOutValue, time );
}

void FloatProperty::StoreValueFromString( const String &strValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=TRUE*/)
{
   static String strNotSet = String::GetResourceString( IDS_NOTSET );

   if (strValue == strNotSet)
      FactoredProperty<float>::StoreValueFromString( strNotSet, time, bStoreUndo );
   else
      StoreValue( strValue.GetCM() / GetFactor(), time, bStoreUndo );
}

void FloatProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/, BOOL bStoreUndo /*= FALSE*/ )
{
   String strValue( fast_round(GetFactoredValue()) + nPixels );
   DoStoreValueCommand( this, strValue, TAGGEDTIME, !bStoreUndo );
}

/////////////////////////////////////////
// ColorProperty

IMPLEMENT_SNIPEOBJECT_CREATE( ColorProperty )

/////////////////////////////////////////
// Vector2Property

IMPLEMENT_SNIPEOBJECT_CREATE( Vector2Property )

SnipeObjectInfo *Vector2Property::GetStorageDriverInfo()
{
   static SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( SOT_Vector2MotionCurveDriver );
   return pInfo;
}

void Vector2Property::GetValueString( String &strOutValue, const Time &time/*=GetTime()*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
   {
      Vector2 vFactoredValue = GetFactoredValue();
      strOutValue = UnitToString( vFactoredValue.x, GetMeasurementUnits() )
            + " " + UnitToString( vFactoredValue.y, GetMeasurementUnits() );
   }
   else
      return FactoredProperty<Vector2>::GetValueString( strOutValue, time );
}

void Vector2Property::StoreValueFromString( const String &strValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=TRUE*/)
{
   static String strNotSet = String::GetResourceString( IDS_NOTSET );

   if (strValue == strNotSet)
      FactoredProperty<Vector2>::StoreValueFromString( strNotSet, time, bStoreUndo );
   else
   {
      Vector2 vValue = GetValue();

      StringArray stringarray;
      strValue.Split( ' ', stringarray );
      int nNumElements = min( 2, stringarray.GetCount());
      for (int i=0; i<nNumElements; i++)
         vValue[i] = stringarray[i].GetCM() / GetFactor();

      StoreValue( vValue, time, bStoreUndo );
   }
}

void Vector2Property::GetValueIndexString( String &strValue, int nComponentIndex/*=0*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
      strValue = UnitToString( GetFactoredValue()[nComponentIndex], GetMeasurementUnits() );
   else
      return FactoredProperty<Vector2>::GetValueString( strValue );
}

void Vector2Property::StoreValueIndexFromString( const String &strOrigValue, int nComponentIndex/*=0*/, const Time &time/*=TAGGEDTIME*/ )
{
   float fValue;
   sscanf( strOrigValue.Get(), "%f", &fValue );
   Vector2 vValue = GetValue();
   vValue[nComponentIndex] = fValue / GetFactor();

   String strValue = UnitToString( vValue.x * GetFactor(), GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.y * GetFactor(), GetMeasurementUnits(), 0.0f );

   DoStoreValueCommand( this, strValue, time );
}

void Vector2Property::OnScrub( int nPixels, int nComponentIndex/*=0*/, BOOL bStoreUndo /*=FALSE*/  )
{
   Vector2 vValue = GetFactoredValue();
   vValue[nComponentIndex] += nPixels;

   String strValue = UnitToString( vValue.x, GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.y, GetMeasurementUnits(), 0.0f );
   DoStoreValueCommand( this, strValue, TAGGEDTIME, !bStoreUndo );
}

/////////////////////////////////////////
// VectorProperty

IMPLEMENT_SNIPEOBJECT_CREATE( VectorProperty )

SnipeObjectInfo *VectorProperty::GetStorageDriverInfo()
{
   static SnipeObjectInfo *pInfo = SnipeObjectInfo::GetBySOT( SOT_VectorMotionCurveDriver );
   return pInfo;
}

void VectorProperty::GetValueString( String &strOutValue, const Time &time/*=GetTime()*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
   {
      Vector vFactoredValue = GetFactoredValue();
      strOutValue = UnitToString( vFactoredValue.x, GetMeasurementUnits() )
            + " " + UnitToString( vFactoredValue.y, GetMeasurementUnits() )
            + " " + UnitToString( vFactoredValue.z, GetMeasurementUnits() );
   }
   else
      return FactoredProperty<Vector>::GetValueString( strOutValue, time );
}

void VectorProperty::StoreValueFromString( const String &strValue, const Time &time/*=GetTime()*/, BOOL bStoreUndo/*=TRUE*/)
{
   static String strNotSet = String::GetResourceString( IDS_NOTSET );

   if (strValue == strNotSet)
      FactoredProperty<Vector>::StoreValueFromString( strNotSet, time, bStoreUndo );
   else
   {
      Vector vValue = GetValue();

      StringArray stringarray;
      strValue.Split( ' ', stringarray );
      int nNumElements = min( 3, stringarray.GetCount());
      for (int i=0; i<nNumElements; i++)
         vValue[i] = stringarray[i].GetCM() / GetFactor();

      StoreValue( vValue, time, bStoreUndo );
   }
}

void VectorProperty::GetValueIndexString( String &strValue, int nComponentIndex/*=0*/ )
{
   if (IsSet() || IsShowValueWhenNotSet())
      strValue = UnitToString( GetFactoredValue()[nComponentIndex], GetMeasurementUnits() );
   else
      return FactoredProperty<Vector>::GetValueString( strValue );
}

void VectorProperty::StoreValueIndexFromString( const String &strOrigValue, int nComponentIndex/*=0*/, const Time &time/*=TAGGEDTIME*/ )
{
   float fValue;
   sscanf( strOrigValue.Get(), "%f", &fValue );
   Vector vValue = GetValue();
   vValue[nComponentIndex] = fValue / GetFactor();

   String strValue = UnitToString( vValue.x * GetFactor(), GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.y * GetFactor(), GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.z * GetFactor(), GetMeasurementUnits(), 0.0f );

   DoStoreValueCommand( this, strValue, time );
}

void VectorProperty::OnScrub( int nPixels, int nComponentIndex/*=0*/, BOOL bStoreUndo /*=FALSE*/  )
{
   Vector vValue = GetFactoredValue();
   vValue[nComponentIndex] += nPixels;

   String strValue = UnitToString( vValue.x, GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.y, GetMeasurementUnits(), 0.0f )
             + " " + UnitToString( vValue.z, GetMeasurementUnits(), 0.0f );

   DoStoreValueCommand( this, strValue, TAGGEDTIME, !bStoreUndo );
}

/////////////////////////////////////////
// TransformProperty

IMPLEMENT_SNIPEOBJECT_CREATE( TransformProperty )

TranslateProperty::Info TransformProperty::m_vTranslateInfo( "Translate", "Translate" );
ScaleProperty::Info TransformProperty::m_vScaleInfo( "Scale", "Scale" );
RotateProperty::Info TransformProperty::m_vRotateInfo( "Rotate", "Rotate" );

TransformProperty::TransformProperty()
{
   m_vTranslate.Init( &m_vTranslateInfo, this );
   m_vScale.Init( &m_vScaleInfo, this );
   m_vRotate.Init( &m_vRotateInfo, this );

   m_tPrevTime.SetTagged();
   m_bMatrixDirty = TRUE;
}

void TransformProperty::ComputeMatrices( const Time &time )
{
   m_matrix.TranslationMatrix(SMLVec3f(m_vTranslate.GetValue().x, m_vTranslate.GetValue().y, m_vTranslate.GetValue().z));

   Matrix44 temp;
   temp.RotationMatrix( m_vRotate.GetValue(time).y * DEGTORAD, 0, 1, 0 );//y rotation
   m_matrix *= temp;
   temp.RotationMatrix( m_vRotate.GetValue(time).x * DEGTORAD, 1, 0, 0 );//x rotation
   m_matrix *= temp;
   temp.RotationMatrix( m_vRotate.GetValue(time).z * DEGTORAD, 0, 0, 1 );//z rotation
   m_matrix *= temp;

   temp.ScaleMatrix(SMLVec3f(m_vScale.GetValue(time).x, m_vScale.GetValue(time).y, m_vScale.GetValue(time).z));
   m_matrix *= temp;

   m_inversematrix.Invert( m_matrix );

   m_tPrevTime = time;
   m_bMatrixDirty = FALSE;
}

const Matrix44 &TransformProperty::GetMatrix( const Time &time )
{
   if (m_bMatrixDirty || time!=m_tPrevTime)
      ComputeMatrices( time );

   return m_matrix;
}

const Matrix44 &TransformProperty::GetInverseMatrix( const Time &time )
{
   if (m_bMatrixDirty || time!=m_tPrevTime)
      ComputeMatrices( time );

   return m_inversematrix;
}

BOOL TransformProperty::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty->GetParent()==this)
            m_bMatrixDirty = TRUE;
         break;
      }
   }

   return Property::OnObjectNotify( msg, wParam, lParam );
}

/////////////////////////////////////////
// Transform2Property

IMPLEMENT_SNIPEOBJECT_CREATE( Transform2Property )

Vector2Property::Info Transform2Property::m_v2TranslateInfo( "Translate", "Translate", 0, -MAX_FLOAT, MAX_FLOAT, 1.0f, Vector2Property::Info::FPT_UNITS );
Vector2Property::Info Transform2Property::m_v2ScaleInfo( "Scale", "Scale", 1, -MAX_FLOAT, MAX_FLOAT, 100.0f, Vector2Property::Info::FPT_PERCENT );
FloatProperty::Info Transform2Property::m_fRotateInfo( "Rotate", "Rotate", 0, -MAX_FLOAT, MAX_FLOAT, 1.0f, FloatProperty::Info::FPT_ANGLE );

Transform2Property::Transform2Property()
{
   m_v2Translate.Init( &m_v2TranslateInfo, this );
   m_v2Scale.Init( &m_v2ScaleInfo, this );
   m_fRotate.Init( &m_fRotateInfo, this );

   m_bMatrixDirty = TRUE;
}

void Transform2Property::ComputeMatrices()
{
   m_matrix.Identity();

   Matrix44 temp;
   temp.TranslationMatrix(SMLVec3f(m_v2Translate.GetValue().x, m_v2Translate.GetValue().y, 0.0f));
   m_matrix *= temp;

   temp.RotationMatrix( m_fRotate * DEGTORAD, 0, 0, 1 );
   m_matrix *= temp;

   temp.ScaleMatrix(SMLVec3f(m_v2Scale.GetValue().x, m_v2Scale.GetValue().y, 1.0f));
   m_matrix *= temp;

#if 1
   m_inversematrix.Identity();
   
   temp.TranslationMatrix(SMLVec3f(-m_v2Translate.GetValue().x, -m_v2Translate.GetValue().y, 1.0f));
   m_inversematrix *= temp;

   temp.RotationMatrix( -m_fRotate * DEGTORAD, 0, 0, 1 );
   m_inversematrix *= temp;

   temp.ScaleMatrix(SMLVec3f(1.0f/m_v2Scale.GetValue().x, 1.0f/m_v2Scale.GetValue().y, 1.0f));
   m_inversematrix *= temp;
#else
   m_inversematrix.Invert( m_matrix );
#endif

   m_bMatrixDirty = FALSE;
}

const Matrix44 &Transform2Property::GetMatrix()
{
   if (m_bMatrixDirty)
      ComputeMatrices();

   return m_matrix;
}

const Matrix44 &Transform2Property::GetInverseMatrix()
{
   if (m_bMatrixDirty)
      ComputeMatrices();

   return m_inversematrix;
}

BOOL Transform2Property::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg) {
      case NOTIFYOBJ_VALUECHANGED:
         {
            Property *pProperty = (Property *)lParam;
            if (pProperty->GetParent()==this)
               m_bMatrixDirty = TRUE;
            break;
         }
   }

   return Property::OnObjectNotify( msg, wParam, lParam );
}

/////////////////////////////////////////
// TranslateProperty

IMPLEMENT_SNIPEOBJECT_CREATE( TranslateProperty )

/////////////////////////////////////////
// ScaleProperty

IMPLEMENT_SNIPEOBJECT_CREATE( ScaleProperty )

/////////////////////////////////////////
// RotateProperty

IMPLEMENT_SNIPEOBJECT_CREATE( RotateProperty )

/////////////////////////////////////////
// StringProperty

IMPLEMENT_SNIPEOBJECT_CREATE( StringProperty )

////////////////////////////////////////
// FileNameProperty

IMPLEMENT_SNIPEOBJECT_CREATE( FileNameProperty )

FileNameProperty::Info::Info( const String &strName, const String &strMatchName, const String &strDefaultFileName, const String &strFilters, const String &strRegistry, UINT nFileNamePropertyInfoFlags/*=FILENAME_PIF_DEFAULT*/, UINT nPropertyInfoFlags/*=PIF_DEFAULT*/ )
: StringProperty::Info( strName, strMatchName, strDefaultFileName, nPropertyInfoFlags )
{
   m_strFilters = strFilters;
   m_strRegistry = strRegistry;

   // Default FileNamePropertyInfoFlags states
   m_nFileNamePropertyInfoFlags = 0;

   // Custom FileNamePropertyInfoFlags states
   if (nFileNamePropertyInfoFlags != FILENAME_PIF_DEFAULT) {
      if (nFileNamePropertyInfoFlags & FILENAME_PIF_OPENDIALOG)
         m_bOpenDialog = TRUE;
   }
}

void FileNameProperty::GetValueIndexString( String &strValue, int nComponentIndex/*=0*/ )
{
   if (!IsSet()) {
      static String strNotSet = String::GetResourceString( IDS_NOTSET );
      strValue = strNotSet;
      return;
   }
   else
      StringProperty::GetValueIndexString( strValue, nComponentIndex );
}

BOOL FileNameProperty::GetFileName( String &strOutFileName, BOOL bForceQuery/*=FALSE*/ )
{
   if (!IsSet() || bForceQuery) {
      String strPath;
      if (!IsSet())
         strPath = (LPCTSTR)::AfxGetApp()->GetProfileString("Files", GetPropertyInfo()->m_strRegistry.Get(), "BOGUS");
      else
         strPath = GetValue();

      if (strPath == "BOGUS")
         strPath = GetPropertyInfo()->m_defaultvalue;
      else
         strPath = strPath + "\\" + GetPropertyInfo()->m_defaultvalue;

      if (!GetSaveFileName( strPath, GetPropertyInfo()->m_strFilters, strPath))
         return FALSE;

      StoreValue( strPath );
      ::AfxGetApp()->WriteProfileString("Files", GetPropertyInfo()->m_strRegistry.Get(), strPath.GetPathOnly().Get());
   }
   strOutFileName = GetValue();
   return TRUE;
}

////////////////////////////////////////
// FileInfoProperty::Info
FileInfoProperty::Info::Info( const String &strName, const String &strMatchName, const String &strDefaultFileName, const String &strFilters, const String &strRegistry, UINT nPropertyInfoFlags/*=PIF_DEFAULT*/ ) :
 Property::Info( strName, strMatchName, nPropertyInfoFlags ),
 m_strFileNameInfo("FileName", "FileName", strDefaultFileName, strFilters, strRegistry, FILENAME_PIF_DEFAULT, PIF_READONLY )
{
}

////////////////////////////////////////
// FileInfoProperty
BoolProperty::Info FileInfoProperty::m_bEmbedInfo( "Embedded", "Embedded", TRUE );

IMPLEMENT_SNIPEOBJECT_CREATE( FileInfoProperty );

FileInfoProperty::FileInfoProperty()
{
   m_bEmbed.Init( &m_bEmbedInfo, this );
}

void FileInfoProperty::Init( Info *pInfo, PropertyObject *pParent )
{
   Property::Init( pInfo, pParent );
   m_strFileName.Init( &pInfo->m_strFileNameInfo, this);
}

////////////////////////////////////////
// ScaleProperty
void ScaleProperty::DrawManipulator( CGLBaseView *pView, const Matrix44 &matrix, UINT nAxis/*=AXIS_X|AXIS_Y|AXIS_Z*/ )
{
   GLDrawPrepper prepper( pView ); // push 2d projection

   if (m_glImageListManipulatorKnob.IsEmpty())
      m_glImageListManipulatorKnob.Create( IDB_SLIDERKNOB, 14 );
   //m_glImageListManipulatorKnob.SetTint( RGBFloat(1,1,0) );

   int nWidth = m_glImageListManipulatorKnob.GetWidthEach();
   int nHeight = m_glImageListManipulatorKnob.GetHeight();

   //////////////////////////////// 2D
   Vector vPos( 0.0f, 0.0f, 0.0f );
   vPos *= matrix;
   pView->WorldToScreen( vPos );
   m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 1, 1.0f );

   vPos.Set( 1.0f, 0.0f, 0.0f );
   vPos *= matrix;
   pView->WorldToScreen( vPos );
   m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );

   vPos.Set( 1.0f, 1.0f, 0.0f );
   vPos *= matrix;
   pView->WorldToScreen( vPos );
   m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );

   vPos.Set( 0.0f, 1.0f, 0.0f );
   vPos *= matrix;
   pView->WorldToScreen( vPos );
   m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );

   //////////////////////////////// 1D
   if (nAxis & AXIS_Y)
   {
      vPos.Set( 0.5f, 0.0f, 0.0f );
      vPos *= matrix;
      pView->WorldToScreen( vPos );
      m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );

      vPos.Set( 0.5f, 1.0f, 0.0f );
      vPos *= matrix;
      pView->WorldToScreen( vPos );
      m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );
   }

   if (nAxis & AXIS_X)
   {
      vPos.Set( 0.0f, 0.5f, 0.0f );
      vPos *= matrix;
      pView->WorldToScreen( vPos );
      m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );

      vPos.Set( 1.0f, 0.5f, 0.0f );
      vPos *= matrix;
      pView->WorldToScreen( vPos );
      m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 0, 1.0f );
   }
}

////////////////////////////////////////
// RotateProperty
void RotateProperty::DrawManipulator( CGLBaseView *pView, const Matrix44 &matrix, UINT nAxis/*=AXIS_X|AXIS_Y|AXIS_Z*/ )
{
   glColor4f( 1.0f, 1.0f, 0.0f, 0.7f );
   glBegin( GL_LINES );
      Vector vPos( 0.5f, 1.0f, 0.0f );
      glVertex3fv( &vPos.x );
      vPos.Set( 0.5f, 1.5f, 0.0f );
      glVertex3fv( &vPos.x );
   glEnd();

   GLDrawPrepper prepper( pView ); // push 2d projection

   if (m_glImageListManipulatorKnob.IsEmpty())
      m_glImageListManipulatorKnob.Create( IDB_SLIDERKNOB, 14 );
   //m_glImageListManipulatorKnob.SetTint( RGBFloat(1,1,0) );

   int nWidth = m_glImageListManipulatorKnob.GetWidthEach();
   int nHeight = m_glImageListManipulatorKnob.GetHeight();

   //////////////////////////////// 2D
   vPos.Set( 0.5f, 1.5f, 0.0f );
   vPos *= matrix;
   pView->WorldToScreen( vPos );
   m_glImageListManipulatorKnob.Draw( vPos.x-(nWidth/2), vPos.y-(nHeight/2), 1, 1.0f );
}