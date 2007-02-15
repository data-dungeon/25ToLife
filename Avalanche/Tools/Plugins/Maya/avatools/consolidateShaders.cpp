#include "stdafx.h"
#include "consolidateShaders.h"
#include "maya/mitmeshpolygon.h"

consolidateShaders::consolidateShaders() {}

consolidateShaders::~consolidateShaders() {}

void* consolidateShaders::creator()
{
	return new consolidateShaders();
}

bool consolidateShaders::isUndoable() const
{
    return false;
}

MStatus consolidateShaders::undoIt()
{
    return MS::kSuccess;
}

MStatus consolidateShaders::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus consolidateShaders::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	if (selection.length()==0)
	{
		MGlobal::displayError("No objects were selected.");
		return MS::kFailure;
	}
	
	// count objects to be evaluated
	IntMap norepeat;
	int objectcount = 0;
	for (int i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;

		selection.getDagPath(i,dagpath);

		// skip over intermediate objects
		MFnDagNode dagNode(dagpath, &status);
		if (dagNode.isIntermediateObject()) 
		{
			continue;
		}

		if ((dagpath.hasFn(MFn::kMesh))&&(dagpath.hasFn(MFn::kTransform)))
		{
			dagpath.push(dagpath.child(0));
		}

		if ((dagpath.hasFn(MFn::kMesh))&&(!dagpath.hasFn(MFn::kTransform)))
		{
			if (MTools::IsInstance(dagpath))
				continue;

			MFnDependencyNode depnode(dagpath.node());
			if (!norepeat.HasKey((int)MTools::GetMPtrBase(depnode)))
			{
				norepeat.SetValue((int)MTools::GetMPtrBase(depnode),1);
				objectcount++;
			}
		}
	}

	MObjectArray nodetrash;
	nodetrash.clear();

	std::vector<HashShader> newshaders;
	newshaders.clear();

	std::vector<HashShader> consolidatedshaders;
	consolidatedshaders.clear();
		
	MObjectArray consolidatedshadinggroups;
	consolidatedshadinggroups.clear();

	IntMap shadermap,shadinggroupmap;

	MTools::StartProgressBar(objectcount,"Consolidating shaders...");

	norepeat.Clear();

	// iterate through DAG
	for (i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;

		selection.getDagPath(i,dagpath);

		// skip over intermediate objects
		MFnDagNode dagNode(dagpath, &status);
		if (dagNode.isIntermediateObject()) 
		{
			continue;
		}

		if ((dagpath.hasFn(MFn::kMesh))&&(dagpath.hasFn(MFn::kTransform)))
		{
			dagpath.push(dagpath.child(0));
		}
		
		MFnDependencyNode depnode(dagpath.node());
		if (!norepeat.HasKey((int)MTools::GetMPtrBase(depnode)))
		{
			if ((dagpath.hasFn(MFn::kMesh))&&(!dagpath.hasFn(MFn::kTransform)))
			{
				if (MTools::IsInstance(dagpath))
					continue;
				
				MFnDependencyNode depnode(dagpath.node());
				MString objectname = depnode.name();
				const char *debug = objectname.asChar();
				
				MFnMesh fnmesh(dagpath);
				
				MObjectArray oldshadinggroups;
				oldshadinggroups.clear();
				
				MIntArray oldkeys;
				oldkeys.clear();
				
				fnmesh.getConnectedShaders(0,oldshadinggroups,oldkeys);
				
				if ((oldkeys.length()==0)||(oldshadinggroups.length()==0))
					continue;
				
				std::vector<int> newkeys;
				newkeys.clear();
				
				for (int i = 0; i < (int)oldkeys.length(); i++)
				{
					int shaderindex = oldkeys[i];
					
					if (shaderindex < 0)
					{
						MGlobal::displayError(MString("Invalid: Face with no shader. ")+objectname);
						return MS::kFailure;
					}
					
					MObject oldshadinggroup = oldshadinggroups[shaderindex];
					
					HashShader hashShader;
					MFnDependencyNode depnode(oldshadinggroup);
					
					int hashvalue;
					
					if (shadinggroupmap.HasKey((int)MTools::GetMPtrBase(depnode)))
					{
						shadinggroupmap.GetValue((int)MTools::GetMPtrBase(depnode),hashvalue);
					}
					else
					{
						CreateHashShader(oldshadinggroup,hashShader,nodetrash);
						hashvalue = hashShader.GetHashValue();
						shadinggroupmap.SetValue((int)MTools::GetMPtrBase(depnode),hashvalue);
					}
					
					if (shadermap.HasKey(hashvalue))
					{
						int reindex;
						shadermap.GetValue(hashvalue,reindex);
						newkeys.push_back(reindex);
					}
					else
					{
						consolidatedshaders.push_back(hashShader);
						int lastindex = consolidatedshaders.size()-1;
						MObject newShadingGroup;
						CreateShadingGroup(hashShader,newShadingGroup);
						consolidatedshadinggroups.append(newShadingGroup);
						shadermap.SetValue(hashvalue,lastindex);
						newkeys.push_back(lastindex);
					}
				}
				ConnectShadingGroups(dagpath,consolidatedshadinggroups,newkeys);
				MTools::StepProgressBar(1);	
			}
			norepeat.SetValue((int)MTools::GetMPtrBase(depnode),1);
		}
	}

	MTools::StopProgressBar();
	

	// trash old shaders
//	MTools::StartProgressBar(nodetrash.length(),"Deleting old shaders...");
//	for (i = 0; i < nodetrash.length(); i++)
//	{
//		MFnDependencyNode depnode(nodetrash[i]);
//		MGlobal::executeCommand(MString("delete ") + depnode.name());
//		dgModifier.doIt();
//		MTools::StepProgressBar(1);
//	}
//	MTools::StopProgressBar();
		
    return MS::kSuccess;
}


MStatus consolidateShaders::ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const std::vector<int> &shaderindicies)
{
	MStatus status;
	MDGModifier dgmodifier;

	////////////////////////////////////////////////////////////////////////////////////////
	// Create connections between Shading Groups and Polys

	if (dagpath.hasFn(MFn::kTransform))
		dagpath.push(dagpath.child(0));
	if (!dagpath.hasFn(MFn::kMesh))
		return MS::kFailure;

	MFnDependencyNode depnode(dagpath.node());
	MString meshname = depnode.name();

	MItMeshPolygon MItPoly(dagpath,MObject::kNullObj,&status);
	if (!status)
	{
		MGlobal::displayWarning("Could not apply poly iterator function set to dagpath.");
		return MS::kFailure;
	}

//	MTools::StartProgressBar(SGList.length(),MString("Assigning textures to ")+meshname+MString("..."));

	for (int nTexture = 0; nTexture < (int)SGList.length(); nTexture++)
	{
		MFnDependencyNode shaderdepnode(SGList[nTexture]);
		MString shadername = shaderdepnode.name();
		MString command = MString("sets -e -forceElement ") + shadername + MString(" ");

		MSelectionList polySelection;
		polySelection.clear();
		for ( MItPoly.reset(); !MItPoly.isDone(); MItPoly.next() )
		{
			if (shaderindicies[MItPoly.index()] < 0)// assign initialShadingGroup
			{
				continue;
			}
			else if (shaderindicies[MItPoly.index()] == nTexture)
			{
				char number[128];
				itoa(MItPoly.index(),number,10);
				command += dagpath.fullPathName() + MString(".f[" + MString(number) + MString("] "));
			}
		} // for poly iterator

//		MFnSet set(SGList[nTexture], &status);
//		if (!status)
//		{
//			MGlobal::displayWarning("Could not create shader set.");
//			return MS::kFailure;
//		}

		MGlobal::executeCommand(command);
		dgmodifier.doIt();

//		MTools::StepProgressBar(1);
	} // nTexture

//	MTools::StopProgressBar();

	return MStatus::kSuccess;
}

void consolidateShaders::CreateHashShader(MObject shadingGroup,HashShader &hashShader,MObjectArray &nodetrash)
{
	MStatus status;
	
	MFnDependencyNode depnode(shadingGroup);
	const char *debug = depnode.name().asChar();
	
	// store shading group for elimination later
	nodetrash.append(shadingGroup);
	
	// store shader name
	const char *shadername = depnode.name().asChar();
	strcpy(hashShader.m_name,shadername);
	
	MPlug fnPlug = depnode.findPlug("surfaceShader",&status);
	if (status)
	{
		MPlugArray connectedNodes;
		fnPlug.connectedTo(connectedNodes,true,false,&status);
		if (connectedNodes.length()==0)
			MGlobal::displayError("Failed to find: Surface Shader Node.");
		if ((status)&&(connectedNodes.length()>0))
		{
			MObject shadernode = connectedNodes[0].node();
			
			// store shader for elimination later
			nodetrash.append(shadernode);
			
			MFnBlinnShader fnBlinn(shadernode);
			MFnPhongShader fnPhong(shadernode);
			MFnLambertShader fnLambert(shadernode);
			
			MColor diffuse;
			
			if (shadernode.apiType() == MFn::kBlinn)
			{
				diffuse = fnBlinn.color(&status);
				hashShader.m_type = HashShader::kBLINN;
			}
			else if (shadernode.apiType() == MFn::kPhong)
			{
				diffuse = fnPhong.color(&status);
				hashShader.m_type = HashShader::kPHONG;
				
			}
			else if (shadernode.apiType() == MFn::kLambert)
			{
				diffuse = fnLambert.color(&status);
				hashShader.m_type = HashShader::kLAMBERT;
			}
			
			MFnDependencyNode depnode(shadernode);
			
			MPlug colorplug = depnode.findPlug("color",&status);
			if (status)
			{
				MPlugArray connectedNodes2;
				colorplug.connectedTo(connectedNodes2,true,false,&status);
				if ((status)&&(connectedNodes2.length() > 0))
				{
					MObject colornode = connectedNodes2[0].node();
					
					if (colornode.apiType() == MFn::kFileTexture)
					{							
						hashShader.m_color.m_type = ShaderChannel::kFILE;
						GetMap(colornode,&hashShader.m_color.m_maps[0],nodetrash);
					}
					else if (colornode.apiType() == MFn::kBlendColors)
					{
						hashShader.m_color.m_type = ShaderChannel::kBLENDER;
						
						MFnDependencyNode depnode(colornode);
						
						MPlug colorplugA = depnode.findPlug("color1",&status);
						
						MPlug colorplugB = depnode.findPlug("color2",&status);
						
						MPlug blenderplug = depnode.findPlug("blender",&status);
						
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
								GetMap(color1object,&hashShader.m_color.m_maps[0],nodetrash);
								GetMap(color2object,&hashShader.m_color.m_maps[1],nodetrash);
								GetMap(blenderobject,&hashShader.m_color.m_maps[2],nodetrash);
							}
						}
					}
				}
				else
				{
					hashShader.m_color.m_type = ShaderChannel::kCOLOR;
					hashShader.m_color.m_rgb = myColor(diffuse.r,diffuse.g,diffuse.b);
				}
			}
			else
			{
				hashShader.m_color.m_type = ShaderChannel::kCOLOR;
				hashShader.m_color.m_rgb = myColor(diffuse.r,diffuse.g,diffuse.b);
			}
		}
	}
}

int consolidateShaders::GetMap(MObject filenode,myMap *pMap,MObjectArray &nodetrash)
{
	MStatus status;

	MFnDependencyNode depnode(filenode);

	MPlug pathnameplug = depnode.findPlug("fileTextureName",&status);

	if (!status)
		return 0;

	MString pathname;
	status = pathnameplug.getValue(pathname);
	strcpy(pMap->m_pathname,pathname.asChar());

	GetPlacementNodeInfo(filenode,pMap,nodetrash);

	// store nodes for elimination later
	nodetrash.append(filenode);

	return 1;
}

int consolidateShaders::GetPlacementNodeInfo(MObject filenode,myMap *pMap,MObjectArray &nodetrash)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode depnode(filenode);

	MPlug uvCoordPlug = depnode.findPlug("uvCoord",&status);

	if (!status)
		return 0;

	MPlugArray connectedNodes;
	uvCoordPlug.connectedTo(connectedNodes,true,false,&status);

	if (!status)
		return 0;

	if (connectedNodes.length()==0)
		return 0;

	MObject placementnode = connectedNodes[0].node();

	MFnDependencyNode placementDepNode(placementnode);

	MPlug repeatUplug = placementDepNode.findPlug("repeatU",&status);
	if (!status)
		return 0;

	MPlug repeatVplug = placementDepNode.findPlug("repeatV",&status);
	if (!status)
		return 0;

	double repeatU,repeatV;

	status = repeatUplug.getValue(repeatU);
	status = repeatVplug.getValue(repeatV);

	pMap->m_ru = (float)repeatU;
	pMap->m_rv = (float)repeatV;


	MPlug wrapUplug = placementDepNode.findPlug("wrapU",&status);
	if (!status)
		return 0;

	MPlug wrapVplug = placementDepNode.findPlug("wrapV",&status);
	if (!status)
		return 0;

	status = wrapUplug.getValue(pMap->m_wrapu);
	status = wrapVplug.getValue(pMap->m_wrapv);

	// store nodes for elimination later
	nodetrash.append(placementnode);

	return 1;
}

int consolidateShaders::CreateShadingGroup(HashShader &shader,MObject &newShadingGroup)
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MFnSet fnSet;
	MFnBlinnShader blinn;
	MFnPhongShader phong;
	MFnLambertShader lambert;
	MColor color;

	status = selection.add(MObject::kNullObj);

	newShadingGroup = fnSet.create(selection, MFnSet::kRenderableOnly, &status);
	if (!status) 
		return 0;

	dgModifier.doIt();
	
	MString shadername = shader.m_name;
	MString shadingGroupName = shader.m_name;

	status = dgModifier.renameNode(newShadingGroup,shadingGroupName);
	
	MObject shaderobject;
	MString newshadername;

	if (shader.m_type == HashShader::kBLINN)
	{
		shaderobject = blinn.create(TRUE,&status);
		if (!status)
			return 0;

		dgModifier.doIt();

		MFnBlinnShader blinnS(shaderobject);

		color = MColor(MColor::kRGB,shader.m_color.m_rgb.r,shader.m_color.m_rgb.g,shader.m_color.m_rgb.b);

		status = blinnS.setColor(color);
		if (!status)
			MGlobal::displayError("Unable to set shader color.");

		MColor black = MColor(MColor::kRGB,0.0,0.0,0.0);
		
		status = blinnS.setSpecularColor(black);
		if (!status)
			MGlobal::displayError("Unable to set shader specular color.");
	}
	else if (shader.m_type == HashShader::kPHONG)
	{
		shaderobject = phong.create(TRUE,&status);
		if (!status)
			return 0;

		dgModifier.doIt();

		MFnPhongShader phongS(shaderobject);

		color = MColor(MColor::kRGB,shader.m_color.m_rgb.r,shader.m_color.m_rgb.g,shader.m_color.m_rgb.b);

		status = phongS.setColor(color);
		if (!status)
			MGlobal::displayError("Unable to set shader color.");

		MColor black = MColor(MColor::kRGB,0.0,0.0,0.0);
		
		status = phongS.setSpecularColor(black);
		if (!status)
			MGlobal::displayError("Unable to set shader specular color.");
	}
	else if (shader.m_type == HashShader::kLAMBERT)
	{
		shaderobject = lambert.create(TRUE,&status);
		if (!status)
			return 0;

		dgModifier.doIt();

		MFnLambertShader lambertS(shaderobject);

		color = MColor(MColor::kRGB,shader.m_color.m_rgb.r,shader.m_color.m_rgb.g,shader.m_color.m_rgb.b);

		status = lambertS.setColor(color);
		if (!status)
			MGlobal::displayError("Unable to set shader color.");
	}

	dgModifier.doIt();
		
	status = dgModifier.renameNode(shaderobject,shadername);
	if (status != MS::kSuccess)
		return 0;

	dgModifier.doIt();

	MFnSet fnShadingGroup(newShadingGroup);

	MFnDependencyNode depnode(shaderobject);

	newshadername = depnode.name();

	MGlobal::executeCommand (
		MString ("defaultNavigation -force true -connectToExisting -source ") +
		newshadername +
		MString (" -destination ") +
		fnShadingGroup.name() );

	dgModifier.doIt();

	if (shader.m_color.m_type == ShaderChannel::kFILE)
	{
		myMap *map = &shader.m_color.m_maps[0];
		
		LinkUpMapNode(
			newshadername,".color",
			MString(map->m_pathname),
			map->m_ru,
			map->m_rv,
			map->m_wrapu,
			map->m_wrapv,
			false);
	}
	else if (shader.m_color.m_type == ShaderChannel::kBLENDER)
	{
		myMap *map0 = &shader.m_color.m_maps[0];
		myMap *map1 = &shader.m_color.m_maps[1];
		myMap *map2 = &shader.m_color.m_maps[2];
		
		MString blendNode;
		MGlobal::executeCommand (MString ("shadingNode -asUtility blendColors"),blendNode );
		MGlobal::executeCommand (MString ("connectAttr ") + blendNode + MString (".output ") + newshadername + MString (".color") );
		
		LinkUpMapNode(
			blendNode,".color1",
			MString(map0->m_pathname),
			map0->m_ru,
			map0->m_rv,
			map0->m_wrapu,
			map0->m_wrapv,
			false);
		
		LinkUpMapNode(
			blendNode,".color2",
			MString(map1->m_pathname),
			map1->m_ru,
			map1->m_rv,
			map1->m_wrapu,
			map1->m_wrapv,
			false);
		
		LinkUpMapNode(
			blendNode,".blender",
			MString(map2->m_pathname),
			map2->m_ru,
			map2->m_rv,
			map2->m_wrapu,
			map2->m_wrapv,
			true);
	}

	return 1;
}


int consolidateShaders::LinkUpMapNode(MString shaderNode,
								 MString attrName,
								 MString mapName,
								 double ru,
								 double rv,
								 int wrapu,
								 int wrapv,
								 int useAlpha)
{
	MString mapNode;

	const char *debug = mapName.asChar();

	MGlobal::executeCommand (
			MString ("shadingNode -asTexture file"),
			mapNode );

	if (!useAlpha)
	{
		MGlobal::executeCommand (
				MString ("connectAttr ") + mapNode + MString (".outColor ") +
				shaderNode + attrName );
	}
	else
	{
		MGlobal::executeCommand (
				MString ("connectAttr ") + mapNode + MString (".outAlpha ") +
				shaderNode + attrName );
	}

	MGlobal::executeCommand (
			MString ("setAttr -type \"string\" ") +
			mapNode + MString (".fileTextureName \"") +
			mapName + MString ("\"") );

	CreatePlacementNode(mapNode,ru,rv,wrapu,wrapv);

	return 1;
}

int consolidateShaders::CreatePlacementNode(MString filenode,double ru,double rv,int wrapu,int wrapv)
{
	MString placementnode;

	MGlobal::executeCommand (
		MString ("shadingNode -asUtility place2dTexture"),placementnode );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".coverage ") +
			filenode + MString (".coverage") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".translateFrame ") +
			filenode + MString (".translateFrame") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".rotateFrame ") +
			filenode + MString (".rotateFrame") );

//	MGlobal::executeCommand (
//		MString ("connectAttr ") + placementnode + MString (".mirror ") +
//			filenode + MString (".mirror") );

	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".mirrorU ") + filenode + MString (".mirrorU") );

	MGlobal::executeCommand(MString ("connectAttr ") + placementnode + MString (".mirrorV ") + filenode + MString (".mirrorV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".stagger ") +
			filenode + MString (".stagger") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".wrapU ") +
			filenode + MString (".wrapU") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".wrapV ") +
			filenode + MString (".wrapV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".repeatUV ") +
			filenode + MString (".repeatUV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".offset ") +
			filenode + MString (".offset") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".rotateUV ") +
			filenode + MString (".rotateUV") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".outUV ") +
			filenode + MString (".uv") );

	MGlobal::executeCommand (
		MString ("connectAttr ") + placementnode + MString (".outUvFilterSize ") +
			filenode + MString (".uvFilterSize") );

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".repeatU ") + ru);

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".repeatV ") + rv);

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".wrapU ") + wrapu);

	MGlobal::executeCommand (
			MString ("setAttr ") +  placementnode + MString (".wrapV ") + wrapv);

	return 1;
}
