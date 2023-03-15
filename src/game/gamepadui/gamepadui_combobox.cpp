#include "gamepadui_combobox.h"

void GamepadUIComboBox::ApplySchemeSettings(vgui::IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    SetFont( pScheme->GetFont( "TextEntry.Font", true ) );
}
