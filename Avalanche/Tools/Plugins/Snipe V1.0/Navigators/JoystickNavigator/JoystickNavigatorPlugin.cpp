//

#include "stdafx.h"
#include "Resource.h"
#include "HCGL3DView.h"
#include "Vector2.h"
#include "HFloatProperty.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HCamera.h"
#include "Matrix44.h"
#include "BoundingBox.h"
#include "HSnipeObjectList.h"
#include "HGLImageList.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "JoystickNavigatorPlugin.h"

static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance, void *pContext )
{
   JoystickNavigatorPlugin *pJoystickNavigatorPlugin = (JoystickNavigatorPlugin *)pContext;
   return pJoystickNavigatorPlugin->EnumJoysticksCallback( pdidInstance );
}

static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE *pdidoi, void *pContext )
{
   JoystickNavigatorPlugin *pJoystickNavigatorPlugin = (JoystickNavigatorPlugin *)pContext;
   return pJoystickNavigatorPlugin->EnumObjectsCallback( pdidoi );
}

static void CALLBACK EXPORT MyTimerProc( HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   JoystickNavigator *pJoystickNavigator = (JoystickNavigator *)nIDEvent;
   pJoystickNavigator->OnTimer();
}

// Plugin ////////////////////////////////////////////////

JoystickNavigatorPlugin::JoystickNavigatorPlugin()
{
   m_pDI = NULL;
   m_pJoystick = NULL;
   m_nSliderCount = 0;
   m_nPOVCount = 0;

   m_pAnimateCameraInfo = HBoolProperty::HInfo::New( "Animate Camera", "AnimateCamera", FALSE );

   InitDirectInput();
}

JoystickNavigatorPlugin::~JoystickNavigatorPlugin()
{
   FreeDirectInput();

   delete m_pAnimateCameraInfo;
   //delete m_pPositionInfo;
   //delete m_pOrientationInfo;
}

HRESULT JoystickNavigatorPlugin::InitDirectInput()
{
   // Register with the DirectInput subsystem and get a pointer
   // to a IDirectInput interface we can use.
   // Create a DInput object
   HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL );
   if (FAILED(hr))
      return hr;

   // Look for a simple Joystick we can use for this sample program.
   hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, ::EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY );
   if (FAILED(hr))
      return hr;

   // Make sure we got a Joystick
   if (!m_pJoystick)
   {
      DisplayWarning( "Joystick not found." );
      return S_OK;
   }

   // Set the data format to "simple Joystick" - a predefined data format 
   //
   // A data format specifies which controls on a device we are interested in,
   // and how they should be reported. This tells DInput that we will be
   // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
   hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 );
   if (FAILED(hr))
      return hr;

   CWnd *pWnd = AfxGetMainWnd();
   if (!pWnd)
      return S_OK;

   // Set the cooperative level to let DInput know how this device should
   // interact with the system and with other DInput applications.
   HWND hWnd = pWnd->GetSafeHwnd();//GetHMainInterface()->GetMainWnd();
   hr = m_pJoystick->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND );
   if (FAILED(hr))
      return hr;

   // Enumerate the Joystick objects. The callback function enabled user
   // interface elements for objects that are found, and sets the min/max
   // values property for discovered axes.
   hr = m_pJoystick->EnumObjects( ::EnumObjectsCallback, this, DIDFT_ALL );
   if (FAILED(hr))
      return hr;

   return S_OK;
}

void JoystickNavigatorPlugin::FreeDirectInput()
{
   // Unacquire the device one last time just in case 
   // the app tried to exit while the device is still acquired.
   if (m_pJoystick)
   {
      m_pJoystick->Unacquire();
      m_pJoystick->Release();
      m_pJoystick = NULL;
   }

   // Release any DirectInput objects.
   if (m_pDI)
   {
      m_pDI->Release();
      m_pDI = NULL;
   }
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated Joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL JoystickNavigatorPlugin::EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance )
{
   // Obtain an interface to the enumerated Joystick.
   HRESULT hr = m_pDI->CreateDevice( pdidInstance->guidInstance, &m_pJoystick, NULL );

   // If it failed, then we can't use this Joystick. (Maybe the user unplugged
   // it while we were in the middle of enumerating it.)
   if (FAILED(hr))
      return DIENUM_CONTINUE;

   // Stop enumeration. Note: we're just taking the first Joystick we get. You
   // could store all the enumerated Joysticks and let the user pick.
   return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       Joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL JoystickNavigatorPlugin::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE *pdidoi )
{
   // For axes that are returned, set the DIPROP_RANGE property for the
   // enumerated axis in order to scale min/max values.
   if( pdidoi->dwType & DIDFT_AXIS )
   {
      DIPROPRANGE diprg; 
      diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
      diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
      diprg.diph.dwHow        = DIPH_BYID; 
      diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
      diprg.lMin              = -1000; 
      diprg.lMax              = +1000; 

      // Set the range for the axis
      if (FAILED( m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph )))
         return DIENUM_STOP;
   }

   return DIENUM_CONTINUE;
}

ManipulatorInstance *JoystickNavigatorPlugin::CreateInstance( HCGLBaseView *pView )
{
   return SNEW JoystickNavigator( this, pView );
}

const char *JoystickNavigatorPlugin::GetTitleName() const 
{ 
   return "Joystick";
}

UINT JoystickNavigatorPlugin::GetBitmapID() const
{
   return IDB_JOYSTICKNAVIGATOR;
}

// Instance //////////////////////////////////////////////
JoystickNavigator::JoystickNavigator( JoystickNavigatorPlugin *pPlugin, HCGLBaseView *pView ) :
 NavigatorInstance( pPlugin, pView )
{
   m_pJoystick = pPlugin->m_pJoystick;

   m_nTimerID = 0;
   if (m_pJoystick)
   {
      CWnd *pWnd = AfxGetMainWnd();
      m_nTimerID = pWnd->SetTimer( (UINT_PTR)this, 1000 / 60, MyTimerProc );
   }

   m_pPropertyArray = HPropertyArray::New();

   m_pAnimateCamera = HBoolProperty::New(pPlugin->m_pAnimateCameraInfo, NULL);
   m_pPropertyArray->Add( m_pAnimateCamera );

   m_pPerspective = NULL;
   if (pView->IsKindOf(SVT_CGL3DView))
      m_nPerspectiveIndex = m_pPropertyArray->Add( m_pPerspective );
   else
      m_nPerspectiveIndex = -1;

   m_pFOV = NULL;
   m_nFOVIndex = m_pPropertyArray->Add( m_pFOV );

   m_pFocusDistance = NULL;
   m_nFocusDistanceIndex = m_pPropertyArray->Add( m_pFocusDistance );

   m_pPosition = NULL;
   m_nPositionIndex = m_pPropertyArray->Add( m_pPosition );

   m_pOrientation = NULL;
   if (pView->IsKindOf(SVT_CGL3DView))
      m_nOrientationIndex = m_pPropertyArray->Add( m_pOrientation );
   else
      m_nOrientationIndex = -1;

   m_pManualClip = NULL;
   if (pView->IsKindOf(SVT_CGL3DView))
      m_nManualClipIndex = m_pPropertyArray->Add( m_pManualClip );
   else
      m_nManualClipIndex = -1;
   m_pNearClip = NULL;
   m_pFarClip = NULL;

   m_bPerspectiveSetLastTime = FALSE;

   ResetCameraProperties();
}

JoystickNavigator::~JoystickNavigator()
{
   delete m_pAnimateCamera;

   delete m_pPropertyArray;

   if (m_nTimerID)
      AfxGetMainWnd()->KillTimer( m_nTimerID );
}

void JoystickNavigator::ResetCameraProperties()
{
   HCGL2DView *p2DView = (HCGL2DView *)m_pView;
   HCamera *pCamera = p2DView->GetCurrentCamera();
   if (!pCamera)
      return;

   HFloatProperty *pFocusDistance = pCamera->GetFocusDistance();
   if (m_pFocusDistance != pFocusDistance)
   {
      if (m_nPerspectiveIndex != -1)
      {
         m_pPerspective = pCamera->GetPerspective();
         m_pPropertyArray->SetAt( m_nPerspectiveIndex, m_pPerspective );
      }

      m_pFOV = pCamera->GetFOV();
      m_pPropertyArray->SetAt( m_nFOVIndex, m_pFOV );

      m_pFocusDistance = pCamera->GetFocusDistance();
      m_pPropertyArray->SetAt( m_nFocusDistanceIndex, m_pFocusDistance );

      m_pPosition = pCamera->GetTransform()->GetTranslate();
      m_pPropertyArray->SetAt( m_nPositionIndex, m_pPosition );

      if (m_nOrientationIndex != -1)
      {
         m_pOrientation = pCamera->GetTransform()->GetRotate();
         m_pPropertyArray->SetAt( m_nOrientationIndex, m_pOrientation );
      }

      if (m_nManualClipIndex != -1)
      {
         m_pManualClip = pCamera->GetManualClip();
         m_pPropertyArray->SetAt( m_nManualClipIndex, m_pManualClip );
      }
   }
}

HPropertyArray *JoystickNavigator::GetPropertyArray() const
{
   return m_pPropertyArray;
}

BOOL JoystickNavigator::SetViewAngle( HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye )
{
   pView->DefaultSetViewAngle( nIndex, bSnapToLastBirdsEye );

   ResetCameraProperties();

   return TRUE;
}

BOOL JoystickNavigator::ZoomFit( HCGLBaseView *pView, HSnipeObjectList *pList )
{
   pView->DefaultZoomFit( pList );

   return TRUE;
}

void JoystickNavigator::OnTimer()
{
   HRESULT hr = m_pJoystick->Poll();
   if (FAILED(hr))  
   {
      hr = m_pJoystick->Acquire();
      return; 
   }

   DIJOYSTATE2 js;
   hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js );
   if (FAILED(hr))
      return;

   // Translate
   int absx = abs(js.lX);
   int absy = abs(js.lY);
   if (absx>50 || absy>50)
   {
      Vector delta( (float)js.lX, 0, (float)js.lY );
      delta /= 100.0f;
      delta *= 1.2f; // speed
      m_pView->ViewToWorld( delta, TRUE );

      Vector pos = m_pPosition->GetValue();
      pos += delta;

      m_pPosition->StoreValue( pos );
   }

   // Rotate
   absx = abs(js.lZ);
   absy = abs(js.lRz);
   if (m_pOrientation && (absx>50 || absy>50))
   {
      float fAngle = m_pOrientation->GetValue().x;
      float fRotation = m_pOrientation->GetValue().y;
      float fRoll = m_pOrientation->GetValue().z;

      fRotation -= (float)js.lRz / 400.0f;

      fAngle += (float)js.lZ / 400.0f;
      if (fAngle > 90.0f)
         fAngle = 90.0f;
      else if (fAngle < -90.0f)
         fAngle = -90.0f;

      m_pOrientation->StoreValue( Vector( fAngle, fRotation, fRoll ));
   }

   // Climb
   if (js.rgdwPOV[0] != -1)
   {
      Matrix44 matrix;
      matrix.RotationMatrix( (float)js.rgdwPOV[0]/100.0f * (float)DEGTORAD, 0.0f, 0.0f, -1.0f );
      Vector dir( 0.0f, 3.0f, 0.0f );
      dir *= matrix;
      m_pView->ViewToWorld( dir, TRUE );
      m_pPosition->StoreValue( m_pPosition->GetValue() + dir );
   }

   // Toggle Perspective
   if (js.rgbButtons[5] & 0x80)
   {
      if (!m_bPerspectiveSetLastTime)
      {
         m_pPerspective->StoreValue( !m_pPerspective->GetValue() );
         m_bPerspectiveSetLastTime = TRUE;
      }
   }
   else if (m_bPerspectiveSetLastTime)
      m_bPerspectiveSetLastTime = FALSE;

   // Zoom In
   if (js.rgbButtons[7] & 0x80)
      m_pFOV->StoreValue( m_pFOV->GetValue() - 1 );

   // Zoom Out
   if (js.rgbButtons[6] & 0x80)
      m_pFOV->StoreValue( m_pFOV->GetValue() + 1 );
}
