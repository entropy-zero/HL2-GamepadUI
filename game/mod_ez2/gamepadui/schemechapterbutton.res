"Scheme"
{
	"BaseSettings"
	{
		"Button.Width.Out"						"340"
		"Button.Width.Over"						"340"
		"Button.Width.Pressed"					"340"

		"Button.Height.Out"						"220"
		"Button.Height.Over"					"220"
		"Button.Height.Pressed"					"220"

		"Button.Text.OffsetX.Out"					"12"
		"Button.Text.OffsetX.Over"					"12"
		"Button.Text.OffsetX.Pressed"				"12"
		"Button.Text.OffsetY.Out"					"100"
		"Button.Text.OffsetY.Over"					"70"
		"Button.Text.OffsetY.Pressed"				"70"

		"Button.Description.OffsetX.Out"			"0"
		"Button.Description.OffsetY.Out"			"-1"
		"Button.Description.OffsetX.Over"			"0"
		"Button.Description.OffsetY.Over"			"-1"
		"Button.Description.OffsetX.Pressed"		"0"
		"Button.Description.OffsetY.Pressed"		"-1"

		"Button.Description.Hide.Out"				"1"
		"Button.Description.Hide.Over"				"0"
		"Button.Description.Hide.Pressed"			"0"

		"Button.Animation.Width"					"0.15"
		"Button.Animation.Height"					"0.25"
		"Button.Animation.Background"				"0.2"
		"Button.Animation.Text"					"0.2"
		"Button.Animation.Description"			"0.3"

		"FooterButtons.OffsetX"					"64"
		"FooterButtons.OffsetY"					"48"
		
		"Button.Sound.Armed"					"ui/buttonrollover.wav"
		"Button.Sound.Released"					"ui/buttonclickrelease.wav"
		"Button.Sound.Depressed"				""
	}

	"Colors"
	{
		"Button.Background.Out"						"0 0 0 0"
		"Button.Background.Over"					"255 255 255 255"
		"Button.Background.Pressed"					"255 255 255 255"

		"Button.Text.Out"							"255 255 255 255" // 150 alpha
		"Button.Text.Over"							"0 0 0 255"
		"Button.Text.Pressed"						"0 0 0 255"

		"Button.Description.Out"					"0 0 0 0"
		"Button.Description.Over"					"0 0 0 255"
		"Button.Description.Pressed"				"0 0 0 255"
	}

	"Fonts"
	{
		"Button.Text.Font"
		{
			"settings"
			{
				"name"			"Frak" // Alte DIN 1451 Mittelschrift
				"tall"			"26"
				"weight"		"800" // 400
				"antialias"		"1"
			}
		}

		"Button.Description.Font"
		{
			"settings"
			{
				"name"			"Frak" // Noto Sans
				"tall"			"21"
				"weight"		"600" // 400
				//"italic"		"1"
				"antialias"		"1"
			}
		}
	}

	"CustomFontFiles"
	{
		"file"		"gamepadui/fonts/NotoSans-Regular.ttf"
		"file"		"gamepadui/fonts/din1451alt.ttf"
	}
}