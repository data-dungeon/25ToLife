
#ifndef CCOMPACTOR_H
#define CCOMPACTOR_H

#include "Game/Misc/ParameterSet.h"
#include "Components/ComponentList.h"
#include "Components/Property.h"
#include "Components/Component.h"

class CCompActor : public CProtagonist
{
public:
	CCompActor(void) : CProtagonist(s_compActorClass), d_components( true ) {}
	CCompActor(const CEntityType &i_type) : CProtagonist( i_type ), d_components( true ) {}
	virtual ~CCompActor(void) { d_components.RemoveAll(); }

	// Handle
	static CCompActor* FromHandle(ActorHandle i_handle);

	// access points that components are allowed to hook into
	virtual void BeginFrame(void);
	virtual void BeginUpdate( void );
	virtual void AttemptUpdate( float i_deltaSec );
	virtual void EndUpdate( float i_deltaSec );
	virtual void EndFrame(void);
	virtual void RunBones();
	virtual void RunAnimation(bool i_forceUpdate);

	virtual bool Initialize( void );

	virtual bool CoupledCollisionDetect(CStageEntity* i_entity, 
			CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect);
	virtual uint CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced);
	virtual bool CoupledCollisionAccept(CStageEntity* i_entity,	CCollisionEvent* i_event);
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);
	virtual uint GetTailoredInteractionHint(CProtagonist* i_actor)
		{return(((CProtagonist*)this)->GetInteractionHint());}

	virtual void SyncActor();

	virtual CInterface* GetInterface(uint i_id);
	virtual CNetActor* CreateNetActor(void);

	// Override master data routines.
	virtual int GetNetMasterMetaDataSize(void);
	virtual bool HasNetMasterMetaDataChangedSinceLastWrite(void);
	virtual void WriteNetMasterMetaData(void* &io_buffer);
	virtual int GetNetMasterDataSize(void);
	virtual bool HasNetMasterDataChangedSinceLastWrite(void);
	virtual void WriteNetMasterData(void* &io_metaDataBuffer,
			void* &io_dataBuffer);
	virtual void ReadNetMasterData(void* &io_metaDataBuffer,
			void* &io_dataBuffer);

	// Override the net data routines.
	virtual void SetupNetData(void);
	virtual void LoadNetData(void);

	// all the actual component stuff
	CActorComponent *AttachComponent( const char *componentname );
	void AttachComponent( CActorComponent *component );
	void DetachComponent( CActorComponent *component );

	// initialize any un-initialized components
	void InitializeComponents( void );

	void ResetAllComponents( void );

	CActorComponentList &GetComponents( void ) { return d_components; }
	CActorComponent *GetComponentByName( const char *name );
	CActorComponent *GetActiveComponentInClass( const char *classname );
	CActorComponent *GetDefaultComponentInClass( const char *classname );
	CActorComponent *GetFirstComponentInClass( const char *classname );
	CActorComponent *GetActiveComponentWithInterface( uint interfaceID );
	CActorComponent *GetFirstComponentWithInterface( uint interfaceID );
	void GetComponentsInClass( const char *classname, CActorComponentList &o_list );
	void GetComponentsWithInterface( uint interfaceID, CActorComponentList &o_list );

	void ResumeSuspendedComponents( void );

	PropertyList &GetProperties( void ) { return d_properties; }
	Property *GetProperty( const char *name ) { return d_properties.GetProperty( name ); }
	Property *QueryProperty( const char *name ) { return d_properties.QueryProperty( name ); }
	void AddProperty( const char *name, PropertyType type, void *dataLocation = NULL, PropertyUpdateFunc *updateFunc = NULL ) { d_properties.AddProperty( *this, name, type, dataLocation, updateFunc ); }
	void SetProperty( const char *name, float f ) { Property *p = GetProperty( name ); if( p ) p->Set( f ); }
	void SetProperty( const char *name, int i ) { Property *p = GetProperty( name ); if( p ) p->Set( i ); }
	void SetProperty( const char *name, Vector3 v ) { Property *p = GetProperty( name ); if( p ) p->Set( v ); }
	void SetProperty( const char *name, DirCos3x3 m ) { Property *p = GetProperty( name ); if( p ) p->Set( m ); }
	void SetProperty( const char *name, bool b ) { Property *p = GetProperty( name ); if( p ) p->Set( b ); }
	void SetProperty( const char *name, void *v ) { Property *p = GetProperty( name ); if ( p ) p->Set( v ); }
	void SetProperty( const char *name, uint32 h ) { Property *p = GetProperty( name ); if ( p ) p->Set( h ); }

protected:
	// properties (put first so it gets created before and deleted after components)
	PropertyList d_properties;
	// component list
	CActorComponentList d_components;

public:
	static const CEntityType &ClassType(void) {return(s_compActorClass);}

private:
	static const CEntityType s_compActorClass;
};

#endif //CCOMPACTOR_H
