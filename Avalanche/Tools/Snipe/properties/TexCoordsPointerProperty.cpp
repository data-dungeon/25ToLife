#include "stdafx.h"
#include "UV.h"
#include "UVPoly.h"
#include "TexCoordsPointerProperty.h"
#include "PolyModel.h"

/////////////////////////////////////////
// TexCoordsPointerProperty

TexCoords *BasicProperty<TexCoords *>::m_prevvalue;

IMPLEMENT_SNIPEOBJECT_CREATE( TexCoordsPointerProperty );

BaseStream &operator << ( BaseStream &bs, const TexCoords *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, TexCoords *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}

BOOL TexCoordsPointerProperty::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   SnipeObject *pCommandObject = GetCommandObject();
   if (!pCommandObject)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   PolyModel *pPolyModel = (PolyModel *)pCommandObject->IsKindOf( SOT_PolyModel );
   if (!pPolyModel)
   {
      ASSERT( FALSE );
      return TRUE;
   }

   TexCoordsContainer *pTexCoordsContainer = pPolyModel->GetTexCoordsContainer();
   if (!pTexCoordsContainer)
      return TRUE;

   for (UINT i=0; i<pTexCoordsContainer->GetNumTexCoords(); i++)
   {
      TexCoords *pTexCoords = pTexCoordsContainer->GetTexCoordsAt( i );
      pSnipeObjectList->AddTail( pTexCoords );
   }

   return TRUE;
}
