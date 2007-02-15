////////////////////////////////////////////////////////////////////////////
//
// FixRogueModifiers
//
// Repair any problems with modifiers
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_FIX_ROGUE_MODIFIERS_H
#define TUPPER3D_FIX_ROGUE_MODIFIERS_H

class TupObjectPool;
class TupModifierPool;
class TupTriMesh;
class TupModifier;

class TupFixRogueModifiers
{
public:
	TupFixRogueModifiers(TupObjectPool *pTupObjectPool,TupModifierPool *pTupModifierPool);
	void Process(void);

private:
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;

	void FixRogueModifier(int modifierRef,TupperwareAggregate *pModifierContextAgg,TupTriMesh &tupBaseMesh);
	void FixRogueApplyVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void FixRogueVertexPaintModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void FixRogueHoldVCModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void FixRogueFaceAttributeModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void FixRogueSkinModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void FixRogueClothModifier(TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);

};

#endif