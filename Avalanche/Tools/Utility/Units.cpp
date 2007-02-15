//

#include "stdafx.h"
#include "AString.h"
#include "ATime.h"
#include <AFXPRIV.h>

static int f_measurementunits = UNIT_CENTIMETERS;

UTILITYEXPORT int GetMeasurementUnits()
{
   return f_measurementunits;
}

UTILITYEXPORT void SetMeasurementUnits( int pmeasurementunits )
{
   f_measurementunits = pmeasurementunits;
}

UTILITYEXPORT CString UnitToString( float cm, int pmeasurementunits/*=-1*/, float modulo /*=.01*/ )
{
   if (pmeasurementunits == -1)
      pmeasurementunits = f_measurementunits;

   CString string;
   switch (pmeasurementunits)
   {
      default:
      case UNIT_NONE:
      {
         if (modulo)
            Round( cm, modulo );
         string.Format( "%g", cm );
         break;
      }
      case UNIT_CENTIMETERS:
      {
         if (modulo)
            Round( cm, modulo );
         string.Format( "%gcm", cm );
         break;
      }
      case UNIT_METERS:
      {
         float meters = cm * .01f;
         if (modulo)
            Round( meters, modulo );
         string.Format( "%gm", meters );
         break;
      }
      case UNIT_FEET:
      {
         float feet = (float) (cm * .03280839895013);
         if (modulo)
            Round( feet, modulo );
         string.Format( "%g'", feet );
         break;
      }
      case UNIT_INCHES:
      {
         float inches = (float) (cm * 0.3937007874016);
         if (modulo)
            Round( inches, modulo );
         string.Format( "%g\"", inches );
         break;
      }
      case UNIT_FEETANDINCHES:
      {
         cm += SIGN(cm)*.005f;
         int feet = fast_ftol_signed((float) (cm * .03280839895013));
         float inches = (float) (fmod((double)cm, 30.48) * 0.3937007874016);
         if (modulo)
            Round( inches, modulo );
         if (feet)
            if (inches)
               string.Format( "%d'%g\"", feet, fabs(inches));
            else
               string.Format( "%d'", feet );
         else
            string.Format( "%g\"", inches );
         break;
      }
      case UNIT_PERCENTAGE:
      {
         if (modulo)
            Round( cm, modulo );
         string.Format( "%g%%", cm );
         break;
      }
      case UNIT_ANGLE:
      {
         if (modulo)
            Round( cm, modulo );
         string.Format( "%g\x7F", cm );//°", cm );
         break;
      }
   }

   return string;
}

static float ToCM( float value, char unitchar )
{
   switch (unitchar) {
      case '\'': // FEET
         return value * 30.48f;
      case '\"': // INCHES
         return value * 2.54f;
      case 'm':
      case 'M':  // Meters
         return value * 100.0f;
      case 'c':
      case 'C':  // Centemeters
      default:
         return value;
   }
}

UTILITYEXPORT float String::GetCM() const
{
   char defaultchar, c;

   switch (f_measurementunits) {
      case UNIT_FEET:
         defaultchar = '\'';
         break;
      case UNIT_FEETANDINCHES:
      case UNIT_INCHES:
         defaultchar = '\"';
         break;
      case UNIT_CENTIMETERS:
         defaultchar = 'c';
         break;
      case UNIT_METERS:
         defaultchar = 'm';
         break;
   }

   float value, finalvalue = 0.0f, sign = 1.0f;;
   enum {LCT_MISC, LCT_DIGIT, LCT_SPACE};
   int i=0, len=Length(), lastchartype=LCT_MISC;
   String numstring;

   for (; i<=len; i++) {
      if (i<len)
         c = m_pData[i];
      else
         c = defaultchar;
      switch (c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
         if (lastchartype==LCT_SPACE && !numstring.IsEmpty()) {
            value = (float) atof(numstring.Get());
            numstring.Empty();
            finalvalue += ToCM( value, defaultchar );
         }
         numstring += c;
         lastchartype = LCT_DIGIT;
         break;
      case '-':
         if (finalvalue == 0)
            sign = -1.0f;
         lastchartype = LCT_DIGIT;
         break;
      case ' ':
         lastchartype = LCT_SPACE;
         break;
      default:
         if (!numstring.IsEmpty()) {
            value = (float) atof(numstring.Get());
            numstring.Empty();
            finalvalue += ToCM( value, c );
         }
         lastchartype = LCT_MISC;
      }
   }
   return finalvalue * sign;
}


UTILITYEXPORT void AFXAPI DDX_Unit(CDataExchange *pDX, int nIDC, float &cm)
{
   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
   if (pDX->m_bSaveAndValidate) {
      TCHAR szBuffer[32];
      ::GetWindowText(hWndCtrl, szBuffer, 32);
      String scm = szBuffer;
      cm = scm.GetCM();
   }
   else {
      CString string = UnitToString( cm );
      AfxSetWindowText(hWndCtrl, string);
   }
}

UTILITYEXPORT void AFXAPI DDV_MinMaxUnit(CDataExchange* pDX, float const& value, float minVal, float maxVal)
{
   DDV_MinMaxFloat(pDX, value, minVal, maxVal);
}
