"Scheme"
{
	"BaseSettings"
	{
		"FgColor"			"255 244 244 100"
		"FgColor_vrmode"	"255 244 244 200"
		"BgColor"			"0 0 0 76"

		"Panel.FgColor"			"255 244 244 100"
		"Panel.BgColor"			"0 0 0 76"
		
		"BrightFg"		"244 36 28 255"

		"DamagedBg"			"180 0 0 200"
		"DamagedFg"			"180 0 0 230"
		"BrightDamagedFg"		"255 0 0 255"

		// weapon selection colors
		"SelectionNumberFg"		"255 244 244 255"
		"SelectionTextFg"		"255 244 244 255"
		"SelectionEmptyBoxBg" 	"0 0 0 80"
		"SelectionBoxBg" 		"0 0 0 80"
		"SelectionSelectedBoxBg" "0 0 0 80"
		
		"ZoomReticleColor"	"255 244 244 255"

		// HL1-style HUD colors
		"Yellowish"			"255 244 244 255"
		"Normal"			"255 244 244 255"
		"Caution"			"255 48 0 255"

		// Top-left corner of the "Half-Life 2" on the main screen
		"Main.Title1.X"			"53"
		"Main.Title1.Y"			"190"
		"Main.Title1.Y_hidef"	"184"
		"Main.Title1.Color"	"244 36 28 200"

		// Top-left corner of secondary title e.g. "DEMO" on the main screen
		"Main.Title2.X"				"291"
		"Main.Title2.Y"				"207"
		"Main.Title2.Y_hidef"		"242"
		"Main.Title2.Color"	"244 36 28 200"

		// Top-left corner of the menu on the main screen
		"Main.Menu.X"			"53"
		"Main.Menu.X_hidef"		"76"
		"Main.Menu.Y"			"240"

		// Blank space to leave beneath the menu on the main screen
		"Main.BottomBorder"	"32"

		Frame.TitleTextInsetX			16
		Frame.ClientInsetX				8
		Frame.ClientInsetY				6
		Frame.BgColor					"160 160 160 128"
		Frame.OutOfFocusBgColor			"160 160 160 32"
		Frame.FocusTransitionEffectTime	"0.0"							// time it takes for a window to fade in/out on focus/out of focus
		Frame.TransitionEffectTime		"0.0"							// time it takes for a window to fade in/out on open/close
		Frame.AutoSnapRange				"0"
		FrameGrip.Color1				"200 200 200 196"
		FrameGrip.Color2				"0 0 0 196"
		FrameTitleButton.FgColor		"200 200 200 196"
		FrameTitleButton.BgColor		"Blank"
		FrameTitleButton.DisabledFgColor	"255 255 255 192"
		FrameTitleButton.DisabledBgColor	"Blank"
		FrameSystemButton.FgColor		"Blank"
		FrameSystemButton.BgColor		"Blank"
		FrameSystemButton.Icon			""
		FrameSystemButton.DisabledIcon	""
		FrameTitleBar.Font				"UiBold"
		FrameTitleBar.Font				"DefaultLarge"
		FrameTitleBar.TextColor			"White"
		FrameTitleBar.BgColor			"Blank"
		FrameTitleBar.DisabledTextColor	"255 255 255 192"
		FrameTitleBar.DisabledBgColor	"Blank"

		Label.TextDullColor				"DullWhite"
		Label.TextColor					"OffWhite"
		Label.TextBrightColor			"White"
		Label.SelectedTextColor			"White"
		Label.BgColor					"Blank"
		Label.DisabledFgColor1			"117 117 117 255"
		Label.DisabledFgColor2			"30 30 30 255"

		ListPanel.TextColor					"OffWhite"
		ListPanel.TextBgColor				"Blank"
		ListPanel.BgColor					"TransparentBlack"
		ListPanel.SelectedTextColor			"Black"
		ListPanel.SelectedBgColor			"Orange"
		ListPanel.SelectedOutOfFocusBgColor	"255 155 0 128"
		ListPanel.EmptyListInfoTextColor	"OffWhite"

		Menu.TextColor					"White"
		Menu.BgColor					"160 160 160 64"
		Menu.ArmedTextColor				"Black"
		Menu.ArmedBgColor				"Orange"
		Menu.TextInset					"6"

		ScrollBar.Wide					17

		ScrollBarButton.FgColor				"White"
		ScrollBarButton.BgColor				"Blank"
		ScrollBarButton.ArmedFgColor		"White"
		ScrollBarButton.ArmedBgColor		"Blank"
		ScrollBarButton.DepressedFgColor	"White"
		ScrollBarButton.DepressedBgColor	"Blank"

		ScrollBarSlider.FgColor				"Blank"			// nob color
		ScrollBarSlider.BgColor				"255 255 255 64"	// slider background color

		SectionedListPanel.HeaderTextColor	"White"
		SectionedListPanel.HeaderBgColor	"Blank"
		SectionedListPanel.DividerColor		"Black"
		SectionedListPanel.TextColor		"DullWhite"
		SectionedListPanel.BrightTextColor	"White"
		SectionedListPanel.BgColor			"TransparentBlack"
		SectionedListPanel.SelectedTextColor			"Black"
		SectionedListPanel.SelectedBgColor				"Orange"
		SectionedListPanel.OutOfFocusSelectedTextColor	"Black"
		SectionedListPanel.OutOfFocusSelectedBgColor	"255 155 0 128"

		Slider.NobColor				"108 108 108 255"
		Slider.TextColor			"180 180 180 255"
		Slider.TrackColor			"31 31 31 255"
		Slider.DisabledTextColor1	"117 117 117 255"
		Slider.DisabledTextColor2	"30 30 30 255"

		TextEntry.TextColor			"OffWhite"
		TextEntry.BgColor			"TransparentBlack"
		TextEntry.CursorColor		"OffWhite"
		TextEntry.DisabledTextColor	"DullWhite"
		TextEntry.DisabledBgColor	"Blank"
		TextEntry.SelectedTextColor	"Black"
		TextEntry.SelectedBgColor	"Orange"
		TextEntry.OutOfFocusSelectedBgColor	"255 155 0 128"
		TextEntry.FocusEdgeColor	"0 0 0 196"
	}
	
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		// fonts are used in order that they are listed
		"DebugFixed"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"10"
				"weight"	"500"
				"antialias" "1"
			}
		}
		// fonts are used in order that they are listed
		"DebugFixedSmall"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"7"
				"weight"	"500"
				"antialias" "1"
			}
		}
		"DefaultFixedOutline"
		{
			"1"
			{
				"name"		 "Lucida Console" [$WINDOWS]
				"name"		 "Lucida Console" [$X360]
				"name"		 "Verdana" [$POSIX]
				"tall"		"14" [$LINUX]
				"tall"		"11" [$OSX]
				"tall"		 "10"
				"tall_lodef" "15"
				"tall_hidef" "20"
				"weight"	 "0"
				"outline"	 "1"
			}
		}
		"Default"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"16"
				"weight"	"500"
			}
		}
		"DefaultBold"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana Bold" [$POSIX]
				"tall"		"16"
				"weight"	"1000"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"16"
				"weight"	"500"
				"underline" "1"
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"12" [!$POSIX]
				"tall"		"13" [$POSIX]
				"weight"	"0"
			}
		}
		"DefaultSmallDropShadow"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"13"
				"weight"	"0"
				"dropshadow" "1"
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"12"
				"weight"	"0"
			}
		}

		"DefaultLarge"
		{
			"1"
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"18"
				"weight"	"0"
			}
		}
		"UiBold"
		{
			"1"	[$WIN32]
			{
				"name"		"Tahoma" [!$POSIX]
				"name"		"Verdana" [$POSIX]
				"tall"		"12"
				"weight"	"1000"
			}
			"1"	[$X360]
			{
				"name"		"Tahoma"
				"tall"		"24"
				"weight"	"2000"
				"outline"	"1"
			}
		}
		"MenuLarge"
		{
			"1"	[$POSIX]
			{
				"name"		"Helvetica Bold"
				"tall"		"20"
				"antialias" "1"
			}
			"1"	[$WIN32]
			{
				"name"		"Verdana" 
				"tall"		"16" 
				"weight"	"600"
				"antialias" "1"
			}
			"1"	[$X360]
			{
				"name"		"Verdana"
				"tall"			"14"
				"tall_hidef"	"20"
				"weight"	"1200"
				"antialias" "1"
				"outline" "1"
			}
		}
		"ConsoleText"
		{
			"1"
			{
				"name"		 "Lucida Console" [$WINDOWS]
				"name"		 "Lucida Console" [$X360]
				"name"		 "Verdana" [$POSIX]
				"tall"		"11" [$OSX]
				"tall"		"14" [$LINUX]
				"tall"		"10"
				"weight"	"500"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
			}
		}

		"Trebuchet24"
		{
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"24"
				"weight"	"900"
			}
		}

		"Trebuchet20"
		{
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"20"
				"weight"	"900"
			}
		}

		"Trebuchet18"
		{
			"1"
			{
				"name"		"Trebuchet MS"
				"tall"		"18"
				"weight"	"900"
			}
		}
		
		"DefaultFixed"
		{
			"1"
			{
				"name"		 "Lucida Console" [$WINDOWS]
				"name"		 "Lucida Console" [$X360]
				"name"		 "Verdana" [$POSIX]
				"tall"		"11" [$POSIX]
				"tall"		"10"
				"weight"	"0"
			}
//			"1"
//			{
//				"name"		"FixedSys"
//				"tall"		"20"
//				"weight"	"0"
//			}
		}

		"DefaultFixedDropShadow"
		{
			"1"
			{
				"name"		 "Lucida Console" [$WINDOWS]
				"name"		 "Lucida Console" [$X360]
				"name"		 "Verdana" [$OSX]
				"name"		 "Courier" [$LINUX]
				"tall"		"14" [$LINUX]
				"tall"		"11" [$POSIX]
				"tall"		"10"
				"weight"	"0"
				"dropshadow" "1"
			}
//			"1"
//			{
//				"name"		"FixedSys"
//				"tall"		"20"
//				"weight"	"0"
//			}
		}
	}
}
