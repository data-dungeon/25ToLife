/*
**
**  File:   InteractionCategory.hpp
**  Date:   March 31, 2003
**  By:     Bryant Collard
**  Desc:   A bitfield where each bit represents a category used to control
**          the interaction of two entities. This is wrapped in a class
**          instead of just being an unsigned int so it can be expanded.
**          In fact, the width of the field should never be considered as
**          known.
**
**          Most of the functions have versions that use an unsigned integer
**          as an argument. This is so categories can be specified in tables,
**          used in scripts, etc. These functions operate on the "dominant"
**          part of the bitfield (that's everything for now).
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/InteractionCategory.hpp $
**  $Revision: 3 $
**      $Date: 6/25/03 10:26a $
**    $Author: Bcollard $
**
*/

#ifdef INTERACTION_CATEGORY_HPP
#undef INTERACTION_CATEGORY_HPP

// Engine includes //
#include "Layers/Assert/Assert.h"

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory::CInteractionCategory()
{
	d_field = 0;
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory::CInteractionCategory(
		const CInteractionCategory &i_category)
{
	d_field = i_category.d_field;
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory::CInteractionCategory(uint32 i_field)
{
	d_field = i_field;
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory::CInteractionCategory(uint32 i_field, uint i_shift)
{
	d_field = i_field << i_shift;
}

/***************************************************************************/
/***************************************************************************/
inline void CInteractionCategory::Set(const CInteractionCategory &i_category)
{
	d_field = i_category.d_field;
}

/***************************************************************************/
/***************************************************************************/
inline void CInteractionCategory::Set(uint32 i_field)
{
	d_field = i_field;
}

/***************************************************************************/
/***************************************************************************/
inline void CInteractionCategory::Set(uint32 i_field, uint i_shift)
{
	d_field = i_field << i_shift;
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator=(
		const CInteractionCategory &i_category)
{
	d_field = i_category.d_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator=(uint32 i_field)
{
	d_field = i_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::operator==(
		const CInteractionCategory &i_category) const
{
	return(d_field == i_category.d_field);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::operator==(uint32 i_field) const
{
	return(d_field == i_field);
}

/***************************************************************************/
/***************************************************************************/
inline bool operator==(uint32 i_field, const CInteractionCategory &i_category)
{
	return(i_field == i_category.d_field);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::operator!=(
		const CInteractionCategory &i_category) const
{
	return(d_field != i_category.d_field);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::operator!=(uint32 i_field) const
{
	return(d_field != i_field);
}

/***************************************************************************/
/***************************************************************************/
inline bool operator!=(uint32 i_field, const CInteractionCategory &i_category)
{
	return(i_field != i_category.d_field);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory CInteractionCategory::operator~() const
{
	CInteractionCategory result;
	result.d_field = ~d_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory CInteractionCategory::operator&(
		const CInteractionCategory &i_category) const
{
	CInteractionCategory result;
	result.d_field = d_field & i_category.d_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory CInteractionCategory::operator&(uint32 i_field) const
{
	CInteractionCategory result;
	result.d_field = d_field & i_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory operator&(uint32 i_field,
		const CInteractionCategory &i_category)
{
	CInteractionCategory result;
	result.d_field = i_field & i_category.d_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory CInteractionCategory::operator|(
		const CInteractionCategory &i_category) const
{
	CInteractionCategory result;
	result.d_field = d_field | i_category.d_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory CInteractionCategory::operator|(uint32 i_field) const
{
	CInteractionCategory result;
	result.d_field = d_field | i_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory operator|(uint32 i_field,
		const CInteractionCategory &i_category)
{
	CInteractionCategory result;
	result.d_field = i_field | i_category.d_field;
	return(result);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator&=(
		const CInteractionCategory &i_category)
{
	d_field &= i_category.d_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator&=(uint32 i_field)
{
	d_field &= i_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator|=(
		const CInteractionCategory &i_category)
{
	d_field |= i_category.d_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CInteractionCategory::operator|=(uint32 i_field)
{
	d_field |= i_field;
	return(*this);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::PositiveMatch(
		const CInteractionCategory &i_category) const
{
	return((d_field & i_category.d_field) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::PositiveMatch(uint32 i_field) const
{
	return((d_field & i_field) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::NegativeMatch(
		const CInteractionCategory &i_category) const
{
	return((d_field & ~i_category.d_field) != 0);
}

/***************************************************************************/
/***************************************************************************/
inline bool CInteractionCategory::NegativeMatch(uint32 i_field) const
{
	return((d_field & ~i_field) != 0);
}

#endif // INTERACTION_CATEGORY_HPP
