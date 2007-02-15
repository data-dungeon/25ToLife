#include "Stdafx.h"
#include "HMainInterface.h"
#include "../HashPlugin.h"
#include "MDL.h"
#include "CP.h"
#include "HSnipeObjectList.h"
#include "HTransform2Property.h"
#include "HTranslateProperty.h"
#include "HScaleProperty.h"
#include "HRotateProperty.h"
#include "HVector2Property.h"
#include "HFloatProperty.h"
#include "HBoolProperty.h"
#include "HTypeProperty.h"
#include "HImagePointerProperty.h"
#include "HTexCoordsPointerProperty.h"
#include "HPropertyPointerProperty.h"
#include "AString.h"
#include "HBone.h"
#include "HPoly.h"
#include "HPatch.h"
#include "HProject.h"
#include "HDecal.h"
#include "HImage.h"
#include "HSurface.h"
#include "HSurfaceDriver.h"
#include "HUVMapSurfaceDriver.h"
#include "HClampMapSurfaceDriver.h"
#include "HTexCoords.h"
#include "VectorHash.h"
#include "HUVPoly.h"
#include "HUV.h"
#include "HIntProperty.h"
#include "HPatchEdge.h"
#include "HPatchVertex.h"
#include "HColorProperty.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Rotate.h"
#include "HPlaceholderObject.h"
#include "HSelection.h"
#include "HashPatch.h"

float HashObject::m_fProductVersion;
Model *MdlTranslator::m_pModel;

inline BOOL IsTConstrained( HPatchVertex *pVertex )
{
   return pVertex->IsConstrained() && pVertex->GetNumSurroundingEdges()==3;
}

void MdlTranslator::SaveImage( BaseStream &fs, HImage *pImage )
{
   fs << "[STILL]" << nl;

      fs << "FileName=" << pImage->GetFileName() << nl;

   fs << "[ENDSTILL]" << nl;
}

Model::Model( HSnipeObject *pSnipeObject )
{
   m_pSnipeObject = pSnipeObject;
   m_pModel = (HPatchModel *)pSnipeObject;
   m_nGroupColorSurfaceIndex = 0;
   m_cpmap.InitHashTable( 1019 );
   m_patchmap.InitHashTable( 1019 );
}

Model::~Model()
{
   for (POSITION pos=m_cpmap.GetStartPosition(); pos; )
   {
      CP *pCP;
      UINT nID;
      m_cpmap.GetNextAssoc( pos, nID, pCP );
      delete pCP;
   }

   for (POSITION pos=m_patchmap.GetStartPosition(); pos; )
   {
      HPoly *pPoly;
      HashPatch *pHashPatch;
      m_patchmap.GetNextAssoc( pos, pPoly, pHashPatch );
      delete pHashPatch;
   }
}

//////////////////////////////////////////////////////////

HashObject::~HashObject()
{
   for (POSITION pos=m_childlist.GetHeadPosition(); pos; )
   {
      HashObject *pChild = m_childlist.GetNext( pos );
      delete pChild;
   }
}

BOOL HashObject::Load( BaseStream &hs )
{
   static MemoryStream ms( 256 );
   static Header lastheader;
   static Header header;
   static Label label;

   int rvalue;
   HashObject *pNewObj;

   while (hs.ReadLine( ms )) {
      if (ms.IsLabel()) {
         ms >> label;
         rvalue = ParseArg( hs, ms, label.string );
         switch (rvalue) {
            case PARSE_END:
               return TRUE;
            case PARSE_READTOEND:
               hs.SkipHeader( lastheader );  // Skip unwanted Header
               return TRUE;
            case PARSE_ABORTLOAD:
               return FALSE;
         }
      }
      else if (ms.IsHeader()) {
         ms >> header;
         lastheader = header;
         rvalue = ParseArg( hs, ms, header.string );
         switch (rvalue) {
            case PARSE_END:
               return TRUE;
            case PARSE_FAIL:
               if (pNewObj = CreateObject( header.string, hs )) {
                  InsertChildAtTail( pNewObj );
                  if (!pNewObj->Load( hs ))
                     return FALSE;
               }
               else
                  hs.SkipHeader(header);  // Skip Unrecognized Header
               break;
            case PARSE_ABORTLOAD:
               return FALSE;
         }
      }
   }

   return TRUE;
}

HashObject *HashObject::CreateObject( const String &strHeader, BaseStream &bs )
{
   if (strHeader == "STILL")
      return SNEW Still;
   else if (strHeader == "DECAL")
      return SNEW Decal( this );
   else if (strHeader == "DECALIMAGE")
      return SNEW DecalImage( this );
   else if (strHeader == "STAMP")
      return SNEW Stamp( this );
   else if (strHeader == "GROUP")
      return SNEW Group( this );
   else if (strHeader == "SEGMENT")
      return SNEW HashBone( this );
   else if (strHeader == "RELATIONCONTAINER"
      || strHeader == "USERPROPERTY")
      return SNEW UnknownHashObject( this, strHeader, bs );
   else
      return NULL;
}

UnknownHashObject::UnknownHashObject( HashObject *pParent, const String &strHeader, BaseStream &bs )
{
   static MemoryStream ms( 256 );
   static Header header;
   static Label label;
   String strEndHeader = "END" + strHeader;
   int nDepth = 1;
   UINT nStartPos = bs.GetPos();
   UINT nLastLinePos = bs.GetPos();

   while (bs.ReadLine( ms ))
   {
      if (ms.IsHeader())
      {
         ms >> header;
         if (strEndHeader == header.string)
         {
            if (--nDepth == 0)
            {
               UINT nLinePos = bs.GetPos();
               bs.Seek( nLastLinePos, BaseStream::ST_BEGINNING );
               nLastLinePos = nLinePos;
               break;
            }
         }
         else if (strHeader == header.string)
            nDepth++;
      }
      nLastLinePos = bs.GetPos();
   }

   UINT nLength = nLastLinePos - nStartPos;
   char *filetext = (char *)malloc( (nLength+1) * sizeof(char) );
   UINT nCurPos = bs.GetPos();
   bs.Seek( nStartPos, BaseStream::ST_BEGINNING );
   bs.Read( filetext, nLength );
   filetext[nLength] = '\0';
   bs.Seek( nCurPos, BaseStream::ST_BEGINNING );

   String strFileText = filetext;
   strFileText.RemoveChar( '\x0d' );
   HPlaceholderObject *pPlaceholderObject = pParent->GetHHierObject()->AddPlaceholderObject( strFileText );
   free( filetext );
   pPlaceholderObject->SetName( strHeader );
   SetHSnipeObject( pPlaceholderObject );
}

HashObject::ParseValue UnknownHashObject::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   return PARSE_END;
}

CMap< String, const String &, HImage *, HImage * > Still::m_imagemap;

HashObject::ParseValue Still::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (strKey == "FileName" || (m_fProductVersion<8.53 && strKey=="Name"))
   {
      String filename;
      ls.ReadLine( filename );
      SetHSnipeObject( GetHMainInterface()->GetCurrentProject()->AddImage( filename ));
      if (m_fProductVersion >= 8.53 && m_fProductVersion < 10.01)
         m_imagemap.SetAt( filename.GetNameOnlyWithoutSuffix(), GetHImage() );
      else
         m_imagemap.SetAt( filename, GetHImage() );
      return PARSE_OK;
   }
   else if (strKey == "ENDSTILL")
      return PARSE_END;

   return HashObject::ParseArg( hs, ls, strKey );
}

Group::Group( HashObject *pParent )
{
   m_pGroupImage = NULL;
}

Group::~Group()
{
}

HashObject *Group::CreateObject( const String &strHeader, BaseStream &bs )
{
   if (strHeader == "DECALIMAGE")
   {
      HSurface *pSurface = CreateHSurface();
      m_pGroupImage = SNEW GroupImage( this );
      return m_pGroupImage;
   }
   else
      return HashObject::CreateObject( strHeader, bs );
}

HSurface *Group::CreateHSurface()
{
   HSurface *pSurface = GetHSurface();
   if (!pSurface)
   {
      Model *pModel = MdlTranslator::GetModel();
      pSurface = pModel->GetHPatchModel()->AddSurface( m_strName.Get(), pModel->m_nGroupColorSurfaceIndex++ );
      SetHSnipeObject( pSurface );
   }

   return pSurface;
}

HashObject::ParseValue Group::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (strKey == "Name")
   {
      ls.ReadLine( m_strName );
      if (GetHSurface())
         GetHSurface()->SetName( m_strName );
      return PARSE_OK;
   }
   else if (strKey == "Count")
   {
      int nCount;
      ls >> nCount;
      ReadCPs( hs, nCount );
      return PARSE_OK;
   }
   else if (strKey == "DiffuseColor")
   {
      RGBFloat color;
      ls >> color;
      color /= 255.0f;
      CreateHSurface()->GetDiffuseProperty()->StoreValue( color, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Transparency")
   {
      float fTransparency;
      ls >> fTransparency;
      CreateHSurface()->GetTransparencyProperty()->StoreValue( fTransparency, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "DECALIMAGES")
      return PARSE_OK;
   else if (strKey == "ENDDECALIMAGES")
   {
      return PARSE_OK;
   }
   else if (strKey == "ENDGROUP")
   {
      if (GetHSurface())
      {
         Model *pModel = MdlTranslator::GetModel();

         CArray<HashPatch *> patches;
         pModel->FindPatchesWithVertices( m_cplist, patches );

         HSurface *pSurface = GetHSurface();
         for (int i = 0; i < patches.GetSize(); i++)
            pSurface->AddFace( patches[i]->GetHPatch() );
      }
      else
      {
         HSelection *pSelection = MdlTranslator::GetModel()->GetHPatchModel()->AddSelection();
         SetHSnipeObject( pSelection );
         pSelection->SetName( m_strName );

         HSnipeObjectList *pVertices = HSnipeObjectList::New();
         for (POSITION pos=m_cplist.GetHeadPosition(); pos; )
         {
            CP *pCP = m_cplist.GetNext( pos );
            pVertices->AddTail( pCP->GetVertex() );
         }
         pSelection->AddObjects( pVertices );
         delete pVertices;
      }
      return PARSE_END;
   }

   return HashObject::ParseArg( hs, ls, strKey );
}

BOOL Group::ReadCPs( BaseStream &hs, int nCount )
{
   Model *pModel = MdlTranslator::GetModel();

   for (int i = 0; i < nCount; i++)
   {
      int cpid;
      hs >> cpid;

      CP *pCP;
      if (!pModel->m_cpmap.Lookup( cpid, pCP ))
      {
         ASSERT(FALSE);
         continue;
      }

      m_cplist.AddTail( pCP );
   }

   return TRUE;
}

Decal::Decal( HashObject *pParent )
{
   HSurface *pSurface = ((Model *)pParent)->GetHPatchModel()->AddSurface();
   SetHSnipeObject( pSurface );
   m_bNamed = FALSE;
}

HashObject::ParseValue Decal::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (strKey == "Name")
   {
      String name;
      ls >> name;
      GetHSurface()->SetName( name );
      m_bNamed = TRUE;
      return PARSE_OK;
   }
   else if (strKey == "DECALIMAGES"
    || strKey == "ENDDECALIMAGES"
    || strKey == "STAMPS"
    || strKey == "ENDSTAMPS")
      return PARSE_OK;
   else if (strKey == "ENDDECAL")
      return PARSE_END;

   return HashObject::ParseArg( hs, ls, strKey );
}

DecalImage::DecalImage( HashObject *pParent )
{
   m_pDecal = (Decal *)pParent;
   m_pDecal->m_decalimages.Add( this );
   m_pSurface = m_pDecal->GetHSurface();
   HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver *)GetHMainInterface()->CreateObject(SOT_UVMapSurfaceDriver);
   m_pSurface->AddSurfaceDriver( pUVMapSurfaceDriver, m_pSurface->GetDiffuseProperty() );
   SetHSnipeObject( pUVMapSurfaceDriver );
}

GroupImage::GroupImage( HashObject *pParent )
{
   Group *pGroup = (Group *)pParent;
   //m_pDecal->m_decalimages.Add( this );
   m_pSurface = pGroup->GetHSurface();
   HClampMapSurfaceDriver *pClampMap = (HClampMapSurfaceDriver *)GetHMainInterface()->CreateObject( SOT_ClampMapSurfaceDriver );
   m_pSurface->AddSurfaceDriver( pClampMap, m_pSurface->GetDiffuseProperty() );
   SetHSnipeObject( pClampMap );
}

HashObject::ParseValue DecalImage::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (strKey == "Name")
   {
      String name;
      ls >> name;
      GetHUVMapSurfaceDriver()->SetName( name );
      return PARSE_OK;
   }
   else if (strKey == "Image"
    || strKey == "EndImage")
      return PARSE_OK;
   else if (strKey == "Value" || strKey == "ShortcutToLinked")
   {
      String strValue;
      ls.ReadLine( strValue );
      StringArray parts;
      strValue.Split('|',parts);
      String temp = parts[parts.GetSize()-1];
      temp = temp.StripQuotes();
      HImage *pImage;
      if (Still::m_imagemap.Lookup( temp, pImage ))
         GetHUVMapSurfaceDriver()->GetImage()->StoreValue( pImage, FALSE );

      if (m_pDecal && !m_pDecal->m_bNamed)
      {
         HSurface *pSurface = m_pDecal->GetHSurface();
         pSurface->SetName( temp.GetNameOnlyWithoutSuffix() );
      }
   }
   else if (strKey == "Repeat")
   {
      Vector2 repeat;
      ls >> repeat;
      Vector2 scale( 1.0f/repeat.x, 1.0f/repeat.y );
      GetHUVMapSurfaceDriver()->GetImage()->GetTransform()->GetScale()->StoreValue( scale, FALSE );
   }
   else if (strKey == "Percent")
   {
      float fPercent;
      ls >> fPercent;
      GetHUVMapSurfaceDriver()->GetOpacity()->StoreValue( fPercent / 100.0f, FALSE );
   }
   else if (strKey == "DecalType")
   {
      String strType;
      ls >> strType;

      HPropertyPointerProperty *pProperty = GetHUVMapSurfaceDriver()->GetProperty();
      if (strType == "Transparency")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "Bump")
         pProperty->StoreValue( m_pSurface->GetNormalOffsetProperty(), FALSE );
      else if (strType == "SpecularSize")
         pProperty->StoreValue( m_pSurface->GetSpecularProperty(), FALSE );
      else if (strType == "SpecularIntensity")
         pProperty->StoreValue( m_pSurface->GetSpecularProperty(), FALSE );
      else if (strType == "Diffuse")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "Reflectivity")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "Ambiance")
         pProperty->StoreValue( m_pSurface->GetAmbientProperty(), FALSE );
      else if (strType == "Cookie-Cut")
      {
         pProperty->StoreValue( m_pSurface->GetDiffuseProperty(), FALSE );
         GetHUVMapSurfaceDriver()->GetBlendStyle()->StoreValue( HUVMapSurfaceDriver::REPLACE, FALSE );
      }
      else if (strType == "Displacement")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "Fractal")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "NextMapFactor")
         pProperty->StoreValue( NULL, FALSE );
      else if (strType == "Other")
         pProperty->StoreValue( NULL, FALSE );

      if (pProperty->GetValue() == NULL)
         DisplayWarning( m_pSurface->GetName() + " uses unsupported decal type: " + strType );
   }
   else if (strKey == "ENDDECALIMAGE")
      return PARSE_END;

   return HashObject::ParseArg( hs, ls, strKey );
}

Stamp::Stamp( HashObject *pParent )
{
   Decal *pDecal = (Decal *)pParent;
   m_pSurface = pDecal->GetHSurface();

   HTexCoords *pTexCoords = (HTexCoords *)pDecal->m_decalimages[0]->GetHUVMapSurfaceDriver()->GetTexCoords()->GetValue();
   if (!pTexCoords)
   {
      HPatchModel *pPatchModel = (HPatchModel *)m_pSurface->GetParent()->GetParent();
      pTexCoords = pPatchModel->AddTexCoords();
      for (int i=0; i<pDecal->m_decalimages.GetCount(); i++)
      {
         DecalImage *pDecalImage = pDecal->m_decalimages[i];
         pDecalImage->GetHUVMapSurfaceDriver()->GetTexCoords()->StoreValue( pTexCoords, FALSE );
      }
   }

   SetHSnipeObject( pTexCoords );
}

HashObject::ParseValue Stamp::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (strKey == "Name")
   {
      String name;
      ls >> name;
      GetHTexCoords()->SetName( name );
      return PARSE_OK;
   }
   else if (strKey == "DATA")
   {
      ReadData( hs );
      return PARSE_OK;
   }
   else if (strKey == "ENDDATA")
      return PARSE_OK;
   else if (strKey == "ENDSTAMP")
      return PARSE_END;

   return HashObject::ParseArg( hs, ls, strKey );
}

static HUV *FindSharedUV( HTexCoords *pTexCoords, HPoly *pConstrainedPoly, CP *pConstrainedCP )
{
   // try to find a uv for the constrained vertex in one of the surrounding polystamps
   HUV *pFoundUV = NULL;
   HUVPoly *pUVPoly0 = NULL, *pUVPoly1 = NULL;
   UINT nNumSurroundingPolys = pConstrainedCP->GetNumSurroundingPolys();
   for (UINT nPolyIndex=0; nPolyIndex<nNumSurroundingPolys; nPolyIndex++)
   {
      HPoly *pPoly = pConstrainedCP->GetSurroundingPolyAt( nPolyIndex );
      HUVPoly *pUVPoly = pPoly->FindUVPoly( pTexCoords );
      if (pUVPoly)
      {
         UINT nIndex = pPoly->FindVertexFaceRelativeIndex( pConstrainedCP->GetVertex() );
         HUV *pUV = pUVPoly->GetUVAt( nIndex );
         if (pUV)
         {
            if (!pUVPoly0)
            {
               pUVPoly0 = pUVPoly;
               pFoundUV = pUV;
            }
            else
            {
               if (pFoundUV != pUV)
                  continue;
               pUVPoly1 = pUVPoly;
               break;
            }
         }
      }
   }

   if (!pUVPoly1)
      return NULL;

   HUVPoly *pConstrainedUVPoly = pConstrainedPoly->FindUVPoly( pTexCoords );
   for (CP *pPrevCP=pConstrainedCP->PreviousInSpline(TRUE); pPrevCP && pPrevCP->IsTConstrained(); pPrevCP=pPrevCP->PreviousInSpline(TRUE))
      ;
   for (CP *pNextCP=pConstrainedCP->NextInSpline(TRUE); pNextCP && pNextCP->IsTConstrained(); pNextCP=pNextCP->NextInSpline(TRUE))
      ;

   if (pPrevCP)
   {
      UINT nPrevIndex = pConstrainedPoly->FindVertexFaceRelativeIndex( pPrevCP->GetVertex() );
      HUV *pPrevUV = pConstrainedUVPoly->GetUVAt( nPrevIndex );
      if (pUVPoly0->FindUVFaceRelativeIndex( pPrevUV )==-1 && pUVPoly1->FindUVFaceRelativeIndex( pPrevUV )==-1)
         return NULL;
   }

   if (pNextCP)
   {
      UINT nNextIndex = pConstrainedPoly->FindVertexFaceRelativeIndex( pNextCP->GetVertex() );
      HUV *pNextUV = pConstrainedUVPoly->GetUVAt( nNextIndex );
      if (pUVPoly0->FindUVFaceRelativeIndex( pNextUV )==-1 && pUVPoly1->FindUVFaceRelativeIndex( pNextUV )==-1)
         return NULL;
   }

   return pFoundUV;
}

class CUVList : public CList<HUV *>
{
};

class CUVMap : public CMap<Vector, const Vector &, CUVList *, CUVList *>
{
public:
   ~CUVMap()
   {
      Vector key;
      for (POSITION pos=GetStartPosition(); pos; )
      {
         CUVList *pUVList;
         GetNextAssoc( pos, key, pUVList );
         delete pUVList;
      }
   }
};

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const Vector &key )
{
   double x = (int)(key.x * 1000.0f + SIGN(key.x) * .5f)/1000.0*12.3;
   double y = (int)(key.y * 1000.0f + SIGN(key.y) * .5f)/1000.0*23.4;
   double z = (int)(key.z * 1000.0f + SIGN(key.z) * .5f)/1000.0*34.5;
   return (UINT) (pow(x*x + y*y + z*z, 0.5 ) * 9.87);  
}

BOOL Stamp::ReadData( BaseStream &hs )
{
   static MemoryStream ms(256);
   Label label;

   int numpatches;
   hs >> label >> numpatches;
   if (strcmp(label.string, "PatchesHit")!=0)
      return FALSE;

   if (numpatches==0)
   {
      String temp = "Skipping empty stamp. [ ";
      temp += GetHTexCoords()->GetName();
      temp += " ]";
      //            DisplayWarning(temp);
      return FALSE;
   }

   CUVMap uvmap;

   HTexCoords *pTexCoords = GetHTexCoords();
   Model *pModel = MdlTranslator::GetModel();
   HPatchModel *pSplineModel = pModel->GetHPatchModel();
   CList<CP *> constrainedcplist;

   for (int i = 0; i < numpatches; i++)
   {
      if (!hs.ReadLine( ms ))
         return FALSE;

      int id[5];
      int cpcount = sscanf( ms.GetBuffer(), "%d %d %d %d %d", &id[0], &id[1], &id[2], &id[3], &id[4] );

      CArray<CP *> vertices;
      vertices.SetSize(4);
      // find poly
      for (int j=0; j<4; j++)// always only check 4, 5th is always "5" on five point patches!
      {
         int cpid = id[j];
         CP *pCP;
         if (!pModel->m_cpmap.Lookup( cpid, pCP ))
            return FALSE;
         vertices.SetAt( j, pCP );
      }

      int offset;
      HashPatch *pPatch = pModel->FindPatchWith4MatchingVerts( vertices, offset );
      HPoly *pPoly = NULL;
      if (pPatch)
         pPoly = pPatch->GetHPatch();
      if (pPoly && !pPoly->FindUVPoly( pTexCoords ))
      {
         HUVPoly *pUVPoly = pTexCoords->AddPoly(pPoly);
         UINT nUVPolyID = pUVPoly->GetID();

         BOOL bBogus = FALSE;
         UINT nNumSides = pUVPoly->GetNumSides();
         for (UINT j=0; j<nNumSides; j++)
         {
            CP *pCP = pPatch->GetCPAt( j );
            //HVertex *pVertex = pPoly->GetVertexAt( j );
            ASSERT( pCP );
            if (pCP->IsTConstrained() && pCP->IsTConstrainedCPInEdge( pPoly, j ))
            {
               constrainedcplist.AddTail( pCP );
               continue;
            }

            if (!hs.ReadLine( ms ))
               return FALSE;

            Vector uvpos;
            ms >> uvpos;

            CUVList *pUVList;
            HUV *pUV = NULL;
            if (!uvmap.Lookup( uvpos, pUVList ))
            {
               pUVList = SNEW CUVList;
               pUV = pTexCoords->AddUV( Vector2(uvpos.x, uvpos.y) );
               UINT nUVID = pUV->GetID();
               pUVList->AddTail( pUV );
               uvmap.SetAt( uvpos, pUVList );
            }
            else
            {
               // see if we can find a uv in the list of uvs at this coord that has a surrounding poly that touches our poly
               for (POSITION pos=pUVList->GetHeadPosition(); pos; )
               {
                  HUV *pUVInList = pUVList->GetNext( pos );
                  ASSERT(pUVInList->GetNumSurroundingUVPolys());
                  HUVPoly *pSurroundingUVPoly = pUVInList->GetSurroundingUVPolyAt(0); // any one will do
                  ASSERT( pSurroundingUVPoly );
                  HPoly *pSurroundingPoly = pSurroundingUVPoly->GetPoly();
                  ASSERT( pSurroundingPoly );

                  // get a vertex in the model that corresponds to the uv
                  UINT nIndex = pSurroundingUVPoly->FindUVFaceRelativeIndex( pUVInList );
                  ASSERT( nIndex != -1 );
                  HVertex *pVertex = pSurroundingPoly->GetVertexAt( nIndex );
                  ASSERT( pVertex );

                  if (pVertex->FindSurroundingPoly( pPoly ) != -1)
                  {
                     pUV = pUVInList;
                     break;
                  }
               }

               if (pUV) // try to see if the same uv is already in this uvpoly (tiny geometry)
               {
                  for (UINT k=0; k<j; k++)
                  {
                     if (pUVPoly->GetUVAt(k) == pUV)
                     {
                        //                     bBogus = TRUE;
                        pUV = NULL;
                        break;
                     }
                  }
               }

               if (!pUV) // we couldn't find one, so make a new one
               {
                  pUV = pTexCoords->AddUV( Vector2(uvpos.x, uvpos.y) );
                  pUVList->AddTail( pUV );
               }
            }

            ASSERT(pUV);
            pUVPoly->SetUVAt( j, pUV );
         }

         if (id[0] == id[3]) // three point patch
         {
            if (!hs.ReadLine( ms )) // read off extra uv coords
               return FALSE;
         }

         if (bBogus)
            pTexCoords->RemovePoly( pUVPoly );
         else
            m_pSurface->AddFace(pPoly);
      }
      else
      {
         for (int j=0; j < cpcount; j++)
         {
            if (!hs.ReadLine( ms ))
               return FALSE;
         }
      }
   }

   // We need to now go back and fill in constrained UV's
   for (POSITION pos=constrainedcplist.GetHeadPosition(); pos; )
   {
      CP *pConstrainedCP = constrainedcplist.GetNext( pos );
      UINT nCPID = pConstrainedCP->GetID();
      //UINT nHookID = pConstrainedCP->GetChildCP()->GetID();
      UINT nNumSurroundingPolys = pConstrainedCP->GetNumSurroundingPolys();
      for (UINT nPolyIndex=0; nPolyIndex<nNumSurroundingPolys; nPolyIndex++)
      {
         HPoly *pPoly = pConstrainedCP->GetSurroundingPolyAt( nPolyIndex );
         UINT nPolyID = pPoly->GetID();
         UINT nVertexCount = pPoly->GetNumSides();
         HUVPoly *pUVPoly = pPoly->FindUVPoly( pTexCoords );

         if (pUVPoly)
         {
            UINT nIndex = pPoly->FindVertexFaceRelativeIndex( pConstrainedCP->GetVertex() );
            HUV *pUV = pUVPoly->GetUVAt( nIndex );
            if (!pUV)
            {
               pUV = FindSharedUV( pTexCoords, pPoly, pConstrainedCP );
               if (pUV)
                  pUVPoly->SetUVAt( nIndex, pUV );
               else
               {
                  int nPos = 1, nEdgeCount = 2;
                  for (CP *pPrevCP=pConstrainedCP->PreviousInSpline(TRUE); pPrevCP && pPrevCP->IsTConstrained(); pPrevCP=pPrevCP->PreviousInSpline(TRUE), nEdgeCount++, nPos++)
                     ;
                  for (CP *pNextCP=pConstrainedCP->NextInSpline(TRUE); pNextCP && pNextCP->IsTConstrained(); pNextCP=pNextCP->NextInSpline(TRUE), nEdgeCount++)
                     ;
                  if (pPrevCP && pNextCP)
                  {
                     UINT nPrevIndex = pPoly->FindVertexFaceRelativeIndex( pPrevCP->GetVertex() );
                     UINT nNextIndex = pPoly->FindVertexFaceRelativeIndex( pNextCP->GetVertex() );
                     HUV *pPrevUV = pUVPoly->GetUVAt( nPrevIndex );
                     HUV *pNextUV = pUVPoly->GetUVAt( nNextIndex );
                     if (pPrevUV && pNextUV)
                     {
                        float fPos = (float)nPos / nEdgeCount;
                        Vector2 p = pPrevUV->GetPos() + (pNextUV->GetPos() - pPrevUV->GetPos()) * fPos;

                        HUV *pUV;
                        CUVList *pUVList;
                        if (!uvmap.Lookup( p, pUVList ))
                        {
                           pUVList = SNEW CUVList;
                           pUV = pTexCoords->AddUV( p );
                           pUVList->AddTail( pUV );
                           uvmap.SetAt( p, pUVList );
                        }
                        else
                        {
                           ASSERT( pUVList->GetCount() == 1 );
                           pUV = pUVList->GetHead();
                        }

                        pUVPoly->SetUVAt( nIndex, pUV );
                     }
                     else
                        ASSERT(FALSE);
                  }
               }
            }
         }
      }
   }

#ifdef _DEBUG
   for (POSITION pos=pTexCoords->GetHeadPropertyPolyPosition(); pos; )
   {
      HPropertyPoly *pPropertyPoly = pTexCoords->GetNextPropertyPoly(pos);
      if (pPropertyPoly->GetPropertySet()->GetSOT()!=SOT_TexCoords)
         continue;
      HUVPoly *pUVPoly = (HUVPoly*)pPropertyPoly;
      for (UINT i=0; i<pUVPoly->GetNumSides(); i++)
      {
         HPoly *pPoly = pUVPoly->GetPoly();
         UINT nPolyID = pPoly->GetID();
         ASSERT( pUVPoly->GetUVAt(i));
      }
   }
#endif

   return TRUE;
}

HashBone::HashBone()
{
   m_pModel = NULL;
}

HashBone::HashBone( HashObject *pParent )
{
   HBone *pBone = (HBone *)GetHMainInterface()->CreateObject( SOT_Bone );
   HashBone *pParentBone = (HashBone *)pParent;
   m_pModel = pParentBone->m_pModel;
   m_pModel->AddBone( pBone, pParentBone->GetHBone() );
   SetHSnipeObject( pBone );
}

HashObject::ParseValue HashBone::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   HBone *pBone = GetHBone();

   if (strKey == "Name")
   {
      String name;
      ls.ReadLine( name );
      GetHBone()->SetName( name );
      return PARSE_OK;
   }
   else if (strKey == "BoneColor")
   {
      RGBFloat color;
      ls >> color;
      color /= 255.0f;
      GetHBone()->GetBoneColor()->StoreValue( color, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Hidden")
   {
      BOOL bHidden;
      ls >> bHidden;
      GetHBone()->GetVisible()->StoreValue( !bHidden, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Start")
   {
      Vector vStart;
      ls >> vStart;
      pBone->GetStart()->StoreValue( vStart, FALSE );
      //HTransformableObject *pParent = (HTransformableObject *)pBone->GetParent();
      //Vector vParentPos;
      //pParent->GetWorldPos( vParentPos );
      //vTranslate -= vParentPos;
      //pBone->GetTransform()->GetTranslate()->StoreValue( vTranslate, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Rotate")
   {
      Vector v;
      float w;
      ls >> v >> w;

      Quaternion quat( v, w );
      RotateEuler euler( quat );
      pBone->GetRotate()->StoreValue( Vector(euler.x, euler.y, euler.z), FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Length")
   {
      float fLength;
      ls >> fLength;
      pBone->GetLength()->StoreValue( fLength, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Chained")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetChained()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Locked")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetLockIK()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "RollMethod")
   {
      String strRollMethod;
      ls >> strRollMethod;
      if (strRollMethod == "Y-Poles-Singularity")
         pBone->GetRollMethod()->StoreValue( HBone::RM_YPOLESSINGULARITY, FALSE );
      else if (strRollMethod == "Roll-History")
         pBone->GetRollMethod()->StoreValue( HBone::RM_ROLLHISTORY, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "Manipulation") // method
   {
      String strManipulationMethod;
      ls >> strManipulationMethod;
      if (strManipulationMethod == "TranslateOnly")
         pBone->GetManipulationMethod()->StoreValue( HBone::MM_TRANSLATEONLY, FALSE );
      else if (strManipulationMethod == "RotateOnly")
         pBone->GetManipulationMethod()->StoreValue( HBone::MM_ROTATEONLY, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "LimitManipulators")
   {
      MemoryStream ms(255);
      hs.ReadLine( ms );
      Label label;
      StreamBOOL sbool;
      ms >> label >> sbool;
      if (strcmp(label.string, "Value") != 0)
         return PARSE_FAIL;
      pBone->GetLimitManipulators()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowXTranslate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowXTranslate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowYTranslate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowYTranslate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowZTranslate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowZTranslate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowXScale")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowXScale()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowYScale")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowYScale()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowZScale")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowZScale()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowXRotate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowXRotate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowYRotate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowYRotate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "AllowZRotate")
   {
      StreamBOOL sbool;
      ls >> sbool;
      pBone->GetAllowZRotate()->StoreValue( sbool.m_bool, FALSE );
      return PARSE_OK;
   }
   else if (strKey == "NONSKINNEDCPS")
   {
      ReadNonSkinnedCPs( hs );
      return PARSE_OK;
   }
   else if (strKey == "ENDSEGMENT")
      return PARSE_END;

   return HashObject::ParseArg( hs, ls, strKey );
}

void HashBone::ReadNonSkinnedCPs( BaseStream &bs )
{
   Label label;
   int nCPCount;
   bs >> label >> nCPCount;
   if (strcmp(label.string, "Count")!=0)
      return;

   HBone *pBone = GetHBone();
   HPatchModel *pModel = MdlTranslator::GetModel()->GetHPatchModel();

   for (int i=0; i<nCPCount; i++)
   {
      UINT nID;
      bs >> nID;

      HVertex *pVertex = pModel->FindVertex( nID );
      if (pVertex)
         pBone->AddVertex( pVertex );
      else
         continue; // just here for a place to put a breakpoint
   }

   String header;
   bs.ReadLine( header ); // [ENDNONSKINNEDCPS]
}

static CP *GetOtherCP( HEdge *pEdge, CP *pCP )
{
   for (; pCP && pCP->GetParentCP(); pCP=pCP->GetParentCP())
      ;

   for (; pCP; pCP=pCP->GetChildCP())
   {
      if (pCP->GetNextEdge() == pEdge)
         return pCP->NextInSpline(TRUE);
      else if (pCP->GetPrevEdge() == pEdge)
         return pCP->PreviousInSpline(TRUE);
   }

   return NULL;
//
//   HVertex *pVertex = pCP->GetVertex();
//   HVertex *pOtherVertex = pEdge->GetConnectedVertex( pVertex );
//   ASSERT( pOtherVertex );
//
//   for (; pCP && pCP->GetParentCP(); pCP=pCP->GetParentCP())
//      ;
//
//   for (; pCP; pCP=pCP->GetChildCP())
//   {
//      if (pCP->NextInSpline() && pCP->NextInSpline()->GetVertex()==pOtherVertex)
//         return pCP->NextInSpline();
//      else if (pCP->PreviousInSpline() && pCP->PreviousInSpline()->GetVertex()==pOtherVertex)
//         return pCP->PreviousInSpline();
//   }
//
//   return NULL;
}

static CP *GetNextInSpline( CP *pPrev, CP *pThis )
{
//UINT nPrevID = pPrev->GetID();
//UINT nThisID = pThis->GetID();

   if (pPrev->NextInSpline(TRUE) && pPrev->NextInSpline(TRUE)->IsAttached(pThis))
      return pThis->NextInSpline(TRUE);
   else if (pPrev->PreviousInSpline(TRUE) && pPrev->PreviousInSpline(TRUE)->IsAttached(pThis))
      return pThis->PreviousInSpline(TRUE);
   else
      return NULL;
}

static HEdge *FindEdge( CP *pStartCP, CP *pEndCP, BOOL &bHasConstrained )
{
   ASSERT( pStartCP );
   ASSERT( pEndCP );

   bHasConstrained = FALSE;

   UINT nNumSurroundingEdges = pStartCP->GetNumSurroundingEdges();
UINT nID0 = pStartCP->GetID();
UINT nID1 = pEndCP->GetID();

   for (UINT i=0; i < nNumSurroundingEdges; i++)
   {
      HEdge *pEdge = pStartCP->GetSurroundingEdgeAt( i );

UINT nID2 = pEdge->GetFirstVertex()->GetID();
UINT nID3 = pEdge->GetSecondVertex()->GetID();

      CP *pPrevCP = pStartCP;
      CP *pNextCP = GetOtherCP( pEdge, pStartCP );

UINT nID4 = pPrevCP->GetID();
UINT nID5 = pNextCP->GetID();

      bHasConstrained = FALSE;
      while (pNextCP && pNextCP->IsTConstrained() && !pNextCP->IsAttached(pEndCP))
      {
         CP *pTemp = GetNextInSpline( pPrevCP, pNextCP ) ;
         pPrevCP = pNextCP;
         pNextCP = pTemp;
         bHasConstrained = TRUE;
      }

      if ( pNextCP && pNextCP->IsAttached(pEndCP) )
         return pEdge;
   }
   
   return NULL;
}

HashPatch *Model::AddPatch()
{
   HashPatch *pPatch = SNEW HashPatch( this );
   m_patchmap.SetAt( pPatch->GetHPatch(), pPatch );
   return pPatch;
}

BOOL Model::ReadPatch( BaseStream &hs, BOOL bFivePoint )
{
   static MemoryStream ms(256);

   AMPatchFlags flags;
   hs >> flags.m_flags;

   if (!hs.ReadLine( ms ))
      return FALSE;

   ULONG id[5];
   int nCount = sscanf( ms.GetBuffer(), "%d %d %d %d %d", 
      &id[0], &id[1], &id[2], &id[3], &id[4] ); // Don't care about real-time normal or attribute indices

   CArray<CP*> cps, cps2;

   UINT numCPs = bFivePoint ? 5 : 4;
   if (id[0] == id[numCPs - 1])
      numCPs = 3;
   cps.SetSize(numCPs);

   for (UINT i = 0; i < numCPs; i++)
   {
      CP *pCP;
      if (!m_cpmap.Lookup( id[i], pCP ))
      {
         ASSERT(FALSE);//invalid cp
         return FALSE;
      }
      cps.SetAt( i, pCP );
   }

   // Search to see if this patch has a hook in one of its walls.
   // If so, add that hook as another vertex in the patch.
   for (UINT i = 0; i < numCPs; i++)
   {
      BOOL bHasConstrained;
      UINT nNextIndex = (i == numCPs-1) ? 0 : i+1;
UINT ID0 = cps[i]->GetID();
UINT ID1 = cps[nNextIndex]->GetID();

      HEdge *pEdge = ::FindEdge( cps[i], cps[nNextIndex], bHasConstrained );
      ASSERT(pEdge);

      cps2.Add( cps[i] );

      if (bHasConstrained)
      {
         CP *pPrevCP = cps[i];
         CP *pNextCP = GetOtherCP( pEdge, pPrevCP );

         while (pNextCP->IsTConstrained())
         {
            cps2.Add( pNextCP );
            CP *pTemp = GetNextInSpline( pPrevCP, pNextCP ) ;
            pPrevCP = pNextCP;
            pNextCP = pTemp;
         }
      }
   }

   HashPatch *pPatch = AddPatch();
UINT nPatchID = pPatch->GetID();
   SetPolyFlagsFromHashFlags( pPatch, flags );

   UINT numVerts = cps2.GetSize();
   pPatch->SetNumSides(numVerts);

   for (UINT i = 0; i < numVerts; i++)
   {
      CP *pCP = cps2[i];
      ASSERT(pCP);

      // build vertices
      pPatch->SetCPAt( i, pCP );
   }

   return TRUE;
}

void Model::SetPolyFlagsFromHashFlags( HashPatch *pPatch, AMPatchFlags flags )
{
   m_patchrotmap.AddKey( (int)pPatch, flags.m_groupimagerotation );
}

HashPatch *Model::FindPatchWith4MatchingVerts(CArray<CP *> &vertices,int &nOffset)
{
   UINT nNumVerts = vertices.GetSize();
   BOOL bFlag;
   for (UINT i = 0; i < nNumVerts; i++)
   {
      if (vertices[i]->IsTConstrained())
         continue;

      UINT nSize = 3;
#if 1
      if (vertices[0] == vertices[3])
         nSize = 2;
#endif
      for (UINT j = 0; j < vertices[i]->GetNumSurroundingPolys(); j++)
      {
         HPoly *pSurroundingPoly = vertices[i]->GetSurroundingPolyAt(j);
         int nStartIndex = pSurroundingPoly->FindVertexFaceRelativeIndex(vertices[i]->GetVertex());
         if (nStartIndex<0)
            continue;

         UINT l = i;
         UINT m = nStartIndex;
         bFlag = FALSE;
         for (UINT k = 0; k < nSize; k++)
         {
            if (vertices[l]->IsTConstrained())
               continue;

            if (++l >= nNumVerts)
               l = 0;

            //do // new do while loop (10/2/05) for skipping hooks
            //{
               if (++m >= pSurroundingPoly->GetNumSides())
                  m = 0;
            //} while (((HPatchVertex *)pSurroundingPoly->GetVertexAt(m))->IsConstrained());

            if (!vertices[l]->IsAttached( pSurroundingPoly->GetVertexAt(m) ))
            {
               bFlag = TRUE;
               break;
            }
         }
         if (!bFlag)
         {
            nOffset = nStartIndex;
            HashPatch *pPatch;
            m_patchmap.Lookup( pSurroundingPoly, pPatch );
            return pPatch;
         }
      }
   }
   return NULL;
}

void Model::FindPatchesWithVertices( CList<CP *> &cplist, CArray<HashPatch *> &patches )
{
   HPatchModel *pModel = GetHPatchModel();

   // first make a map of the top vertex indices for quick lookup
   CMap<HVertex *, HVertex *, UINT, UINT> vertexmap;
   vertexmap.InitHashTable( 1019 );
   for (POSITION pos=cplist.GetHeadPosition(); pos; )
   {
      CP *pCP = cplist.GetNext( pos );
      HVertex *pVertex = pCP->GetVertex();
      vertexmap.SetAt( pVertex, 0 );
   }

   for (POSITION pos=m_patchmap.GetStartPosition(); pos; )
   {
      HPoly *pPoly;
      HashPatch *pPatch;
      m_patchmap.GetNextAssoc( pos, pPoly, pPatch );
UINT nPatchID = pPatch->GetID();
      UINT nNumSides = pPatch->GetNumSides();
      for (UINT nIndex=0; nIndex<nNumSides; nIndex++)
      {
         CP *pCP = pPatch->GetCPAt( nIndex );
         HVertex *pVertex = pCP->GetVertex();
         UINT nValue;
         if (!vertexmap.Lookup( pVertex, nValue ))
            break;
      }

      if (nIndex == nNumSides) // found all vertices in this patch
         patches.Add( pPatch );
   }
}
