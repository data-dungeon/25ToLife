//============================================================================
//=
//= DataMgr.h
//=
//= Manage chunks of data. Whee.
//=
//============================================================================

#ifndef _DATAMGR_H
#define _DATAMGR_H

#include "container/dllist.h"

namespace DataMgr
{
	class DataRecord
	{
	public:
		DataRecord*		next;
		DataRecord*		prev;
		void*				d_userData;
		u16				d_type;
		u16				d_flags;
		u32				d_size;
		bool				IsPermanent();
		void				Clear();
		DataRecord()	{ Clear(); }
	};
	class OwnerRecord
	{
	public:
		OwnerRecord*	next;
		OwnerRecord*	prev;
		void*				d_userData;
		u16				d_type;
		u16				d_flags;
		DoublyLinkedList<DataRecord> d_data;
		DataRecord*		AddData( void* userData, u16 type, u16 flags, u32 size );
		DataRecord*		FindData( void* userData );
		DataRecord*		FindDataByType( u16 type );
		DataRecord*		FirstData();
		void				RemoveData( void* userData );
		bool				IsPermanent();
		void				Clear();
		OwnerRecord() : d_data(false) { Clear(); }
	};
	void Init( int ownerRecords, int dataRecords );
	void Reset();
	void Shutdown();
	bool IsEmpty( bool ignorePermanent );
	OwnerRecord* AddOwner( void* data, u16 type );
	OwnerRecord* FindOwner( void* data );
	OwnerRecord* FirstOwner();
	OwnerRecord* NextOwner();
	void			 RemoveOwner( OwnerRecord* owner );
	void			 MarkPermanent();
};

#endif //_DATAMGR_H
