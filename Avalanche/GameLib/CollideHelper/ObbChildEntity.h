/***************************************************************************/
// A simple class to manage a list of OBB's
/***************************************************************************/
#ifndef OBBCHILD_ENTITY_H
#define OBBCHILD_ENTITY_H

#include "collide/QueryEntity.h"
#include "CollideHelper/obbchild.h"

class OBB;

class OBBChildListEntity : public CQueryEntity
{
  public:
	OBBChildListEntity(OBBChildList &i_obbChildList) :
		CQueryEntity(s_obbChildListClass), d_obbChildList(i_obbChildList) {}
	static const CEntityType &ClassType(void) {return(s_obbChildListClass);}

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull)
		{return(d_obbChildList.GetCollisionCull(o_cull));}
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select)
		{
			if (IsIgnoredEntity(i_entity))
				return(NULL);
			return(&d_obbChildList);
		}
	virtual const char* GetClassName(void) {return("ObbChildList");}

  private:
	OBBChildListEntity();

	static const CEntityType s_obbChildListClass;
	OBBChildList &d_obbChildList;
};

#endif // OBBCHILD_ENTITY_H
