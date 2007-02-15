// Dan  3/11/2002  \Dan90\Plugin\HXT\MyProperties\Property.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "F_Protos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CHxtApp::AddModelProperties(HModelCache *modelcache, HDynamicCategoryProperty *pluginproperties)
{
	String filepath = String::GetAppPath();
	filepath += "\\hxt\\ModelProperties.cfg";

	if (!m_ModelProperties.ReadPropertyTree(filepath.buf))
					MessageBox(NULL,"Can't find property configuration file.", "ERROR", 1);


	AddProperties(m_ModelProperties.GetRoot(),&m_ModelProperties,pluginproperties);

	return TRUE;
}

//*******************************************************
//*******************************************************
//*******************************************************
//*******************************************************

BOOL CHxtApp::AddGroupProperties(HGroup *group, HDynamicCategoryProperty *pluginproperties)
{
	String filepath = String::GetAppPath();
	filepath += "\\hxt\\GroupProperties.cfg";

	m_GroupProperties.ReadPropertyTree(filepath.buf);

	AddProperties(m_GroupProperties.GetRoot(),&m_GroupProperties,pluginproperties);

	return TRUE;
}

void CHxtApp::AddProperty(PropertyNode *node,PropertyTree *tree,HDynamicCategoryProperty *pluginproperties)
{
	CString string;
	PropContainer *container = node->GetProperty();
	HDynamicCategoryProperty *pluginproperties2;
			
	if (container->GetType() == PropContainer::CONTAINER_PROP)
	{
		HPropertyInfo *mypropertiesinfo = HPropertyInfo::New(container->_name.c_str(),container->_name.c_str());		
		HDynamicCategoryProperty *myproperties = HDynamicCategoryProperty::New(mypropertiesinfo);
		pluginproperties->AddProperty(myproperties);
		pluginproperties2 = (HDynamicCategoryProperty *)myproperties;
	}
	else if (container->GetType() == PropContainer::BOOL_PROP)
	{
		PropBool *prop = (PropBool *) container;
		HBoolPropertyInfo *myboolinfo = HBoolPropertyInfo::New(prop->_name.c_str(), prop->_name.c_str());
		myboolinfo->SetDefaultValue(prop->_default);
		HBoolProperty *mybool = HBoolProperty::New(myboolinfo);
		pluginproperties->AddProperty(mybool);
		pluginproperties2 = (HDynamicCategoryProperty *)mybool;
	}
	else if (container->GetType() == PropContainer::ENUM_PROP)
	{
		PropEnum *prop = (PropEnum *) container;
		TypeInfoArray *m_typeinfoarray = new TypeInfoArray;
		m_typeinfoarray->SetArraySize(prop->_types.size());
		for (int i=0; i < prop->_types.size(); i++) {
			HTypeInfo *typeinfo = (*m_typeinfoarray)[i];
			const char *test = prop->_types[i].c_str();
			typeinfo->SetMatchName(prop->_types[i].c_str());
			typeinfo->SetLocalizedName(prop->_types[i].c_str());
		}
		m_typeinfoarray2.push_back(m_typeinfoarray);	
		HTypePropertyInfo *mytypeinfo = HTypePropertyInfo::New(prop->_name.c_str(), prop->_name.c_str(), m_typeinfoarray);
		mytypeinfo->SetDefaultIndex(prop->_default);
		HTypeProperty *mytype = HTypeProperty::New(mytypeinfo);
		pluginproperties->AddProperty(mytype);
		pluginproperties2 = (HDynamicCategoryProperty *)mytype;
	}
	else if (container->GetType() == PropContainer::INT_PROP)
	{
		PropInt *prop = (PropInt *) container;
		HIntPropertyInfo *myintinfo = HIntPropertyInfo::New(prop->_name.c_str(),prop->_name.c_str());
		myintinfo->SetDefaultValue(prop->_default);
		myintinfo->SetMinValue(prop->_min);
		myintinfo->SetMaxValue(prop->_max);
		HIntProperty *myint = HIntProperty::New(myintinfo);
		pluginproperties->AddProperty(myint);
		pluginproperties2 = (HDynamicCategoryProperty *)myint;		
	}
	else if (container->GetType() == PropContainer::FLOAT_PROP)
	{
		PropFloat *prop = (PropFloat *) container;
		HFloatPropertyInfo *myfloatinfo = HFloatPropertyInfo::New(prop->_name.c_str(),prop->_name.c_str());
		myfloatinfo->SetDefaultValue(prop->_default);
		myfloatinfo->SetMinValue(prop->_min);
		myfloatinfo->SetMaxValue(prop->_max);
		HFloatProperty *myfloat = HFloatProperty::New(myfloatinfo);
		pluginproperties->AddProperty(myfloat);
		pluginproperties2 = (HDynamicCategoryProperty *)myfloat;		
	}
	else if (container->GetType() == PropContainer::IMAGE_PROP)
	{
		PropImage *prop = (PropImage *) container;
		HClipPropertyInfo *myclipinfo = HClipPropertyInfo::New(prop->_name.c_str(),prop->_name.c_str());
		HClipProperty *myclip = HClipProperty::New(myclipinfo);
		pluginproperties->AddProperty(myclip);
		pluginproperties2 = (HDynamicCategoryProperty *)myclip;
	}
	else if (container->GetType() == PropContainer::STRING_PROP)
	{
		PropString *prop = (PropString *) container;
		HStringPropertyInfo *mystringinfo = HStringPropertyInfo::New(prop->_name.c_str(),prop->_name.c_str());
		HStringProperty *mystring = HStringProperty::New(mystringinfo);
		pluginproperties->AddProperty(mystring);
		pluginproperties2 = (HDynamicCategoryProperty *)mystring;		
	}

	for (int i = 0; i < node->_children.size(); i++)
	{
		PropertyNode *debug = tree->GetChildPropertyNode(node,i);
		AddProperty(tree->GetChildPropertyNode(node,i),tree,pluginproperties2);
	}
}

void CHxtApp::AddProperties(PropertyNode *root,PropertyTree *tree,HDynamicCategoryProperty *pluginproperties)
{
	AddProperty(root,tree,pluginproperties);
}