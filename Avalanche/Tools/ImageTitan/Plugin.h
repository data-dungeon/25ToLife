#pragma once

#include "SDK/ImageFormatEntry.h"

class ImagePluginReg
{
public:
   HINSTANCE m_hinstance;
   String m_strFileName;
   String m_strPluginClassName;
   CArray<ImageFormatEntry *> m_entries;

   ImagePluginReg(){}
   ImagePluginReg(HINSTANCE hinstance, const String &strFileName, const String &strPluginClassName );
	virtual ~ImagePluginReg();

   ImageFormatEntry *FindEntryBySuffix(const String &strSuffix);
   ImageFormatEntry *FindEntryByClassName(const String &strClassName);
   ImagePluginReg &operator = (const ImagePluginReg &other);
};