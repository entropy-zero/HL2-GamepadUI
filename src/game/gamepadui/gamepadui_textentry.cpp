#include "gamepadui_textentry.h"
#include "gamepadui_interface.h"
#include "gamepadui_util.h"

#include "vgui/IVGui.h"
#include "vgui/ISurface.h"
#include "vgui/IInput.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

GamepadUITextEntry::GamepadUITextEntry( vgui::Panel *pParent, const char *pSchemeFile )
    : BaseClass( pParent, "" )
{
    SetScheme(vgui::scheme()->LoadSchemeFromFile( pSchemeFile, "SchemePanel" ) );
}

void GamepadUITextEntry::ApplySchemeSettings(vgui::IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    SetPaintBorderEnabled( false );
    SetPaintBackgroundEnabled( true );
    SetConsoleStylePanel( true );

    UpdateSchemeProperties( this, pScheme );

    //SetVerticalScrollbar( false );

    m_hTextFont        = pScheme->GetFont( "TextEntry.Font", true );
    m_hTextFontOver    = pScheme->GetFont( "TextEntry.Font.Over", true );
    if (m_hTextFontOver == vgui::INVALID_FONT )
        m_hTextFontOver = m_hTextFont;

    m_ePreviousState = ButtonStates::Out;

    if (GamepadUI::GetInstance().GetScreenRatio() != 1.0f)
    {
        float flScreenRatio = GamepadUI::GetInstance().GetScreenRatio();

        m_flWidth *= flScreenRatio;
        for (int i = 0; i < ButtonStates::Count; i++)
            m_flWidthAnimationValue[i] *= flScreenRatio;
    }

    SetSize( m_flWidth, m_flHeight );
    DoAnimations( true );
}

void GamepadUITextEntry::RunAnimations( ButtonState state )
{
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_flWidth, vgui::AnimationController::INTERPOLATOR_LINEAR );
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_flHeight, vgui::AnimationController::INTERPOLATOR_LINEAR );
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_colBackgroundColor, vgui::AnimationController::INTERPOLATOR_LINEAR );
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_colTextColor, vgui::AnimationController::INTERPOLATOR_LINEAR );
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_flBorder, vgui::AnimationController::INTERPOLATOR_LINEAR );
    GAMEPADUI_RUN_ANIMATION_COMMAND( m_colBorder, vgui::AnimationController::INTERPOLATOR_LINEAR );
}

void GamepadUITextEntry::DoAnimations( bool bForce )
{
    ButtonState state = this->GetCurrentButtonState();
    if (m_ePreviousState != state || bForce)
    {
        this->RunAnimations( state );
        m_ePreviousState = state;
    }

    SetSize(m_flWidth, m_flHeight);
}

void GamepadUITextEntry::OnThink()
{
    BaseClass::OnThink();
    DoAnimations();

    ButtonState state = GetCurrentButtonState();
    SetFont( state == ButtonStates::Out ? m_hTextFont : m_hTextFontOver );
    SetBgColor( m_colBackgroundColor );
    SetFgColor( m_colTextColor );
}

void GamepadUITextEntry::PaintBorders()
{
    if ( m_flBorder )
    {
        // Need to make the borders draw outside of the text entry
        if (GetVParent())
            vgui::surface()->PushMakeCurrent( GetVParent(), false );

        vgui::surface()->DrawSetColor(m_colBorder);

        int nX, nY;
        GetPos( nX, nY );

        vgui::surface()->DrawFilledRect( nX - m_flBorder, nY - m_flBorder, nX, nY + m_flHeight );
        vgui::surface()->DrawFilledRect( nX, nY - m_flBorder, nX + m_flWidth + m_flBorder, nY );

        vgui::surface()->DrawFilledRect( nX - m_flBorder, nY + m_flHeight, nX + m_flWidth, nY + m_flHeight + m_flBorder );
        vgui::surface()->DrawFilledRect( nX + m_flWidth, nY, nX + m_flWidth + m_flBorder, nY + m_flHeight + m_flBorder );

        if (GetVParent())
            vgui::surface()->PopMakeCurrent( GetVParent() );
    }
}

void GamepadUITextEntry::Paint()
{
    BaseClass::Paint();

    PaintBorders();

    m_bNavigateTo = false;
}

ButtonState GamepadUITextEntry::GetCurrentButtonState()
{
    if ( m_bControllerInputCaptured )
        return ButtonStates::Pressed;
    else if ( m_bCursorOver && IsEnabled() )
        return ButtonStates::Over;
    else if ( m_bNavigateTo || BaseClass::HasFocus() )
    {
        return ButtonStates::Over;
    }
    else
        return ButtonStates::Out;
}

void GamepadUITextEntry::NavigateTo()
{
    BaseClass::NavigateTo();

    m_bNavigateTo = true;
    RequestFocus( 0 );
}


void GamepadUITextEntry::NavigateFrom()
{
    BaseClass::NavigateFrom();

    m_bNavigateTo = false;
}

void GamepadUITextEntry::OnMousePressed( vgui::MouseCode code )
{
	BaseClass::OnMousePressed( code );

	if (m_bCursorOver)
	{
		ControllerInputEnabled();
	}
    else if (m_bControllerInputCaptured)
    {
        vgui::input()->SetMouseCapture( NULL );
        m_bControllerInputCaptured = false;
    }
}

void GamepadUITextEntry::OnKeyCodeTyped( vgui::KeyCode code )
{
    if (!m_bControllerInputCaptured)
    {
        // Fall through to base panel
        Panel::OnKeyCodeTyped( code );
        return;
    }

    BaseClass::OnKeyCodeTyped( code );
}

void GamepadUITextEntry::OnKeyCodePressed( vgui::KeyCode code )
{
    switch (code)
    {
        case KEY_ENTER:
        case KEY_XBUTTON_A:
            {
                ControllerInputEnabled();
            }
            break;
        case KEY_ESCAPE:
        case KEY_XBUTTON_B:
            {
                m_bControllerInputCaptured = false;
            }
            break;
    }

	if (!m_bControllerInputCaptured)
    {
        // Fall through to base panel
        Panel::OnKeyCodePressed( code );
        return;
    }

	BaseClass::OnKeyCodePressed( code );
}

void GamepadUITextEntry::OnKeyTyped( wchar_t unichar )
{
    if (!m_bControllerInputCaptured)
    {
        // Fall through to base panel
        Panel::OnKeyTyped( unichar );
        return;
    }

    BaseClass::OnKeyTyped( unichar );
}

void GamepadUITextEntry::OnKillFocus()
{
    m_bControllerInputCaptured = false;
}

void GamepadUITextEntry::OnCursorEntered()
{
#ifdef STEAM_INPUT
    if ( GamepadUI::GetInstance().GetSteamInput()->IsEnabled() || !IsEnabled() )
#elif defined(HL2_RETAIL) // Steam input and Steam Controller are not supported in SDK2013 (Madi)
    if ( g_pInputSystem->IsSteamControllerActive() || !IsEnabled() )
#else
    if ( !IsEnabled() )
#endif
        return;

    BaseClass::OnCursorEntered();

    m_bCursorOver = true;
}

void GamepadUITextEntry::OnCursorExited()
{
    BaseClass::OnCursorExited();

    m_bCursorOver = false;
}

void GamepadUITextEntry::FireActionSignal()
{
    BaseClass::FireActionSignal();

    PostMessageToAllSiblingsOfType< GamepadUIButton >( new KeyValues( "OnSiblingGamepadUIButtonOpened" ) );
}

bool GamepadUITextEntry::HasFocus()
{
    if (!m_bControllerInputCaptured)
        return false;

    return BaseClass::HasFocus();
}
