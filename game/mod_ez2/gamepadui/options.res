"Options"
{
	"Gameplay" // Difficulty
	{
		"title"			"#GameUI_Gameplay"
		"alternating"   "1"
		//"horizontal"	"1"
		"items"
		{
			"DifficultyHeader"
			{
				"text"			"#GameUI_Difficulty"
				"type"			"headeryheader"
				"center"		"1"
			}
			
			"Easy"
			{
				"text"			"#GameUI_Easy"
				"description"	"#GameUI_EasyDescription"
				"type"			"skillyskill"
				"image"			"gamepadui/easy"
				"skill"			"1"
				"horizontal"	"1"
			}

			"Medium"
			{
				"text"			"#GameUI_MediumSkill"
				"description"	"#GameUI_NormalDescription"
				"type"			"skillyskill"
				"image"			"gamepadui/medium"
				"skill"			"2"
				"horizontal"	"1"
			}

			"Hard"
			{
				"text"			"#GameUI_Hard"
				"description"	"#GameUI_HardDescription"
				"type"			"skillyskill"
				"image"			"gamepadui/hard"
				"skill"			"3"
				"horizontal"	"1"
			}
			
			"HUDHeader"
			{
				"text"			"#GameUI_Gameplay_HUD"
				"type"			"headeryheader"
			}
			
			//"UseInstructor"
			//{
			//	"text"			"#GameUI_DynamicInstructor"
			//	"type"			"skillyskill"
			//	"convar"		"player_use_instructor"
			//
			//	"options"
			//	{
			//		"0"		"#gameui_disabled"
			//		"1"	"#gameui_enabled"
			//	}
			//}
			
			"SLAMGlow"
			{
				"text"			"#GameUI_SLAMGlow"
				"type"			"wheelywheel"
				"convar"		"cl_slam_glow"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"	"#gameui_enabled"
				}
			}
			
			//"SquadHeader"
			//{
			//	"text"			"#GameUI_Gameplay_Squad"
			//	"type"			"headeryheader"
			//}
			
			"SquadmateGlow"
			{
				"text"			"#GameUI_SquadmateGlow"
				"type"			"wheelywheel"
				"convar"		"sv_squadmate_glow"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}
			
			"SquadmateGlowStyle"
			{
				"text"			"#GameUI_SquadmateGlowStyle"
				"type"			"wheelywheel"
				"convar"		"sv_squadmate_glow_style"

				"options"
				{
					"0"		"#GameUI_SquadmateGlowStyle_0"
					"1"		"#GameUI_SquadmateGlowStyle_1"
				}
			}
			
			"VisibleCommandPoint"
			{
				"text"			"#GameUI_VisibleCommandPoint"
				"type"			"wheelywheel"
				"convar"		"sv_visible_command_point"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}
			
			"MiscHeader"
			{
				"text"			"#Valve_Miscellaneous_Title"
				"type"			"headeryheader"
			}
			
			//"ChapterZeroBlur"
			//{
			//	"text"			"#GameUI_ChapterZeroBlur"
			//	"type"			"wheelywheel"
			//	"convar"		"c0_blur"
			//	"description"	"#GameUI_ChapterZeroBlur_Desc"
			//
			//	"options"
			//	{
			//		"0"		"#gameui_disabled"
			//		"1"		"#gameui_enabled"
			//	}
			//}
			
			"DeathScreenFadeout"
			{
				"text"			"#GameUI_DeathScreenFadeout"
				"type"			"wheelywheel"
				"convar"		"cl_death_screen_fadeout"
				"description"	"#GameUI_DeathScreenFadeout_Desc"

				"options"
				{
					"0"		"#gameui_no"
					"1"		"#gameui_yes"
				}
			}
		}
	}
	"Keyboard"
	{
		"title"			"#GameUI_Keyboard"
		"items_from"	"keyboard"
		"alternating"   "1"
		"items"
		{
			"AdvancedHeader"
			{
				"text"			"#GameUI_AdvancedNoEllipsis"
				"type"			"headeryheader"
				"advanced"		"1"
			}
			
			"FastWeaponSwitch"
			{
				"text"			"#GameUI_FastSwitchCheck"
				"type"			"wheelywheel"
				"convar"		"hud_fastswitch"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}
			
			"DeveloperConsole"
			{
				"text"			"#GameUI_DeveloperConsoleCheck"
				"type"			"wheelywheel"
				"convar"		"con_enable"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}
		}
	}
	"Mouse"
	{
		"title"			"#GameUI_Mouse"
		"alternating"   "1"
		"items"
		{
			"ReverseMouse"
			{
				"text"			"#GameUI_ReverseMouse"
				"type"			"wheelywheel"
				"convar"		"m_pitch"
				"signonly"		"1"

				"options"
				{
					"1"		"#gameui_disabled"
					"-1"	"#gameui_enabled"
				}
			}

			"MouseFilter"
			{
				"text"			"#GameUI_MouseFilter"
				"type"			"wheelywheel"
				"convar"		"m_filter"

				"options"
				{
					"0"	"#gameui_disabled"
					"1"	"#gameui_enabled"
				}
			}

			"MouseSensitivity"
			{
				"text"			"#GameUI_MouseSensitivity"
				"type"			"slideyslide"
				"convar"		"sensitivity"

				"min"			"0.1"
				"max"			"6.0"
				"step"			"0.1"
				"mouse_step"	"0.01"

				"textprecision" "2"
			}

			"RawInput"
			{
				"text"			"#GameUI_MouseRaw"
				"type"			"wheelywheel"
				"convar"		"m_rawinput"

				"options"
				{
					"0"	"#gameui_disabled"
					"1"	"#gameui_enabled"
				}
			}

			"MouseAcceleration"
			{
				"text"			"#GameUI_MouseAcceleration"
				"type"			"wheelywheel"
				"convar"		"m_customaccel"

				"options"
				{
					"0"	"#gameui_disabled"
					"1"	"#gameui_enabled"
				}
			}

			"MouseAccelerationExponent"
			{
				"text"			""
				"type"			"slideyslide"
				"convar"		"m_customaccel_exponent"

				"min"			"1.0"
				"max"			"1.4"
				"step"			"0.05"
				"mouse_step"	"0.01"

				"depends_on"	"m_customaccel"

				"textprecision" "2"
			}
		}
	}
	"Audio"
	{
		"title"			"#GameUI_Audio"
		"alternating"   "1"
		"items"
		{
			"SFXSlider"
			{
				"text"			"#GameUI_SoundEffectVolume"
				"type"			"slideyslide"
				"convar"		"volume"

				"min"			"0.0"
				"max"			"1.0"
				"step"			"0.1"
				"mouse_step"	"0.01" // NEW
			}

			"MusicSlider"
			{
				"text"			"#GameUI_MusicVolume"
				"type"			"slideyslide"
				"convar"		"snd_musicvolume"

				"min"			"0.0"
				"max"			"1.0"
				"step"			"0.1"
				"mouse_step"	"0.01" // NEW
			}

			"SpeakerConfiguration"
			{
				"text"			"#GameUI_SpeakerConfiguration"
				"type"			"wheelywheel"
				"convar"		"snd_surround_speakers"

				"options"
				{
					"0"	"#GameUI_Headphones" [!$POSIX]
					"2"	"#GameUI_2Speakers"
					"4"	"#GameUI_4Speakers" [!$POSIX]
					"5"	"#GameUI_5Speakers" [!$POSIX]
					"7"	"#GameUI_7Speakers" [!$POSIX]
				}
			}

			"SoundQuality"
			{
				"text"			"#GameUI_SoundQuality"
				"type"			"wheelywheel"
				"convar"		"_gamepadui_sound_quality"

				"options"
				{
					"0"	"#GameUI_Low"
					"1"	"#GameUI_Medium"
					"2"	"#GameUI_High"
				}
			}

			"CloseCaption"
			{
				"text"			"#GameUI_CloseCaptions_Checkbox"
				"type"			"wheelywheel"
				"convar"		"_gamepadui_closecaptions"

				"options"
				{
					"0"	"#GameUI_NoClosedCaptions"
					"1"	"#GameUI_Subtitles"
					"2"	"#GameUI_SubtitlesAndSoundEffects"
				}
			}

			"LoseFocus"
			{
				"text"			"#GameUI_SndMuteLoseFocus"
				"type"			"wheelywheel"
				"convar"		"snd_mute_losefocus"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}

			"OpenTechCredits"
			{
				"text"			"#GameUI_ThirdPartyTechCredits"
				"type"			"button"
				"command"		"open_techcredits"
			}
		}
	}
	"Video"
	{
		"title"			"#GameUI_Video"
		"alternating"   "1"
		"items"
		{
			"Resolution"
			{
				"text"			"#GameUI_Resolution"
				"type"			"wheelywheel"
				"convar"		"_gamepadui_resolution"

				"options_from"	"resolutions"
			}

			"AspectRatio"
			{
				"text"			"#GameUI_AspectRatio"
				"type"			"wheelywheel"

				"convar"		"_gamepadui_aspectratio"
				"instantapply"	"1"
				"options"
				{
					"0"			"#GameUI_AspectNormal"
					"1"			"#GameUI_AspectWide16x9"
					"2"			"#GameUI_AspectWide16x10"
				}
			}

			"DisplayMode"
			{
				"text"			"#GameUI_DisplayMode"
				"type"			"wheelywheel"
				"convar"		"_gamepadui_displaymode"
				"instantapply"	"1"

				"options_from"	"displaymode"
			}

			//"HUDAspectRatio"
			//{
			//	"text"			"#GameUI_HUDAspectRatio"
			//	"type"			"wheelywheel"
			//
			//	"convar"		"_gamepadui_hudaspect"
			//	"instantapply"	"1"
			//	"options"
			//	{
			//		"0"			"#GameUI_Achievement_Unlocked"
			//		"1"			"#GameUI_AspectNormal"
			//		"2"			"#GameUI_AspectWide16x9"
			//		"3"			"#GameUI_AspectWide16x10"
			//	}
			//}

			"AdvancedHeader"
			{
				"text"			"#GameUI_AdvancedNoEllipsis"
				"type"			"headeryheader"
				"advanced"		"1"
			}

			"ModelDetail"
			{
				"text"			"#GameUI_Model_Detail"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"r_rootlod"

				"options"
				{
					"2"	"#gameui_low"
					"1"	"#gameui_medium"
					"0"	"#gameui_high"
				}
			}

			"TextureDetail"
			{
				"text"			"#GameUI_Texture_Detail"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_picmip"

				"options"
				{
					"2"		"#gameui_low"
					"1"		"#gameui_medium"
					"0"		"#gameui_high"
					"-1"	"#gameui_ultra"
				}
			}

			"ShaderDetail"
			{
				"text"			"#GameUI_Shader_Detail"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_reducefillrate"

				"options"
				{
					"1"		"#gameui_low"
					"0"		"#gameui_high"
				}
			}

			"WaterDetail"
			{
				"text"			"#GameUI_Water_Detail"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"_gamepadui_water_detail"

				"options"
				{
					"0"		"#gameui_noreflections"
					"1"		"#gameui_reflectonlyworld"
					"2"		"#gameui_reflectall"
				}
			}

			"ShadowDetail"
			{
				"text"			"#GameUI_Shadow_Detail"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"_gamepadui_shadow_detail"

				"options"
				{
					"0"		"#gameui_low"
					"1"		"#gameui_medium"
					"2"		"#gameui_high"
				}
			}

			"ColorCorrection"
			{
				"text"			"#GameUI_Color_Correction"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_colorcorrection"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
				
				// E:Z2
				"confirm"
				{
					"0"		"#gameui_confirm_disable_cc"
				}
			}

			"Antialiasing"
			{
				"text"			"#GameUI_Antialiasing_Mode"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"_gamepadui_antialiasing"

				"options_from"	"antialiasing"
			}

			"FilteringMode"
			{
				"text"			"#GameUI_Filtering_Mode"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_forceaniso"

				"options"
				{
					"0"		"#GameUI_Bilinear"
					"1"		"#GameUI_Trilinear"
					"2"		"#GameUI_Anisotropic2X"
					"4"		"#GameUI_Anisotropic4X"
					"8"		"#GameUI_Anisotropic8X"
					"16"	"#GameUI_Anisotropic16X"
				}
			}

			"VerticalSync"
			{
				"text"			"#GameUI_Wait_For_VSync"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_vsync"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}

			"MotionBlur"
			{
				"text"			"#GameUI_MotionBlur"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_motion_blur_enabled"

				"options"
				{
					"0"		"#gameui_disabled"
					"1"		"#gameui_enabled"
				}
			}


			"FOV"
			{
				"text"			"#GameUI_FOV"
				"type"			"slideyslide"
				"convar"		"fov_desired"
				"advanced"		"1"

				"min"			"75"
				"max"			"120"
				"step"			"5"
				"mouse_step"	"1" // NEW

				"textprecision" "0"
			}

			"MulticoreRendering"
			{
				"text"			"#GameUI_MulticoreRendering"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_queue_mode"

				"options"
				{
					"0"		"#gameui_disabled"
					"-1"	"#gameui_enabled"
				}
			}

			"HDR"
			{
				"text"			"#GameUI_HDR"
				"type"			"wheelywheel"
				"advanced"		"1"
				"convar"		"mat_hdr_level"

				"options"
				{
					"0"		"#GameUI_hdr_level0"
					"1"		"#GameUI_hdr_level1"
					"2"		"#GameUI_hdr_level2"
				}
			}
		}
	}
	"SteamInput"
	{
		"title"			"Steam Input"
		"alternating"   "0"
		"items"
		{
			"OpenSteamInput"
			{
				"text"			"#Deck_Open_Steam_Input"
				"type"			"button"
				"command"		"open_steaminput"
			}
			
			//"MiscHeader"
			//{
			//	"text"			"#Valve_Miscellaneous_Title"
			//	"type"			"headeryheader"
			//}
			//
			//"RestartSteamInput"
			//{
			//	"text"			"#GameUI_RestartSteamInput"
			//	"type"			"button"
			//	"command"		"cmd si_restart"
			//}
			//
			//"UseGlyphs"
			//{
			//	"text"			"#GameUI_UseGlyphs"
			//	"description"	"#GameUI_UseGlyphs_Desc"
			//	"type"			"wheelywheel"
			//	"convar"		"si_use_glyphs"
			//
			//	"options"
			//	{
			//		"0"		"#gameui_no"
			//		"1"		"#gameui_yes"
			//	}
			//}
			//
			//"ForceGlyphController"
			//{
			//	"text"			"#GameUI_ForceGlyphController"
			//	"description"	"#GameUI_ForceGlyphController_Desc"
			//	"type"			"wheelywheel"
			//	"convar"		"si_force_glyph_controller"
			//
			//	"options"
			//	{
			//		"-1"		"#GameUI_None"
			//		"1"			"Steam Controller"
			//		"2"			"Xbox 360 Controller"
			//		"3"			"Xbox One Controller"
			//		"5"			"PS4 Controller"
			//		"8"			"Switch Joy-Con Pair"
			//		"9"			"Switch Joy-Con Single"
			//		"10"		"Switch Pro Controller"
			//		"12"		"PS3 Controller"
			//		"13"		"PS5 Controller"
			//		"14"		"Steam Deck Controller"
			//	}
			//}
		}
	}
}