#ifndef GAMEPADUI_COMBOBOX_H
#define GAMEPADUI_COMBOBOX_H
#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_button.h"
#include "vgui_controls/ComboBox.h"

class GamepadUIComboBox : public vgui::ComboBox //, public SchemeValueMap
{
    DECLARE_CLASS_SIMPLE( GamepadUIComboBox, vgui::ComboBox );
public:
	GamepadUIComboBox( Panel *parent, const char *panelName, int numLines, bool allowEdit ) :
		BaseClass( parent, panelName, numLines, allowEdit )
	{

	}

	void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
};

#endif // GAMEPADUI_COMBOBOX_H
