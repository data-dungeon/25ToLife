////////////////////////////////////////////////////////////////////////////
//
// DBLExport
//
// DBL exporter
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "DBLExp.h"
#include "TupExport.h"

void* DBLExport::creator()
{
   return new DBLExport();
}

//////////////////////////////////////////////////////////////

MStatus DBLExport::writer ( const MFileObject& file,
                               const MString& options,
                               FileAccessMode mode )
{
   bool bConsolidateObjects = true;

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

         if( theOption[0] == MString("filterchainpath") && theOption.length() > 1 )
         {
            m_filterchainpath = theOption[1];
         }
         if( theOption[0] == MString("processfile") && theOption.length() > 1 )
         {
            m_processfile = theOption[1];
         }
      }
   }

   if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
      ( mode == MPxFileTranslator::kSaveAccessMode ) )
   {
      TupExport *pTupExport = new TupExport(false);
      pTupExport->BuildScene(file.fullName().asChar());
      TupperwareAggregate *pSceneAgg = pTupExport->GetSceneAgg();
      ExportDBL(pSceneAgg,file.fullName().asChar());
      delete pTupExport;
   }
   else if( mode == MPxFileTranslator::kExportActiveAccessMode )
   {
      MSelectionList slist;
      MGlobal::getActiveSelectionList( slist );
      MItSelectionList iter( slist );
      if (iter.isDone())
         return MS::kFailure;

      TupExport *pTupExport = new TupExport(true);
      pTupExport->BuildScene(file.fullName().asChar());
      TupperwareAggregate *pSceneAgg = pTupExport->GetSceneAgg();
      ExportDBL(pSceneAgg,file.fullName().asChar());
      delete pTupExport;
   }

   return MS::kSuccess;
}

MPxFileTranslator::MFileKind DBLExport::identifyFile (
   const MFileObject& fileName,
   const char* buffer,
   short size) const
{
   const char * name = fileName.name().asChar();
   int nameLength = strlen(name);

   if ((nameLength > 4) && !strcasecmp(name+nameLength-4, ".dbl"))
      return kCouldBeMyFileType;
   else
      return kNotMyFileType;
}

void DBLExport::ExportDBL(TupperwareAggregate *pSceneAgg,const char *filepathname)
{
   MString filterchaindll = m_filterchainpath + MString("/filterchain.dll");
   const char *debug = filterchaindll.asChar();

   // load dll into memory
   HINSTANCE hInstance = LoadLibraryEx(filterchaindll.asChar(), NULL, 0);
   if (hInstance)
   {
      bool (*vProcess)(TupperwareAggregate *, int, char**);

      vProcess = (bool (*)(TupperwareAggregate *, int, char**))(const char *(*)())GetProcAddress(hInstance, "LibProcess");

      if (vProcess)
      {
         char *params[3];
         params[0] = (char *)filepathname;
         params[1] = (char *)m_processfile.asChar();
         params[2] = (char *)filepathname;
         bool result = vProcess(pSceneAgg,3,params);

         FreeLibrary(hInstance);
      }
   }
}
