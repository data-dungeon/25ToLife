#include "ClothPCH.h"

float SquaredDistanceToLine(const Point3& lineBegin, const Point3& lineEnd, const Point3& point, bool clamp)
{
	float t = ((point.x - lineBegin.x) * (lineEnd.x - lineBegin.x) + (point.y - lineBegin.y) * (lineEnd.y - lineBegin.y)) / (lineEnd - lineBegin).LengthSquared();
	
	if (clamp)
	{
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0;
	}
	
	Point3 intersection = lineBegin + t * (lineEnd - lineBegin);
	
	return LengthSquared(point - intersection);
}


int FindPoint(Tab<int> pointList, int pointToFind)
{
	for (int i = 0; i < pointList.Count(); ++i)
	{
		if (pointList[i] == pointToFind)
			return i;
	}

	return -1;
}

BOOL SelectMouseProc::HitTest(ClothModData *pModData, ViewExp* pViewport, IPoint2* pt, Object* pObject, INode* pNode)
{
	if (pModData == NULL)
		return FALSE;
	
	Ray ray;

	// create a ray through the current mouse position
	pViewport->MapScreenToWorldRay((float)pt->x, (float)pt->y, ray);
	
	Matrix3 tm = Inverse(pNode->GetObjectTM(pInterface->GetTime()));
	
	ray.p = tm * ray.p;
	ray.dir = VectorTransform(tm, ray.dir);	
	
	float at;
	Point3 norm;
	
	// see if the ray intersects with the object
	if (pObject->IntersectRay(m_pCloth->m_pInterface->GetTime(), ray, at, norm)) 
	{
		pModData->hitPoint = ray.p + ray.dir * at;
		
		// create a bounding box around the intersection point
		Box3 boundingBox;
		boundingBox.Init();

		// move the bounding box to the hit point
		boundingBox += pModData->hitPoint;

		// expand it to the size set by the user
		float brushSize = m_pCloth->m_pBrushSizeSpinner->GetFVal();
		boundingBox.EnlargeBy(brushSize);
		
		switch (m_pCloth->m_selectionState)
		{
		case Cloth::SEL_CONNECTIONS:
			{
				int numConns = pModData->connections.Count();
				float closestDistance = 1000000.0f;
				int closestConnection = -1;

				// clear out the highlighted connections
				pModData->hilightedConnections.ZeroCount();
				pModData->hilightedConnections.Shrink();
		
				// step through all the connections, and see which are within the
				// radius of the brush. In addition, keep track of the closest
				// connection, so that if none are within the brush, the closest
				// connection is hilighted
				for (int i = 0; i < numConns; ++i)
				{
					float dist = SquaredDistanceToLine(pObject->GetPoint(pModData->connections[i].v1), pObject->GetPoint(pModData->connections[i].v2), pModData->hitPoint);
					
					// determine whether this connection is closest
					if (dist <= closestDistance && !m_pCloth->IsConnectionFiltered(pModData, i))
					{
						closestDistance = dist;
						closestConnection = i;
					}

					// test to see if this connection is within the brush radius
					if ((dist < brushSize * brushSize) && !m_pCloth->IsConnectionFiltered(pModData, i))
					{
						pModData->hilightedConnections.Append(1, &i);
					}
				}

				// add the closest connection if none were in the brush radius
				if (closestConnection != -1 && !pModData->hilightedConnections.Count())
				{
					pModData->hilightedConnections.Append(1, &closestConnection);
				}
			} break;
			
		case Cloth::SEL_LINK:
			{
				int numPoints = pObject->NumPoints();
				float closestDistance = 1000000.0f;
				int closestPoint = -1;

				// clear out the highlighted points
				pModData->hilightedPoints.ZeroCount();
				pModData->hilightedPoints.Shrink();
		
				// when linking, only one point can be selected, so find the
				// closest nonfiltered one
				for (int i = 0; i < numPoints; ++i)
				{
					float dist = LengthSquared(pObject->GetPoint(i) - pModData->hitPoint);
					
					if (dist < closestDistance && !m_pCloth->IsPointFiltered(pModData, i))
					{
						closestDistance = dist;
						closestPoint = i;
					}
				}

				if (closestPoint != -1)
					pModData->hilightedPoints.Append(1, &closestPoint);

			} break;

		case Cloth::SEL_POINTS:
			{
				int numPoints = pObject->NumPoints();
				float closestDistance = 1000000.0f;
				int closestPoint = -1;

				// clear out the highlighted points
				pModData->hilightedPoints.ZeroCount();
				pModData->hilightedPoints.Shrink();
		
				// step through all the points, and see which are within the box
				// in addition, keep track of the closest point, so that if the
				// box is empty, the closest point is hilighted
				for (int i = 0; i < numPoints; ++i)
				{
					// determine whether this point is closest
					float dist = LengthSquared(pObject->GetPoint(i) - pModData->hitPoint);
					
					if (dist < closestDistance && !m_pCloth->IsPointFiltered(pModData, i))
					{
						closestDistance = dist;
						closestPoint = i;
					}

					// test to see if this point is in the bounding box
					if (boundingBox.Contains(pObject->GetPoint(i)) &&
						 (dist < brushSize * brushSize) &&
						 !m_pCloth->IsPointFiltered(pModData, i))
					{
						pModData->hilightedPoints.Append(1, &i);
					}
				}

				// add the closest point if nothing else was found
				if (closestPoint !=-1 && !pModData->hilightedPoints.Count())
				{
					pModData->hilightedPoints.Append(1, &closestPoint);
				}
			} break;
		}
		
		return TRUE;
	}

	pModData->hilightedPoints.ZeroCount();
	pModData->hilightedPoints.Shrink();
	pModData->hilightedConnections.ZeroCount();
	pModData->hilightedConnections.Shrink();
	
	return FALSE;
}


int SelectMouseProc::proc(HWND hwnd, int msg, int point, int flags, IPoint2 mousePos)
{
	ViewExp *pViewport = pInterface->GetViewport(hwnd);   
	int res = TRUE;
	float st = 1.0f;
	int flip = 0;

	ModContextList mcList;
	INodeTab nodes;

	if (!m_pCloth->m_pInterface)
		return 0;

	m_pCloth->m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; k++) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;

		if (pModData)
		{
			ObjectState os = nodes[k]->EvalWorldState(m_pCloth->m_pInterface->GetTime());
			Object* pObject = os.obj;

			switch (msg)
			{
			case MOUSE_PROPCLICK:
			case MOUSE_ABORT:
				{
					pInterface->SetCommandMode(m_pCloth->m_pOldCmdMode);
					pInterface->DeleteMode(m_pCloth->m_pSelectMode);
					if (m_pCloth->m_selectionState != Cloth::SEL_NONE)
						theHold.Cancel();
					m_pCloth->m_selectionState = Cloth::SEL_NONE;
					m_pCloth->ClearSelected();
				} break;

			case MOUSE_POINT:
				{
					if (!pModData->isDragging && !(flags & MOUSE_SHIFT) && m_pCloth->m_selectionState != Cloth::SEL_LINK)
					{
						pModData->selectedPoints.ZeroCount();
						pModData->selectedPoints.Shrink();
						for (int curConn = 0; curConn < pModData->connections.Count(); ++curConn)
						{
							pModData->connections[curConn].selected = false;
						}
					}

					pModData->isDragging = FALSE;

					switch (m_pCloth->m_selectionState)
					{
					case Cloth::SEL_POINTS:
						{
							if (pModData->hilightedPoints.Count())
							{
								for (int curPoint = 0; curPoint < pModData->hilightedPoints.Count(); ++curPoint)
								{
									// determine whether or not the point was in the selection set
									int selIndex = FindPoint(pModData->selectedPoints, pModData->hilightedPoints[curPoint]);

									// remove the point if the ctrl key is down
									if (flags & MOUSE_CTRL)
									{
										if (selIndex != -1)
										{
											pModData->selectedPoints.Delete(selIndex, 1);
										}
									}
									// or select it otherwise
									else
									{
										if (selIndex == -1)
										{
											pModData->selectedPoints.Append(1, &pModData->hilightedPoints[curPoint]);
										}
									}
								}
							}
						} break;

					case Cloth::SEL_LINK:
						{
							if (pModData->hilightedPoints.Count())
							{
								m_pCloth->CreateConnections(pModData);
								m_pCloth->BeginSelect(ID_POINT_SEL);
							}
							else
							{
								m_pCloth->ClearSelected();
							}
						} break;

					case Cloth::SEL_CONNECTIONS:
						{
							for (int curConn = 0; curConn < pModData->hilightedConnections.Count(); ++curConn)
							{
								// deselect if the ctrl key is down
								if (flags & MOUSE_CTRL)
								{
									pModData->connections[pModData->hilightedConnections[curConn]].selected = false;
								}

								// otherwise, select
								else
								{
									pModData->connections[pModData->hilightedConnections[curConn]].selected = true;
								}
							}
						} break;
					}

					m_pCloth->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
					m_pCloth->m_pInterface->RedrawViews(m_pCloth->m_pInterface->GetTime());
					m_pCloth->DisplayProperties();
				} break;

			case MOUSE_MOVE:
			case MOUSE_FREEMOVE:
				{
					if (HitTest(pModData, pViewport, &mousePos, pObject, nodes[k]))
					{
						SetCursor(pInterface->GetSysCursor(SYSCUR_SELECT));
						pModData->isHit = TRUE;

						m_pCloth->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
						m_pCloth->m_pInterface->RedrawViews(m_pCloth->m_pInterface->GetTime());

						// see if the user is click dragging, and if so, update
						// the selection set
						if (MOUSE_MOVE == msg)
						{
							if (!pModData->isDragging)
							{
								pModData->isDragging = TRUE;

								if (!(flags & MOUSE_SHIFT))
								{
									pModData->selectedPoints.ZeroCount();
									pModData->selectedPoints.Shrink();
									for (int curConn = 0; curConn < pModData->connections.Count(); ++curConn)
									{
										pModData->connections[curConn].selected = false;
									}
								}
							}

							if (Cloth::SEL_POINTS == m_pCloth->m_selectionState)
							{
								for (int curPoint = 0; curPoint < pModData->hilightedPoints.Count(); ++curPoint)
								{
									// determine whether or not the point was in the selection set
									int selIndex = FindPoint(pModData->selectedPoints, pModData->hilightedPoints[curPoint]);

									// remove the point if the ctrl key is down
									if (flags & MOUSE_CTRL)
									{
										if (selIndex != -1)
										{
											pModData->selectedPoints.Delete(selIndex, 1);
										}
									}
									// or select it otherwise
									else
									{
										if (selIndex == -1)
										{
											pModData->selectedPoints.Append(1, &pModData->hilightedPoints[curPoint]);
										}
									}
								}
							}
							else if (Cloth::SEL_CONNECTIONS == m_pCloth->m_selectionState)
							{
								for (int curConn = 0; curConn < pModData->hilightedConnections.Count(); ++curConn)
								{
									// deselect if the ctrl key is down
									if (flags & MOUSE_CTRL)
									{
										pModData->connections[pModData->hilightedConnections[curConn]].selected = false;
									}

									// otherwise, select
									else
									{
										pModData->connections[pModData->hilightedConnections[curConn]].selected = true;
									}
								}
							}
						}
					}
					else
					{
						SetCursor(LoadCursor(NULL,IDC_ARROW));

						if (pModData->isHit)
						{
							pModData->isHit = FALSE;
							m_pCloth->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
							m_pCloth->m_pInterface->RedrawViews(m_pCloth->m_pInterface->GetTime());
						}
					}
				} break;
			} // end switch
		} // end if
	} // end for
	
	if (pViewport)
		pInterface->ReleaseViewport(pViewport);

	return res;
}



