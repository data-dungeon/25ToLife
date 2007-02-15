////////////////////////////////////////////////////////////////////////////
//
// PrintTup.cpp
//
// Print each key/value pair in a tupperware file.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include <Assert.h>
#include <string.h>

#include "Tupperware/Aggregate.h"
#include "Tupperware/Atom.h"
#include "Tupperware/Iterator.h"
#include "Tupperware/List.h"
#include "Tupperware/Log.h"
#include "Tupperware/Scalar.h"
#include "Tupperware/path.h"

#include "Tupperware/loadsave.h"

char* Indent( int level ) {
	static char indent[ 256 ];

	level *= 4;
	if ( level > 255 ) {
		level = 255;
	}

	memset( indent, 0, 256 );
	memset( indent, ' ', level );

	return indent;
}

const char* Key( TupperwareAtom* atom ) {
	static char key[ 256 ];
	strcpy( key, "[" );
	if ( atom->GetKey() ) {
		strcat( key, atom->GetKey() );
	}
	strcat( key, "]" );
	return key;
}

const char* Name( TupperwareAtom* atom ) {
	static char name[ 256 ];
	strcpy( name, "\"" );
	if ( atom->GetName() ) {
		strcat( name, atom->GetName() );
	}
	strcat( name, "\"" );
	return name;
}

void PrintList( TupperwareList* list, int indent_level ) {
	static char* type = NULL;

	switch ( list->GetDataType() ) {
	case TupperwareAtom::DATA_FLOAT:
		type = "FLOAT";
		break;
	case TupperwareAtom::DATA_INT:
		type = "INT";
		break;
	case TupperwareAtom::DATA_STRING:
		type = "STRING";
		break;
	case TupperwareAtom::DATA_UNKNOWN:
		type = "UNKNOWN";
		break;
	default:
		TupperwareLog::Write( "PrintList(): Bad data type.  Tupperware file is likely corrupt.\n" );
		assert( 0 );
		break;
	}

	printf( "%sL %s %s %s %d\n", Indent( indent_level ), Key( list ), Name( list ), type, list->GetLength() );
}

void PrintScalar( TupperwareScalar* scalar, int indent_level ) {
	static char* type = NULL;

	switch ( scalar->GetDataType() ) {
	case TupperwareAtom::DATA_FLOAT:
		type = "FLOAT";
		break;
	case TupperwareAtom::DATA_INT:
		type = "INT";
		break;
	case TupperwareAtom::DATA_STRING:
		type = "STRING";
		break;
	case TupperwareAtom::DATA_BINARY:
		type = "BINARY";
		break;
	case TupperwareAtom::DATA_UNKNOWN:
		type = "UNKNOWN";
		break;
	default:
		TupperwareLog::Write( "PrintScalar(): Bad data type.  Tupperware file is likely corrupt.\n" );
		assert( 0 );
		break;
	}

	printf( "%sS %s %s %s\n", Indent( indent_level ), Key( scalar ), Name( scalar ), type );
}

void PrintUnknown( TupperwareAtom* atom, int indent_level ) {
	printf( "%sU %s %s\n", indent_level, Key( atom ), Name( atom ) );
}

void PrintAggregate( TupperwareAggregate* aggregate, int indent_level ) {
	printf( "%sA %s %s\n", Indent( indent_level ), Key( aggregate ), Name( aggregate ) );
	indent_level += 1;
	for ( TupperwareIterator i = aggregate->FindAll(); ! i.Finished(); i.Next() ) {
		TupperwareAtom* atom = i.GetAsAtom();
		int type = atom->GetAtomType();
		switch ( type ) {
		case TupperwareAtom::ATOM_AGGREGATE:
			PrintAggregate( (TupperwareAggregate*)atom, indent_level );
			break;
		case TupperwareAtom::ATOM_LIST:
			PrintList( (TupperwareList*)atom, indent_level );
			break;
		case TupperwareAtom::ATOM_SCALAR:
			PrintScalar( (TupperwareScalar*)atom, indent_level );
			break;
		case TupperwareAtom::ATOM_UNKNOWN:
			PrintUnknown( atom, indent_level );
			break;
		default:
			TupperwareLog::Write( "PrintAggregate(): Bad structure type.  Tupperware file is likely corrupt.\n" );
			assert( 0 );
			break;
		}
	}
}

void test_write( const char* string ) {
	printf( "test_write(): STRING = %s\n", string );
}

int main( int argc, char** argv ) {

	char* file_name;
	if ( argv[ 1 ] ) {
		file_name = argv[ 1 ];
	} else {
		file_name = "test.tup";
	}

	// Read/Write Test
	printf( "READ/WRITE TEST\n" );
	printf( "---------------\n\n" );

	TupperwareLog::Write( "Reading %s...\n", file_name );
	TupperwareAggregate *root = new TupperwareAggregate;
	int result = TupperwareLoadSaveManager::Read(file_name,&root);
	assert(result);
	TupperwareLog::Write( "Printing %s...\n", file_name );
//	PrintAggregate(root, 0);
	TupperwareLog::Write( "Writing out.tup...\n" );
	TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,"outascii.tup",root);
	TupperwareLog::Write( "Finished.\n" );
	TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::BINARY,"outbinary.tup",root,0);
	TupperwareAggregate *newRoot = new TupperwareAggregate;
	result = TupperwareLoadSaveManager::Read("outbinary.tup",&newRoot);
	assert(result);
	TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::BINARY,"outbinary2.tup",newRoot);

	TupperwarePath pathTest(newRoot);
	TupperwareScalar *stringTestScalar1 = pathTest.FindScalar("SCENE/MATERIAL:NMSU_cement1_DOUBLESIDED/PIXELMAP");
	assert(stringTestScalar1);
	TupperwareScalar *stringTestScalar2 = pathTest.FindScalar("SCENE/TESTAGGREGATE1/TESTAGGREGATE2/TESTAGGREGATE3/FLOAT");
	assert(stringTestScalar2);
	pathTest.SetBase(const_cast<TupperwareAggregate *>(stringTestScalar2->GetParent()));
	char buffer[200];
	pathTest.GetPath(stringTestScalar1,buffer,200);
	printf(buffer);
	TupperwareScalar *stringTestScalar3 = pathTest.FindScalar(buffer);

	// Log callback test
	printf( "\n\n" );
	printf( "LOG CALLBACK TEST\n" );
	printf( "-----------------\n\n" );

	TupperwareLog::OnWrite( test_write );
	TupperwareLog::Write( "Callback test...(should show up on stdout)\n" );
	TupperwareLog::OnWrite( NULL );
	TupperwareLog::Write( "Another Callback test...(should show up in tupperware.log)\n" );

	delete root;
	delete newRoot;

	return 0;
}
