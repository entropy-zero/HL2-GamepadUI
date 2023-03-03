#ifndef GAMEPADUI_TEXTENTRY_H
#define GAMEPADUI_TEXTENTRY_H
#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_button.h"
#include "vgui_controls/TextEntry.h"

class GamepadUITextEntry : public vgui::TextEntry, public SchemeValueMap
{
    DECLARE_CLASS_SIMPLE( GamepadUITextEntry, vgui::TextEntry );
public:
	GamepadUITextEntry( vgui::Panel *pParent, const char *pSchemeFile );
    ~GamepadUITextEntry()
    {
    }

	void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	void OnThink() OVERRIDE;
	void Paint() OVERRIDE;
	void OnMousePressed( vgui::MouseCode code ) OVERRIDE;
	void OnKeyCodeTyped( vgui::KeyCode code ) OVERRIDE;
	void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	void OnKeyTyped( wchar_t unichar ) OVERRIDE;
	void NavigateTo() OVERRIDE;
	void NavigateFrom() OVERRIDE;
	void OnCursorEntered() OVERRIDE;
	void OnCursorExited() OVERRIDE;
	void FireActionSignal() OVERRIDE;
	bool HasFocus() OVERRIDE;

	virtual void RunAnimations( ButtonState state );
	void DoAnimations( bool bForce = false );
	void PaintBorders();

	virtual ButtonState GetCurrentButtonState();

	void ControllerInputEnabled()
	{
		int nX, nY, nW, nH;
		GetBounds( nX, nY, nW, nH );
		if (!GamepadUI::GetInstance().GetSteamInput()->IsEnabled() || GamepadUI::GetInstance().GetSteamInput()->ShowFloatingGamepadTextInput( true, nX, nY, nW, nH ))
		{
			m_bControllerInputCaptured = true;
		}
	}

	void OnKillFocus();

private:
	bool m_bControllerInputCaptured = false;

	bool m_bCursorOver = false;
	bool m_bNavigateTo = false;

	ButtonState m_ePreviousState = ButtonStates::Out;

public:

	GAMEPADUI_BUTTON_ANIMATED_PROPERTY( float, m_flWidth,                "TextEntry.Width",               "350",             SchemeValueTypes::ProportionalFloat );
    GAMEPADUI_BUTTON_ANIMATED_PROPERTY( float, m_flHeight,               "TextEntry.Height",              "300",             SchemeValueTypes::ProportionalFloat );
    GAMEPADUI_BUTTON_ANIMATED_PROPERTY( Color, m_colBackgroundColor,     "TextEntry.Background",          "0 0 0 0",         SchemeValueTypes::Color );
    GAMEPADUI_BUTTON_ANIMATED_PROPERTY( Color, m_colTextColor,           "TextEntry.Text",                "255 255 255 255", SchemeValueTypes::Color );

    GAMEPADUI_BUTTON_ANIMATED_PROPERTY( float, m_flBorder,               "TextEntry.Text.Border",       "0",               SchemeValueTypes::ProportionalFloat );
    GAMEPADUI_BUTTON_ANIMATED_PROPERTY( Color, m_colBorder,              "TextEntry.Background.Border", "0 0 0 0",         SchemeValueTypes::Color );
	
    vgui::HFont m_hTextFont        = vgui::INVALID_FONT;
    vgui::HFont m_hTextFontOver    = vgui::INVALID_FONT;
};

#endif // GAMEPADUI_TEXTENTRY_H
