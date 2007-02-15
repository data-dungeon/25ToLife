//-----------------------------------------------------------------------------
//	File:		ColorSet.h
//	Created:	November 16, 2004
//				Copyright (c) 2002-2004, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Adam Clayton
//
//	Desc:	The ColorSet handles keeping track of the colors in a color set as well as the query expression
//
//-----------------------------------------------------------------------------

#ifndef __COLORSET__
#define __COLORSET__

class ColorSet
{
public:
	struct ApplyColorValue
	{
		float m_red,m_green,m_blue;
	};

	enum ColorValues
	{
		COLOR_BLACK,
		COLOR_DK_GREY,
		COLOR_DK_MED_GREY,
		COLOR_LT_MED_GREY,
		COLOR_LT_GREY,
		COLOR_WHITE,
		COLOR_DK_RED,
		COLOR_RED,
		COLOR_LT_RED,
		COLOR_DK_ORANGE,
		COLOR_ORANGE,
		COLOR_LT_ORANGE,
		COLOR_DK_YELLOW,
		COLOR_YELLOW,
		COLOR_LT_YELLOW,
		COLOR_DK_GREEN,
		COLOR_GREEN,
		COLOR_LT_GREEN,
		COLOR_DK_CYAN,
		COLOR_CYAN,
		COLOR_LT_CYAN,
		COLOR_DK_BLUE,
		COLOR_BLUE,
		COLOR_LT_BLUE,
		COLOR_DK_MAGENTA,
		COLOR_MAGENTA,
		COLOR_LT_MAGENTA,
		COLOR_BLACK_WHITE_GRADIENT,
		COLOR_RED_GRADIENT,
		COLOR_ORANGE_GRADIENT,
		COLOR_YELLOW_GRADIENT,
		COLOR_GREEN_GRADIENT,
		COLOR_CYAN_GRADIENT,
		COLOR_BLUE_GRADIENT,
		COLOR_MAGENTA_GRADIENT,
		NUM_COLORS
	};

private:
	TupString m_name;
	Keeper<TupString> m_colorNames;
	TupArray<int> m_colorIndices;
	TupperwareAggregate *m_pQueryExpression;

public:

	ColorSet(void);
	ColorSet(TupperwareAggregate *pSetAgg);
	~ColorSet();
	ColorSet(const ColorSet &s);
	ColorSet& operator=(const ColorSet& s);
	void Init(TupperwareAggregate *pSetAgg);

	const char *GetName(void) const;

	ApplyColorValue GetColor(const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const;
	int GetNumColorsUsed(void) const;
	int GetColorValueIndex(int colorIndex) const;
	const char *GetColorName(int colorIndex) const;

private:

	void CheckExpression(TupperwareAtom *pTupAtom,int &colorIndex,float &colorMultiplier,const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const;
	TupperwareAtom* HandleElseCommands(TupperwareAtom *pTupAtom,int &colorIndex,float &colorMultiplier,const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const;

	static const char *ColorNames[NUM_COLORS];
	static ApplyColorValue ColorValues[NUM_COLORS];
	static ApplyColorValue GetColorValue(int index, float colorMultiplier = 1.0f);
	static TupperwareAtom* SkipElseCommands(TupperwareAtom *pTupAtom);


};

#endif