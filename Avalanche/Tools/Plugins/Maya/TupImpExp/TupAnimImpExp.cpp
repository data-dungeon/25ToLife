////////////////////////////////////////////////////////////////////////////
//
// TupAnimImpExp
//
// Tupperware animation importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupAnimImpExp.h"
#include "TupAnimExport.h"
#include "TupAnimImport.h"

void* TupAnimImpExp::creator()
{
   return new TupAnimImpExp();
}

//////////////////////////////////////////////////////////////

MStatus TupAnimImpExp::reader ( const MFileObject& file,
                           const MString& options,
                           FileAccessMode mode)
{
   bool bImportAsClip = false;

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

         // forces animation onto bones
         if( theOption[0] == MString("importasclip") && theOption.length() > 1 )
            if( theOption[1].asInt() > 0 )
               bImportAsClip = true;
            else
               bImportAsClip = false;
      }
   }

   int result = false;
   try
   {
      TupAnimImport *pTupAnimImport = new TupAnimImport();
      result = pTupAnimImport->ImportAnimation(file.fullName().asChar(),bImportAsClip);
      delete pTupAnimImport;
   }
   catch (...) 
   {
   }

   if (result)
      return MS::kSuccess;
   else
      return MS::kFailure;
}


MStatus TupAnimImpExp::writer ( const MFileObject& file,
                           const MString& options,
                           FileAccessMode mode )
{
   bool bBinaryOutput = false;

   TupString optionsString(options.asChar());

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

         if( theOption[0] == MString("binaryoutput") && theOption.length() > 1 )
         {
            if( theOption[1].asInt() > 0 )
               bBinaryOutput= true;
            else
               bBinaryOutput = false;
         }
      }
   }

   MStatus status;
   // export all
   if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
      ( mode == MPxFileTranslator::kSaveAccessMode ) )
   {
//      TupExport *pTupExport = new TupExport(bConsolidateObjects,bBinaryOutput,false);
//      status = pTupExport->ExportScene(file.fullName().asChar());
//      delete pTupExport;
   }
   // export selected
   else if( mode == MPxFileTranslator::kExportActiveAccessMode )
   {
      MSelectionList slist;
      MGlobal::getActiveSelectionList( slist );
      MItSelectionList iter( slist );
      if (iter.isDone())
         return MS::kFailure;

      TupAnimExport *pTupAnimExport = new TupAnimExport(bBinaryOutput,true);
      status = pTupAnimExport->ExportAnimation(file.fullName().asChar());
      delete pTupAnimExport;
   }

   return status;
}

bool TupAnimImpExp::haveReadMethod () const
{
   return true;
}

bool TupAnimImpExp::haveWriteMethod () const
{
   return true;
}

MString TupAnimImpExp::defaultExtension () const
{
   return "anm";
}

MPxFileTranslator::MFileKind TupAnimImpExp::identifyFile (
   const MFileObject& fileName,
   const char* buffer,
   short size) const
{
   const char * name = fileName.name().asChar();
   int nameLength = strlen(name);

   if ((nameLength > 4) && !strcasecmp(name+nameLength-4, ".anm"))
      return kCouldBeMyFileType;
   else
      return kNotMyFileType;
}

