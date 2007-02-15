//===================================================================
//  File:  TableData.hpp
//  Description:
//     Implementation.
//===================================================================

#include <stdarg.h>
#include <string.h>
#include "WatchClient/WatchClient.h"
#include "Layers/Debug.h"

#include "EngineHelper/filestream.h"

//----------------------------------------------------------------------
// DataTable::DataTable
//    Constructor
//----------------------------------------------------------------------
template<class T> DataTable<T>::DataTable()
	: fData(0), fFile(0), fRegistered(false),
	  fRowLabels(0), fColLabels(0), nRows(0), nCols(0)
{
}

//----------------------------------------------------------------------
// DataTable::~DataTable
//    Destructor
//----------------------------------------------------------------------
template<class T> DataTable<T>::~DataTable()
{
	delete fFile;
	DeleteLabels();
	DeleteData();
}

//----------------------------------------------------------------------
// DataTable::Load
//    Load the table from a file.
//----------------------------------------------------------------------
template<class T> bool DataTable<T>::Load(char* filename)
{
	DeleteLabels();
	DeleteData();

	//---------------------------------------------
	//  Save the file off for later.
	//---------------------------------------------
	SetFile(filename);

	FileStream* fs = FileStream::NewAndOpen(fFile, FileStream::READ, false);
	if (!fs)
	{
		nRows = nCols = 0;
		Register();
		return true;
	}

	//---------------------------------------------
	//  Header
	//---------------------------------------------
	int version;
	fs->Read(&version, sizeof(version));
	fs->Read(&nRows, sizeof(nRows));
	fs->Read(&nCols, sizeof(nCols));

	//---------------------------------------------
	//  Labels
	//---------------------------------------------
	fRowLabels = new char*[nRows];
	fColLabels = new char*[nCols];

	int size;
	int i, j;
	for (i = 0; i < nRows; i++)
	{
		fs->Read(&size, sizeof(int));
		fRowLabels[i] = new char[size];
		fs->Read(fRowLabels[i], size);
	}
	for (i = 0; i < nCols; i++)
	{
		fs->Read(&size, sizeof(int));
		fColLabels[i] = new char[size];
		fs->Read(fColLabels[i], size);
	}

	//---------------------------------------------
	//  Data
	//---------------------------------------------
	fData = new T[nRows*nCols];

	fs->Read(&size, sizeof(size));
	for (i = 0; i < nRows; i++)
		for (j = 0; j < nCols; j++)
		{
			ReadRecord(fs, &fData[i*nCols + j], size, version);
		}

	delete fs;

	Register();

	return true;
}

//----------------------------------------------------------------------
// DataTable::ReadRecord
//    This actually reads the record from the file and copies it into
//    the structure.
//----------------------------------------------------------------------
template<class T> void DataTable<T>::ReadRecord(FileStream* fs, T* buf, int size,
																int version)
{
	ASSERT(sizeof(T) >= size);
	fs->Read(buf, size);
}

//----------------------------------------------------------------------
// DataTable::Save
//    Save the table to file.
//----------------------------------------------------------------------
template<class T> bool DataTable<T>::Save(char* filename)
{
	if (!fData)
	{
		char msg[100];
		sprintf(msg, "Cannot save table <%s>.", TableName());
		conPuts(msg);
		return false;
	}

	SetFile(filename);

	//---------------------------------------------
	//  Open the stream
	//---------------------------------------------
	FileStream* fs = FileStream::NewAndOpen(fFile, FileStream::WRITE, false);
	if (!fs)
	{
		char msg[100];
		sprintf(msg, "Error creating '%s' for output.", fFile);
		conPuts(msg);
		return false;
	}

	//---------------------------------------------
	//  Header info
	//---------------------------------------------
	int tmp = Version();
	fs->Write(&tmp, sizeof(int));
	fs->Write(&nRows, sizeof(nRows));
	fs->Write(&nCols, sizeof(nCols));

	//---------------------------------------------
	//  Labels
	//---------------------------------------------
	int size;
	int i, j;
	for (i = 0; i < nRows; i++)
	{
		size = strlen(fRowLabels[i])+1;
		fs->Write(&size, sizeof(int));
		fs->Write(fRowLabels[i], size);
	}
	for (i = 0; i < nCols; i++)
	{
		size = strlen(fColLabels[i])+1;
		fs->Write(&size, sizeof(int));
		fs->Write(fColLabels[i], size);
	}

	//---------------------------------------------
	//  Data
	//---------------------------------------------
	size = sizeof(T);
	fs->Write(&size, sizeof(size));
	for (i = 0; i < nRows; i++)
		for (j = 0; j < nCols; j++)
		{
			fs->Write(&fData[i*nCols + j], sizeof(T));
		}
	delete fs;
	return true;
}

//----------------------------------------------------------------------
// DataTable::Register
//    Register this table and all it's variables with Watch.
//----------------------------------------------------------------------
template<class T> void DataTable<T>::Register()
{
	if (fRegistered)
	{
		RegisterLabels();
		return;
	}

	fRegistered = true;
#ifdef ENABLE_WATCHCLIENT
	WatchClient::Instance()->RegisterTable(TableName(), this);
#endif //ENABLE_WATCHCLIENT
	RegisterLabels();
	RegisterVariables();
}

//----------------------------------------------------------------------
// DataTable::RegisterLabels
//    Register this table and all it's variables with Watch.
//----------------------------------------------------------------------
template<class T> void DataTable<T>::RegisterLabels()
{
#ifdef ENABLE_WATCHCLIENT
	WatchClient* wc = WatchClient::Instance();
	int i;
	// Rows
	for (i = 0; i < nRows; i++)
		wc->RegisterLabel(TableName(), this, i, fRowLabels[i], true);
	// Columns
	for (i = 0; i < nCols; i++)
		wc->RegisterLabel(TableName(), this, i, fColLabels[i], false);
#endif //ENABLE_WATCHCLIENT
}

//----------------------------------------------------------------------
// DataTable::NewRowColumn
//    The watch program is requesting a new row or column.  I'll
//    add both because that's the kind of man I am.
//----------------------------------------------------------------------
template<class T> bool DataTable<T>::NewRowColumn(char* name, bool isRow)
{
	//---------------------------------------------
	//  Add the label (need a new array of labels.
	//---------------------------------------------
	if (isRow)
	{
		AppendRowLabel(name); // This increments nRow
#ifdef ENABLE_WATCHCLIENT
		WatchClient::Instance()->RegisterLabel(TableName(), this, nRows-1,
															fRowLabels[nRows-1], true);
#endif //ENABLE_WATCHCLIENT
	}
	else
	{
		AppendColLabel(name); // This increments nCols
#ifdef ENABLE_WATCHCLIENT
		WatchClient::Instance()->RegisterLabel(TableName(), this, nCols-1,
															fColLabels[nCols-1], false);
#endif //ENABLE_WATCHCLIENT
	}

	//---------------------------------------------
	//  Copy the old data in.
	//---------------------------------------------
	T* tmpdata = new T[nRows*nCols];
	int oldRows = nRows;
	int oldCols = nCols;

	if (isRow)
		oldRows--;
	else
		oldCols--;

	int i, j;
	for (i = 0; i < oldRows; i++)
		for (j = 0; j < oldCols; j++)
		{
			tmpdata[i*nCols+j] = fData[i*oldCols+j];
		}
	//-------------------------------------------------
	//  Apply the new data and register the new label.
	//-------------------------------------------------
	delete[] fData;
	fData = tmpdata;
	return true;
}

//----------------------------------------------------------------------
// DataTable::AppendLabel
//    Add a label to the end of a label list.
//----------------------------------------------------------------------
template<class T> void DataTable<T>::AppendLabel(char* label, char*** list, int* size)
{
	int i;
	char** tmplabels;
	tmplabels = new char*[(*size)+1];
	for (i = 0; i < (*size); i++)
	{
		tmplabels[i] = (*list)[i];
	}
	tmplabels[i] = new char[strlen(label)+1];
	strcpy(tmplabels[i], label);
	delete[] *list;
	*list = tmplabels;
	(*size)++;
}

//----------------------------------------------------------------------
// DataTable::FillRowColumn
//    Fill the entire row or column with the data given.
//----------------------------------------------------------------------
template<class T> bool DataTable<T>::FillRowColumn(TABLE_DATA& data)
{
	if (data.isRow)
	{
		for (int i = 0; i < nCols; i++)
			SetData(data.row, i, data.dataIndex, data.data);
	}
	else
	{
		for (int i = 0; i < nRows; i++)
			SetData(i, data.column, data.dataIndex, data.data);
	}

//  	int fixed = (data.isRow) ? data.row : data.column;
//  	for (int i = 0; i < fSize; i++)
//  	{
//  		SetData(fixed, i, data.data);
//  	}
	return true;
}

template<class T> void DataTable<T>::SetFile(char* filename)
{
	char* buf = filename;

	// Cut off the ".var" if it's there.
	while(*buf != '\0')
	{
		if (*buf == '.')
		{
			*buf = '\0';
			break;
		}
		buf++;
	}

	// Save off the file name.
  	if (fFile)
  		delete fFile;
	static char* path = "var/";
  	fFile = new char[strlen(filename)+strlen(path)+strlen(".var")+1];
  	sprintf(fFile, "%s%s.var", path, filename);
}
