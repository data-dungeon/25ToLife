#include "stdafx.h"
#include "TupViewer.h"
#include "MainFrm.h"
#include "PropertyManager.h"
#include "TokenizeString.h"

PropertyManager::PropertyManager(COptionTree &otTree) :
	m_otTree(otTree)
{
}

void PropertyManager::BuildProperties(TupperwareAtom *pAtom,const char *pPropertyKey)
{
	TupArray<COptionTreeItem *> parentOptionItemStack;
	parentOptionItemStack.Add(NULL); // start with root of tree

	m_otTree.DeleteAllItems();
	m_propertyEntries.RemoveAll();
	int keyIndex = m_propertyKeys.GetIndex(pPropertyKey);
	if (keyIndex!=-1)
	{
		PropertyTableEntryData *pPropertyTableEntryData = m_propertyEntryDataArray[keyIndex];
		while(pPropertyTableEntryData)
		{
			switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_TYPE_MASK)
			{
				case PropertyTableEntryData::FLAGS_TYPE_END:
					// done
					pPropertyTableEntryData = NULL;
					break;
				case PropertyTableEntryData::FLAGS_TYPE_KEY:
					{
						if (pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
						{
							TupperwareAggregate *pAgg = (TupperwareAggregate *)pAtom;
							if (pPropertyTableEntryData->m_pChildKey)
							{
								if (pPropertyTableEntryData->m_pChildName)
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKeyAndName(pPropertyTableEntryData->m_pChildKey,pPropertyTableEntryData->m_pChildName);
									if (pChildAtom)
									{
										AddPropertyKey(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
								else
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKey(pPropertyTableEntryData->m_pChildKey);
									if (pChildAtom)
									{
										AddPropertyKey(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
							}
							else
							{
								AddPropertyKey(pAtom,pPropertyTableEntryData,parentOptionItemStack);
							}
						}
						else
						{
							AddPropertyKey(pAtom,pPropertyTableEntryData,parentOptionItemStack);
						}
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_NAME:
					{
						if (pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
						{
							TupperwareAggregate *pAgg = (TupperwareAggregate *)pAtom;
							if (pPropertyTableEntryData->m_pChildKey)
							{
								if (pPropertyTableEntryData->m_pChildName)
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKeyAndName(pPropertyTableEntryData->m_pChildKey,pPropertyTableEntryData->m_pChildName);
									if (pChildAtom)
									{
										AddPropertyName(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
								else
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKey(pPropertyTableEntryData->m_pChildKey);
									if (pChildAtom)
									{
										AddPropertyName(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
							}
							else
							{
								AddPropertyName(pAtom,pPropertyTableEntryData,parentOptionItemStack);
							}
						}
						else
						{
							AddPropertyName(pAtom,pPropertyTableEntryData,parentOptionItemStack);
						}
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_DATA:
					{
						if (pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
						{
							TupperwareAggregate *pAgg = (TupperwareAggregate *)pAtom;
							if (pPropertyTableEntryData->m_pChildKey)
							{
								if (pPropertyTableEntryData->m_pChildName)
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKeyAndName(pPropertyTableEntryData->m_pChildKey,pPropertyTableEntryData->m_pChildName);
									if (pChildAtom)
									{
										AddPropertyData(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
								else
								{
									TupperwareAtom *pChildAtom = pAgg->FindAtomByKey(pPropertyTableEntryData->m_pChildKey);
									if (pChildAtom)
									{
										AddPropertyData(pChildAtom,pPropertyTableEntryData,parentOptionItemStack);
									}
								}
							}
						}
						else
						{
							AddPropertyData(pAtom,pPropertyTableEntryData,parentOptionItemStack);
						}
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_SIZE:
					break;
				case PropertyTableEntryData::FLAGS_TYPE_TABLE_ENTRY:
					{
						COptionTreeItem *pOptionTreeItemParent = GetOptionTreeItemParent(pPropertyTableEntryData,parentOptionItemStack);
						COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItem(), pOptionTreeItemParent);

						SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
					}
					break;
			}
			if (pPropertyTableEntryData)
				pPropertyTableEntryData++;
		}
	}
	m_otTree.Invalidate();
}

void PropertyManager::SetGeneralOptionTreeData(TupperwareAtom *pAtom,COptionTreeItem *pOptionTreeItem,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack)
{
	if (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_READ_ONLY)
		pOptionTreeItem->ReadOnly(TRUE);

	if (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_EXPANDED)
		pOptionTreeItem->Expand(TRUE);

	CString label = "Label";
	if (pPropertyTableEntryData->m_pLabel)
		label = pPropertyTableEntryData->m_pLabel;
	pOptionTreeItem->SetLabelText(label);

	CString infoText = "Info Text";
	if (pPropertyTableEntryData->m_pInfoText)
		infoText = pPropertyTableEntryData->m_pInfoText;
	pOptionTreeItem->SetInfoText(infoText);

	int indentLevel = (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_INDENT_MASK)>>PropertyTableEntryData::FLAGS_INDENT_SHIFT;
	if ((indentLevel+1)>parentOptionItemStack.GetSize())
		indentLevel = parentOptionItemStack.GetSize()-1;
	parentOptionItemStack.SetSize(indentLevel+1);
	parentOptionItemStack.Add(pOptionTreeItem);

	PropertyTableEntry newPropertyTableEntry;
	newPropertyTableEntry.m_pEntryData = pPropertyTableEntryData;
	newPropertyTableEntry.m_pAtom = pAtom;
	newPropertyTableEntry.m_optionTreeItems.Add(pOptionTreeItem);

	pOptionTreeItem->SetData((void *)m_propertyEntries.GetSize());
	m_propertyEntries.Add(newPropertyTableEntry);
}

void PropertyManager::SetGeneralOptionTreeDataSub(COptionTreeItem *pOptionTreeItem,PropertyTableEntryData *pPropertyTableEntryData,const char *pLabel,const char *pInfoText)
{
	if (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_READ_ONLY)
		pOptionTreeItem->ReadOnly(TRUE);

	CString label = "Label";
	if (pLabel)
		label = pLabel;
	pOptionTreeItem->SetLabelText(label);

	CString infoText = "Info Text";
	if (pInfoText)
		infoText = pInfoText;
	pOptionTreeItem->SetInfoText(infoText);

	PropertyTableEntry &lastPropertyTableEntry =	m_propertyEntries[m_propertyEntries.GetSize()-1];

	ASSERT(lastPropertyTableEntry.m_pEntryData == pPropertyTableEntryData);
	lastPropertyTableEntry.m_optionTreeItems.Add(pOptionTreeItem);

	pOptionTreeItem->SetData((void *)(m_propertyEntries.GetSize()-1));
}


COptionTreeItem *PropertyManager::GetOptionTreeItemParent(PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack)
{
	int indentLevel = (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_INDENT_MASK)>>PropertyTableEntryData::FLAGS_INDENT_SHIFT;
	if ((indentLevel+1)>parentOptionItemStack.GetSize())
		indentLevel = parentOptionItemStack.GetSize()-1;
	return parentOptionItemStack[indentLevel];
}

void PropertyManager::AddPropertyKey(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack)
{
	CString valueString;
	if (pAtom->GetKey())
	{
		valueString = pAtom->GetKey();
	}

	COptionTreeItem *pOptionTreeItemParent = GetOptionTreeItemParent(pPropertyTableEntryData,parentOptionItemStack);
	COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pOptionTreeItemParent);

	SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);

	COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
	if (pNewOptionTreeItemEdit->CreateEditItem(0,0) == TRUE)
	{
		pNewOptionTreeItemEdit->SetEditString(valueString);
	}
}

void PropertyManager::AddPropertyName(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack)
{
	CString valueString;
	if (pAtom->GetName())
	{
		valueString = pAtom->GetName();
	}

	COptionTreeItem *pOptionTreeItemParent = GetOptionTreeItemParent(pPropertyTableEntryData,parentOptionItemStack);
	COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pOptionTreeItemParent);

	SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);

	COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
	if (pNewOptionTreeItemEdit->CreateEditItem(0,0) == TRUE)
	{
		pNewOptionTreeItemEdit->SetEditString(valueString);
	}
}

// figure out how many times to shift a mask to bring it down to the lsbits
static int ComputeMaskShift(int mask)
{
	int shift = 0;
	if (mask)
	{
		while ((mask&1)==0)
		{
			mask = mask>>1;
			shift++;
		}
	}
	return shift;
}

void PropertyManager::AddPropertyData(TupperwareAtom *pAtom,PropertyTableEntryData *pPropertyTableEntryData,TupArray<COptionTreeItem *> &parentOptionItemStack)
{
	COptionTreeItem *pOptionTreeItemParent = GetOptionTreeItemParent(pPropertyTableEntryData,parentOptionItemStack);

	switch (pAtom->GetAtomType())
	{
		case TupperwareAtom::ATOM_SCALAR:
			{
				TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
				switch (pScalar->GetDataType())
				{
					case TupperwareScalar::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_NORMAL:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
										pNewOptionTreeItemEdit->SetSignificantDigits(0);
										pNewOptionTreeItemEdit->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_INT_ALLOW_NEGATIVE_VALUES) ? true : false);
										if (pNewOptionTreeItemEdit->CreateEditItem(OT_EDIT_NUMERICAL,0) == TRUE)
										{
											pNewOptionTreeItemEdit->SetEditInt(pScalar->GetAsInt());
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_SPINNER:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemSpinner(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemSpinner *pNewOptionTreeItemSpinner = (COptionTreeItemSpinner *)pNewOptionTreeItem;
										double value = pScalar->GetAsInt();
										double minValue = 0.0;
										double maxValue = 100.0;
										double incrementValue = 1.0;
										SpinnerValues *pSpinnerValues = (SpinnerValues *)pPropertyTableEntryData->m_pData;
										if (pSpinnerValues)
										{
											minValue = pSpinnerValues->m_min;
											maxValue = pSpinnerValues->m_max;
											incrementValue = pSpinnerValues->m_incrementBy;
										}
										pNewOptionTreeItemSpinner->SetSignificantDigits(0);
										pNewOptionTreeItemSpinner->SetIncrementBy(incrementValue);
										if (pNewOptionTreeItemSpinner->CreateSpinnerItem(OT_EDIT_USEREDIT,value,minValue,maxValue) == TRUE)
										{
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_CHECKBOX:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemCheckBox(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemCheckBox *pNewOptionTreeItemCheckBox = (COptionTreeItemCheckBox *)pNewOptionTreeItem;
										DWORD dwOptions = OT_CHECKBOX_SHOWCHECK | OT_CHECKBOX_SHOWTEXT;
										int mask = (int)pPropertyTableEntryData->m_pData;
										int checked = (pScalar->GetAsInt() & mask) ? TRUE : FALSE;
										CString checkedString = "True";
										CString unCheckedString = "False";
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TupArray<TupString> tokenStrings;
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
											if (tokenStrings.GetSize()==2)
											{
												checkedString = tokenStrings[0];
												unCheckedString = tokenStrings[1];
											}
										}
   										if (pNewOptionTreeItemCheckBox->CreateCheckBoxItem(checked, dwOptions) == TRUE)
										{
											pNewOptionTreeItemCheckBox->SetCheckText(checkedString,unCheckedString);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_RADIO_GROUP:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemRadio(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemRadio *pNewOptionTreeItemRadio = (COptionTreeItemRadio *)pNewOptionTreeItem;
										int mask = (int)pPropertyTableEntryData->m_pData;
										ASSERT(pPropertyTableEntryData->m_pOptionString);
										TupArray<TupString> tokenStrings;
										TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										int maskShift = ComputeMaskShift(mask);
										int selection = (pScalar->GetAsInt()&mask)>>maskShift;
										if (pNewOptionTreeItemRadio->CreateRadioItem() == TRUE)
										{
											int numRadioItems = tokenStrings.GetSize();
											for (int radioIndex=0;radioIndex<numRadioItems;radioIndex++)
											{
												CString radioName = tokenStrings[radioIndex];
												BOOL bChecked = (radioIndex == selection) ? TRUE : FALSE;
												pNewOptionTreeItemRadio->InsertNewRadio(radioName,bChecked);
											}
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_COMBO_BOX:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemComboBox(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemComboBox *pNewOptionTreeItemComboBox = (COptionTreeItemComboBox *)pNewOptionTreeItem;
										int mask = (int)pPropertyTableEntryData->m_pData;
										ASSERT(pPropertyTableEntryData->m_pOptionString);
										TupArray<TupString> tokenStrings;
										TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										int maskShift = ComputeMaskShift(mask);
										int selection = (pScalar->GetAsInt()&mask)>>maskShift;
										if (pNewOptionTreeItemComboBox->CreateComboItem(NULL) == TRUE)
										{
											int numRadioItems = tokenStrings.GetSize();
											for (int radioIndex=0;radioIndex<numRadioItems;radioIndex++)
											{
												CString radioName = tokenStrings[radioIndex];
												pNewOptionTreeItemComboBox->AddString(radioName);
											}
											pNewOptionTreeItemComboBox->SetCurSel(selection);
										}
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_NORMAL:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
										pNewOptionTreeItemEdit->SetSignificantDigits((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS)>>PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT);
										pNewOptionTreeItemEdit->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES) ? true : false);
										if (pNewOptionTreeItemEdit->CreateEditItem(OT_EDIT_NUMERICAL,0) == TRUE)
										{
											pNewOptionTreeItemEdit->SetEditFloat(pScalar->GetAsFloat());
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_SPINNER:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemSpinner(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemSpinner *pNewOptionTreeItemSpinner = (COptionTreeItemSpinner *)pNewOptionTreeItem;
										double value = pScalar->GetAsFloat();
										double minValue = 0.0;
										double maxValue = 100.0;
										double incrementValue = 1.0;
										SpinnerValues *pSpinnerValues = (SpinnerValues *)pPropertyTableEntryData->m_pData;
										if (pSpinnerValues)
										{
											minValue = pSpinnerValues->m_min;
											maxValue = pSpinnerValues->m_max;
											incrementValue = pSpinnerValues->m_incrementBy;
										}
										pNewOptionTreeItemSpinner->SetSignificantDigits((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS)>>PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT);
										pNewOptionTreeItemSpinner->SetIncrementBy(incrementValue);
										if (pNewOptionTreeItemSpinner->CreateSpinnerItem(OT_EDIT_USEREDIT,value,minValue,maxValue) == TRUE)
										{
										}
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_STRING:
						{
							COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pOptionTreeItemParent);
							SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
							COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
							DWORD dwOptions = 0;
							if (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_SCALAR_STRING_MULTILINE)
								dwOptions |= OT_EDIT_MULTILINE;

							if (pNewOptionTreeItemEdit->CreateEditItem(dwOptions,0) == TRUE)
							{
								CString valueString = pScalar->GetAsString();
								pNewOptionTreeItemEdit->SetEditString(valueString);
							}
						}
						break;
					case TupperwareScalar::DATA_BINARY:
						break;
				}
			}
			break;
		case TupperwareAtom::ATOM_LIST:
			{
				TupperwareList *pList = (TupperwareList *) pAtom;
				switch (pList->GetDataType())
				{
					case TupperwareList::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_GRID:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemList(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemList *pNewOptionTreeItemList = (COptionTreeItemList *)pNewOptionTreeItem;
										pNewOptionTreeItemList->SetSignificantDigits(0);
										pNewOptionTreeItemList->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_INT_ALLOW_NEGATIVE_VALUES) ? true : false);
										pNewOptionTreeItemList->SetNumColumns(((pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_MASK)>>PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)+1);
										pNewOptionTreeItemList->SetOption(OT_LIST_NUMERICAL,TRUE);
										pNewOptionTreeItemList->SetOption(OT_LIST_ADJUST_COLUMNS,(pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE)?TRUE:FALSE);
										int numItems = pList->GetLength();
										pNewOptionTreeItemList->SetNumItems(numItems);
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											pNewOptionTreeItemList->SetItemInt(itemIndex,pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_SUB:
									{
										// parent item
										COptionTreeItem *pNewOptionTreeItemParent = m_otTree.InsertItem(new COptionTreeItem(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItemParent,pPropertyTableEntryData,parentOptionItemStack);

										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										if (tokenStrings.GetSize()<numItems)
											numItems = tokenStrings.GetSize();
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pNewOptionTreeItemParent);
											CString label,infoText;
											label.Format("Value %d",itemIndex);
											infoText = label;

											TupArray<TupString> tokenSubStrings;
											TokenizeString::BuildArray(tokenStrings[itemIndex],"|",tokenSubStrings);
											label = tokenSubStrings[0];
											infoText = tokenSubStrings[(tokenSubStrings.GetSize()>1)?1:0];

											SetGeneralOptionTreeDataSub(pNewOptionTreeItem,pPropertyTableEntryData,label,infoText);
											COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
											pNewOptionTreeItemEdit->SetSignificantDigits(0);
											pNewOptionTreeItemEdit->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_INT_ALLOW_NEGATIVE_VALUES) ? true : false);
											if (pNewOptionTreeItemEdit->CreateEditItem(OT_EDIT_NUMERICAL,0) == TRUE)
											{
												pNewOptionTreeItemEdit->SetEditInt(pValues[itemIndex]);
											}

										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_COLOR:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemColor(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);

										int colors[3] = {0,0,0};

										int numItems = pList->GetLength();
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<3;itemIndex++)
										{
											if (itemIndex<numItems)
												colors[itemIndex] = pValues[itemIndex];
										}
										COLORREF color = RGB(colors[0],colors[1],colors[2]);
										COptionTreeItemColor *pNewOptionTreeItemColor = (COptionTreeItemColor *)pNewOptionTreeItem;
										if (pNewOptionTreeItemColor->CreateColorItem(OT_COLOR_SHOWHEX,color,RGB(0,0,0)) == TRUE)
										{
										}
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_GRID:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemList(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemList *pNewOptionTreeItemList = (COptionTreeItemList *)pNewOptionTreeItem;
										pNewOptionTreeItemList->SetSignificantDigits((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS)>>PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT);
										pNewOptionTreeItemList->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES) ? true : false);
										pNewOptionTreeItemList->SetNumColumns(((pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_MASK)>>PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)+1);
										pNewOptionTreeItemList->SetOption(OT_LIST_NUMERICAL,TRUE);
										pNewOptionTreeItemList->SetOption(OT_LIST_ADJUST_COLUMNS,(pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE)?TRUE:FALSE);
										int numItems = pList->GetLength();
										pNewOptionTreeItemList->SetNumItems(numItems);
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											pNewOptionTreeItemList->SetItemFloat(itemIndex,pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_SUB:
									{
										// parent item
										COptionTreeItem *pNewOptionTreeItemParent = m_otTree.InsertItem(new COptionTreeItem(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItemParent,pPropertyTableEntryData,parentOptionItemStack);

										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										if (tokenStrings.GetSize()<numItems)
											numItems = tokenStrings.GetSize();
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pNewOptionTreeItemParent);
											CString label,infoText;
											label.Format("Value %d",itemIndex);
											infoText = label;

											TupArray<TupString> tokenSubStrings;
											TokenizeString::BuildArray(tokenStrings[itemIndex],"|",tokenSubStrings);
											label = tokenSubStrings[0];
											infoText = tokenSubStrings[(tokenSubStrings.GetSize()>1)?1:0];

											SetGeneralOptionTreeDataSub(pNewOptionTreeItem,pPropertyTableEntryData,label,infoText);
											COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
											pNewOptionTreeItemEdit->SetSignificantDigits((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS)>>PropertyTableEntryData::FLAGS_FLOAT_SIGNIFICANT_DIGITS_SHIFT);
											pNewOptionTreeItemEdit->SetAllowNegativeValues((pPropertyTableEntryData->m_flags&PropertyTableEntryData::FLAGS_FLOAT_ALLOW_NEGATIVE_VALUES) ? true : false);
											if (pNewOptionTreeItemEdit->CreateEditItem(OT_EDIT_NUMERICAL,0) == TRUE)
											{
												pNewOptionTreeItemEdit->SetEditFloat(pValues[itemIndex]);
											}
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_COLOR:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemColor(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);

										int colors[3] = {0,0,0};

										int numItems = pList->GetLength();
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<3;itemIndex++)
										{
											if (itemIndex<numItems)
											{
												float color = pValues[itemIndex];
												if (color<0.0f) color = 0.0f;
												if (color>1.0f) color = 1.0f;
												colors[itemIndex] = (int)(color*255.0f);
											}
										}
										COLORREF color = RGB(colors[0],colors[1],colors[2]);
										COptionTreeItemColor *pNewOptionTreeItemColor = (COptionTreeItemColor *)pNewOptionTreeItem;
										if (pNewOptionTreeItemColor->CreateColorItem(OT_COLOR_SHOWHEX,color,RGB(0,0,0)) == TRUE)
										{
										}
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_STRING:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_GRID:
									{
										COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemList(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItem,pPropertyTableEntryData,parentOptionItemStack);
										COptionTreeItemList *pNewOptionTreeItemList = (COptionTreeItemList *)pNewOptionTreeItem;
										pNewOptionTreeItemList->SetNumColumns(((pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_MASK)>>PropertyTableEntryData::FLAGS_GRID_COLUMNS_SHIFT)+1);
										pNewOptionTreeItemList->SetOption(OT_LIST_ADJUST_COLUMNS,(pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_GRID_COLUMNS_ADJUSTABLE)?TRUE:FALSE);
										int numItems = pList->GetLength();
										pNewOptionTreeItemList->SetNumItems(numItems);
										char **pValues = pList->GetAsString();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											CString value("NULL");
											if (pValues[itemIndex])
											{
												value = pValues[itemIndex];
											}
											pNewOptionTreeItemList->SetItemString(itemIndex,value);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_SUB:
									{
										// parent item
										COptionTreeItem *pNewOptionTreeItemParent = m_otTree.InsertItem(new COptionTreeItem(), pOptionTreeItemParent);
										SetGeneralOptionTreeData(pAtom,pNewOptionTreeItemParent,pPropertyTableEntryData,parentOptionItemStack);

										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										char **pValues = pList->GetAsString();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItem *pNewOptionTreeItem = m_otTree.InsertItem(new COptionTreeItemEdit(), pNewOptionTreeItemParent);
											CString label,infoText;
											label.Format("Value %d",itemIndex);
											infoText = label;
											if (tokenStrings.GetSize()>itemIndex)
											{
												TupArray<TupString> tokenSubStrings;
												TokenizeString::BuildArray(tokenStrings[itemIndex],"|",tokenSubStrings);
												label = tokenStrings[0];
												infoText = tokenStrings[(tokenStrings.GetSize()>1)?1:0];
											}
											SetGeneralOptionTreeDataSub(pNewOptionTreeItem,pPropertyTableEntryData,label,infoText);
											COptionTreeItemEdit *pNewOptionTreeItemEdit = (COptionTreeItemEdit *)pNewOptionTreeItem;
											if (pNewOptionTreeItemEdit->CreateEditItem(0,0) == TRUE)
											{
												CString value("NULL");
												if (pValues[itemIndex])
												{
													value = pValues[itemIndex];
												}
												pNewOptionTreeItemEdit->SetEditString(value);
											}
										}
									}
									break;
							}
						}
						break;
				}
			}
	}
}


void PropertyManager::UpdateFromOptions(COptionTreeItem *pOptionTreeItem)
{
	int tableEntryIndex = (int)pOptionTreeItem->GetData();
	ASSERT(tableEntryIndex<m_propertyEntries.GetSize());
	PropertyTableEntry &propertyTableEntry = m_propertyEntries[tableEntryIndex];
	PropertyTableEntryData *pPropertyTableEntryData = propertyTableEntry.m_pEntryData;
	TupperwareAtom *pAtom = propertyTableEntry.m_pAtom;

	switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_TYPE_MASK)
	{
		case PropertyTableEntryData::FLAGS_TYPE_KEY:
			{
				COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
				CString value;
				pOptionTreeItemEdit->GetEditString(value);
				pAtom->SetKey(value);
			}
			break;
		case PropertyTableEntryData::FLAGS_TYPE_NAME:
			{
				COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
				CString value;
				pOptionTreeItemEdit->GetEditString(value);
				pAtom->SetName(value);
			}
			break;
		case PropertyTableEntryData::FLAGS_TYPE_DATA:
			{
				UpdateFromOptionsData(pOptionTreeItem,propertyTableEntry);
			}
			break;
		case PropertyTableEntryData::FLAGS_TYPE_SIZE:
		case PropertyTableEntryData::FLAGS_TYPE_TABLE_ENTRY:
			break;
	}
	theApp.RefreshSelected(CMainFrame::WINDOW_TUP_TREE_VIEW_ITEM,pAtom->GetData());
	UpdateFromTree(pAtom);

}

void PropertyManager::UpdateFromOptionsData(COptionTreeItem *pOptionTreeItem,PropertyTableEntry &propertyTableEntry)
{
	PropertyTableEntryData *pPropertyTableEntryData = propertyTableEntry.m_pEntryData;
	TupperwareAtom *pAtom = propertyTableEntry.m_pAtom;

	switch (pAtom->GetAtomType())
	{
		case TupperwareAtom::ATOM_SCALAR:
			{
				TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
				switch (pScalar->GetDataType())
				{
					case TupperwareScalar::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_NORMAL:
									{
										COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
										int value;
										pOptionTreeItemEdit->GetEditInt(value);
										pScalar->SetAsInt(value);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_SPINNER:
									{
										COptionTreeItemSpinner *pOptionTreeItemSpinner = (COptionTreeItemSpinner *)pOptionTreeItem;
										int value;
										pOptionTreeItemSpinner->GetEditInt(value);
										pScalar->SetAsInt(value);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_CHECKBOX:
									{
										COptionTreeItemCheckBox *pOptionTreeItemCheckBox = (COptionTreeItemCheckBox *)pOptionTreeItem;
										int mask = (int)pPropertyTableEntryData->m_pData;
										int value = pScalar->GetAsInt() & ~mask;
										if (pOptionTreeItemCheckBox->GetCheck())
										{
											value |= mask;
										}
										pScalar->SetAsInt(value);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_RADIO_GROUP:
									{
										COptionTreeItemRadio *pOptionTreeItemRadio = (COptionTreeItemRadio *)pOptionTreeItem;
										int mask = (int)pPropertyTableEntryData->m_pData;
										int maskShift = ComputeMaskShift(mask);
										int value = pScalar->GetAsInt() & ~mask;
										int selection = pOptionTreeItemRadio->GetCheckedRadio();
										value |= selection<<maskShift;
										pScalar->SetAsInt(value);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_COMBO_BOX:
									{
										COptionTreeItemComboBox *pOptionTreeItemComboBox = (COptionTreeItemComboBox *)pOptionTreeItem;
										int mask = (int)pPropertyTableEntryData->m_pData;
										int maskShift = ComputeMaskShift(mask);
										int value = pScalar->GetAsInt() & ~mask;
										int selection = pOptionTreeItemComboBox->GetCurSel();
										value |= selection<<maskShift;
										pScalar->SetAsInt(value);
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_NORMAL:
									{
										COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
										float value;
										pOptionTreeItemEdit->GetEditFloat(value);
										pScalar->SetAsFloat(value);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_SPINNER:
									{
										COptionTreeItemSpinner *pOptionTreeItemSpinner = (COptionTreeItemSpinner *)pOptionTreeItem;
										float value;
										pOptionTreeItemSpinner->GetEditFloat(value);
										pScalar->SetAsFloat(value);
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_STRING:
						{
							COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
							CString value;
							pOptionTreeItemEdit->GetEditString(value);
							pScalar->SetAsString(value);
						}
						break;
					case TupperwareScalar::DATA_BINARY:
						break;
					}
			}
			break;
		case TupperwareAtom::ATOM_LIST:
			{
				TupperwareList *pList = (TupperwareList *) pAtom;
				switch (pList->GetDataType())
				{
					case TupperwareList::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_GRID:
									{
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)pOptionTreeItem;
										int numItems;
										pOptionTreeItemList->GetNumItems(numItems);
										TupArray<int> newData;
										newData.SetSize(numItems);
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											int newValue;
											if (pOptionTreeItemList->GetItemInt(itemIndex,newValue))
											{
												newData[itemIndex] = newValue;
											}
										}
										pList->SetAsInt(newData);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_SUB:
									{
										int numTreeItems = propertyTableEntry.m_optionTreeItems.GetSize();
										// start with first child (index 0 is the parent)
										int numItems = pList->GetLength();
										int *pValues = pList->GetAsInt();
										for (int treeItemIndex=1;treeItemIndex<numTreeItems;treeItemIndex++)
										{
											// did we find it?
											if (propertyTableEntry.m_optionTreeItems[treeItemIndex]==pOptionTreeItem)
											{
												COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
												int value;
												if (pOptionTreeItemEdit->GetEditInt(value))
												{
													pValues[treeItemIndex-1]=value;
												}
												break;
											}
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_COLOR:
									{
										COptionTreeItemColor *pOptionTreeItemColor = (COptionTreeItemColor *)pOptionTreeItem;
										COLORREF color = pOptionTreeItemColor->GetColor();
										int numItems = pList->GetLength();
										int *pValues = pList->GetAsInt();
										if (numItems>0) pValues[0] = (int)GetRValue(color);
										if (numItems>1) pValues[1] = (int)GetGValue(color);
										if (numItems>2) pValues[2] = (int)GetBValue(color);
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_GRID:
									{
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)pOptionTreeItem;
										int numItems;
										pOptionTreeItemList->GetNumItems(numItems);
										TupArray<float> newData;
										newData.SetSize(numItems);
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											float newValue;
											if (pOptionTreeItemList->GetItemFloat(itemIndex,newValue))
											{
												newData[itemIndex] = newValue;
											}
										}
										pList->SetAsFloat(newData);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_SUB:
									{
										int numTreeItems = propertyTableEntry.m_optionTreeItems.GetSize();
										// start with first child (index 0 is the parent)
										int numItems = pList->GetLength();
										float *pValues = pList->GetAsFloat();
										for (int treeItemIndex=1;treeItemIndex<numTreeItems;treeItemIndex++)
										{
											// did we find it?
											if (propertyTableEntry.m_optionTreeItems[treeItemIndex]==pOptionTreeItem)
											{
												COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
												float value;
												if (pOptionTreeItemEdit->GetEditFloat(value))
												{
													pValues[treeItemIndex-1]=value;
												}
												break;
											}
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_COLOR:
									{
										COptionTreeItemColor *pOptionTreeItemColor = (COptionTreeItemColor *)pOptionTreeItem;
										COLORREF color = pOptionTreeItemColor->GetColor();
										int numItems = pList->GetLength();
										float *pValues = pList->GetAsFloat();
										if (numItems>0) pValues[0] = ((float)GetRValue(color))/255.0f;
										if (numItems>1) pValues[1] = ((float)GetGValue(color))/255.0f;
										if (numItems>2) pValues[2] = ((float)GetBValue(color))/255.0f;
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_STRING:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_GRID:
									{
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)pOptionTreeItem;
										int numItems;
										pOptionTreeItemList->GetNumItems(numItems);
										TupArray<TupString> newData;
										newData.SetSize(numItems);
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											CString newValue;
											pOptionTreeItemList->GetItemString(itemIndex,newValue);
											newData[itemIndex] = newValue;
										}
										pList->SetAsString(newData);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_SUB:
									{
										int numTreeItems = propertyTableEntry.m_optionTreeItems.GetSize();
										// start with first child (index 0 is the parent)
										int numItems = pList->GetLength();
										TupArray<TupString> newData;
										pList->GetAsString(newData);
										for (int treeItemIndex=1;treeItemIndex<numTreeItems;treeItemIndex++)
										{
											// did we find it?
											if (propertyTableEntry.m_optionTreeItems[treeItemIndex]==pOptionTreeItem)
											{
												COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)pOptionTreeItem;
												CString value;
												pOptionTreeItemEdit->GetEditString(value);
												newData[treeItemIndex-1]=value;
												break;
											}
										}
										pList->SetAsString(newData);
									}
									break;
							}
						}
						break;
				}
			}
	}
}

bool PropertyManager::RegisterProperty(const char *pPropertyKey,PropertyTableEntryData *pPropertyEntries)
{
	TupString key(pPropertyKey);
	int oldSize = m_propertyKeys.GetSize();
	int newIndex = m_propertyKeys.Add(key);
	if (newIndex==oldSize)
	{
	  m_propertyEntryDataArray.Add(pPropertyEntries);
	  return true;
	}
	return false;
}

void PropertyManager::UpdateFromTree(TupperwareAtom *pAtom)
{
	int numTableEntries = m_propertyEntries.GetSize();
	for (int tableEntryIndex=0;tableEntryIndex<numTableEntries;tableEntryIndex++)
	{
		PropertyTableEntry &propertyTableEntry = m_propertyEntries[tableEntryIndex];
		PropertyTableEntryData *pPropertyTableEntryData = propertyTableEntry.m_pEntryData;
		// is this a match?
		if (propertyTableEntry.m_pAtom == pAtom)
		{
			switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_TYPE_MASK)
			{
				case PropertyTableEntryData::FLAGS_TYPE_KEY:
					{
						CString valueString;
						if (pAtom->GetKey())
						{
							valueString = pAtom->GetKey();
						}
						ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
						COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[0];
						pOptionTreeItemEdit->SetEditString(valueString);
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_NAME:
					{
						CString valueString;
						if (pAtom->GetName())
						{
							valueString = pAtom->GetName();
						}
						ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
						COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[0];
						pOptionTreeItemEdit->SetEditString(valueString);
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_DATA:
					{
						UpdateFromTreeData(pAtom,propertyTableEntry);
					}
					break;
				case PropertyTableEntryData::FLAGS_TYPE_SIZE:
					break;
				case PropertyTableEntryData::FLAGS_TYPE_TABLE_ENTRY:
					break;
			}
		}
	}
}

void PropertyManager::UpdateFromTreeData(TupperwareAtom *pAtom,PropertyTableEntry &propertyTableEntry)
{
	PropertyTableEntryData *pPropertyTableEntryData = propertyTableEntry.m_pEntryData;

	switch (pAtom->GetAtomType())
	{
		case TupperwareAtom::ATOM_SCALAR:
			{
				TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
				switch (pScalar->GetDataType())
				{
					case TupperwareScalar::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_NORMAL:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[0];
										pOptionTreeItemEdit->SetEditInt(pScalar->GetAsInt());
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_SPINNER:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemSpinner *pOptionTreeItemSpinner = (COptionTreeItemSpinner *)propertyTableEntry.m_optionTreeItems[0];
										pOptionTreeItemSpinner->SetEditInt(pScalar->GetAsInt());
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_CHECKBOX:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemCheckBox *pOptionTreeItemCheckBox = (COptionTreeItemCheckBox *)propertyTableEntry.m_optionTreeItems[0];
										int mask = (int)pPropertyTableEntryData->m_pData;
										int checked = (pScalar->GetAsInt() & mask) ? TRUE : FALSE;
										pOptionTreeItemCheckBox->SetCheck(checked);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_RADIO_GROUP:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemRadio *pOptionTreeItemRadio = (COptionTreeItemRadio *)propertyTableEntry.m_optionTreeItems[0];
										int mask = (int)pPropertyTableEntryData->m_pData;
										int maskShift = ComputeMaskShift(mask);
										int selection = (pScalar->GetAsInt()&mask)>>maskShift;
										pOptionTreeItemRadio->SetCheckedRadio(selection);
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_INT_COMBO_BOX:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemComboBox *pOptionTreeItemComboBox = (COptionTreeItemComboBox *)propertyTableEntry.m_optionTreeItems[0];
										int mask = (int)pPropertyTableEntryData->m_pData;
										int maskShift = ComputeMaskShift(mask);
										int selection = (pScalar->GetAsInt()&mask)>>maskShift;
										pOptionTreeItemComboBox->SetCurSel(selection);
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_NORMAL:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[0];
										pOptionTreeItemEdit->SetEditFloat(pScalar->GetAsFloat());
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_SCALAR_FLOAT_SPINNER:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemSpinner *pOptionTreeItemSpinner = (COptionTreeItemSpinner *)propertyTableEntry.m_optionTreeItems[0];
										pOptionTreeItemSpinner->SetEditFloat(pScalar->GetAsFloat());
									}
									break;
							}
						}
						break;
					case TupperwareScalar::DATA_STRING:
						{
							ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
							COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[0];
							CString valueString = pScalar->GetAsString();
							pOptionTreeItemEdit->SetEditString(valueString);
						}
						break;
					case TupperwareScalar::DATA_BINARY:
						break;
					}
			}
			break;
		case TupperwareAtom::ATOM_LIST:
			{
				TupperwareList *pList = (TupperwareList *) pAtom;
				switch (pList->GetDataType())
				{
					case TupperwareList::DATA_INT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_GRID:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)propertyTableEntry.m_optionTreeItems[0];
										int numItems = pList->GetLength();
										pOptionTreeItemList->SetNumItems(numItems);
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											pOptionTreeItemList->SetItemInt(itemIndex,pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_SUB:
									{
										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										if (tokenStrings.GetSize()<numItems)
											numItems = tokenStrings.GetSize();
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[itemIndex+1];
											pOptionTreeItemEdit->SetEditInt(pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_INT_COLOR:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemColor *pOptionTreeItemColor = (COptionTreeItemColor *)propertyTableEntry.m_optionTreeItems[0];
										int colors[3] = {0,0,0};

										int numItems = pList->GetLength();
										int *pValues = pList->GetAsInt();
										for (int itemIndex=0;itemIndex<3;itemIndex++)
										{
											if (itemIndex<numItems)
												colors[itemIndex] = pValues[itemIndex];
										}
										COLORREF color = RGB(colors[0],colors[1],colors[2]);
										pOptionTreeItemColor->SetColor(color);
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_FLOAT:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_GRID:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)propertyTableEntry.m_optionTreeItems[0];
										int numItems = pList->GetLength();
										pOptionTreeItemList->SetNumItems(numItems);
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											pOptionTreeItemList->SetItemFloat(itemIndex,pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_SUB:
									{
										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										if (tokenStrings.GetSize()<numItems)
											numItems = tokenStrings.GetSize();
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[itemIndex+1];
											pOptionTreeItemEdit->SetEditFloat(pValues[itemIndex]);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_FLOAT_COLOR:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemColor *pOptionTreeItemColor = (COptionTreeItemColor *)propertyTableEntry.m_optionTreeItems[0];
										int colors[3] = {0,0,0};

										int numItems = pList->GetLength();
										float *pValues = pList->GetAsFloat();
										for (int itemIndex=0;itemIndex<3;itemIndex++)
										{
											if (itemIndex<numItems)
											{
												float color = pValues[itemIndex];
												if (color<0.0f) color = 0.0f;
												if (color>1.0f) color = 1.0f;
												colors[itemIndex] = (int)(color*255.0f);
											}
										}
										COLORREF color = RGB(colors[0],colors[1],colors[2]);
										pOptionTreeItemColor->SetColor(color);
									}
									break;
							}
						}
						break;
					case TupperwareList::DATA_STRING:
						{
							switch (pPropertyTableEntryData->m_flags & PropertyTableEntryData::FLAGS_STYLE_MASK)
							{
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_GRID:
									{
										ASSERT(propertyTableEntry.m_optionTreeItems.GetSize()==1);
										COptionTreeItemList *pOptionTreeItemList = (COptionTreeItemList *)propertyTableEntry.m_optionTreeItems[0];
										int numItems = pList->GetLength();
										pOptionTreeItemList->SetNumItems(numItems);
										char **pValues = pList->GetAsString();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											CString value("NULL");
											if (pValues[itemIndex])
											{
												value = pValues[itemIndex];
											}
											pOptionTreeItemList->SetItemString(itemIndex,value);
										}
									}
									break;
								case PropertyTableEntryData::FLAGS_STYLE_LIST_STRING_SUB:
									{
										TupArray<TupString> tokenStrings;
										if (pPropertyTableEntryData->m_pOptionString)
										{
											TokenizeString::BuildArray(pPropertyTableEntryData->m_pOptionString,",",tokenStrings);
										}
										int numItems = pList->GetLength();
										if (tokenStrings.GetSize()<numItems)
											numItems = tokenStrings.GetSize();
										char **pValues = pList->GetAsString();
										for (int itemIndex=0;itemIndex<numItems;itemIndex++)
										{
											COptionTreeItemEdit *pOptionTreeItemEdit = (COptionTreeItemEdit *)propertyTableEntry.m_optionTreeItems[itemIndex+1];
											CString value("NULL");
											if (pValues[itemIndex])
											{
												value = pValues[itemIndex];
											}
											pOptionTreeItemEdit->SetEditString(value);
										}
									}
									break;
							}
						}
						break;
				}
			}
	}
}
