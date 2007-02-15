/***************************************************************************/
// A cast member is a level of indirection added to an actor so that it can
// be seen in multiple casts
/***************************************************************************/

/***************************************************************************/
// Enumerate the cast members
/***************************************************************************/
inline CCastMember* CCast::GetNextMember(CCastMember* i_curr) const
{
    if (i_curr)
        return (i_curr->GetNextActor());
    return (d_actorHead);
}

/***************************************************************************/
// Find a cast member
/***************************************************************************/
inline bool CCast::IsMember(CActor* i_actor) const
{
    CCastMember* curr = d_actorHead;
    while(curr)
    {
        if (i_actor == curr->GetActor())
			return(true);
        curr = curr->GetNextActor();
    }
    return(false);
}
