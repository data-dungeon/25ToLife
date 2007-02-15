////////////////////////////////////////////////////////////////////////////
//
// TupperwareWriter
//
// A writer of tupperware files.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Writer.h"

#include "Aggregate.h"
#include "Codec.h"
#include "List.h"
#include "Log.h"
#include "Iterator.h"
#include "Scalar.h"
#include "Comment.h"

////////////////////////////////////////////////////////////////////////////

TupperwareWriter::TupperwareWriter( FILE* file, const char* file_name ) 
{
	m_File = file;
	m_FileName = new char[ strlen( file_name ) + 1 ];
	strcpy( m_FileName, file_name );
}


TupperwareWriter::~TupperwareWriter() 
{
	delete [] m_FileName;
}


int TupperwareWriter::Write( const char* file_name, const TupperwareAggregate* root ) 
{
	FILE* f = fopen( file_name, "wt" );

	if ( f == NULL )
		return 0;

	TupperwareWriter writer( f, file_name );
	writer.WriteAggregate( root, true, 0 );

	fclose( f );
	return 1;
}


void TupperwareWriter::Indent( int indent_level ) 
{
	for ( int i = 0; i < indent_level; i++ ) 
	{
		fprintf( m_File, "\t" );
	}
}

void TupperwareWriter::WriteList( const TupperwareList* list, int indent_level ) 
{
	fprintf(m_File, "\n");
	Indent(indent_level);
	fprintf(m_File, "[\n");
	indent_level+=1;
	Indent(indent_level);
	int length = list->GetLength();
	int step = 0;
	TupperwareAtom::DataType type = list->GetDataType();
	// try to show the list so that it is easier to read
	switch (type)
	{
		case TupperwareAtom::DATA_STRING:
			step = 1;
			break;
		case TupperwareAtom::DATA_FLOAT:
			if (length%3==0)
				step = 3;
			else
				step = 4;
			break;
		case TupperwareAtom::DATA_INT:
			{
				for (step=3;step<16;step++)
				{
					if (length%step==0)
						break;
				}
			}
			break;
	}
	for (int i = 0,curStep = 0; i < length; i++) 
	{
		if (curStep >= step)
		{
			fprintf(m_File, "\n");
			Indent(indent_level);
			curStep = 0;
		}

		if (type == TupperwareAtom::DATA_STRING) 
		{
			PrintString(list->GetAsString()[i]);
		}
		else if (type == TupperwareAtom::DATA_FLOAT) 
		{
			fprintf(m_File, "%f", list->GetAsFloat()[i]);
		}
		else if (type == TupperwareAtom::DATA_INT) 
		{
			fprintf(m_File, "%d", list->GetAsInt()[i]);
		}
		curStep++;
		if (curStep<step)
		{
			fprintf(m_File, " ");
		}
	}

	fprintf(m_File, "\n");
	indent_level -= 1;
	Indent(indent_level);
	fprintf(m_File, "]\n");
}


void TupperwareWriter::WriteScalar(const TupperwareScalar* scalar) 
{
	TupperwareAtom::DataType type = scalar->GetDataType();

	if (type == TupperwareAtom::DATA_STRING) 
	{
		PrintString(scalar->GetAsString());
		fprintf(m_File, "\n");
	} 
	else if (type == TupperwareAtom::DATA_BINARY) 
	{
		PrintBinaryData(scalar);
		fprintf(m_File, "\n");
	}
	else if (type == TupperwareAtom::DATA_FLOAT) 
	{
		fprintf(m_File, "%f\n", scalar->GetAsFloat());
	}
	else if (type == TupperwareAtom::DATA_INT) 
	{
		fprintf(m_File, "%d\n", scalar->GetAsInt());
	}
}

void TupperwareWriter::WriteComment(const TupperwareComment* comment) 
{
	fprintf(m_File,"#%s\n",comment->GetValue());
}

void TupperwareWriter::WriteAggregate(const TupperwareAggregate* aggregate, 
									  bool root_aggregate, 
									  int indent_level) 
{
	if (!root_aggregate) 
	{
		fprintf(m_File, "\n");
		Indent(indent_level);
		fprintf(m_File, "{\n");
		indent_level += 1;
	}

	for (TupperwareIterator i = aggregate->FindAll(); !i.Finished(); i.Next()) 
	{
		TupperwareAtom *atom = i.GetAsAtom();

		Indent(indent_level);

		if (atom->GetAtomType() == TupperwareAtom::ATOM_COMMENT) 
		{
			WriteComment((TupperwareComment *)atom); 
		}
		else
		{
			fprintf(m_File, "%s", atom->GetKey());

			if (atom->GetName()) 
			{
				fprintf(m_File, " ");
				PrintString(atom->GetName());
			}

			fprintf(m_File, " = ");

			if (atom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE) 
			{
				WriteAggregate((TupperwareAggregate *)atom, false, indent_level);
			}
			else if (atom->GetAtomType() == TupperwareAtom::ATOM_LIST) 
			{
				WriteList((TupperwareList *)atom, indent_level);
			}
			else if (atom->GetAtomType() == TupperwareAtom::ATOM_SCALAR) 
			{
				WriteScalar((TupperwareScalar *)atom);
			}
		}
	}

	if (!root_aggregate) 
	{
		indent_level -= 1;
		Indent(indent_level);
		fprintf(m_File, "}\n");
	}
}


void TupperwareWriter::PrintString( const char* str ) 
{
	fprintf(m_File, "\"");
	
	for (int j = 0; str[j]; j++) 
	{
		if (str[j] == '"') 
		{
			fprintf(m_File, "\"\"");
		}
		else 
		{
			fprintf(m_File, "%c", str[j]);
		}
	}
	fprintf(m_File, "\"");
}

void TupperwareWriter::PrintBinaryData( const TupperwareScalar* scalar ) 
{
	int length;
	const void *binaryData = scalar->GetAsBinary(&length);
	char* encoded = TupperwareCodec::BinaryToAscii(binaryData,length);
	
	fprintf(m_File, "$");
	
	for (int j = 0; encoded[j]; j++) 
	{
		if (encoded[j] == '>') 
		{
			fprintf(m_File, ">>");
		}
		else 
		{
			fprintf(m_File, "%c", encoded[j]);
		}
	}
	fprintf(m_File, "$");
	delete [] encoded;
}

////////////////////////////////////////////////////////////////////////////
