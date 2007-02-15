#pragma once
#include "ParentableObject.h"
#include "../Interface/MainInterface.h"
#include "../Utility/Matrix44.h"

class CGLControl;

class Property;
class PropertyHeader : public SnipeObjectHeaderBase
{
public:
   String m_strValue;

    PropertyHeader() {}
    PropertyHeader( Property *pProperty, BOOL bWriteHeader, BaseStream &bs );
   ~PropertyHeader();

   static BOOL IsHeader( MemoryStream &ms );

   Property *GetProperty() { return (Property *)m_pObject; }
};

BaseStream &operator >> (BaseStream &bs, PropertyHeader &value);

enum PropertyInfoFlags {
   PIF_DEFAULT      = 0,
   PIF_HIDDEN       = 1 << 0,
   PIF_READONLY     = 1 << 1,
   PIF_NOUNDO       = 1 << 2,
   PIF_SHOWVALUEWHENNOTSET = 1 << 3,
};

class PropertyDriver;

class Property : public PropertyObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE( Property, PropertyObject )
   class Info 
   {
   public:
      String m_strName;
      String m_strMatchName;
      union {
         ULONG m_nPropertyInfoFlags;
         struct {
            ULONG m_bReadOnly : 1,
                  m_bVisible  : 1,
                  m_bUndoable : 1,
                  m_bShowValueWhenNotSet : 1;
         };
      };

      Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags=PIF_DEFAULT );
   };

   static GLImageList m_glImageListManipulatorKnob;

   Property       *m_pCache;
   Info           *m_pPropertyInfo;
   SnipeObject    *m_pParent;
   PropertyDriver *m_pPropertyDriverHead;
   union {
      ULONG        m_nPropertyFlags;
      struct {    
         ULONG     m_bSet                  : 1,
                   m_bIsPreDestructed      : 1,
                   m_bIsPostConstructed    : 1,
                   m_bCompositedValueDirty : 1;
      };
   };

            Property();
   virtual ~Property();

// Operations
   void Init( Info *pInfo, PropertyObject *pParent );
   void OnValueChanged( BOOL bStoreUndo=FALSE );
   void OnValueStored( const Time &time, BOOL bStoreUndo );
   void SendNotificationToDependents( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   SnipeObject *GetObject();
   Property *GetCache() { return m_pCache ? m_pCache : this; } // Note: returns this if it is the cache
   void SetParent( SnipeObject *pParent ) { m_pParent = pParent; }

   BOOL IsReadOnly() const { return m_pPropertyInfo->m_bReadOnly; };
   BOOL IsUndoable() const { return m_pPropertyInfo->m_bUndoable; };
   BOOL IsShowValueWhenNotSet() const { return m_pPropertyInfo->m_bShowValueWhenNotSet; };
   BOOL IsSet() const { return m_bSet; }
   void SetSet( BOOL bSet ) { m_bSet = bSet; }

   PropertyDriver *GetPropertyDriverHead() const { return m_pPropertyDriverHead; }
   UINT GetNumPropertyDrivers( SnipeObjectType nSOT=SOT_PropertyDriver ) const;
   PropertyDriver *GetPropertyDriverAt( UINT nIndex, SnipeObjectType nSOT=SOT_PropertyDriver ) const;
   PropertyDriver *CreateStorageDriver();
   void AddPropertyDriver( PropertyDriver *pPropertyDriver );
   BOOL RemovePropertyDriver( PropertyDriver *pPropertyDriver );

   BOOL IsCompositedValueDirty( Time time ) const;
   void CompositeDrivers( Time time );

// Overridables
   virt_base virtual SnipeObjectInfo *GetStorageDriverInfo() { return NULL; }
   virt_base virtual void CreatePropertyControls( CGLControl *pParent );

   virt_base virtual void GetValueIndexString( String &strValue, int nComponentIndex=0 );
   virt_base virtual catch_msg void StoreValueIndexFromString( const String &strValue, int nComponentIndex=0, const Time &time=TAGGEDTIME );

   virt_base virtual void GetValueString( String &strOutValue, const Time &time=GetTime() );
   virt_base virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE ) {}

   virt_base virtual void Validate() {}
   virt_base virtual BOOL IsSave();
   virt_base virtual BOOL WriteValue( BaseStream &bs ) { return FALSE; } // return value is whether anything got written out
   virt_base virtual void ReadValue( BaseStream &bs ) {}
   virt_base virtual BOOL IsDefaultValue( BOOL bIncludeChildren=FALSE );
   virt_base virtual void CopyCacheValue() {}

// InstanceObject overrides
   virtual String GetTitle() const;
   virtual CGLObjectTreeItem *AllocTreeItem( CGLTreeControl *pTreeControl );
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual int GetItemHeight() { return 20; }

// SnipeObject overrides
   virtual SnipeObject *GetParent() const { return m_pParent; }
   virtual BOOL IsCache() const { return m_pCache == NULL; }
   virtual void PostConstruct();
   virtual void PreDestruct();
   virtual BOOL IsProperty() const { return TRUE; }
   virtual String GetMatchName() const;
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual void SetVisible( BOOL bVisible ) { SnipeObject::SetVisible(bVisible); SetVisibleInTree(bVisible); }
};

template< typename TYPE >
class BasicProperty : public Property
{
public:
   class Info : public Property::Info
   {
   public:
      TYPE m_defaultvalue;
      TYPE m_minvalue;
      TYPE m_maxvalue;

      Info( const String &strName, const String &strMatchName, TYPE defaultvalue, TYPE minvalue, TYPE maxvalue, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : Property::Info( strName, strMatchName, nPropertyInfoFlags )
      {
         m_defaultvalue = defaultvalue;
         m_minvalue = minvalue;
         m_maxvalue = maxvalue;
      }

      const TYPE &GetDefaultValue() const { return m_defaultvalue; }
      void SetDefaultValue( const TYPE &value ) { m_defaultvalue = value; }

      const TYPE &GetMinValue() const { return m_minvalue; }
      void SetMinValue( const TYPE &value ) { m_minvalue = value; }

      const TYPE &GetMaxValue() const { return m_maxvalue; }
      void SetMaxValue( const TYPE &value ) { m_maxvalue = value; }
   };

private:
   static TYPE m_prevvalue;
   TYPE m_value;

public:
   void Init( Info *pInfo, PropertyObject *pParent )
   {
      Property::Init( pInfo, pParent );
      m_prevvalue = 0;
      m_value = pInfo->m_defaultvalue; // don't call SetValue because they might try to look at the previous value and it has crap in it.
      OnObjectNotify( NOTIFYOBJ_VALUECHANGED, 2, (LPARAM)this );
      SendNotificationToDependents( NOTIFYOBJ_VALUECHANGED, 2, (LPARAM)this );
   }
   Info *GetPropertyInfo() const
   {
      return (Info *)m_pPropertyInfo;
   }
   BasicProperty<TYPE> *GetCache() // Note: returns this if it is the cache
   {
      return (BasicProperty<TYPE> *)Property::GetCache();
   }
   operator TYPE()
   {
      return GetValue();
   }
   void StoreValue( const TYPE &value, const Time &time=GetTime(), BOOL bStoreUndo = TRUE)
   {
      if (IsSet() && m_value==value)
         return;
      SetValue( value );
      OnValueStored( time, bStoreUndo );
   }
   const TYPE &GetCurrentValue() const
   {
      return m_value;
   }
   const TYPE &GetValue( Time time=GetTime() )
   {
      if (IsCompositedValueDirty( time ))
         CompositeDrivers( time );
      return m_value;
   }
   const TYPE &GetPrevValue() const // only valid during NOTIFYOBJ_VALUECHANGED
   {
      return m_prevvalue;
   }
   void CompositeDrivers( Time time )
   {
      m_prevvalue = m_value;

      Property::CompositeDrivers( time );

      if (m_value != m_prevvalue)
         OnValueChanged();
   }

// Overridables
   virt_base virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE ) {}
   virt_base virtual void SetValue( const TYPE &value ) { m_prevvalue = m_value; m_value = value; }
   virt_base virtual TYPE GetFactoredValue() { return GetValue(); }
   virt_base virtual TYPE GetFactoredValue( Time time ) { return GetValue( time ); }
   virt_base virtual float GetFactor() const { return 1.0f; }

// Property overrides
   virtual void GetValueString( String &strOutValue, const Time &time=GetTime() )
   {
      if (IsSet() || IsShowValueWhenNotSet())
      {
         MemoryStream ms;
         ms.Open(BaseStream::BS_OUTPUT);
         GetValue( time );
         WriteValue(ms);
         ms << '\0';
         strOutValue = (char *)ms.GetBuffer();
      }
      else
      {
         static String strNotSet = String::GetResourceString( 214/*IDS_NOTSET*/ );
         strOutValue = strNotSet;
      }
   }
   virtual BOOL WriteValue( BaseStream &bs ) 
   {
      bs << m_value;
      return TRUE;
   }
   virtual void ReadValue( BaseStream &bs ) 
   {
      TYPE value;
      bs >> value;
      SetValue( value );
   }

   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE)
   {
      static String strNotSet = String::GetResourceString( 214/*IDS_NOTSET*/ );

      if (strValue == strNotSet)
      {
         TYPE newvalue = GetPropertyInfo()->GetDefaultValue();
         StoreValue( newvalue, time, bStoreUndo );
         SetSet( FALSE );
      }
      else
      {
         MemoryStream ms;
         ms.Open(strValue.Get(), strValue.Length(), BaseStream::BS_ASCII|BaseStream::BS_OUTPUT);

         TYPE origvalue = GetValue();
         ReadValue( ms );
         TYPE newvalue = GetValue();
         SetValue(origvalue); // Put back like we found it
         StoreValue( newvalue, time, bStoreUndo );
      }
   }

   virtual BOOL IsDefaultValue( BOOL bIncludeChildren=FALSE )
   {
      if (m_value != GetPropertyInfo()->m_defaultvalue)
         return FALSE;
      return Property::IsDefaultValue( bIncludeChildren );
   }

   virtual void CopyCacheValue()
   {
      ASSERT(IsInstance());

      m_prevvalue = m_value;
      SetValue( GetCache()->GetValue());
      if (m_value != m_prevvalue)
         OnValueChanged();
   }
};

class BoolProperty : public BasicProperty<BOOL>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(BoolProperty, BasicProperty<BOOL>)

   class Info : public BasicProperty<BOOL>::Info
   {
   public:
      union {
         ULONG    m_nBoolPropertyFlags;
         struct {
         ULONG    m_bAutoHideChildren  : 1;
         };
      };

      Info( const String &strName, const String &strMatchName, BOOL defaultvalue, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : BasicProperty<BOOL>::Info( strName, strMatchName, defaultvalue, FALSE, TRUE, nPropertyInfoFlags )
      {
         m_nBoolPropertyFlags = 0;
         m_bAutoHideChildren = TRUE;
      }

      void SetAutoHideChildren( BOOL bAutoHideChildren ) { m_bAutoHideChildren = bAutoHideChildren; }
   };

   Info *GetPropertyInfo() const { return (Info *)BasicProperty<BOOL>::GetPropertyInfo(); }

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual BOOL WriteValue( BaseStream &bs );
   virtual void ReadValue( BaseStream &bs );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class IntProperty : public BasicProperty<int>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(IntProperty, BasicProperty<int>)

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual void Validate()
   {
      if (GetValue() > GetPropertyInfo()->m_maxvalue)
         SetValue( GetPropertyInfo()->m_maxvalue );
      if (GetValue() < GetPropertyInfo()->m_minvalue)
         SetValue( GetPropertyInfo()->m_minvalue );
   }

// BasicProperty overrides
   virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE );
};

class TypeProperty : public IntProperty
{
public:
   DECLARE_SNIPEOBJECT_CREATE(TypeProperty, IntProperty)

   class Info : public IntProperty::Info
   {
   public:
      BOOL m_bDisplayAsRadios;

      class Node 
      {
      private:
         String m_strName;
         String m_strMatchName;

      public:
         Node() {} // For Plugins

         Node( const String &strName, const String &strMatchName )
         {
            m_strName = strName;
            m_strMatchName = strMatchName;
         }

         const String &GetName() const { return m_strName; }
         void SetName( const String &strName ) { m_strName = strName; }

         const String &GetMatchName() const { return m_strMatchName; }
         void SetMatchName( const String &strMatchName ) { m_strMatchName = strMatchName; }
      };

   private:
      Node *m_NodeArray;
      int   m_nNodeCount;

   public:
      Info( const String &strName, const String &strMatchName, Node nodearray[], int nNodeCount, int nDefaultValue, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : IntProperty::Info( strName, strMatchName, nDefaultValue, 0, nNodeCount-1, nPropertyInfoFlags )
      {
         m_NodeArray = nodearray;
         m_nNodeCount = nNodeCount;
         m_bDisplayAsRadios = FALSE;
      }

      Node *GetNodeArray() { return m_NodeArray; }
      int GetNodeCount() { return m_nNodeCount; }
   };

   Info *GetPropertyInfo() const { return (Info *)IntProperty::GetPropertyInfo(); }

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual BOOL WriteValue( BaseStream &bs );
   virtual void ReadValue( BaseStream &bs );
};

template< typename TYPE >
class FactoredProperty : public BasicProperty<TYPE>
{
public:
   class Info : public BasicProperty<TYPE>::Info
   {
   public:
      float m_fFactor;
      int   m_nType; enum { FPT_DEFAULT, FPT_UNITS, FPT_PERCENT, FPT_ANGLE };

      Info( const String &strName, const String &strMatchName, TYPE defaultvalue, TYPE minvalue, TYPE maxvalue, float fFactor=1.0f, int nType=FPT_DEFAULT, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : BasicProperty<TYPE>::Info( strName, strMatchName, defaultvalue, minvalue, maxvalue, nPropertyInfoFlags )
      {
         m_fFactor = fFactor;
         m_nType = nType;
      }
   };

   Info *GetPropertyInfo() const { return (Info *)m_pPropertyInfo; }
   int   GetType() const { return GetPropertyInfo()->m_nType; }
   int   GetMeasurementUnits() const
   {
      switch (GetType())
      {
      case Info::FPT_UNITS:
         return ::GetMeasurementUnits();
      case Info::FPT_PERCENT:
         return UNIT_PERCENTAGE;
      case Info::FPT_ANGLE:
         return UNIT_ANGLE;
      }

      return UNIT_NONE;
   }

// BasicProperty<TYPE> overrides
   virtual TYPE GetFactoredValue() { return GetValue() * GetFactor(); }
   virtual TYPE GetFactoredValue( Time time ) { return GetValue( time ) * GetFactor(); }
   virtual float GetFactor() const { return GetPropertyInfo()->m_fFactor; }
};

class TimeProperty : public BasicProperty<Time>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(TimeProperty, BasicProperty<Time>)

// BasicProperty<Time> overrides
   virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE );

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual void Validate();
   virtual void GetValueString( String &strOutValue, const Time &time=GetTime() );
   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
};

class FloatProperty : public FactoredProperty<float>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(FloatProperty, FactoredProperty<float>)

// BasicProperty<float> overrides
   virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE );

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual SnipeObjectInfo *GetStorageDriverInfo();
   virtual void Validate();
   virtual void GetValueString( String &strOutValue, const Time &time=GetTime() );
   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
};

class Vector2Property : public FactoredProperty<Vector2>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Vector2Property, FactoredProperty<Vector2>)

// BasicProperty<Vector2> overrides
   virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE );

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual SnipeObjectInfo *GetStorageDriverInfo();
   virtual void Validate() {}
   virtual void GetValueString( String &strOutValue, const Time &time=GetTime() );
   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
   virtual void GetValueIndexString( String &strValue, int nComponentIndex=0 );
   virtual catch_msg void StoreValueIndexFromString( const String &strValue, int nComponentIndex=0, const Time &time=TAGGEDTIME );
};

class VectorProperty : public FactoredProperty<Vector>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(VectorProperty, FactoredProperty<Vector>)

// BasicProperty overrides
   virtual void OnScrub( int nPixels, int nComponentIndex=0, BOOL bStoreUndo = FALSE );

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual SnipeObjectInfo *GetStorageDriverInfo();
   virtual void Validate() {}
   virtual void GetValueString( String &strOutValue, const Time &time=GetTime() );
   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );
   virtual void GetValueIndexString( String &strValue, int nComponentIndex=0 );
   virtual catch_msg void StoreValueIndexFromString( const String &strValue, int nComponentIndex=0, const Time &time=TAGGEDTIME );
};

class ColorProperty : public BasicProperty<RGBFloat>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ColorProperty, BasicProperty<RGBFloat>)

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
};

class TranslateProperty : public VectorProperty
{
public:
   DECLARE_SNIPEOBJECT_CREATE(TranslateProperty, VectorProperty)

   class Info : public VectorProperty::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, Vector defaultvalue=0.0f, Vector minvalue=-MAX_FLOAT, Vector maxvalue=MAX_FLOAT, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : VectorProperty::Info( strName, strMatchName, defaultvalue, minvalue, maxvalue, 1.0f, VectorProperty::Info::FPT_UNITS, nPropertyInfoFlags )
      {
      }
   };
};

class ScaleProperty : public VectorProperty
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ScaleProperty, VectorProperty)

   class Info : public VectorProperty::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, Vector defaultvalue=1.0f, Vector minvalue=-MAX_FLOAT, Vector maxvalue=MAX_FLOAT, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : VectorProperty::Info( strName, strMatchName, defaultvalue, minvalue, maxvalue, 100.0f, VectorProperty::Info::FPT_PERCENT, nPropertyInfoFlags )
      {
      }
   };

   enum {
      AXIS_X = 1<<0,
      AXIS_Y = 1<<1,
      AXIS_Z = 1<<2
   };

   void DrawManipulator( CGLBaseView *pView, const Matrix44 &matrix, UINT nAxis=AXIS_X|AXIS_Y|AXIS_Z );
};

class RotateProperty : public VectorProperty
{
public:
   DECLARE_SNIPEOBJECT_CREATE(RotateProperty, VectorProperty)

   class Info : public VectorProperty::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, Vector defaultvalue=0.0f, Vector minvalue=-MAX_FLOAT, Vector maxvalue=MAX_FLOAT, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : VectorProperty::Info( strName, strMatchName, defaultvalue, minvalue, maxvalue, 1.0f, VectorProperty::Info::FPT_ANGLE, nPropertyInfoFlags )
      {
      }
   };

   void SetValue( const Quaternion &quat )
   {
      RotateEuler euler( quat );
      VectorProperty::SetValue( Vector(euler.x, euler.y, euler.z) );
   }
   void StoreValue( const Vector &value, const Time &time=GetTime(), BOOL bStoreUndo=TRUE ) // compiler can't find the one below without this proto
   {
      VectorProperty::StoreValue( value, time, bStoreUndo );
   }
   void StoreValue( const Quaternion &quat, const Time &time=GetTime(), BOOL bStoreUndo=TRUE )
   {
      RotateEuler euler( quat );
      VectorProperty::StoreValue( Vector(euler.x, euler.y, euler.z), time, bStoreUndo );
   }

   enum {
      AXIS_X = 1<<0,
      AXIS_Y = 1<<1,
      AXIS_Z = 1<<2
   };
   void DrawManipulator( CGLBaseView *pView, const Matrix44 &matrix, UINT nAxis=AXIS_X|AXIS_Y|AXIS_Z );
};

class TransformProperty : public Property
{
public:
   DECLARE_SNIPEOBJECT_CREATE(TransformProperty, Property)

   class Info : public Property::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : Property::Info( strName, strMatchName, nPropertyInfoFlags )
      {
      }
   };

   static TranslateProperty::Info m_vTranslateInfo;
   static ScaleProperty::Info     m_vScaleInfo;
   static RotateProperty::Info    m_vRotateInfo;

   TranslateProperty m_vTranslate;
   ScaleProperty     m_vScale;
   RotateProperty    m_vRotate;

   Matrix44 m_matrix;
   Matrix44 m_inversematrix;
   Time     m_tPrevTime;
   BOOL     m_bMatrixDirty;

   TransformProperty();

   void ComputeMatrices( const Time &time );
   const Matrix44 &GetMatrix( const Time &time=GetTime() );
   const Matrix44 &GetInverseMatrix( const Time &time=GetTime() );

   // SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class Transform2Property : public Property
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Transform2Property, Property)

   class Info : public Property::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : Property::Info( strName, strMatchName, nPropertyInfoFlags )
      {
      }
   };

   static Vector2Property::Info m_v2TranslateInfo;
   static Vector2Property::Info m_v2ScaleInfo;
   static FloatProperty::Info   m_fRotateInfo;

   Vector2Property m_v2Translate;
   Vector2Property m_v2Scale;
   FloatProperty   m_fRotate;

   Matrix44 m_matrix;
   Matrix44 m_inversematrix;
   BOOL m_bMatrixDirty;

   Transform2Property();

   void ComputeMatrices();
   const Matrix44 &GetMatrix();
   const Matrix44 &GetInverseMatrix();

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class StringProperty : public BasicProperty<String>
{
public:
   DECLARE_SNIPEOBJECT_CREATE(StringProperty, Property)

   class Info : public BasicProperty<String>::Info
   {
   public:
      BOOL m_bMultiLine;

      Info( const String &strName, const String &strMatchName, const String &strDefaultValue, BOOL bMultiLine=FALSE, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : BasicProperty<String>::Info( strName, strMatchName, strDefaultValue, "", "", nPropertyInfoFlags )
      {
         m_bMultiLine = bMultiLine;
      }

      BOOL IsMultiLine() const { return m_bMultiLine; }
      void SetMultiLine( BOOL bMultiLine ) { m_bMultiLine = bMultiLine; }
   };

   Info *GetPropertyInfo() const { return (Info *)BasicProperty<String>::GetPropertyInfo(); }
   BOOL IsMultiLine() { return GetPropertyInfo()->IsMultiLine(); }

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual void ReadValue( BaseStream &bs );
   virtual catch_msg void StoreValueFromString( const String &strValue, const Time &time=GetTime(), BOOL bStoreUndo=TRUE );

// SnipeObject overrides
   virtual int GetItemHeight();
};

enum FileNamePropertyInfoFlags {
   FILENAME_PIF_DEFAULT     = 0,
   FILENAME_PIF_OPENDIALOG  = 1 << 0,
};

class FileNameProperty : public StringProperty
{
public:
   DECLARE_SNIPEOBJECT_CREATE(FileNameProperty, StringProperty)

   class Info : public StringProperty::Info
   {
   public:
      String m_strRegistry;
      String m_strFilters;
      ULONG m_nFileNamePropertyInfoFlags;
      struct {
         ULONG m_bOpenDialog : 1;
      };

      Info( const String &strName, const String &strMatchName, const String &strDefaultFileName, const String &strFilters, const String &strRegistry, UINT nFileNamePropertyInfoFlags=FILENAME_PIF_DEFAULT, UINT nPropertyInfoFlags=PIF_DEFAULT );
   };

   FileNameProperty()
   {
      m_bSet = FALSE;
   }
   Info *GetPropertyInfo() const { return (Info *)StringProperty::GetPropertyInfo(); }

   String GetFilters() const { return GetPropertyInfo()->m_strFilters; }
   BOOL GetFileName( String &strOutFileName, BOOL bForceQuery=FALSE );// Will Prompt if NULL or bForceQuery

#ifdef _DEBUG
   void StoreValue( const String &value, const Time &time=GetTime(), BOOL bStoreUndo=TRUE)
   {
      ASSERT(!IsRelativePath( value ));
      StringProperty::StoreValue( value, time, bStoreUndo );
   }
#endif // _DEBUG

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual void GetValueIndexString( String &strValue, int nComponentIndex=0 );
   virtual void ReadValue( BaseStream &bs );
   virtual BOOL WriteValue( BaseStream &bs );
};

class FileInfoProperty : public Property
{
private:
   static BoolProperty::Info m_bEmbedInfo;
   
public:
   DECLARE_SNIPEOBJECT_CREATE(FileInfoProperty, Property)

   class Info : public Property::Info
   {
   public:
      FileNameProperty::Info m_strFileNameInfo;

      Info( const String &strName, const String &strMatchName, const String &strDefaultFileName, const String &strFilters, const String &strRegistry, UINT nPropertyInfoFlags=PIF_DEFAULT );
   };

   BoolProperty     m_bEmbed;
   FileNameProperty  m_strFileName;

   FileInfoProperty();

   void Init( Info *pInfo, PropertyObject *pParent );
   Info *GetPropertyInfo() const { return (Info *)m_pPropertyInfo; }
   FileNameProperty *GetFileNameProperty() { return &m_strFileName; }
   BOOL IsSet() { return m_strFileName.IsSet(); }
};

template< class TYPE >
class PointerProperty : public BasicProperty<TYPE>
{
public:
   class Info : public BasicProperty<TYPE>::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags=PIF_DEFAULT )
       : BasicProperty<TYPE>::Info( strName, strMatchName, NULL, NULL, NULL, nPropertyInfoFlags )
      {
      }
   };

   String m_strDescription; // During load, it may not be possible to find the object until the load is complete.  We store the description here until that point.

   virtual ~PointerProperty();

// Operations
   void ResolveDescription( BOOL bOutputError=FALSE );

// Property overrides
   virtual void CreatePropertyControls( CGLControl *pParent );
   virtual void SetValue( TYPE const &value );
   virtual BOOL WriteValue( BaseStream &bs );
   virtual void ReadValue( BaseStream &bs );
   virtual void Validate()
   {
      if (GetCurrentValue())
      {
         SnipeObjectList possibleobjects;
         if (GetPossiblePointerPropertyChoices( this, &possibleobjects ))
         {
            if (!possibleobjects.Find( GetCurrentValue() ))
               SetValue( NULL );
         }
      }
   }

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual catch_msg void OnEndProjectLoad();
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
   {
      return GetObject()->GetPossiblePointerPropertyChoices( pPointerProperty, pSnipeObjectList );
   }
};

class PointerPropertyNode 
{
public:
   PointerPropertyNode            *m_pNextNode;
   PointerProperty<SnipeObject *> *m_pPointerProperty;

   PointerPropertyNode( PointerProperty<SnipeObject *> *pPointerProperty, PointerPropertyNode *pNextNode )
   {
      m_pNextNode = pNextNode;
      m_pPointerProperty = pPointerProperty;
   }
};

template<class TYPE>
PointerProperty<TYPE>::~PointerProperty()
{
   if (GetCurrentValue())
      GetCurrentValue()->RemoveNotifyNode( this );
}

template<class TYPE>
void PointerProperty<TYPE>::SetValue( TYPE const &value )
{
   if (GetCurrentValue())
      GetCurrentValue()->RemoveNotifyNode( this );

   BasicProperty<TYPE>::SetValue( value );

   if (GetCurrentValue())
      GetCurrentValue()->AddNotifyNode( NOTIFYOBJ_DELETE | NOTIFYOBJ_REMOVEDFROMPROJECT | NOTIFYOBJ_ADDEDTOPROJECT, this );
}

template<class TYPE>
BOOL PointerProperty<TYPE>::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch( msg )
   {
      case NOTIFYOBJ_DELETE:
      {
         TYPE pSnipeObject = (TYPE)lParam;
         if (GetCurrentValue() == pSnipeObject)
            SetValue( NULL );
         else
            pSnipeObject->RemoveNotifyNode( this );
         break;
      }
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
      {
         TYPE pSnipeObject = (TYPE)lParam;
         ASSERT(GetCurrentValue() == pSnipeObject);
         BasicProperty<TYPE>::SetValue( NULL ); // don't remove the NotifyNode
         break;
      }
      case NOTIFYOBJ_ADDEDTOPROJECT:
      {
         TYPE pSnipeObject = (TYPE)lParam;
         ASSERT(GetCurrentValue() == NULL);
         BasicProperty<TYPE>::SetValue( pSnipeObject ); // don't add/remove the NotifyNode
         break;
      }
   }
   return BasicProperty<TYPE>::OnObjectNotify( msg, wParam, lParam );
}

template<class TYPE>
BOOL PointerProperty<TYPE>::WriteValue( BaseStream &bs )
{
   SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( GetObject());
   BOOL rValue = BasicProperty<TYPE>::WriteValue( bs );
   GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
   return rValue;
}

template<class TYPE>
void PointerProperty<TYPE>::ResolveDescription( BOOL bOutputError/*=FALSE*/ )
{
   if (!m_strDescription.IsEmpty())
   {
      SnipeObject *pOldWorkingObject = GetMainInterface()->SetCurrentWorkingObject( GetObject());
      String strError;
      SetValue( (TYPE)GetMainInterface()->DescriptionToObject( m_strDescription, &strError ));
      if (GetCurrentValue())
         m_strDescription.Empty();
      else if (bOutputError)
         g_outputWindow << strError;
      GetMainInterface()->SetCurrentWorkingObject( pOldWorkingObject );
   }
}

template<class TYPE>
void PointerProperty<TYPE>::ReadValue( BaseStream &bs )
{
   bs.ReadLine( m_strDescription );

   SetValue( NULL );
   ResolveDescription();
}

template<class TYPE>
void PointerProperty<TYPE>::OnEndProjectLoad()
{
   ResolveDescription( TRUE );

   return BasicProperty<TYPE>::OnEndProjectLoad();
}

///////////////////////////////////////////////////////////////

typedef String (* PFN_GETSTATISTIC)( void *pData );

class StatisticProperty : public Property
{
private:
   PFN_GETSTATISTIC m_pfnGetStatistic;
   void *m_pData;

public:
   void Init( Info *pInfo, PropertyObject *pParent, PFN_GETSTATISTIC pfnGetStatistic, void *pData )
   {
      Property::Init( pInfo, pParent );
      m_pfnGetStatistic = pfnGetStatistic;
      m_pData = pData;
      OnObjectNotify( NOTIFYOBJ_VALUECHANGED, 2, (LPARAM)this );
      SendNotificationToDependents( NOTIFYOBJ_VALUECHANGED, 2, (LPARAM)this );
   }

// Property overrides
   virtual BOOL WriteValue( BaseStream &bs ) 
   {
      bs << m_pfnGetStatistic( m_pData );
      return TRUE;
   }
   virtual void CreatePropertyControls( CGLControl *pParent );
};

///////////////////////////////////////////////////////////////

void DoStoreValueCommand( Property *pProperty, const String &strValue, const Time &time=TAGGEDTIME, BOOL bTemp=FALSE);
void DoStoreValueCommandCommitTemp( Property *pProperty );

#define CONVERTTOSTRING( value, string ) \
{ \
   MemoryStream ms; \
   ms.Open( BaseStream::BS_OUTPUT ); \
   ms << value; \
   ms << '\0'; \
   string = ms.GetBuffer(); \
}

#define CONVERTFACTOREDTOSTRING( value, string ) \
{ \
   MemoryStream ms; \
   ms.Open( BaseStream::BS_OUTPUT ); \
   ms << (LPCTSTR)UnitToString( GetCurrentValue() * GetFactor(), GetMeasurementUnits(), -1, 0.0001f ); \
   ms << '\0'; \
   string = ms.GetBuffer(); \
}
