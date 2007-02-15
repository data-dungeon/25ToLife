#pragma once

#include "FileStream.h"
#include "MemoryStream.h"
#include "IntHash.h"
#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 
#include "HPatchModel.h"
#include "..\Utility\AString.h"

class Model;
class HashPatch;

class MdlTranslator : public HModelTranslatorPluginHelper
{
protected:
   static Model *m_pModel;

public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( MdlTranslator, HModelTranslatorPluginHelper )

   MdlTranslator() {}

// Operations
   static Model *GetModel() { return m_pModel; }

   void SaveImage( BaseStream &fs, HImage *pImage );

// HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "Hash Model"; }
   virtual const char *GetFilterExtension() { return "mdl"; }
   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_PATCHMODEL; }

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HPolyModel *pPolyModel, const char *szFileName);
   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HPolyModel *pPolyModel, const char *szFileName);

// HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Hash Mdl"; }
};

class HashObject
{
public:
   static float m_fProductVersion;
   enum ParseValue { PARSE_OK, PARSE_FAIL, PARSE_END, PARSE_ABORTLOAD, PARSE_READTOEND };

   HSnipeObject *m_pSnipeObject;
   CList<HashObject *> m_childlist;

   HashObject()
   {
      m_pSnipeObject = NULL;
   }
   virtual ~HashObject();

// Operations
   HSnipeObject *GetHSnipeObject() { return m_pSnipeObject; }
   HHierObject *GetHHierObject() { return (HHierObject *)m_pSnipeObject; }
   void SetHSnipeObject( HSnipeObject *pSnipeObject ) { m_pSnipeObject = pSnipeObject; }
   BOOL Load( BaseStream &hs );

// Overridables
   virt_base virtual HashObject *CreateObject( const String &strHeader, BaseStream &bs );
   virt_base virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
   {
      if (strKey == "USERCONTAINER" || strKey == "ENDUSERCONTAINER")
         return PARSE_OK;
      else
         return PARSE_FAIL;
   }
   virt_base virtual void InsertChildAtTail( HashObject *pChild ) { m_childlist.AddTail( pChild ); }
   virt_base virtual BOOL Save( BaseStream &hs ) { return TRUE; }
};

//class CP
//{
//public:
//   int m_nID;
//   union {
//      ULONG    m_cpflags;
//      struct {
//         ULONG m_issmooth           : 1,
//               m_isloop             : 1,
//               m_ishook             : 1,
//               m_isendhook          : 1,
//               m_ishookbase         : 1,
//               m_ishookcomputed     : 1;
//      };
//   };
//
//   CP( Model *pModel );
//};

class HBone;

class HashBone : public HashObject
{
public:
   HPatchModel *m_pModel;

   HashBone();
   HashBone( HashObject *pParent );

// Operations
   HBone *GetHBone() { return (HBone *)GetHSnipeObject(); }
   void ReadNonSkinnedCPs( BaseStream &bs );

// HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class HUVPoly;
class HMapSurfaceDriver;
class HTexCoords;
struct AMCPFlags;
struct AMPatchFlags;
class CP;

class Model : public HashBone
{
public:
   CMap<UINT, UINT, CP *, CP *> m_cpmap;
   IntHash     m_vmap;//,m_vnmap,m_uvmap;
   IntHash     m_patchrotmap;
   CList<CP *> m_hooks;
   CList<CP *> m_splinelist;
   CMap<HPoly *, HPoly *, HashPatch *, HashPatch *> m_patchmap;
   UINT        m_nGroupColorSurfaceIndex;

    Model( HSnipeObject *pSnipeObject );
   ~Model();

   HPatchModel *GetHPatchModel() { return (HPatchModel *)GetHSnipeObject(); }

   HashPatch *AddPatch();
   void SavePatches( BaseStream &hs );
   void SavePatch( BaseStream &bs, HPatch *pPatch );
   void SaveMesh( BaseStream &hs );
   void SaveSpline( BaseStream &bs, CP *pSplineCPHead );
   void SaveCP( BaseStream &bs, CP *pCP );
   void SaveDecals( BaseStream &hs );
   void SaveDecal( BaseStream &hs, HSurface *pSurface );
   void SaveDecalImage( BaseStream &hs, HSurface *pSurface, HMapSurfaceDriver *pSurfaceDriver );
   void SaveStamp( BaseStream &hs, HTexCoords *pTexCoords, HSurface *pSurface );
   void SaveGroups( BaseStream &hs );
   void SaveGroup( BaseStream &bs, HSurface *pSelection );
   void SaveGroup( BaseStream &bs, HSelection *pSelection );
   void SaveBones( BaseStream &hs );
   void SaveChildBones( BaseStream &bs, HHierObject *pParent );
   void SaveBone( BaseStream &bs, HBone *pBone );
   void SavePlaceholders( BaseStream &bs );

   BOOL LoadMesh( BaseStream &hs );
   BOOL ReadSpline( BaseStream &hs );
   CP *ReadCP( BaseStream &hs, CP *pPrevCP );
   BOOL LoadPatches( BaseStream &hs );
   BOOL ReadPatch( BaseStream &hs, BOOL bFivePoint );
   void SetCPFlagsFromHashFlags( CP *pBezierCP, AMCPFlags flags );
   void SetPolyFlagsFromHashFlags( HashPatch *pPatch, AMPatchFlags flags );
   void FixUVPolyForHookPatch( HashPatch *pPatch, HUVPoly *pUVPoly );
   HashPatch *FindPatchWith4MatchingVerts( CArray<CP *> &vertices, int &nOffset );
   void FindPatchesWithVertices( CList<CP *> &cplist, CArray<HashPatch *> &polys );
   void SetConstrainedFlagsForHooks();
   void SetConstrainedFlagsForHook( CP *pHook );
   void BuildEdges();
   void PeakEdges();
   CP *GetCPOnEdge( HEdge *pEdge, CP *pCP );

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
   virtual BOOL Save( BaseStream &hs );
};

class Still : public HashObject
{
public:
   static CMap< String, const String &, HImage *, HImage * > m_imagemap;

   // Operations
   HImage *GetHImage() { return (HImage *)GetHSnipeObject(); }

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class Decal;
class HUVMapSurfaceDriver;
class DecalImage : public HashObject
{
public:
   Decal *m_pDecal;
   HSurface *m_pSurface;

   DecalImage() { m_pDecal = NULL; m_pSurface = NULL; }
   DecalImage( HashObject *pParent );

   // Operations
   HUVMapSurfaceDriver *GetHUVMapSurfaceDriver() { return (HUVMapSurfaceDriver *)GetHSnipeObject(); }

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class Decal : public HashObject
{
public:
   CArray<DecalImage *> m_decalimages;
   BOOL m_bNamed;

   Decal( HashObject *pParent );

   // Operations
   HSurface *GetHSurface() { return (HSurface *)GetHSnipeObject(); }

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class Stamp : public HashObject
{
public:
   HSurface *m_pSurface;

   Stamp( HashObject *pParent );

   // Operations
   HTexCoords *GetHTexCoords() { return (HTexCoords *)GetHSnipeObject(); }
   BOOL ReadData( BaseStream &hs );

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class HClampMap;
class GroupImage : public DecalImage
{
public:
   GroupImage( HashObject *pParent );

   // Operations
   HClampMap *GetHClampMap() { return (HClampMap *)GetHSnipeObject(); }
};

class Group : public HashObject
{
public:
   GroupImage       *m_pGroupImage;
   CList<CP *>       m_cplist;
   String            m_strName;

   Group( HashObject *pParent );
   ~Group();

   // Operations
   HSurface *CreateHSurface();
   HSurface *GetHSurface() { return (HSurface *)GetHSnipeObject(); }
   BOOL ReadCPs( BaseStream &hs, int nCount );

   // HashObject overrides
   virtual HashObject *CreateObject( const String &strHeader, BaseStream &bs );
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class UnknownHashObject : public HashObject
{
public:
   UnknownHashObject( HashObject *pParent, const String &strHeader, BaseStream &bs );

   // HashObject overrides
   virtual ParseValue ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey );
};

class Bias
{
public:
   float m_alpha, m_gamma, m_magnitude;
   Bias(){}
   Bias(float alpha, float gamma, float magnitude)
   {
      m_alpha = alpha;
      m_gamma = gamma;
      m_magnitude = magnitude;
   }
   int operator == ( const Bias &a ) const { return (m_alpha == a.m_alpha) && (m_gamma == a.m_gamma) && (m_magnitude == a.m_magnitude); }
   int operator != ( const Bias &a ) const { return !(*this == a); }   
   Bias &operator = (const Bias &other) {
      m_alpha = other.m_alpha;
      m_gamma = other.m_gamma;
      m_magnitude = other.m_magnitude;
      return *this;
   }
   BOOL IsIdentity()
   {
      return m_alpha==0.0f && m_gamma==0.0f && m_magnitude==1.0f;
   }
};

AFX_INLINE UINT AFXAPI HashKey(Bias &key)
{
   double x = (int)(key.m_alpha * 1000.0f + SIGN(key.m_alpha) * .5f)/1000.0*12.3;
   double y = (int)(key.m_gamma * 1000.0f + SIGN(key.m_gamma) * .5f)/1000.0*23.4;
   double z = (int)(key.m_magnitude * 1000.0f + SIGN(key.m_magnitude) * .5f)/1000.0*34.5;
   return (UINT) (pow(x*x + y*y + z*z, 0.5 ) * 9.87);  
}

//class HookCP : public CP {
//public:
//   float  m_s;
//
//   HookCP( Model *model );
//   float GetS() { return m_s = (m_s <= .375f ? .25f : (m_s <= .625f ? .5f : .75f)); }
//   void SetS( float s ) { m_s = s; } 
//};

typedef struct AMCPFlags {
#if 1
   union {
      ULONG          m_flags;
      struct {
         ULONG       m_issmooth                : 1,   // 1 
                     m_isflip                  : 1,   // 2 
                     m_isloop                  : 1,   // 3 
                     UNUSED1                   : 1,   // 4 
                     m_ishook                  : 1,   // 5 
                     m_ishide                  : 1,   // 6 not saved
                     m_tempbit2                : 1,   // 7 
                     m_isprevskin              : 1,   // 8 
                     m_isclonegroup            : 1,   // 9 
                     m_islocked                : 1,   // 10
                     m_ishookcomputed          : 1,   // 11 not saved
                     m_ismodelhookcomputed     : 1,   // 12 not saved
                     m_isendhook               : 1,   // 13
                     UNUSED2                   : 1,   // 14
                     m_iscachedtangents        : 1,   // 15 not saved
                     m_ishookbase              : 1,   // 16
                     m_isassociatedgrouped     : 1,   // 17
                     m_isnextskin              : 1,   // 18
                     m_isbiasednormal          : 1,   // 19
                     UNUSED3                   : 5,   // 24
                     m_screentangentcounter    : 8;   // 32
      };
   };
#else//old hash flags
   union {
      ULONG          m_flags;
      struct {
       ULONG         m_issmooth                : 1,   // 1 
							m_isflip                  : 1,   // 2 
							m_isloop                  : 1,   // 3 
							m_isdependent             : 1,   // 4 
							m_ishook                  : 1,   // 5 
							m_ishide                  : 1,   // 6 
							m_isgroup                 : 1,   // 7 
							m_isprevskin              : 1,   // 8 
							m_isclonegroup            : 1,   // 9 
							m_islocked                : 1,   // 10
							m_isdmconvert             : 1,   // 11
							m_isdmuscle               : 1,   // 12
							m_isendhook               : 1,   // 13
							m_isneedsdrawn            : 1,   // 14
							m_isdit                   : 1,   // 15
							m_isdot                   : 1,   // 16
							m_isassociatedgrouped     : 1,   // 17
							m_isnextskin              : 1,   // 18
							m_ismuscleconvert         : 1,   // 19
							m_isdscreenit             : 1,   // 20
							m_isdscreenot             : 1,   // 21
							m_isdscreenp              : 1,   // 22
							m_ismusclecp              : 1,   // 23
							UNUSED1                   : 1,   // 24
							UNUSED2                   : 1,   // 25
							UNUSED3                   : 1,   // 26
							UNUSED4                   : 1,   // 27
							m_isdmusclebias			  : 1,   // 28
							m_isdmp                   : 1,   // 29
							m_isdmit                  : 1,   // 30
							m_isdmot                  : 1,   // 31
							m_ishookbase              : 1;   // 32
      };
   };
#endif
} AMCPFlags;

typedef struct AMPatchFlags {
   union {
      ULONG          m_flags;
      struct {
         ULONG       m_is5point           : 1,
                     m_ischeckednormal    : 1,
                     m_isflipnormal       : 1,
                     m_isflipp1           : 1,
                     m_isflipp2           : 1,
                     m_isflipp3           : 1,
                     m_isflipp4           : 1,
                     m_isdisplace         : 1,
                     m_groupimagerotation : 2,
                     m_isflipp5           : 1,
                     m_isfractal          : 1,
                     m_ishook1            : 1,
                     m_ishook2            : 1,
                     m_ishook3            : 1,
                     m_ishook4            : 1,
                     m_isusehook1         : 1,
                     m_isusehook2         : 1,
                     m_isusehook3         : 1,
                     m_isusehook4         : 1,
                     m_isendhook1         : 1,
                     m_isendhook2         : 1,
                     m_isendhook3         : 1,
                     m_isendhook4         : 1,
                     m_ishookdata1        : 1,
                     m_ishookdata2        : 1,
                     m_ishookdata3        : 1,
                     m_ishookdata4        : 1,
                     m_isdependent        : 1,
                     m_isshaded           : 1,
                     m_isbound            : 1,
                     m_isvector           : 1; // 32
      };
   };
} AMPatchFlags;
