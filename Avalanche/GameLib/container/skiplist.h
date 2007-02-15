/********************************************************************
	created:	2003/12/22
	created:	22:12:2003   15:27
	filename: 	c:\dev\avalanche\gamelib\container\skiplist.h
	file path:	c:\dev\avalanche\gamelib\container
	file base:	skiplist
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Interface for a templated skip list.
				The <Key> class must have the '<', '==', and '<=' operators defined.
				The <Key> class should have a sane copy constructor.
				The <Value> class should have a sane copy constructor.
				The list stores a copy of the inserted data, and will call the destructor when Delete( key ) is called.

	additional:
				http://www.cs.umd.edu/~pugh/skiplist.pdf or
				http://epaperpress.com/sortsearch/download/skiplist.pdf

*********************************************************************/

#if !defined( SKIP_LIST_H )
#define SKIP_LIST_H

/**
	Templated skip list implementation.  See http://www.cs.umd.edu/~pugh/ for a detailed
	discussion about skip lists.
*/
template< class Key, class Value, unsigned int DefaultMaxLevel = 13 >
class SkipList
{
public:
	/**@name Constructors */
	//@{
	/**
		Construct a skip list with ListMaxLevel as the max possible skip level.
	*/
			SkipList( void );
	//@}

	/**@name Destructors */
	//@{
	/**
		Cleans up any allocated memory.
	*/
	virtual	~SkipList( void );
	//@}

	/**@name Operators */
	//@{
	//@}

	/**@name Methods */
	//@{
	/**
		Return the number of unique values currently stored in the SkipList.  Used mainly for iteration.
	*/
	virtual unsigned int	Count( void ) const;

	/**
		Return the next item while iterating.
		@param item Reference to the value just iterated.
		@return Returns false when the iteration is complete, of if the list has changed during the iteration.
	*/
	virtual bool	Iterate( Value &item );

	/**
	Return the next item while iterating.
	@param key The key associated with the value.
	@param item Reference to the value just iterated.
	@return Returns false when the iteration is complete, of if the list has changed during the iteration.
	*/
	virtual bool	Iterate( Key &key, Value &item );

	/**
		Prepare the list for iteration.  Must be called once before iteration may begin.  List should not change during iteration.
	*/
	virtual void	IterateInit( void );

	/**
		Take a key and a value, copy the value and associate it with the given key.  If there is
		data already associated with the given key, it is first Deleted, then the new value is inserted.
		@param key The key to associate value with.
		@param value the value to store in the list.
		@return Returns true if the value was successfully inserted into the list.
		@see Delete
	*/
	virtual bool	Insert( const Key &key, const Value &value );

	/**
		Take a key, and remove the associated value from the list.  The destructor (if any) for
		the value and key are called as a result of using this method.
		@param key The key to remove from the list.
		@return Returns true if the key existed in the list, false if the key didn't exist.
	*/
	virtual bool	Delete( const Key &key );

	/**
		Searches the list for the value associated with the given key.  A reference to the lists
		copy of the data is put into the value parameter.
		@param key The key to search for.
		@param value Holds the reference to the found data, if there was one.
		@return Returns true if the key had associated data.
	*/
	virtual bool	Find( const Key &key, Value &value );

	/**
		Clear the list of all keys and their associated data.  Delete is called for every entry in the list.
		@see Delete
	*/
	virtual void	Clear( void );
	//@}

protected:

	/**@name Protected Member Classes*/
	//@{
	/**
	Internal bucket class.
	*/
	class SkipListNode
	{
	public:
		/**@name Constructor */
		//@{
		/**
		Construct a node of level iLevel.
		@param iLevel The level of this node.
		*/
		SkipListNode( unsigned int iLevel );
		//@}

		/**@name Destructor */
		//@{
		/// Destruct the given node.
		virtual	~SkipListNode( void );
		//@}

		/**@name Class members */
		//@{
		/**
		This node's 'level'.  Must be >= 0.
		*/
		unsigned int	m_iLevel;

		/**
		Pointer to array of 'next' pointers.
		*/
		SkipListNode	**m_pChildren;

		/**
		Storage for this node's key.
		*/
		Key				m_Key;

		/**
		Storage for this node's user data.
		*/
		Value			m_Value;
		//@}

	protected:
		SkipListNode( void );
		SkipListNode( SkipListNode &copy );
	};
	//@}

	/**@name Protected Class Methods */
	//@{
	/**
	Create a SkipListNode object, with it's m_pChildren array allocated in the same chunk.
	@param iLevel The level of the newly created node.
	@return Returns the newly created node.
	*/
	virtual SkipListNode	*CreateNode( unsigned int iLevel );
	//@}

	/**@name Protected Class Members */
	//@{
	/**
		Head of the list.  This is a special node that is ListMaxLevel.  Each pointer points to the head of the
		list for that level.  This node has no valid data stored in it.
	*/
	SkipListNode		*m_pHead;		// head of the list - should have m_pHead->m_iLevel == ( ListMaxLevel - 1 )

	/**
		Current max level of the list.  Each time a random level is chosen above this number, if it is less than
		the list's max level, this value is used instead of the level randomly picked.  This helps keep the list
		a little more 'compact'.
	*/
	unsigned int		m_iCurrMaxLevel;

	/**
		The max level for this list.  This is the highest level that may be inserted into.
	*/
	unsigned int		m_iMaxListLevel;

	/**
		Record how many nodes are currently inserted into the skip list.
	*/
	unsigned int		m_iNodeCount;

	/**
		Pointer to the node to return on next call to Iterate.  Will be NULL when iteration is done or invalid (due to list changing during iteration).
	*/
	SkipListNode		*m_pCurrIterationNode;

	/**
		Pool objects for nodes of the varying levels.
	*/
	Pool< SkipListNode >		*m_pNodePools[ DefaultMaxLevel ];

	//@}
};

// the implementation
#include "container/skiplist.hpp"

#endif