/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/penetrate.h"

CPenetrationTable g_penetrationTable;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SPenetrationData::SPenetrationData()
{
	d_energy = -1.0f;
	d_deflection = 0.0f;
	d_percentBlocked = 0;
	d_percentRicochet = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CPenetrationTable::CPenetrationTable()
{
	d_table = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CPenetrationTable::~CPenetrationTable()
{
	Unload();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SPenetrationData* CPenetrationTable::GetEntry(
int i_surface1,
int i_surface2)
{
	ASSERT(d_table && d_lookup);

	int entry = d_lookup->GetEntry(i_surface1, i_surface2);
	ASSERT(entry >= 0);
	if (entry >= 0)
	   return &d_table[entry];

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CPenetrationTable::Init(int i_size)
{
	delete[] d_table;
	d_table = new SPenetrationData[i_size];
	ASSERT(d_table != NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CPenetrationTable::LoadVariable(const char* i_variable, int i_entry)
{
	ASSERT(d_table && d_lookup);

	if (strcmpi(i_variable, "energy") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_energy);
	else if (strcmpi(i_variable, "deflection") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_deflection);
	else if (strcmpi(i_variable, "percent_blocked") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_percentBlocked);
	else if (strcmpi(i_variable, "percent_ricochet") == 0)
		return d_lookup->ParseNextToken(d_table[i_entry].d_percentRicochet);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CPenetrationTable::Copy(int i_destEntry, int i_srcEntry)
{
	d_table[i_destEntry] = d_table[i_srcEntry];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CPenetrationTable::SaveEntry(int i_entry)
{
	ASSERT(d_table && d_lookup);

	d_lookup->SaveNamedToken("energy", d_table[i_entry].d_energy);
	d_lookup->SaveNamedToken("deflection", d_table[i_entry].d_deflection);
	d_lookup->SaveNamedToken("percent_blocked", d_table[i_entry].d_percentBlocked);
	d_lookup->SaveNamedToken("percent_ricochet", d_table[i_entry].d_percentRicochet);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CPenetrationTable::Unload()
{
	delete[] d_table;
	d_table = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Watch stuff - HACK ALERT!!
///////////////////////////////////////////////////////////////////////////////

#include "stage/SurfaceLookup.h"  // To downcast d_lookup!!

void CPenetrationTable::LoadWatch()
{
	//yuch-- this is done in UnloadWatch, which should be paired with this call
	//    DeleteLabels();
	//    DeleteData();

	//---------------------------------------------
	//  Header
	//---------------------------------------------
	nRows = ((CSurfaceLookup*)d_lookup)->GetNumRow();
	nCols = ((CSurfaceLookup*)d_lookup)->GetNumCol();

	//---------------------------------------------
	//  Labels
	//---------------------------------------------
	fRowLabels = new char*[nRows];
	fColLabels = new char*[nCols];

	int i;
	for (i = 0; i < nRows; i++)
	{
	   char* label = ((CSurfaceLookup*)d_lookup)->GetMappedRow(i);
	   if (!label)
	      continue;
	   fRowLabels[i] = new char[strlen(label) + 1];
	   strcpy(fRowLabels[i], label);
	}
	for (i = 0; i < nCols; i++)
	{
	   char* label = ((CSurfaceLookup*)d_lookup)->GetMappedCol(i);
	   if (!label)
	      continue;
	   fColLabels[i] = new char[strlen(label) + 1];
	   strcpy(fColLabels[i], label);
	}

	//---------------------------------------------
	//  Data
	//---------------------------------------------
	fData = 0;

	Register();
}

///////////////////////////////////////////////////////////////////////////////
// clean up
///////////////////////////////////////////////////////////////////////////////
void CPenetrationTable::UnloadWatch()
{
	DeleteLabels();
	DeleteData();
}

//----------------------------------------------------------------------
// CPenetrationTable::Register
//    Register this table and all it's variables with Watch.
//----------------------------------------------------------------------
void CPenetrationTable::RegisterVariables()
{
#ifdef ENABLE_WATCHCLIENT
	WatchClient* wc = WatchClient::Instance();

	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, -1.0f, 10000.0f, 0, "Energy");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_REAL, 0.0f, 89.0f, 0, "Deflection");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_INT, 0.0f, 100.0f, 0, "PercentBlocked");
	wc->RegisterTableVariable(TableName(), this, VARTYPE_INT, 0.0f, 100.0f, 0, "PercentRicochet");
#endif //ENABLE_WATCHCLIENT
}

//----------------------------------------------------------------------
// CPenetrationTable::RequestData
//    The watch program is asking for some table data.
//----------------------------------------------------------------------
void CPenetrationTable::RequestData(int row, int column)
{
#ifdef ENABLE_WATCHCLIENT
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
	   return;

	VAR_VAL data[4];
	SetVARVAL(data[0], d_table[entry].d_energy);
	SetVARVAL(data[1], d_table[entry].d_deflection);
	SetVARVAL(data[2], d_table[entry].d_percentBlocked);
	SetVARVAL(data[3], d_table[entry].d_percentRicochet);

	WatchClient::Instance()->TableData(this, row, column, data, 4);
#endif //ENABLE_WATCHCLIENT
}

//----------------------------------------------------------------------
// CPenetrationTable::SetData
//    Set table data here.  This is called from the watch program.
//----------------------------------------------------------------------
void CPenetrationTable::SetData(int row, int column, int dataIndex, VAR_VAL& data)
{
#ifdef ENABLE_WATCHCLIENT
	int entry = ((CSurfaceLookup*)d_lookup)->GetMappedEntry(row, column);
	if (entry < 0)
	   return;

	if (dataIndex == 0)
		d_table[entry].d_energy = GetVARVALFloat(data);
	else if (dataIndex == 1)
		d_table[entry].d_deflection = GetVARVALFloat(data);
	else if (dataIndex == 2)
		d_table[entry].d_percentBlocked = GetVARVALInt(data);
	else if (dataIndex == 3)
		d_table[entry].d_percentRicochet = GetVARVALInt(data);
#endif
}

//----------------------------------------------------------------------
// CPenetrationTable::NewRowColumn
//    Disallow adding rows of columns via watch.
//----------------------------------------------------------------------
bool CPenetrationTable::NewRowColumn(char* name, bool isRow)
{
	return true;
}

bool CPenetrationTable::Save(char* filename)
{
	char filepath[100];
	strcpy(filepath, "tables/");
	strcat(filepath, filename);
	int i = 0;
	while(filepath[i] != '\0')
	{
	   if (filepath[i] == '.')
	   {
	      filepath[i] = '\0';
	      break;
	   }
	   i++;
	}

	strcat(filepath, ".tbd");
	d_lookup->Save(filepath);
	return true;
}
