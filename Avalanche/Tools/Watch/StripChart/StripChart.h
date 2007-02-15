#ifndef _STRIPCHART_H_
#define _STRIPCHART_H_

#include "windows.h"

#define SC_EXPORT extern "C" __declspec (dllexport)

#define STRIPCHART_TITLE    0x1
#define STRIPCHART_X_LABEL  0x2
#define STRIPCHART_Y_LABEL  0x4

typedef struct
{
    char* label;
    char* units;
    COLORREF color;
    BOOL visible;
    short trackID;
    double value;
    double expectedMin;
    double expectedMax;
    BOOL changed;
} TStripChartVariable;

typedef struct
{
    void* pchart;
    char* xLabel;
    char* xUnits;
    int numVariables;
    int bufferSize;
    TStripChartVariable* variables;
    short numTracks;
    int captionMask;
    BOOL tracksChanged;
    int currentVariable;
    int pointsInVariable;
    float lastX;
    float lastY;
    float minX;
    float maxX;
    float minY;
    float maxY;
    float histMinY;
    float histMaxY;
    float xScale;
    float yScale;
    bool noHist;
} TStripChart;

SC_EXPORT void* InitStripChartWindow (HWND hWndParent);
SC_EXPORT void UnInitStripChartWindow (void* lpvParent);
SC_EXPORT HWND CreateStripChart (TStripChart *sc, LPCTSTR lpszWindowName,
        DWORD dwStyle, int x, int y, int nWidth, int nHeight, int captionFlag,
        void* lpvParent);
SC_EXPORT void DestroyStripChart (TStripChart* sc);
SC_EXPORT void StripChartTogglePause (TStripChart* sc);
SC_EXPORT void StripChartProperties (TStripChart* sc);
SC_EXPORT void StripChartX (TStripChart* sc, float value);
SC_EXPORT void StripChartY (TStripChart* sc, short index, float value);
SC_EXPORT void StripChartMove (TStripChart* sc, int x, int y, int nWidth,
        int nHeight);
SC_EXPORT int StripChartMinHeight (TStripChart* sc);
SC_EXPORT int StripChartMinWidth (TStripChart* sc);
SC_EXPORT void StripChartToggleVariable (TStripChart* sc, int id, int track);
SC_EXPORT void StripChartStamp (TStripChart* sc);

SC_EXPORT void InitStripChart (TStripChart* sc);
SC_EXPORT void UnInitStripChart (TStripChart* sc);
SC_EXPORT void SetStripChartX (TStripChart* sc, char* xl, char* xu, int nv,
        int bufSize);
SC_EXPORT void AddStripChartVariable (TStripChart* sc, int index, char* vl,
        char* vu, double vMin, double vMax);
SC_EXPORT void SetStripChartCaption (TStripChart* sc, int captionFlag,
        char* caption);
SC_EXPORT void StripChartClear (TStripChart* sc);
SC_EXPORT void StripChartScale (TStripChart* sc, float xScale, float yScale);
SC_EXPORT bool StripChartXYArray (TStripChart* sc, int numPoints,
        float* xArray, float* yArray, int color, bool closed);
SC_EXPORT void StripChartFitExtents (TStripChart* sc);
SC_EXPORT void StripChartGrowExtents (TStripChart* sc);

#endif // _STRIPCHART_H_
