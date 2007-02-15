#ifndef PROPERTY_TABLE_MACROS_H
#define PROPERTY_TABLE_MACROS_H

#ifndef PROPERTY_MANAGER_H
#include "PropertyManager.h"
#endif

#define DECLARE_PROPERTY_TABLE(b)\
	extern PropertyManager::PropertyTableEntryData b[];

#define SPINNER_VALUES(name,min,max,incrementBy) \
	PropertyManager::SpinnerValues name = {min,max,incrementBy};

#define PROPERTY_TABLE_START(b) \
	PropertyManager::PropertyTableEntryData b[] = {

#define PROPERTY_TABLE_END \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_END, NULL,NULL,NULL,NULL,NULL,NULL} \
	};

#define PROPERTY_TABLE_ENTRY(indent,label,info,flags,expanded) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_TABLE_ENTRY| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(expanded ? PropertyManager::PropertyTableEntryData::FLAGS_EXPANDED : 0)| \
	(flags),\
	label,\
	info,\
	NULL,\
	NULL,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_KEY(indent,label,info,childKey,childName,flags) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_KEY| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_NAME(indent,label,info,childKey,childName,flags) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_NAME| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_SCALAR_INT_DATA_NORMAL(indent,label,info,childKey,childName,flags,allowNegative) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_NORMAL| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_INT_ALLOW_NEGATIVE_VALUES : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_SCALAR_INT_DATA_SPINNER(indent,label,info,childKey,childName,flags,spinnerValues) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_SPINNER| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	(void *)&spinnerValues},

#define PROPERTY_TABLE_ENTRY_SCALAR_INT_DATA_CHECKBOX(indent,label,info,childKey,childName,flags,stateNames,mask) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_CHECKBOX| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	stateNames,\
	(void *)mask},

#define PROPERTY_TABLE_ENTRY_SCALAR_INT_DATA_RADIO_GROUP(indent,label,info,childKey,childName,flags,stateNames,mask) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_RADIO_GROUP| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	stateNames,\
	(void *)mask},

#define PROPERTY_TABLE_ENTRY_SCALAR_INT_DATA_COMBO_BOX(indent,label,info,childKey,childName,flags,stateNames,mask) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_COMBO_BOX| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	stateNames,\
	(void *)mask},

#define PROPERTY_TABLE_ENTRY_SCALAR_FLOAT_DATA_NORMAL(indent,label,info,childKey,childName,flags,significantDigits,allowNegative) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_NORMAL| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(significantDigits<<PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_SCALAR_FLOAT_DATA_SPINNER(indent,label,info,childKey,childName,flags,significantDigits,spinnerValues) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_SPINNER| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(significantDigits<<PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	(void *)&spinnerValues},

#define PROPERTY_TABLE_ENTRY_LIST_INT_DATA_GRID(indent,label,info,childKey,childName,flags,allowNegative,numColumns,columnsAdjustable) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_INT_GRID| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES : 0)| \
	((numColumns-1)<<PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)| \
	(columnsAdjustable ? PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_INT_DATA_SUB(indent,label,info,childKey,childName,flags,subNames,allowNegative) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_INT_SUB| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	subNames,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_INT_DATA_COLOR(indent,label,info,childKey,childName,flags) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_INT_COLOR| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_FLOAT_DATA_GRID(indent,label,info,childKey,childName,flags,significantDigits,allowNegative,numColumns,columnsAdjustable) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_GRID| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(significantDigits<<PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES : 0)| \
	((numColumns-1)<<PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)| \
	(columnsAdjustable ? PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_FLOAT_DATA_SUB(indent,label,info,childKey,childName,flags,subNames,significantDigits,allowNegative) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_SUB| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(significantDigits<<PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT)| \
	(allowNegative ? PropertyManager::PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	subNames,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_FLOAT_DATA_COLOR(indent,label,info,childKey,childName,flags) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_COLOR| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_SCALAR_STRING_DATA(indent,label,info,childKey,childName,flags,multiline) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(multiline ? PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_STRING_MULTILINE : PropertyManager::PropertyTableEntryData::FLAGS_STYLE_SCALAR_STRING_NORMAL)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_STRING_DATA_GRID(indent,label,info,childKey,childName,flags,numColumns,columnsAdjustable) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_GRID| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	((numColumns-1)<<PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)| \
	(columnsAdjustable ? PropertyManager::PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE : 0)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	NULL,\
	NULL},

#define PROPERTY_TABLE_ENTRY_LIST_STRING_DATA_SUB(indent,label,info,childKey,childName,flags,subNames) \
	{ PropertyManager::PropertyTableEntryData::FLAGS_TYPE_DATA| \
	PropertyManager::PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_SUB| \
	(indent<<PropertyManager::PropertyTableEntryData::FLAGS_INDENT_SHIFT)| \
	(flags),\
	label,\
	info,\
	childKey,\
	childName,\
	subNames,\
	NULL},

#endif // PROPERTY_TABLE_MACROS_H