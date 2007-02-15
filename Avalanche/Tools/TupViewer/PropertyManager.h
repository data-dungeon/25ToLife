
#ifndef PROPERTY_MANAGER_H
#define PROPERTY_MANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PropertyManager
{
public:
	struct PropertyTableEntryData
	{
		enum Flags
		{
			FLAGS_TYPE_MASK									= 0x0000000f,
			FLAGS_TYPE_END										= 0x00000000,
			FLAGS_TYPE_KEY										= 0x00000001,
			FLAGS_TYPE_NAME									= 0x00000002,
			FLAGS_TYPE_DATA									= 0x00000003,
			FLAGS_TYPE_SIZE									= 0x00000004,
			FLAGS_TYPE_TABLE_ENTRY							= 0x00000005,

			FLAGS_STYLE_MASK									= 0x000000f0,
			// scalar int
			FLAGS_STYLE_SCALAR_INT_NORMAL					= 0x00000000, 
			FLAGS_STYLE_SCALAR_INT_SPINNER				= 0x00000010, 
			FLAGS_STYLE_SCALAR_INT_CHECKBOX				= 0x00000020, 
			FLAGS_STYLE_SCALAR_INT_RADIO_GROUP			= 0x00000030, 
			FLAGS_STYLE_SCALAR_INT_COMBO_BOX				= 0x00000040, 
			// scalar float
			FLAGS_STYLE_SCALAR_FLOAT_NORMAL				= 0x00000000, 
			FLAGS_STYLE_SCALAR_FLOAT_SPINNER				= 0x00000010, 
			// scalar string
			FLAGS_STYLE_SCALAR_STRING_NORMAL				= 0x00000000, 
			FLAGS_STYLE_SCALAR_STRING_MULTILINE			= 0x00000010, 
			// list int
			FLAGS_STYLE_LIST_INT_GRID						= 0x00000000,
			FLAGS_STYLE_LIST_INT_SUB						= 0x00000010,
			FLAGS_STYLE_LIST_INT_COLOR						= 0x00000020,
			// list float
			FLAGS_STYLE_LIST_FLOAT_GRID					= 0x00000000,
			FLAGS_STYLE_LIST_FLOAT_SUB						= 0x00000010,
			FLAGS_STYLE_LIST_FLOAT_COLOR					= 0x00000020,
			// list string
			FLAGS_STYLE_LIST_STRING_GRID					= 0x00000000,
			FLAGS_STYLE_LIST_STRING_SUB					= 0x00000010,

			FLAGS_GRID_COLUMNS_MASK							= 0x00000700,
			FLAGS_GRID_COLUMNS_SHIFT						= 8,
			FLAGS_GRID_COLUMNS_1								= 0x00000000,
			FLAGS_GRID_COLUMNS_2								= 0x00000100,
			FLAGS_GRID_COLUMNS_3								= 0x00000200,
			FLAGS_GRID_COLUMNS_4								= 0x00000300,
			FLAGS_GRID_COLUMNS_5								= 0x00000400,
			FLAGS_GRID_COLUMNS_6								= 0x00000500,
			FLAGS_GRID_COLUMNS_7								= 0x00000600,
			FLAGS_GRID_COLUMNS_8								= 0x00000700,

			FLAGS_GRID_COLUMNS_ADJUSTABLE					= 0x00000800,

			FLAGS_INDENT_MASK									= 0x0000f000,
			FLAGS_INDENT_SHIFT								= 12,

			FLAGS_READ_ONLY									= 0x00010000,
			FLAGS_EXPANDED										= 0x00020000,

			FLAGS_FLOAT_SIGNIFICANT_DIGITS				= 0x00700000,
			FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT		= 20,
			FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES			= 0x00800000,
			FLAGS_INT_ALLOW_NEGATIVE_VALUES				= 0x00800000,
		};
		unsigned long m_flags;
		const char *m_pLabel;
		const char *m_pInfoText;
		const char *m_pChildKey;
		const char *m_pChildName;
		const char *m_pOptionString;
		void *m_pData;
	};

	struct SpinnerValues
	{
		float m_min;
		float m_max;
		float m_incrementBy;
	};

private:
	struct PropertyTableEntry
	{
		PropertyTableEntryData *m_pEntryData;
		TupperwareAtom *m_pAtom;
		TupArray<COptionTreeItem *> m_optionTreeItems;
	};
	// Construction
public:
	PropertyManager(COptionTree &otTree);

	void BuildProperties(TupperwareAtom *pAtom,const char *pPropertyKey);
	void UpdateFromOptions(COptionTreeItem *pOptionTreeItem);
	void UpdateFromTree(TupperwareAtom *pAtom);

	bool RegisterProperty(const char *pPropertyKey,PropertyTableEntryData *pPropertyEntries);
private:
	COptionTree &m_otTree;
	TupArray<PropertyTableEntry> m_propertyEntries;

	Keeper<TupString> m_propertyKeys;
	TupArray<PropertyTableEntryData *> m_propertyEntryDataArray;

	void SetGeneralOptionTreeData(TupperwareAtom *pAtom,COptionTreeItem *pOptionTreeItem,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack);
	void SetGeneralOptionTreeDataSub(COptionTreeItem *pOptionTreeItem,PropertyTableEntryData *pPropertyTableEntryData,const char *pLabel,const char *pInfoText);
	COptionTreeItem *GetOptionTreeItemParent(PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack);
	void AddPropertyKey(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack);
	void AddPropertyName(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack);
	void AddPropertyData(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack);
	void UpdateFromOptionsData(COptionTreeItem *pOptionTreeItem,PropertyTableEntry &propertyTableEntry);
	void UpdateFromTreeData(TupperwareAtom *pAtom,PropertyTableEntry &propertyTableEntry);

};

#endif // PROPERTY_MANAGER_H
