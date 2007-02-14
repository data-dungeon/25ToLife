#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EORConnect.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

ScreenEORConnect::ScreenEORConnect()
{
    _status = RUNNING;
    _cancelDnas = false;
}

void ScreenEORConnect::OnScreenIntro( void )
{
    _status = RUNNING;
    _cancelDnas = false;

    ScreenBaseClass::OnScreenIntro();
}

void ScreenEORConnect::OnScreenExit( void )
{
    _status = RUNNING;
    _cancelDnas = false;

    ScreenBaseClass::OnScreenExit();
}

void ScreenEORConnect::OnButtonCancel( void )
{
     _cancelDnas = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the screen.  Move the bullet
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORConnect::OnScreenUpdate( void )
{
    float x, y;
    static float speed = 5.0f;

    m_pScreen->GetSprite(ID_DNAS_STATUS)->GetPosition(x, y);
    x += 5.0f;
    if (x > 576.0f - m_pScreen->GetSprite(ID_DNAS_STATUS)->GetWidth())
    {
        x = 0.0f;
    }
    m_pScreen->GetSprite(ID_DNAS_STATUS)->SetPositionX(x);
}