"MainMenu"
{	
	"ResumeGame"
	{
		"text"			"#GameUI_GameMenu_ResumeGame"
		"command"		"cmd gamemenucommand resumegame"
		"priority"		"8"
		"family"		"ingame"
	}
	
	"NewGame"
	{
		"text"			"#GameUI_GameMenu_NewGame"
		"command"		"cmd gamepadui_opennewgamedialog"
		"priority"		"7"
		"family"		"all"
	}
	
	"BonusMaps"
	{
		"text"			"#GameUI_GameMenu_BonusMaps"
		"command"		"cmd gamepadui_openbonusmapsdialog"
		"priority"		"6"
		"family"		"all"
	}
	
	"SaveGame"
	{
		"text"			"#GameUI_GameMenu_SaveGame"
		"command"		"cmd gamepadui_opensavegamedialog"
		"priority"		"5"
		"family"		"ingame"
	}

	"LoadGame"
	{
		"text"			"#GameUI_GameMenu_LoadGame"
		"command"		"cmd gamepadui_openloadgamedialog"
		"priority"		"4"
		"family"		"all"
	}

	"Options"
	{
		"text"			"#GameUI_GameMenu_Options"
		"command"		"cmd gamepadui_openoptionsdialog"
		"priority"		"3"
		"family"		"all"
	}
	
	"Workshop Publisher"
	{
		"text"			"#GameUI_GameMenu_Workshop"
		"command"		"cmd gamepadui_openpublishdialog"
		"priority"		"2"
		"family"		"all"
	}
	
	//"Achievements"
	//{
	//	"text"			"#GameUI_GameMenu_Achievements"
	//	"command"		"cmd gamepadui_openachievementsdialog"
	//	"priority"		"2"
	//	"family"		"all"
	//}

	"Quit"
	{
		"text"			"#GameUI_GameMenu_Quit"
		"command"		"cmd gamepadui_openquitgamedialog"
		"priority"		"1"
		"family"		"all"
	}
}