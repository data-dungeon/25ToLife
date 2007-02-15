#include <string.h>
#include "stdafx.h"
#include "windows.h"
#include "resource.h"
#include "StripChart.h"
#include "strip.h"

static int initialized = 0;

#define MIN_CLIENT_HEIGHT       40
#define MIN_CLIENT_WIDTH        40
#define DEFAULT_LEFT_MARGIN     30
#define DEFAULT_RIGHT_MARGIN    15
#define DEFAULT_BOTTOM_MARGIN   25
#define DEFAULT_TOP_MARGIN      15
#define DEFAULT_CAPTION_HEIGHT  15
#define TITLE_HEIGHT_OFFSET      9
#define X_LABEL_HEIGHT_OFFSET   10
#define Y_LABEL_HEIGHT_OFFSET    4

#define NUM_LINE_COLORS    11
static int lineColor[NUM_LINE_COLORS][3] =
{
    0xff, 0x00, 0x00,    // Red
    0x00, 0xff, 0x00,    // Green
    0x00, 0x00, 0xff,    // Blue
    0xe7, 0xe7, 0x00,    // Yellow
    0xff, 0x00, 0xff,    // Magenta
    0x7f, 0x00, 0x00,    // Dark Red
    0x00, 0x7f, 0x00,    // Dark Green
    0x00, 0x00, 0x7f,    // Dark Blue
    0x7f, 0x7f, 0x00,    // Olive Green
    0x7f, 0x00, 0x7f,    // Purple
    0x00, 0x7f, 0x7f     // Teal
};

static void SetTrackLimits (TStripChart* sc, short track);
static int GetCaptionID (TStripChart* sc, int captionFlag);
static int SearchForSegmentEnd (int numPoints, float* xArray, int start,
        bool closed);
static bool DrawCurveSegment (TStripChart* sc, int numPoints,
        float* xArray, float* yArray, int minIndex, int maxIndex, int color);
static bool DrawPoint (TStripChart* sc, float* xArray, float* yArray,
        int index, int color);

SC_EXPORT void* InitStripChartWindow (HWND hWndParent)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CWnd* parent;

    if (!initialized)
    {
        initialized = 1;
        AfxEnableControlContainer ();
    }
    OleInitialize (NULL);

    if ((parent = new CWnd) == 0)
        return (0);

    parent->Attach (hWndParent);
    return ((void*) parent);
}

SC_EXPORT void UnInitStripChartWindow (void* lpvParent)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CWnd* parent = (CWnd*) (lpvParent);
    parent->Detach ();
    delete parent;
    OleUninitialize ();
}

SC_EXPORT HWND CreateStripChart (TStripChart *sc, LPCTSTR lpszWindowName,
        DWORD dwStyle, int x, int y, int nWidth, int nHeight, int captionFlag,
        void* lpvParent)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CWnd* parent = (CWnd*)lpvParent;
    CStrip* strip;
    CString strLicenseKey = "Copyright (c) 1997 ";
    BSTR bstrLicKey = strLicenseKey.AllocSysString();

    RECT rect;
    int i;

    if ((parent == 0) || (sc->pchart != 0))
        return (0);

    if ((strip = new CStrip) == 0)
        return (0);

    rect.left = x;
    rect.top = y;
    rect.right = x + nWidth;
    rect.bottom = y + nHeight;
    sc->numTracks = 1;
    sc->tracksChanged = FALSE;
    strip->Create (lpszWindowName, dwStyle, rect, parent, IDC_STRIP, NULL,
            FALSE, bstrLicKey);
    strip->SetBackColor (GetSysColor (COLOR_BTNFACE));
    strip->SetBevelWidth (1);
    strip->SetTracks (1);
    strip->SetTrackID (0);
    strip->SetTrackTicLabelAlign (1);
    strip->SetTrackBackColor (RGB (0xff, 0xff, 0xff));
    strip->SetGridColor (RGB (0xb0, 0xb0, 0xb0));
    strip->SetVariables (sc->numVariables);
    int colorIndex;
    for (i = 0; i < sc->numVariables; i++)
    {
        colorIndex = i % NUM_LINE_COLORS;
        sc->variables[i].color = RGB (lineColor[colorIndex][0],
                lineColor[colorIndex][1], lineColor[colorIndex][2]);
        sc->variables[i].visible = FALSE;
        sc->variables[i].trackID = -1;
        sc->variables[i].changed = FALSE;
        strip->SetVariableID (i);
        strip->SetVariableDeltaX (0.0);
        strip->SetVariableTrackID (0);
        strip->SetVariableVisible (FALSE);
        strip->SetVariableColor (sc->variables[i].color);
    }
    strip->SetVariableID (0);
    strip->SetMaxBufferSize (sc->bufferSize);
    strip->SetHandles (3);
    strip->SetXSpan (0.1);
    strip->SetCursorMode (0);
    strip->SetStampColor (RGB (0xff, 0x0, 0x0));
    strip->SetStampMode (0);
    strip->SetStampSymbol (4);
    strip->SetStampSymbolSize (0.02);

    int borderWidth = 0;
    if (strip->GetBevelInner())
        borderWidth += strip->GetBevelWidth();
    if (strip->GetBevelOuter())
        borderWidth += strip->GetBevelWidth();
    borderWidth += strip->GetBorderWidth();
    double areaX = (double) (nWidth - 2 * borderWidth);
    double areaY = (double) (nHeight - 2 * borderWidth);

    int numCaptions = 0;
    if (captionFlag & STRIPCHART_Y_LABEL)
    {
        strip->SetAreaLeft((double)(DEFAULT_LEFT_MARGIN +
                DEFAULT_CAPTION_HEIGHT) / areaX);
        numCaptions++;
    }
    else
        strip->SetAreaLeft((double)DEFAULT_LEFT_MARGIN / areaX);

    strip->SetAreaRight(1.0 - ((double)DEFAULT_RIGHT_MARGIN / areaX));

    if (captionFlag & STRIPCHART_TITLE)
    {
        strip->SetAreaTop((double)(DEFAULT_TOP_MARGIN +
                DEFAULT_CAPTION_HEIGHT) / areaY);
        numCaptions++;
    }
    else
        strip->SetAreaTop((double)DEFAULT_TOP_MARGIN / areaY);

    if (captionFlag & STRIPCHART_X_LABEL)
    {
        strip->SetAreaBottom(1.0 - ((double)(DEFAULT_BOTTOM_MARGIN +
                DEFAULT_CAPTION_HEIGHT) / areaY));
        numCaptions++;
    }
    else
        strip->SetAreaBottom(1.0 - ((double)DEFAULT_BOTTOM_MARGIN / areaY));

    strip->SetCaptions(numCaptions);
    sc->captionMask = captionFlag;

    if ((i = GetCaptionID(sc, STRIPCHART_TITLE)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaption("");
        strip->SetCaptionAlign(2);
        strip->SetCaptionOrientation(0);
        strip->SetCaptionX(0.5);
        strip->SetCaptionY((double)TITLE_HEIGHT_OFFSET / areaY);
    }
    if ((i = GetCaptionID(sc, STRIPCHART_X_LABEL)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaption("");
        strip->SetCaptionAlign(2);
        strip->SetCaptionOrientation(0);
        strip->SetCaptionX(0.5 * (strip->GetAreaLeft() +
                strip->GetAreaRight()));
        strip->SetCaptionY(1.0 - ((double)X_LABEL_HEIGHT_OFFSET / areaY));
    }
    if ((i = GetCaptionID(sc, STRIPCHART_Y_LABEL)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaption("");
        strip->SetCaptionAlign(2);
        strip->SetCaptionOrientation(1);
        strip->SetCaptionX((double)Y_LABEL_HEIGHT_OFFSET / areaX);
        strip->SetCaptionY(0.5 * (strip->GetAreaTop() +
                strip->GetAreaBottom()));
    }

    sc->pchart = (void*) strip;
    SetWindowLong (strip->m_hWnd, GWL_USERDATA, (LONG) sc);

    return (strip->m_hWnd);
}

SC_EXPORT void DestroyStripChart (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    delete (CStrip*) (sc->pchart);
    sc->pchart = 0;
}

SC_EXPORT void StripChartTogglePause (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    if (((CStrip*) (sc->pchart))->GetPause ())
        ((CStrip*) (sc->pchart))->SetPause (FALSE);
    else
        ((CStrip*) (sc->pchart))->SetPause (TRUE);
}

SC_EXPORT void StripChartProperties (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    ((CStrip*) (sc->pchart))->ShowPropertyPage ();
}

SC_EXPORT void StripChartX (TStripChart* sc, float value)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    COLORREF newColor;
    BOOL newVisible;
    short newTrack;

    CStrip* strip = (CStrip*) (sc->pchart);
    short id = strip->GetVariableID ();
    short track = strip->GetTrackID ();
    int i;
    for (i = 0; i < sc->numVariables; i++)
    {
        strip->SetVariableID (i);
        strip->SetVariableLastX ((double) value);
        if ((newColor = strip->GetVariableColor ()) != sc->variables[i].color)
        {
            sc->variables[i].color = newColor;
            sc->variables[i].changed = TRUE;
        }
        if ((newVisible = strip->GetVariableVisible ()) !=
                sc->variables[i].visible)
        {
            sc->variables[i].visible = newVisible;
            sc->variables[i].changed = TRUE;
        }
        if (((newTrack = strip->GetVariableTrackID ()) !=
                sc->variables[i].trackID) && (sc->variables[i].visible))
        {
            SetTrackLimits (sc, newTrack);
            sc->variables[i].trackID = newTrack;
            sc->variables[i].changed = TRUE;
        }
    }
    if ((newTrack = strip->GetTracks ()) != sc->numTracks)
    {
        if (newTrack > sc->numTracks)
        {
            for (i = sc->numTracks; i < newTrack; i++)
            {
                strip->SetTrackID (i);
                strip->SetTrackTicLabelAlign (1);
            }
        }
        sc->numTracks = newTrack;
        sc->tracksChanged = TRUE;
    }
    strip->SetVariableID (id);
    strip->SetTrackID (track);
}

SC_EXPORT void StripChartY (TStripChart* sc, short index, float value)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    ((CStrip*) (sc->pchart))->AddY (index, (double) value);
    sc->variables[index].value = (double) value;
}

SC_EXPORT void StripChartMove (TStripChart* sc, int x, int y, int nWidth,
        int nHeight)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    RECT rect;
    CStrip* strip = (CStrip*) (sc->pchart);

    strip->GetClientRect (&rect);
    double widthRatio = (double) (rect.right);
    double heightRatio = (double) (rect.bottom);
    double areaLeft = strip->GetAreaLeft ();
    double areaRight = strip->GetAreaRight ();
    double areaTop = strip->GetAreaTop ();
    double areaBottom = strip->GetAreaBottom ();
    double trackSeparation = strip->GetTrackSeparation ();
    strip->MoveWindow (x, y, nWidth, nHeight, TRUE);
    strip->GetClientRect (&rect);
    widthRatio /= (double) (rect.right);
    heightRatio /= (double) (rect.bottom);
    strip->SetAreaLeft (areaLeft * widthRatio);
    strip->SetAreaRight (((areaRight - 1.0) * widthRatio) + 1.0);
    strip->SetAreaTop (areaTop * heightRatio);
    strip->SetAreaBottom (((areaBottom - 1.0) * heightRatio) + 1.0);
    strip->SetTrackSeparation (trackSeparation * heightRatio);

    int borderWidth = 0;
    if (strip->GetBevelInner())
        borderWidth += strip->GetBevelWidth();
    if (strip->GetBevelOuter())
        borderWidth += strip->GetBevelWidth();
    borderWidth += strip->GetBorderWidth();
    double areaX = (double) (nWidth - 2 * borderWidth);
    double areaY = (double) (nHeight - 2 * borderWidth);
    int i;
    if ((i = GetCaptionID(sc, STRIPCHART_TITLE)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaptionY((double)TITLE_HEIGHT_OFFSET / areaY);
    }
    if ((i = GetCaptionID(sc, STRIPCHART_X_LABEL)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaptionX(0.5 * (strip->GetAreaLeft() +
                strip->GetAreaRight()));
        strip->SetCaptionY(1.0 - ((double)X_LABEL_HEIGHT_OFFSET / areaY));
    }
    if ((i = GetCaptionID(sc, STRIPCHART_Y_LABEL)) >= 0)
    {
        strip->SetCaptionID(i);
        strip->SetCaptionX((double)Y_LABEL_HEIGHT_OFFSET / areaX);
        strip->SetCaptionY(0.5 * (strip->GetAreaTop() +
                strip->GetAreaBottom()));
    }
}

SC_EXPORT int StripChartMinHeight (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    RECT rect;
    CStrip* strip = (CStrip*) (sc->pchart);

    strip->GetClientRect (&rect);
    return (MIN_CLIENT_HEIGHT + (int) ((1.0 + strip->GetAreaTop () -
            strip->GetAreaBottom ()) * (double) rect.bottom));
}

SC_EXPORT int StripChartMinWidth (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    RECT rect;
    CStrip* strip = (CStrip*) (sc->pchart);

    strip->GetClientRect (&rect);
    return (MIN_CLIENT_WIDTH + (int) ((1.0 + strip->GetAreaLeft () -
            strip->GetAreaRight ()) * (double) rect.right));
}

SC_EXPORT void StripChartToggleVariable (TStripChart* sc, int id, int track)
{
    CStrip* strip = (CStrip*) (sc->pchart);

    if ((id < 0) || (id >= strip->GetVariables ()))
        return;

    short currID = strip->GetVariableID ();
    strip->SetVariableID (id);

    if (strip->GetVariableVisible ())
        strip->SetVariableVisible (FALSE);
    else if ((track < 0) || (track >= strip->GetTracks ()))
        return;
    else
    {
        strip->SetVariableTrackID (track);
        strip->SetVariableVisible (TRUE);
    }

    strip->SetVariableID (currID);
}

SC_EXPORT void StripChartStamp (TStripChart* sc)
{
    CStrip* strip = (CStrip*) (sc->pchart);

    strip->AddSymbol (strip->GetAreaTop ());
}

SC_EXPORT void InitStripChart (TStripChart* sc)
{
    sc->pchart = 0;
    sc->xLabel = 0;
    sc->xUnits = 0;
    sc->numVariables = 0;
    sc->variables = 0;
    sc->numTracks = 1;
    sc->noHist = true;
    sc->xScale = 1.0f;
    sc->yScale = 1.0f;
}

SC_EXPORT void UnInitStripChart (TStripChart* sc)
{
    if (sc->pchart)
        return;

    sc->xLabel = 0;
    sc->xUnits = 0;

    if (sc->variables)
        delete[] sc->variables;
    sc->numVariables = 0;
    sc->variables = 0;
    sc->noHist = true;
}

SC_EXPORT void SetStripChartX (TStripChart* sc, char* xl, char* xu, int nv,
        int bufSize)
{
    int i;

    sc->xLabel = xl;
    sc->xUnits = xu;
    sc->bufferSize = bufSize;

    if (sc->variables)
        delete[] sc->variables;
    if (nv > 0)
    {
        sc->numVariables = nv;
        sc->variables = new TStripChartVariable[sc->numVariables];
        for (i = 0; i < sc->numVariables; i++)
        {
            sc->variables[i].label = 0;
            sc->variables[i].units = 0;
            sc->variables[i].color = 0;
            sc->variables[i].trackID = -1;
            sc->variables[i].value = 0.0;
            sc->variables[i].expectedMin = -10.0;
            sc->variables[i].expectedMax = 10.0;
            sc->variables[i].changed = FALSE;
        }
    }
    else
    {
        sc->numVariables = 0;
        sc->variables = 0;
    }
}

SC_EXPORT void AddStripChartVariable (TStripChart* sc, int index, char* vl,
        char* vu, double vMin, double vMax)
{
    if ((index < 0) || (index >= sc->numVariables))
        return;

    sc->variables[index].label = vl;
    sc->variables[index].units = vu;
    if (vMax >= vMin)
    {
        sc->variables[index].expectedMin = vMin;
        sc->variables[index].expectedMax = vMax;
    }
    else
    {
        sc->variables[index].expectedMin = vMax;
        sc->variables[index].expectedMax = vMin;
    }
}

static void SetTrackLimits (TStripChart* sc, short track)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CStrip* strip = (CStrip*) (sc->pchart);
    BOOL found = FALSE;
    double minVal, maxVal;

    short id = strip->GetVariableID ();
    for (int i = 0; i < sc->numVariables; i++)
    {
        strip->SetVariableID (i);
        if (strip->GetVariableVisible () &&
                (track == strip->GetVariableTrackID ()))
        {
            if (!found)
            {
                found = TRUE;
                minVal = sc->variables[i].expectedMin;
                maxVal = sc->variables[i].expectedMax;
            }
            else
            {
                if (sc->variables[i].expectedMin < minVal)
                    minVal = sc->variables[i].expectedMin;
                if (sc->variables[i].expectedMax > maxVal)
                    maxVal = sc->variables[i].expectedMax;
            }
        }
    }
    strip->SetVariableID (id);

    if (found)
    {
        short id = strip->GetTrackID ();
        strip->SetTrackID (track);
        strip->SetTrackDisplayMin (minVal);
        strip->SetTrackDisplayMax (maxVal);
        strip->SetTrackMin (minVal);
        strip->SetTrackMax (maxVal);
        strip->SetTrackID (id);
    }
}

static int GetCaptionID (TStripChart* sc, int captionFlag)
{
    captionFlag = captionFlag & (STRIPCHART_TITLE | STRIPCHART_X_LABEL |
            STRIPCHART_Y_LABEL);
    if (!(captionFlag & sc->captionMask) || ((captionFlag != STRIPCHART_TITLE)
            && (captionFlag != STRIPCHART_X_LABEL)
            && (captionFlag != STRIPCHART_Y_LABEL)))
        return (-1);

    int id = 0;
    if (captionFlag == STRIPCHART_TITLE)
        return (id);
    if (sc->captionMask & STRIPCHART_TITLE)
        id++;
    if (captionFlag == STRIPCHART_X_LABEL)
        return (id);
    if (sc->captionMask & STRIPCHART_X_LABEL)
        id++;
    return (id);
}

SC_EXPORT void SetStripChartCaption (TStripChart* sc, int captionFlag,
        char* caption)
{
    int captionID = GetCaptionID(sc, captionFlag);
    if (captionID < 0)
        return;

    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CStrip* strip = (CStrip*) (sc->pchart);
    strip->SetCaptionID(captionID);
    strip->SetCaption(caption);
}

SC_EXPORT void StripChartClear (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CStrip* strip = (CStrip*) (sc->pchart);
    strip->ClearAll();
    sc->currentVariable = 0;
    sc->pointsInVariable = 0;
    for (int i = 0; i < sc->numVariables; i++)
    {
        strip->SetVariableID (i);
        strip->SetVariableTrackID (0);
        strip->SetVariableVisible (FALSE);
    }
}

SC_EXPORT void StripChartScale (TStripChart* sc, float xScale, float yScale)
{
    sc->xScale = xScale;
    sc->yScale = yScale;
}

SC_EXPORT bool StripChartXYArray (TStripChart* sc, int numPoints,
        float* xArray, float* yArray, int color, bool closed)
{
    if (numPoints <= 0)
        return(true);

    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    if (sc->currentVariable == 0)
    {
        sc->minX = sc->xScale * xArray[0];
        sc->maxX = sc->minX;
        sc->minY = sc->yScale * yArray[0];
        sc->maxY = sc->minY;
    }

    if (numPoints <= 2)
    {
        return (DrawCurveSegment (sc, numPoints, xArray, yArray, 0,
                numPoints - 1, color));
    }

    int crvStart;
    if (closed)
        crvStart = SearchForSegmentEnd (numPoints, xArray, 0, closed);
    else
        crvStart = 0;

    int segStart, segEnd;
    segStart = crvStart;
    do
    {
        segEnd = SearchForSegmentEnd (numPoints, xArray, segStart, closed);
        if (!DrawCurveSegment (sc, numPoints, xArray, yArray, segStart,
                segEnd, color))
            return (false);
        if (!closed && (segEnd == (numPoints - 1)))
            break;
        segStart = segEnd;
    } while (segEnd != crvStart);
    return (true);
}

static int SearchForSegmentEnd (int numPoints, float* xArray, int start,
        bool closed)
{
    int end = start + 1;
    if (end >= numPoints)
    {
        if (closed)
            end = 0;
        else
            return (start);
    }

    if (xArray[end] > xArray[start])
    {
        for (; end < numPoints - 1; end++)
        {
            if (xArray[end + 1] <= xArray[end])
                break;
        }
        if ((end == (numPoints - 1)) && closed && (xArray[0] > xArray[end]))
        {
            for (end = 0; end < start - 1; end++)
            {
                if (xArray[end + 1] <= xArray[end])
                    break;
            }
        }
    }
    else if (xArray[end] < xArray[start])
    {
        for (; end < numPoints - 1; end++)
        {
            if (xArray[end + 1] >= xArray[end])
                break;
        }
        if ((end == (numPoints - 1)) && closed && (xArray[0] < xArray[end]))
        {
            for (end = 0; end < start - 1; end++)
            {
                if (xArray[end + 1] >= xArray[end])
                    break;
            }
        }
    }
    else
    {
        for (; end < numPoints - 1; end++)
        {
            if (xArray[end + 1] != xArray[end])
                break;
        }
        if ((end == (numPoints - 1)) && closed && (xArray[0] == xArray[end]))
        {
            for (end = 0; end < start - 1; end++)
            {
                if (xArray[end + 1] != xArray[end])
                    break;
            }
        }
    }
    return (end);
}

static bool DrawCurveSegment (TStripChart* sc, int numPoints,
        float* xArray, float* yArray, int minIndex, int maxIndex, int color)
{
    if ((sc->currentVariable >= sc->numVariables) ||
            (sc->bufferSize <= 0))
        return (false);
/*
 *  Only one point in the segment.
 */
    if (minIndex == maxIndex)
    {
        if (!DrawPoint (sc, xArray, yArray, minIndex, color))
            return(false);
    }
/*
 *  The segment is stored left to right.
 */
    else if (xArray[minIndex] < xArray[maxIndex])
    {
        if (minIndex > maxIndex)
        {
            for (; minIndex < numPoints; minIndex++)
            {
                if (!DrawPoint (sc, xArray, yArray, minIndex, color))
                    return(false);
            }
            minIndex = 0;
        }
        for (; minIndex <= maxIndex; minIndex++)
        {
            if (!DrawPoint (sc, xArray, yArray, minIndex, color))
                return(false);
        }
    }
/*
 *  The segment is stored right to left.
 */
    else if (xArray[minIndex] > xArray[maxIndex])
    {
        if (minIndex > maxIndex)
        {
            for (; maxIndex >= 0; maxIndex--)
            {
                if (!DrawPoint (sc, xArray, yArray, maxIndex, color))
                    return(false);
            }
            maxIndex = numPoints - 1;
        }
        for (; maxIndex >= minIndex; maxIndex--)
        {
            if (!DrawPoint (sc, xArray, yArray, maxIndex, color))
                return(false);
        }
    }
/*
 *  All of the points lie on a vertical line.
 */
    else
    {
        int minVal = minIndex;
        int maxVal = minIndex;
        minIndex++;
        if (minIndex > maxIndex)
        {
            for (; minIndex < numPoints; minIndex++)
            {
                if (yArray[minIndex] < yArray[minVal])
                    minVal = minIndex;
                if (yArray[minIndex] > yArray[maxVal])
                    maxVal = minIndex;
            }
            minIndex = 0;
        }
        for (; minIndex <= maxIndex; minIndex++)
        {
            if (yArray[minIndex] < yArray[minVal])
                minVal = minIndex;
            if (yArray[minIndex] > yArray[maxVal])
                maxVal = minIndex;
        }
        if (!DrawPoint (sc, xArray, yArray, minVal, color) ||
                !DrawPoint (sc, xArray, yArray, maxVal, color))
            return(false);
    }
/*
 *  Get ready for the next segment.
 */
    sc->currentVariable++;
    sc->pointsInVariable = 0;

    return(true);
}

static bool DrawPoint (TStripChart* sc, float* xArray, float* yArray,
        int index, int color)
{
    CStrip* strip = (CStrip*) (sc->pchart);

    if (sc->pointsInVariable == 0)
    {
        strip->SetVariableID (sc->currentVariable);
        strip->SetVariableVisible (TRUE);
        strip->SetVariableColor (RGB (lineColor[color][0], lineColor[color][1],
                lineColor[color][2]));
    }
    if (sc->pointsInVariable >= sc->bufferSize)
    {
        sc->currentVariable++;
        if (sc->currentVariable >= sc->numVariables)
            return(false);
        strip->SetVariableID (sc->currentVariable);
        strip->SetVariableVisible (TRUE);
        strip->SetVariableColor (RGB (lineColor[color][0], lineColor[color][1],
                lineColor[color][2]));
        strip->AddXY(sc->currentVariable, sc->lastX, sc->lastY);
        sc->pointsInVariable = 1;
    }

    sc->lastX = sc->xScale * xArray[index];
    sc->lastY = sc->yScale * yArray[index];
    strip->AddXY(sc->currentVariable, sc->lastX, sc->lastY);
    sc->pointsInVariable++;

    if (sc->lastX < sc->minX)
        sc->minX = sc->lastX;
    if (sc->lastX > sc->maxX)
        sc->maxX = sc->lastX;
    if (sc->lastY < sc->minY)
        sc->minY = sc->lastY;
    if (sc->lastY > sc->maxY)
        sc->maxY = sc->lastY;

    return(true);
}

SC_EXPORT void StripChartFitExtents (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CStrip* strip = (CStrip*) (sc->pchart);

    strip->SetTrackID(0);
    strip->SetXSpan(sc->maxX - sc->minX);
    sc->histMinY = sc->minY;
    sc->histMaxY = sc->maxY;
    strip->SetTrackMin(sc->minY);
    strip->SetTrackMax(sc->maxY);
    strip->SetTrackDisplayMin(sc->minY);
    strip->SetTrackDisplayMax(sc->maxY);
    sc->noHist = false;
}

SC_EXPORT void StripChartGrowExtents (TStripChart* sc)
{
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());

    CStrip* strip = (CStrip*) (sc->pchart);

    strip->SetXSpan(sc->maxX - sc->minX);
    strip->SetTrackID(0);
    if (sc->noHist)
    {
        sc->histMinY = sc->minY;
        sc->histMaxY = sc->maxY;
        strip->SetTrackMin(sc->minY);
        strip->SetTrackMax(sc->maxY);
        strip->SetTrackDisplayMin(sc->minY);
        strip->SetTrackDisplayMax(sc->maxY);
        sc->noHist = false;
    }
    else
    {
        if (sc->minY < sc->histMinY)
        {
            sc->histMinY = sc->minY;
            strip->SetTrackMin(sc->minY);
        }
        if (sc->maxY > sc->histMaxY)
        {
            sc->histMaxY = sc->maxY;
            strip->SetTrackMax(sc->maxY);
        }
    }
}
