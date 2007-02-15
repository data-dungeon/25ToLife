/***********************************/
/*                                 */
/* Export.h                        */
/* Pigpen particle tool  1/5/00    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dbl file export                 */
/*                                 */
/***********************************/

#ifndef __EXPORT_H
#define __EXPORT_H

/******************* includes ****************************/

/******************* defines *****************************/

/******************* forward declarations ****************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

bool ExportDBL(CPigpenDoc *pDoc, CObArray *pArrays, char *pExportFileName, CEmitter *pPreviewEmitter = NULL, CPigpenSwooshEmitter *pPreviewSwooshEmitter = NULL);
int AnalyzeDataForExport(CObArray *pArrays, CEmitter *pPreviewEmitter, CPigpenSwooshEmitter *pPreviewSwooshEmitter);
bool ValidExportLogFile(void);
char *GetExportLogFilename(void);
void AnalyzeTextureSizes(CObArray& Graphics);



#endif // __EXPORT_H
