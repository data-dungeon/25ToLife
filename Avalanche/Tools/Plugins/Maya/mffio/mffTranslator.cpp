#include "mffTranslator.h"

#include <iostream.h>

#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSet.h>
#include <maya/MDagModifier.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnTransform.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MDGModifier.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFloatPointArray.h>

#include <assert.h>

#define DEGTORAD  (3.14 / 180.0)
#define RADTODEG  (180.0 / 3.14)

//#define MAYA40


const char *const mffOptionScript = "mffTranslatorOptions";
const char *const mffDefaultOptions = 
	"consolshapes = 0;"
	"shadertableperobject = 0;"
	"oneshaderperobject = 0;"
	;

void* mffTranslator::creator()
{
    return new mffTranslator();
}

//////////////////////////////////////////////////////////////

MStatus mffTranslator::reader ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode)
{
	MStatus status;
	MDGModifier dgModifier;

	m_mff = new mffTools;

	//flags
	m_consolshapes = false;
	m_shadertableperobject = false;
	m_oneshaderperobject = false;

	m_count = 0;
	m_oldcount = 0;

//	MObjectArray's
	m_sgsets.clear();
	m_filenodes.clear();
	m_uvchoosernodes.clear();
	m_layers.clear();

//	Hash Tables
	m_shadermap.Clear();
	m_shadinggroupmap.Clear();
	m_filenodemap.Clear();
	m_uvchoosernodemap.Clear();

	m_instmap.Clear();

	m_layernames.clear();

	if (options.length() > 0)
	{
        int i, length;
        // Start parsing.
        MStringArray optionList;
        MStringArray theOption;
        options.split(';', optionList); // break out all the options.

		length = optionList.length();
		for( i = 0; i < length; ++i )
		{
            theOption.clear();
            optionList[i].split( '=', theOption );

			if( theOption[0] == MString("consolshapes") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_consolshapes = true;
				else
					m_consolshapes = false;
			if( theOption[0] == MString("shadertableperobject") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_shadertableperobject = true;
				else
					m_shadertableperobject = false;
			if( theOption[0] == MString("oneshaderperobject") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_oneshaderperobject = true;
				else
					m_oneshaderperobject = false;
		}
	}
	
	MTools::StartProgressBar(100,"Importing ...");
	if (m_mff->ImportFile((char*)file.fullName().asChar()))
	{
		MTools::StepProgressBar(100);
		MTools::StopProgressBar();

		std::vector<int> flags;
		flags.resize(m_mff->GetNumReferences());

//		reference paths
		m_references.clear();
		m_references.resize(m_mff->GetNumReferences());

		m_pShaderTable = NULL;
		m_pMapTable = NULL;

		int i;

		if (!m_shadertableperobject)
		{
	//		get shader table pointer
			for (i = 0; i  < m_mff->GetNumReferences(); i++)
			{
				mffNode*node = m_mff->GetReferenceNode(i);
				if (node->GetNodeType() == kSHADERTABLENODE)
				{
					m_pShaderTable = (mffShaderTableNode*)node;
				}
			}
		}

//		get map table pointer
		for (i = 0; i  < m_mff->GetNumReferences(); i++)
		{
			mffNode*node = m_mff->GetReferenceNode(i);
			if (node->GetNodeType() == kMAPTABLENODE)
			{
				m_pMapTable = (mffMapTableNode*)node;
			}
		}
		
//		get display layer table pointer
		mffDisplayLayerTableNode *pDisplayLayerTable;

		for (i = 0; i  < m_mff->GetNumReferences(); i++)
		{
			mffNode*node = m_mff->GetReferenceNode(i);
			if (node->GetNodeType() == kDISPLAYLAYERTABLENODE)
			{
				pDisplayLayerTable = (mffDisplayLayerTableNode*)node;
			}
		}
		
//		create display layers
		for (i = 0; i < pDisplayLayerTable->m_layernames.size(); i++)
		{
			m_layernames.append(MString(pDisplayLayerTable->m_layernames[i].c_str()));
			MTools::CreateDisplayLayer(MString(pDisplayLayerTable->m_layernames[i].c_str()));
		}

		MTools::ConvertNamesToMObjects(m_layernames,m_layers);


		if (!m_shadertableperobject)
		{
			if (!m_pShaderTable)
			{
				MGlobal::displayError("No shader table.");
				return MS::kFailure;
			}
		}
			
		if (!m_pMapTable)
		{
			MGlobal::displayError("No map table.");
			return MS::kFailure;
		}
		
		if (!m_shadertableperobject)
		{
			MTools::StartProgressBar(m_pShaderTable->size(),"Creating Shading Groups ...");

	//		create and store shaders in m_sgsets
			CreateShadingGroups(m_pShaderTable->m_shaders,m_sgsets);

			MTools::StopProgressBar();
		}

//		loop through instances build tree
		IntMap modelmap;
		
		int total = m_mff->GetNumInstances();

		MTools::StartProgressBar(total,"Building Scene ...");

		for (i = 0; i < m_mff->GetNumInstances(); i++)
		{
			mffNode *node = m_mff->GetInstanceNode(i);

			while (node->m_parentindex>=0)
			{
				node = m_mff->GetInstanceNode(node->m_parentindex);
			}

			if (!modelmap.HasKey((int)node))
			{
				CreateBranch(MObject::kNullObj,node,NULL,flags);
				modelmap.SetValue((int)node,1);
			}
		}
		MTools::StopProgressBar();

		dgModifier.doIt();

		delete m_mff;
	}
	else
	{
		MGlobal::displayError("Failed to load MFF file.");
		return MS::kFailure;
	}

//	stupid Maya bug clear arrays so when user hits new it won't crash
	m_sgsets.clear();
	m_filenodes.clear();
	m_uvchoosernodes.clear();
	m_layers.clear();
	m_attributes.clear();

	m_layernames.clear();
	
	return MS::kSuccess;
}

void mffTranslator::CreateBranch(MObject parentobject,mffNode *node,mffNode *parent,std::vector<int> &flags)
{
	MStatus status;
	MDGModifier dgModifier;
	MDagModifier dagModifier;

//	step progress bar
	int diff = m_count - m_oldcount;
	if (diff>0)
		MTools::StepProgressBar(diff);
	m_oldcount = m_count;

	int numInstances = m_mff->GetNumInstancesOfReference(node);

	if (node->GetNodeType() == kGROUPTRANSFORMINSTANCE)
	{
		mffGroupTransformInstance *extendedtransform = (mffGroupTransformInstance *)node;

		if (extendedtransform->m_childindicies.size()==0)
			return;

		if (extendedtransform->m_childindicies.size()==1)
		{
			mffNode *childnode = m_mff->GetInstanceNode(extendedtransform->m_childindicies[0]);
			if (childnode->GetNodeType() == kMODELINSTANCE)
			{
				mffModelInstance *modelinstance = (mffModelInstance*)childnode;
				if (modelinstance->GetModelType() == kPOLYGONMODELINSTANCE)
				{
					CreateBranch(parentobject,childnode,node,flags);
					return;
				}
				if (modelinstance->GetModelType() == kCURVEMODELINSTANCE)
				{
					CreateBranch(parentobject,childnode,node,flags);
					return;
				}
			}
			else if (childnode->GetNodeType() == kLIGHTINSTANCE)
			{
				CreateBranch(parentobject,childnode,node,flags);
				return;
			}
			else if (childnode->GetNodeType() == kNULLINSTANCE)
			{
				CreateBranch(parentobject,childnode,node,flags);
				return;
			}
		}

//		get node in heirachy of extended shape node (transform immediately above shape)
		bool flag = false;
		mffNode *starttransform;

		for (int i = 0; i < extendedtransform->m_childindicies.size(); i++)
		{
			starttransform = m_mff->GetInstanceNode(extendedtransform->m_childindicies[i]);
			if (flags[starttransform->m_refidx])
			{
				flag = true;
				starttransform = m_mff->GetInstanceNode(starttransform->m_parentindex);
				break;
			}
		}

		MObject transformobject;

//		creates and stores away extended extended transform for later
		if ((!flags[extendedtransform->m_refidx])&&(!flag))
		{
			MFnTransform transform;
			transformobject = transform.create();
			MDagPath transformpath;
			MFnDagNode dagnode(transformobject);
			dagnode.getPath(transformpath);

			mffGroupTransform *grouptransform = m_mff->GetGroupTransform(extendedtransform->m_refidx);
			SetTransform(&grouptransform->m_transform,transformpath);
			
//			sets name of transform node
			dgModifier.renameNode(transformpath.node(),extendedtransform->m_name.c_str());
			dgModifier.doIt();
			
			m_references[extendedtransform->m_refidx] = transformpath;

//			add object attributes
			for (int j = 0; j < extendedtransform->m_attrindicies.size(); j++)
			{
				mffAttribute *pAttr = m_mff->GetAttribute(extendedtransform->m_attrindicies[j]);
				AddAttribute(pAttr,transformpath);
			}

			MFnDependencyNode depnode(transformpath.node());

			MGlobal::executeCommand(MString("dgdirty ") + depnode.name());
			dgModifier.doIt();
			
			flags[extendedtransform->m_refidx] = 1;
			
//			increment instance counter
			m_count++;

		}
//		instance instanced transforms then parent to unique parent transform
		else
		{
			MFnDependencyNode fnNode(m_references[starttransform->m_refidx].node());
			const char *debug = fnNode.name().asChar();

			assert(starttransform->m_refidx < m_references.size());

//			instance object starting at instanced transform
			MDagPath instancepath;
			MTools::InstanceObject(m_references[starttransform->m_refidx],instancepath,true);

			SetNameFromHierarchy(starttransform,instancepath.node());

			mffGroupTransform *transformref = m_mff->GetGroupTransform(extendedtransform->m_refidx);
			SetTransform(&transformref->m_transform,instancepath);
			flags[extendedtransform->m_refidx] = 1;

//			add object attributes
			for (int j = 0; j < extendedtransform->m_attrindicies.size(); j++)
			{
				mffAttribute *pAttr = m_mff->GetAttribute(extendedtransform->m_attrindicies[j]);
				AddAttribute(pAttr,instancepath);
			}

			MFnDependencyNode fnNode2(instancepath.node());
			fnNode2.setName(extendedtransform->m_name.c_str());

			transformobject = instancepath.node();

			if (parentobject != MObject::kNullObj)
			{
				MFnDagNode dagNode(parentobject);
				dagNode.addChild(transformobject);
			}
			else
			{
				dagModifier.reparentNode(transformobject);
				dagModifier.doIt();
			}

			dgModifier.doIt();

//			increment instance counter
			m_count+=2;

			return;
		}

		if (parentobject != MObject::kNullObj)
		{
			MFnDagNode dagNode(parentobject);
			dagNode.addChild(transformobject);
		}
		else
		{
			dagModifier.reparentNode(transformobject);
			dagModifier.doIt();
		}

		dgModifier.doIt();

		int numChildren = node->m_childindicies.size();

		for (int j = 0; j < numChildren; j++)
		{
			int childidx = node->m_childindicies[j];
			mffNode *childnode = m_mff->GetInstanceNode(childidx);
			CreateBranch(transformobject,childnode,node,flags);
		}
	}
	
//	create model instance
	else if (node->GetNodeType() == kMODELINSTANCE)
	{
		mffModelInstance *model = (mffModelInstance *)node;

//		create polygon instance
		if (model->GetModelType() == kPOLYGONMODELINSTANCE)
		{
			MObject finalobject;
			
			mffPolyModelInstance *polymodelinstance = (mffPolyModelInstance *)model;

			if (!polymodelinstance)
				return;

			mffGroupTransformInstance *shapetransform;
			if (parent)
				shapetransform = (mffGroupTransformInstance*)parent;

			if (!flags[polymodelinstance->m_refidx])
			{
				mffPolyModel *polymodel = (mffPolyModel *)m_mff->GetPolyModel(polymodelinstance->m_refidx);

				if (polymodel)
				{
					MDagPath foundpath;					
					if (MTools::GetMDagPath(shapetransform->m_name.c_str(),foundpath))
					{
						assert(shapetransform->m_refidx < m_references.size());

						m_references[shapetransform->m_refidx] = foundpath;
						MDagPath childpath(foundpath);
						m_references[polymodelinstance->m_refidx] = childpath;
						flags[shapetransform->m_refidx] = 1;
					}

					MDagPath transformpath;
					mffNode *grouptransformreference = m_mff->m_references[shapetransform->m_refidx];
/*
					if (grouptransformreference->m_isexternalreference)
					{
						if (!MTools::CreateReference(grouptransformreference->m_filename.c_str(),transformpath,true,true))
						{
							MGlobal::displayError(MString("Failed to create reference. ") + MString(grouptransformreference->m_filename.c_str()));

							status  = CreatePolygonMesh(polymodel,transformpath,polymodelinstance);
							if (!status)
								return;
						}
						MFnDependencyNode depnode(transformpath.node());
						const char *debug = depnode.name().asChar();
					}
					else
					{
*/
					if (m_oneshaderperobject)
						status  = CreatePolygonMesh2(polymodel,transformpath,polymodelinstance);
					else
						status  = CreatePolygonMesh(polymodel,transformpath,polymodelinstance);

					if (!status)
						return;
//					}

//					add to layer
					if (shapetransform->m_layerindex>=0)
					{
						MFnDependencyNode depnode(transformpath.node());
						MTools::AddNodeToDisplayLayer(depnode.name(),m_layernames[shapetransform->m_layerindex]);
					}
	
					finalobject = transformpath.node();

					SetNameFromHierarchy(shapetransform,transformpath.node());

					MDagPath shapepath(transformpath);
					shapepath.push(transformpath.child(0));

					assert(shapetransform->m_refidx < m_references.size());

					m_references[shapetransform->m_refidx] = transformpath;
					flags[shapetransform->m_refidx] = 1;

					assert(polymodelinstance->m_refidx < m_references.size());
					
					m_references[polymodelinstance->m_refidx] = transformpath;//Do this to facilitate instancing below.
//					shapes[polymodelinstance->m_refidx] = shapepath;
					flags[polymodelinstance->m_refidx] = 1;
					
					MFnDependencyNode depnode(transformpath.node());
					const char *debug = depnode.name().asChar();
					
//					Add attributes at shape level
					for (int j = 0; j < polymodelinstance->m_attrindicies.size(); j++)
					{
						mffAttribute *pAttr = m_mff->GetAttribute(polymodelinstance->m_attrindicies[j]);
						AddAttribute(pAttr,shapepath);
					}	

					mffGroupTransform *transformref = m_mff->GetGroupTransform(shapetransform->m_refidx);
					SetTransform(&transformref->m_transform,transformpath);

//					Add attributes at transform level
					for (j = 0; j < shapetransform->m_attrindicies.size(); j++)
					{
						int index = shapetransform->m_attrindicies[j];
						mffAttribute *pAttr = m_mff->GetAttribute(index);
						AddAttribute(pAttr,transformpath);
					}

					MGlobal::executeCommand(MString("dgdirty ") + depnode.name());
					dgModifier.doIt();

//					increment instance counter
					m_count+=2;

					m_instmap.SetValue((int)polymodelinstance,1);
				}
			}
			else
			{
				// I only want the one reference object not all its unique transforms

				assert(polymodelinstance->m_refidx < m_references.size());
				MDagPath dagpath2 = m_references[polymodelinstance->m_refidx];
				MString fullpathname = dagpath2.fullPathName();
				const char *debug = fullpathname.asChar();

				MDagPath instancepath;
				if (m_instmap.HasKey((int)polymodelinstance))
					MTools::InstanceObject(m_references[polymodelinstance->m_refidx],instancepath,true);
				else
					MTools::DuplicateObject(m_references[polymodelinstance->m_refidx],instancepath,true);

				finalobject = instancepath.node();

				MFnDependencyNode depnode(instancepath.node());

				mffGroupTransform *transformref = m_mff->GetGroupTransform(shapetransform->m_refidx);

//				add to layer
				if (transformref->m_layerindex>=0)
				{
					MFnDependencyNode depnode(instancepath.node());
					MTools::AddNodeToDisplayLayer(depnode.name(),m_layernames[transformref->m_layerindex]);
				}

//				Don't set the childrens' names Maya has a cow.
				dgModifier.renameNode(instancepath.node(),transformref->m_name.c_str());
				dgModifier.doIt();

//				SetNameFromHierarchy(shapetransform,instancepath.node());

				SetTransform(&transformref->m_transform,instancepath);

				flags[shapetransform->m_refidx] = 1;

				for (int j = 0; j < shapetransform->m_attrindicies.size(); j++)
				{
					mffAttribute *pAttr = m_mff->GetAttribute(shapetransform->m_attrindicies[j]);
					AddAttribute(pAttr,instancepath);
				}

				MGlobal::executeCommand(MString("dgdirty ") + depnode.name());
				dgModifier.doIt();

				if (parentobject != MObject::kNullObj)
				{
					MFnDagNode dagNode(parentobject);
					dagNode.addChild(finalobject);
				}
				else
				{
					dagModifier.reparentNode(finalobject);
					dagModifier.doIt();
				}
				
				dgModifier.doIt();
				
//				increment instance counter
				m_count++;

				return;
			}

			flags[polymodelinstance->m_refidx] = 1;

			if (parentobject != MObject::kNullObj)
			{
				MFnDagNode dagNode(parentobject);
				dagNode.addChild(finalobject);
			}
			else
			{
				dagModifier.reparentNode(finalobject);
				dagModifier.doIt();
			}

			dgModifier.doIt();
		}
		else if (model->GetModelType() == kCURVEMODELINSTANCE)
		{
			MObject finalobject;
			
			mffCurveModelInstance *curvemodelinstance = (mffCurveModelInstance *)model;

			if (!curvemodelinstance)
				return;

			mffGroupTransformInstance *shapetransform;
			if (parent)
				shapetransform = (mffGroupTransformInstance*)parent;

			if (!flags[curvemodelinstance->m_refidx])
			{
				mffCurveModel *curvemodel = (mffCurveModel *)m_mff->GetReferenceNode(curvemodelinstance->m_refidx);

				if (curvemodel)
				{
					MDagPath transformpath;
					status  = CreateCurve(curvemodel,transformpath,curvemodelinstance);
					if (!status)
						return;

					finalobject = transformpath.node();

					m_references[shapetransform->m_refidx] = transformpath;

					MDagPath shapepath;
					MFnDagNode dagnode(transformpath.child(0));
					dagnode.getPath(shapepath);

					m_references[curvemodelinstance->m_refidx] = shapepath;

					MFnDependencyNode depnode(finalobject);
					const char *debug = depnode.name().asChar();

					SetNameFromHierarchy(shapetransform,transformpath.node());

//					dgModifier.renameNode(finalobject,shapetransform->m_name);
//					dgModifier.doIt();

					mffGroupTransform *transformref = m_mff->GetGroupTransform(shapetransform->m_refidx);
					SetTransform(&transformref->m_transform,transformpath);
					flags[shapetransform->m_refidx] = 1;
					for (int j = 0; j < shapetransform->m_attrindicies.size(); j++)
					{
						mffAttribute *pAttr = m_mff->GetAttribute(shapetransform->m_attrindicies[j]);
						AddAttribute(pAttr,transformpath);
					}

					MGlobal::executeCommand(MString("dgdirty -a ") + depnode.name());
					dgModifier.doIt();
				}

//				increment reference counter
				m_count+=2;
			}
			else
			{
				MFnDependencyNode fnNode(m_references[curvemodelinstance->m_refidx].node());
				const char *name = fnNode.name().asChar();

				assert(curvemodelinstance->m_refidx < m_references.size());

				MDagPath instancepath;
				MTools::InstanceObject(m_references[curvemodelinstance->m_refidx],instancepath,true);
				
				finalobject = instancepath.node();

//				SetNameFromHierarchy(curvemodelinstance,instancepath.node());

				dgModifier.renameNode(instancepath.node(),curvemodelinstance->m_name.c_str());
				dgModifier.doIt();

				mffGroupTransform *transformref = m_mff->GetGroupTransform(shapetransform->m_refidx);

				SetTransform(&transformref->m_transform,instancepath);

				flags[shapetransform->m_refidx] = 1;

				for (int j = 0; j < shapetransform->m_attrindicies.size(); j++)
				{
					mffAttribute *pAttr = m_mff->GetAttribute(shapetransform->m_attrindicies[j]);
					AddAttribute(pAttr,instancepath);
				}

				MFnDependencyNode depnode(instancepath.node());

				MGlobal::executeCommand(MString("dgdirty -a ") + depnode.name());
				dgModifier.doIt();

				if (parentobject != MObject::kNullObj)
				{
					MFnDagNode dagNode(parentobject);
					dagNode.addChild(finalobject);
				}
				else
				{
					dagModifier.reparentNode(finalobject);
					dagModifier.doIt();
				}
				
				dgModifier.doIt();

//				increment instance counter
				m_count++;

				return;
			}

			flags[curvemodelinstance->m_refidx] = 1;

			if (parentobject != MObject::kNullObj)
			{
				MFnDagNode dagNode(parentobject);
				dagNode.addChild(finalobject);
			}
			else
			{
				dagModifier.reparentNode(finalobject);
				dagModifier.doIt();
			}

			dgModifier.doIt();
		}
					
	}
	else if (node->GetNodeType() == kLIGHTINSTANCE)
	{
		mffLightInstance *lightinstance = (mffLightInstance *)node;

		mffLight *pLight = (mffLight*)m_mff->GetReferenceNode(lightinstance->m_refidx);

		MString type;

		if (pLight->m_type == mffLight::omni)
			type = "pointLight";
		else if (pLight->m_type == mffLight::spot)
			type = "spotLight";
		else if (pLight->m_type == mffLight::directional)
			type = "directionalLight";

		MObject lightobject = MTools::CreateLight(type,MString(pLight->m_name.c_str()));

		MDagPath lightpath;
		MFnDagNode dagnode(lightobject);
		MObject lightshape = dagnode.child(0);

		dagnode.getPath(lightpath);
		MFnLight light(lightshape);

		light.setColor(MColor(pLight->m_color.r,pLight->m_color.g,pLight->m_color.b));
		light.setIntensity(pLight->m_intensity);

		mffGroupTransformInstance *groupinstance = m_mff->GetGroupTransformInstance(lightinstance->m_parentindex);
		mffGroupTransform *grouptransform = m_mff->GetGroupTransform(groupinstance->m_refidx);
		SetTransform(&grouptransform->m_transform,lightpath);

//		increment reference counter
		m_count++;	
	}
	else if (node->GetNodeType() == kNULLINSTANCE)
	{
		mffNULLInstance *nullinstance = (mffNULLInstance *)node;

		mffNULL *pNULL = (mffNULL*)m_mff->GetReferenceNode(nullinstance->m_refidx);

		MStringArray result;
		MGlobal::executeCommand("createNode \"locator\"",result);
		dgModifier.doIt();

		MDagPath locatorpath;
		MTools::GetMDagPath(result[0],locatorpath);
		
		mffGroupTransformInstance *groupinstance = m_mff->GetGroupTransformInstance(nullinstance->m_parentindex);
		mffGroupTransform *grouptransform = m_mff->GetGroupTransform(groupinstance->m_refidx);
		SetTransform(&grouptransform->m_transform,locatorpath);

//		add object attributes
		for (int j = 0; j < grouptransform->m_attrindicies.size(); j++)
		{
			mffAttribute *pAttr = m_mff->GetAttribute(grouptransform->m_attrindicies[j]);
			AddAttribute(pAttr,locatorpath);
		}
			
//		increment reference counter
		m_count++;	
	}
}

void mffTranslator::SetNameFromHierarchy(mffNode *node,MObject object)
{
	MStatus status;
	MDGModifier dgmodifier;
	MFnDagNode dagnode(object);

	for (int i = 0; i < dagnode.childCount(); i++)
	{
		if (node->m_childindicies.size()>i)
			SetNameFromHierarchy(m_mff->GetInstanceNode(node->m_childindicies[i]),dagnode.child(i,&status));
	}
	dgmodifier.renameNode(object,node->m_name.c_str());
	dgmodifier.doIt();
	MFnDependencyNode depnode(object);
	MString newname = depnode.name();
	const char *debug = newname.asChar();
}

void mffTranslator::AddAttribute(mffAttribute *attr,MDagPath dagpath)
{
	MDGModifier dgModifier;
	MStatus status;

	const char*debug = attr->GetAttrTypeStr();

	if (attr->m_type == mffAttribute::kFACE)
	{
		if (attr->GetAttrType()== mffAttribute::kDBLINTARRAY)
		{
			if (!dagpath.hasFn(MFn::kMesh))
				return;

			mffDblIntArray *mffattr = (mffDblIntArray *)attr;

			MIntArray compIDs,data;
			compIDs.clear();
			data.clear();

			for (int i = 0; i < mffattr->m_values1.size(); i++)
				compIDs.append(mffattr->m_values1[i]);
			for (i = 0; i < mffattr->m_values2.size(); i++)
				data.append(mffattr->m_values2[i]);
			
			MString test = dagpath.fullPathName();
			const char *debug = test.asChar();

			MFnMesh fnMesh(dagpath,&status);

			MString longname = mffattr->m_name;
			MString shortname = mffattr->m_shortname;

			MStringArray longnames,shortnames,formatnames;
			longnames.clear();
			shortnames.clear();
			formatnames.clear();

			longnames.append(longname);
			shortnames.append(shortname);
			formatnames.append("int");

			if (!fnMesh.isBlindDataTypeUsed(mffattr->m_id))
			{
				status = fnMesh.createBlindDataType(mffattr->m_id,longnames,shortnames,formatnames);
				if (!status)
					MGlobal::displayError("Couldn't create blind data.");
			}
			status = fnMesh.setIntBlindData(compIDs,MFn::kMeshPolygonComponent,mffattr->m_id,longname,data);
			if (!status)
				MGlobal::displayError("Couldn't set blind data.");

			fnMesh.updateSurface();
			fnMesh.syncObject();
		}
	}
	else
	{
		if (attr->GetAttrType() == mffAttribute::kBOOLEAN)
		{
			MFnNumericAttribute mattr;
			mffBoolean *mffattr = (mffBoolean *)attr;
			MObject object = mattr.create(mffattr->m_name,mffattr->m_shortname,MFnNumericData::kBoolean,false,&status);
			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);
			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
		else if (attr->GetAttrType() == mffAttribute::kINTEGER)
		{
			MFnNumericAttribute mattr;
			mffInt *mffattr = (mffInt *)attr;
			MObject object =mattr.create(mffattr->m_name,mffattr->m_shortname,MFnNumericData::kInt,0,&status);
			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);
			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
		else if (attr->GetAttrType() == mffAttribute::kFLOAT)
		{
			MFnNumericAttribute mattr;
			mffFloat *mffattr = (mffFloat *)attr;
			MObject object =mattr.create(mffattr->m_name,mffattr->m_shortname,MFnNumericData::kFloat,0.0,&status);
			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);
			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
		else if (attr->GetAttrType() == mffAttribute::kDOUBLE)
		{
			MFnNumericAttribute mattr;
			mffDouble *mffattr = (mffDouble *)attr;
			MObject object = mattr.create(mffattr->m_name,mffattr->m_shortname,MFnNumericData::kDouble,0.0,&status);
			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);
			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
		else if (attr->GetAttrType()== mffAttribute::kSTRING)
		{
			MFnTypedAttribute mattr;
			mffString *mffattr = (mffString *)attr;
			MObject object = mattr.create(mffattr->m_name,mffattr->m_shortname,MFnData::kString,&status);
			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);
			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
		else if (attr->GetAttrType()== mffAttribute::kENUM)
		{
			MFnEnumAttribute mattr;
			mffEnum *mffattr = (mffEnum *)attr;
			MObject object = mattr.create(mffattr->m_name,mffattr->m_shortname,0,&status);

			for (unsigned j = 0; j < mffattr->m_names.size(); j++)
				mattr.addField(mffattr->m_names[j].c_str(),j);

			dgModifier.addAttribute(dagpath.node(),object,MFnDependencyNode::kLocalDynamicAttr);

			MPlug mplug(dagpath.node(),object);
			mplug.setValue(mffattr->m_value);
			mplug.setKeyable(true);
		}
	}
}

void mffTranslator::SetTransform(mffTransform *transform,MDagPath dagpath)
{
	MStatus status;
	MFnTransform fnTransform(dagpath.node());

	MVector translate;
	translate.x = transform->translate.x;
	translate.y = transform->translate.y;
	translate.z = transform->translate.z;		
	status = fnTransform.setTranslation(translate,MSpace::kTransform);
	
	MVector pivottranslation;
	pivottranslation = MVector(transform->rotatepivottranslation.x,transform->rotatepivottranslation.y,transform->rotatepivottranslation.z);
	status = fnTransform.setRotatePivotTranslation(pivottranslation,MSpace::kTransform);

	MPoint pivot;
	pivot = MPoint(transform->rotatepivot.x,transform->rotatepivot.y,transform->rotatepivot.z);
	status = fnTransform.setRotatePivot(pivot,MSpace::kTransform,true);

	double dRotate[3];
	dRotate[0] = DEGTORAD * transform->rotate.x;
	dRotate[1] = DEGTORAD * transform->rotate.y;
	dRotate[2] = DEGTORAD * transform->rotate.z;

	status = fnTransform.setRotation(dRotate,MTransformationMatrix::kXYZ,MSpace::kTransform);

	pivottranslation = MVector(transform->scalepivottranslation.x,transform->scalepivottranslation.y,transform->scalepivottranslation.z);

	pivot = MPoint(transform->scalepivot.x,transform->scalepivot.y,transform->scalepivot.z);

	status = fnTransform.setScalePivotTranslation(pivottranslation,MSpace::kTransform);

	status = fnTransform.setScalePivot(pivot,MSpace::kTransform,true);

	double dScale[3];
	dScale[0] = transform->scale.x;
	dScale[1] = transform->scale.y;
	dScale[2] = transform->scale.z;

	status = fnTransform.setScale(dScale);
}

MStatus mffTranslator::CreatePolygonMesh(mffPolyModel *polymodel,MDagPath &result,mffPolyModelInstance *polymodelinstance)
{
	MDGModifier dgModifier;

	MFnMesh fnMesh;

	MPointArray vertices;
	MVectorArray normals;
	MIntArray polycounts,polyconnects;
	MFloatArray uarray,varray;
	MIntArray uvIds;
	MIntArray facelist;

	MStatus status;

	MObjectArray SGList;

	mffVectorToMPoint(polymodel->m_VertexArray.m_values,vertices);
	intToMInt(polymodel->m_VertexCounts.m_values,polycounts);
	intToMInt(polymodel->m_PolyVertexIndicies.m_values,polyconnects);
	
//	CREATE POLYGON MESH
	int numVerts = vertices.length();
	int numPolys = polycounts.length();

	MObject polyObject = fnMesh.create(numVerts, numPolys,
		vertices, polycounts, polyconnects, MObject::kNullObj, &status );
	if (!status)
	{
		MGlobal::displayError("Failed to create mesh.");
		return MStatus::kFailure;
	}

	MDagPath dagpath;
	MFnDagNode dagnode(polyObject);
	dagnode.getPath(dagpath);

	result = dagpath;
	
	MFnMesh fnMesh2(dagpath);

//	MULTIMAPPING

	MString curruvsetname;
	fnMesh2.getCurrentUVSetName(curruvsetname);

	for (int i = 0; i < polymodel->m_UVMaps.size(); i++)
	{
		MIntArray uvcounts,uvids;

		mffUVMap *uvmap = &polymodel->m_UVMaps[i];

		MString uvsetname = uvmap->m_name.c_str();

		if (i == 0)
			uvsetname = curruvsetname;
		else
			status = fnMesh2.createUVSet(uvsetname);

		fnMesh2.setCurrentUVSetName(uvsetname);

		const char *debug = uvsetname.asChar();
		
		uarray.clear();
		varray.clear();
		uvids.clear();

//		Build UV Table

		intToMInt(uvmap->m_UVCounts.m_values,uvcounts);

		int sum = 0;
		for (int j = 0; j < polymodel->m_VertexCounts.size(); j++)
			sum += polymodel->m_VertexCounts[j];

//		For some reason Maya wants all the tables to be the same length.
//		So pad arrays with 0's.
		int cnt = 0;
		for (j = 0; j < polymodel->m_VertexCounts.size(); j++)
		{
			for (int k = 0; k < polymodel->m_VertexCounts[j]; k++)
			{
				if (uvmap->m_UVCounts[j]>0)
				{
					int index = uarray.length();
					uarray.append(uvmap->m_UVArray[uvmap->m_PolyUVIndicies[cnt]].x);
					varray.append(uvmap->m_UVArray[uvmap->m_PolyUVIndicies[cnt]].y);
					uvids.append(index);
					cnt++;
				}
			}
		}

		int numuvs = fnMesh2.numUVs(uvsetname);

		fnMesh2.clearUVs(&uvsetname);

		status = fnMesh2.setUVs(uarray, varray ,&uvsetname);
		if (!status)
			MGlobal::displayError("Failed to assign UV table.");
		
		status = fnMesh2.assignUVs( polycounts, uvids ,&uvsetname);
		if (!status)
			MGlobal::displayError("Failed to assign uvs.");
	}

	int sum = 0;
	for (int j = 0; j < polycounts.length(); j++)
		sum += polycounts[j];

#ifdef MAYA40
	facelist.clear();
	int cnt = 0;
	for (int a = 0; a < polycounts.length(); a++)
	{
		for (int b = 0; b < polycounts[a]; b++)
		{
			facelist.append(a);
		}
	}
#else
	facelist.clear();
	facelist.setLength(sum);
	int cnt = 0;
	for (int a = 0; a < polycounts.length(); a++)
	{
		for (int b = 0; b < polycounts[a]; b++)
		{
			facelist[cnt++] = a;
		}
	}
#endif

//	Assign Colors
	if (polymodelinstance->m_vcid>=0)
	{
		mffVertexColorNode *vcnode = (mffVertexColorNode*)m_mff->GetReferenceNode(polymodelinstance->m_vcid);
		
		int numVertexColors = vcnode->m_VertexColors.size();
		if (numVertexColors>0)
		{
			MColorArray colors;
			for (a = 0; a < numVertexColors; a++)
			{
				mffColor *color = &vcnode->m_VertexColors[a];//355-1166
				colors.append(MColor(color->r,color->g,color->b,color->a));
			}
			status = fnMesh2.setFaceVertexColors(colors,facelist,polyconnects);
			if (!status)
				MGlobal::displayError("Failed to assign vertex colors.");
		}
	}


//	Rebuilds tables - then assigns normals
	if (polymodel->m_VertexNormalArray.size()>0)
	{

#ifdef MAYA40
		normals.clear();
		for (i = 0; i < polymodel->m_VertexCounts.size(); i++)
		{
			for (int j = 0; j < polymodel->m_VertexCounts[i]; j++)
			{
				mffVector *mffvect = &polymodel->m_VertexNormalArray[polymodel->m_VertexNormalIndicies[cnt++]];
				normals.append(MVector(mffvect->x,mffvect->y,mffvect->z));
			}
		}	
#else
		cnt = 0;
		normals.setLength(sum);
		for (i = 0; i < polymodel->m_VertexCounts.size(); i++)
		{
			for (int j = 0; j < polymodel->m_VertexCounts[i]; j++)
			{
				mffVector *mffvect = &polymodel->m_VertexNormalArray[polymodel->m_VertexNormalIndicies[cnt]];
				normals[cnt] = MVector(mffvect->x,mffvect->y,mffvect->z);
				cnt++;
			}
		}
#endif
		status = fnMesh2.setFaceVertexNormals(normals, facelist, polyconnects, MSpace::kObject);
		if (!status)
			MGlobal::displayError("Failed to assign normals.");
	}

	// force intitial shading group on object
//	MFnDependencyNode depnode(polyObject);
//	MString objectname = depnode.name();
//	MGlobal::executeCommand(MString("sets -e -forceElement initialShadingGroup " + objectname));
//	dgModifier.doIt();

	int useobjectshaders = 0;
	if (polymodel->m_shaders.size()>0)
		useobjectshaders = 1;


	if (useobjectshaders)
	{
		MObjectArray sgsets;
		// create and connect to local shader table
		CreateShadingGroups(polymodel->m_shaders,sgsets);
		MTools::ConnectShadingGroups(dagpath,sgsets,polymodel->m_ShaderIndicies.m_values);
	}
	else
	{
		// connect to global shader table
		MTools::ConnectShadingGroups(dagpath,m_sgsets,polymodel->m_ShaderIndicies.m_values);
	}

	for (i = 0; i < polymodel->m_ShaderIndicies.size(); i++)
	{

		int shaderindex = polymodel->m_ShaderIndicies[i];

		mffShader *mffshader;

		if (useobjectshaders)
			mffshader = &polymodel->m_shaders[shaderindex];
		else
			mffshader = &(*m_pShaderTable)[shaderindex];
		
		if (mffshader->m_colorMap)
		{
			mffMapObject *mapobj = mffshader->m_colorMap;

			for (int j = 0; j < mapobj->m_mapindicies.size(); j++)
			{
				mffMap *mffmap = &(*m_pMapTable)[mapobj->m_mapindicies[j]];
				HookUpChooser(dagpath,mffmap);
			}
		}
	}

	return MStatus::kSuccess;
}

// this function breaks up meshes into single shader objects
MStatus mffTranslator::CreatePolygonMesh2(mffPolyModel *polymodel,MDagPath &result,mffPolyModelInstance *polymodelinstance)
{
	MDGModifier dgModifier;
	MDagModifier dagModifier;

	MFnMesh fnMesh;

	MPointArray vertices;
	MVectorArray normals;
	MIntArray polycounts,polyconnects;
	MFloatArray uarray,varray;
	MIntArray uvIds;
	MIntArray facelist;

	MStatus status;
	MIntArray uvcounts,uvids;
	MIntArray shaderindicies;
	MColorArray colors;

	MFnTransform transform;
	MObject transformobject = transform.create();
	MDagPath transformpath;
	MFnDagNode tdagnode(transformobject);
	tdagnode.getPath(transformpath);

	// create and connect to local shader table
	MObjectArray sgsets;
	CreateShadingGroups(polymodel->m_shaders,sgsets);

	for (int shaderindex = 0; shaderindex < polymodel->m_shaders.size(); shaderindex++)
	{

		// BUILD TABLES

		vertices.clear();
		polyconnects.clear();
		polycounts.clear();
		uvcounts.clear();
		shaderindicies.clear();
		uarray.clear();
		varray.clear();
		uvids.clear();
		colors.clear();
		normals.clear();
		
		IntMap vmap,vnmap,uvmap;

		int cnt = 0;

		for (int polyindex = 0; polyindex < polymodel->m_VertexCounts.size(); polyindex++)
		{
			int index = polymodel->m_ShaderIndicies[polyindex];
			int numVerts = polymodel->m_VertexCounts[polyindex];
			
			if (index==shaderindex)
			{
				// GET MAPPING
				mffUVMap *mffuvmap = NULL;

				if (polymodel->m_UVMaps.size()>0)
					mffuvmap = &polymodel->m_UVMaps[0];

				// GET VERTEXCOLORS
				mffVertexColorNode *vcnode = NULL;

				if (polymodelinstance->m_vcid>=0)
					vcnode = (mffVertexColorNode*)m_mff->GetReferenceNode(polymodelinstance->m_vcid);
					

				for (int vertexindex = 0; vertexindex < numVerts; vertexindex++)
				{
					int vindex = polymodel->m_PolyVertexIndicies[cnt];
					
					if (!vmap.HasKey(vindex))
					{
						vertices.append(polymodel->m_VertexArray[vindex].x,
							polymodel->m_VertexArray[vindex].y,
							polymodel->m_VertexArray[vindex].z);
						polyconnects.append(vertices.length()-1);
						vmap.SetValue(vindex,vertices.length()-1);
					}
					else
					{
						int value = vmap.GetValue(vindex);
						polyconnects.append(value);
					}

					if (mffuvmap)
					{
						int uvindex = mffuvmap->m_PolyUVIndicies[cnt];

//						if (!uvmap.HasKey(uvindex))
//						{
							uarray.append(mffuvmap->m_UVArray[uvindex].x);
							varray.append(mffuvmap->m_UVArray[uvindex].y);
							uvids.append(uarray.length()-1);
//							uvmap.SetValue(uvindex,uarray.length()-1);
//						}
//						else
//						{
//							int value = uvmap.GetValue(vindex);
//							uvids.append(value);
//						}
					}

					if (vcnode)
					{
						int numVertexColors = vcnode->m_VertexColors.size();
						if (numVertexColors>0)
						{
							mffColor *color = &vcnode->m_VertexColors[cnt];
							colors.append(MColor(color->r,color->g,color->b,color->a));
						}

					}
					
					mffVector *mffvect = &polymodel->m_VertexNormalArray[polymodel->m_VertexNormalIndicies[cnt]];
					normals.append(MVector(mffvect->x,mffvect->y,mffvect->z));

					cnt++;
				}
				polycounts.append(numVerts);
				uvcounts.append(numVerts);
				shaderindicies.append(shaderindex);
			}
			else
				cnt +=numVerts;
		}

		if (polycounts.length()==0)
			continue;

	//	CREATE POLYGON MESH
		int numVerts = vertices.length();
		int numPolys = polycounts.length();

		MObject polyObject = fnMesh.create(numVerts, numPolys,
			vertices, polycounts, polyconnects, MObject::kNullObj, &status );
		if (!status)
		{
			MGlobal::displayError("Failed to create mesh.");
			return MStatus::kFailure;
		}
		MDagPath dagpath;
		MFnDagNode dagnode(polyObject);
		dagnode.getPath(dagpath);

		tdagnode.addChild(polyObject);
		dagModifier.doIt();

		MFnMesh fnMesh2(dagpath);

	//	ASSIGN MAPPING

		MString curruvsetname;
		fnMesh2.getCurrentUVSetName(curruvsetname);

		if (polymodel->m_UVMaps.size()>0)
		{
			MString uvsetname;

			fnMesh2.getCurrentUVSetName(uvsetname);

			int numuvs = fnMesh2.numUVs(uvsetname);

			fnMesh2.clearUVs(&uvsetname);

			status = fnMesh2.setUVs(uarray, varray ,&uvsetname);
			if (!status)
				MGlobal::displayError("Failed to assign UV table.");
			
			status = fnMesh2.assignUVs( uvcounts, uvids ,&uvsetname);
			if (!status)
				MGlobal::displayError("Failed to assign uvs.");
		}
	
		// BUILD FACE LIST
		int sum = 0;
		for (int j = 0; j < polycounts.length(); j++)
			sum += polycounts[j];

		facelist.clear();
		facelist.setLength(sum);
		cnt = 0;
		for (int a = 0; a < polycounts.length(); a++)
		{
			for (int b = 0; b < polycounts[a]; b++)
			{
				facelist[cnt++] = a;
			}
		}

		// ASSIGN VERTEX COLORS
		if (colors.length()>0)
		{
			status = fnMesh2.setFaceVertexColors(colors,facelist,polyconnects);
			if (!status)
				MGlobal::displayError("Failed to assign vertex colors.");
		}

		// ASSIGN VERTEX NORMALS
		status = fnMesh2.setFaceVertexNormals(normals, facelist, polyconnects, MSpace::kObject);
		if (!status)
			MGlobal::displayError("Failed to assign normals.");

		MTools::ConnectShadingGroups(dagpath,sgsets,shaderindicies);
	}

	result = transformpath;


	return MStatus::kSuccess;
}

MStatus mffTranslator::CreateCurve(mffCurveModel *curvemodel,MDagPath &dagpath,mffCurveModelInstance *curvemodelinstance)
{
	MDGModifier dgModifier;
	MStatus status;

	MFnNurbsCurve fnCurve;

	MPointArray cvs;
	MDoubleArray knots;

	mffVectorToMPoint(curvemodel->m_CVArray.m_values,cvs);
	doubleToMDouble(curvemodel->m_KnotParams.m_values,knots);

	MFnNurbsCurve::Form frm = (MFnNurbsCurve::Form)curvemodel->m_form;

	MObject curveObject = fnCurve.create(cvs,knots,curvemodel->m_degree,frm,false,false,MObject::kNullObj,&status);
	if (!status)
		return MS::kFailure;

	dgModifier.doIt();

	MFnDagNode dagnode(curveObject);

	dagnode.getPath(dagpath);

	return MStatus::kSuccess;
}

MStatus mffTranslator::writer ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode )
{
	m_mff = new mffTools;

	m_attributestacks.clear();
	m_instmap.Clear();
	m_attrmap.Clear();;
	m_attributes.clear();

	m_shadertable.clear();
	m_sgsetmap.Clear();
	m_layers.clear();
	m_layermap.Clear();

	//flags
	m_consolshapes = false;
	m_shadertableperobject = false;
	m_oneshaderperobject = false;

//	m_meshtestmap.Clear();
//	m_meshtesttable.clear();
//	m_meshtesthash.Clear();
//	CreateMeshTestTable();

	if (options.length() > 0)
	{
        int i, length;
        // Start parsing.
        MStringArray optionList;
        MStringArray theOption;
        options.split(';', optionList); // break out all the options.

		length = optionList.length();
		for( i = 0; i < length; ++i )
		{
            theOption.clear();
            optionList[i].split( '=', theOption );

			if( theOption[0] == MString("consolshapes") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_consolshapes = true;
				else
					m_consolshapes = false;
			if( theOption[0] == MString("shadertableperobject") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_shadertableperobject = true;
				else
					m_shadertableperobject = false;
			if( theOption[0] == MString("oneshaderperobject") && theOption.length() > 1 )
				if( theOption[1].asInt() > 0 )
					m_oneshaderperobject = true;
				else
					m_oneshaderperobject = false;
		}
	}
 
//	Create Layer Lookup Table
	mffDisplayLayerTableNode displaylayertablenode;
	MStringArray layernames;
	MTools::GetDisplayLayers(layernames);
	MTools::ConvertNamesToMObjects(layernames,m_layers);

	for (int i = 0; i < layernames.length(); i++)
	{
//		int visibility;
//		MString state;
//		MTools::GetDisplayLayerProperties(layernames[i],visibility,state);

		displaylayertablenode.m_layernames.push_back(mffString(layernames[i].asChar()));
		displaylayertablenode.m_layerflags.push_back(0);
		MStringArray nodenames;
		MTools::GetNodesInLayer(layernames[i],nodenames);
		MObjectArray nodes;
		MTools::ConvertNamesToMObjects(nodenames,nodes);
		for (int j = 0; j < nodes.length(); j++)
		{
			MFnDependencyNode depnode(nodes[j]);
			m_layermap.SetValue((int)depnode.f_ptr,i);
		}
	}
	m_mff->AddDisplayLayerTableNode(displaylayertablenode);
	
	if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
        ( mode == MPxFileTranslator::kSaveAccessMode ) )
	{
		ExportAll();
	}
	else if( mode == MPxFileTranslator::kExportActiveAccessMode )
	{
		ExportSelected();
	}

	m_mff->AddShaderTableNode(m_shadertable);
	m_mff->AddMapTableNode(m_maptable);

	int success = m_mff->ExportFile((char*)file.fullName().asChar());
	
	delete m_mff;

//	stupid Maya bug clear arrays so when user hits new it won't crash
	m_attributes.clear();
	m_sgsets.clear();
	m_layers.clear();
	m_filenodes.clear();
	m_uvchoosernodes.clear();

	return MS::kSuccess;
}

bool mffTranslator::haveReadMethod () const
{
    return true;
}

bool mffTranslator::haveWriteMethod () const
{
    return true;
}

MString mffTranslator::defaultExtension () const
{
    return "mff";
}

MPxFileTranslator::MFileKind mffTranslator::identifyFile (
                                        const MFileObject& fileName,
                                        const char* buffer,
                                        short size) const
{
    const char * name = fileName.name().asChar();
    int nameLength = strlen(name);
    
    if ((nameLength > 4) && !strcasecmp(name+nameLength-4, ".mff"))
        return kCouldBeMyFileType;
    else
        return kNotMyFileType;
}

MStatus mffTranslator::ExportSelected()
{
	MStatus status = MS::kSuccess;

	// Create an iterator for the active selection list
	//
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist );

	if (iter.isDone())
	    return MS::kFailure;

    // We will need to interate over a selected node's heirarchy 
    // in the case where shapes are grouped, and the group is selected.
    MItDag dagIterator( MItDag::kDepthFirst, MFn::kInvalid, &status);

	// Selection list loop
	for ( ; !iter.isDone(); iter.next() )
	{	 
		MDagPath objectPath;
		// get the selected node
		status = iter.getDagPath(objectPath);

		// reset iterator's root node to be the selected node.
		status = dagIterator.reset(objectPath.node(), 
									MItDag::kDepthFirst, MFn::kInvalid );
		
		MSelectionList visited;

		// DAG iteration beginning at at selected node
		for ( ; !dagIterator.isDone(); dagIterator.next() )
		{
			MDagPath mdagPath;
			MObject  component = MObject::kNullObj;

			status = dagIterator.getPath(mdagPath);
			if (!status)
				return MS::kFailure;

			std::vector<MDagPath> mdagPathArray;

			// skip over intermediate objects
			//
			MFnDagNode dagNode2( mdagPath, &status );
			if (dagNode2.isIntermediateObject()) 
			{
				continue;
			}

////////////////////////////////////////////////////////////////////////////////////////////////////
//			Only hand off top of branch to ExportBranch.
//			Loop for parent.

			MObject object;
			MObject parentobject = mdagPath.node();

			if (parentobject.hasFn(MFn::kWorld))
				continue;
			do
			{
				MFnDependencyNode fnNode(parentobject);
				const char *debug = fnNode.name().asChar();

				object = parentobject;
				MFnDagNode dagNode(object);
				parentobject = dagNode.parent(0,&status);
			} while (!parentobject.hasFn(MFn::kWorld));

			MFnDagNode dagNode(object);

			if (!visited.hasItem(object))
			{
				if (!ExportBranch(mdagPath,mdagPathArray)) return MS::kFailure;
				visited.add(object);
			}
////////////////////////////////////////////////////////////////////////////////////////////////////		

		}
	}
		
	return status;
}

MStatus mffTranslator::ExportAll()
{
	MStatus status = MS::kSuccess;

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( MS::kSuccess != status)
	{
		MGlobal::displayError("Failure in DAG iterator setup.");
	    return MS::kFailure;
	}

	IntMap visited;

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath mdagPath;
		MObject  component = MObject::kNullObj;

		status = dagIterator.getPath(mdagPath);
		if (!status)
		{
			MGlobal::displayError("Failure getting DAG path.");
			return MS::kFailure;
		}

		std::vector<MDagPath> mdagPathArray;

		// skip over intermediate objects
		//
		MFnDagNode dagNode2( mdagPath, &status );
		if (dagNode2.isIntermediateObject()) 
		{
			continue;
		}	

////////////////////////////////////////////////////////////////////////////////////////////////////
//		Only hand off top of branch to ExportBranch.
//		Loop for parent.

		MObject object;
		MObject parentobject = mdagPath.node();

		if (parentobject.hasFn(MFn::kWorld))
			continue;
		do
		{
			MFnDependencyNode fnNode(parentobject);
			const char *debug = fnNode.name().asChar();

			object = parentobject;
			MFnDagNode dagNode(object);
			parentobject = dagNode.parent(0,&status);
		} while (!parentobject.hasFn(MFn::kWorld));

		MFnDependencyNode fnNode(object);
		const char *debug = fnNode.name().asChar();

		if (!visited.HasKey((int)fnNode.f_ptr))
		{
			if (fnNode.name()!="References")
			{
				if (!ExportBranch(mdagPath,mdagPathArray)) return MS::kFailure;
				visited.SetValue((int)fnNode.f_ptr,1);
			}
		}
////////////////////////////////////////////////////////////////////////////////////////////////////		
	}
	return status;
}

int mffTranslator::ExportDagPaths(std::vector<MDagPath> &mdagPathArray)
{
	MStatus status;
	MObject parentobj = MObject::kNullObj;
	int parentidx = -1;
	int refidx = -1;
	int instidx = -1;
	int layerindex = -1;
	
	for (int i = 0; i < mdagPathArray.size(); i++)
	{
		MFnDependencyNode fnNode(mdagPathArray[i].node());
		const char *objectname = fnNode.name().asChar();

		MDagPath *pDagPath = &mdagPathArray[i];
		MObject	object = pDagPath->node();

		int index = -1;
		if (m_instmap.HasKey((int)fnNode.f_ptr))
		{
			index = m_instmap.GetValue((int)fnNode.f_ptr);
		}

		layerindex = -1;
		if (m_layermap.HasKey((int)fnNode.f_ptr))
		{
			layerindex = m_layermap.GetValue((int)fnNode.f_ptr);
		}

		int isinstance = MTools::IsInstance(*pDagPath);

		bool exportvcs = false;
/*
		if (index < 0)
		{
			if ((pDagPath->hasFn(MFn::kMesh))&&(!(pDagPath->hasFn(MFn::kTransform))))
			{
//				if ((!isinstance)&&(m_consolshapes))
				if (!isinstance)
				{
					if (m_meshtestmap.HasKey((int)fnNode.f_ptr))
					{
						int meshtestindex = m_meshtestmap.GetValue((int)fnNode.f_ptr);
						MeshTest &meshtest = m_meshtesttable[meshtestindex];
						if (m_meshtesthash.HasKey((int)meshtest.hashValue))
						{
							int depnodeptr = m_meshtesthash.GetValue((int)meshtest.hashValue);

							int meshtestindex2 = m_meshtestmap.GetValue(depnodeptr);

							MeshTest &meshtest2 = m_meshtesttable[meshtestindex2];
							if (meshtest == meshtest2)
							{
								if (m_instmap.HasKey(depnodeptr))
								{
									index = m_instmap.GetValue(depnodeptr);
									mffNode *debug = m_mff->m_instances[index];
								}
							}
						}
						exportvcs = true;
					}
				}
			}
		}
*/
		if (index >=0)
		{
			int oldparentidx = parentidx;

			if (exportvcs)
			{
				if (pDagPath->hasFn(MFn::kMesh))
				{
					mffPolyModelInstance *polyinstance = (mffPolyModelInstance*)m_mff->GetInstanceNode(index);

					parentidx = ExportPolygonMeshInstance(parentidx,polyinstance->m_refidx,layerindex,*pDagPath,NULL);

					mffPolyModelInstance *polyinstance0 = (mffPolyModelInstance*)m_mff->GetInstanceNode(parentidx);

					polyinstance0->m_vcid = ExportPolygonVertexColors(*pDagPath);
				}
				else if (pDagPath->hasFn(MFn::kNurbsCurve))
				{
					mffCurveModelInstance *curveinstance = (mffCurveModelInstance*)m_mff->GetInstanceNode(index);

					parentidx = ExportCurveInstance(parentidx,curveinstance->m_refidx,*pDagPath);
				}
			}
			else
			{
				parentidx = index;
			}

			if (oldparentidx >= 0)
			{
				mffNode *parent = m_mff->GetInstanceNode(oldparentidx);

				bool flag = false;

				for (int j = 0; j < parent->m_childindicies.size(); j++)
				{
					if (parent->m_childindicies[j] == parentidx)
						flag = true;
				}

				if (!flag)
					parent->m_childindicies.push_back(parentidx);
			}
		}
		else
		{
			if ((pDagPath->hasFn(MFn::kTransform))&&(!pDagPath->hasFn(MFn::kJoint)))
			{
				mffIntArray attrindicies;
				AddAttributesForExport(object,attrindicies);

				parentidx = ExportGroupTransformInstance(parentidx,layerindex,*pDagPath,&attrindicies);

				m_attributestacks.push_back(attrindicies);
			}
			else if (pDagPath->hasFn(MFn::kMesh))
			{
				MFnDependencyNode depnode(pDagPath->node());
				const char *debug = depnode.name().asChar();

				int vcid;
				refidx = ExportPolygonMesh(*pDagPath,vcid,status);
				if (refidx<0)
					return 0;

				mffIntArray attrindicies;
				AddAttributesForExport(object,attrindicies);
				m_attributestacks.push_back(attrindicies);

				int oldparentidx = parentidx;
				parentidx = ExportPolygonMeshInstance(parentidx,refidx,layerindex,*pDagPath,&attrindicies);
				if (oldparentidx >=0)
				{
					mffNode *parent = m_mff->GetInstanceNode(oldparentidx);
					parent->m_childindicies.push_back(parentidx);
				}
				mffPolyModelInstance *modelinstance = (mffPolyModelInstance*)m_mff->GetInstanceNode(parentidx);
				modelinstance->m_vcid = vcid;
			}
			else if (pDagPath->hasFn(MFn::kLight))
			{
				parentidx = ExportLightInstance(parentidx,*pDagPath);
			}
			else if (pDagPath->hasFn(MFn::kNurbsCurve))
			{
				int vcid;
				refidx = ExportCurve(*pDagPath,MObject::kNullObj,vcid,status);
				mffIntArray empty;
				m_attributestacks.push_back(empty);
				int oldparentidx = parentidx;
				parentidx = ExportCurveInstance(parentidx,refidx,*pDagPath);
				if (oldparentidx >=0)
				{
					mffNode *parent = m_mff->GetInstanceNode(oldparentidx);
					parent->m_childindicies.push_back(parentidx);
				}
				mffCurveModelInstance *modelinstance = (mffCurveModelInstance*)m_mff->GetInstanceNode(parentidx);
				modelinstance->m_vcid = vcid;
			}
			else if (pDagPath->hasFn(MFn::kLocator))
			{
				mffIntArray attrindicies;
				AddAttributesForExport(object,attrindicies);
				m_attributestacks.push_back(attrindicies);
				parentidx = ExportLocatorInstance(parentidx,*pDagPath,&attrindicies);
			}
			else if (pDagPath->hasFn(MFn::kJoint))
			{
				parentidx = ExportJointInstance(parentidx,*pDagPath);
			}
			
			MFnDependencyNode depnode(object);
			m_instmap.SetValue((int)depnode.f_ptr,m_mff->m_instances.size()-1);
		}
	}
	return 1;
}

int mffTranslator::ExportBranch(MDagPath mdagPath,std::vector<MDagPath> mdagPathArray)
{
	MFnDependencyNode fnNode(mdagPath.node());
	const char *objectname = fnNode.name().asChar();
	MStatus status;

	// skip over intermediate objects
	//
	MFnDagNode dagNode(mdagPath,&status);
	if (!dagNode.isIntermediateObject()) 
	{
		if (mdagPath.hasFn(MFn::kWorld))
		{
			return 1;
		}
		else if (mdagPath.hasFn(MFn::kMesh))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else if (mdagPath.hasFn(MFn::kNurbsCurve))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else if (mdagPath.hasFn(MFn::kTransform))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else if (mdagPath.hasFn(MFn::kLight))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else if (mdagPath.hasFn(MFn::kLocator))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else if (mdagPath.hasFn(MFn::kJoint))
		{
			mdagPathArray.push_back(mdagPath);
		}
		else
		{
			return 1;
		}
		
		int numChildren = mdagPath.childCount();
		if (numChildren==0)
		{
			if (mdagPath.hasFn(MFn::kMesh))
			{
				if (!ExportDagPaths(mdagPathArray)) return 0;
			}
			else if (mdagPath.hasFn(MFn::kNurbsCurve))
			{
				if (!ExportDagPaths(mdagPathArray)) return 0;
			}
			else if (mdagPath.hasFn((MFn::kLight)))
			{
				if (!ExportDagPaths(mdagPathArray)) return 0;
			}
			else if (mdagPath.hasFn((MFn::kLocator)))
			{
				if (!ExportDagPaths(mdagPathArray)) return 0;
			}
			else if (mdagPath.hasFn((MFn::kJoint)))
			{
				if (!ExportDagPaths(mdagPathArray)) return 0;
			}

			return 1;
		}
		for (int i = 0; i < numChildren; i++)
		{
			MFnDagNode dagNode2(dagNode.child(i));
			MDagPath mdagPath2;
			dagNode2.getPath(mdagPath2);
			
			if (!ExportBranch(mdagPath2,mdagPathArray)) return 0;
		}
	}
	return 1;
}

void mffTranslator::AddAttributesForExport(MObject object,mffIntArray &attrindicies)
{
	MStatus status;

	MFnDependencyNode fnNode(object);

	attrindicies.clear();

	const char *debug = fnNode.name().asChar();

	MStringArray result;
	MGlobal::executeCommand(MString("listAttr -ud ")+fnNode.name(),result);
	int numAttributes = result.length();

//	int numAttributes = fnNode.attributeCount();
	
//	Adds user defined object attributes
	for (int i = 0; i < numAttributes; i++)
	{	
		
		MPlug plug = fnNode.findPlug(result[i],&status);
		if (!status) continue;

		MObject attr = plug.attribute(&status);
		if (!status) continue;
/*
		MObject attr = fnNode.attribute(i);

		MFnAttribute attrib(attr);

//		Gets user defined object attributes from object
		if (!attrib.isDynamic())
			continue;
*/		
		MFnAttribute fnAttr(attr);

		MString longname,shortname;

		longname = fnAttr.name();


#ifndef MAYA40
		shortname = fnAttr.shortName();
#else
		shortname = fnAttr.name();
#endif

		const char *attrname = fnAttr.name().asChar();

		int aidx = -1;
		aidx = MTools::FindMObject(attr,m_attributes);

		if (aidx<0)
		{
			MFn::Type attrtype = attr.apiType();

			int attridx = -1;
			
			if (attrtype == MFn::kNumericAttribute)
			{
				MFnNumericAttribute mattr(attr);

				MFnNumericData::Type numtype = mattr.unitType();

				if (numtype == MFnNumericData::kBoolean)
				{
					mffBoolean mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());
					MPlug mplug(object,attr);
					bool temp;
					mplug.getValue(temp);
					mffattr.m_value = temp;
					attridx = m_mff->AddBoolAttribute(mffattr);
				}
				else if ((numtype == MFnNumericData::kInt)||
					(numtype == MFnNumericData::kShort)||
					(numtype == MFnNumericData::kLong))
				{
					mffInt mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());
					MPlug mplug(object,attr);
					mplug.getValue(mffattr.m_value);
					attridx = m_mff->AddIntAttribute(mffattr);
				}
				else if (numtype == MFnNumericData::kByte)
				{
					mffInt mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());
					MPlug mplug(object,attr);
					char byte;
					mplug.getValue(byte);
					mffattr.m_value = byte;
					attridx = m_mff->AddIntAttribute(mffattr);

				}
				else if (numtype == MFnNumericData::kFloat)
				{
					mffFloat mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());
					MPlug mplug(object,attr);
					mplug.getValue(mffattr.m_value);
					attridx = m_mff->AddFloatAttribute(mffattr);
				}
				else if (numtype == MFnNumericData::kDouble)
				{
					mffDouble mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());
					MPlug mplug(object,attr);
					mplug.getValue(mffattr.m_value);
					attridx = m_mff->AddDoubleAttribute(mffattr);
				}
			}
			else if (attrtype == MFn::kTypedAttribute)
			{
				MFnTypedAttribute mattr(attr);

				if (mattr.accepts(MFnData::kString))
				{
					mffString mffattr;
					mffattr.m_type = mffAttribute::kOBJECT;
					strcpy(mffattr.m_name,longname.asChar());
					strcpy(mffattr.m_shortname,shortname.asChar());

					MPlug mplug(object,attr);

					MString field;
					mplug.getValue(field);
								
					strcpy(mffattr.m_value,field.asChar());

					attridx = m_mff->AddStringAttribute(mffattr);
				}
			}
			else if (attrtype == MFn::kEnumAttribute)
			{
				MFnEnumAttribute mattr(attr);

				mffEnum mffattr;
				mffattr.m_type = mffAttribute::kOBJECT;
				strcpy(mffattr.m_name,longname.asChar());
				strcpy(mffattr.m_shortname,shortname.asChar());

				MPlug mplug(object,attr);
				
				short minimum,maximum;
				mattr.getMin(minimum);
				mattr.getMax(maximum);

				int count = maximum-minimum+1;
				for(unsigned i = 0; i < count; i++)
				{
					MString fieldname;
					fieldname = mattr.fieldName(i);
					mffattr.m_names.push_back(fieldname.asChar());
					const char*debug = fieldname.asChar();
				}
				int value;
				mplug.getValue(value);
				mffattr.m_value = value;
				attridx = m_mff->AddEnumAttribute(mffattr);
			}
			else
			{
				continue;
			}
	
			if (attridx>=0)
			{
				attrindicies.push_back(attridx);
				m_attributes.append(attr);
			}

		}
		else
		{
			attrindicies.push_back(aidx);
		}
	}
//	Adds face attributes
	if (!object.hasFn(MFn::kMesh))
		return;

	MFnMesh fnMesh(object);

	MIntArray compIDs,data;

	MIntArray typearray;
	fnMesh.getBlindDataTypes(MFn::kMeshPolygonComponent,typearray);
	for (i = 0; i < typearray.length(); i++)
	{
		MStringArray longnames,shortnames,typenames;
		fnMesh.getBlindDataAttrNames(typearray[i],longnames,shortnames);
	
		for (int j = 0; j < longnames.length(); j++)
		{
			fnMesh.getIntBlindData(MFn::kMeshPolygonComponent,typearray[i],longnames[j],compIDs,data);

			mffDblIntArray mffdblintarray;
			mffdblintarray.m_id = typearray[i];
			strcpy(mffdblintarray.m_name,longnames[j].asChar());
			strcpy(mffdblintarray.m_shortname,shortnames[j].asChar());
			
			mffdblintarray.m_type = mffAttribute::kFACE;

			for (int k = 0; k < compIDs.length(); k++)
			{
				mffdblintarray.m_values1.push_back(compIDs[k]);
				mffdblintarray.m_values2.push_back(data[k]);
			}

			int attridx = m_mff->AddDblIntArrayAttribute(mffdblintarray);
			attrindicies.push_back(attridx);
		}
	}
}

int mffTranslator::ExportPolygonVertexColors(MDagPath dagPath)
{
	MStatus status;
	mffVertexColorNode vcnode;

	MFnMesh fnMesh(dagPath);

	MColorArray colorarray;
	status = fnMesh.getFaceVertexColors(colorarray);
	if (!status)
		return -1;

	MFnDependencyNode depnode(dagPath.node());

	int badcolor = 0;
	for (int i = 0; i < colorarray.length(); i++)
	{
		if ((colorarray[i].r < 0.0)||(colorarray[i].g < 0.0)||(colorarray[i].b))
		{
//			MGlobal::displayError(MString("Empty vertex color on object ") + depnode.name() + MString("."));
			break;
		}
	}

	vcnode.m_VertexColors.resize(colorarray.length());
	for (i = 0; i < colorarray.length(); i++)
	{
		float r = 1.0f,g = 0.0f,b = 1.0f,a = 1.0f;
		if (colorarray[i].r >=0)
			r = colorarray[i].r;
		if (colorarray[i].g >=0)
			g = colorarray[i].g;
		if (colorarray[i].b >=0)
			b = colorarray[i].b;
		if (colorarray[i].a >=0)
			a = colorarray[i].a;
		vcnode.m_VertexColors[i] = mffColor(r,g,b,a);
	}

	return m_mff->AddVertexColorNode(vcnode);
}

int mffTranslator::ExportPolygonMesh(MDagPath mdagPath,int &vcid,MStatus &status)
{
	mffPolyModel polymodel;
		
	MFnDependencyNode meshdepnode(mdagPath.node());

	polymodel.m_name = meshdepnode.name().asChar();

	MFnMesh fnMesh(mdagPath, &status);
	if (!status)
		return -1;

	const char *debug = meshdepnode.name().asChar();
	
	MIntArray keys,keys0;
	MObjectArray SGSets;
	SGSets.clear();
	keys.clear();
	keys0.clear();

	fnMesh.getConnectedShaders(0,SGSets,keys);
	

	if (m_shadertableperobject)
		CreateShaderTable(SGSets,polymodel.m_shaders);
	else
		CreateShaderTable(SGSets,m_shadertable.m_shaders);


	int ShadingGroupTableLength = SGSets.length();

	if (ShadingGroupTableLength == 0)
	{
		MGlobal::displayError(MString("Shading group table size is 0. ") + meshdepnode.name());
		return -1;
	}

	int i;

	if (!m_shadertableperobject)
	{

#ifdef MAYA40
	for (i = 0; i < keys.length(); i++)
	{
		MFnDependencyNode depnode(SGSets[keys[i]]);
		const char *debug = depnode.name().asChar();
		if (m_sgsetmap.HasKey((int)depnode.f_ptr))
		{
			int index = m_sgsetmap.GetValue((int)depnode.f_ptr);
			keys0.append(index);
		}
		else
		{
			keys0.append(-1);
		}
	}	

#else
	int index;

	keys0.setLength(keys.length());

	for (i = 0; i < keys.length(); i++)
	{
		index = keys[i];
		if (index <0)
		{
			MGlobal::displayError(MString("Found face with no shading group assigned.")+ meshdepnode.name());
			return -1;
		}

		MFnDependencyNode depnode(SGSets[keys[i]]);
		const char *debug = depnode.name().asChar();

		if (m_sgsetmap.HasKey((int)depnode.f_ptr))
		{
			int index = m_sgsetmap.GetValue((int)depnode.f_ptr);
			keys0[i] = index;
		}
		else
		{
			keys0[i] = -1;
		}
	}
#endif

	}


	// Vertex Table
	MFloatPointArray vertices;
	status = fnMesh.getPoints(vertices,MSpace::kObject);
	int numvertices = vertices.length();
	polymodel.m_VertexArray.resize(vertices.length());
	for (i = 0; i < vertices.length(); i++)
		polymodel.m_VertexArray[i] = mffVector(vertices[i].x,vertices[i].y,vertices[i].z);
	if (!status)
		return -1;

//	Multiple UV Sets
	int NumUVSets = fnMesh.numUVSets();

	MStringArray uvsetnames;
	fnMesh.getUVSetNames(uvsetnames);

	for (i = 0; i < NumUVSets; i++)
	{
		MFloatArray uArray,vArray;
		mffUVMap uvmap;

//		Name
		uvmap.m_name = uvsetnames[i].asChar();

//		Get UV Table
		fnMesh.getUVs(uArray,vArray,&uvsetnames[i]);
		uvmap.m_UVArray.resize(uArray.length());
		for (int j = 0; j < uArray.length(); j++)
			uvmap.m_UVArray[j] = mffVector2(uArray[j],vArray[j]);
		
		MIntArray uvcounts,uvids;

		fnMesh.getAssignedUVs(uvcounts,uvids,&uvsetnames[i]);

		uvmap.m_UVCounts.resize(uvcounts.length());
		uvmap.m_PolyUVIndicies.resize(uvids.length());

		for (j = 0; j < uvcounts.length(); j++)
			uvmap.m_UVCounts[j] = uvcounts[j];
		
		for (j = 0; j < uvids.length(); j++)
			uvmap.m_PolyUVIndicies[j] = uvids[j];

		polymodel.m_UVMaps.push_back(uvmap);
	}

    // Normal Table

	MFloatVectorArray norms;
	fnMesh.getNormals( norms, MSpace::kObject );
	polymodel.m_VertexNormalArray.resize(norms.length());
	for (i = 0; i < norms.length(); i++)
	{
		MFloatVector tmpf = norms[i];
		polymodel.m_VertexNormalArray[i] = mffVector(tmpf[0],tmpf[1],tmpf[2]);
    }
	
	mffVertexColorNode vcnode;
	MColorArray colorarray;
	vcid = -1;
	status = fnMesh.getFaceVertexColors(colorarray);
	if (status)
	{
		if (colorarray.length()>0)
		{

			MFnDependencyNode depnode(mdagPath.node());

			int badcolor = 0;
			for (int i = 0; i < colorarray.length(); i++)
			{
				if ((colorarray[i].r < 0.0)||(colorarray[i].g < 0.0)||(colorarray[i].b))
				{
//					MGlobal::displayWarning(MString("Empty vertex color on object ") + depnode.name() + MString("."));
					break;
				}
			}

			vcnode.m_VertexColors.resize(colorarray.length());
			for (i = 0; i < colorarray.length(); i++)
			{
				float r = 1.0f,g = 0.0f,b = 1.0f,a = 1.0f;
				if (colorarray[i].r >=0)
					r = colorarray[i].r;
				if (colorarray[i].g >=0)
					g = colorarray[i].g;
				if (colorarray[i].b >=0)
					b = colorarray[i].b;
				if (colorarray[i].a >=0)
					a = colorarray[i].a;
				vcnode.m_VertexColors[i] = mffColor(r,g,b,a);
			}
			vcid = m_mff->AddVertexColorNode(vcnode);
		}
	}
	
	// Face level iteration for poly vertex indicies
	MItMeshPolygon polyIter(mdagPath, MObject::kNullObj, &status);
	if (!status)
		return -1;

	int shaderindex,polyindex;

	for ( ; !polyIter.isDone(); polyIter.next() )
	{
		int polyVertexCount = polyIter.polygonVertexCount();
		polymodel.m_VertexCounts.push_back(polyVertexCount);

		MIntArray vertexIndicies;
		status = polyIter.getVertices(vertexIndicies);
		for (i = 0; i < polyVertexCount; i++ )
		{
			polymodel.m_PolyVertexIndicies.push_back(vertexIndicies[i]);
 			polymodel.m_VertexNormalIndicies.push_back(polyIter.normalIndex(i));
		}

		polyindex = polyIter.index();

	if (m_shadertableperobject)
		shaderindex = keys[polyindex];
	else
		shaderindex = keys0[polyindex];

		polymodel.m_ShaderIndicies.push_back(shaderindex);
	}
	
	return m_mff->AddPolyModel(polymodel);
}

int mffTranslator::ExportCurve(MDagPath mdagPath,MObject mComponent,int &vcid,MStatus &status)
{
	mffCurveModel curvemodel;
		
	MFnDependencyNode fnNode(mdagPath.node());

	curvemodel.m_name = fnNode.name().asChar();

	MFnNurbsCurve fnCurve(mdagPath,&status);
	if (!status)
		return -1;

	curvemodel.m_degree = fnCurve.degree();
	curvemodel.m_spans = fnCurve.numSpans();

	unsigned numKnots = fnCurve.numKnots();
	unsigned numCVs = fnCurve.numCVs();

	curvemodel.m_CVCounts.push_back(numCVs);
	curvemodel.m_KnotCounts.push_back(numKnots);

	double param;
	MPoint point;
	for (unsigned i = 0; i < numKnots; i++)
	{
		param = fnCurve.knot(i,&status);
		if (!status)
			continue;
		fnCurve.getPointAtParam(param,point,MSpace::kObject);
		curvemodel.m_KnotParams.push_back(param);
		curvemodel.m_KnotArray.push_back(mffVector(point.x,point.y,point.z));
		curvemodel.m_KnotConnects.push_back(curvemodel.m_KnotArray.size()-1);
	}
	
	for (i = 0; i < numCVs; i++)
	{
		status = fnCurve.getCV(i,point,MSpace::kObject);
		curvemodel.m_CVArray.push_back(mffVector(point.x,point.y,point.z));
		curvemodel.m_CVConnects.push_back(curvemodel.m_CVArray.size()-1);	
	}

//	MFnNurbsCurve::Form frm = fnCurve.form();
//	if (frm == MFnNurbsCurve::kInvalid)
//		curvemodel.m_type = 0;
//	else if (frm == MFnNurbsCurve::kInvalid)
//		curvemodel.m_type = 0;
//	else
		curvemodel.m_form = fnCurve.form();;

	vcid = -1;
	return m_mff->AddCurveModel(curvemodel);
}

int mffTranslator::ExportPolygonMeshInstance(int parentidx,int refidx,int layerindex,MDagPath &mdagPath,mffIntArray *attrindicies)
{
	MStatus status;
	mffPolyModelInstance polymodelinstance;

	MFnDependencyNode fnNode(mdagPath.node());

	polymodelinstance.m_name = fnNode.name().asChar();

	polymodelinstance.m_parentindex = parentidx;

	polymodelinstance.m_refidx = refidx;

	polymodelinstance.m_layerindex = layerindex;

	MFnDependencyNode fnNode2(mdagPath.child(0));

	mffIntArray temp;
	if (attrindicies)
		temp = *attrindicies;

	return m_mff->AddPolyModelInstance(polymodelinstance,temp);
}

int mffTranslator::ExportCurveInstance(int parentidx,int refidx,MDagPath &mdagPath)
{
	MStatus status;
	mffCurveModelInstance curvemodelinstance;

	MFnDependencyNode fnNode(mdagPath.node());

	curvemodelinstance.m_name = fnNode.name().asChar();

	curvemodelinstance.m_parentindex = parentidx;

	curvemodelinstance.m_refidx = refidx;

	MFnDependencyNode fnNode2(mdagPath.child(0));

	return m_mff->AddCurveModelInstance(curvemodelinstance);
}

int mffTranslator::ExportGroupTransformInstance(int parentidx,int layerindex,MDagPath &mdagPath,mffIntArray *attrindicies)
{
	MStatus status;
	mffGroupTransform group;

	MFnDependencyNode fnNode(mdagPath.node());

	const char *debug = fnNode.name().asChar();

	group.m_name = fnNode.name().asChar();

	if (MTools::IsReference(mdagPath),false)
	{
		group.m_isexternalreference = 1;
		MString reffilename = MTools::GetReferenceFileName(mdagPath);
		group.m_filename = reffilename.asChar();
	}
	else
	{
		MDagPath childpath(mdagPath);

		if ((childpath.childCount()>0)&&(MFn::kMesh)&&(MFn::kTransform))
		{
			childpath.push(mdagPath.child(0));

			if (MTools::IsReference((childpath),false))
			{
				group.m_isexternalreference = 1;
				MString reffilename = MTools::GetReferenceFileName(childpath);
				group.m_filename = reffilename.asChar();
			}
		}
	}

	status = GetTransform(group.m_transform,mdagPath);

	mffIntArray temp;
	if (attrindicies)
		temp = *attrindicies;

	return m_mff->AddGroupTransformInstance(parentidx,layerindex,group,temp);
}

int mffTranslator::ExportLightInstance(int parentidx,MDagPath &mdagPath)
{
	MStatus status;
	mffLight light;

	MFnDependencyNode fnNode(mdagPath.node());

	light.m_name = fnNode.name().asChar();

	MFnLight fnLight(mdagPath.node());

	if (fnLight.typeName() == "pointLight")
		light.m_type = mffLight::omni;
	else if (fnLight.typeName() == "spotLight")
		light.m_type = mffLight::spot;
	else if (fnLight.typeName() == "directionalLight")
		light.m_type = mffLight::directional;

	light.m_intensity = fnLight.intensity();

	MColor mcolor;

	mcolor = fnLight.color();

	light.m_color = mffColor(mcolor.r,mcolor.g,mcolor.b,mcolor.a);

	return m_mff->AddLightInstance(parentidx,light);
}

int mffTranslator::ExportLocatorInstance(int parentidx,MDagPath &mdagPath,mffIntArray *attrindicies)
{
	MStatus status;
	mffNULL Null;

	MFnDependencyNode fnNode(mdagPath.node());

	Null.m_name = fnNode.name().asChar();

	mffIntArray temp;
	if (attrindicies)
		temp = *attrindicies;

	return m_mff->AddNULLInstance(parentidx,Null,temp);
}

int mffTranslator::ExportJointInstance(int parentidx,MDagPath &mdagPath)
{
	MStatus status;
	mffBone bone;

	MFnDependencyNode fnNode(mdagPath.node());

	bone.m_name = fnNode.name().asChar();

	return m_mff->AddBoneInstance(parentidx,bone);
}

MStatus mffTranslator::GetTransform(mffTransform &transform,MDagPath &mdagPath)
{
	MStatus status;
	MVector translate, rotate, scale;

	MFnTransform fnTransform(mdagPath.node());

	translate = fnTransform.translation(MSpace::kTransform,&status);
	if (status != MS::kSuccess)
		return MS::kFailure;

	transform.translate.x = translate.x;
	transform.translate.y = translate.y;
	transform.translate.z = translate.z;

	MEulerRotation rotation;

	status = fnTransform.getRotation(rotation);
	if (status != MS::kSuccess)
		return MS::kFailure;

	transform.rotate.x = RADTODEG * rotation.x;
	transform.rotate.y = RADTODEG * rotation.y;
	transform.rotate.z = RADTODEG * rotation.z;

	double dScale[3];

	status = fnTransform.getScale(dScale);
	if (status != MS::kSuccess)
		return MS::kFailure;

	transform.scale.x = dScale[0];
	transform.scale.y = dScale[1];
	transform.scale.z = dScale[2];

	MPoint pivot;
	pivot = fnTransform.rotatePivot(MSpace::kTransform,&status);
	transform.rotatepivot = mffVector(pivot.x,pivot.y,pivot.z);

	MVector pivottranslation;
	pivottranslation = fnTransform.rotatePivotTranslation(MSpace::kTransform,&status);
	transform.rotatepivottranslation = mffVector(pivottranslation.x,pivottranslation.y,pivottranslation.z);

	pivot = fnTransform.scalePivot(MSpace::kTransform,&status);
	transform.scalepivot = mffVector(pivot.x,pivot.y,pivot.z);

	pivottranslation = fnTransform.scalePivotTranslation(MSpace::kTransform,&status);
	transform.scalepivottranslation = mffVector(pivottranslation.x,pivottranslation.y,pivottranslation.z);

	return MS::kSuccess;
}

void mffTranslator::mffVectorToMVector(std::vector <mffVector> vectorArray, MVectorArray &mvectorArray)
{
	mvectorArray.clear();
	for (unsigned i = 0; i < vectorArray.size(); i++)
		mvectorArray.append(MVector(vectorArray[i].x,vectorArray[i].y,vectorArray[i].z));
}

void mffTranslator::mffVectorToMPoint(std::vector <mffVector> vectorArray, MPointArray &mpointArray)
{
	mpointArray.clear();
	for (unsigned i = 0; i < vectorArray.size(); i++)
		mpointArray.append(MPoint(vectorArray[i].x,vectorArray[i].y,vectorArray[i].z));
}

void mffTranslator::doubleToMDouble(std::vector <double> doublearray, MDoubleArray &mdoublearray)
{
	mdoublearray.clear();
	for (unsigned i = 0; i < doublearray.size(); i++)
		mdoublearray.append(doublearray[i]);
}

void mffTranslator::intToMInt(std::vector <int> intArray, MIntArray &mintArray)
{
	mintArray.clear();
	for (unsigned i = 0; i < intArray.size(); i++)
		mintArray.append(intArray[i]);
}

void mffTranslator::CreateShaderTable(MObjectArray &SGSets,std::vector<mffShader> &shaders)
{
	MStatus status;

	for (int i = 0; i < SGSets.length();i++)
	{
		MFnDependencyNode SGdepnode(SGSets[i]);

	if (!m_shadertableperobject)
		if (m_sgsetmap.HasKey((int)SGdepnode.f_ptr))
			continue;

//		{
			const char *shadername = SGdepnode.name().asChar();

			mffShader shader;

			strcpy(shader.m_name,shadername);

			MPlug fnPlug = SGdepnode.findPlug("surfaceShader",&status);
			if (status)
			{
				MPlugArray connectedNodes;
				fnPlug.connectedTo(connectedNodes,true,false,&status);
				if (status)
				{
					if (connectedNodes.length()==0)
					{
						shaders.push_back(shader);
						m_sgsetmap.SetValue((int)SGdepnode.f_ptr,shaders.size()-1);
						continue;
					}

					MObject shadernode = connectedNodes[0].node();

					MFnBlinnShader fnBlinn(shadernode);
					MFnPhongShader fnPhong(shadernode);
					MFnLambertShader fnLambert(shadernode);

					MColor diffuse;
					float reflectivity = 0.0f;

					if (shadernode.apiType() == MFn::kBlinn)
					{
						diffuse = fnBlinn.color(&status);
						reflectivity = fnBlinn.reflectivity(&status);					
					}
					else if (shadernode.apiType() == MFn::kPhong)
					{
						diffuse = fnPhong.color(&status);
						reflectivity = fnPhong.reflectivity(&status);
					}
					else if (shadernode.apiType() == MFn::kLambert)
						diffuse = fnLambert.color(&status);

					shader.m_color = mffColor(diffuse.r,diffuse.g,diffuse.b,diffuse.a);
					shader.m_reflect = reflectivity;

					MFnDependencyNode fnNode(shadernode);

					MPlug colorplug = fnNode.findPlug("color",&status);
					if (status)
					{
						MPlugArray connectedNodes2;
						colorplug.connectedTo(connectedNodes2,true,false,&status);
						if ((status)&&(connectedNodes2.length() > 0))
						{
							MObject colorNode = connectedNodes2[0].node();

							if (colorNode.apiType() == MFn::kFileTexture)
							{
								mffStd *standard = new mffStd;

								mffMap mffmap;
								GetMap(colorNode,mffmap,standard);
								shader.m_colorMap = standard;
							}
							else if (colorNode.apiType() == MFn::kBlendColors)
							{
								mffBlender *blender = new mffBlender;

								MFnDependencyNode fnNode2(colorNode);

								MPlug colorplugA = fnNode2.findPlug("color1",&status);

								MPlug colorplugB = fnNode2.findPlug("color2",&status);
					
								MPlug blenderplug = fnNode2.findPlug("blender",&status);

								MPlugArray color1plugarray;
								colorplugA.connectedTo(color1plugarray,true,false,&status);

								MPlugArray color2plugarray;
								colorplugB.connectedTo(color2plugarray,true,false,&status);

								MPlugArray blenderplugarray;
								blenderplug.connectedTo(blenderplugarray,true,false,&status);

								if ((color1plugarray.length() > 0)&&(color2plugarray.length() > 0) &&(blenderplugarray.length() > 0))
								{

									MObject color1object,color2object,blenderobject;

									color1object = color1plugarray[0].node();
									color2object = color2plugarray[0].node();
									blenderobject = blenderplugarray[0].node();

									if ((color1object.apiType() == MFn::kFileTexture)&&(color2object.apiType() == MFn::kFileTexture)
											&&(blenderobject.apiType() == MFn::kFileTexture))
									{
										mffMap mffmap0,mffmap1,mffmap2;

										GetMap(color1object,mffmap0,blender);

										GetMap(color2object,mffmap1,blender);

										GetMap(blenderobject,mffmap2,blender);

										shader.m_colorMap = blender;
									}
								}
							}
							else if (colorNode.apiType() == MFn::kLayeredTexture)
							{
								mffLayeredTexture *layeredtexture = new mffLayeredTexture;

								MFnDependencyNode shaderdepnode(colorNode);

								MPlug plug = shaderdepnode.findPlug("inputs",&status);
								if (!status)
									continue;
								
								int numelements = plug.numConnectedElements(&status);
								
								layeredtexture->m_types.resize(numelements);
	//							layeredtexture->m_alphavalues.resize(numelements);
								layeredtexture->m_mapsconnected.resize(numelements);
	//							layeredtexture->m_alphasconnected.resize(numelements);

	//							Loop through array of ".inputs"
								for (int j = 0; j < numelements; j++)
								{
									MPlug elementplug = plug.elementByPhysicalIndex(j,&status);
									MString elementplugname = elementplug.name();
									const char *debug = elementplugname.asChar();

									mffMap map;

	//								first child in compound attribute is ".color"
									MPlug colorplug = elementplug.child(0,&status);
									if (status)
									{
										mffColor colorvalue = mffColor(1.0f,1.0f,1.0f,1.0f);

										MString colorplugname = colorplug.name();
										const char *debug0 = colorplugname.asChar();

										MPlugArray connected;
										colorplug.connectedTo(connected,true,false,&status);

										if (connected.length()>0)
										{
											MObject filenode = connected[0].node();
											if (filenode.apiType() == MFn::kFileTexture)
											{
												mffMap mffmap;
												GetMap(filenode,mffmap,layeredtexture);
												layeredtexture->m_mapsconnected[j] = 1;
											}
										}
										else
										{
											layeredtexture->m_mapsconnected[j] = 0;
										}
									}
	/*	
									float alphavalue = 1.0;
									mffMap alpha;
	//								second child is ".alpha"
									MPlug alphaplug = elementplug.child(1,&status);
									if (status)
									{
										MPlugArray connected;
										alphaplug.connectedTo(connected,true,false,&status);

										if (connected.length()>0)
										{
											MObject filenode = connected[0].node();
											if (filenode.apiType() == MFn::kFileTexture)
											{
												mffMap mffmap;
												GetMap(filenode,mffmap);
												int index = m_maptable.push_back(mffmap);
												layeredtexture->m_mapindicies.push_back(index);
												layeredtexture->m_alphasconnected[j] = 1;
											}
										}
										else
										{
											alphaplug.getValue(alphavalue);
											layeredtexture->m_alphasconnected[j] = 0;

										}
									}
									layeredtexture->m_alphavalues[j] = (double)alphavalue;
	*/
	//								third child is ".blend"
									MPlug blendplug = elementplug.child(2,&status);
									int blend;
									blendplug.getValue(blend);
									layeredtexture->m_types[j] = blend;
								}
								shader.m_colorMap = layeredtexture;
							}
						}
					}
					MPlug reflectplug = fnNode.findPlug("reflectedColor",&status);
					if (status)
					{
						MPlugArray connectedNodes;
						reflectplug.connectedTo(connectedNodes,true,false,&status);

						if ((status)&&(connectedNodes.length() > 0))
						{
							MObject envball = connectedNodes[0].node();

							MFnDependencyNode envballdepnode(envball);
							
							const char *debug = envballdepnode.name().asChar();

							if (envball.hasFn(MFn::kEnvBall))
							{
								MPlug imageplug = envballdepnode.findPlug("image",&status);
								if (status)
								{
									MPlugArray connectedNodes2;
									imageplug.connectedTo(connectedNodes2,true,false,&status);

									MObject imageobject = connectedNodes2[0].node();
								
									if (imageobject.apiType() == MFn::kFileTexture)
									{
										mffStd *standard = new mffStd;
										GetMap(imageobject,standard->m_map,standard);
										shader.m_reflectMap = standard;
									}
								}
							}
						}
					}
				}
			}
			shaders.push_back(shader);
			
	if (!m_shadertableperobject)
			m_sgsetmap.SetValue((int)SGdepnode.f_ptr,shaders.size()-1);
//		}
	}
		
}

int mffTranslator::GetMap(MObject filenode,mffMap &mffmap,mffMapObject *mapobject)
{
	MStatus status;

	int index;

	MFnDependencyNode filedepnode(filenode);

	if (!m_sgsetmap.HasKey((int)filedepnode.f_ptr))
	{

		const char *debug = filedepnode.name().asChar();

		if (!filenode.hasFn(MFn::kFileTexture))
			return 0;

		MPlug filenameplug = filedepnode.findPlug("fileTextureName",&status);

		if (!status)
			return 0;

		MString filename;
		status = filenameplug.getValue(filename);

		strcpy(mffmap.m_filename,filename.asChar());

		GetPlacementNodeInfo(filenode,mffmap);

		GetUVChooserInfo(filenode,mffmap);

	//	Color balance options
	//	default color
		MPlug defaultcolorplugR = filedepnode.findPlug("defaultColorR",&status);
		MPlug defaultcolorplugG = filedepnode.findPlug("defaultColorG",&status);
		MPlug defaultcolorplugB = filedepnode.findPlug("defaultColorB",&status);
		if (status)
		{
			defaultcolorplugR.getValue(mffmap.m_defaultcolor.r);
			defaultcolorplugG.getValue(mffmap.m_defaultcolor.g);
			defaultcolorplugB.getValue(mffmap.m_defaultcolor.b);
		}
	//	color gain
		MPlug colorgainplugR = filedepnode.findPlug("colorGainR",&status);
		MPlug colorgainplugG = filedepnode.findPlug("colorGainG",&status);
		MPlug colorgainplugB = filedepnode.findPlug("colorGainB",&status);
		if (status)
		{
			colorgainplugR.getValue(mffmap.m_colorgain.r);
			colorgainplugG.getValue(mffmap.m_colorgain.g);
			colorgainplugB.getValue(mffmap.m_colorgain.b);
		}
	//	color offset
		MPlug coloroffsetplugR = filedepnode.findPlug("colorOffsetR",&status);
		MPlug coloroffsetplugG = filedepnode.findPlug("colorOffsetG",&status);
		MPlug coloroffsetplugB = filedepnode.findPlug("colorOffsetB",&status);
		if (status)
		{
			coloroffsetplugR.getValue(mffmap.m_coloroffset.r);
			coloroffsetplugG.getValue(mffmap.m_coloroffset.g);
			coloroffsetplugB.getValue(mffmap.m_coloroffset.b);
		}
	//	alpha gain
		MPlug alphagainplug = filedepnode.findPlug("alphaGain",&status);
		if (status)
		{
			alphagainplug.getValue(mffmap.m_alphagain);
		}
	//	alpha offset
		MPlug alphaoffsetplug = filedepnode.findPlug("alphaOffset",&status);
		if (status)
		{
			alphaoffsetplug.getValue(mffmap.m_alphaoffset);
		}

		m_maptable.push_back(mffmap);

		index = m_maptable.size()-1;

		m_sgsetmap.SetValue((int)filedepnode.f_ptr,index);
	}
	else
	{
		index =  m_sgsetmap.GetValue((int)filedepnode.f_ptr);
	}

	mapobject->m_mapindicies.push_back(index);
	
	return 1;
}

//	The placement nodes stores a file textures tiling information
int mffTranslator::GetPlacementNodeInfo(MObject filenode,mffMap &mffmap)
{
	MStatus status;

	MFnDependencyNode fnNode(filenode);

	const char *debug = fnNode.name().asChar();

	MPlug uvCoordPlug = fnNode.findPlug("uvCoord",&status);
	if (!status)
		return 0;

	MPlugArray connectedNodes;
	uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
	if (!status)
		return 0;

	if (connectedNodes.length()==0)
		return 0;
	

	if (!connectedNodes[0].node().hasFn(MFn::kPlace2dTexture))
		return 0;

	MObject placementNode = connectedNodes[0].node();

	MFnDependencyNode placementDepNode(placementNode);

//	repeat
	MPlug repeatUplug = placementDepNode.findPlug("repeatU",&status);
	MPlug repeatVplug = placementDepNode.findPlug("repeatV",&status);
	if (status)
	{
		double repeatU,repeatV;
		status = repeatUplug.getValue(repeatU);
		status = repeatVplug.getValue(repeatV);
		mffmap.m_repeat.x = repeatU;
		mffmap.m_repeat.y = repeatV;
	}

//	wrap
	MPlug wrapUplug = placementDepNode.findPlug("wrapU",&status);
	MPlug wrapVplug = placementDepNode.findPlug("wrapV",&status);
	if (status)
	{
		status = wrapUplug.getValue(mffmap.m_wrapu);
		status = wrapVplug.getValue(mffmap.m_wrapv);
	}

#ifndef MAYA40
//	mirror
	MPlug mirrorUplug = placementDepNode.findPlug("mirrorU",&status);
	MPlug mirrorVplug = placementDepNode.findPlug("mirrorV",&status);
	if (status)
	{
		status = mirrorUplug.getValue(mffmap.m_mirroru);
		status = mirrorVplug.getValue(mffmap.m_mirrorv);
	}
#endif

//	stagger
	MPlug staggerplug = placementDepNode.findPlug("stagger",&status);
	if (status)
		status = staggerplug.getValue(mffmap.m_stagger);

//	translation
	MPlug offsetUplug = placementDepNode.findPlug("offsetU",&status);
	MPlug offsetVplug = placementDepNode.findPlug("offsetV",&status);
	if (status)
	{
		double offsetU,offsetV;
		status = offsetUplug.getValue(offsetU);
		status = offsetVplug.getValue(offsetV);
		mffmap.m_translation.x = offsetU;
		mffmap.m_translation.y = offsetV;
	}

//	rotation
	MPlug rotateUVplug = placementDepNode.findPlug("rotateUV",&status);
	if (status)
	{
		double rotateUV;
		status = rotateUVplug.getValue(rotateUV);
		mffmap.m_rotation = rotateUV;
	}
		
	GetUVChooserInfo(placementNode,mffmap);

	return 1;
}

int mffTranslator::GetUVChooserInfo(MObject node,mffMap &mffmap)
{
	MStatus status;

	MFnDependencyNode depnode(node);

	const char *debug = depnode.name().asChar();

	MPlug uvCoordPlug = depnode.findPlug("uvCoord",&status);
	if (!status)
		return 0;

	MPlugArray connectedNodes;
	uvCoordPlug.connectedTo(connectedNodes,true,false,&status);
	if (!status)
		return 0;

	if (connectedNodes.length()==0)
		return 0;
	
	if (!connectedNodes[0].node().hasFn(MFn::kUvChooser))
		return 0;

	MObject uvchooser = connectedNodes[0].node();

	MFnDependencyNode uvchooserdepnode(uvchooser);

	MPlug uvsetplug = uvchooserdepnode.findPlug("uvSets",&status);
	if (!status)
		return 0;

	int numelements = uvsetplug.numElements();

	for (int i = 0; i < numelements; i++)
	{
		MPlug elementplug = uvsetplug.elementByPhysicalIndex(i,&status);

		elementplug.connectedTo(connectedNodes,true,false,&status);
		if (!status)
			return 0;

		int numconnected = connectedNodes.length();
		
		if (numconnected > 0)
		{
			MPlug uvsetnameplug = connectedNodes[0];

			MString uvsetname;

			connectedNodes[0].getValue(uvsetname);
			
			const char *debug0 = uvsetname.asChar();

			strcpy(mffmap.m_uvsetname,uvsetname.asChar());
		}
	}
	
	return 1;
}

MStatus mffTranslator::CreateShadingGroups(std::vector<mffShader> &shaders,MObjectArray &sgsets)
{
	for (int i = 0; i < shaders.size(); i++)
	{
		CreateShadingGroup(shaders,i,sgsets);
		MTools::StepProgressBar(1);
	}

	return MStatus::kSuccess;
}

// dagpath needs to belong to the mesh shape of the object for hooking up "uvchooser" node
int mffTranslator::CreateShadingGroup(std::vector<mffShader> &shaders,unsigned index,MObjectArray &sgsets)
{
	mffShader *mffshader = &shaders[index];
	MDGModifier dgmodifier;
	MString shadinggroupname;

	MStatus status;
	MDGModifier dgModifier;
	MFnSet fnSet;
	MColor color;

	MSelectionList selection;
	status = selection.add(MObject::kNullObj);

	MObject shadingGroup = fnSet.create(selection, MFnSet::kRenderableOnly, &status);
	if (!status) 
		return 0;

	sgsets.append(shadingGroup);

	MString shadername = mffshader->m_name;
		
	status = dgModifier.renameNode(shadingGroup,shadername);
	dgModifier.doIt();

	MFnBlinnShader blinn;
	MObject mshader = blinn.create(TRUE,&status);
	if (!status)
		return 0;

	MFnBlinnShader fnshader(mshader);

//	Set diffuse color
	status = fnshader.setColor(MColor(MColor::kRGB,mffshader->m_color.r,mffshader->m_color.g,mffshader->m_color.b));
	if (!status)
		return 0;
		
//	Set specular to black, we don't need for games, yet!?
	status = fnshader.setSpecularColor(MColor(MColor::kRGB,0.0,0.0,0.0));
	if (!status)
		return 0;

	status = dgModifier.renameNode(mshader,shadername+"Shader");
	if (!status)
		return 0;

	shadinggroupname = fnshader.name();

//	Force new connection between shading group and shader, there really isn't a simpler way.
	MGlobal::executeCommand (
		MString ("defaultNavigation -quiet true -force true -connectToExisting -source ") +
			shadinggroupname + MString (" -destination ") + fnSet.name(),false);
	
	if (mffshader->m_colorMap)
	{
		if (mffshader->m_colorMap->GetType() == mffMapObject::kSTANDARD)
		{
			mffStd *standard = (mffStd*)mffshader->m_colorMap;
			mffMap *mffmap = &(*m_pMapTable)[standard->m_mapindicies[0]];

			LinkUpMapNode(shadinggroupname,".color",mffmap);
		}
		else if (mffshader->m_colorMap->GetType() == mffMapObject::kBLENDER)
		{
			mffBlender *blender = (mffBlender*)mffshader->m_colorMap;
			mffMap *mffmap = &(*m_pMapTable)[blender->m_mapindicies[0]];
			MString blendnode;
			
			MGlobal::executeCommand (MString ("shadingNode -asUtility blendColors"),blendnode );
			MGlobal::executeCommand (MString ("connectAttr ") + blendnode + MString (".output ") + shadinggroupname + MString (".color") );

			LinkUpMapNode(blendnode,".color1",mffmap);

			mffmap = &(*m_pMapTable)[blender->m_mapindicies[1]];

			LinkUpMapNode(blendnode,".color2",mffmap);
			
			mffmap = &(*m_pMapTable)[blender->m_mapindicies[2]];

			LinkUpMapNode(blendnode,".blender",mffmap,true);
		}
		else if (mffshader->m_colorMap->GetType() == mffMapObject::kLAYEREDTEXTURE)
		{
			mffLayeredTexture *layeredtexture = (mffLayeredTexture*)mffshader->m_colorMap;

			MString layeredtexturenode;			
			MGlobal::executeCommand(MString ("shadingNode -asUtility layeredTexture"),layeredtexturenode );

			//	Force new connection between shading group and shader, there really isn't a simpler way.
			MGlobal::executeCommand (
				MString ("defaultNavigation -quiet true -force true -connectToExisting -source ") +
					layeredtexturenode + MString (" -destination ") + shadinggroupname + MString(".color"));

			int cnt = 0;
			for (int j = 0; j < layeredtexture->m_mapsconnected.size(); j++)
			{
				mffMap *mffmap = &(*m_pMapTable)[layeredtexture->m_mapindicies[j]];

				MGlobal::executeCommand(MString("setAttr ") + layeredtexturenode +
					MString(".inputs[") + j + MString("].color -type \"double3\" ")
					+ 1.0 + MString(" ") + 1.0 + MString(" ") + 1.0);

				MGlobal::executeCommand(MString("setAttr ") + layeredtexturenode + MString(".inputs[") + j + MString("].alpha 1"));

				MGlobal::executeCommand(MString("setAttr ") + layeredtexturenode + MString(".inputs[") + j + MString("].blendMode ") + layeredtexture->m_types[j]);

				if (layeredtexture->m_mapsconnected[j])
					LinkUpMapNode(layeredtexturenode+MString(".inputs[") + j + MString("]"),".color",mffmap);		

//				if (layeredtexture->m_alphasconnected[j])
//					LinkUpMapNode(layeredtexturenode+MString(".inputs[") + j + MString("]"),".alpha",layeredtexture->m_maps[cnt++],true,true);
			}
		}		
	}

	if (mffshader->m_reflectMap)
	{
		if (mffshader->m_reflectMap->GetType() == mffMapObject::kSTANDARD)
		{
			MString envball,place3dtexture;			
			MGlobal::executeCommand(MString ("shadingNode -asTexture envBall"),envball);
			MGlobal::executeCommand(MString ("shadingNode -asUtility place3dTexture"),place3dtexture);

			MGlobal::executeCommand(MString ("connectAttr -force ") + envball + MString (".outColor ") + shadinggroupname + MString (".reflectedColor") );

			MObject place3dtexturemobject = MTools::GetMObject(place3dtexture);
			MObject envballmobject = MTools::GetMObject(envball);

			MFnDependencyNode place3dtexturedepnode(place3dtexturemobject);
			MPlug worldInverseMatrixPlug = place3dtexturedepnode.findPlug("worldInverseMatrix",&status);
			int numelements = worldInverseMatrixPlug.numElements();
			MPlug worldInverseMatrixPlugN = worldInverseMatrixPlug.elementByLogicalIndex(numelements);
			MString worldInverseMatrixPlugNname = worldInverseMatrixPlugN.name();
			const char *debug = worldInverseMatrixPlugNname.asChar();

			MFnDependencyNode envballdepnode(envballmobject);
			MPlug placementMatrixPlug = envballdepnode.findPlug("placementMatrix",&status);
			if (!status)
				return 0;
			
			status = dgmodifier.connect(worldInverseMatrixPlugN,placementMatrixPlug);
			if (!status)
				MGlobal::displayError("Couldn't hook up 3d placement node.");									
		
			mffStd *standard = (mffStd*)mffshader->m_reflectMap;
			mffMap *mffmap = &(*m_pMapTable)[standard->m_mapindicies[0]];

			LinkUpMapNode(envball,".image",mffmap);
		}
	}
	return 1;
}


MString mffTranslator::FixFileName(const char *filename)
{
	MString fname = filename,newfilename;
	MStringArray fsplit;

	newfilename = filename;
	fname.split('\\',fsplit);
	if (fsplit.length()>1)
	{
		newfilename = fsplit[0];
		for (unsigned i = 1; i < fsplit.length(); i++)
		{	
			newfilename += MString("/") + fsplit[i];
		}
	}
	return newfilename;
}

int mffTranslator::LinkUpMapNode(MString shadernode,
											MString attrname,
											mffMap *mffmap,
											bool asAlpha)
{

	MStatus status;
	MString filename = FixFileName(mffmap->m_filename);
	MDGModifier dgmodifier;

	MString placementnode;
	MString filenode;

	if (!m_filenodemap.HasKey((int)mffmap))
	{
		MGlobal::executeCommand (MString ("shadingNode -asTexture file"),filenode);

		if (!asAlpha)
		{
			MGlobal::executeCommand (
					MString ("connectAttr -force ") + filenode + MString (".outColor ") +
					shadernode + attrname,false,false );
		}
		else
		{
			MGlobal::executeCommand (
					MString ("connectAttr -force ") + filenode + MString (".outAlpha ") +
					shadernode + attrname,false,false );
		}

	//	Set filename
		MGlobal::executeCommand (
				MString ("setAttr -type \"string\" ") +
				filenode + MString (".fileTextureName \"") +
				filename + MString ("\"") );

	//	Set color balance attributes
	//	default color
		MGlobal::executeCommand (
				MString ("setAttr ") +  filenode + MString (".defaultColor ") +
					MString ("-type double3 ") + mffmap->m_defaultcolor.r +
					MString(" ") + mffmap->m_defaultcolor.g +
					MString(" ") + mffmap->m_defaultcolor.b);

	//	color gain
		MGlobal::executeCommand (
				MString ("setAttr ") +  filenode + MString (".colorGain ") +
					MString ("-type double3 ") + mffmap->m_colorgain.r +
					MString(" ") + mffmap->m_colorgain.g +
					MString(" ") + mffmap->m_colorgain.b);

	//	color offset
		MGlobal::executeCommand (
				MString ("setAttr ") +  filenode + MString (".colorOffset ") +
					MString ("-type double3 ") + mffmap->m_coloroffset.r +
					MString(" ") + mffmap->m_coloroffset.g +
					MString(" ") + mffmap->m_coloroffset.b);

	//	alpha gain
		MGlobal::executeCommand (
				MString ("setAttr ") +  filenode + MString (".alphaGain ") +
					MString ("-type double ") + mffmap->m_alphagain);

	//	alpha offset
		MGlobal::executeCommand (
				MString ("setAttr ") +  filenode + MString (".alphaOffset ") +
					MString ("-type double ") + mffmap->m_alphaoffset);


		CreatePlacementNode(filenode,mffmap,placementnode);

		MObject filenmobject = MTools::GetMObject(filenode);
		m_filenodes.append(filenmobject);
		m_filenodemap.SetValue((int)mffmap,m_filenodes.length()-1);

		MString uvchoosernode;
		
		if (strcmp(mffmap->m_uvsetname,"map1")!=0)
		{
			CreateUVChooserNode(placementnode,mffmap,uvchoosernode);

			MObject uvchooserobject = MTools::GetMObject(uvchoosernode);

			m_uvchoosernodes.append(uvchooserobject);

			m_uvchoosernodemap.SetValue((int)mffmap,m_uvchoosernodes.length()-1);
		}
	}
	else
	{
		int idx = m_filenodemap.GetValue((int)mffmap);
		MFnDependencyNode depnode(m_filenodes[idx]);
		filenode = depnode.name();

		int result;
		if (!asAlpha)
		{
			MGlobal::executeCommand(MString("isConnected ") + filenode + MString(".outColor ") + shadernode + attrname,result);
			if (!result)
				MGlobal::executeCommand(MString ("connectAttr -force ") + filenode + MString (".outColor ") + shadernode + attrname,false,false );
		}
		else
		{
			MGlobal::executeCommand(MString("isConnected ") + filenode + MString(".outAlpha ") + shadernode + attrname,result);
			if (!result)
				MGlobal::executeCommand(MString ("connectAttr -force ") + filenode + MString (".outAlpha ") + shadernode + attrname,false,false );
		}

//		CreatePlacementNode(filenode,mffmap,placementnode);

//		MString uvchoosernode;
		
//		CreateUVChooserNode(placementnode,mffmap,uvchoosernode);

//		MObject uvchooserobject = MTools::GetMObject(uvchoosernode);

//		m_uvchoosernodes.append(uvchooserobject);

//		m_uvchoosernodemap.SetValue((int)mffmap,m_uvchoosernodes.length()-1);
	}

	return 1;
}

int mffTranslator::HookUpChooser(MDagPath dagpath,mffMap *mffmap)
{
	MStatus status;
	MDGModifier dgmodifier;
	MObject shape = dagpath.child(0);

	MFnDependencyNode shapedepnode(shape);
	const char *debugshapename = shapedepnode.name().asChar();

	if (!m_uvchoosernodemap.HasKey((int)mffmap))
		return 0;

	int index = m_uvchoosernodemap.GetValue((int)mffmap);
	MObject uvchooser = m_uvchoosernodes[index];

	MFnDependencyNode depnode(shape);

	MPlug uvSetplug = depnode.findPlug("uvSet",&status);
	int test = uvSetplug.evaluateNumElements(&status);
	int numelements = uvSetplug.numElements(&status);
	for (int i = 0; i < numelements; i++)
	{
		MPlug plug = uvSetplug.elementByLogicalIndex(i,&status);
		if (status)
		{
			int numchildren = plug.numChildren();
			if (numchildren>0)
			{
				MPlug uvsetnameplug = plug.child(0,&status);
				if (status)
				{

					if (!uvsetnameplug.isConnected())
					{

						MString uvsetnameplugname = uvsetnameplug.name();
						const char *debug = uvsetnameplugname.asChar();
							
						MString uvsetname;
						uvsetnameplug.getValue(uvsetname);
						const char *debug0 = uvsetname.asChar();
						
						if (uvsetname == "map1")
							return 1;

						if (strcmp(uvsetname.asChar(),mffmap->m_uvsetname) == 0)
						{
							MFnDependencyNode depnode(uvchooser);
							MPlug uvSetsplug = depnode.findPlug("uvSets",&status);
							if (status)
							{
								int test = uvSetsplug.evaluateNumElements(&status);
								int numelements = uvSetsplug.numElements(&status);
								MPlug uvchooseuvsetsplug = uvSetsplug.elementByLogicalIndex(numelements,&status);
								MString uvchooseruvsetplugname = uvchooseuvsetsplug.name();
								const char *debug1 = uvchooseruvsetplugname.asChar();

								MFnDependencyNode uvsetnamedepnode(uvsetnameplug.attribute());
								MFnDependencyNode uvchooseruvsetsdepnode(uvchooseuvsetsplug.attribute());

								status = dgmodifier.connect(uvsetnameplug,uvchooseuvsetsplug);
								if (!status)
									MGlobal::displayError("Couldn't hook up mesh to uv chooser.");									
							}
						}
					}
				}
			}
		}
	}

	return 1;
}

int mffTranslator::CreatePlacementNode(MString filenode,mffMap *mffmap,MString &placementnode)
{
	placementnode = "";

//	create place2dTexture node
	MGlobal::executeCommand (
		MString ("shadingNode -asUtility place2dTexture"),placementnode );


//	BUILD PLACEMNET NODE CONNECTIONS

//	coverage
	MGlobal::executeCommand ( MString ("connectAttr ") + placementnode + MString (".coverage ") + filenode + MString (".coverage") );

//	translateFrame
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".translateFrame ") + filenode + MString (".translateFrame"));

//	rotateFrame
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".rotateFrame ") + filenode + MString (".rotateFrame") );

#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".mirrorU ") + filenode + MString (".mirrorU") );

	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".mirrorV ") + filenode + MString (".mirrorV") );
#endif

//	stagger
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".stagger ") + filenode + MString (".stagger"));

//	wrapU
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".wrapU ") + filenode + MString (".wrapU"));

//	wrapV
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".wrapV ") +filenode + MString (".wrapV"));

//	repeatUV
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".repeatUV ") + filenode + MString (".repeatUV"));

//	offset
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".offset ") + filenode + MString (".offset"));

//	rotateUV
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".rotateUV ") + filenode + MString (".rotateUV") );

//	outUV
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".outUV ") + filenode + MString (".uv") );

//	outUvFilterSize
	MGlobal::executeCommand (MString ("connectAttr ") + placementnode + MString (".outUvFilterSize ") + filenode + MString (".uvFilterSize") );

//	vertexCameraOne
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".vertexCameraOne ") + filenode + MString (".vertexCameraOne") );
#endif

//	vertexUVCameraOne
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".vertexUvOne ") + filenode + MString (".vertexUvOne") );
#endif

//	vertexUVCameraTwo
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".vertexUvTwo ") + filenode + MString (".vertexUvTwo") );
#endif

//	vertexUVCameraThree
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".vertexUvThree ") + filenode + MString (".vertexUvThree") );
#endif
	
//	SET PLACEMENT NODE ATTRIBUTES

//	repeat
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".repeatU ") + MString ("-type double ") + mffmap->m_repeat.x);

	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".repeatV ") + MString ("-type double ") + mffmap->m_repeat.y);

//	wrap
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".wrapU ") + mffmap->m_wrapu);

	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".wrapV ") + mffmap->m_wrapv);

#ifndef MAYA40
//	mirror
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".mirrorU ") + mffmap->m_mirroru);

	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".mirrorV ") + mffmap->m_mirrorv);
#endif

//	stagger
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".stagger ") + mffmap->m_stagger);

//	translation
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".offsetU ") + MString ("-type double ") + mffmap->m_translation.x);

	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".offsetV ") + MString ("-type double ") + mffmap->m_translation.y);

//	rotation
	MGlobal::executeCommand (MString ("setAttr ") +  placementnode + MString (".rotateUV ") + MString ("-type double ") + mffmap->m_rotation);

	return 1;
}

int mffTranslator::CreateUVChooserNode(MString placementnode,mffMap *mffmap,MString &uvchoosernode)
{
	uvchoosernode = "";
//	Create place2dTexture node
	MGlobal::executeCommand("createNode \"uvChooser\"",uvchoosernode);

//	Build UV Chooser node connections
//	outUV
	MGlobal::executeCommand(MString ("connectAttr ") + uvchoosernode + MString (".outUv ") + placementnode + MString (".uvCoord") );

//	vertexCameraOne
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + uvchoosernode + MString (".outVertexCameraOne ") + placementnode + MString (".vertexCameraOne") );
#endif
	
//	vertexUVCameraOne
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + uvchoosernode + MString (".outVertexUvOne ") + placementnode + MString (".vertexUvOne") );
#endif

//	vertexUVCameraTwo
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + uvchoosernode + MString (".outVertexUvTwo ") + placementnode + MString (".vertexUvTwo") );
#endif

//	vertexUVCameraThree
#ifndef MAYA40
	MGlobal::executeCommand(MString ("connectAttr ") + uvchoosernode + MString (".outVertexUvThree ") + placementnode + MString (".vertexUvThree") );
#endif

	return 1;
}

int mffTranslator::CreateMeshTestTable()
{
	MStatus status;;

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	if (!status)
	    return 0;

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath dagpath;
		status = dagIterator.getPath(dagpath);
		if (!status)
			return 0;

		if ((!dagpath.hasFn(MFn::kTransform))&&(dagpath.hasFn(MFn::kMesh)))
		{
			MFnDependencyNode depnode(dagpath.node());

			MeshTest meshtest(dagpath);
			m_meshtesttable.push_back(meshtest);
			m_meshtestmap.SetValue((int)depnode.f_ptr,m_meshtesttable.size()-1);
//			m_meshtesthash.SetValue((int)meshtest.hashValue,m_meshtesttable.size()-1);
			m_meshtesthash.SetValue((int)meshtest.hashValue,(int)depnode.f_ptr);
		}
	}
	return 1;
}