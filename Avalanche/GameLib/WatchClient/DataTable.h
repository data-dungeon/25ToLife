//===================================================================
//  File:  DataTable.h
//  Description:
//     Header file
//===================================================================
#ifndef __hDataTable__
#define __hDataTable__

#include "platform/BaseType.h"
#include "WatchClient/WatchTable.h"

// Forward declarations
class FileStream;

//---------------------------------------------
//  DataTable
//---------------------------------------------
template<class T> class DataTable : public WatchTable
{
public:
	DataTable();
	virtual ~DataTable();

	virtual T* Get(int i , int j)
	{
		ASSERT(i < nRows && j < nCols);
		return &fData[i*nCols + j];
	}
	bool Load(char* filename);
	bool Save(char* filename);
	bool Open(char* filename) {return Load(filename);}


	// Interface inherited from WatchTable
	virtual bool NewRowColumn(char*, bool);
	virtual bool FillRowColumn(TABLE_DATA&);

	// Pure virtual functions.
	virtual char* TableName() = 0;
	virtual int   Version() = 0;

	char* GetRowLabel(int i) const {if (i < nRows) return fRowLabels[i]; return 0;}
	char* GetColLabel(int i) const {if (i < nCols) return fColLabels[i]; return 0;}

	int NumRows() const {return nRows;}
	int NumCols() const {return nCols;}

protected:
	void DeleteLabels();
	void DeleteData();
	virtual void Register();
	virtual void RegisterLabels();
	virtual void RegisterVariables() = 0;
	void AppendLabel(char*, char***, int*);
	void AppendRowLabel(char* label) {AppendLabel(label, &fRowLabels, &nRows);}
	void AppendColLabel(char* label) {AppendLabel(label, &fColLabels, &nCols);}
	virtual void ReadRecord(FileStream*, T*, int, int);
	void SetFile(char*);

	T* fData;
	int nRows;
	int nCols;
	char** fRowLabels;
	char** fColLabels;

private:
	char* fFile;
	bool fRegistered;
};

//----------------------------------------------------------------------
// DataTable::DeleteLabels
//----------------------------------------------------------------------
template<class T> inline void DataTable<T>::DeleteLabels()
{
	// Rows
	int i;
	for (i = 0; i < nRows; i++)
		delete fRowLabels[i];
	delete[] fRowLabels;
	fRowLabels = 0;
	nRows = 0;
	// Columns
	for (i = 0; i < nCols; i++)
		delete fColLabels[i];
	delete[] fColLabels;
	fColLabels = 0;
	nCols = 0;
};

//----------------------------------------------------------------------
// DataTable::DeleteData
//----------------------------------------------------------------------
template<class T> inline void DataTable<T>::DeleteData()
{
	if (!fData)
		return;
	delete[] fData;
	fData = 0;
};

#include "WatchClient/DataTable.hpp"

#endif // __hDataTable__

