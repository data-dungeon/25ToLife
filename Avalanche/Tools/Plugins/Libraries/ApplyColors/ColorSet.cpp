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

#include "ApplyColorsPCH.h"
#include "ColorSet.h"

ColorSet::ColorSet(void) : m_pQueryExpression(NULL)
{
}

ColorSet::ColorSet(TupperwareAggregate *pSetAgg) : m_pQueryExpression(NULL)
{
	Init(pSetAgg);
}

void ColorSet::Init(TupperwareAggregate *pSetAgg)
{
	delete m_pQueryExpression;
	m_pQueryExpression = NULL;

	m_name = pSetAgg->GetName();

	for( TupperwareIterator tupIterator = pSetAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
	{
		// Check to see what the current atom's key is:
		TupperwareAtom *pTupAtom = tupIterator.GetAsAtom();
		const char *pCharKey = pTupAtom->GetKey();

		if (strcmp(pCharKey, "COLOR") == 0)
		{
			if (pTupAtom->GetName() != NULL && pTupAtom->GetAtomType() == TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pColorScalar = tupIterator.GetAsScalar();
				if (pColorScalar->GetDataType() == TupperwareScalar::DATA_STRING)
				{
					int oldNumColorNames = m_colorNames.GetSize();
					int newIndex = m_colorNames.Add(pTupAtom->GetName());
					// added a new one
					if (oldNumColorNames==newIndex)
					{
						bool bFoundColor = false;
						for (int colorIndex=0;colorIndex<NUM_COLORS;colorIndex++)
						{
							if (strcmp(ColorNames[colorIndex],pColorScalar->GetAsString())==0)
							{
								bFoundColor = true;
								break;
							}
						}
						if (!bFoundColor)
						{
							colorIndex = 0;
						}
						m_colorIndices.Add(colorIndex);
					}
				}
			}
		}
		else if (strcmp(pCharKey, "QUERY_EXPRESSION") == 0)
		{
			if (pTupAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
			{
				m_pQueryExpression = new TupperwareAggregate;
				TupperwareAggregate *pExpressionAgg = tupIterator.GetAsAggregate();
				m_pQueryExpression->CopyChildren(pExpressionAgg);
			}
		}
	}
}

ColorSet::~ColorSet()
{
	delete m_pQueryExpression;
}

ColorSet::ColorSet(const ColorSet &s) : 
	m_colorIndices(s.m_colorIndices),
	m_colorNames(s.m_colorNames)
{
	if (s.m_pQueryExpression != NULL)
	{
		m_pQueryExpression = new TupperwareAggregate;
		m_pQueryExpression->CopyChildren(s.m_pQueryExpression);
	}
	else
	{
		m_pQueryExpression = NULL;
	}
}

ColorSet& ColorSet::operator=(const ColorSet& s)
{
	m_colorIndices = s.m_colorIndices;
	m_colorNames = s.m_colorNames;
	delete m_pQueryExpression;
	m_pQueryExpression = NULL;
	if (s.m_pQueryExpression != NULL)
	{
		m_pQueryExpression = new TupperwareAggregate;
		m_pQueryExpression->CopyChildren(s.m_pQueryExpression);
	}
	return *this;
}

const char *ColorSet::GetName(void) const
{
	return m_name;
}
                 
const char *ColorSet::ColorNames[NUM_COLORS] = 
{
	"Black", // COLOR_BLACK
	"Dk Grey", // COLOR_DK_GREY
	"Dk Med Grey", // COLOR_DK_MED_GREY
	"Lt Med Grey", // COLOR_LT_MED_GREY
	"Lt Grey", // COLOR_LT_GREY
	"White", // COLOR_WHITE
	"Dk Red", // COLOR_DK_RED
	"Red", // COLOR_RED
	"Lt Red", // COLOR_LT_RED
	"Dk Orange", // COLOR_DK_ORANGE
	"Orange", // COLOR_ORANGE
	"Lt Orange", // COLOR_LT_ORANGE
	"Dk Yellow", // COLOR_DK_YELLOW
	"Yellow", // COLOR_YELLOW
	"Lt Yellow", // COLOR_LT_YELLOW
	"Dk Green", // COLOR_DK_GREEN
	"Green", // COLOR_GREEN
	"Lt Green", // COLOR_LT_GREEN
	"Dk Cyan", // COLOR_DK_CYAN
	"Cyan", // COLOR_CYAN
	"Lt Cyan", // COLOR_LT_CYAN
	"Dk Blue", // COLOR_DK_BLUE
	"Blue", // COLOR_BLUE
	"Lt Blue", // COLOR_LT_BLUE
	"Dk Magenta", // COLOR_DK_MAGENTA
	"Magenta", // COLOR_MAGENTA
	"Lt Magenta", // COLOR_LT_MAGENTA
	"Black White Gradient", // COLOR_BLACK_WHITE_GRADIENT
	"Red Gradient", // COLOR_RED_GRADIENT
	"Orange Gradient", // COLOR_ORANGE_GRADIENT
	"Yellow Gradient", // COLOR_YELLOW_GRADIENT
	"Green Gradient", // COLOR_GREEN_GRADIENT
	"Cyan Gradient", // COLOR_CYAN_GRADIENT
	"Blue Gradient", // COLOR_BLUE_GRADIENT
	"Magenta Gradient", // COLOR_MAGENTA_GRADIENT
};

ColorSet::ApplyColorValue ColorSet::ColorValues[NUM_COLORS] = 
{
	{ 0.0f, 0.0f, 0.0f }, // COLOR_BLACK
	{ 0.25f, 0.25f, 0.25f }, // COLOR_DK_GREY
	{ 0.5f, 0.5f, 0.5f }, // COLOR_DK_MED_GREY
	{ 0.75f, 0.75f, 0.75f }, // COLOR_LT_MED_GREY
	{ 0.875f, 0.875f, 0.875f }, // COLOR_LT_GREY
	{ 1.0f, 1.0f, 1.0f }, // COLOR_WHITE
	{ 0.5f, 0.0f, 0.0f }, // COLOR_DK_RED
	{ 1.0f, 0.0f, 0.0f }, // COLOR_RED
	{ 1.0f, 0.75f, 0.75f }, // COLOR_LT_RED
	{ 0.5f, 0.25f, 0.0f }, // COLOR_DK_ORANGE
	{ 1.0f, 0.5f, 0.0f }, // COLOR_ORANGE
	{ 1.0f, 0.875f, 0.75f }, // COLOR_LT_ORANGE
	{ 0.5f, 0.5f, 0.0f }, // COLOR_DK_YELLOW
	{ 1.0f, 1.0f, 0.0f }, // COLOR_YELLOW
	{ 1.0f, 1.0f, 0.75f }, // COLOR_LT_YELLOW
	{ 0.0f, 0.5f, 0.0f }, // COLOR_DK_GREEN
	{ 0.0f, 1.0f, 0.0f }, // COLOR_GREEN
	{ 0.75f, 1.0f, 0.75f }, // COLOR_LT_GREEN
	{ 0.0f, 0.5f, 0.5f }, // COLOR_DK_CYAN
	{ 0.0f, 1.0f, 1.0f }, // COLOR_CYAN
	{ 0.75f, 1.0f, 1.0f }, // COLOR_LT_CYAN
	{ 0.0f, 0.0f, 0.5f }, // COLOR_DK_BLUE
	{ 0.0f, 0.0f, 1.0f }, // COLOR_BLUE
	{ 0.75f, 0.75f, 1.0f }, // COLOR_LT_BLUE
	{ 0.5f, 0.0f, 0.5f }, // COLOR_DK_MAGENTA
	{ 1.0f, 0.0f, 1.0f }, // COLOR_MAGENTA
	{ 1.0f, 0.75f, 1.0f }, // COLOR_LT_MAGENTA
	{ 1.0f, 1.0f, 1.0f }, // COLOR_BLACK_WHITE_GRADIENT
	{ 1.0f, 0.0f, 0.0f }, // COLOR_RED_GRADIENT
	{ 1.0f, 0.5f, 0.0f }, // COLOR_ORANGE_GRADIENT
	{ 1.0f, 1.0f, 0.0f }, // COLOR_YELLOW_GRADIENT
	{ 0.0f, 1.0f, 0.0f }, // COLOR_GREEN_GRADIENT
	{ 0.0f, 1.0f, 1.0f }, // COLOR_CYAN_GRADIENT
	{ 0.0f, 0.0f, 1.0f }, // COLOR_BLUE_GRADIENT
	{ 1.0f, 0.0f, 1.0f }, // COLOR_MAGENTA_GRADIENT
};

ColorSet::ApplyColorValue ColorSet::GetColorValue(int index, float colorMultiplier)
{
	ApplyColorValue newColorValue(ColorValues[index]);
	if (colorMultiplier!=1.0f)
	{
		newColorValue.m_red *= colorMultiplier;
		newColorValue.m_green *= colorMultiplier;
		newColorValue.m_blue *= colorMultiplier;
	}
	return newColorValue;
}

int ColorSet::GetNumColorsUsed(void) const
{
	return m_colorNames.GetSize();
}

int ColorSet::GetColorValueIndex(int colorIndex) const
{
	return m_colorIndices[colorIndex];
}

const char *ColorSet::GetColorName(int colorIndex) const
{
	return m_colorNames[colorIndex];
}

ColorSet::ApplyColorValue ColorSet::GetColor(const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const
{
	int colorIndex = 0;
	float colorMultiplier = 1.0f;

	if (m_pQueryExpression!=NULL)
	{
		TupperwareAtom *pStartAtom = m_pQueryExpression->GetFirstChildAtom();
		if (pStartAtom!=NULL)
		{
			CheckExpression(pStartAtom,colorIndex,colorMultiplier,keyNames,valueNames);
		}
	}
	return GetColorValue(colorIndex,colorMultiplier);
}

TupperwareAtom* ColorSet::SkipElseCommands(TupperwareAtom *pTupAtom)
{
	while (pTupAtom)
	{
		const char *pCharKey = pTupAtom->GetKey();
		if (strcmp(pCharKey,"ELSEIF_KEY_EXISTS") == 0 ||
			strcmp(pCharKey,"ELSEIF_KEY_VALUE") == 0 ||
			strcmp(pCharKey,"ELSE") == 0)
		{
			pTupAtom = pTupAtom->GetNextAtom();
		}
		else
		{
			break; // found that is not an else
		}
	}
	return pTupAtom;
}

TupperwareAtom *ColorSet::HandleElseCommands(TupperwareAtom *pTupAtom,int &colorIndex,float &colorMultiplier,const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const
{
	while (pTupAtom)
	{
		const char *pCharKey = pTupAtom->GetKey();
		if (strcmp(pCharKey,"ELSEIF_KEY_EXISTS") == 0)
		{
			bool bExpressionTrue = false;
			if (pTupAtom->GetName())
			{
				if (pTupAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE )
				{
					for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
					{
						if (strcmp(keyNames[keyIndex],pTupAtom->GetName())==0)
						{
							bExpressionTrue = true;
							break;
						}
					}
				}
				if (bExpressionTrue)
				{
					TupperwareAggregate *pIfAgg = (TupperwareAggregate *)pTupAtom;
					CheckExpression(pIfAgg->GetFirstChildAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
					pTupAtom = SkipElseCommands(pTupAtom->GetNextAtom());
					break;
				}
			}
			if (!bExpressionTrue)
			{
				pTupAtom = pTupAtom->GetNextAtom(); // go to next else if there
			}
		}
		else if (strcmp(pCharKey,"ELSEIF_KEY_VALUE") == 0)
		{
			bool bExpressionTrue = false;
			if (pTupAtom->GetName())
			{
				TupString name(pTupAtom->GetName());
				int equalsIndex = name.Find('=');
				// name is key=value
				if (equalsIndex!=-1 && pTupAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE )
				{
					TupString keyName = name.Left(equalsIndex);
					TupString valueName = name.Mid(equalsIndex+1);
					for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
					{
						if (strcmp(keyNames[keyIndex],keyName)==0)
						{
							if (strcmp(valueNames[keyIndex],valueName)==0)
							{
								bExpressionTrue = true;
							}
							break;
						}
					}
				}
				if (bExpressionTrue)
				{
					TupperwareAggregate *pIfAgg = (TupperwareAggregate *)pTupAtom;
					CheckExpression(pIfAgg->GetFirstChildAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
					pTupAtom = SkipElseCommands(pTupAtom->GetNextAtom());
					break;
				}
			}
			if (!bExpressionTrue)
			{
				pTupAtom = pTupAtom->GetNextAtom(); // go to next else if there
			}
		}
		else if (strcmp(pCharKey,"ELSE") == 0)
		{
			TupperwareAggregate *pIfAgg = (TupperwareAggregate *)pTupAtom;
			CheckExpression(pIfAgg->GetFirstChildAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
			pTupAtom = SkipElseCommands(pTupAtom->GetNextAtom());
			break;
		}
		else
		{
			break; // found that is not an else
		}
	}
	return pTupAtom;
}

void ColorSet::CheckExpression(TupperwareAtom *pTupAtom,int &colorIndex,float &colorMultiplier,const TupArray<TupString> &keyNames,const TupArray<TupString> &valueNames) const
{
	while(pTupAtom)
	{
		const char *pCharKey = pTupAtom->GetKey();
		if (strcmp(pCharKey, "SET_COLOR") == 0)
		{
			if (pTupAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pTupScalar = (TupperwareScalar *)pTupAtom;
				if (pTupScalar->GetDataType()==TupperwareScalar::DATA_STRING)
				{
					for (int sourceIndex=0;sourceIndex<m_colorNames.GetSize();sourceIndex++)
					{
						if (strcmp(m_colorNames[sourceIndex],pTupScalar->GetAsString())==0)
						{
							colorIndex = m_colorIndices[sourceIndex];
							break;
						}
					}
				}
			}
			pTupAtom = pTupAtom->GetNextAtom();
		}
		else if (strcmp(pCharKey, "SET_COLOR_MULTIPLIER") == 0)
		{
			// constant multiplier set (SET_COLOR_MULTIPLIER = 1.0)
			if (pTupAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
			{
				TupperwareScalar *pTupScalar = (TupperwareScalar *)pTupAtom;
				if (pTupScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
				{
					colorMultiplier = pTupScalar->GetAsFloat();
				}
			}
			// multiplier set based on key's value (SET_COLOR_MULTIPLIER "keyName" = [ min max default ]
			else if (pTupAtom->GetName() != NULL && pTupAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
			{
				TupperwareList *pTupList = (TupperwareList *)pTupAtom;
				if (pTupList->GetLength() == 3)
				{
					if (pTupList->GetDataType() == TupperwareList::DATA_FLOAT)
					{
						float *pValues = pTupList->GetAsFloat();
						float value = pValues[2]; // set to default
						bool bFound = false;
						for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
						{
							if (strcmp(keyNames[keyIndex],pTupAtom->GetName())==0)
							{
								bFound = true;
								break;
							}
						}
						if (bFound)
						{
							value = (float)atof(valueNames[keyIndex]);
						}
						if (value<pValues[0])
						{
							value = pValues[0];
						}
						if (value>pValues[1])
						{
							value = pValues[1];
						}
						colorMultiplier = (value-pValues[0])/(pValues[1]-pValues[0]);
					}
					else if (pTupList->GetDataType() == TupperwareList::DATA_INT)
					{
						int *pValues = pTupList->GetAsInt();
						int value = pValues[2]; // set to default
						bool bFound = false;
						for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
						{
							if (strcmp(keyNames[keyIndex],pTupAtom->GetName())==0)
							{
								bFound = true;
								break;
							}
						}
						if (bFound)
						{
							value = atoi(valueNames[keyIndex]);
						}
						if (value<pValues[0])
						{
							value = pValues[0];
						}
						if (value>pValues[1])
						{
							value = pValues[1];
						}
						colorMultiplier = ((float)(value-pValues[0]))/((float)(pValues[1]-pValues[0]));
					}
				}
			}
			pTupAtom = pTupAtom->GetNextAtom();
		}
		else if (strcmp(pCharKey, "IF_KEY_EXISTS") == 0)
		{
			bool bExpressionTrue = false;
			if (pTupAtom->GetName())
			{
				if (pTupAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE )
				{
					bool bFound = false;
					for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
					{
						if (strcmp(keyNames[keyIndex],pTupAtom->GetName())==0)
						{
							bExpressionTrue = true;
							break;
						}
					}
					// do the commands within the aggregate
					if (bExpressionTrue)
					{
						TupperwareAggregate *pIfAgg = (TupperwareAggregate *)pTupAtom;
						CheckExpression(pIfAgg->GetFirstChildAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
						pTupAtom = SkipElseCommands(pTupAtom->GetNextAtom());
						break;
					}
				}
			}
			if (!bExpressionTrue)
			{
				pTupAtom = HandleElseCommands(pTupAtom->GetNextAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
				break;
			}
		}
		else if (strcmp(pCharKey, "IF_KEY_VALUE") == 0)
		{
			bool bExpressionTrue = false;
			if (pTupAtom->GetName())
			{
				TupString name(pTupAtom->GetName());
				int equalsIndex = name.Find('=');
				// name is key=value
				if (equalsIndex!=-1 && pTupAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE )
				{
					TupString keyName = name.Left(equalsIndex);
					TupString valueName = name.Mid(equalsIndex+1);
					for (int keyIndex=0;keyIndex<keyNames.GetSize();keyIndex++)
					{
						if (strcmp(keyNames[keyIndex],keyName)==0)
						{
							if (strcmp(valueNames[keyIndex],valueName)==0)
							{
								bExpressionTrue = true;
							}
							break;
						}
					}
				}
				if (bExpressionTrue)
				{
					TupperwareAggregate *pIfAgg = (TupperwareAggregate *)pTupAtom;
					CheckExpression(pIfAgg->GetFirstChildAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
					pTupAtom = SkipElseCommands(pTupAtom->GetNextAtom());
					break;
				}
			}
			if (!bExpressionTrue)
			{
				pTupAtom = HandleElseCommands(pTupAtom->GetNextAtom(),colorIndex,colorMultiplier,keyNames,valueNames);
				break;
			}
		}
		else // command we don't understand
		{
			pTupAtom = pTupAtom->GetNextAtom();
		}
	}
}

