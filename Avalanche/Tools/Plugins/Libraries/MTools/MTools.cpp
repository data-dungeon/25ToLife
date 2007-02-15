
#include "StdAfx.h"
#include "MTools.h"
#include "IntHash.h"

//#define MAYA40

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	these functions were written to aid in creating and editing reference layers and reference groups
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MTools::MakeReferenceLayer(MDagPath dagpath,const MObjectArray &objects,MDagPathArray &results)
{
	MDGModifier dgModifier;
	MStatus status;
	MString result;
	MObject object = dagpath.node();
	MObjectArray newobjects = objects;

	MDagPath refdagpath;
	if (!GetReferenceObject(dagpath,refdagpath))
		return 0;
	
	MFnDependencyNode fnNode(refdagpath.node());
	MString objectname = fnNode.name() + MString("_ref");

	if ((object != refdagpath.node())&&(objects.length()==0))
		newobjects.append(object);

	MGlobal::executeCommand(MString("createDisplayLayer -empty -name \"") + objectname + MString("\""),result);
	dgModifier.doIt();

	if (result != objectname)
	{
		MGlobal::executeCommand(MString("delete ") + result);
		dgModifier.doIt();
		return 0;
	}	

	MFnEnumAttribute enumattr;
	MObject enumattrobject = enumattr.create("LayerType","LayerType",1,&status);
	enumattr.addField("default",0);
	enumattr.addField("reference",1);

	MFnTypedAttribute typedattr;
	MObject typedattrobject = typedattr.create("ShapeReference","ShapeReference",MFnData::kString,&status);

	MObject layer = GetMObject(result);

	status = dgModifier.addAttribute(layer,enumattrobject,MFnDependencyNode::kLocalDynamicAttr);
	dgModifier.doIt();
	status = dgModifier.addAttribute(layer,typedattrobject,MFnDependencyNode::kLocalDynamicAttr);
	dgModifier.doIt();
	if (status)
	{
		MPlug mplug(layer,typedattrobject);
		status = mplug.setValue(fnNode.name());
		if (!status)
		{
			MGlobal::displayError("Couldn't set ShapeReference attribute on layer.");
		}
	}

	dgModifier.doIt();

	if (newobjects.length()>0)
	{
		MDagPath dagpath1;
		MFnDagNode dagnode(refdagpath.node());
		dagnode.getPath(dagpath1);

		MDagPathArray dagpatharray;
		dagpatharray.clear();
		for (unsigned i = 0; i < newobjects.length(); i++)
		{
			MDagPath dagpath;
			MFnDagNode dagnode(newobjects[i]);
			dagnode.getPath(dagpath);
			dagpatharray.append(dagpath);
		}
		
		MTools::ReplaceInstances(dagpath1,dagpatharray,results);

		MStringArray instances;

		MTools::ConvertDagPathsToNames(results,instances);

		MFnDependencyNode fnNode(layer);

		MString result = fnNode.name();

		MTools::AddNodesToDisplayLayer(instances,result);
	}
	return 1;
}

int MTools::MakeReferenceGroup(MDagPath dagpath,const MObjectArray &objects,MDagPath &result)
{
	MDGModifier dgModifier;
	MString result0;
	MStatus status;
	MObject object = dagpath.node();
	MObjectArray newobjects = objects;

	MDagPath refdagpath;
	if (!GetReferenceObject(dagpath,refdagpath))
		return 0;

	MFnDependencyNode fnNode(refdagpath.node());
	MString groupname = fnNode.name() + MString("_ref");

	groupname = groupname.toUpperCase();

	if ((object != refdagpath.node())&&(objects.length()==0))
		newobjects.append(object);

	MDagPathArray results;

	if (newobjects.length()>0)
	{
		MDagPath dagpath1;
		MFnDagNode dagnode(refdagpath.node());
		dagnode.getPath(dagpath1);

		MDagPathArray dagpatharray;
		dagpatharray.clear();
		for (unsigned i = 0; i < newobjects.length(); i++)
		{
			MDagPath dagpath;
			MFnDagNode dagnode(newobjects[i]);
			dagnode.getPath(dagpath);
			dagpatharray.append(dagpath);
		}
		
		MTools::ReplaceInstances(dagpath1,dagpatharray,results);
	}	

	MString command = MString("group -name ") + MString(groupname) + MString(" -world");

	if (results.length()>0)
	{
		for (unsigned i = 0; i < results.length(); i++)
		{
			MFnDependencyNode fnNode(results[i].node());
			command += MString(" ") + fnNode.name();
		}
	}
	else
	{
		command += MString(" -empty");
	}

	MGlobal::executeCommand(command,result0);
	dgModifier.doIt();

	if (result0 != groupname)
	{
		return 0;
	}
	
	MFnEnumAttribute enumattr;
	MObject enumattrobject = enumattr.create("GroupType","GroupType",1,&status);
	enumattr.addField("default",0);
	enumattr.addField("reference",1);

	MFnTypedAttribute typedattr;
	MObject typedattrobject = typedattr.create("ShapeReference","ShapeReference",MFnData::kString,&status);

	MDagPath groupdagpath;
	if (!GetMDagPath(result0,groupdagpath))
		return 0;

	status = dgModifier.addAttribute(groupdagpath.node(),enumattrobject,MFnDependencyNode::kLocalDynamicAttr);
	dgModifier.doIt();
	status = dgModifier.addAttribute(groupdagpath.node(),typedattrobject,MFnDependencyNode::kLocalDynamicAttr);
	dgModifier.doIt();
	if (status)
	{
		MPlug mplug(groupdagpath.node(),typedattrobject);
		status = mplug.setValue(fnNode.name());
		if (!status)
		{
			MGlobal::displayError("Couldn't set ShapeReference attribute on layer.");
		}
	}

	dgModifier.doIt();

	result = groupdagpath;

	return 1;
}

bool MTools::GetShapeReferenceFromLayer(MObject layer,MString &reference)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode fnNode(layer);

	const char *debug = fnNode.name().asChar();

	MPlug plug = fnNode.findPlug("ShapeReference",&status);
	if (status)
	{
		status = plug.getValue(reference);
		if (status)
			return true;
	}
	return 0;
}

bool MTools::GetShapeReferenceFromReferenceGroup(MObject igroup,MString &reference)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode fnNode(igroup);

	const char *debug = fnNode.name().asChar();

	MPlug plug = fnNode.findPlug("ShapeReference",&status);
	if (status)
	{
		status = plug.getValue(reference);
		if (status)
			return true;
	}
	return 0;
}

int MTools::IsRefLayer(MObject layer)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode fnNode(layer);

	const char *debug = fnNode.name().asChar();

	MPlug plug = fnNode.findPlug("LayerType",&status);
	if (status)
	{
		int val;
		status = plug.getValue(val);
		if (status)
		{
			if (val == 1)
				return 1;
		}
	}
	return 0;
}

int MTools::IsRefGroup(MObject group)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode fnNode(group);

	const char *debug = fnNode.name().asChar();

	MPlug plug = fnNode.findPlug("GroupType",&status);
	if (status)
	{
		int val;
		status = plug.getValue(val);
		if (status)
		{
			if (val == 1)
				return 1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	these functions help with manipulating Maya4.5 notes field
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MTools::InsertKeyValueToNotes(MObject object,MString key,MString value)
{

	MString oldnote,newnote = "";
	GetNote(object,oldnote);

	MString temp = "";
	for (unsigned i = 0; i < oldnote.length(); i++)
	{
		if (oldnote.substring(i,i) != "\n")
			temp += oldnote.substring(i,i);
	}

	MStringArray notelines;
	temp.split('\r',notelines);

	const char *debugkey = key.asChar();
	const char *debugvalue = value.asChar();

	int idx = FindLine("BEGIN OBJECTATTRIBUTES",notelines);
	if (idx<0)
	{
		notelines.append("BEGIN OBJECTATTRIBUTES");
		notelines.append(key + MString("=") + value);
		notelines.append("END OBJECTATTRIBUTES");
		
		for (i = 0; i < notelines.length(); i++)		
			newnote += notelines[i] + MString("\r\n");
		CreateOrReplaceNote(object,newnote);

		return 1;
	}

	const char *pKey = key.asChar();

	MString oldvalue;
	int keyidx = FindKey(key,notelines,oldvalue);

	if (keyidx < 0)
	{
		notelines.insert(key + MString("=") + value,idx+1);
	}
	else
	{
		notelines[keyidx] = key + MString("=") + value;
		const char *debug = notelines[keyidx].asChar();
	}
	
	for (i = 0; i < notelines.length(); i++)
		newnote += notelines[i] + MString("\r\n");
	
	const char*debug = newnote.asChar();

	CreateOrReplaceNote(object,newnote);
	
	return 1;
}

int MTools::DeleteKeyValueFromNotes(MObject object,MString key)
{

	MString oldnote,newnote = "";
	GetNote(object,oldnote);

	MString temp = "";
	for (unsigned i = 0; i < oldnote.length(); i++)
	{
		if (oldnote.substring(i,i) != "\n")
			temp += oldnote.substring(i,i);
	}

	MStringArray notelines;
	temp.split('\r',notelines);

	const char *pKey = key.asChar();

	MString oldvalue;
	int keyidx = FindKey(key,notelines,oldvalue);

	if (keyidx >= 0)
	{
		notelines.remove(keyidx);
	}
	else
		return 0;

	for (i = 0; i < notelines.length(); i++)		
		newnote += notelines[i] + MString("\r\n");
	CreateOrReplaceNote(object,newnote);
	
	return 1;
}

int MTools::GetKeyValueFromNotes(MObject object,MString key,MString &value)
{

	MString oldnote,newnote = "";
	GetNote(object,oldnote);

	MString temp = "";
	for (unsigned i = 0; i < oldnote.length(); i++)
	{
		if (oldnote.substring(i,i) != "\n")
			temp += oldnote.substring(i,i);
	}

	MStringArray notelines;
	temp.split('\r',notelines);

	const char *pKey = key.asChar();

	MString oldvalue;
	int keyidx = FindKey(key,notelines,oldvalue);

	if (keyidx >= 0)
	{
		value = oldvalue;
	}
	else
		return 0;
	
	return 1;
}

void MTools::ListKeysFromNotes(MObject object,MStringArray &results)
{
	results.clear();

	MString oldnote,newnote = "";
	GetNote(object,oldnote);

	MString temp = "";
	for (unsigned i = 0; i < oldnote.length(); i++)
	{
		if (oldnote.substring(i,i) != "\n")
			temp += oldnote.substring(i,i);
	}

	MStringArray notelines;
	temp.split('\r',notelines);

	int idx = FindLine("BEGIN OBJECTATTRIBUTES",notelines);

	if (idx < 0)
		return;

	for (i = idx; i < notelines.length(); i++)
	{
		if (notelines[i] == "END OBJECTATTRIBUTES")
			return;

		MStringArray splitargs;
		notelines[i].split('=',splitargs);

		if (splitargs.length() != 2)
			continue;

		results.append(splitargs[0]);
	}
}

int MTools::FindKey(MString key,MStringArray &notelines,MString &value)
{
	const char *pKey = key.asChar();

	for (unsigned i = 0; i < notelines.length(); i++)
	{
		const char *debug = notelines[i].asChar();

		MStringArray noteparse;
		notelines[i].split('=',noteparse);

		if (noteparse.length()<=1)
			continue;

		if (key == noteparse[0])
		{
			value = noteparse[1];
			return i;
		}
	}
	return -1;
}

int MTools::FindLine(MString line,MStringArray &notelines)
{
	for (unsigned i = 0; i < notelines.length(); i++)
	{
		if (line == notelines[i])
		{
			return i;
			break;
		}	
	}
	return -1;
}

int MTools::GetNote(MObject object, MString &note)
{
	MStatus status;
	MFnDependencyNode fnNode(object);

	MPlug plug = fnNode.findPlug("notes",&status);
	if (!status)
		return 0;

	plug.getValue(note);	

	return 1;
}

int MTools::CreateOrReplaceNote(MObject object,MString note)
{
	MDGModifier dgModifier;
	MStatus status;
	MString command;
	MFnDependencyNode fnNode(object);

	const char *debug = note.asChar();

	MPlug plug = fnNode.findPlug("notes",&status);
	if (!status)
	{
		command = MString("addAttr -dt \"string\" -ln \"notes\" -sn \"nts\" ") + fnNode.name();
		MGlobal::executeCommand(command);

		plug = fnNode.findPlug("notes",&status);
		if (!status)
			return 0;
	}

	status = plug.setValue(note);
	if (!status)
		return 0;

	dgModifier.doIt();

	MGlobal::executeCommand("autoUpdateAttrEd");
	dgModifier.doIt();

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	these functions help with manipulating Maya4.5 notes field
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// this function tests to see if names are similar
int MTools::IsNodeNameSimilar(MString name0,MString name1)
{
	MString newname0 = ExtractName(name0);
	MString newname1 = ExtractName(name1);
	if (strcmp(newname0.asChar(),newname1.asChar())==0)
		return 1;
	return 0;
}

//	this function walks backwards through numbers on the end of a string and stops at the first alpha character
// it then returns the name previous and including that character
MString MTools::ExtractName(MString name)
{
	const char *pTemp = name.asChar();
	char temp[256];

	unsigned len = strlen(pTemp);
	unsigned idx = 0;
	for (unsigned j = 0; j < len; j++)
	{
		if ((pTemp[len-j-1] < '0') || (pTemp[len-j-1] > '9'))
		{
			idx = len-j;
			break;
		}
	}

	for (j = 0; j < idx; j++)
		temp[j] = pTemp[j];

	temp[j] = '\0';

	return MString(temp);
}

MString MTools::ExtractName2(MString name)
{
	const char *pTemp = name.asChar();
	char temp[256];

	unsigned len = strlen(pTemp);
	unsigned idx = 0;
	for (unsigned j = 0; j < len; j++)
	{
		if ((pTemp[j] > '0') && (pTemp[j] < '9'))
		{
			idx = j;
			break;
		}

	}
	if (j == len)
	{
		idx = len;
	}

	for (j = 0; j < idx; j++)
		temp[j] = pTemp[j];

	temp[j] = '\0';

	return MString(temp);
}

// this function updates all instances
// instance chain is choosen by dagpath
int MTools::UpdateInstances(MDagPath dagpath,bool usename)
{
	MDGModifier dgModifier;
	MDagModifier dagModifier;
	MStatus status;

	if (!usename)//if not flagged for similar lookin instances
		if (!IsInstance(dagpath))//if not a conventional maya instance
			return 0;//bail

// get first shape from dagpath
	MDagPath firstshapedagpath;
	if (!GetFirstShape(dagpath,firstshapedagpath,true))
		return 0;

	MDagPathArray dagpatharray;

// usename is a flag that looks for instances by name similarity
	if (usename)
	{
		MFnDependencyNode depnode(firstshapedagpath.node());
		GetDagPathsOfSimilarName(depnode.name(),dagpatharray);
	}
//	otherwise just use mayas conventional instancing 
	else
	{
		dagpath.getAllPathsTo(firstshapedagpath.node(),dagpatharray);
	}

	MDagPath masterdagpath;// this becomes the sourcepath to update instances from
	MDagPathArray instances;
	instances.clear();

//	loop through all the instances collected from above
	for (unsigned i = 0; i < dagpatharray.length(); i++)
	{
		MDagPath instancepath;
//		pops dagpath until it gets the top transform
		if (!GetTransformRoot(dagpatharray[i],instancepath))
			return 0;

		MFnDependencyNode depnode(instancepath.node());
		if (depnode.name() == "References")
		{
//			pops dagpath until it gets the transform under the 'References' group
			if (!GetTransformAfterReference(dagpatharray[i],masterdagpath))
				return 0;
		}
		else
		{
			MDagPath refdagpath;
			if (!MTools::GetReferenceObject(dagpatharray[i],refdagpath,usename))
				return 0;

			MFnDependencyNode refobjectdepnode(refdagpath.node());
			MString refobjectname = refobjectdepnode.name();

			MDagPath founddagpath;
//			this function iterates up and down the starting at a dagpath looking for a transform named similarly to name passed in
			if (FindSimilarName(dagpatharray[i],refobjectname,founddagpath))
				instances.append(founddagpath);
		}
	}

	for (i = 0; i < instances.length(); i++)
	{
		MDagPath instancepath = instances[i];
		MObject parent = MObject::kNullObj;
		MFnDagNode dagnode(instancepath.node());
		if (dagnode.parentCount(&status)>0)
		{
			MObject temp = dagnode.parent(0,&status);
			if (temp.hasFn(MFn::kTransform))
			{
				parent = temp;
			}
		}

//		create new instance
		MDagPath newinstancedagpath;
		if (!usename)
		{
			if (!InstanceObject(masterdagpath,newinstancedagpath,true,false,parent))
				return 0;
		}
		else
		{
			if (!DuplicateObject(masterdagpath,newinstancedagpath,false,parent))
				return 0;
		}

//		heirarchical copy transforms from instance
		CopyHierarchicalTransforms(instancepath,newinstancedagpath,true,true,true);

//		heirarchical copy attributes from instance
		CopyHierarchicalAttributes(instancepath,newinstancedagpath);//get attributes from instance

		dgModifier.doIt();//just in case

		MFnDependencyNode depnode(instancepath.node());
		MString oldinstancename = depnode.name();

//		delete old instance
		MGlobal::executeCommand(MString("delete ")+instancepath.fullPathName());
		dgModifier.doIt();

//		give new instance old name
		dgModifier.renameNode(newinstancedagpath.node(),oldinstancename);
		dgModifier.doIt();
	}
	return 1;
}

int MTools::GetReferenceObject(MDagPath dagpath,MDagPath &result,bool usename)
{
	MDGModifier dgModifier;

	MDagPathArray dagpatharray;

// usename is a flag that looks for instances by name similarity
	if (usename)
	{
		MDagPath firstshapedagpath;
		if (!GetFirstShape(dagpath,firstshapedagpath,false))
			return 0;
		MFnDependencyNode depnode(firstshapedagpath.node());
		GetDagPathsOfSimilarName(depnode.name(),dagpatharray);
	}
//	otherwise just use mayas conventional instancing 
	else
	{
		MDagPath firstshapedagpath;
		if(!GetFirstShape(dagpath,firstshapedagpath,true))
			return 0;
		dagpath.getAllPathsTo(firstshapedagpath.node(),dagpatharray);	
	}

	MDagPath instancedagpath;

	for (unsigned i = 0; i < dagpatharray.length(); i++)
	{
//		pops dagpath until it gets the top transform
		if (!GetTransformRoot(dagpatharray[i],instancedagpath))
			return 0;

		MFnDependencyNode depnode(instancedagpath.node());
		if (depnode.name() == "References")
		{
//			pops dagpath until it gets the transform under the 'References' group
			if (!GetTransformAfterReference(dagpatharray[i],result))
				return 0;
			return 1;
		}
	}
	return 0;
}

// Returns first instance not belonging to the "References" folder
int MTools::GetNonReferenceInstance(MDagPath dagpath,MDagPath &result)
{
	MDGModifier dgModifier;

	MDagPath firstshapedagpath;
	if(!GetFirstShape(dagpath,firstshapedagpath,true))
		return 0;

	MDagPathArray dagpatharray;

	dagpath.getAllPathsTo(firstshapedagpath.node(),dagpatharray);

	for (unsigned i = 0; i < dagpatharray.length(); i++)
	{
		MDagPath instancedagpath;
		if (!GetTransformRoot(dagpatharray[i],instancedagpath))
			return 0;

		MFnDependencyNode fnNode(instancedagpath.node());
		if (fnNode.name() != "References")
		{
			if(!GetTransformAfterReference(dagpatharray[i],result))
				return 0;
			return 1;
		}
	}
	return 0;
	
}

int MTools::ReplaceInstances(MDagPath sourcedagpath,MDagPathArray &dests,MDagPathArray &results,bool copy,bool removeold,bool uniquenames)
{
	MStatus status;
	MDGModifier dgModifier;
	results.clear();

	MDagPath refdagpath;
	if (!GetReferenceObject(sourcedagpath,refdagpath))
		return 0;

	MDagPath instancedagpath,newinstancedagpath;
	
	// loop through destination instance
	for (unsigned i = 0; i < dests.length(); i++)
	{
		instancedagpath = dests[i];

		MObject parent = MObject::kNullObj;
//		if destination instance has parent then new instance or copy gets
//		parented to the destination instance parent
		MFnDagNode dagnode(instancedagpath.node());
		if (dagnode.parentCount(&status)>0)
		{
			MObject temp = dagnode.parent(0,&status);
			if (temp.hasFn(MFn::kTransform))
			{
				parent = temp;
			}
		}	
		
		if (!copy)
		{
//			instance with unique transforms
			if(!InstanceObject(refdagpath,newinstancedagpath,true,false,parent))
				return 0;
		}
		else
		{
//			copy object
			if(!DuplicateObject(refdagpath,newinstancedagpath,false,parent,uniquenames))
				return 0;
		}

		MDagPath skeletonroot;
		if (ExploreChildren(MFn::kJoint,newinstancedagpath,skeletonroot))
		{
			MObjectArray characterobjects;
			if (FindCharacters(instancedagpath,characterobjects))
			{
				CopyCharacter(characterobjects[0],skeletonroot);
			}
		}

		results.append(newinstancedagpath);

		CopyTransform(instancedagpath,newinstancedagpath,true,true,true);// just immediate transform
		CopyHierarchicalAttributes(instancedagpath,newinstancedagpath);// get attributes from instance

		if (removeold)
		{
			MGlobal::executeCommand(MString("delete ")+instancedagpath.fullPathName());
			dgModifier.doIt();
		}
	}
	return 1;
}

void MTools::CopyHierarchicalAttributes(MDagPath sourcedagpath, MDagPath destdagpath)
{
	MFnDagNode dagnode(sourcedagpath.node());

	if (!dagnode.isIntermediateObject())
	{
		CopyAttributes(sourcedagpath,destdagpath);
	}

	unsigned numChildren = sourcedagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath sourcechilddagpath(sourcedagpath);
		MDagPath destchilddagpath(destdagpath);

		sourcechilddagpath.push(sourcedagpath.child(i));
		destchilddagpath.push(destdagpath.child(i));

		CopyHierarchicalAttributes(sourcechilddagpath,destchilddagpath);
	}
}

MString MTools::GetShortName(MObject object,MString longname)
{
	MDGModifier dgModifier;
	MStatus status;

	MFnDependencyNode fnNode(object);

#ifdef MAYA40
	MStringArray result;
	MGlobal::executeCommand(MString("listAttr ") + fnNode.name() + MString(""),result);
	
	MStringArray result2;
	MGlobal::executeCommand(MString("listAttr -sn ") + fnNode.name() + MString(""),result2);

	for (unsigned i = 0; i < result.length(); i++)
	{
		const char *debug = result[i].asChar();

		if (longname == result[i])
			return result2[i];
	}
#else
	MPlug plug = fnNode.findPlug(longname,&status);
	if (status)
	{
		MFnAttribute attr(plug.attribute());
		return attr.shortName();
	}

#endif
	return "";
}

void MTools::CopyAttributes(MDagPath sourcedagpath, MDagPath destdagpath)
{
	MStatus status;
	MDGModifier dgModifier;
		
	MFnDependencyNode srcDepNode(sourcedagpath.node());
	MFnDependencyNode dstDepNode(destdagpath.node());

	MString sourcefullpath = sourcedagpath.fullPathName();
	const char *debug = sourcefullpath.asChar();

	MStringArray result;
	MGlobal::executeCommand(MString("listAttr -ud ")+sourcedagpath.fullPathName(),result);
	
	int numAttributes = result.length();

	for (int i = 0; i < numAttributes; i++)
	{
		MPlug srcPlug = srcDepNode.findPlug(result[i],&status);

		if (!status) continue;

		MObject srcattrobject = srcPlug.attribute(&status);

		if (!status) continue;

		const char *debug = result[i].asChar();

		MPlug dstPlug = dstDepNode.findPlug(result[i],&status);

		bool attributealreadyexists = false;
		if (status)
			attributealreadyexists = true;

		MFn::Type attrtype = srcattrobject.apiType();

		if (attrtype == MFn::kNumericAttribute)
		{
			MFnNumericAttribute srcattr(srcattrobject);

			MFnNumericData::Type numtype = srcattr.unitType();

			MString longname = srcattr.name();

			if (numtype == MFnNumericData::kBoolean)
			{
				bool val;
				status  = srcPlug.getValue(val);
				if (!status)
					continue;
				int iskeyable = srcPlug.isKeyable();

				if (!attributealreadyexists)
				{
					MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"bool\" ")+ destdagpath.fullPathName());
					dgModifier.doIt();

					if (iskeyable)
					{
						MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
						dgModifier.doIt();
					}
				}
				if (val)
					MGlobal::executeCommand(MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" true"));
				else
					MGlobal::executeCommand(MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" false"));
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);

				dgModifier.doIt();
			}
			else if (numtype == MFnNumericData::kShort)
			{
				short val;
				status  = srcPlug.getValue(val);
				if (!status)
					continue;
				int iskeyable = srcPlug.isKeyable();

				MFnNumericAttribute srcattr(srcattrobject);

				MString longname = srcattr.name();

				if (!attributealreadyexists)
				{

					MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"short\" ")+ destdagpath.fullPathName());
					dgModifier.doIt();

					if (iskeyable)
					{
						MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
						dgModifier.doIt();
					}
				}
				char temp[128];
				itoa(val,temp,10);
				MString command = MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" ") + MString(temp);
				MGlobal::executeCommand(command);
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);

				dgModifier.doIt();
			}
			else if (numtype == MFnNumericData::kLong)
			{
				int val;
				status  = srcPlug.getValue(val);
				if (!status)
					continue;
				int iskeyable = srcPlug.isKeyable();

				MFnNumericAttribute srcattr(srcattrobject);

				MString longname = srcattr.name();

				if (!attributealreadyexists)
				{
					MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"long\" ")+ destdagpath.fullPathName());
					dgModifier.doIt();

					if (iskeyable)
					{
						MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
						dgModifier.doIt();
					}
				}
				char temp[128];
				itoa(val,temp,10);
				MString command = MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" ") + MString(temp);
				MGlobal::executeCommand(command);
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);
				dgModifier.doIt();
			}
			else if (numtype == MFnNumericData::kFloat)
			{
				float val;
				status  = srcPlug.getValue(val);
				if (!status)
					continue;
				int iskeyable = srcPlug.isKeyable();

				MFnNumericAttribute srcattr(srcattrobject);

				MString longname = srcattr.name();

				if (!attributealreadyexists)
				{
					MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"float\" ")+ destdagpath.fullPathName());
					dgModifier.doIt();

					if (iskeyable)
					{
						MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
						dgModifier.doIt();
					}
				}
				char temp[128];
				sprintf(temp,"%f",val);
				MString command = MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" ") + MString(temp);
				MGlobal::executeCommand(command);
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);
				dgModifier.doIt();
			}
			else if (numtype == MFnNumericData::kDouble)
			{
				double val;
				status  = srcPlug.getValue(val);
				if (!status)
					continue;
				int iskeyable = srcPlug.isKeyable();

				if (!attributealreadyexists)
				{
					MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"double\" ")+ destdagpath.fullPathName());
					dgModifier.doIt();

					if (iskeyable)
					{
						MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
						dgModifier.doIt();
					}
				}
				char temp[128];
				sprintf(temp,"%f",val);
				MString command = MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" ") + MString(temp);
				const char *debug = command.asChar();
				MGlobal::executeCommand(command);
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);
				dgModifier.doIt();
			}
		}
		else if (attrtype == MFn::kTypedAttribute)
		{
			MString val;
			status  = srcPlug.getValue(val);
			if (!status)
				continue;
			int iskeyable = srcPlug.isKeyable();

			MFnTypedAttribute srcattr(srcattrobject);

			MString longname = srcattr.name();

			if (!attributealreadyexists)
			{
				MString command = MString("addAttr -ln ") + longname + MString(" -dt \"string\" ")+ destdagpath.fullPathName();
				const char*debug = command.asChar();
				MGlobal::executeCommand(command);
				dgModifier.doIt();

				if (iskeyable)
				{
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
					dgModifier.doIt();
				}

			}
	
			MFnDependencyNode depnode(dstDepNode);
			MPlug plug = depnode.findPlug(longname,&status);
			if (!status)
				continue;
			plug.setValue(val.asChar());

			if (iskeyable)
				MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName()+ MString(".") + longname);
			dgModifier.doIt();
		}
		else if (attrtype == MFn::kEnumAttribute)
		{
			int val;
			status  = srcPlug.getValue(val);
			if (!status)
				continue;
			int iskeyable = srcPlug.isKeyable();

			MFnEnumAttribute srcattr(srcattrobject);

			MString longname = srcattr.name();

			if (!attributealreadyexists)
			{
				short minimum,maximum;
				status = srcattr.getMin(minimum);
				if (!status) continue;
				srcattr.getMax(maximum);
				if (!status) continue;

				int count = maximum-minimum+1;
				if (count == 0)
					continue;

				MString names = srcattr.fieldName(0);
				for (unsigned j = 1; j < (unsigned)count; j++)
					names += MString(":") + srcattr.fieldName(j);

				MGlobal::executeCommand(MString("addAttr -ln ") + longname + MString(" -at \"enum\" -en \"")
					+ names + MString("\" ") + destdagpath.fullPathName());

				dgModifier.doIt();
				if (iskeyable)
					MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);

				dgModifier.doIt();
			}
			char temp[128];
			itoa(val,temp,10);
			MGlobal::executeCommand(MString("setAttr ") + destdagpath.fullPathName() + MString(".") + longname + MString(" ") + MString(temp));
			if (iskeyable)
				MGlobal::executeCommand(MString("setAttr -e -keyable true ") + destdagpath.fullPathName() + MString(".") + longname);
			dgModifier.doIt();
		}
	}
	dgModifier.doIt();
}

void MTools::CopyHierarchicalTransforms(MDagPath sourcedagpath, MDagPath destdagpath, bool translation, bool rotation, bool scale)
{
	MFnDagNode dagnode(sourcedagpath.node());

	if ((!sourcedagpath.isInstanced())&&(!dagnode.isIntermediateObject()))
	{
		CopyTransform(sourcedagpath,destdagpath,translation,rotation,scale);
	}

	unsigned numChildren = sourcedagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath sourcechilddagpath(sourcedagpath);
		MDagPath destchilddagpath(destdagpath);
		sourcechilddagpath.push(sourcedagpath.child(i));
		destchilddagpath.push(destdagpath.child(i));
		CopyHierarchicalTransforms(sourcechilddagpath,destchilddagpath,translation,rotation,scale);
	}
}

int MTools::MelConnect(MPlug sourceplug,MPlug destplug)
{
	MString sourceplugname = sourceplug.name();
	MString destplugname = destplug.name();

	MDGModifier dgmodifier;

	int result;
	MGlobal::executeCommand (
		MString ("defaultNavigation -quiet true -force true -connectToExisting -source ") +
		sourceplugname + MString (" -destination ") + destplugname,result);
	dgmodifier.doIt();
	return result;
}

int MTools::RedirectConnections(MPlug source,MPlug dest,
								bool breaksourceconnection,
								bool duplicatedest)
{
	MDGModifier dgmodifier;
	MDagModifier dagmodifier;
	MStatus status;
	MPlugArray connections;

	MString sourceplugname = source.name();
	const char *debug0 = sourceplugname.asChar();
	
	source.connectedTo(connections,true,false,&status);
	if (!status)
		return 0;

	for (int i = 0; i < (int)connections.length(); i++)
	{
		MString connectionname = connections[i].name();
		const char *debug = connectionname.asChar();

		MPlug currentconnection = connections[i];

		if (breaksourceconnection)
		{
			dgmodifier.disconnect(currentconnection,source);
			dgmodifier.doIt();
			if (!status)
				return 0;
		}
		
		if (duplicatedest)
		{
			MString plugname = connections[i].name();
			MStringArray parts;
			plugname.split('.',parts);

			const char *debug1 = parts[1].asChar();

			MFnDagNode dagnode(connections[i].node());

			MFnDependencyNode conndepnode(connections[i].node());
			const char *debug2 = conndepnode.name().asChar();
			
			MObject copy;
			MStringArray results;
			MGlobal::executeCommand((MString("duplicate ") + conndepnode.name()), results);
			dgmodifier.doIt();
			copy = GetMObject(results[0]);
			if (copy == MObject::kNullObj)
				return 0;

			MFnDependencyNode depnode(copy);
			MPlug newplug = depnode.findPlug(parts[1],&status);
			if (!status)
				return 0;
			
			MString newplugname = newplug.name();
			const char *debug3 = newplugname.asChar();

 			currentconnection = newplug;
		}

//		status = dgmodifier.connect(currentconnection,dest);
//		dgmodifier.doIt();
//		if (!status)
//			return 0;

		if (!MelConnect(currentconnection,dest))
			return 1;
	}
	
	return 1;
}

void MTools::CopyTransform(MDagPath sourcedagpath, MDagPath destdagpath, bool translation, bool rotation, bool scale)
{
	MStatus status;
	MDGModifier dgModifier;

	MFnTransform srctransform(sourcedagpath.node());

	MFnTransform dsttransform(destdagpath.node());
	
	MPoint rotatepivot = srctransform.rotatePivot(MSpace::kTransform,&status);
	MVector rotatepivottranslation = srctransform.rotatePivotTranslation(MSpace::kTransform,&status);

	MPoint scalepivot = srctransform.scalePivot(MSpace::kTransform,&status);
	MVector scalepivottranslation = srctransform.scalePivotTranslation(MSpace::kTransform,&status);

	MFnDependencyNode srcdepnode(srctransform);

	MPlug translationXplugSrc = srcdepnode.findPlug("translateX",&status);
	if (!status)
		return;
	MPlug translationYplugSrc = srcdepnode.findPlug("translateY",&status);
	MPlug translationZplugSrc = srcdepnode.findPlug("translateZ",&status);

	MPlug rotationXplugSrc = srcdepnode.findPlug("rotateX",&status);
	MPlug rotationYplugSrc = srcdepnode.findPlug("rotateY",&status);
	MPlug rotationZplugSrc = srcdepnode.findPlug("rotateZ",&status);

	MPlug scaleXplugSrc = srcdepnode.findPlug("scaleX",&status);
	MPlug scaleYplugSrc = srcdepnode.findPlug("scaleY",&status);
	MPlug scaleZplugSrc = srcdepnode.findPlug("scaleZ",&status);

	MPlug visibilityPlugSrc = srcdepnode.findPlug("visibility",&status);

	MFnDependencyNode dstdepnode(dsttransform);
	
	MPlug translationXplugDst = dstdepnode.findPlug("translateX",&status);
	MPlug translationYplugDst = dstdepnode.findPlug("translateY",&status);
	MPlug translationZplugDst = dstdepnode.findPlug("translateZ",&status);

	MPlug rotationXplugDst = dstdepnode.findPlug("rotateX",&status);
	MPlug rotationYplugDst = dstdepnode.findPlug("rotateY",&status);
	MPlug rotationZplugDst = dstdepnode.findPlug("rotateZ",&status);

	MPlug scaleXplugDst = dstdepnode.findPlug("scaleX",&status);
	MPlug scaleYplugDst = dstdepnode.findPlug("scaleY",&status);
	MPlug scaleZplugDst = dstdepnode.findPlug("scaleZ",&status);
	
	MPlug visibilityPlugDst = dstdepnode.findPlug("visibility",&status);

	if (translationXplugSrc.isConnected())
		RedirectConnections(translationXplugSrc,translationXplugDst);
	if (translationYplugSrc.isConnected())
		RedirectConnections(translationYplugSrc,translationYplugDst);
	if (translationZplugSrc.isConnected())
		RedirectConnections(translationZplugSrc,translationZplugDst);

	if (rotationXplugSrc.isConnected())
		RedirectConnections(rotationXplugSrc,rotationXplugDst);
	if (rotationYplugSrc.isConnected())
		RedirectConnections(rotationYplugSrc,rotationYplugDst);
	if (rotationZplugSrc.isConnected())
		RedirectConnections(rotationZplugSrc,rotationZplugDst);

	if (scaleXplugSrc.isConnected())
		RedirectConnections(scaleXplugSrc,scaleXplugDst);
	if (scaleYplugSrc.isConnected())
		RedirectConnections(scaleYplugSrc,scaleYplugDst);
	if (scaleZplugSrc.isConnected())
		RedirectConnections(scaleZplugSrc,scaleZplugDst);

	if (visibilityPlugSrc.isConnected())
		RedirectConnections(visibilityPlugSrc,visibilityPlugDst);

	if (translation)
	{

		MVector translate;
		translate = srctransform.translation(MSpace::kTransform,&status);
		status = dsttransform.setTranslation(translate,MSpace::kTransform);
	}
	
	if (rotation)
	{
		status = dsttransform.setRotatePivotTranslation(rotatepivottranslation,MSpace::kTransform);
		status = dsttransform.setRotatePivot(rotatepivot,MSpace::kTransform,true);

		MEulerRotation rotation;
		status = srctransform.getRotation(rotation);
		status = dsttransform.setRotation(rotation);
	}
	

	if (scale)
	{
		status = dsttransform.setScalePivotTranslation(scalepivottranslation,MSpace::kTransform);
		status = dsttransform.setScalePivot(scalepivot,MSpace::kTransform,true);

		double dScale[3];
		status = srctransform.getScale(dScale);
		status = dsttransform.setScale(dScale);
	}

	dgModifier.doIt();
}

void MTools::CopyShadersBranch(MDagPath sourcedagpath, MDagPath destdagpath)
{
	MStatus status;

	MString srcpathname = sourcedagpath.fullPathName();
	MString dstpathname = destdagpath.fullPathName();

	const char *debug = srcpathname.asChar();
	const char *debug0 = dstpathname.asChar();
	
	if (sourcedagpath.apiType() == MFn::kJoint)
		return;
	
	MFnDagNode dagnode(sourcedagpath.node());

	if ((sourcedagpath.apiType() == MFn::kMesh)&&(!dagnode.isIntermediateObject()))
	{
		CopyShaders(sourcedagpath,destdagpath);
	}

	unsigned numChildren = sourcedagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath sourcechilddagpath(sourcedagpath);
		MDagPath destchilddagpath(destdagpath);

		sourcechilddagpath.push(sourcedagpath.child(i));
		destchilddagpath.push(destdagpath.child(i));

		CopyShadersBranch(sourcechilddagpath,destchilddagpath);
	}
}

void MTools::CopyShaders(MDagPath sourcedagpath, MDagPath destdagpath)
{
	MDGModifier dgModifier;
	
	MObjectArray shadinggroups;
	shadinggroups.clear();

	MIntArray keys;
	keys.clear();

	MDagPath result,result0;

	result = sourcedagpath;
	result0 = destdagpath;

	if (sourcedagpath.apiType() != MFn::kMesh)
		if (!ExploreChildren(MFn::kMesh,sourcedagpath,result))
			return;

	if (destdagpath.apiType() != MFn::kMesh)
		if (!ExploreChildren(MFn::kMesh,destdagpath,result0))
			return;

	MFnMesh fnMesh(result);

	MString pathname = result.fullPathName();
	MString pathname1 = result0.fullPathName();

	const char *debug = pathname.asChar();
	const char *debug0 = pathname1.asChar();

	fnMesh.getConnectedShaders(0,shadinggroups,keys);

	int numshadinggroups = shadinggroups.length();

	ConnectShadingGroups(result0,shadinggroups,keys);
}

int MTools::FindMObject(MObject object,MObjectArray &mobjects)
{
	for (unsigned i = 0; i < mobjects.length(); i++)
	{
		if (object == mobjects[i])
			return i;
	}
	return -1;
}

// pops dagpath until it gets the top transform
int MTools::GetTransformRoot(MDagPath dagpath,MDagPath &result)
{
	MString fullpathname = dagpath.fullPathName();

	const char *debug = fullpathname.asChar();

	MStringArray splitarray;

	fullpathname.split('|',splitarray);

	unsigned pathcount = splitarray.length();

	if (pathcount == 0)
		return 0;

	for (unsigned i = 0; i < pathcount-1; i++)
		dagpath.pop();

	result = dagpath;
	return 1;
}

int MTools::NumParents(MDagPath dagpath)
{
	MStatus status;
	status = dagpath.pop();
	if (status)
		return NumParentsRecurse(dagpath,0);
	return 0;
}

int MTools::NumParentsRecurse(MDagPath dagpath,int count)
{
	MStatus status;
	MDagPath parentpath(dagpath);
	status = parentpath.pop();
	if (status)
		return NumParentsRecurse(parentpath,count+1);
	return count;
}

// pops dagpath until it gets the transform just under the reference
int MTools::GetTransformAfterReference(MDagPath dagpath,MDagPath &result)
{
	MString fullpathname = dagpath.fullPathName();

	const char *debug = fullpathname.asChar();

	MStringArray splitarray;

	fullpathname.split('|',splitarray);

	unsigned pathcount = splitarray.length();

	if (pathcount == 0)
		return 0;

	for (unsigned i = 0; i < pathcount-2; i++)
		dagpath.pop();

	result = dagpath;

	return 1;
}

int MTools::InstanceObject(MDagPath dagpath,MDagPath &result,bool uniquetransforms,bool copyattributes,MObject parentobject)
{
	MStatus status;
	MDGModifier dgModifier;
	MDagModifier dagModifier;

	MString fullpathname = dagpath.fullPathName();
	const char *debug = fullpathname.asChar();
	
	MStringArray results;
//	instance object using MEL so that shader groups remain intact
	if (uniquetransforms)
	{
		MGlobal::executeCommand ((MString("instance -lf ") + dagpath.fullPathName()), results);
	}
	else
	{
		MGlobal::executeCommand ((MString("instance ") + dagpath.fullPathName()), results);
	}
	dgModifier.doIt();

	MDagPath instancepath;

	const char *debug1 = results[0].asChar();

//	parent instance to parent object
	if (!GetMDagPath(results[0],instancepath))
		return 0;
	
	if (parentobject != MObject::kNullObj)
	{
		MFnDagNode dagNode(parentobject);
		dagNode.addChild(instancepath.node());
	}
	else
	{
		dagModifier.reparentNode(instancepath.node());
	}

//	copy attributes
	if (copyattributes)
		CopyHierarchicalAttributes(dagpath,instancepath);

	dgModifier.doIt();
	dagModifier.doIt();

	result = instancepath;

//	MString displaylayername;
//	MFnDependencyNode depnode(dagpath.node());
//	GetDisplayLayer(depnode.name(),displaylayername);
//	MTools::AddNodeToDisplayLayer(result.fullPathName(),displaylayername);

	return 1;
}

int MTools::DuplicateObject(MDagPath dagpath,MDagPath &result,bool copyattributes,MObject parentobject,bool uniquenames)
{
	MStatus status;
	MDGModifier dgModifier;
	MDagModifier dagModifier;
	MDagPath duplicatepath;

	MString sourcepathname = dagpath.fullPathName();
	const char*debug0 = sourcepathname.asChar();
	
	MDagPath joint;
	if (ExploreChildren(MFn::kJoint,dagpath,joint))
	{
		if (!MTools::DuplicateBoundGeometryAndCharacter(dagpath,result))
			return MS::kFailure;

		MString fullpathname = result.fullPathName();
		const char *debug = fullpathname.asChar();

	//	parent copy to parent object
		if (parentobject != MObject::kNullObj)
		{
			MFnDagNode dagNode(parentobject);
			dagNode.addChild(result.node());
		}
		else
		{
			dagModifier.reparentNode(result.node());
		}

		dagModifier.doIt();

		//	copy attributes
		if (copyattributes)
			CopyHierarchicalAttributes(dagpath,result);

		CopyHierarchicalTransforms(dagpath,result,true,true,true);

		return 1;
	}

	MStringArray results;
//	duplicate object using MEL so that shader groups remain intact
	if (!uniquenames)
	{
		MGlobal::executeCommand((MString("duplicate ") + dagpath.fullPathName()), results);
		dgModifier.doIt();
	}
	else
	{
		MGlobal::executeCommand((MString("duplicate -rc ") + dagpath.fullPathName()), results);
		dgModifier.doIt();
	}

	if (!GetMDagPath(results[0],duplicatepath))
		return 0;
	
//	parent copy to parent object
	if (parentobject != MObject::kNullObj)
	{
		MFnDagNode dagNode(parentobject);
		dagNode.addChild(duplicatepath.node());
	}
	else
	{
		dagModifier.reparentNode(duplicatepath.node());
	}

//	copy attributes
	if (copyattributes)
		CopyHierarchicalAttributes(dagpath,duplicatepath);

	dgModifier.doIt();
	dagModifier.doIt();

	//	copy light with light attenuation
	if (dagpath.hasFn(MFn::kLight))
	{
		MDagPath lightshape = dagpath;

		lightshape.push(dagpath.child(0));
		
		MFnDependencyNode depnode(lightshape.node());
		const char *debug = depnode.name().asChar();
		MPlug plug = depnode.findPlug("intensity",&status);
		if (status)
		{
			MPlugArray plugarray;
			plug.connectedTo(plugarray,true,true,&status);
			
			if (plugarray.length()>0)
			{
				MObject animcurve = plugarray[0].node();
				MFnDependencyNode depnode2(animcurve);
				const char *debug = depnode2.name().asChar();
				
				MStringArray result;
				MGlobal::executeCommand (MString("duplicate ") + depnode2.name(), result);
				dgModifier.doIt();
				
				MTools::CreateLightAttenuationNetwork(duplicatepath.node(),result[0]);
			}
		}
	}
	
	
	result = duplicatepath;
	
//	MString displaylayername;
//	MFnDependencyNode depnode(dagpath.node());
//	GetDisplayLayer(depnode.name(),displaylayername);
//	MTools::AddNodeToDisplayLayer(result.fullPathName(),displaylayername);
	
	return 1;
}

// loop recursively searching through dagpath and dagpath's children
int MTools::IsInstance(MDagPath dagpath)
{
	if (dagpath.isInstanced(false))
		return 1;

	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));
		if (IsInstance(temppath)) return 1;
	}
	return 0;
}

int MTools::GetFirstShape(MDagPath dagpath,MDagPath &result,bool lookforinstance)
{
//	if dagpath is already pointing to shape return dagpath
	if ((dagpath.hasFn(MFn::kShape))&&(!dagpath.hasFn((MFn::kTransform))))
	{
		if (lookforinstance)
		{
			if (dagpath.isInstanced())
				result = dagpath;
				return 1;
		}
		else
		{
			result = dagpath;
			return 1;
		}
	}
//	recurse through children looking for first shape when found return new dagpath
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath firstshapedagpath;
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));

		if (GetFirstShape(temppath,firstshapedagpath))
		{
			result = firstshapedagpath;
			return 1;
		}
	}
	return 0;
}

void MTools::GetShapes(MDagPath dagpath,MDagPathArray &shapes)
{
	if ((dagpath.hasFn(MFn::kShape))&&(!dagpath.hasFn((MFn::kTransform))))
	{
		shapes.append(dagpath);
	}

	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));
		GetShapes(temppath,shapes);
	}
}

int MTools::IsReference(MDagPath dagpath,bool recursive)
{
	MFnDagNode dagNode(dagpath.node());

	if (dagNode.isFromReferencedFile())
		return 1;

	if (recursive)
	{
		unsigned numChildren = dagpath.childCount();
		for (unsigned i = 0; i < numChildren; i++)
		{
			MDagPath temppath(dagpath);
			temppath.push(dagpath.child(i));
			if (IsReference(temppath)) return 1;
		}
	}
	return 0;
}

int MTools::DoesObjectBelongToReferences(MDagPath dagpath)
{
	MDagPath rootdagpath;
	if (!MTools::GetTransformRoot(dagpath,rootdagpath))
		return 0;

	MFnDependencyNode fnNode(rootdagpath.node());
	if (fnNode.name() == "References")
	{
		return 1;
	}
	return 0;
}

int MTools::GetRootDagPath(MDagPath dagpath,MDagPath &result)
{
	MFnDagNode dagnode(dagpath.node());
	MFnDagNode dagnode2(dagnode.dagRoot());
	dagnode2.getPath(result);
	return 1;
}

int MTools::GetTransformBeforeReference(MDagPath dagpath,MDagPath &result)
{
	MFnDagNode dagnode(dagpath.node());

	MDagPath rootdagpath;
	GetRootDagPath(dagpath,rootdagpath);

	if (dagnode.isFromReferencedFile())
	{
		result = rootdagpath;
		return 1;
	}

	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath transformpath;
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));
		GetTransformBeforeReference(temppath,transformpath);
		if (transformpath==rootdagpath)
		{
			result = dagpath;
			return 1;
		}
		else
		{
			result = transformpath;
			return 1;
		}
	}
	return 0;
}

int MTools::GetTransformBeforeInstance(MDagPath dagpath,MDagPath &result)
{
	MStatus status;
	MFnDagNode dagNode(dagpath.node());

	MDagPath rootdagpath;
	GetRootDagPath(dagpath,rootdagpath);

	if (dagpath.isInstanced(false))
	{
		if (dagNode.parentCount()>0)
		{
			MDagPath temppath(dagpath);
			temppath.pop();
			if (temppath.isInstanced(false))
			{
				MDagPath transformpath;
				GetTransformBeforeInstance(temppath,transformpath);
				return 1;
			}
			if (temppath.hasFn(MFn::kTransform))
			{
				result = temppath;
				return 1; 
			}

			return 0;
		}
	}

	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath transformpath;
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));
		GetTransformBeforeInstance(temppath,transformpath);
		if (transformpath==rootdagpath)
		{
			result = dagpath;
			return 1;
		}
		else
		{
			result = transformpath;
			return 1;
		}
	}
	return 0;
}

// this function iterates up and down the starting at a dagpath
// looking for a transform named similarly to name passed in
int MTools::FindSimilarName(MDagPath dagpath,MString name,MDagPath &result)
{
	if (FindSimilarNameLookParents(dagpath,name,result))
		return 1;

	if (FindSimilarNameLookChildren(dagpath,name,result))
		return 1;
	return 0;
}

int MTools::FindSimilarNameLookParents(MDagPath dagpath,MString name,MDagPath &result)
{
	MStatus status;
	MFnDependencyNode depnode(dagpath.node());
//	if dagpath already points to node of a similar name return dagpath
	if (IsNodeNameSimilar(name,depnode.name()))
	{
		result = dagpath;
		return 1;
	}
	MFnDagNode dagNode(dagpath.node());

	if (dagNode.parentCount() > 0)
	{
//	if parent is not world pop dagpath
		if (!dagNode.parent(0,&status).hasFn(MFn::kWorld))
		{
			MDagPath temppath(dagpath);
			temppath.pop();
// recurse parents
			MDagPath founddagpath;
			if (FindSimilarNameLookParents(temppath,name,founddagpath))
			{
				result = founddagpath;
				return 1;
			}
		}
	}
	return 0;
}

int MTools::FindSimilarNameLookChildren(MDagPath dagpath,MString name,MDagPath &result)
{
	MStatus status;
	MFnDependencyNode depnode(dagpath.node());

//	if dagpath already points to node of a similar name return dagpath
	if (IsNodeNameSimilar(name,depnode.name()))
	{
		result = dagpath;
		return 1;
	}
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath temppath(dagpath);
		temppath.push(dagpath.child(i));

// recurse children
		MDagPath founddagpath;
		if(FindSimilarNameLookChildren(temppath,name,founddagpath))
		{
			result = founddagpath;
			return 1;
		}
	}
	return 0;
}

MString MTools::GetReferenceFileName(MDagPath dagpath)
{
	MFnDagNode dagNode(dagpath.node());

	if (dagNode.isFromReferencedFile())
	{
		MString result;
		MGlobal::executeCommand(MString("reference -q -f ") + dagpath.fullPathName(),result);
		const char *debug = result.asChar();
		return result;
	}
		
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childpath(dagpath);
		childpath.push(dagpath.child(i));
		MString fname = GetReferenceFileName(childpath);
		if (fname != "")
			return fname;
	}
	return "";
}

MObject MTools::GetMObject(MString name)
{
	MSelectionList selection;

	MStatus status = selection.add(name);

	if (!status)
		return MObject::kNullObj;

	MObject object;

	selection.getDependNode(0,object);

	return object;
}

int MTools::GetMDagPath(MString name,MDagPath &dagpath)
{
	MSelectionList selection;

	MStatus status = selection.add(name);
	if (!status)
		return 0;

	selection.getDagPath(0,dagpath);

	return 1;
}


// iterates through dag for similar names
MStatus MTools::GetDagPathsOfSimilarName(MString name,MDagPathArray &dagpaths)
{
	MStatus status;

	dagpaths.clear();

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	if (!status)
		return MS::kFailure;

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath dagPath;
		MObject  component = MObject::kNullObj;

		status = dagIterator.getPath(dagPath);
		if (!status)
			return MS::kFailure;

		MFnDependencyNode depnode(dagPath.node());
		const char *debug = depnode.name().asChar();

		if (IsNodeNameSimilar(depnode.name(),name))
			dagpaths.append(dagPath);
	}
	return MS::kSuccess;
}

// this function is useful because it iterates through non DAG objects
void MTools::GetNodesOfType(MString type,MObjectArray &objects)
{
	MStatus status;

	// Get all of the sets in maya and put them into
	// a selection list
	
	MStringArray result;
	MGlobal::executeCommand( "ls", result );
	MSelectionList setList;

	objects.clear();

	for (int i = 0; i < (int)result.length(); i++)
		setList.add(result[i]);

	// Convert selection list to MObjectArray

	MObject object;
	for (i = 0; i < (int)result.length(); i++)
	{
		setList.getDependNode(i,object);
		MFnDependencyNode fnNode(object);
		const char *debug = fnNode.name().asChar();

		if (MString(object.apiTypeStr()) == type)
		{
			objects.append(object);
		}
	}
}

// this function iterates through the DAG for types
MStatus MTools::GetDagNodesOfType(MString type,MDagPathArray &dagpaths)
{
	MStatus status;

	dagpaths.clear();

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	if (!status)
		return MS::kFailure;

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath dagPath;
		MObject  component = MObject::kNullObj;

		status = dagIterator.getPath(dagPath);
		if (!status)
			return MS::kFailure;

		MFnDependencyNode fnNode(dagPath.node());
		const char *debug = fnNode.name().asChar();

//		if (dagPath.hasFn(type))
		if (MString(dagPath.node().apiTypeStr()) == type)
			dagpaths.append(dagPath);
	}
	return MS::kSuccess;
}

void MTools::GetDisplayLayers(MStringArray &layers)
{
	MDGModifier dgModifier;
//	MGlobal::executeCommand("ls -type displayLayer",layers);
	MGlobal::executeCommand("ls -et displayLayer",layers);
	dgModifier.doIt();
}

// failed if returned ""
void MTools::GetCurrentDisplayLayer(MString &name)
{
	MDGModifier dgModifier;
	MGlobal::executeCommand("editDisplayLayerGlobals -q -cdl",name);
	dgModifier.doIt();
}

// creates reference and creates an instance
int MTools::CreateReference(MString filename,MDagPath &result,bool uniquetransforms,bool createinstance)
{
	MDGModifier dgModifier;
	MStatus status;
	MFileIO fileio;

	const char *debug = filename.asChar();
	
	status = fileio.reference(filename);
	if (!status)
		return 0;

/*	
	MString fixedfilename = filename;
	MTools::FindAndReplace(fixedfilename,"\\","/");

	const char *debug1 = fixedfilename.asChar();

	MString resultstring;
	MGlobal::executeCommand(MString("file -reference \"") + fixedfilename + MString("\""),resultstring);
	dgModifier.doIt();
	const char*debug0 = resultstring.asChar();

	if (resultstring == "")
		return 0;
*/

//	hopefully there is only one if not we are in big trouble
	MObject referencefolder = MTools::GetMObject("References");

//	if there is no reference group transform create one
	if (referencefolder == MObject::kNullObj)
	{

		MFnTransform transform;
//		create empty group transform
		referencefolder = transform.create(MObject::kNullObj);
		dgModifier.doIt();

//		name it 'References'
		dgModifier.renameNode(referencefolder,"References");
		dgModifier.doIt();

		MFnDependencyNode depnode(referencefolder);
//		make group transform invisible
		MPlug visibility = depnode.findPlug("visibility",&status);
		if (status)
		{
			visibility.setValue(0);
		}
	}

	MStringArray refnodes;
	fileio.getReferenceNodes(filename,refnodes);

	int result0;
	MDagPath refdagpath;
//	loop through reference nodes
	for (int i = 0; i < (int)refnodes.length(); i++)
	{
		result0 = MTools::GetMDagPath(refnodes[i],refdagpath);
		if(!result0)
			continue;// if can't find object name skip

		if (refdagpath.hasFn(MFn::kTransform))
		{
			break;// break on transform only
		}
	}

//	parent reference to reference group transform
	if ((referencefolder != MObject::kNullObj)&&(result0))
	{
		MFnDagNode dagNode(referencefolder);
		dagNode.addChild(refdagpath.node());
		dgModifier.doIt();
	}

//	create instance of reference
	if (createinstance)
	{
		MDagPath instancedagpath;
		if (uniquetransforms)
		{
			if(!MTools::InstanceObject(refdagpath,instancedagpath,true))
				return 0;
		}
		else
		{
			if (!MTools::InstanceObject(refdagpath,instancedagpath))
				return 0;
		}
		result = instancedagpath;
		return 1;
	}
	return 1;
}

int MTools::RemoveReference(MDagPath dagpath)
{
	MDGModifier dgModifier;
	MDagPath refdagpath;
	if (!GetReferenceObject(dagpath,refdagpath))
		return 0;

	MString refname = GetReferenceFileName(dagpath);

	MGlobal::executeCommand(MString("file -rr -f \"") + refname + MString("\""));
	dgModifier.doIt();
	return 1;
}

int MTools::ReloadReference(MDagPath dagpath)
{
	MDGModifier dgModifier;
	MDagPath refdagpath;
	if (!GetReferenceObject(dagpath,refdagpath))
		return 0;

	MString filename = GetReferenceFileName(dagpath);

	MGlobal::executeCommand(MString("file -lr -f \"") + filename + MString("\""));// reloads reference
	dgModifier.doIt();
	return 1;
}

void MTools::CreateDisplayLayer(MString layername)
{
	MDGModifier dgmodifier;
	MGlobal::executeCommand(MString("createDisplayLayer -empty -name ") + layername);
	dgmodifier.doIt();	
}

void MTools::SetDisplayLayerProperties(MString layername,int visibility,MString state)
{
	MDGModifier dgmodifier;

	char visibility_cstr[128];
	itoa(visibility,visibility_cstr,10);

//	MGlobal::executeCommand(MString("layerButton -c ") MString(color.r) + MString(color.g) + MString(color.b) + layername);
	MGlobal::executeCommand(MString("layerButton -e -layerVisible ") + MString(visibility_cstr) + MString(" ") + layername);
	MGlobal::executeCommand(MString("layerButton -e -layerState ") + state + MString(" ") + layername);

	dgmodifier.doIt();	
}

void MTools::GetDisplayLayerProperties(MString layername,int &visibility,MString &state)
{
	MDGModifier dgmodifier;
	const char *layer = layername.asChar();
	MGlobal::executeCommand(MString("layerButton -q -layerVisible \"") + layername + MString("\""),visibility);
	MGlobal::executeCommand(MString("layerButton -q -layerState \"") + layername + MString("\""),state);
	dgmodifier.doIt();	
	const char *debug = state.asChar();
}

void MTools::AddNodesToDisplayLayer(MStringArray nodes,MString layername)
{
	MDGModifier dgModifier;

	MString command = MString("editDisplayLayerMembers -nr ") + layername;

	for (unsigned i = 0; i < nodes.length(); i++)
	{
		command += MString (" ") + nodes[i];
	}

	MGlobal::executeCommand(command);

	dgModifier.doIt();
}

void MTools::AddNodeToDisplayLayer(MString node,MString layername)
{
	MDGModifier dgModifier;

	MGlobal::executeCommand(MString("editDisplayLayerMembers -nr ") + layername + MString(" ") + node);

	dgModifier.doIt();
}

void MTools::AddNodesToInstanceGroup(const MDagPathArray &nodes,MObject igroup)
{
	MDGModifier dgModifier;

	MFnDependencyNode igroupdepnode(igroup);

	MFnDependencyNode fnNode(nodes[0].node());
	MString command = fnNode.name();

	for (unsigned i = 1; i < nodes.length(); i++)
	{
		MFnDependencyNode fnNode(nodes[i].node());
		command += MString(" ") + fnNode.name();
	}

	MGlobal::executeCommand(MString("parent ") + command + MString(" ") + igroupdepnode.name());

	dgModifier.doIt();
}

void MTools::GetNodesInLayer(MString layer,MStringArray &nodes)
{
	MDGModifier dgModifier;

	MGlobal::executeCommand(MString("editDisplayLayerMembers -q ")+layer,nodes);

	dgModifier.doIt();
}

void MTools::ConvertDagPathsToNames(MDagPathArray dagpaths,MStringArray &mstrings)
{
	mstrings.clear();
	for (unsigned i = 0; i < dagpaths.length(); i++)
	{
		MFnDependencyNode fnNode(dagpaths[i].node());
		mstrings.append(fnNode.name());
	}
}

void MTools::ConvertMObjectsToNames(MObjectArray objects,MStringArray &mstrings)
{
	mstrings.clear();
	for (unsigned i = 0; i < objects.length(); i++)
	{
		MFnDependencyNode fnNode(objects[i]);
		mstrings.append(fnNode.name());
	}
}

void MTools::ConvertNamesToMObjects(const MStringArray &mstrings,MSelectionList &objects)
{
	objects.clear();
	for (unsigned i = 0; i < mstrings.length(); i++)
		objects.add(mstrings[i]);
}

void MTools::ConvertNamesToMObjects(const MStringArray &mstrings,MObjectArray &objects)
{
	MSelectionList selection;

	ConvertNamesToMObjects(mstrings,selection);

	objects .clear();

	MObject object;

	for (int i = 0; i < (int)selection.length(); i++)
	{
		selection.getDependNode(i,object);
		objects.append(object);
	}
}

void MTools::ConvertSelectionToMObjects(const MSelectionList &selection,MObjectArray &objects)
{
	MObject object;

	objects.clear();

	for (int i = 0; i < (int)selection.length(); i++)
	{
		selection.getDependNode(i,object);
		objects.append(object);
	}
}

MStatus MTools::ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const MIntArray &shaderindicies)
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

	MFnMesh fnMesh(dagpath);
	if (fnMesh.numPolygons()==0)
		return MS::kFailure;

	for (int i = 0; i < (int)SGList.length(); i++)
	{
		MSelectionList polySelection;
		polySelection.clear();
		bool flag = false;

		for ( MItPoly.reset(); !MItPoly.isDone(); MItPoly.next() )
		{
			if (shaderindicies[MItPoly.index()] < 0)// assign initialShadingGroup
			{
				continue;
			}
			else if (shaderindicies[MItPoly.index()] == i)
			{
				MObject polyface = MItPoly.polygon(&status);
				polySelection.add(dagpath,polyface);
				flag = true;
			}
		}

		MFnSet set(SGList[i], &status);
		if (!status)
		{
			MGlobal::displayWarning("Could not create shader set.");
			return MS::kFailure;
		}

		if (flag)
		{
			status = set.addMembers(polySelection);
			if (!status)
			{
				MGlobal::displayWarning(MString("Could not add poly members to shader set. ") + status.errorString());
				return MS::kFailure;
			}
		}
	}

	return MStatus::kSuccess;
}


MStatus MTools::ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const std::vector<int> &shaderindicies)
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

	MFnMesh fnMesh(dagpath);
	if (fnMesh.numPolygons()==0)
		return MS::kFailure;

	for (int i = 0; i < (int)SGList.length(); i++)
	{
		MSelectionList polySelection;
		polySelection.clear();
		
		bool flag = false;

		for ( MItPoly.reset(); !MItPoly.isDone(); MItPoly.next() )
		{
			if (shaderindicies[MItPoly.index()] < 0)// assign initialShadingGroup
			{
				continue;
			}
			else if (shaderindicies[MItPoly.index()] == i)
			{
				MObject polyface = MItPoly.polygon(&status);
				polySelection.add(dagpath,polyface);
				flag = true;
			}
		}

		MFnSet set(SGList[i], &status);
		if (!status)
		{
			MGlobal::displayWarning("Could not create shader set.");
			return MS::kFailure;
		}
		if (flag)
		{
			status = set.addMembers(polySelection);
			if (!status)
			{
				MGlobal::displayWarning(MString("Could not add poly members to shader set. ") + status.errorString());
				return MS::kFailure;
			}
		}
	}

	return MStatus::kSuccess;
}

MObject MTools::CreateLight(MString type,MString name)
{
	MDGModifier dgModifier;
	MString result;

	MGlobal::executeCommand(MString("shadingNode -asLight ")+ type + MString(" -name ") + name + MString("shape"),result);
	dgModifier.doIt();

	MObject lighttransform = GetMObject(result);

	dgModifier.renameNode(lighttransform,name);
	dgModifier.doIt();

	return lighttransform;
}

int MTools::BindSkin(MDagPath meshtransform,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups)
{
	if (bones.size()==0)
		return 1;
	if (bones.size()!=vertexgroups.size())
		return 0;

	MStatus status;
	MDGModifier dgmodifier;

	bool firstjointclusterflag = true;
	MObject firstjointcluster;
	
	MObject prevjointcluster = MObject::kNullObj;

	MObjectArray bonesetarray,jointclusterarray,groupidarray;
	bonesetarray.clear();
	jointclusterarray.clear();
	groupidarray.clear();

	MObject bindpose = MTools::CreateBindPose();
	if (bindpose == MObject::kNullObj)
		return 0;

	// need mesh extended transform
	MObject shape = meshtransform.child(0,&status);
	if (!status)
		return 0;

	MFnDependencyNode shapedepnode(shape);
	MString modelname = shapedepnode.name();

	MFnMesh fnmesh(meshtransform);

	MObject copyshape = fnmesh.copy(shape,meshtransform.node(),&status);
	if (!status)
		return 0;

	dgmodifier.renameNode(copyshape,modelname + "Orig");
	dgmodifier.doIt();
	
//	parent copy shape under original objects transform
	MDagModifier dagmodifier;
	dagmodifier.reparentNode(copyshape,meshtransform.node());
	dagmodifier.doIt();

	MFnDagNode dagnode(copyshape);
	dagnode.setIntermediateObject(true);

	MFnDependencyNode copyshapedepnode(copyshape);

	MPlug shapeoutmeshplug = copyshapedepnode.findPlug("outMesh",&status);
	if (!status)
		return 0;
	
	MPlug meshinplug = shapedepnode.findPlug("inMesh",&status);
	if (!status)
		return 0;

	status = dgmodifier.connect(shapeoutmeshplug,meshinplug);
	dgmodifier.doIt();
	if (!status)
		return 0;

//	create bones and much more
	IntHash bonesetmap;
	for (int i = 0; i < (int)bones.size(); i++)
	{
			MObject bone = bones[i].node();
			MString fullpathname = bones[i].fullPathName();
			const char *debug = fullpathname.asChar();

			if (bone == MObject::kNullObj)
				continue;

			MSelectionList *vertices = &vertexgroups[i];

//			if (vertices->length()==0)
//				continue;

			MObject jointcluster,boneset;

			if (!HookUpBoneToBindPose(bones[i],bindpose,prevjointcluster,*vertices,jointcluster,boneset))
				return 0;
		
//			store boneset pointer in map
			MFnDependencyNode bonesetdepnode(boneset);
	
			bonesetarray.append(boneset);
			jointclusterarray.append(jointcluster);

			bonesetmap.AddKey((int)MTools::GetMPtrBase(bonesetdepnode),jointclusterarray.length()-1);

			if (firstjointclusterflag)
			{
				firstjointcluster = jointcluster;
				firstjointclusterflag = false;
			}	
				
			prevjointcluster = jointcluster;
	}

// connect joint cluster chain just before in mesh
	MPlugArray connected;
	meshinplug.connectedTo(connected,true,false,&status);
	if (connected.length()==0)
		return 0;
	MPlug nodeplug_connected_to_inmesh = connected[0];

	dgmodifier.disconnect(nodeplug_connected_to_inmesh,meshinplug);
	dgmodifier.doIt();
	
	MFnDependencyNode firstjointclusterdepnode(jointclusterarray[0]);

	MPlug jointclusterinput = firstjointclusterdepnode.findPlug("input",&status);
	if (!status)
			return 0;

	int nextindex = jointclusterinput.numConnectedElements(&status);

	MPlug jointclusterinputplug = jointclusterinput.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return 0;
		
	MPlug inputgeometryplug = jointclusterinputplug.child(0,&status);

	status = dgmodifier.connect(nodeplug_connected_to_inmesh,inputgeometryplug);
	dgmodifier.doIt();
	if (!status)
		return 0;

	MFnDependencyNode lastjointclusterdepnode(jointclusterarray[jointclusterarray.length()-1]);
	MPlug lastjointclusteroutputgeom = lastjointclusterdepnode.findPlug("outputGeometry",&status);
	if (!status)
		return 0;

	nextindex = lastjointclusteroutputgeom.numConnectedElements(&status);
		
	MPlug lastjointclusteroutputgeom2 = lastjointclusteroutputgeom.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return 0;

	dgmodifier.connect(lastjointclusteroutputgeom2,meshinplug);
	dgmodifier.doIt();
	if (!status)
		return 0;

//	create skin partition
	MObject skinpartition = MTools::CreateSkinPartition();
	if (skinpartition == MObject::kNullObj)
		return 0;

	MFnDependencyNode partitiondepnode(skinpartition);

	MPlug partitionsetsplug = partitiondepnode.findPlug("sets",&status);
	if (!status)
		return 0;

	for (i = 0; i < (int)bonesetarray.length(); i++)
	{
		MFnDependencyNode bonesetdepnode(bonesetarray[i]);
		MPlug bonesetpartitionplug = bonesetdepnode.findPlug("partition",&status);
		if (!status)
			return 0;

		int nextindex = partitionsetsplug.evaluateNumElements(&status);

		MPlug partitionsetplug = partitionsetsplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;

		status = dgmodifier.connect(bonesetpartitionplug,partitionsetplug);
		dgmodifier.doIt();
		if (!status)
			return 0;
	}

//	assign group parts to jointclusters

	MPlug instobjgroupsplug = shapedepnode.findPlug("instObjGroups",&status);
	if (!status)
		return 0;

	int numelements = instobjgroupsplug.evaluateNumElements(&status);

	for (i = 0; i < numelements; i++)
	{
		MPlug instobjgroupsplug2 = instobjgroupsplug.elementByPhysicalIndex(i,&status);

		MPlug instobjgroupsplug3 = instobjgroupsplug2.child(0,&status);
		if (!status)
			return 0;

		MString instobjgroupsplugname = instobjgroupsplug2.name();
		const char *debug0 = instobjgroupsplugname.asChar();

		int numelements2 = instobjgroupsplug3.evaluateNumElements(&status);

		for (int j = 0; j < numelements2; j++)
		{
			MPlug instobjgroupplug = instobjgroupsplug3.elementByPhysicalIndex(j);

			MPlugArray connected2;
			instobjgroupplug.connectedTo(connected2,false,true,&status);
			for (int k = 0; k < (int)connected2.length(); k++)
			{
				MObject setobject = connected2[k].node();

				MFnDependencyNode setdepnode(setobject);
				const char *setname = setdepnode.name().asChar();

				if (bonesetmap.HasKey((int)MTools::GetMPtrBase(setdepnode)))
				{
//					get objectgroupid child
					MPlug instobjgroupidplug = instobjgroupplug.child(1,&status);
					if (!status)
						return 0;

					MPlugArray connected;
					instobjgroupidplug.connectedTo(connected,true,false,&status);
					if (!status)
						return 0;
					
					if (connected.length()==0)
						return 0;

					MObject groupidnode = connected[0].node();
					
					MFnDependencyNode groupiddepnode(groupidnode);

					MPlug groupidplug = groupiddepnode.findPlug("groupId",&status);

					int index;
					bonesetmap.FindKey((int)MTools::GetMPtrBase(setdepnode),index);
					
					MObject jointcluster = jointclusterarray[index];
					
					MFnDependencyNode jointclusterdepnode(jointcluster);

					MPlug jointclusterinputplug = jointclusterdepnode.findPlug("input",&status);
					if (!status)
						return 0;
		
					MPlug jointclusterinputplug2 = jointclusterinputplug.elementByLogicalIndex(0,&status);
					if (!status)
						return 0;

					MPlug jointclusterinputchildplug = jointclusterinputplug2.child(1,&status);
					if (!status)
						return 0;

					MString groupidplugname = groupidplug.name();
					MString jointclusterinputchildplugname = jointclusterinputchildplug.name();

					const char *debugA = groupidplugname.asChar();
					const char *debugB = jointclusterinputchildplugname.asChar();
	
					dgmodifier.connect(groupidplug,jointclusterinputchildplug);
					if (!status)
						return 0;
					
					dgmodifier.doIt();
				}
			}	
		}
	}

	MObject rootbone = bones[0].node();
	MFnDependencyNode rootbonedepnode(rootbone);

	MGlobal::executeCommand(MString("bindSkin -enable 1 ") + rootbonedepnode.name());
	dgmodifier.doIt();

	return 1;
}

// find joint cluster from bone
MObject MTools::GetJointCluster(MObject bone)
{
	MStatus status;
	
	MFnDependencyNode depnode(bone);

	MPlug worldmatrixplug = depnode.findPlug("worldMatrix",&status);
	if (!status)
		return MObject::kNullObj;

	if (worldmatrixplug.evaluateNumElements()==0)
		return MObject::kNullObj;

	MPlug worldmatrixplug2 = worldmatrixplug.elementByLogicalIndex(0,&status);
	if (!status)
		return MObject::kNullObj;

	MPlugArray plugs;

	worldmatrixplug2.connectedTo(plugs,false,true,&status);
	if (!status)
		return MObject::kNullObj;

	if (plugs.length()==0)
		return MObject::kNullObj;

	if (plugs[0].node().apiType() != MFn::kJointCluster)
		return MObject::kNullObj;

	return plugs[0].node();
}

// create bind pose node
MObject MTools::CreateBindPose()
{
	MStatus status;
	MFnDependencyNode newdepnode;
	MDGModifier dgmodifier;
//	Ask Maya some day, why the hell does this not work!
//	MObject newbindpose = newdepnode.create(MFn::kDagPose,"bindPose",&status);
//	MObject newbindpose = dgmodifier.createNode(MFn::kDagPose,&status);

	MObject newbindpose = newdepnode.create("dagPose","bindPose",&status);
	if (!status)
		return MObject::kNullObj;

	MFnDependencyNode bindposedepnode(newbindpose);

//	MPlug bindposeflag = bindposedepnode.findPlug("bindPose",&status);

//	this is evil!!!
//	status = bindposeflag.setValue(true);
//	if (!status)
//		return MObject::kNullObj;

//	MPlug bindposeglobal = bindposedepnode.findPlug("global",&status);
//	if (!status)
//		return MObject::kNullObj;

//	status = bindposeglobal.setValue(true);
//	if (!status)
//		return MObject::kNullObj;

	return newbindpose;
}

MObject MTools::CreateSkinPartition()
{
	MStatus status;
	MFnDependencyNode newdepnode;
	MDGModifier dgmodifier;

	MObject newskinpartition = newdepnode.create("partition","skinPartition",&status);
	if (!status)
		return MObject::kNullObj;

	return newskinpartition;
}
/*
// create joint node
int MTools::CreateBone(MDagPath &dagpath,MObject parent,MVector pivottranslation,MQuaternion *orientation)
{
	MStatus status;
	MFnIkJoint fnjoint;
	MDGModifier dgmodifier;

//	create joint
	MObject joint = fnjoint.create(parent,&status);
	if (!status)
		return 0;

//	set joint pivot
	MFnTransform fnTransform(joint);
	status = fnTransform.setTranslation(pivottranslation,MSpace::kObject);
	if (!status)
		MGlobal::displayError("Couldn't set transform of bone.");

//	set joint orientation
	if (orientation)
	{
		status = fnjoint.setOrientation(*orientation);
		if (!status)
			MGlobal::displayError("Couldn't set orientation of bone.");
	}

	MFnDagNode dagnode(joint);
	dagnode.getPath(dagpath);

	return 1;
}
*/

// create joint node
int MTools::CreateBone(MDagPath &dagpath,MObject parent,MVector pivottranslation,MQuaternion *orientation)
{
	MStatus status;
	MFnIkJoint fnjoint;
	MDagModifier dagmodifier;

//	create joint
	MObject joint = fnjoint.create(MObject::kNullObj,&status);
	if (!status)
		return 0;

//	set joint pivot
	status = fnjoint.setTranslation(pivottranslation,MSpace::kObject);
	if (!status)
		MGlobal::displayError("Couldn't set transform of bone.");

//	set joint orientation
	if (orientation)
	{
		status = fnjoint.setOrientation(*orientation);
		if (!status)
			MGlobal::displayError("Couldn't set orientation of bone.");
	}
	
	if (parent!=MObject::kNullObj)
	{
		MFnDagNode dagNode(parent);
		dagNode.addChild(joint);
	}
	
	MFnDagNode dagnode(joint);
	dagnode.getPath(dagpath);
	
	dagmodifier.doIt();

	return 1;
}

int MTools::GetBindPoseMatrix(MDagPath bonedagpath,MTransformationMatrix &matrix)
{
	MStatus status;
	MFnDependencyNode jointdepnode(bonedagpath.node());

	MPlug bindposeplug = jointdepnode.findPlug("bindPose",&status);
	if (!status)
		return 0;

	MPlugArray connected;
	bindposeplug.connectedTo(connected,false,true,&status);
	if (!status)
		return 0;

	if (connected.length()==0)
		return 0;

	MPlug connectedbindposeplug = connected[0];

	MObject dagposeobject = connected[0].node();

	MFnDependencyNode dagposedepnode(dagposeobject);

	MObject localtoworldmatrixobject;

	status = connectedbindposeplug.getValue(localtoworldmatrixobject);
	if (!status)
		return 0;

	MFnMatrixData dMatrix(localtoworldmatrixobject);

	matrix = dMatrix.transformation(&status);
	if (!status)
		return 0;

	return 1;
}

int MTools::GetBindPoseFromJoint(MDagPath bonedagpath,MObject &bindposeobject)
{
	MStatus status;
	MFnDependencyNode jointdepnode(bonedagpath.node());

	MPlug bindposeplug = jointdepnode.findPlug("bindPose",&status);
	if (!status)
		return 0;
	
	MPlugArray connected;

	bindposeplug.connectedTo(connected,false,true,&status);
	if (!status)
		return 0;

	if (connected.length()>0)
	{
		bindposeobject = connected[0].node();
		return 1;
	}
	
	unsigned numChildren = bonedagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childdagpath(bonedagpath);

		childdagpath.push(bonedagpath.child(i));

		if(GetBindPoseFromJoint(childdagpath,bindposeobject))
			return 1;
	}
	
	return 0;
}

int MTools::HookUpBoneToBindPose(MDagPath jointdagpath,MObject bindpose,MObject prevjointcluster,MSelectionList vertices,MObject &jointcluster,MObject &boneset)
{
	MStatus status;
	MDGModifier dgmodifier;

	MObject joint = jointdagpath.node();
	MString bonefullpathname = jointdagpath.fullPathName();
	const char *fullpathname = bonefullpathname.asChar();

	MDagPath dagpath;
	MFnDagNode dagnode(joint);
	dagnode.getPath(dagpath);

	MFnIkJoint fnjoint(joint);
	MTransformationMatrix xformMatrix = fnjoint.transformation(&status);

	MMatrix localtoworld = dagpath.inclusiveMatrix();

	MMatrix localtoparent = fnjoint.transformationMatrix();

	if (bindpose != MObject::kNullObj)
	{
		if(!MTools::CreateJointClusterChain(jointdagpath,vertices,prevjointcluster,jointcluster,boneset))
		{	
			MGlobal::displayWarning("Couldn't create jointcluster chain.");
			return 0;
		}

//		get message, members and bindpose plugs
		MFnDependencyNode jointdepnode(joint);
		MPlug messageplug = jointdepnode.findPlug("message",&status);
		if (!status)
			return 0;
		MPlug bindposeplug = jointdepnode.findPlug("bindPose",&status);
		if (!status)
			return 0;

		MFnDependencyNode bindposedepnode(bindpose);

		MString bindposename = bindposedepnode.name();
		const char*debug = bindposename.asChar();

		MPlug membersplug = bindposedepnode.findPlug("members",&status);
		if (!status)
			return 0;

		int nextindex = membersplug.evaluateNumElements(&status);

		MPlug membersplug2 = membersplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;

//		get world matrix plug
		MPlug worldmatrixplug = bindposedepnode.findPlug("worldMatrix",&status);
		if (!status)
			return 0;

		nextindex = worldmatrixplug.evaluateNumElements(&status);

		MPlug worldmatrixplug2;
		
		worldmatrixplug2 = worldmatrixplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;
	
		MString plugname = worldmatrixplug2.name();
		const char *debug2 = plugname.asChar();

//		get xform matrix plug
		MPlug xformmatrixplug = bindposedepnode.findPlug("xformMatrix",&status);
		if (!status)
			return 0;

		MPlug xformmatrixplug2 = xformmatrixplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;

/////////////////////////////////////////////////////////////////////////////////
//		Set joint bind pose local to world

		MFnMatrixData dMatrix;
		MObject localtoworldmatrixobject = dMatrix.create(localtoworld,&status);
		if (!status)
			return 0;

		status = bindposeplug.setValue(localtoworldmatrixobject);
		if (!status)
			return 0;
/////////////////////////////////////////////////////////////////////////////////		
		
//		connect bindpose plug on joint to worldmatrix plug on bindpose
		status = dgmodifier.connect(bindposeplug,worldmatrixplug2);
		if (!status)
			return 0;

//		connect joint message to bindpose members	
		status = dgmodifier.connect(messageplug,membersplug2);
		if (!status)
			return 0;

		dgmodifier.doIt();
		
//		SET bindpose "xformMatrix"
		MFnMatrixData dMatrix2;
		MObject xformmatrixobject = dMatrix2.create(xformMatrix,&status);
		if (!status)
			return 0;

		status = xformmatrixplug2.setValue(xformmatrixobject);
		if (!status)
			return 0;

//		dgmodifier.doIt();

//		SET jointcluster "weightCompensationMatrix"
		MFnDependencyNode jointclusterdepnode(jointcluster);
		MPlug weightedCompensationMatrixPlug = jointclusterdepnode.findPlug("weightedCompensationMatrix",&status);

		status = weightedCompensationMatrixPlug.setValue(xformmatrixobject);
		if (!status)
			return 0;

//		SET jointcluster "geomMatrix"
		MPlug geomMatrixPlug = jointclusterdepnode.findPlug("geomMatrix",&status);

		nextindex = geomMatrixPlug.evaluateNumElements(&status);

		MPlug geomMatrixPlug2 = geomMatrixPlug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;

		MFnMatrixData dMatrix3;
		MMatrix ident;
		ident.setToIdentity();
		MObject geommatrixobject = dMatrix3.create(ident,&status);
		
		status = geomMatrixPlug2.setValue(geommatrixobject);
		if (!status)
			return 0;

//		SET jointcluser "bindPreMatrix"
		MPlug bindPreMatrixPlug = jointclusterdepnode.findPlug("bindPreMatrix",&status);
		
		MMatrix localtoworldinverse = localtoworld.inverse();
		MObject localtoworldinverseobject = dMatrix.create(localtoworldinverse,&status);

		status = bindPreMatrixPlug.setValue(localtoworldinverseobject);
		if (!status)
			return 0;
		
//		dgmodifier.doIt();

		return 1;
	}
	return 0;
}

// create joint cluster node, boneset, clusterhandle, link them all up
int MTools::CreateJointClusterChain(MDagPath bonedagpath,MSelectionList vertices,
									MObject prevjointcluster,
									MObject &jointcluster,
									MObject &boneset)
{
	MStatus status;
	MDGModifier dgmodifier;
	MDagModifier dagmodifier;
	
	MObject bone = bonedagpath.node();
	MString bonefullpathname = bonedagpath.fullPathName();
	const char *fullpathname = bonefullpathname.asChar();

//	Create Joint Cluster
	jointcluster = CreateJointCluster(bone);
	if (jointcluster==MObject::kNullObj)
		return 0;

//	Connect bone.worldMatrix to jointCluster.matrix
	MFnDependencyNode bonedepnode(bone);

	MPlug srcplug = bonedepnode.findPlug("worldMatrix",&status);
	if (!status)
		return 0;

	int nextindex = srcplug.evaluateNumElements(&status);
	
	MPlug plug;
	if (nextindex > 0)
	{
		plug = srcplug.elementByPhysicalIndex(0,&status);
		if (!status)
			return 0;
	}
	else
	{
		plug = srcplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;
	}
	
	MFnDependencyNode jointclusterdepnode(jointcluster);
	MPlug dstplug = jointclusterdepnode.findPlug("matrix",&status);
	if (!status)
		return 0;

	MString plugname = plug.name();
	MString dstplugname = dstplug.name();

	const char *debug = plugname.asChar();
	const char *debug0 = dstplugname.asChar();

	status = dgmodifier.connect(plug,dstplug);
	if (!status)
		return 0;

//	Create Bone Set
	boneset = CreateBoneSet(bone,vertices);
	if (boneset==MObject::kNullObj)
		return 0;

//	Connect jointCluster.message to boneSet.usedBy
	MPlug srcplug2 = jointclusterdepnode.findPlug("message",&status);
	if (!status)
		return 0;

	MFnDependencyNode bonesetdepnode(boneset);

	MPlug dstplug2 = bonesetdepnode.findPlug("usedBy",&status);
	if (!status)
		return 0;

	nextindex = dstplug2.numConnectedElements(&status);
	
	MPlug plug2 = dstplug2.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return 0;

	status = dgmodifier.connect(srcplug2,plug2);
	dgmodifier.doIt();

	MFnDependencyNode newdepnode;

//	Create ClusterHandle
	MObject jointclusterhandle = dagmodifier.createNode("clusterHandle",bone,&status);
	dagmodifier.doIt();
	if (!status)
		return 0;

	MFnDagNode dagnode(jointclusterhandle);
	dagnode.setIntermediateObject(true);

//	Connect ClusterHandle.clusterTransforms to JointCluster.clusterXForms
	MFnDependencyNode clusterhandledepnode(jointclusterhandle);

	MPlug clusterhandletransform = clusterhandledepnode.findPlug("clusterTransforms",&status);
	if (!status)
		return 0;

	nextindex = clusterhandletransform.numConnectedElements(&status);

	MPlug clusterhandletransform2 = clusterhandletransform.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return 0;

	MPlug clusterxform = jointclusterdepnode.findPlug("clusterXforms",&status);
	if (!status)
		return 0;

	status = dgmodifier.connect(clusterhandletransform2,clusterxform);
	dgmodifier.doIt();

//	Connect jointCluster.input.inputGeometry to previous jointCluster.outputGeometry
	if (prevjointcluster != MObject::kNullObj)
	{
		MFnDependencyNode prevjointclusterdepnode(prevjointcluster);

		const char *debug = prevjointclusterdepnode.name().asChar();

		const char *debug0 = jointclusterdepnode.name().asChar();

		MPlug prejointclusteroutputgeom = prevjointclusterdepnode.findPlug("outputGeometry",&status);
		if (!status)
			return 0;

		nextindex = prejointclusteroutputgeom.numConnectedElements(&status);
		
		MPlug prejointclusteroutputgeom2 = prejointclusteroutputgeom.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;

		MString prejointclusteroutputgeomname = prejointclusteroutputgeom2.name();
		const char *debug4 = prejointclusteroutputgeomname.asChar();
		
		MPlug jointclusterinput = jointclusterdepnode.findPlug("input",&status);
		if (!status)
			return 0;

		nextindex = jointclusterinput.numConnectedElements(&status);

		MPlug jointclusterinputplug = jointclusterinput.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return 0;
		
		MPlug inputgeometryplug = jointclusterinputplug.child(0,&status);

		MString inputgeometryplugname = inputgeometryplug.name();
		const char *debug3 = inputgeometryplugname.asChar();

		status = dgmodifier.connect(prejointclusteroutputgeom2,inputgeometryplug);
		dgmodifier.doIt();
		if (!status)
			return 0;	
	}

	return 1;
}

//	create joint cluster node
MObject MTools::CreateJointCluster(MObject bone)
{	
	MStatus status;
	MDGModifier dgmodifier;

	MFnDependencyNode bonedepnode(bone);

	MFnDependencyNode newdepnode;

	MObject newjointcluster = newdepnode.create("jointCluster",bonedepnode.name() + "Cluster",&status);
	if (!status)
		return MObject::kNullObj;

	return newjointcluster;
}

// create bone set
MObject MTools::CreateBoneSet(MObject bone,MSelectionList vertices)
{
	MStatus status;
	MFnSet mfnboneset;
	MDGModifier dgmodifier;

	int numverticesinset = vertices.length();
	
	MObject bonesetobject = mfnboneset.create(vertices,MFnSet::kVerticesOnly,false,&status);

	if (!status)
		return MObject::kNullObj;

	MFnDependencyNode depnode(bone);

	dgmodifier.renameNode(bonesetobject,depnode.name() + MString("Set"));
	dgmodifier.doIt();

	return bonesetobject;
}

// get bone set from jointcluster
MObject MTools::GetBoneSet(MObject jointcluster)
{
	MStatus status;

	MFnGeometryFilter geofilter(jointcluster);

	return geofilter.deformerSet(&status);;
}

void MTools::GetAssociatedSets(MDagPath dagpath,MObject component,MObjectArray &sets)
{
	MSelectionList sel;
	sel.clear();
	sel.add(dagpath,component);
	MGlobal::getAssociatedSets(sel,sets);
}

MStatus MTools::GetBoneLookup(MDagPath dagpath,std::vector<int> &keys,MObjectArray &bonesets)
{
	MStatus status;

	bonesets.clear();

	// Get all of the sets in maya and put them into
	// a selection list
	
	MFnDependencyNode depnode(dagpath.node());

	MStringArray results;
	MGlobal::executeCommand(MString("listSets -o ") + depnode.name() + MString(" -type 2"),results);

	int testlen = results.length();

	ConvertNamesToMObjects(results,bonesets);
	
	MFnMesh fnMesh(dagpath);

	unsigned numVerts = fnMesh.numVertices(&status);
	keys.clear();
	keys.resize(numVerts);

	MItMeshVertex vit(dagpath,MObject::kNullObj);

	for (; !vit.isDone(); vit.next())
	{
		MObjectArray sets;
		GetAssociatedSets(dagpath,vit.vertex(),sets);

		bool flag = false;

		for (int k = 0; k < (int)bonesets.length(); k++)
		{

			for (int j = 0; j < (int)sets.length(); j++)
			{
				if (sets[j] == bonesets[k])
				{
					keys[vit.index()] = k;
					flag = true;
					break;
				}
			}
			if (flag)
			{
				break;
			}
		}
		if (!flag)
			keys[vit.index()] = -1;
	}
	return MS::kSuccess;
}

void MTools::CreateLightAttenuationNetwork(MObject light)
{
	MStatus status;
	MDGModifier dgModifier;
	MObject lightshape;

	if (light.hasFn(MFn::kTransform))
	{
		MFnDagNode dagnode(light);
		lightshape = dagnode.child(0,&status);	
	}
	
	MFnDependencyNode depnode(lightshape);

	MString lightshapename = depnode.name();
	MString lightinfoname;

	MGlobal::executeCommand("createNode lightInfo",lightinfoname);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("connectAttr ") + lightshapename + MString(".worldMatrix ") +
		lightinfoname + MString(".worldMatrix"));
	dgModifier.doIt();

	MString animcurvename;
	MGlobal::executeCommand("createNode -name \"IntensityCurve\" animCurveUU",animcurvename);
	dgModifier.doIt();

	const char *debug = animcurvename.asChar();
	MGlobal::executeCommand(MString("connectAttr ") + lightinfoname + MString(".sampleDistance ") +
		animcurvename + MString(".input"));
	dgModifier.doIt();

	MGlobal::executeCommand(MString("connectAttr ") + animcurvename + MString(".output ") +
		lightshapename + MString(".intensity"));
	dgModifier.doIt();

	MGlobal::executeCommand(MString("setKeyframe -float 0 -value 3.0 ") + animcurvename);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("setKeyframe -float 790 -value 2.2 ") + animcurvename);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("setKeyframe -float 1000 -value 0 ") + animcurvename);
	dgModifier.doIt();
}

void MTools::CreateLightAttenuationNetwork(MObject light,MString animcurvename)
{
	MStatus status;
	MDGModifier dgModifier;
	MObject lightshape;

	if (light.hasFn(MFn::kTransform))
	{
		MFnDagNode dagnode(light);
		lightshape = dagnode.child(0,&status);	
	}
	
	MFnDependencyNode depnode(lightshape);

	MString lightshapename = depnode.name();
	MString lightinfoname;

	MGlobal::executeCommand("createNode lightInfo",lightinfoname);
	dgModifier.doIt();

	MGlobal::executeCommand(MString("connectAttr ") + lightshapename + MString(".worldMatrix ") +
		lightinfoname + MString(".worldMatrix"));
	dgModifier.doIt();

	const char *debug = animcurvename.asChar();

	MGlobal::executeCommand(MString("connectAttr ") + lightinfoname + MString(".sampleDistance ") +
		animcurvename + MString(".input"));
	dgModifier.doIt();

	MGlobal::executeCommand(MString("connectAttr ") + animcurvename + MString(".output ") +
		lightshapename + MString(".intensity"));
	dgModifier.doIt();
}

bool MTools::FindBoneByName(const char *name,const MDagPath &dagPath,MDagPath &result)
{
	MFnDependencyNode depNode(dagPath.node());
	if ((dagPath.apiType() == MFn::kJoint)&&(depNode.name()==name))
	{
		result = dagPath;
		return true;
	}
   // recurse through children for match
	UINT nNumChildren = dagPath.childCount();
	for (UINT i = 0; i < nNumChildren; i++)
	{
		MDagPath childPath(dagPath);
		childPath.push(dagPath.child(i));

		MDagPath founddagpath;
		if (FindBoneByName(name,childPath,result))
			return true;
	}
	return false;
}

//	replaces search key in mstring
void MTools::FindAndReplace(MString &mstr,const char *find,const char *replace)
{
	MString result;

	const char *debug = mstr.asChar();
	
	int len = strlen(find);
	for (int i = 0; i < (int)mstr.length(); i++)
	{
		MString test = mstr.substring(i,i+(len-1));
		const char *debug1 = test.asChar();
		if (mstr.substring(i,i+len-1)==MString(find))
		{
			result += MString(replace);
			i+=(len-1);
			continue;
		}
		result += mstr.substring(i,i);
		const char *debug0 = result.asChar();
	}
	mstr = result;
}

// Set orientation is currently not working. Who knows why?!
int MTools::SetParentJointOrientationToChildBone(MDagPath parentpath)
{
	MStatus status;
		
	MObject childobject = parentpath.child(0,&status);
	
	MDagPath childpath;
	MFnDagNode dagnode(childobject);
	dagnode.getPath(childpath);

	MFnIkJoint jointchild(childpath);
	MFnIkJoint jointparent(parentpath);

	MFnDependencyNode depnode(parentpath.node());
	const char *debug = depnode.name().asChar();

	MVector childpos = jointchild.translation(MSpace::kWorld,&status);
	MVector parentpos = jointparent.translation(MSpace::kWorld,&status);

	MVector bonevect = childpos - parentpos;
	bonevect.normalize();

	MVector identity(0,0,1);

	MQuaternion quat = identity.rotateTo(bonevect);
	
	jointparent.setOrientation(quat);

	return 1;
}

int MTools::GetInclusiveOrientation(MDagPath bonepath,MQuaternion &orientation)
{
	MQuaternion quat;
	quat = orientation = MQuaternion::identity;

	MDagPath dagpath(bonepath);

	int numParents = NumParents(bonepath);

	for (int i = 0; i < (numParents+1); i++)
	{
		if (dagpath.apiType() == MFn::kJoint)
		{
			MFnIkJoint joint(dagpath);
			joint.getOrientation(quat);
			orientation *= quat;
			dagpath.pop();
		}
	}
	return 1;
}

int MTools::GetInclusiveBindPoseMatrix(MDagPath bonepath,MMatrix &resultmatrix)
{
#if 0
	resultmatrix.setToIdentity();
	
	MDagPath dagpath(bonepath);

	int numParents = NumParents(bonepath);

	for (int i = 0; i < (numParents+1); i++)
	{
		if (dagpath.apiType() == MFn::kJoint)
		{
			MTransformationMatrix bpjointmatrix;
			if (!MTools::GetBindPoseMatrix(dagpath,bpjointmatrix))
				return 0;

			MMatrix temp = bpjointmatrix.asMatrix();

			resultmatrix *= temp;
			dagpath.pop();
		}
	}
#else if 1
	MDagPath dagpath(bonepath);
	MTransformationMatrix bpjointmatrix;
	// WARNING: LOOKS LIKE BIND POSE MATRIX IS INCLUSIVE
	if (!MTools::GetBindPoseMatrix(dagpath,bpjointmatrix))
		return 0;
	resultmatrix = bpjointmatrix.asMatrix();
#endif
	return 1;
}

int MTools::GetExclusiveBindPoseMatrix(MDagPath bonepath,MMatrix &resultmatrix)
{
#if 0
	resultmatrix.setToIdentity();
	
	MDagPath dagpath(bonepath);

	dagpath.pop();

	int numParents = NumParents(dagpath);

	for (int i = 0; i < (numParents+1); i++)
	{
		if (dagpath.apiType() == MFn::kJoint)
		{
			MTransformationMatrix bpjointmatrix;
			if (!MTools::GetBindPoseMatrix(dagpath,bpjointmatrix))
				return 0;

			MMatrix temp = bpjointmatrix.asMatrix();

			resultmatrix *= temp;
			dagpath.pop();
		}
	}
#else if 1
	MDagPath dagpath(bonepath);

	dagpath.pop();

	MTransformationMatrix bpjointmatrix;
	// WARNING: LOOKS LIKE BIND POSE MATRIX IS INCLUSIVE
	if (!MTools::GetBindPoseMatrix(dagpath,bpjointmatrix))
		return 0;

	resultmatrix = bpjointmatrix.asMatrix();
#endif

	return 1;
}

int MTools::GetChildBoneThatMatchesOrientationVector(MDagPath bonepath,MDagPath &endbonepath)
{
	MStatus status;

	MString bonename = bonepath.fullPathName();
	const char *debug = bonename.asChar();

	MVector boneidentity(0,0,1);

	MQuaternion quat;

	GetInclusiveOrientation(bonepath,quat);
	
	MVector orientationvector = boneidentity.rotateBy(quat);

	orientationvector.normalize();//isn't really necessary
	
	MTransformationMatrix bpjointmatrix;
	if (!GetBindPoseMatrix(bonepath,bpjointmatrix))
		return 0;

	MVector pivotpos = bpjointmatrix.translation(MSpace::kWorld,&status);
	if (!status)
		return 0;

	unsigned numChildren = bonepath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childpath(bonepath);
		childpath.push(bonepath.child(i));

		MString bonename = childpath.fullPathName();
		const char *debug1 =bonename.asChar();
		
		MTransformationMatrix bpjointmatrix;
		if (!GetBindPoseMatrix(childpath,bpjointmatrix))
			return 0;

		MVector endpos = bpjointmatrix.translation(MSpace::kWorld,&status);
		if (!status)
			return 0;

		MVector bonevect = endpos-pivotpos;
		MVector crazyvect = bonevect.rotateBy(quat.inverse());

		bonevect.normalize();
		crazyvect.normalize();
		
		if (bonevect.isParallel(orientationvector,0.1))
		{
			endbonepath = childpath;
			return 1;
		}
	}
	return 0;
}

int MTools::BakeTransformAndCenterPivot(MDagPath dagpath)
{
	MDGModifier dgModifier;
	MGlobal::executeCommand("performFreezeTransformations(0)");
	dgModifier.doIt();

	MFnTransform transform(dagpath.node());
	MStatus status;
	
	status = transform.setRotatePivot(MPoint(0,0,0),MSpace::kTransform,true);
	if (!status)
		return 0;

	status = transform.setScalePivot(MPoint(0,0,0),MSpace::kTransform,true);
	if (!status)
		return 0;

	return 1;
}

int MTools::StartProgressBar(int steps,MString displayline)
{
	MDGModifier dgmodifier;
	MString command = MString("progressBar -edit -beginProgress -isInterruptable false -status \"") + displayline +  MString("\" -maxValue ") + steps + MString(" $gMainProgressBar");
	MGlobal::executeCommand(command);
	dgmodifier.doIt();
	return 1;
}

int MTools::StepProgressBar(int step)
{
	MDGModifier dgmodifier;
	MString command = MString("progressBar -edit -step ") + step + MString(" $gMainProgressBar");
	MGlobal::executeCommand(command);
	dgmodifier.doIt();
	return 1;
}

int MTools::StopProgressBar()
{
	MDGModifier dgmodifier;
	MString command = MString("progressBar -edit -endProgress $gMainProgressBar");
	MGlobal::executeCommand(command);
	dgmodifier.doIt();
	return 1;
}

int MTools::IsPlugConnectedToPlug(MPlug plugA,MPlug plugB)
{
	MStatus status;
	MPlugArray connected;
	MString plugAname = plugA.name();
	MString plugBname = plugB.name();

	const char *debugA = plugAname.asChar();
	const char *debugB = plugBname.asChar();

	plugA.connectedTo(connected,true,true,&status);

	for (int i = 0; i < (int)connected.length(); i++)
	{
		MString plugCname = connected[i].name();
		const char *debugC = plugCname.asChar();
		if (plugB == connected[i])
			return 1;
	}
	return 0;
}

int MTools::IsMeshConnectedToUVChooser(MDagPath dagpath,MObject uvchooser)
{
	MStatus status;
	MDGModifier dgmodifier;
	MObject shape = dagpath.child(0);

	MFnDependencyNode depnode(shape);

	MPlug uvSetplug = depnode.findPlug("uvSet",&status);
	int numelements = uvSetplug.evaluateNumElements(&status);
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
					MString uvsetnameplugname = uvsetnameplug.name();
					const char *debug = uvsetnameplugname.asChar();
	
					MFnDependencyNode depnode(uvchooser);
					MPlug uvSetsplug = depnode.findPlug("uvSets",&status);
					MString plugname3 = uvSetsplug.name();
					const char *debug3 = plugname3.asChar();
					if (status)
					{
						int numelements = uvSetsplug.evaluateNumElements(&status);

						for (int j = 0; j < numelements; j++)
						{
							MPlug uvchooseruvsetplug = uvSetsplug.elementByLogicalIndex(j,&status);
							MString plugname2 = uvchooseruvsetplug.name();
							const char *debug1 = plugname2.asChar();

							if (IsPlugConnectedToPlug(uvsetnameplug,uvchooseruvsetplug))
							{
								return 1;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

//#define ReplaceWithInitialShadingGroup
// Obsolete: Thanks to Maya 5.0!
int MTools::BakeVertexColors(MSelectionList selection,bool shadowmaps,bool resuseshadowmaps)
{
	MStatus status;
	MDGModifier dgmodifier;
	
	MFloatMatrix cameramatrix;
	cameramatrix.setToIdentity();
	
	for ( int i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);
		
		if (dagpath.hasFn(MFn::kTransform))
			dagpath.push(dagpath.child(0));
		
		if (!dagpath.hasFn(MFn::kMesh))
			continue;

		MFnMesh fnmesh(dagpath);

#ifdef ReplaceWithInitialShadingGroup

		MObjectArray shaders;
		MIntArray indicies;

		shaders.clear();
		indicies.clear();

		status = fnmesh.getConnectedShaders(0,shaders,indicies);
		if (!status)
			continue;

		MGlobal::executeCommand(MString("sets -e -forceElement initialShadingGroup ") + dagpath.fullPathName());
		fnmesh.syncObject();

#endif

		MFloatPointArray points;
		MFloatVectorArray normals;
		
		points.clear();
		normals.clear();
		
		status = fnmesh.getPoints(points,MSpace::kWorld);
		if (!status)
			return 0;
		
		
		MIntArray vlist;
		MIntArray flist;
		vlist.clear();
		flist.clear();
		
		MFloatPointArray newpoints;
		newpoints.clear();
		
		MItMeshPolygon polyiter(dagpath,MObject::kNullObj,&status);
		if (!status)
			return 0;
		
		for ( polyiter.reset(); !polyiter.isDone(); polyiter.next() )
		{
			MIntArray vertices;
			MVectorArray vertexnormals;
			vertices.clear();
			vertexnormals.clear();
			polyiter.getVertices(vertices);
			polyiter.getNormals(vertexnormals,MSpace::kWorld);
			
			int faceindex = polyiter.index();

			for (int j = 0; j < (int)vertices.length(); j++)
			{
				flist.append(faceindex);
				vlist.append(vertices[j]);
				newpoints.append(points[vertices[j]]);
				normals.append(vertexnormals[j]);
			}
		}
				
		if (!BakeVertexColorsPerShadingGroup(MString("initialShadingGroup"),dagpath,vlist,flist,newpoints,normals,shadowmaps,resuseshadowmaps))
			return 0;

#ifdef ReplaceWithInitialShadingGroup
		
		MGlobal::executeCommand("sets -e -cl initialShadingGroup ");
		dgmodifier.doIt();
		fnmesh.syncObject();
		
		status = ConnectShadingGroups(dagpath,shaders,indicies);
		if (!status)
			return 0;

		fnmesh.syncObject();

#endif

	}
	return 1;	
}

int MTools::BakeVertexColorsPerShadingGroup(MString shadername,MDagPath &dagpath,
											MIntArray &vlist,MIntArray &flist,
											MFloatPointArray &points,MFloatVectorArray &normals,
											bool shadowmaps,bool reuseshadowmaps)
{
	MStatus status;

	const char *debug = shadername.asChar();
	
	MFnMesh fnmesh(dagpath);

	MFloatMatrix cameramatrix;
	cameramatrix.setToIdentity();

	MFloatVectorArray resultcolors,resulttransparencies;
	resultcolors.clear();
	resulttransparencies.clear();

	status = MRenderUtil::sampleShadingNetwork(shadername,points.length(),shadowmaps,reuseshadowmaps,cameramatrix,&points,NULL,NULL,&normals,NULL,NULL,NULL,NULL,resultcolors,resulttransparencies);
	if (!status)
	{
		MGlobal::displayError("Failed to sample shading network.");
		return 0;
	}

	int numresultcolors = resultcolors.length();
	int numresulttransparencies = resulttransparencies.length();

	MColorArray colorarray;
	colorarray.clear();
	colorarray.setLength(resultcolors.length());

	for (int i = 0; i < numresultcolors; i++)
		colorarray[i] = MColor(resultcolors[i].x,resultcolors[i].y,resultcolors[i].z);
	
	status = fnmesh.setFaceVertexColors(colorarray,flist,vlist);
	if (!status)
	{
		MGlobal::displayError("Failed to set face vertex colors.");
		return 0;
	}

	fnmesh.updateSurface();

	return 1;
}

int MTools::BakeBoneScaleIntoTranslation(MDagPath dagpath)
{
	MFnIkJoint joint(dagpath);

	for (int i = 0; i < (int)dagpath.childCount(); i++)
	{
		MDagPath childpath(dagpath);
		childpath.push(dagpath.child(i));

		BakeBoneScaleIntoTranslation(childpath);
		
		MFnIkJoint childjoint(childpath);

		MVector translate = childjoint.translation(MSpace::kWorld);

		double scale[3] = { 1, 1, 1 };

		joint.setScale(scale);

		childjoint.setTranslation(translate,MSpace::kWorld);
	}
	return 1;
}

int MTools::MakeObjectsNamesSimilar(MDagPath srcpath,MSelectionList selection)
{
	for (int i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);
		MakeObjectNamesSimilar(srcpath,dagpath,i);
	}
	return 1;
}

int MTools::MakeObjectNamesSimilar(MDagPath srcpath,MDagPath dstpath,int index)
{
	
	for (int i = 0; i < (int)srcpath.childCount(); i++)
	{
		MDagPath srcchildpath(srcpath);
		srcchildpath.push(srcpath.child(i));
		MDagPath dstchildpath(dstpath);
		dstchildpath.push(dstpath.child(i));
		
		MakeObjectNamesSimilar(srcchildpath,dstchildpath,index);
	}
	
	MDagModifier dagmodifier;
	MFnDependencyNode depnode(srcpath.node());
	char number[128];
	itoa(index,number,10);
	MString newname = ExtractName(depnode.name()) + MString(number);
	const char *debug = newname.asChar();
	dagmodifier.renameNode(dstpath.node(),newname);
	dagmodifier.doIt();
	
	return 1;
}

int MTools::MakeObjectsNamesDifferent(MDagPath srcpath,MSelectionList selection)
{
	for (int i = 0; i < (int)selection.length(); i++)
	{
		MDagPath dagpath;
		selection.getDagPath(i,dagpath);
		MakeObjectNamesDifferent(srcpath,dagpath,i);
	}
	return 1;
}

int MTools::MakeObjectNamesDifferent(MDagPath srcpath,MDagPath dstpath,int index)
{
	for (int i = 0; i < (int)srcpath.childCount(); i++)
	{
		MDagPath srcchildpath(srcpath);
		srcchildpath.push(srcpath.child(i));
		MDagPath dstchildpath(dstpath);
		dstchildpath.push(dstpath.child(i));
		
		MakeObjectNamesDifferent(srcchildpath,dstchildpath,index);
	}
	
	MDagModifier dagmodifier;
	MFnDependencyNode depnode(srcpath.node());
	char number[128];
	itoa(index,number,10);
	MString newname = ExtractName(depnode.name()) + MString(number) + MString("_");
	
	const char *debug = newname.asChar();
	dagmodifier.renameNode(dstpath.node(),newname);
	dagmodifier.doIt();
	
	return 1;
}

void MTools::CopyVertexColors(MDagPath srcPath,MDagPath dstPath)
{
	MStatus status;

	MFnDependencyNode fnSrcNode(srcPath.node());
	MFnDependencyNode fnDstNode(dstPath.node());
	// if they are the same mesh then kick out
	if ((int)MTools::GetMPtrBase(fnSrcNode)==(int)MTools::GetMPtrBase(fnDstNode))
		return;

	MFnMesh fnDstMesh(dstPath);

	MObject mComponent = MObject::kNullObj;

	MItMeshPolygon polyIter(srcPath, mComponent, &status);

	if (status != MS::kSuccess) 
	{
		MGlobal::displayError("Failure in MItMeshPolygon initialization.");
		return;
	}

	MIntArray faceRemoveColorArray;

	MColorArray colorsToAdd;
	MIntArray faceArray;
	MIntArray vertexArray;

	// Faces
	int numFaces = 0;
	int numVertices = 0;
	for ( ; !polyIter.isDone(); polyIter.next() )
	{
		int polyVertexCount = polyIter.polygonVertexCount();

		MIntArray vertexIndicies;
		status = polyIter.getVertices(vertexIndicies);

		int numVertexColors;
		polyIter.numColors(numVertexColors);
		if (numVertexColors > 0)
		{
			MColorArray colors;
			polyIter.getColors(colors);
			if (colors.length() == polyVertexCount)
			{
				for (int index=0;index<polyVertexCount;index++)
				{
					MColor newColor = colors[index];
					if (newColor.r <0)
						newColor.r = 0.0f;
					if (newColor.g <0)
						newColor.g = 0.0f;
					if (newColor.b <0)
						newColor.b = 0.0f;
					if (newColor.a <0)
						newColor.a = 1.0f;
					
					colorsToAdd.append(newColor);
					faceArray.append(numFaces);
					vertexArray.append(vertexIndicies[index]);
				}
			}
		}
		faceRemoveColorArray.append(numFaces);
		numVertices+= polyVertexCount;
		numFaces++;
	}
	status = fnDstMesh.removeFaceColors(faceRemoveColorArray);
	if (status != MS::kSuccess) 
	{
		MGlobal::displayError("Error removing colors.");
		return;
	}
	int numColorsToAdd = colorsToAdd.length();
	int faceArrayLength = faceArray.length();
	int vertexArrayLength = vertexArray.length();

	status = fnDstMesh.setFaceVertexColors(colorsToAdd,faceArray,vertexArray);
	if (status != MS::kSuccess) 
	{
		MGlobal::displayError("Error copying colors.");
		return;
	}
}

int MTools::ArePolygonObjectsIdentical(MDagPath dagpathA,MDagPath dagpathB)
{
	MStatus status;
	MFnMesh fnmeshA(dagpathA);
	MFnMesh fnmeshB(dagpathB);

	if (fnmeshA.numVertices()!=fnmeshB.numVertices())
		return 0;

	MPointArray verticesA,verticesB;

	status = fnmeshA.getPoints(verticesA,MSpace::kWorld);
	if (!status)
		return 0;

	status = fnmeshB.getPoints(verticesB,MSpace::kWorld);
	if (!status)
		return 0;

	for (int i = 0; i < (int)verticesA.length(); i++)
	{
		if ((verticesA[i].x != verticesB[i].x)&&(verticesA[i].y != verticesB[i].y)&&(verticesA[i].z != verticesB[i].z))
			return 0;
	}

	return 1;
}


int MTools::ParentMObject(MObject child,MObject parent)
{
	MDGModifier dgmodifier;
	MFnDependencyNode parentdepnode(parent);
	MFnDependencyNode childdepnode(child);
	MGlobal::executeCommand(MString("parent ") + childdepnode.name() + MString(" ") + parentdepnode.name());
	dgmodifier.doIt();
	return 1;
}

int MTools::FindCharacters(MDagPath dagpath,MObjectArray &characterobjects)
{
	MStatus status;
	MObject characterobject;
	int flag = 0;

	int result = LookThroughTransformPlugsToCharacter(dagpath,characterobject);
	if (result)
	{
		characterobjects.append(characterobject);
		flag = 1;
//		return 1;
	}
	
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childdagpath(dagpath);

		childdagpath.push(dagpath.child(i));

		result = FindCharacters(childdagpath,characterobjects);
		if (result)
			flag = 1;
//			return 1;
	}
	return flag;
}

int MTools::LookThroughTransformPlugsToCharacter(MDagPath dagpath,MObject &characterobject)
{
	MStatus status;
	MFnDependencyNode depnode(dagpath.node());

	// get our MAYA attribute plugs
	MPlug translateX = depnode.findPlug("translateX",&status);
	MPlug translateY = depnode.findPlug("translateY",&status);
	MPlug translateZ = depnode.findPlug("translateZ",&status);

	MPlug rotateX = depnode.findPlug("rotateX",&status);
	MPlug rotateY = depnode.findPlug("rotateY",&status);
	MPlug rotateZ = depnode.findPlug("rotateZ",&status);

	MPlug scaleX = depnode.findPlug("scaleX",&status);
	MPlug scaleY = depnode.findPlug("scaleY",&status);
	MPlug scaleZ = depnode.findPlug("scaleZ",&status);

	int result = FollowConnectionToCharacter(translateX,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(translateY,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(translateZ,characterobject);
	if (result)
		return result;
	
	result = FollowConnectionToCharacter(rotateX,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(rotateY,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(rotateZ,characterobject);
	if (result)
		return result;

	result = FollowConnectionToCharacter(scaleX,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(scaleY,characterobject);
	if (result)
		return result;
	result = FollowConnectionToCharacter(scaleZ,characterobject);
	if (result)
		return result;

	return result;
}

int MTools::FollowConnectionToCharacter(MPlug plug,MObject &characterobject)
{
	MStatus status;
	MPlugArray connected;
	plug.connectedTo(connected,true,false,&status);
	if (connected.length()==0)
		return 0;
	
	if (connected[0].node().hasFn(MFn::kCharacter))
	{
		characterobject = connected[0].node();
		return 1;
	}
	return 0;
}

int MTools::GetDisplayLayer(MString objectname,MString &layername)
{
	MStringArray layernames;
	MTools::GetDisplayLayers(layernames);
	for (int i = 0; i < (int)layernames.length(); i++)
	{
		MStringArray nodenames;
		MTools::GetNodesInLayer(layernames[i],nodenames);
		for (int j = 0; j < (int)nodenames.length(); j++)
		{
			if (objectname == nodenames[j])
			{
				layername = layernames[i];
				return 1;
			}
		}
	}
	return 0;
}

int MTools::ExploreChildren(MFn::Type mfntype,MDagPath dagpath,MDagPath &result)
{
	MStatus status;
	MFn::Type test = dagpath.apiType();
	if (dagpath.apiType() == mfntype)
	{
		result = dagpath;
		return 1;
	}
	
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childdagpath(dagpath);

		childdagpath.push(dagpath.child(i));

		if(ExploreChildren(mfntype,childdagpath,result))
			return 1;
	}
	return 0;
}


void MTools::CollectBoneAndVertexAssignments(MDagPath dagpath,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups)
{
	MStatus status;
	MString bonename = dagpath.fullPathName();
	const char *debug = bonename.asChar();

	if (dagpath.apiType() == MFn::kJoint)
	{
		MFnIkJoint joint(dagpath);
		MObject jointclusterobject = GetJointCluster(dagpath.node());
		if ( jointclusterobject != MObject::kNullObj )
		{
			MObject bonesetobject = GetBoneSet(jointclusterobject);
			if (bonesetobject != MObject::kNullObj)
			{
				MFnSet boneset(bonesetobject);
				MSelectionList members,newmembers;
				members.clear();
				newmembers.clear();
				boneset.getMembers(members,true);

				const char *debug0 = boneset.name().asChar();

				for (int i = 0; i < (int)members.length(); i++)
				{
					MDagPath meshdagpath;
					MObject component;

					members.getDagPath(i,meshdagpath,component);

					MItMeshVertex viter(meshdagpath,component,&status);

					for (;!viter.isDone();viter.next())
					{
						newmembers.add(meshdagpath,viter.vertex());
					}
					
					if (newmembers.length()>0)
					{
						vertexgroups.push_back(newmembers);
						bones.push_back(dagpath);
					}
				}
			}
		}
	}

	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childdagpath(dagpath);

		childdagpath.push(dagpath.child(i));

		CollectBoneAndVertexAssignments(childdagpath,bones,vertexgroups);
	}		
}

void MTools::ConvertBoneAndVertexAssignments(MDagPath newrootbone,MDagPath newmesh,std::vector<MDagPath> &bones,std::vector<MSelectionList> &vertexgroups,
											 std::vector<MDagPath> &newbones,std::vector<MSelectionList> &newvertexgroups)
{
	MStatus status;
	newbones.clear();
	newvertexgroups.clear();
	for (int i = 0; i < (int)bones.size(); i++)
	{
		MFnDependencyNode bonedepnode(bones[i].node());
		MDagPath newbone;
		
		MString bonename = bones[i].fullPathName();

		const char *debug = bonename.asChar();

		if (!FindBoneByName(bonedepnode.name().asChar(),newrootbone,newbone))
			continue;

		MSelectionList *members = &vertexgroups[i];
		MSelectionList newmembers;
		
		newmembers.clear();

		int memberslength = members->length();
		
		for (int j = 0; j < (int)members->length(); j++)
		{
			MDagPath dagpath;
			MObject component;

			members->getDagPath(j,dagpath,component);
			
			MItMeshVertex viter(dagpath,component);

			int index = viter.index();
			MItMeshVertex newviter(newmesh);
			for (;!newviter.isDone();newviter.next())
				if (newviter.index()==index)
				{
					newmembers.add(newmesh,newviter.vertex());
					break;
				}
		}
		if (members->length()>0)
		{
			newbones.push_back(newbone);
			newvertexgroups.push_back(newmembers);
		}
	}
}

int MTools::DuplicateBoundGeometryAndCharacter(MDagPath dagpath,MDagPath &result)
{
	DuplicateBoundGeometry(dagpath,result);

	MString resultpathname = result.fullPathName();

	const char *debug = resultpathname.asChar();

	MObjectArray characterobjects;
	if (FindCharacters(dagpath,characterobjects))
	{
		if (!CopyCharacter(characterobjects[0],result))
			return 0;
	}
	return 1;
}

void MTools::CopyNames(MDagPath sourcedagpath, MDagPath destdagpath)
{
	MStatus status;


	MFnDependencyNode srcdepnode(sourcedagpath.node());
	MFnDependencyNode dstdepnode(destdagpath.node());

	MString srcpathname = sourcedagpath.fullPathName();
	MString dstpathname = destdagpath.fullPathName();

	const char *debug = srcpathname.asChar();
	const char *debug0 = dstpathname.asChar();

	MFnDagNode dagnode(sourcedagpath.node());
	if (!dagnode.isIntermediateObject())
	{
		MString origname = srcdepnode.name();
		dstdepnode.setName(origname);
	}

	unsigned numChildren = sourcedagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath sourcechilddagpath(sourcedagpath);
		MDagPath destchilddagpath(destdagpath);

		sourcechilddagpath.push(sourcedagpath.child(i));
		destchilddagpath.push(destdagpath.child(i));

		CopyNames(sourcechilddagpath,destchilddagpath);
	}
}

int MTools::DuplicateBoundGeometry(MDagPath dagpath,MDagPath &result)
{
	MStatus status;
	MDGModifier dgmodifier;

	MStringArray results;

	MFnDagNode dagnode(dagpath);
	MObject newobject = dagnode.duplicate(false,false,&status);
	if (!status)
		return 0;

	MFnDagNode dagnode2(newobject);
	MDagPath newdagpath;
	dagnode2.getPath(newdagpath);

	CopyNames(dagpath,newdagpath);

	MString pathname = newdagpath.fullPathName();
	const char *debug = pathname.asChar();

	MDagPath skeletonroot;
	if	(!ExploreChildren(MFn::kJoint,dagpath,skeletonroot))
		return 0;

	MDagPath skeletonroot2;
	if	(!ExploreChildren(MFn::kJoint,newdagpath,skeletonroot2))
		return 0;

	MDagPath meshshape;
	if	(!ExploreChildren(MFn::kMesh,newdagpath,meshshape))
		return 0;

	std::vector<MDagPath> bones,newbones;
	std::vector<MSelectionList> vertexgroups,newvertexgroups;

	CollectBoneAndVertexAssignments(skeletonroot,bones,vertexgroups);
	ConvertBoneAndVertexAssignments(skeletonroot2,meshshape,bones,vertexgroups,newbones,newvertexgroups);

	meshshape.pop();

	if (!BindSkin(meshshape,newbones,newvertexgroups))
		return 0;

	result = newdagpath;
	
	CopyShadersBranch(dagpath,newdagpath);
	
	return 1;
}

int MTools::FindMemberNameInSkeleton(MObject member,MDagPath dagpath,MDagPath &result)
{
	MStatus status;
	MFnDependencyNode depnode(member);
	MFnDependencyNode depnode2(dagpath.node());

	MString membername = depnode.name();
	MString dagnodename = depnode2.name();

	const char *debug1 = membername.asChar();

//	MString name = ExtractName(membername);
//	MString name0 = ExtractName(dagnodename);

//	const char *debug = name.asChar();
//	const char *debug0 = name0.asChar();
	
	if (membername == dagnodename)
	{
		result = dagpath;
		return 1;
	}
	
	unsigned numChildren = dagpath.childCount();
	for (unsigned i = 0; i < numChildren; i++)
	{
		MDagPath childdagpath(dagpath);

		childdagpath.push(dagpath.child(i));

		int flag = FindMemberNameInSkeleton(member,childdagpath,result);
		if (flag)
			return 1;
	}
	return 0;
}

int MTools::CopyCharacter(MObject srcobject,MDagPath dagpath)
{
	MStatus status;
	MDagPath dagpathroot;
	MDGModifier fMod;

	MString dagpathname = dagpath.fullPathName();
	const char *debug1 = dagpathname.asChar();

	if (srcobject.apiType() != MFn::kCharacter)
		return 0;

	// get root dagpath
//	MTools::GetTransformRoot (dagpath, dagpathroot);

	MDagPath skeletonroot;
	ExploreChildren(MFn::kJoint,dagpath,skeletonroot);

	MString skeletonrootname = skeletonroot.fullPathName();
	const char *debug = skeletonrootname.asChar();

	MFnCharacter srccharacter(srcobject);
	MSelectionList members;
	srccharacter.getMembers(members,true);

	MSelectionList newmembers;

	IntHash membermap;

	for (int i = 0; i < (int)members.length(); i++)
	{
		MObject member;
		MDagPath result;
		members.getDependNode(i,member);
		MFnDependencyNode depnode(member);
		const char *debug = depnode.name().asChar();
		if (!membermap.HasKey((int)MTools::GetMPtrBase(depnode)))
		{
			if (FindMemberNameInSkeleton(member,skeletonroot,result))
			{
				MString bonepath = result.fullPathName();
				const char *debug3 = bonepath.asChar();

				status = newmembers.add(result);
				if (!status)
					MGlobal::displayError("Couldn't add bone to new character.");
			}
			membermap.AddKey((int)MTools::GetMPtrBase(depnode),1);
		}
	}

	MObject newcharacterobject;
	MFnCharacter fncharacter;
	MObjectArray characterobjects;

//	if (!MTools::FindCharacters(skeletonroot,characterobjects))
//	{
		newcharacterobject = fncharacter.create(newmembers,MFnSet::kNone,&status);
		if (!status)
			return MS::kFailure;
//	}
//	else
//		newcharacterobject = characterobjects[0];

//	fncharacter.setObject(newcharacterobject);
	
	if (!CopyClips(srcobject,newcharacterobject))
		return 0;
	
	return 1;
}

int MTools::GetInstanceClipsPerCharacter(MObject characterobject,MObject sourceclipobject,MObjectArray &instanceclips)
{
	MStatus status;
	MFnDependencyNode depnode(sourceclipobject);

	int hashvalue = (int)MTools::GetMPtrBase(depnode);

	MFnCharacter character(characterobject);

	int schedclipcount = character.getScheduledClipCount(&status);

	instanceclips.clear();

	for (int i = 0; i < schedclipcount; i++)
	{
		MObject srcshedclipobject = character.getScheduledClip(i);

		MFnClip srcshedclip(srcshedclipobject);

		MObject sourceclipobject = srcshedclip.sourceClip(&status);

		MFnDependencyNode depnode2(sourceclipobject);

		int hashvalue2 = (int)MTools::GetMPtrBase(depnode2);

		if (hashvalue == hashvalue2)
			instanceclips.append(sourceclipobject);
	}

	return 1;
}

int MTools::GetInstanceClipsPerScene(MObject sourceclipobject,MObjectArray &characterobjects,MObjectArray &instanceclips)
{
	MStatus status;
	MFnDependencyNode depnode(sourceclipobject);

	int hashvalue = (int)MTools::GetMPtrBase(depnode);

	characterobjects.clear();
	instanceclips.clear();

	MItDependencyNodes nodeIt;
	for (; !nodeIt.isDone(); nodeIt.next())
	{
		MObject node = nodeIt.item();
		if (node.isNull())
				continue;

		if (node.hasFn (MFn::kCharacter))
		{
			MFnCharacter character(node);

			int schedclipcount = character.getScheduledClipCount(&status);

			for (int i = 0; i < schedclipcount; i++)
			{
				MObject srcshedclipobject = character.getScheduledClip(i);

				MFnClip srcshedclip(srcshedclipobject);

				MObject sourceclipobject = srcshedclip.sourceClip(&status);

				MFnDependencyNode depnode2(sourceclipobject);

				int hashvalue2 = (int)MTools::GetMPtrBase(depnode2);

				if (hashvalue == hashvalue2)
					instanceclips.append(sourceclipobject);
			}
			characterobjects.append(node);
		}
	}

	return 1;
}

int MTools::CopyClipAttributes(MObject srcclipobject,MObject dstclipobject)
{
	MStatus status;
	MFnClip srcclip(srcclipobject);

	MTime starttime = srcclip.getStartFrame(&status);

	double cycle = srcclip.getCycle(&status);
	double scale = srcclip.getScale(&status);

	MString clipname = srcclip.name(&status);

	MFnClip dstclip(dstclipobject);

	MDGModifier fMod;
	
	dstclip.setScale(scale);
	dstclip.setCycle(cycle);
	dstclip.setName(clipname,&status);

	dstclip.setStartFrame(starttime);
	
	return 1;
}

int MTools::CopyClips(MObject srcobject,MObject dstobject)
{
	MStatus status;

	MFnCharacter srccharacter(srcobject);
	MFnCharacter dstcharacter(dstobject);

	int srcclipcount = srccharacter.getSourceClipCount(&status);
	
	int i;

	IntHash sourceclipmap;
	MObjectArray sourceclips;

	for (i = 0; i < srcclipcount; i++)
	{
		MObject srcclipobject = srccharacter.getSourceClip(i);

		MObject dstclipobject;

		MDGModifier fMod;
		
		MFnClip srcclip(srcclipobject);

		MTime starttime = srcclip.getSourceStart(&status);
		MTime duration = srcclip.getSourceDuration(&status);

		double cycle = srcclip.getCycle(&status);
		double scale = srcclip.getScale(&status);

		MString clipname = srcclip.name(&status);

		MFnClip dstclip;

		dstclipobject = dstclip.createSourceClip(starttime,duration,fMod,&status);
		if (!status)
			continue;

		MFnDependencyNode depnode(srcclipobject);

		if (!sourceclipmap.HasKey((int)MTools::GetMPtrBase(depnode)))
		{
			sourceclipmap.AddKey((int)MTools::GetMPtrBase(depnode),sourceclips.length());
			sourceclips.append(dstclipobject);
		}
		
		dstclip.setName(clipname,&status);
		
		// attach source to character
		status = dstcharacter.attachSourceToCharacter(dstclipobject,fMod);
		if (!status)
			continue;

		MPlugArray dstmemberplugs;
		dstcharacter.getMemberPlugs(dstmemberplugs);
		
		MObjectArray animcurves;
		MPlugArray memberplugs;
		status = srcclip.getMemberAnimCurves(animcurves,memberplugs);
		if (!status)
			continue;
		
		for (int j = 0; j < (int)animcurves.length(); j++)
		{
			MString srcmemberplugname = memberplugs[j].name();
//			MStringArray splitarray;
//			srcmemberplugname.split('.',splitarray);
//			MString name = ExtractName(splitarray[0]);
//			name += MString(".") + splitarray[1];
			const char *debug = srcmemberplugname.asChar();
		
			for (int k = 0; k < (int)dstmemberplugs.length(); k++)
			{
				MString dstmemberplugname = dstmemberplugs[k].name();
//				MStringArray splitarray;
//				dstmemberplugname.split('.',splitarray);
//				MString name0 = ExtractName(splitarray[0]);
//				name0 += MString(".") + splitarray[1];

				const char *debug0 = dstmemberplugname.asChar();
				
				if (srcmemberplugname == dstmemberplugname)
					dstcharacter.addCurveToClip(animcurves[j],dstclipobject,dstmemberplugs[k],fMod);
			}
		}
	}

	int schedclipcount = srccharacter.getScheduledClipCount(&status);

	for (i = 0; i < schedclipcount; i++)
	{
		MObject srcshedclipobject = srccharacter.getScheduledClip(i);

		MFnClip srcshedclip(srcshedclipobject);

		MObject sourceclipobject = srcshedclip.sourceClip(&status);

		MFnClip sourceclip(sourceclipobject);

		MFnDependencyNode depnode(sourceclipobject);

		MObject destclipobject;

		if (!sourceclipmap.HasKey((int)MTools::GetMPtrBase(depnode)))
			continue;
		int index;
		sourceclipmap.FindKey((int)MTools::GetMPtrBase(depnode),index);
		destclipobject = sourceclips[index];

		MTime starttime = srcshedclip.getStartFrame(&status);

		MString clipname = sourceclip.name(&status);

		MDGModifier fMod;

		MFnClip destclip(destclipobject);

		// create and attach instance clip to model
		MObject instancedclipobject = destclip.createInstancedClip(destclipobject,starttime,fMod,&status);
		if (!status)
			return MS::kFailure;

		status = dstcharacter.attachInstanceToCharacter(instancedclipobject,fMod);
		if (!status)
			return MS::kFailure;

		CopyClipAttributes(srcshedclipobject,instancedclipobject);
	}

	return 1;
}


int MTools::GetCharacterFromClip(MObject clipobject,MObject &character)
{
	MStatus status;
	MFnClip fnclip(clipobject);

	MFnDependencyNode clipdepnode(clipobject);

	MPlug clipcycleplug = clipdepnode.findPlug("cycle",&status);
	if (!status)
		return 0;

	MPlugArray connections;

	clipcycleplug.connectedTo(connections,false,true,&status);

	if (connections.length()==0)
		return 0;

	MObject clipschedulerobject = connections[0].node();

	MFnDependencyNode clipshedulerdepnode(clipschedulerobject);

	MPlug clipschedulerclipevalplug = clipshedulerdepnode.findPlug("clipEvaluate",&status);

	clipschedulerclipevalplug.connectedTo(connections,false,true,&status);
	
	if (connections.length()==0)
		return 0;

	if (connections[0].node().apiType() == MFn::kCharacter)
	{
		character = connections[0].node();
		return 1;
	}
	return 0;
}

int MTools::ExploreParents(MFn::Type mfntype,MDagPath dagpath,MDagPath child,MDagPath &result,bool inverse)
{
	MStatus status;
	MFnDependencyNode depnode(dagpath.node());

	MString pathname = dagpath.fullPathName();
	const char *debug = pathname.asChar();
	
	// if mfntype found return
	if (!inverse)
	{
		if (dagpath.apiType() == mfntype)
		{
			return 1;
		}
	}
	else
	{
		if (dagpath.apiType() != mfntype)
		{
			result = child;
			return 1;
		}
	}

	MFnDagNode dagNode(dagpath.node());

	if (dagNode.parentCount() > 0)
	{
		//	stop recursing at kWorld
//		if (dagNode.parent(0,&status).apiType() != MFn::kWorld)
//		{
			MDagPath temppath(dagpath);
			temppath.pop();

			//	recurse
			MDagPath founddagpath;
			if (ExploreParents(mfntype,temppath,dagpath,founddagpath,inverse))
			{
				result = founddagpath;
				return 1;
			}
//		}
	}
	return 0;
}

int MTools::GetRootBone(MDagPath dagpath,MDagPath &rootjointdagpath)
{
	ExploreParents(MFn::kJoint,dagpath,dagpath,rootjointdagpath,true);
	return 1;
}

int MTools::GetRootBoneFromCharacter(MObject character,MDagPath &rootjointdagpath)
{
	MFnCharacter fncharacter(character);
	MPlugArray plugs;
	fncharacter.getMemberPlugs(plugs);

	if (plugs.length()==0)
		return 0;
	
	MFnDagNode dagnode(plugs[0].node());
	MDagPath dagpath;
	dagnode.getPath(dagpath);

	GetRootBone(dagpath,rootjointdagpath);

	return 1;
}

int MTools::GetRootBoneFromClip(MObject clipobject,MDagPath &rootjointdagpath)
{
	MObject character;
	GetCharacterFromClip(clipobject,character);

	MFnCharacter fncharacter(character);
	MPlugArray plugs;
	fncharacter.getMemberPlugs(plugs);

	if (plugs.length()==0)
		return 0;
	
	MFnDagNode dagnode(plugs[0].node());
	MDagPath dagpath;
	dagnode.getPath(dagpath);

	GetRootBone(dagpath,rootjointdagpath);

	return 1;
}

int MTools::GetClipIndicies(MObject clipscheduler,MStringArray &clipnames,MIntArray &indicies)
{
	MDGModifier dgmodifier;

	clipnames.clear();
	indicies.clear();

	MFnDependencyNode clipschedulerdepnode(clipscheduler);

	MStringArray results;
	MGlobal::executeCommand(MString("clipSchedule -q ") + clipschedulerdepnode.name(),results); 
	dgmodifier.doIt();

	for (int i = 0; i < (int)results.length(); i++)
	{	
			MStringArray splitarray;

			results[i].split(',',splitarray);

			int clipindex = splitarray[1].asInt();	

			clipnames.append(splitarray[0]);
			indicies.append(clipindex);
	}
	return 1;
}

int MTools::GetClipIndex(MObject clip,MObject clipscheduler,int &index)
{
	MFnDependencyNode clipdepnode(clip);

	MStringArray clipnames;
	MIntArray indicies;

	GetClipIndicies(clipscheduler,clipnames,indicies);

	for (int i = 0; i < (int)clipnames.length(); i++)
	{
		if (clipnames[i] == clipdepnode.name())
		{
			index = indicies[i];
			return 1;
		}
	}
	return 0;
}


int MTools::GetAnimCurveFromPlug(MPlug plug,MObject &animcurve)
{
	MStatus status;
	MPlugArray connected;

	plug.connectedTo(connected,true,false,&status);
	if (!status)
		return 0;

	for (int i = 0; i < (int)connected.length(); i++)
	{
		if ((connected[i].node().apiType() == MFn::kAnimCurve)
			||(connected[i].node().apiType() == MFn::kAnimCurveTimeToAngular)
			||(connected[i].node().apiType() == MFn::kAnimCurveTimeToDistance)
			||(connected[i].node().apiType() == MFn::kAnimCurveTimeToUnitless)
			||(connected[i].node().apiType() == MFn::kAnimCurveUnitlessToAngular)
			||(connected[i].node().apiType() == MFn::kAnimCurveUnitlessToDistance)
			||(connected[i].node().apiType() == MFn::kAnimCurveUnitlessToUnitless))
		{
			animcurve = connected[i].node();
			return 1;
		}
	}

	return 0;	
}

int MTools::PasteKeysFromClipboard(MDagPath rootdagpath,bool includechildren)
{
	MStatus status;
	MDGModifier dgmodifier;

	bool myerror = false;

	MString clipboardstring;
	if (!ReadStringFromClipboard(clipboardstring))
		return 0;
	
	MStringArray lines;
	clipboardstring.split('\r',lines);

	if (lines[0] != "[BeginMayaAnimationKeys]")
		return 0;

	const char *debug = lines[0].asChar();

	MTime currentTime = MAnimControl::currentTime();
	
	MTime::Unit currentUnit = currentTime.unit();
	
	for (int i = 1; i < (int)lines.length(); i++)
	{
		MStringArray args;
		
		args.clear();
		lines[i].split('=',args);
		
		debug = lines[i].asChar();

		if (args[0] == "[BeginAnimChannel]")
		{
			i++;
			args.clear();
			lines[i].split('=',args);
			debug = lines[i].asChar();

			if (args[0] == "DagPath")
			{
				MString dagpath = args[1];
				i++;
				args.clear();
				lines[i].split('=',args);
				debug = lines[i].asChar();

				if (args[0] == "Plug")
				{
					MString plugname = args[1];
					i++;
					args.clear();
					lines[i].split('=',args);
					debug = lines[i].asChar();

					if(args[0] == "No.Keys")
					{
						int numKeys = args[1].asInt();
						for (int j = 0; j < numKeys; j++)
						{
							
							// get AnimKey label
							i++;
							args.clear();
							lines[i].split('=',args);
							debug = lines[i].asChar();
							
							if (args[0] == "[BeginAnimKey]")
							{
								// get key
								i++;
								args.clear();
								lines[i].split('=',args);
								int key = (int)args[1].asFloat();
								
								// get value
								i++;
								args.clear();
								lines[i].split('=',args);
								float value = (float)args[1].asFloat();
								
								// get in tangent
								i++;
								args.clear();
								lines[i].split('=',args);
								int intan = args[1].asInt();
								
								// get out tangent
								i++;
								args.clear();
								lines[i].split('=',args);
								int outtan = args[1].asInt();
								
								// get plug name
								MStringArray parts;
								plugname.split('.',parts);
								const char*debug = parts[1].asChar();
								
								MFnDependencyNode depnode(rootdagpath.node());
								
								if (dagpath == rootdagpath.fullPathName())
								{
									MPlug plug = depnode.findPlug(parts[1],&status);
									if (!status)
										return 0;
									
									MObject animcurve;
									GetAnimCurveFromPlug(plug,animcurve);
									
									MFnAnimCurve fnanimcurve(animcurve);
									
									unsigned index;
									if (fnanimcurve.find(currentTime,index,&status))
									{
										status = fnanimcurve.setValue(index,value);
										if (!status)
											myerror = true;

										MFnAnimCurve::TangentType intangent = (MFnAnimCurve::TangentType)intan;
										MFnAnimCurve::TangentType outtangent = (MFnAnimCurve::TangentType)outtan;

										fnanimcurve.setInTangentType(index,intangent);
										fnanimcurve.setOutTangentType(index,outtangent);
									}
									else
									{
										status = fnanimcurve.addKeyframe(currentTime,value,(MFnAnimCurve::TangentType)intan,(MFnAnimCurve::TangentType)outtan);
										if (!status)
											myerror = true;
									}
								}
								i++;//skip end label
							}
							else
								myerror = true;
						}
					}
					else
						myerror = true;
				}
				else
					myerror = true;
			}
			else
				myerror = true;
			i++;//skip end label
		}
		else if (args[0] == "[EndMayaAnimationKeys]")
		{
			break;
		}
		else
			myerror = true;
	}
	if (myerror)
	{
		MGlobal::displayError("Failed to set keyframe.");
		return 0;
	}
	
	return 1;	
}
/*
int MTools::GetCurveInterpolation(MObject curveobject,int &curveinterpolation)
{
	MStatus status;
	MFnDependencyNode depnode(curveobject);
	MPlug plug = depnode.findPlug("roti",&status);
	if (!status)
		return 0;
	status = plug.getValue(curveinterpolation);
	if (!status)
		return 0;
	return 1;
}
*/
int MTools::CopyKeysToClipboard(MDagPath rootdagpath,int startframe,int endframe)
{
	MStatus status;
	MDGModifier dgModifier;
	MString clipboardstring;
	
	clipboardstring = "[BeginMayaAnimationKeys]\r";

	MDagPath dagpath = rootdagpath;

	MSelectionList list;
	list.add(dagpath);

	if (!MAnimUtil::isAnimated (dagpath))
		return 0;

	MFnDependencyNode depnode(dagpath.node());
	MString nodename = depnode.name();

	MPlugArray animatedPlugs;
	MAnimUtil::findAnimatedPlugs (list, animatedPlugs);

	bool hasTime = false;
	double startTime = 0.0;
	double endTime = 0.0;
	bool hasUnitless = false;
	double startUnitless = 0.0;
	double endUnitless = 0.0;
	
	MTime currentTime = MAnimControl::currentTime();

	unsigned numPlugs = animatedPlugs.length();
	
	unsigned i;

	for (i = 0; i < numPlugs; i++)
	{
		MPlug plug = animatedPlugs[i];
		MObjectArray animation;
		// Find the animation curve(s) that animate this plug
		//
		if (!MAnimUtil::findAnimation (plug, animation))
			continue;
		
		unsigned int numCurves = animation.length();

		for (unsigned j = 0; j < numCurves; j++)
		{
			MObject animCurveNode = animation[j];
			if (!animCurveNode.hasFn (MFn::kAnimCurve))
				continue;

			MFnAnimCurve animCurve (animCurveNode);

			unsigned numKeys = animCurve.numKeys();

			if (numKeys == 0)
				continue;
			
			if (animCurve.isUnitlessInput())
			{
				if (!hasUnitless)
				{
					startUnitless = animCurve.unitlessInput (0);
					endUnitless = animCurve.unitlessInput (numKeys - 1);
					hasUnitless = true;
				}
				else
				{
					startUnitless = min (startUnitless, animCurve.unitlessInput (0));
					endUnitless = max (endUnitless, animCurve.unitlessInput (numKeys - 1));
				}
			}
			else
			{
				if (!hasTime)
				{
					startTime = animCurve.time (0).value();
					endTime = animCurve.time (numKeys - 1).value();
					hasTime = true;
				}
				else
				{
					startTime = min (startTime, animCurve.time (0).value());
					endTime = max (endTime, animCurve.time (numKeys - 1).value());
				}
			}
		}
	}
	
	unsigned int numObjects = list.length();
	for (i = 0; i < numObjects; i++)
	{
		MDagPath path;
		MObject node;
		if (list.getDagPath (i, path) == MS::kSuccess)
		{
			AddDagPathToClipboard(path,startframe,endframe,clipboardstring);
		}
		else if (list.getDependNode (i, node) == MS::kSuccess)
		{
			AddMObjectToClipboard(node,startframe,endframe,clipboardstring);
		}
	}

	clipboardstring += "[EndMayaAnimationKeys]";

	if (!MTools::WriteStringToClipboard(clipboardstring))
		return MS::kFailure;
	
	return 1;
}


int MTools::AddDagPathToClipboard(const MDagPath &path,int startframe,int endframe,MString &clipboardstring)
{
	// Walk through the dag breadth first
	//
	MItDag dagIt (MItDag::kDepthFirst);
	dagIt.reset (path, MItDag::kDepthFirst);
	for (; !dagIt.isDone(); dagIt.next()) {
		MDagPath thisPath;
		if (dagIt.getPath (thisPath) != MS::kSuccess) {
			continue;
		}
		// Find the animated plugs for this object
		//
		MPlugArray animatedPlugs;
		MObject node = thisPath.node();
		MFnDependencyNode fnNode (node);
		MAnimUtil::findAnimatedPlugs (thisPath, animatedPlugs);
		unsigned int numPlugs = animatedPlugs.length();
		if (numPlugs == 0) {
			// If the object is not animated, then write out place holder
			// information
			//
//			animFile << "anim " << fnNode.name().asChar() << " " << dagIt.depth() << " " << thisPath.childCount() << " 0;\n";
		}
		else {
			// Otherwise write out each animation curve
			//
			AddAnimatedPlugs (animatedPlugs,fnNode.name(),startframe,endframe,dagIt.depth(),thisPath.childCount(),&thisPath,clipboardstring);
		}
	}
	return 1;
}

int MTools::AddMObjectToClipboard(const MObject &node,int startframe,int endframe,MString &clipboardstring)
{
	// Watch out for characters and handle them a little differently
	//
	if (node.hasFn (MFn::kCharacter))
	{
		MObjectArray characterList;
		characterList.append (node);
		MIntArray depths;
		depths.append (0);
		unsigned int current = 0;
		while (current < characterList.length())
		{
			const MObject &thisNode = characterList[current];
			int thisDepth = depths[current++];
			// If this node is a character, then check for any immediate
			// subCharacters
			//
			MFnSet fnSet (thisNode);
			// Now find the set members
			//
			MSelectionList members;
			fnSet.getMembers (members, false);
			unsigned int childCount = 0;
			MItSelectionList iter (members, MFn::kCharacter);
			for (; !iter.isDone(); iter.next()) {
				MObject childNode;
				iter.getDependNode (childNode);
				characterList.insert (childNode, current + childCount);
				depths.insert (thisDepth + 1, current + childCount);
				childCount++;
			}
			// Find the animated plugs for this object
			//
			MPlugArray animatedPlugs;
			MAnimUtil::findAnimatedPlugs (thisNode, animatedPlugs);
			unsigned int numPlugs = animatedPlugs.length();
			if (numPlugs == 0) {
				// If the object is not animated, then write out place holder
				// information
				//
//				animFile << "anim " << fnSet.name().asChar() << " " << thisDepth << " " << childCount << " 0;\n";
			}
			else {
				// Otherwise write out each animation curve
				//
				AddAnimatedPlugs (animatedPlugs, fnSet.name(),startframe,endframe,thisDepth, childCount,NULL,clipboardstring);
			}
		}
		return 1;
	}
	// Find the animated plugs for this object
	//
	MPlugArray animatedPlugs;
	MFnDependencyNode fnNode (node);
	MAnimUtil::findAnimatedPlugs (node, animatedPlugs);
	unsigned int numPlugs = animatedPlugs.length();
	if (numPlugs != 0)
	{
		// If the object is animated the write out each animation curve
		//
		AddAnimatedPlugs(animatedPlugs,fnNode.name(),startframe,endframe,0,0,NULL,clipboardstring);
	}
	return 1;
}

int MTools::AddAnimatedPlugs(const MPlugArray &animatedPlugs,
							 const MString &nodeName,
							 int startframe,int endframe,
							 unsigned int depth,unsigned int childCount,
							 MDagPath *dagpath,
							 MString &clipboardstring)
{
	MStatus status;
	// Walk through each animated plug and write out the animation curve(s)
	//
	unsigned int numPlugs = animatedPlugs.length();

	for (unsigned i = 0; i < numPlugs; i++)
	{
		MPlug plug = animatedPlugs[i];

		MObjectArray animation;
		if (!MAnimUtil::findAnimation (plug, animation))
			continue;

		// build up the full attribute name
		//
		MPlug attrPlug (plug);
		MObject attrObj = attrPlug.attribute();
		MFnAttribute fnAttr (attrObj);
		MString fullAttrName (fnAttr.name());
		attrPlug = attrPlug.parent();
		while (!attrPlug.isNull())
		{
			attrObj = attrPlug.attribute();
			MFnAttribute fnAttr (attrObj);
			fullAttrName = fnAttr.name() + "." + fullAttrName;
			attrPlug = attrPlug.parent();
		}
		attrObj = plug.attribute();
		
		MFnAttribute fnLeafAttr (attrObj);

		unsigned int numCurves = animation.length();

		// Adds each animation curve that animates the plug
		//
		for (unsigned j = 0; j < numCurves; j++)
		{
			MObject animCurveNode = animation[j];

			if (!animCurveNode.hasFn (MFn::kAnimCurve))
				continue;

			MFnAnimCurve animCurve(animCurveNode);
			
			MTime currentTime = MAnimControl::currentTime();

			MTime::Unit currentUnit = currentTime.unit();

			clipboardstring += "[BeginAnimChannel]\r";

			if ((startframe<0)&&(endframe<0))
			{
				startframe = (int)currentTime.as(currentUnit);
				endframe = startframe + 1;
			}
			if (dagpath)
				clipboardstring += "DagPath=" + dagpath->fullPathName()+ MString("\r");
			else
				clipboardstring += "DagPath=NONE\r";
				
			clipboardstring += "Plug=" + plug.name() + MString("\r");
		
			MString numkeys;
			numkeys.set(endframe-startframe);
			clipboardstring += "No.Keys=" + numkeys + MString("\r");
			
			for (int keyframe = startframe; keyframe < endframe; keyframe++)
			{
				clipboardstring += "[BeginAnimKey]\r";

				MTime thistime(keyframe,currentUnit);

				unsigned index;
				double value;
				MFnAnimCurve::TangentType intangent,outtangent;

				if (animCurve.find(thistime,index))
				{
					value = animCurve.value(index);
					intangent = animCurve.inTangentType(index);
					outtangent = animCurve.outTangentType(index);
				}
				else
				{
					value = animCurve.evaluate(thistime,&status);
					if (!status)
						continue;
					intangent = MFnAnimCurve::kTangentSmooth;
					outtangent = MFnAnimCurve::kTangentSmooth;
				}


				MString currtime,currval;

				currtime.set(currentTime.value());
				currval.set(value);

				clipboardstring += MString("Key=") + currtime + MString("\r");
				clipboardstring += MString("Value=") + currval + MString("\r");

				int intan = (int)intangent;
				int outtan = (int)outtangent;

				MString intanstr,outtanstr;

				intanstr.set(intan);
				outtanstr.set(outtan);

				clipboardstring += MString("InTangent=") + intanstr + MString("\r");
				clipboardstring += MString("OutTangent=") + outtanstr + MString("\r");

				clipboardstring += "[EndAnimKey]\r";
			}
		}
		clipboardstring += "[EndAnimChannel]\r";
	}
	return 1;
}

int MTools::CopyAnimCurve(MObject source, MObject dest)
{
	MStatus status;
	MFnAnimCurve sourceanim,destanim;

	for (unsigned i = 0; i < sourceanim.numKeyframes(); i++)
	{
		MTime time = sourceanim.time(i,&status);
		double value = sourceanim.value(i,&status);
		MFnAnimCurve::TangentType intan = sourceanim.inTangentType(i,&status);
		MFnAnimCurve::TangentType outtan = sourceanim.outTangentType(i,&status);

		status = destanim.addKeyframe(time,value,intan,outtan,NULL);
	}

	return 1;
}

int MTools::ReadStringFromClipboard(MString &clipboard)
{
	MStatus status;
	HGLOBAL hglb; 

	HWND hwnd = M3dView::applicationShell(&status);
	if (!status)
		return 0;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
 		return FALSE;

	if (OpenClipboard(hwnd))
	{
		hglb = GetClipboardData(CF_TEXT);
		LPCTSTR lpstr = (LPCTSTR)GlobalLock(hglb);
		clipboard = lpstr;
		GlobalUnlock(hglb);
		CloseClipboard();
		return TRUE;
	}

	return FALSE;
}

int MTools::WriteStringToClipboard(const MString &clipboard)
{
	MStatus status;
	HGLOBAL hglb; 

	HWND hwnd = M3dView::applicationShell(&status);
	if (!status)
		return FALSE;

	const char *text = clipboard.asChar();

	hglb = GlobalAlloc(GMEM_MOVEABLE, strlen(text)+1); 
	if (!hglb)
		return FALSE;
	
	LPSTR lpstr = (LPSTR)GlobalLock(hglb);
	if (lpstr)
	{
		strcpy(lpstr,text);

		GlobalUnlock(hglb);

		if (OpenClipboard(hwnd))
		{
			if (EmptyClipboard())
			{
				SetClipboardData(CF_TEXT,hglb);
				hglb = NULL;
			}
			CloseClipboard();
			if (hglb != NULL)
			{
				GlobalFree(hglb);	 // free if not able to set
			}
			else
				return TRUE; // don't free if SetClipboardData worked
		}
	}
	return FALSE;
}

int MTools::GetParentOrderIndex(const MDagPath &dagPath)
{
	MDagPath parentDagPath = dagPath;
	parentDagPath.pop();
	int numChildren = parentDagPath.childCount();
	int validChildIndex = 0;
	for (int childIndex=0;childIndex<numChildren;childIndex++)
	{
		MObject childObject = parentDagPath.child(childIndex);
		MDagPath childDagPath = parentDagPath;
		childDagPath.push(childObject);

		TupString childDagPathString(childDagPath.fullPathName().asChar());

		MFnDagNode fnDagNode(childDagPath);
		if (childDagPath==dagPath)
			return validChildIndex;

		if (fnDagNode.isIntermediateObject())
			continue;

		validChildIndex++;
	}
	return -1;
}

struct MObjectTemp
{
	MPtrBase *		ptr;
	MFn::Type		tp;
};

const MPtrBase *MTools::GetMPtrBase(const MObject &mObject)
{
	return ((const MObjectTemp *)&mObject)->ptr;
}

MPtrBase *MTools::GetMPtrBase(MObject &mObject)
{
	return ((MObjectTemp *)&mObject)->ptr;
}

MPtrBase* MTools::GetMPtrBase(MFnBase &mfnBase)
{
	MObject mObject = mfnBase.object();
	return GetMPtrBase(mObject);
}

const MPtrBase* MTools::GetMPtrBase(const MFnBase &mfnBase)
{
	MObject mObject = mfnBase.object();
	return GetMPtrBase(mObject);
}

