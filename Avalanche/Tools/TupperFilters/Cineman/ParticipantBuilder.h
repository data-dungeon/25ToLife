////////////////////////////////////////////////////////////////////////////
//
// ParticipantBuilder.h
//
// Class to build the array of participants
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ParticipantBuilder.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/18/03    Time: 3:33p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * added change node names function
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 6/23/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * parent and child participants
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 2:38p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * static values from parent transform (when skin is attached)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 1:54p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * group transform connecting to boned mesh transform
 * animation from bones connecting to actions for mesh transform
 * added more camera channels for hooking up
 * added instance clip blends
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * first phase of source and instanced actions
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/03    Time: 3:21p
 * Updated in $/Avalanche/tools/TupperFilters/Cineman
 * start of participants classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PARTICIPANT_BUILDER_H
#define PARTICIPANT_BUILDER_H

class Participant;

// this is a duplicated class from OjbectAttributes.  Really should be a utility class that both dlls can use.
class CExclusions
{
public:
	void AddKey( const char* key )
	{
		if ( strlen( key ) )
		{
			m_exclusionList.Add( key );
		}
	}

	bool AddKeysFromFile( const char* filename )
	{
		FILE* fp = fopen( filename, "rt" );
		char buf[ 256 ];
		if ( !fp ) return false;
		while ( fgets( buf, sizeof( buf ), fp ) )
		{
			// kill comments and strip white space from end of line
			char* p;
			if ( ( p = strstr( buf, "//" ) ) != NULL ) { *p = 0; }
			if ( ( p = strstr( buf, ";"  ) ) != NULL ) { *p = 0; }
			for ( p = buf + strlen( buf ) - 1 ; p > buf && isspace( *p ) ; )
				*p-- = 0;

			// find start of key without leading space
			for ( p = buf ; isspace( *p ) ; )
				p++;

			AddKey( p );
		}
		fclose( fp );
		return true;
	}

	bool IsExcluded( const char* key )
	{
		for ( int i = 0 ; i < m_exclusionList.GetSize() ; i++ )
			if ( stricmp( m_exclusionList[ i ], key ) == 0 )
				return true;
		return false;
	}

	void Dump()
	{
		for ( int i = 0 ; i < m_exclusionList.GetSize() ; i++ )
		{
			printf( "exclusion '%s'\n", (const char *)m_exclusionList[ i ] );
		}
	}

private:
	TupArray<TupString> m_exclusionList;
};

class ParticipantBuilder
{
	class ConnectedNode
	{
	public:
		enum Types
		{
			MAKE_ACTION,
			ADD_CHANNELS,
		};

		Types m_type;
		int m_targetNode;
	};

	class ParticipantClip
	{
	public:
		int m_setIndex;
		int m_clipIndex;
		bool operator==(const ParticipantClip &src) const
		{
			return (m_setIndex==src.m_setIndex && m_clipIndex==src.m_clipIndex); 
		}
	};

public:

	ParticipantBuilder(TupperwareAggregate *pSceneAgg,TupArray<Participant> *pParticipants,Keeper<int> *pParticipantNodeRefs);
	ParticipantBuilder(TupperwareAggregate *pSceneAgg,TupArray<Participant> *pParticipants,Keeper<int> *pParticipantNodeRefs,CExclusions *exclusions);
	~ParticipantBuilder();

private:
	void GetParticipants(void);
	void AddParents(void);
	void AddParents(int nodeRef);
	void AddChannel(Participant &participant,Keeper<int> &channelTypes,float value,int type);
	void AddChannels(Participant &participant,Keeper<int> &channelTypes,float *pValues,int numValues,int startType);
	void AddNodeChannels(Participant &participant,Keeper<int> &channelTypes,TupNode &tupNode);
	void AddObjectChannels(Participant &participant,Keeper<int> &channelTypes,int objectRef);
	bool AddNodeSkinConnections(TupNode &tupNode,int nodeRef,int objectRef,int &parentNodeRef);

	void AddAnimation(void);
	int GetChannelType(const char *pAttributeName);
	void ChangeNodeNames(void);

	// I build this
	TupArray<Participant> *m_pParticipants;
	Keeper<int> *m_pParticipantNodeRefs;
	TupArray<Keeper<int> > m_participantChannelTypes;
	TupArray<Keeper<ParticipantClip> > m_participantSourceClips;
	TupArray<Keeper<ParticipantClip> > m_participantInstancedClips;

	Keeper<int> m_connectedSourceNodeRefs;
	TupArray<ConnectedNode> m_connectedNodes;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pRootNodeAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	TupperwareAggregate *m_pAnimSetPoolAgg;
	
	char *GetEnvStr( TupKeyValueContext &tupKeyValueContext , char *s );

	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupNode *m_pTupRootNode;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupAnimSetPool *m_pTupAnimSetPool;

	TupConfigRoot *m_pTupNodeConfigRoot;

	int m_rootNodeReference;

	CExclusions	*m_exclusions;
};

#endif
