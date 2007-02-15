// Dan  6/25/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\MirrorBones\Evaluate.cpp
#include "stdafx.h"
#include "Hxt.h"
#include <vector>
#include <afxwin.h>
#include <afxdlgs.h>
#include "sdk/hconstra.h"
#include "F_Protos.h"
#include "Files.h"
#include "MessageR.h"
#include "sdk/misc.h"

double DistanceTo(Vector &A,Vector &B)
{
	return (Vector(A.x-B.x,A.y-B.y,A.z-B.z).Norm());
}

HBoneCache *CHxtApp::DuplicateBone(HBoneCache *boneSrc, HBoneCache *boneParent,String name, bool invertX)
{									
	HSegmentCache *segmentDst = m_hmc->AddSegment(boneParent,NULL,name,GetColor(m_hmc->CountBones()));
				
	HBoneCache *boneDst = (HBoneCache*)segmentDst;

	CopyBoneAttributes(boneDst,boneSrc,invertX);

	return boneDst;
}

HBoneCache *CHxtApp::CreateLeftBone(BonePair *bonepair)
{
	HBoneCache *boneRight = (HBoneCache*)bonepair->right;
	HBoneCache *boneRightParent = (HBoneCache*)bonepair->rightparent;

	String name = Replace(boneRight->GetMatchName(),"Right","Left");
				
	HBoneCache *boneLeftParent = (HBoneCache*)bonepair->rightparent;

	bool HasKey = m_parentmap.HasKey((int)boneRightParent);

	HBoneCache *boneLeft = NULL;
	
	if (HasKey)
	{
      int index;
      m_parentmap.GetValue((int)boneRightParent, index);
		BonePair *bonepair2 = &m_bonepairs[index];
		if (bonepair2->leftparent)
		{
			boneLeftParent = (HBoneCache*)bonepair2->left;
		}
		else
		{
			boneLeftParent = CreateLeftBone(bonepair2);
		}
	}

	boneLeft = DuplicateBone(boneRight,boneLeftParent,name);

	bonepair->left = boneLeft;
	
	bonepair->leftparent = boneLeftParent;

	return boneLeft;
}

HBoneCache *CHxtApp::CreateRightBone(BonePair *bonepair)
{
	HBoneCache *boneLeft = (HBoneCache*)bonepair->left;
	HBoneCache *boneLeftParent = (HBoneCache*)bonepair->leftparent;

	String name = Replace(boneLeft->GetMatchName(),"Left","Right");
				
	HBoneCache *boneRightParent = (HBoneCache*)bonepair->leftparent;

	bool HasKey = m_parentmap.HasKey((int)boneLeftParent);

	HBoneCache *boneRight = NULL;
	
	if (HasKey)
	{
		int index;
      m_parentmap.GetValue((int)boneLeftParent, index);
//		String debug = boneLeftParent->GetMatchName();
		BonePair *bonepair2 = &m_bonepairs[index];

		ASSERT(bonepair2!=bonepair);

		if (bonepair2->rightparent)
		{
			boneRightParent = (HBoneCache*)bonepair2->right;
		}
		else
		{
			boneRightParent = CreateRightBone(bonepair2);
		}
	}

//	String debug = boneRightParent->GetMatchName();
	
	boneRight = DuplicateBone(boneLeft,boneRightParent,name);

	bonepair->right = boneRight;
	
	bonepair->rightparent = boneRightParent;

	return boneRight;
}

void CHxtApp::RebuildRelationshipList()
{
   m_relationships.clear();
   HTreeObject *relationshipcontainer = HashTools::FindTreeObject("User Properties",m_hmc);
   if (relationshipcontainer)
      HashTools::FindTreeObject(HOT_RELATION,(HTreeObject*)relationshipcontainer, m_relationships);
}   

BOOL CHxtApp::Evaluate(HTreeObject *htreeobject)
{
	int i;
	CWaitCursor cursor;
   
	HSegment *hs = (HSegment *) htreeobject;
	m_hmc = (HModelCache *)hs->GetParentOfType(HOT_MODEL);

	if (!htreeobject)
		return FALSE;

	if (htreeobject->GetObjectType()!=HOT_SEGMENT)
		return FALSE;

   m_hprogress = GetProgressBar();
   if (m_hprogress) {
      m_hprogress->Initialize();
      m_hprogress->SetMessage1("Initializing");
      m_hprogress->SetMessage2("Tables");
   }

	IntMap parentmap;

	m_bonepairs.clear();
	m_bonesingles.clear();
	m_bonessinglemap.Clear();
	m_parentmap.Clear();
	
	EvaluateBoneChildren(htreeobject,NULL);
	
	HashCPs cps;
	HashTools::GetAllCPs(m_hmc,cps);
   
// Get all relationships
   RebuildRelationshipList();

   m_currprogress = 0;
   m_totalprogresssize = m_bonepairs.size();
   if (m_assignvertices)
      m_totalprogresssize+=m_bonesingles.size();
   if (m_generatelogfile)
      m_totalprogresssize+=m_bonepairs.size();

   if (m_hprogress)
      m_hprogress->SetMessage1("Mirroring Bone");
   
   //Pairs
	for (i = 0; i < m_bonepairs.size(); i++)
	{
		Vector pivot,end;

		BonePair *bonepair = &m_bonepairs[i];

      if (m_hprogress) {
         m_hprogress->SetMessage2(bonepair->name.Get());
         m_hprogress->SetProgress(fast_ftol_unsigned(((float)m_currprogress++/m_totalprogresssize)*100));
      }
      
      BOOL createdleft = FALSE, createdright = FALSE;
      
		if (!bonepair->left)
		{
			if (m_createbones)
			{
				CreateLeftBone(bonepair);
            createdleft = TRUE;
			}
		}

		if (!bonepair->right)
		{
			if (m_createbones)
			{
				CreateRightBone(bonepair);
            createdright = TRUE;
			}
		}

		if (!bonepair->left)
			continue;
		if (!bonepair->right)
			continue;

		HBoneCache *leftbone = (HBoneCache*)bonepair->left;
		HBoneCache *rightbone = (HBoneCache*)bonepair->right;

		if (m_adjustright)
		{
			CopyBoneAttributes(rightbone,leftbone);

			if (m_assignvertices)
			{
            if (leftbone->GetObjectType()==HOT_SEGMENT ||
               rightbone->GetObjectType()==HOT_SEGMENT) {
            
				   HSegmentCache *leftsegment = (HSegmentCache*)leftbone;
				   HSegmentCache *rightsegment = (HSegmentCache*)rightbone;
            
				   for (HGroupCP *hgcpleft = leftsegment->GetGroupCP();hgcpleft; hgcpleft = hgcpleft->GetNext())
				   {
					   Vector findpos = *hgcpleft->GetCP()->GetPosition();
		   
					   findpos.x = -findpos.x;

					   // loop through all cps for inverted x position
					   for (int i = 0; i < cps.size(); i++)
					   {

						   HCP *hcp = cps[i];//cp from pool of all vertices

						   Vector pos = *hcp->GetPosition();

						   double distance = DistanceTo(findpos,pos);

						   // if position is within tolerance
						   if (distance<m_tolerance)
						   {
							   // does cp already belong to right bone
							   bool found = false;
							   for (HGroupCP *hgcpright = rightsegment->GetGroupCP();hgcpright; hgcpright = hgcpright->GetNext())
							   {
								   HCP *hcpright = hgcpright->GetCP();
								   if (hcp == hcpright)
								   {
									   found = true;
									   break;
								   }
							   }
							   // if it isn't already assigned to right bone, assign it
							   if (!found)
							   {
								   rightsegment->AquireCP(hcp);
							   }
						   }
					   }
               }
				}
			}
		}
		else
		{
			CopyBoneAttributes(leftbone,rightbone);

			if (m_assignvertices)
			{
            if (leftbone->GetObjectType()==HOT_SEGMENT ||
               rightbone->GetObjectType()==HOT_SEGMENT) {
				   HSegmentCache *leftsegment = (HSegmentCache*)leftbone;
				   HSegmentCache *rightsegment = (HSegmentCache*)rightbone;
				   
				   for (HGroupCP *hgcpright = rightsegment->GetGroupCP();hgcpright; hgcpright = hgcpright->GetNext())
				   {
					   Vector findpos = *hgcpright->GetCP()->GetPosition();
		   
					   findpos.x = -findpos.x;

					   // loop through all cps for inverted x position
					   for (int i = 0; i < cps.size(); i++)
					   {
						   HCP *hcp = cps[i];//cp from pool of all vertices
						   
						   Vector pos = *hcp->GetPosition();

						   double distance = DistanceTo(findpos,pos);

						   // if position is within tolerance
						   if (distance<m_tolerance)
						   {
							   // does cp already belong to right bone
							   bool found = false;
							   for (HGroupCP *hgcpleft = leftsegment->GetGroupCP();hgcpleft; hgcpleft = hgcpleft->GetNext())
							   {
								   HCP *hcpleft = hgcpleft->GetCP();
								   if (hcp == hcpleft)
								   {
									   found = true;
									   break;
								   }
							   }
							   // if it isn't already assigned to right bone, assign it
							   if (!found)
							   {
								   leftsegment->AquireCP(hcp);
							   }
						   }
					   }
				   }
            }
			}
		}
	}

	if (m_assignvertices)
	{
      if (m_hprogress)
         m_hprogress->SetMessage1("Assigning Unpaired Bone");

		// Handle vertex assignment for non paired bones
		for (i = 0; i < m_bonesingles.size(); i++) {
         if (m_bonesingles[i]->GetObjectType()!=HOT_SEGMENT)
            continue;
			
         HSegmentCache *segment = (HSegmentCache *)m_bonesingles[i];
			String name = segment->GetMatchName();

         if (m_hprogress) {
            m_hprogress->SetMessage2(name.Get());
            m_hprogress->SetProgress(fast_ftol_unsigned(((float)m_currprogress++/m_totalprogresssize)*100));
         }

			for (HGroupCP *hgcp = segment->GetGroupCP();hgcp; hgcp = hgcp->GetNext())
			{
				Vector findpos = *hgcp->GetCP()->GetPosition();

				if (m_adjustright)
				{
					if (findpos.x>0)
					{
						findpos.x = -findpos.x;

						for (int i = 0; i < cps.size(); i++)
						{
							HCP *hcp = cps[i];//cp from pool of all vertices
							Vector pos = *hcp->GetPosition();
							double distance = DistanceTo(findpos,pos);
							// if position is within tolerance
							if (distance<m_tolerance)
							{
								segment->AquireCP(hcp);
							}
						}
					}
				}
				else
				{
					if (findpos.x<0)
					{
						findpos.x = -findpos.x;
						for (int i = 0; i < cps.size(); i++)
						{
							HCP *hcp = cps[i];//cp from pool of all vertices
							Vector pos = *hcp->GetPosition();
							double distance = DistanceTo(findpos,pos);
							// if position is within tolerance
							if (distance<m_tolerance)
							{
								segment->AquireCP(hcp);
							}
						}
					}
				}
			}		
		}
	}

   if (m_generatelogfile)
      GenerateLogFile();
      
   if (m_hprogress) {
      m_hprogress->Uninitialize();
      m_hprogress = NULL;
   }

	HHashObject *hashobject = (HHashObject*)htreeobject;
	HProject *project = (HProject*)hashobject->GetParentOfType(HOT_PROJECT);
	project->UpdateAllViews(TRUE);

	return TRUE;
}

void CHxtApp::MirrorExpressionTargets(HExpression *dst)
{
   String matchstring, replacestring;
   if (m_adjustright) {
      matchstring = "Left";
      replacestring = "Right";
   }
   else {
      matchstring = "Right";
      replacestring = "Left";
   }

   String string = dst->GetExpressionString();
   string.Replace(matchstring, replacestring);
   int error = dst->SetExpressionString(string);
}

void CHxtApp::MirrorConstraintTargets(HConstraint *src, HConstraint *dst)
{
   HPointerProperty *target1 = dst->GetTarget1();
   HPointerProperty *target2 = dst->GetTarget2();
   HPointerProperty *srctarget1 = src->GetTarget1();
   HPointerProperty *srctarget2 = src->GetTarget2();
   String matchstring, replacestring;
   
   if (m_adjustright) {
      matchstring = "Left";
      replacestring = "Right";
   }
   else {
      matchstring = "Right";
      replacestring = "Left";
   }

   String target;
   if (target1) {
      target = srctarget1->GetDescription();
      target.Replace(matchstring, replacestring);
      target1->SetDescription(target);
   }

   if (target2) {
      target = srctarget2->GetDescription();
      target.Replace(matchstring, replacestring);
      target2->SetDescription(target);
   }
}

BOOL CHxtApp::HandleSpecial(HPropertyDriver *src, HPropertyDriver *dst, HBoneCache *srcbone, HBoneCache *dstbone)
{
   if (dst->IsConstraint()) {
      MirrorConstraintTargets((HConstraint *)src, (HConstraint *)dst);
      // handle the below ourselves now (TODO)
      if (dst->IsStorageDriver())
         dst->CopyKeys(src);
      else
         CopyPropertyDrivers(src, dst, srcbone, dstbone);
      return TRUE; 
   }
   switch (dst->GetObjectType()) {
      case HOT_EXPRESSION:
         MirrorExpressionTargets((HExpression*)dst);
         return TRUE;
   }
   String matchname = dst->GetMatchName();
   //if (matchname == "Translate")
   return FALSE;
}

void CHxtApp::CopyPropertyDrivers(HHashObject *srcparent, HHashObject *dstparent, HBoneCache *srcbone, HBoneCache *dstbone)
{
   HPropertyDriver *src, *dst;
   for (src = (HPropertyDriver *)srcparent->GetChild(); src; src=(HPropertyDriver *)src->GetSibling()) {
#ifdef _DEBUG
String debug = src->GetName();
UINT hot = src->GetObjectType();
#endif
      dst = src->MakeCopy();

      if (dst == NULL)
         continue;

      dst->Copy(src); // OK, this is kind of lame because MakeCopy should have done it
                      // but many derivatives of PropertyDriver do not do it, like all constraints, and emptydriver derivatives.
                      
      dstparent->InsertChildAtTail(dst);
      dst->AddSelfAppliedDrivers(); // This is to hookup Drivers that are self applied, like Drivers of a constraint
      
      if (!HandleSpecial(src, dst, srcbone, dstbone)) {
         if (dst->IsStorageDriver())
            dst->CopyKeys(src);
         else
            CopyPropertyDrivers(src, dst, srcbone, dstbone);
      }
   }
}

int CHxtApp::CopyBoneAttributes(HBoneCache *boneDst,HBoneCache *boneSrc,bool invertX)
{
	if (!boneDst)
		return 0;

	if (!boneSrc)
		return 0;


	HashTools::CopyProperties((HTreeObject*)boneDst,(HTreeObject*)boneSrc);
	
	boneDst->SetBoneColor(boneSrc->GetBoneColor());

	Vector pivotpos,endpos,rollpos;
	pivotpos = boneSrc->GetPivot()->GetValue();
	endpos = boneSrc->GetEnd()->GetValue();
	rollpos = boneSrc->GetRollHandle();

	if (invertX)
	{
		pivotpos.x = -pivotpos.x;
		endpos.x = -endpos.x;
		rollpos.x = -rollpos.x;
	}

	boneDst->GetPivot()->StoreValue(Time(0),pivotpos,FALSE);

	Vector bonedir = (endpos - pivotpos).Normalized();
	Vector rolldir = (rollpos - pivotpos).Normalized();
	Quaternion rotate = VectorsToStandardQ( bonedir, rolldir ).Inverse();

	boneDst->GetRotate()->StoreValue(Time(0), rotate, TRUE);

	boneDst->GetLength()->StoreValue(Time(0),boneSrc->GetLength()->GetValue(),FALSE);


#ifdef _DEBUG
String debugstr = boneSrc->GetMatchName();
#endif
	for (int i = 0; i < m_relationships.size(); i++)
	{
		HTreeObject *relationship = m_relationships[i];
		HTreeObject *bonecontainer = HashTools::FindTreeObject("Bones",relationship);
      if (bonecontainer==NULL)
         continue;
#ifdef _DEBUG
String debugrelation = relationship->GetParent()->GetMatchName();
#endif

		HAnimObjectShortcut *bonerelationshipsrc = (HAnimObjectShortcut *)HashTools::FindTreeObject(boneSrc->GetMatchName().buf, bonecontainer);

		if (!bonerelationshipsrc)
			continue;

      HAnimObjectShortcut *bonerelationshipdst = (HAnimObjectShortcut *)HashTools::FindTreeObject(boneDst->GetMatchName().buf, bonecontainer);

		if (!bonerelationshipdst) {
			String newboneshortcutname;

			bool adjustright = IsLeftSide(bonerelationshipsrc);
			if (adjustright)
				newboneshortcutname = Replace(boneSrc->GetMatchName(),"Left","Right");
			else
				newboneshortcutname = Replace(boneSrc->GetMatchName(),"Right","Left");
			
			HAnimObjectShortcut *newanimobjectshortcut = HAnimObjectShortcut::New(newboneshortcutname.buf,true);

			((HHashObject*)bonecontainer)->InsertChildAtTail((HHashObject*)newanimobjectshortcut);
         
         
         CopyPropertyDrivers(bonerelationshipsrc, newanimobjectshortcut, boneSrc, boneDst);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// DO ORIENTLIKE CONSTRAINTS
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
			HashTreeObjects orientlikecontraints;
			HashTools::FindTreeObject(HOT_ORIENTLIKECONSTRAINT,bonerelationshipsrc,orientlikecontraints);

			for (int i = 0; i < orientlikecontraints.size(); i++)
			{
				HOrientLikeConstraint *orientlikeconstraint = (HOrientLikeConstraint *)orientlikecontraints[i];
				String matchname = orientlikeconstraint->GetMatchName();
				((HTreeObject*)orientlikeconstraint)->Interpolate(Time(0));
				((HTreeObject*)orientlikeconstraint)->Interpolate(Time(100));

				HOrientLikeConstraint *neworientlikeconstraint = HOrientLikeConstraint::New();
				((HHashObject*)newanimobjectshortcut)->InsertChildAtTail((HHashObject*)neworientlikeconstraint);
				HashTools::CopyProperties((HTreeObject*)neworientlikeconstraint,(HTreeObject*)orientlikeconstraint);

				Quaternion oldrotate = orientlikeconstraint->GetRotateOffset()->GetValue(Time(100));

				Vector bonedir = oldrotate * Vector(0,0,1);
				Vector rolldir = oldrotate * Vector(0,1,0);
			
				bonedir.x = -bonedir.x;
				rollpos.x = -rollpos.x;

				Quaternion invertxrotateoffset = VectorsToStandardQ( bonedir, rolldir ).Inverse();
				neworientlikeconstraint->GetRotateOffset()->StoreValue(Time(100),invertxrotateoffset);
			
				if (orientlikeconstraint->GetBone()->GetValue())
				{
					HTreeObject *treeobject = orientlikeconstraint->GetBone()->GetValue();
					if (m_parentmap.HasKey((int)treeobject))
					{
                  BonePair *bonepair;
						ASSERT((BonePair*)m_parentmap.GetValue((int)treeobject, (int &)bonepair));
						String newdescription;
						if (adjustright)
							newdescription = bonepair->right->GetMatchName();
						else
							newdescription = bonepair->left->GetMatchName();

						neworientlikeconstraint->GetBone()->SetDescription(newdescription);
					}
				}
				else
				{
					String description = orientlikeconstraint->GetBone()->GetDescription();
					String newdescription;

					if (adjustright)
						newdescription = Replace(description,"Left","Right");
					else
						newdescription = Replace(description,"Right","Left");

					neworientlikeconstraint->GetBone()->SetDescription(newdescription);
				}				
				((HTreeObject*)orientlikeconstraint)->Interpolate(Time(0));
				((HTreeObject*)orientlikeconstraint)->Interpolate(Time(100));
				((HTreeObject*)neworientlikeconstraint)->Interpolate(Time(0));
				((HTreeObject*)neworientlikeconstraint)->Interpolate(Time(100));
			}
			
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// DO TRANSLATE TO CONSTRAINTS
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			HashTreeObjects translatetocontraints;
			HashTools::FindTreeObject(HOT_TRANSLATETOCONSTRAINT,bonerelationshipsrc,translatetocontraints);

			for (i = 0; i < translatetocontraints.size(); i++)
			{
				HTranslateToConstraint *translatetoconstraint = (HTranslateToConstraint *)translatetocontraints[i];
				String matchname = translatetoconstraint->GetMatchName();
				((HTreeObject*)translatetoconstraint)->Interpolate(Time(100));

				HTranslateToConstraint *newtranslatetoconstraint = HTranslateToConstraint::New();
				((HHashObject*)newanimobjectshortcut)->InsertChildAtTail((HHashObject*)newtranslatetoconstraint);
				HashTools::CopyProperties((HTreeObject*)newtranslatetoconstraint,(HTreeObject*)translatetoconstraint);

				Vector pos = translatetoconstraint->GetTranslateOffset()->GetValue();
				pos.x = -pos.x;
				newtranslatetoconstraint->GetTranslateOffset()->StoreValue(Time(100),pos);

				if (translatetoconstraint->GetBone()->GetValue())
				{
					HTreeObject *treeobject = translatetoconstraint->GetBone()->GetValue();
					if (m_parentmap.HasKey((int)treeobject))
					{
                  BonePair *bonepair;
						ASSERT(m_parentmap.GetValue((int)treeobject, (int &)bonepair));
						String newdescription;
						if (adjustright)
							newdescription = bonepair->right->GetMatchName();
						else
							newdescription = bonepair->left->GetMatchName();

						newtranslatetoconstraint->GetBone()->SetDescription(newdescription);
					}
				}
				else
				{
					String description = translatetoconstraint->GetBone()->GetDescription();
					String newdescription;

					if (adjustright)
						newdescription = Replace(description,"Left","Right");
					else
						newdescription = Replace(description,"Right","Left");

					newtranslatetoconstraint->GetBone()->SetDescription(newdescription);

					((HTreeObject*)newtranslatetoconstraint)->Interpolate(Time(100));
				}
			}	
*/         
		}		
	}
	return 1;
}

int CHxtApp::EvaluateBoneChildren(HTreeObject *htreeobject,HTreeObject *htreeobjectparent)
{

	BonePair bonepair;
	BonePair *bonepairptr = NULL;

//	String debug = htreeobject->GetMatchName();
	
	bonepair.name = GetBoneName(htreeobject);//Name without Left anfd Right
	
	int index = m_bonepairs.size();
	for (int i = 0; i < m_bonepairs.size(); i++)
	{
		if (bonepair.name == m_bonepairs[i].name)
		{
			bonepairptr = &m_bonepairs[i];
			index = i;
			break;
		}
	}
	
	bool flag = false;// flag whether to add it to the pairs map
	if (!bonepairptr)
	{
		bonepairptr = &bonepair;
		flag = true;
	}
	
	if (IsLeftSide(htreeobject))
	{
		bonepairptr->left = htreeobject;
		bonepairptr->leftparent = htreeobjectparent;
		m_parentmap.SetValue((int)htreeobject,index);//map from bone to index in bonepairs list
	}
	else if (IsRightSide(htreeobject))
	{
		bonepairptr->right = htreeobject;
		bonepairptr->rightparent = htreeobjectparent;
		m_parentmap.SetValue((int)htreeobject,index);
	}
	else
	{
		flag = false;
		if (!m_bonessinglemap.HasKey((int)htreeobject))
		{
			m_bonesingles.push_back(htreeobject);
			m_bonessinglemap.SetValue((int)htreeobject,1);
		}
	}

	if (flag)
		m_bonepairs.push_back(bonepair);

	for (HTreeObject *child = htreeobject->GetTreeProgeny();child; child = child->GetTreeSibling())
	{
		if (!EvaluateBoneChildren(child,htreeobject))
			return 0;
	}

	return 1;
}

//Removes Left and Right out of name (Could have used Replace("Left", "") and Replace("Right", ""); (DAN)
String CHxtApp::GetBoneName(HTreeObject *htreeobject)
{
	String newname="";
	String matchname = htreeobject->GetMatchName();
	String uppercasename = matchname.ToUpper();

	for (int i = 0; i < matchname.length(); i++)
	{
		if ((i+4)<=matchname.length())
		{
			if ((uppercasename[i] == 'L')&&(uppercasename[i+1] == 'E')&&(uppercasename[i+2] == 'F')&&(uppercasename[i+3] == 'T'))
				i+=4;
			if ((uppercasename[i] == 'R')&&(uppercasename[i+1] == 'I')&&(uppercasename[i+2] == 'G')&&(uppercasename[i+3] == 'H')&&(uppercasename[i+4] == 'T'))
				i+=5;
		}
		newname+=matchname[i];
	}
	return newname;
}

//Could use String::Replace(replace, replacewith) (DAN)
String CHxtApp::Replace(String fullstring,String replace,String replacewith)
{
	bool flag;
	int i,j;
	String result;
	for (i = 0; i < fullstring.length(); i++)
	{
		j = 0;
		flag = false;
		char a,b;
		a = fullstring[i];
		b = replace[j];
		
		if ((a>='a')&&(a<='z'))
		{
			a = a - 'a';
			a = a + 'A';
		}
		if ((b>='a')&&(b<='z'))
		{
			b = b - 'a';
			b = b + 'A';
		}

		while (a == b)
		{
			a = fullstring[i+j];
			b = replace[j];
			
			if ((a>='a')&&(a<='z'))
			{
				a = a - 'a';
				a = a + 'A';
			}
			if ((b>='a')&&(b<='z'))
			{
				b = b - 'a';
				b = b + 'A';
			}

			if (j == replace.length()-1)
			{
				flag = true;
				i = i+j+1;
				break;
			}
			j++;
		}
		if (flag)
		{
			for (j = 0; j < replacewith.length(); j++)
			{
				result += replacewith[j];
				continue;
			}
		}
		result+=fullstring[i];
	}
	return result;
}

bool CHxtApp::IsLeftSide(HTreeObject *htreeobject)
{
	String matchname = htreeobject->GetMatchName();
	matchname = matchname.ToUpper();//Find is case insensitive (DAN)
	if (matchname.Find("LEFT")==-1)
		return false;
	return true;
}

bool CHxtApp::IsRightSide(HTreeObject *htreeobject)
{
	String matchname = htreeobject->GetMatchName();
	matchname = matchname.ToUpper();//Find is case insensitive (DAN)
	if (matchname.Find("RIGHT")==-1)
		return false;
	return true;
}
void CHxtApp::GenerateLogFile()
{
   String logfilename;
   char path[MAX_PATH];
   if (GetTempPath(MAX_PATH, path)) {
      logfilename = path;
      logfilename = CombineNameToPath(logfilename, "MirrorBones.txt");
   }
   else
      logfilename = ".";

//   logfilename = "C:\\MirrorBones.xml";

   if (!m_fs.Open( logfilename, HashStream::HS_ASCII)) {
      String msg("Error opening [%1].");
      msg.Replace("%1", logfilename);
      MessageRequest(msg);
      return;
   }
   
   if (m_hprogress)
      m_hprogress->SetMessage1("Generating Logfile");

   RebuildRelationshipList();
   
   m_fs << "Start Logfile" << nl;
   
   if (m_hprogress)
      m_hprogress->SetMessage1("Generating Logfile");
   
	for ( int i = 0; i < m_bonepairs.size(); i++) {
		BonePair *bonepair = &m_bonepairs[i];

      if (m_hprogress) {
         m_hprogress->SetMessage2(bonepair->name.Get());
         m_hprogress->SetProgress(fast_ftol_unsigned(((float)m_currprogress++/m_totalprogresssize)*100));
      }
      
      LogRelationships(bonepair);
   }
   m_fs << "End Logfile" << nl;
   m_fs.Close();
   LaunchAssociatedApp(logfilename);
}

void CHxtApp::LogRelationships(BonePair *pair)
{
   String missingrelation("Bone:[%1] missing relation [%2]");
   String missingrelationshipchild("Bone:[%1] missing child [%2] in relationship [%3]");

	for (int i = 0; i < m_relationships.size(); i++)
	{
		HTreeObject *relationship = m_relationships[i];
		HTreeObject *bonecontainer = HashTools::FindTreeObject("Bones",relationship);
      if (bonecontainer==NULL)
         continue;

		HTreeObject *bonerelationshipleft = HashTools::FindTreeObject(pair->left->GetMatchName().buf, bonecontainer);
      HTreeObject *bonerelationshipright = HashTools::FindTreeObject(pair->right->GetMatchName().buf, bonecontainer);
      
      String relationname = relationship->GetParent()->GetMatchName();
      
      if (bonerelationshipleft == NULL && bonerelationshipright) {
         String temp = missingrelation;
         temp.Replace("%1", pair->left->GetMatchName());
         temp.Replace("%2", relationname);
         m_fs << temp << nl;
      }
      else if (bonerelationshipleft && !bonerelationshipright) {
         String temp = missingrelation;
         temp.Replace("%1", pair->right->GetMatchName());
         temp.Replace("%2", relationname);
         m_fs << temp << nl;
      }
      else if(bonerelationshipleft && bonerelationshipright ){ // both sides have this relationship
         HTreeObject *srcbonerelation;
         HTreeObject *dstbonerelation;
         String bonename;
         if (m_adjustright) { //right side is new
            srcbonerelation = bonerelationshipleft;
            dstbonerelation = bonerelationshipright;
            bonename = pair->right->GetMatchName();
         }
         else {
            srcbonerelation = bonerelationshipright;
            dstbonerelation = bonerelationshipleft;
            bonename = pair->left->GetMatchName();
         }
         for (HTreeObject *child = srcbonerelation->GetTreeProgeny(); child; child = child->GetTreeSibling()) {
            String treename = ((HHashObject *)child)->GetName();
            if (m_adjustright)
               treename.Replace("Left", "Right");
            else   
               treename.Replace("Right", "Left");
            if (!dstbonerelation->FindChildByTreeName(treename.Get())) {
               String temp(missingrelationshipchild);
               temp.Replace("%1", bonename);
               temp.Replace("%2", treename);
               temp.Replace("%3", relationname);
               m_fs << temp << nl;
            }
         }
      }
   }
}