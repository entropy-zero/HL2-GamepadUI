#include "gamepadui_interface.h"
#include "gamepadui_image.h"
#include "gamepadui_util.h"
#include "gamepadui_frame.h"
#include "gamepadui_scroll.h"
#include "gamepadui_genericconfirmation.h"
#include "gamepadui_textentry.h"
#include "gamepadui_combobox.h"

#include "ienginevgui.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include <vgui/ISystem.h>
#include <vgui/IInput.h>
#include <time.h>

#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ScrollBar.h"
#include "vgui_controls/FileOpenDialog.h"
#include "vgui_controls/DirectorySelectDialog.h"

#include "KeyValues.h"
#include "filesystem.h"
#include "tier1/fmtstr.h"

#include "tier0/memdbgon.h"

static const int g_nTagsGridRowCount = 3;
static const int g_nTagsGridColumnCount = 3;

//
// Workshop tags, displayed on a grid.
// Put NULL to skip grid cells.
//
// Current layout supports 5x3 tags,
// adding more will require changing the layout or implementing a scrollbar on the grid.
//
const char *g_ppWorkshopTags[ g_nTagsGridRowCount ][ g_nTagsGridColumnCount ] =
{
	{ "Custom Map", "Campaign Addon / Mod", "Misc Map" },
	{ "Weapon", "NPC", "Item" },
	{ "Sound", "Script", "Misc Asset" },
};


using namespace vgui;

const char *GetResultDesc( EResult eResult )
{
	switch ( eResult )
	{
	case k_EResultOK:
	{
		return "The operation completed successfully.";
	}
	case k_EResultFail:
	{
		return "Generic failure.";
	}
	case k_EResultNoConnection:
	{
		return "Failed network connection.";
	}
	case k_EResultInsufficientPrivilege:
	{
		return "The user is currently restricted from uploading content due to a hub ban, account lock, or community ban. They would need to contact Steam Support.";
	}
	case k_EResultBanned:
	{
		return "The user doesn't have permission to upload content to this hub because they have an active VAC or Game ban.";
	}
	case k_EResultTimeout:
	{
		return "The operation took longer than expected. Have the user retry the creation process.";
	}
	case k_EResultNotLoggedOn:
	{
		return "The user is not currently logged into Steam.";
	}
	case k_EResultServiceUnavailable:
	{
		return "The workshop server hosting the content is having issues - have the user retry.";
	}
	case k_EResultInvalidParam:
	{
		return "One of the submission fields contains something not being accepted by that field.";
	}
	case k_EResultAccessDenied:
	{
		return "There was a problem trying to save the title and description. Access was denied.";
	}
	case k_EResultLimitExceeded:
	{
		return "The user has exceeded their Steam Cloud quota. Have them remove some items and try again.";
	}
	case k_EResultFileNotFound:
	{
		return "The uploaded file could not be found.";
	}
	case k_EResultDuplicateRequest:
	{
		return "The file was already successfully uploaded. The user just needs to refresh.";
	}
	case k_EResultDuplicateName:
	{
		return "The user already has a Steam Workshop item with that name.";
	}
	case k_EResultServiceReadOnly:
	{
		return "Due to a recent password or email change, the user is not allowed to upload new content. Usually this restriction will expire in 5 days, but can last up to 30 days if the account has been inactive recently.";
	}
	case k_EResultIOFailure:
	{
		return "Generic IO failure.";
	}
	case k_EResultDiskFull:
	{
		return "Operation canceled - not enough disk space.";
	}
	default: return "";
	}
}


enum
{
	k_ERemoteStoragePublishedFileVisibilityUnlisted_149 = 3
};

class GamepadUIAddonButton;
class GamepadUIAddonEditPanel;


#define GAMEPADUI_WORKSHOP_RESOURCE_FOLDER GAMEPADUI_RESOURCE_FOLDER "workshop_publish" CORRECT_PATH_SEPARATOR_S
#define GAMEPADUI_ADDON_BUTTON_SCHEME GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemeaddonbutton.res"
#define GAMEPADUI_CREATE_ADDON_SCHEME GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemecreateaddonbutton.res"

#define NUM_HEADERS 5

#define HEADER_TITLE 0
#define HEADER_LAST_UPDATED 1
#define HEADER_DATE_CREATED 2
#define HEADER_VISIBILITY 3
#define HEADER_ID 4

ConVar gamepadui_force_workshop_publish_state("gamepadui_force_workshop_publish_state", "-1");

class GamepadUIWorkshopPublishPanel : public GamepadUIFrame
{
    DECLARE_CLASS_SIMPLE( GamepadUIWorkshopPublishPanel, GamepadUIFrame );

public:
    GamepadUIWorkshopPublishPanel( vgui::Panel *pParent, const char* pPanelName );
	~GamepadUIWorkshopPublishPanel();

	static GamepadUIWorkshopPublishPanel *GetInstance()
	{
		return s_pWorkshopPublishPanel;
	}

    void UpdateGradients();

    void OnThink() OVERRIDE;
	void Paint() OVERRIDE;
    void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
    void OnCommand( char const* pCommand ) OVERRIDE;

    MESSAGE_FUNC_HANDLE( OnGamepadUIButtonNavigatedTo, "OnGamepadUIButtonNavigatedTo", button );

    void LayoutAddonHeaders();
    void LayoutAddonButtons();

	void SortColumn( int nHeader );
	int GetHeaderOffset( int nHeader );

    void PaintSpinner( int x0, int y0, int x1, int y1 )
    {
		vgui::surface()->DrawSetColor( m_colSpinnerImage );
        vgui::surface()->DrawSetTexture( m_SpinnerImage );

        int nNumFrames = vgui::surface()->GetTextureNumFrames( m_SpinnerImage );
        if (nNumFrames > 1)
        {
            static unsigned int nFrameCache;
            vgui::surface()->DrawSetTextureFrame( m_SpinnerImage, (Plat_MSTime() / 10) % nNumFrames, &nFrameCache );
        }

        vgui::surface()->DrawTexturedRect( x0, y0, x1, y1 );
        vgui::surface()->DrawSetTexture( 0 );
    }

    void OnMouseWheeled( int delta ) OVERRIDE;

    
public:
	void QueryItem( PublishedFileId_t );
	bool QueryAll();
	void CreateItem();
	void SubmitItemUpdate( PublishedFileId_t );
	void DeleteItem( PublishedFileId_t );

	UGCUpdateHandle_t GetItemUpdate() const { return m_hItemUpdate; }

	void Refresh()
	{
        m_pAddonButtons.PurgeAndDeleteElements();
		m_nQueryPage = 1;

		delete m_pNewAddonButton;
		m_pNewAddonButton = NULL;

		if ( QueryAll() )
		{
			m_nQueryTime = system()->GetTimeMillis();
            m_iState = State_Refreshing;

            FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select;
            SetFooterButtons( buttons, FooterButtons::Select );
		}
	}

private:
    GamepadUIButton *m_pHeaderButtons[NUM_HEADERS];
    CUtlVector< GamepadUIAddonButton* > m_pAddonButtons;
	GamepadUIButton *m_pNewAddonButton = NULL;

	// The last sorted column
	int m_nSortedColumn = HEADER_ID;

    GamepadUIScrollState m_ScrollState;

    GAMEPADUI_PANEL_PROPERTY( float, m_AddonOffsetX, "WorkshopPublish.OffsetX", "0", SchemeValueTypes::ProportionalFloat );
    GAMEPADUI_PANEL_PROPERTY( float, m_AddonOffsetY, "WorkshopPublish.OffsetY", "0", SchemeValueTypes::ProportionalFloat );
    GAMEPADUI_PANEL_PROPERTY( float, m_AddonSpacing, "WorkshopPublish.Spacing", "0", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_flAddonFade, "WorkshopPublish.Fade", "80", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_flHeaderWidth, "WorkshopPublish.Header.Width", "648", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_flHeaderSpacing, "WorkshopPublish.Header.Spacing", "8", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_flVertDividerThick, "WorkshopPublish.Divider.Vert.Thickness", "2", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_flHorzDividerThick, "WorkshopPublish.Divider.Horz.Thickness", "2", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( Color, m_colSpinnerImage, "WorkshopPublish.Spinner.Color", "255 255 255 255", SchemeValueTypes::Color );
	GAMEPADUI_PANEL_PROPERTY( float, m_flSpinnerSize, "WorkshopPublish.Spinner.Size", "64", SchemeValueTypes::ProportionalFloat );

    GamepadUIAddonEditPanel *m_pItemPublishDialog;
    UGCUpdateHandle_t m_hItemUpdate;

    //DHANDLE< CModalMessageBox > m_pLoadingMessageBox;
    //DHANDLE< CDeleteMessageBox > m_pDeleteMessageBox;
    PublishedFileId_t m_nItemToDelete;

    uint32 m_nQueryPage;
    long m_nQueryTime;

	enum
	{
		State_Active,
		State_Refreshing,
		State_Error,
	};
	
	int m_iState = State_Refreshing;
	GamepadUIString m_strResultDesc;

	GamepadUIImage m_SpinnerImage;
    
private:
	void OnCreateItemResult( CreateItemResult_t *pResult, bool bIOFailure );
	void OnSteamUGCQueryCompleted( SteamUGCQueryCompleted_t *pResult, bool bIOFailure );
	void OnSubmitItemUpdateResult( SubmitItemUpdateResult_t *pResult, bool bIOFailure );
	void OnDeleteItemResult( RemoteStorageDeletePublishedFileResult_t *pResult, bool bIOFailure );
	void OnSteamUGCRequestUGCDetailsResult( SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure );
	void OnRemoteStorageDownloadUGCResult( RemoteStorageDownloadUGCResult_t *pResult, bool bIOFailure );

	CCallResult< GamepadUIWorkshopPublishPanel, CreateItemResult_t > m_CreateItemResult;
	CCallResult< GamepadUIWorkshopPublishPanel, SteamUGCQueryCompleted_t > m_SteamUGCQueryCompleted;
	CCallResult< GamepadUIWorkshopPublishPanel, SubmitItemUpdateResult_t > m_SubmitItemUpdateResult;
	CCallResult< GamepadUIWorkshopPublishPanel, RemoteStorageDeletePublishedFileResult_t > m_DeleteItemResult;
	CCallResult< GamepadUIWorkshopPublishPanel, SteamUGCRequestUGCDetailsResult_t > m_SteamUGCRequestUGCDetailsResult;
	CCallResult< GamepadUIWorkshopPublishPanel, RemoteStorageDownloadUGCResult_t > m_RemoteStorageDownloadUGCResult;

	static GamepadUIWorkshopPublishPanel *s_pWorkshopPublishPanel;
};

GamepadUIWorkshopPublishPanel *GamepadUIWorkshopPublishPanel::s_pWorkshopPublishPanel = NULL;

class GamepadUIAddonButton : public GamepadUIButton
{
    DECLARE_CLASS_SIMPLE( GamepadUIAddonButton, GamepadUIButton );

	friend class GamepadUIWorkshopPublishPanel;
public:
    GamepadUIAddonButton( vgui::Panel *pParent, vgui::Panel *pActionSignalTarget, const char *pSchemeFile, const char *pCommand, const char *pText, const char *pDescription )
        : BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
    {
        m_pWorkshopPublishParent = static_cast<GamepadUIWorkshopPublishPanel*>( pParent );
    }

    GamepadUIAddonButton( vgui::Panel *pParent, vgui::Panel *pActionSignalTarget, const char *pSchemeFile, const char *pCommand, const wchar_t *pText, const wchar_t *pDescription )
        : BaseClass( pParent, pActionSignalTarget, pSchemeFile, pCommand, pText, pDescription )
    {
        m_pWorkshopPublishParent = static_cast<GamepadUIWorkshopPublishPanel*>( pParent );
    }

    void InitAddon( PublishedFileId_t nID, uint32 nTimeUpdated, uint32 nTimeCreated, ERemoteStoragePublishedFileVisibility nVisibility, const char *pszTags )
    {
        m_nID = nID;
        m_nTimeUpdated = nTimeUpdated;
        m_nTimeCreated = nTimeCreated;
        m_nVisibility = nVisibility;

        m_strID.SetRawUTF8( CNumStr( m_nID ) );
        
		time_t t = nTimeUpdated;
		tm *date = localtime( &t );
		m_strTimeUpdated.SetRawUTF8( CFmtStrN< 32 >("%d-%02d-%02d %02d:%02d:%02d",
			date->tm_year+1900, date->tm_mon+1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec) );

		t = nTimeCreated;
		date = localtime( &t );
        m_strTimeCreated.SetRawUTF8( CFmtStrN< 32 >("%d-%02d-%02d %02d:%02d:%02d",
			date->tm_year+1900, date->tm_mon+1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec) );

		switch ( nVisibility )
		{
		case k_ERemoteStoragePublishedFileVisibilityPublic:
			m_strVisibility.SetText( "#Workshop_Public" );
			break;
		case k_ERemoteStoragePublishedFileVisibilityFriendsOnly:
			m_strVisibility.SetText( "#Workshop_FriendsOnly" );
			break;
		case k_ERemoteStoragePublishedFileVisibilityPrivate:
			m_strVisibility.SetText( "#Workshop_Private" );
			break;
		case k_ERemoteStoragePublishedFileVisibilityUnlisted_149:
			m_strVisibility.SetText( "#Workshop_Unlisted" );
			break;
		}

		m_strTags.SetRawUTF8( pszTags );
    }

    void Paint()
    {
        BaseClass::Paint();

        vgui::surface()->DrawSetTextFont( m_hTextFont );

        PaintText( m_pWorkshopPublishParent->GetHeaderOffset( HEADER_ID ), m_strID.String(), m_strID.Length());
        PaintText( m_pWorkshopPublishParent->GetHeaderOffset( HEADER_LAST_UPDATED ), m_strTimeUpdated.String(), m_strTimeUpdated.Length() );
        PaintText( m_pWorkshopPublishParent->GetHeaderOffset( HEADER_DATE_CREATED ), m_strTimeCreated.String(), m_strTimeCreated.Length() );
        PaintText( m_pWorkshopPublishParent->GetHeaderOffset( HEADER_VISIBILITY ), m_strVisibility.String(), m_strVisibility.Length() );
    }

    void PaintText( int nOffsetX, const wchar_t *pszUnicode, int nLen )
    {
        int nTextW, nTextH;
        vgui::surface()->GetTextSize( m_hTextFont, pszUnicode, nTextW, nTextH );

        int nTextY = m_flHeight / 2 - nTextH / 2 + m_flTextOffsetY;

        vgui::surface()->DrawSetTextPos( nOffsetX + m_flTextOffsetX, nTextY );
        vgui::surface()->DrawPrintText( pszUnicode, nLen );
    }

	GamepadUIString &GetStringForHeader( int nHeader )
	{
		switch (nHeader)
		{
			case HEADER_TITLE:
				return m_strButtonText;
			case HEADER_ID:
				return m_strID;
			case HEADER_LAST_UPDATED:
				return m_strTimeUpdated;
			case HEADER_DATE_CREATED:
				return m_strTimeCreated;
			case HEADER_VISIBILITY:
				return m_strVisibility;
		}

		static GamepadUIString dummy;
		return dummy;
	}

	static bool s_bSortMode;

	struct AddonSortInfo_t
	{
		AddonSortInfo_t( GamepadUIAddonButton *_b ) { pButton = _b; }
		GamepadUIAddonButton *pButton;
	};

	static int __cdecl SortTitles( const AddonSortInfo_t *pLeft, const AddonSortInfo_t *pRight )
	{
		int result = _wcsicmp( pLeft->pButton->m_strButtonText.String(), pRight->pButton->m_strButtonText.String() );
		if (s_bSortMode)
			result = -result;
		return result;
	}

	static int __cdecl SortIDs( const AddonSortInfo_t *pLeft, const AddonSortInfo_t *pRight )
	{
		int result = pLeft->pButton->m_nID - pRight->pButton->m_nID;
		if (s_bSortMode)
			result = -result;
		return result;
	}

	static int __cdecl SortTimeUpdated( const AddonSortInfo_t *pLeft, const AddonSortInfo_t *pRight )
	{
		int result = pLeft->pButton->m_nTimeUpdated - pRight->pButton->m_nTimeUpdated;
		if (s_bSortMode)
			result = -result;
		return result;
	}

	static int __cdecl SortTimeCreated( const AddonSortInfo_t *pLeft, const AddonSortInfo_t *pRight )
	{
		int result = pLeft->pButton->m_nTimeCreated - pRight->pButton->m_nTimeCreated;
		if (s_bSortMode)
			result = -result;
		return result;
	}

	static int __cdecl SortVisibility( const AddonSortInfo_t *pLeft, const AddonSortInfo_t *pRight )
	{
		int result = pLeft->pButton->m_nVisibility - pRight->pButton->m_nVisibility;
		if (s_bSortMode)
			result = -result;
		return result;
	}

private:
    PublishedFileId_t m_nID;
    uint32 m_nTimeUpdated;
    uint32 m_nTimeCreated;
    ERemoteStoragePublishedFileVisibility m_nVisibility;

    GamepadUIString m_strID;
    GamepadUIString m_strTimeUpdated;
    GamepadUIString m_strTimeCreated;
    GamepadUIString m_strVisibility;
    GamepadUIString m_strTags;

    GamepadUIWorkshopPublishPanel *m_pWorkshopPublishParent;
};

bool GamepadUIAddonButton::s_bSortMode;

class GamepadUIAddonEditPanel : public GamepadUIFrame
{
    DECLARE_CLASS_SIMPLE( GamepadUIAddonEditPanel, GamepadUIFrame );

public:
	GamepadUIAddonEditPanel( vgui::Panel *pParent, const char* pPanelName );
	~GamepadUIAddonEditPanel();

    void UpdateGradients();

	void OnThink() OVERRIDE;
	void Paint() OVERRIDE;
	void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;

	void PerformLayout() OVERRIDE;

	void PaintFieldTitle( GamepadUIString &str, int nOffsetX, int nOffsetY );
	void PaintFieldTitles();

	void OnCommand( char const *pCommand ) OVERRIDE;

	void SetPreviewImage( const char *filename );
	void SetPreviewImage( CUtlBuffer &file, const char *filename );
	void UpdateFields( const SteamUGCDetails_t& );

	MESSAGE_FUNC_CHARPTR( OnFileSelected, "FileSelected", fullpath );
	MESSAGE_FUNC_CHARPTR( OnDirectorySelected, "DirectorySelected", dir );
	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );

public:

	GamepadUITextEntry *m_pTitleInput;
	GamepadUITextEntry *m_pDescInput;
	GamepadUITextEntry *m_pChangesInput;
	GamepadUITextEntry *m_pPreviewInput;
	GamepadUITextEntry *m_pContentInput;

	GamepadUIButton *m_pContentBrowse;
	GamepadUIButton *m_pPreviewBrowse;

	GamepadUIComboBox *m_pVisibility;

	//CSimpleGrid *m_pTagsGrid;

	GamepadUIImage m_PreviewImage;
	bool m_bLoadingImage;

	bool m_bLoadingInfo = false;

private:
	uint64 m_item;
	
	GamepadUIString m_strTitleInput;
	GamepadUIString m_strDescInput;
	GamepadUIString m_strChangesInput;
	GamepadUIString m_strPreviewInput;
	GamepadUIString m_strContentInput;
	GamepadUIString m_strVisibility;
	GamepadUIString m_strTags;

	GamepadUIString m_strLoading;
	GamepadUIString m_strNoImage;


	vgui::HFont m_hFieldTitleFont;

	GAMEPADUI_PANEL_PROPERTY( float, m_ImageOffsetX, "WorkshopPublish.Addon.Image.OffsetX", "119", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_ImageOffsetY, "WorkshopPublish.Addon.Image.OffsetY", "96", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_ImageWidth, "WorkshopPublish.Addon.Image.Width", "190", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_ImageHeight, "WorkshopPublish.Addon.Image.Height", "190", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonTitleOffsetX, "WorkshopPublish.Addon.Title.OffsetX", "400", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonTitleOffsetY, "WorkshopPublish.Addon.Title.OffsetY", "100", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonDescOffsetX, "WorkshopPublish.Addon.Desc.OffsetX", "550", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonDescOffsetY, "WorkshopPublish.Addon.Desc.OffsetY", "150", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonChangesOffsetX, "WorkshopPublish.Addon.Changes.OffsetX", "550", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonChangesOffsetY, "WorkshopPublish.Addon.Changes.OffsetY", "300", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonPreviewPathOffsetX, "WorkshopPublish.Addon.PreviewPath.OffsetX", "64", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonPreviewPathOffsetY, "WorkshopPublish.Addon.PreviewPath.OffsetY", "320", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonContentPathOffsetY, "WorkshopPublish.Addon.ContentPath.OffsetY", "370", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonTagsOffsetX, "WorkshopPublish.Addon.Tags.OffsetX", "400", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonTagsOffsetY, "WorkshopPublish.Addon.Tags.OffsetY", "150", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonTagsWidth, "WorkshopPublish.Addon.Tags.Width", "100", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( float, m_AddonVisibilityOffsetX, "WorkshopPublish.Addon.Visibility.OffsetX", "400", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonVisibilityOffsetY, "WorkshopPublish.Addon.Visibility.OffsetY", "280", SchemeValueTypes::ProportionalFloat );
	GAMEPADUI_PANEL_PROPERTY( float, m_AddonVisibilityWidth, "WorkshopPublish.Addon.Visibility.Width", "100", SchemeValueTypes::ProportionalFloat );

	GAMEPADUI_PANEL_PROPERTY( Color, m_colSpinnerImage, "WorkshopPublish.Addon.Spinner.Color", "255 255 255 255", SchemeValueTypes::Color );
	GAMEPADUI_PANEL_PROPERTY( float, m_flSpinnerSize, "WorkshopPublish.Addon.Spinner.Size", "64", SchemeValueTypes::ProportionalFloat );
};


GamepadUIWorkshopPublishPanel::GamepadUIWorkshopPublishPanel( vgui::Panel *pParent, const char* PanelName ) : BaseClass( pParent, PanelName )
{
	s_pWorkshopPublishPanel = this;

    vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( GAMEPADUI_DEFAULT_PANEL_SCHEME, "SchemePanel" );
    SetScheme( hScheme );

    GetFrameTitle() = GamepadUIString( "#WorkshopMgr_BrowsePublishedFiles" ); // GameUI_GameMenu_Workshop
    FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select;
    SetFooterButtons( buttons, FooterButtons::Select );

	m_SpinnerImage.SetImage( "gamepadui/spinner" );

	m_strResultDesc.SetRawUTF8( GetResultDesc(k_EResultFail) );

    Activate();

	static const char *pszHeaderTitles[NUM_HEADERS] =		{ "#WorkshopMgr_Title",	"#WorkshopMgr_LastUpdated",	"#WorkshopMgr_DateCreated",	"#WorkshopMgr_Visibility",	"ID" };
	static const char *pszHeaderCommands[NUM_HEADERS] =		{ "header_title",		"header_lastupdated",		"header_datecreated",		"header_visibility",		"header_id" };

	for (int i = 0; i < NUM_HEADERS; i++)
	{
		m_pHeaderButtons[i] = new GamepadUIButton(
		                this, this,
		                GAMEPADUI_RESOURCE_FOLDER "schemetab.res",
		                pszHeaderCommands[i],
		                pszHeaderTitles[i], "");
		            m_pHeaderButtons[i]->SetZPos(50);
	}

    UpdateGradients();

	Refresh();
}

GamepadUIWorkshopPublishPanel::~GamepadUIWorkshopPublishPanel()
{
	m_pAddonButtons.PurgeAndDeleteElements();

	delete m_pNewAddonButton;
	m_pNewAddonButton = NULL;
}

void GamepadUIWorkshopPublishPanel::UpdateGradients()
{
    const float flTime = GamepadUI::GetInstance().GetTime();
    GamepadUI::GetInstance().GetGradientHelper()->ResetTargets( flTime );
    GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient( GradientSide::Up, { 1.0f, 1.0f }, flTime );
    GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient( GradientSide::Down, { 1.0f, 0.5f }, flTime );
}

void GamepadUIWorkshopPublishPanel::SortColumn( int nHeader )
{
	CUtlVector<GamepadUIAddonButton::AddonSortInfo_t> pSortButtons;
	for (int i = 0; i < m_pAddonButtons.Count(); i++)
		pSortButtons.AddToTail( GamepadUIAddonButton::AddonSortInfo_t( m_pAddonButtons[i] ) );

	if (m_nSortedColumn != nHeader)
	{
		m_nSortedColumn = nHeader;
		GamepadUIAddonButton::s_bSortMode = false;
	}
	else
	{
		GamepadUIAddonButton::s_bSortMode = !GamepadUIAddonButton::s_bSortMode;
	}

	switch (nHeader)
	{
		case HEADER_TITLE:
			pSortButtons.Sort( &GamepadUIAddonButton::SortTitles );
			break;
		case HEADER_LAST_UPDATED:
			pSortButtons.Sort( &GamepadUIAddonButton::SortTimeUpdated );
			break;
		case HEADER_DATE_CREATED:
			pSortButtons.Sort( &GamepadUIAddonButton::SortTimeCreated );
			break;
		case HEADER_VISIBILITY:
			pSortButtons.Sort( &GamepadUIAddonButton::SortVisibility );
			break;
		case HEADER_ID:
			pSortButtons.Sort( &GamepadUIAddonButton::SortIDs );
			break;
	}

	m_pAddonButtons.RemoveAll();
	for (int i = 0; i < pSortButtons.Count(); i++)
		m_pAddonButtons.AddToTail( pSortButtons[i].pButton );
}

int GamepadUIWorkshopPublishPanel::GetHeaderOffset( int nHeader )
{
	int nOffset = 0;
	for (int i = 0; i < nHeader; i++)
		nOffset += m_pHeaderButtons[i]->GetWide();

	return nOffset;
}

void GamepadUIWorkshopPublishPanel::LayoutAddonHeaders()
{
	int nHeaderSizes[NUM_HEADERS];
	memset( nHeaderSizes, 0, sizeof( int ) * NUM_HEADERS );

	// Default header space
	for ( int i = 0; i < NUM_HEADERS; i++ )
    {
		int nTextW, nTextH;
		vgui::surface()->GetTextSize( m_pHeaderButtons[i]->m_hTextFont, m_pHeaderButtons[i]->GetButtonText().String(), nTextW, nTextH);
		
		nTextW += m_flHeaderSpacing;
		if (nTextW > nHeaderSizes[i])
			nHeaderSizes[i] = nTextW;
    }

	// Fit headers to the buttons
    for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
    {
		for (int h = 0; h < NUM_HEADERS; h++)
		{
			int nTextW, nTextH;
			vgui::surface()->GetTextSize( m_pAddonButtons[i]->m_hTextFont, m_pAddonButtons[i]->GetStringForHeader(h).String(), nTextW, nTextH );
			
			nTextW += m_flHeaderSpacing;
			if (nTextW > nHeaderSizes[h])
				nHeaderSizes[h] = nTextW;
		}
    }

	int nTotalHeaderSizes = 0;
	for (int h = 0; h < NUM_HEADERS; h++)
		nTotalHeaderSizes += nHeaderSizes[h];

	if (nTotalHeaderSizes != m_flHeaderWidth)
	{
		// Resize each header
		float flRatio = (m_flHeaderWidth) / ((float)nTotalHeaderSizes);
		for (int h = 0; h < NUM_HEADERS; h++)
			nHeaderSizes[h] = ((float)nHeaderSizes[h]) * flRatio;
	}

	for (int h = 0; h < NUM_HEADERS; h++)
	{
		// Set the header sizes
		m_pHeaderButtons[h]->SetWide( nHeaderSizes[h] );
		m_pHeaderButtons[h]->m_flWidth = nHeaderSizes[h];
		m_pHeaderButtons[h]->m_flWidthAnimationValue[ButtonState::Out] = nHeaderSizes[h];
		m_pHeaderButtons[h]->m_flWidthAnimationValue[ButtonState::Over] = nHeaderSizes[h];
		m_pHeaderButtons[h]->m_flWidthAnimationValue[ButtonState::Pressed] = nHeaderSizes[h];
	}
}

void GamepadUIWorkshopPublishPanel::LayoutAddonButtons()
{
    int nParentW, nParentH;
	GetParent()->GetSize( nParentW, nParentH );

    int x = m_AddonOffsetX;
    int y = 0;

	// Calculate header buttons

	for (int i = 0; i < NUM_HEADERS; i++)
    {
        GamepadUIButton *header = m_pHeaderButtons[ i ];
        header->SetPos( x, m_AddonOffsetY );
        header->SetVisible( true );
        x += header->GetWide();
        y = m_AddonOffsetY + header->GetTall();
    }

    int previousSizes = 0;
    int buttonWide = 0;
    for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
    {
		GamepadUIButton *pButton = m_pAddonButtons[i];

        int fade = 255;

        int buttonY = y;
        int buttonX = m_AddonOffsetX;
        {
            buttonY = y + previousSizes - m_ScrollState.GetScrollProgress();
            if ( buttonY < y )
                fade = RemapValClamped( y - buttonY, abs(m_flAddonFade - pButton->GetTall()), 0, 0, 255 );
            else if ( buttonY > ( nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight - m_flAddonFade ) )
                fade = RemapValClamped( ( nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight ) - ( buttonY + pButton->GetTall() ), 0, m_flAddonFade, 0, 255 );
            if ( ( pButton->HasFocus() && pButton->IsEnabled() ) && fade != 0 )
                fade = 255;
        }

        pButton->SetAlpha(fade);
        pButton->SetVisible(true);
        pButton->SetPos( buttonX, buttonY );

        if ( pButton->IsEnabled() && pButton->IsVisible() && fade /*&& m_Tabs[nActiveTab].bAlternating*/)
        {
            buttonWide = pButton->GetWide();
            if ( i % 2 )
                vgui::surface()->DrawSetColor( Color( 0, 0, 0, ( 20 * Min( 255, fade + 127 ) ) / 255 ) );
            else
                vgui::surface()->DrawSetColor( Color( fade, fade, fade, fade > 64 ? 1 : 0 ) );

            vgui::surface()->DrawFilledRect( buttonX, buttonY, buttonX + buttonWide, buttonY + pButton->GetTall() );
        }

        previousSizes += pButton->GetTall();
    }

    int yMax = 0;
    {
        int nScrollCount = m_pAddonButtons.Count() - 7; // 8
        for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
        {
            GamepadUIButton *pButton = m_pAddonButtons[ i ];
            if ( i < nScrollCount )
                yMax += pButton->GetTall();
        }
    }

	// The button for adding a new addon
	if (m_pNewAddonButton)
	{
		int fade = 255;

        int buttonY = y;
        int buttonX = m_AddonOffsetX;
        {
            buttonY = y + previousSizes - m_ScrollState.GetScrollProgress();
            if ( buttonY < y )
                fade = RemapValClamped( y - buttonY, abs(m_flAddonFade - m_pNewAddonButton->GetTall()), 0, 0, 255 );
            else if ( buttonY > ( nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight - m_flAddonFade ) )
                fade = RemapValClamped( ( nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight ) - ( buttonY + m_pNewAddonButton->GetTall() ), 0, m_flAddonFade, 0, 255 );
            if ( (m_pNewAddonButton->HasFocus() && m_pNewAddonButton->IsEnabled() ) && fade != 0 )
                fade = 255;
        }

        m_pNewAddonButton->SetAlpha(fade);
        m_pNewAddonButton->SetVisible(true);
        m_pNewAddonButton->SetPos( buttonX, buttonY );

        if (m_pNewAddonButton->IsEnabled() && m_pNewAddonButton->IsVisible() && fade /*&& m_Tabs[nActiveTab].bAlternating*/)
        {
            buttonWide = m_pNewAddonButton->GetWide();
            vgui::surface()->DrawSetColor( Color( 0, 0, 0, ( 20 * Min( 255, fade + 127 ) ) / 255 ) );
            vgui::surface()->DrawFilledRectFade( buttonX, buttonY, buttonX + buttonWide, buttonY + m_pNewAddonButton->GetTall(), 255, 0, true );
        }

		yMax += m_pNewAddonButton->GetTall();
	}

	m_ScrollState.UpdateScrollBounds( 0.0f, yMax );

    //if ( yMax != 0 )
    //{
    //    vgui::surface()->DrawSetColor( Color( 255, 255, 255, 200 ) );
    //    int scrollbarY = RemapValClamped( m_ScrollState.GetScrollProgress(), 0, yMax, y, nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight - m_flScrollBarHeight );
    //    vgui::surface()->DrawFilledRect( m_AddonOffsetX + m_flScrollBarOffsetX, scrollbarY, m_AddonOffsetX + m_flScrollBarOffsetX + m_flScrollBarWidth, scrollbarY + m_flScrollBarHeight );
    //}

    m_ScrollState.UpdateScrolling( 2.0f, GamepadUI::GetInstance().GetTime() );
}

void GamepadUIWorkshopPublishPanel::OnThink()
{
    BaseClass::OnThink();

	LayoutAddonHeaders();
	LayoutAddonButtons();
}

void GamepadUIWorkshopPublishPanel::Paint()
{
	BaseClass::Paint();

	int iState = m_iState;
	if (gamepadui_force_workshop_publish_state.GetInt() != -1)
		iState = gamepadui_force_workshop_publish_state.GetInt();

	switch ( iState )
	{
		case State_Active:
			{
				int nParentW, nParentH;
				GetParent()->GetSize( nParentW, nParentH );

				int nBottom = ( nParentH - m_flFooterButtonsOffsetY - m_nFooterButtonHeight - m_flAddonFade );

				vgui::surface()->DrawSetColor( Color( 255, 255, 255, 200 ) );

				// Add vertical dividers
				for (int h = 0; h < NUM_HEADERS; h++)
				{
					int nX, nY;
					m_pHeaderButtons[h]->GetPos( nX, nY );

					vgui::surface()->DrawFilledRect( nX - (m_flVertDividerThick/2), nY, nX + (m_flVertDividerThick/2), nBottom );
					vgui::surface()->DrawFilledRectFade( nX - (m_flVertDividerThick/2), nBottom, nX + (m_flVertDividerThick/2), nBottom + m_flAddonFade, 255, 0, false );

					if (h == NUM_HEADERS-1)
					{
						// Closing divider
						nX += m_pHeaderButtons[h]->GetWide();

						vgui::surface()->DrawFilledRect( nX - (m_flVertDividerThick/2), nY, nX + (m_flVertDividerThick/2), nBottom );
						vgui::surface()->DrawFilledRectFade( nX - (m_flVertDividerThick/2), nBottom, nX + (m_flVertDividerThick/2), nBottom + m_flAddonFade, 255, 0, false );
					}
				}

				// Add horizontal dividers
				for ( int i = 1; i < m_pAddonButtons.Count(); i++ )
				{
					int nX, nY;
					m_pAddonButtons[i]->GetPos( nX, nY );

					vgui::surface()->DrawSetColor( Color( 255, 255, 255, m_pAddonButtons[i]->GetAlpha() ) );
					vgui::surface()->DrawFilledRect( nX, nY - (m_flHorzDividerThick/2), nX + m_flHeaderWidth, nY + (m_flVertDividerThick/2) );
				}

				// Add horizontal divider to add button
				if (m_pNewAddonButton)
				{
					int nX, nY;
					m_pNewAddonButton->GetPos( nX, nY );

					vgui::surface()->DrawSetColor( Color( 255, 255, 255, m_pNewAddonButton->GetAlpha() ) );
					vgui::surface()->DrawFilledRect( nX, nY - (m_flHorzDividerThick/2), nX + m_pNewAddonButton->GetWide(), nY + (m_flVertDividerThick/2) );
				}
				
				// Add horizontal dividers to the headers
				if (m_pHeaderButtons[0] && m_pAddonButtons[0])
				{
					int nX, nY;
					m_pHeaderButtons[0]->GetPos( nX, nY );

					vgui::surface()->DrawSetColor( Color( 255, 255, 255, 200 ) );
					
					vgui::surface()->DrawFilledRect( nX, nY - (m_flHorzDividerThick/2), nX + m_pAddonButtons[0]->GetWide(), nY + (m_flVertDividerThick/2) );
					nY += m_pHeaderButtons[0]->GetTall();
					vgui::surface()->DrawFilledRect( nX, nY - (m_flHorzDividerThick/2), nX + m_pAddonButtons[0]->GetWide(), nY + (m_flVertDividerThick/2) );
				}
			}
			break;
		case State_Refreshing:
			{
				// Show spinner if we're refreshing
				int nX = m_AddonOffsetX + (m_flHeaderWidth/2);
				int nY = m_AddonOffsetY + (GetTall()/3);
				nX -= (m_flSpinnerSize/2);
				nY -= (m_flSpinnerSize/2);

				PaintSpinner( nX, nY, nX + m_flSpinnerSize, nY + m_flSpinnerSize );
			}
			break;
		case State_Error:
			{
				// Show error text
				int nTextW, nTextH;
				vgui::surface()->GetTextSize( m_hGenericFont, m_strResultDesc.String(), nTextW, nTextH );
				
				int nX = m_AddonOffsetX + (m_flHeaderWidth/2);
				int nY = m_AddonOffsetY + (GetTall()/3);

				nX -= (nTextW/2);
				nY -= (nTextH/2);

				vgui::surface()->DrawSetTextFont( m_hGenericFont );
				DrawPrintWrappedText( m_hGenericFont, nX, nY, m_strResultDesc.String(), m_strResultDesc.Length(), m_flHeaderWidth, true );
			}
			break;
	}
}

void GamepadUIWorkshopPublishPanel::ApplySchemeSettings( vgui::IScheme* pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );

    if (GamepadUI::GetInstance().GetScreenRatio() != 1.0f)
    {
        float flScreenRatio = GamepadUI::GetInstance().GetScreenRatio();
        m_AddonOffsetX *= (flScreenRatio*flScreenRatio);
    }
}

void GamepadUIWorkshopPublishPanel::OnGamepadUIButtonNavigatedTo( vgui::VPANEL button )
{
    GamepadUIButton *pButton = dynamic_cast< GamepadUIButton * >( vgui::ipanel()->GetPanel( button, GetModuleName() ) );
    if ( pButton )
    {
        int nParentW, nParentH;
	    GetParent()->GetSize( nParentW, nParentH );

        int nX, nY;
        pButton->GetPos( nX, nY );
        if ( nX + pButton->m_flWidth > nParentW || nX < 0 )
        {
            int nTargetX = pButton->GetPriority() * (pButton->m_flWidth + m_AddonSpacing);

            if ( nX < nParentW / 2 )
            {
                nTargetX += nParentW - m_AddonOffsetX;
                // Add a bit of spacing to make this more visually appealing :)
                nTargetX -= m_AddonSpacing;
            }
            else
            {
                nTargetX += pButton->m_flWidth;
                // Add a bit of spacing to make this more visually appealing :)
                nTargetX += (pButton->m_flWidth / 2) + m_AddonSpacing;
            }


            m_ScrollState.SetScrollTarget( nTargetX - ( nParentW - m_AddonOffsetX ), GamepadUI::GetInstance().GetTime() );
        }
    }
}

void GamepadUIWorkshopPublishPanel::QueryItem( PublishedFileId_t item )
{
	// NOTE: ISteamUGC::RequestUGCDetails was deprecated in favour of ISteamUGC::CreateQueryUGCDetailsRequest
	SteamAPICall_t hSteamAPICall = GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->RequestUGCDetails(item, 5);
	if ( hSteamAPICall == k_uAPICallInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::QueryItem() Steam API call failed\n" );
		return;
	}

	m_SteamUGCRequestUGCDetailsResult.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnSteamUGCRequestUGCDetailsResult );

	m_pItemPublishDialog->m_bLoadingInfo = true;
}

void GamepadUIWorkshopPublishPanel::OnSteamUGCRequestUGCDetailsResult( SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure )
{
	Assert( !bIOFailure );

	const SteamUGCDetails_t &details = pResult->m_details;
	GamepadUIAddonButton *pAddonButton = NULL;

	for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
	{
		if ( details.m_nPublishedFileId == m_pAddonButtons[i]->m_nID)
		{
			pAddonButton = m_pAddonButtons[i];
			break;
		}
	}

	if ( !pAddonButton )
	{
		// Cannot fail, it was requested via an item in the list
		Assert(0);
		Warning( "Requested item (%llu) was not found in the list\n", details.m_nPublishedFileId );
		m_pItemPublishDialog->Close();
		return;
	}

	Assert( m_pItemPublishDialog );
	Assert( m_pLoadingMessageBox );

	m_pItemPublishDialog->m_bLoadingInfo = false;
	m_pItemPublishDialog->UpdateFields( details );

	// Don't set downloaded image if preview was already set
	if ( !m_pItemPublishDialog->m_pPreviewInput->GetTextLength() )
	{
		SteamAPICall_t hSteamAPICall = GamepadUI::GetInstance().GetSteamAPIContext()->SteamRemoteStorage()->UGCDownload( details.m_hPreviewFile, 1 );
		if ( hSteamAPICall == k_uAPICallInvalid )
		{
			Warning( "GamepadUIWorkshopPublishPanel::QueryItem() Steam API call failed\n" );
		}
		else
		{
			m_RemoteStorageDownloadUGCResult.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnRemoteStorageDownloadUGCResult );
			m_pItemPublishDialog->m_bLoadingImage = true;
		}
	}
}

void GamepadUIWorkshopPublishPanel::OnRemoteStorageDownloadUGCResult( RemoteStorageDownloadUGCResult_t *pResult, bool bIOFailure )
{
	Assert( !bIOFailure );

	m_pItemPublishDialog->m_bLoadingImage = false;

	if ( pResult->m_eResult != k_EResultOK )
	{
		Warning( "GamepadUIWorkshopPublishPanel::OnRemoteStorageDownloadUGCResult() (%i) %s\n", pResult->m_eResult, GetResultDesc( pResult->m_eResult ) );
		return;
	}

	DevMsg( "Downloaded %s '%s'\n", V_pretifymem( pResult->m_nSizeInBytes, 2, true ), pResult->m_pchFileName );

	if ( !m_pItemPublishDialog )
		return; // panel closed before download

	Assert( m_pItemPublishDialog->m_item );

	// Don't set downloaded image if preview was already set
	if ( !m_pItemPublishDialog->m_pPreviewInput->GetTextLength() )
	{
		byte *pImage = (byte*)stackalloc( pResult->m_nSizeInBytes * sizeof(byte) );

		GamepadUI::GetInstance().GetSteamAPIContext()->SteamRemoteStorage()->UGCRead( pResult->m_hFile, pImage, pResult->m_nSizeInBytes, 0, k_EUGCRead_ContinueReadingUntilFinished );

		CUtlBuffer buf( pImage, pResult->m_nSizeInBytes, CUtlBuffer::READ_ONLY );
		m_pItemPublishDialog->SetPreviewImage( buf, pResult->m_pchFileName );
	}
}

bool GamepadUIWorkshopPublishPanel::QueryAll()
{
	UGCQueryHandle_t hUGCQuery = GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->CreateQueryUserUGCRequest( GamepadUI::GetInstance().GetSteamAPIContext()->SteamUser()->GetSteamID().GetAccountID(),
		k_EUserUGCList_Published,
		k_EUGCMatchingUGCType_Items,
		k_EUserUGCListSortOrder_LastUpdatedDesc,
		GamepadUI::GetInstance().GetSteamAPIContext()->SteamUtils()->GetAppID(),
		GamepadUI::GetInstance().GetSteamAPIContext()->SteamUtils()->GetAppID(),
		m_nQueryPage );

	if ( hUGCQuery == k_UGCQueryHandleInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::Refresh() k_UGCQueryHandleInvalid\n" );
		return false;
	}

	SteamAPICall_t hSteamAPICall = GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->SendQueryUGCRequest( hUGCQuery );
	if ( hSteamAPICall == k_uAPICallInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::Refresh() Steam API call failed\n" );
		GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->ReleaseQueryUGCRequest( hUGCQuery );
		return false;
	}

	m_SteamUGCQueryCompleted.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnSteamUGCQueryCompleted );
	return true;
}

void GamepadUIWorkshopPublishPanel::OnSteamUGCQueryCompleted( SteamUGCQueryCompleted_t *pResult, bool bIOFailure )
{
	Assert( !bIOFailure );

	if ( pResult->m_eResult != k_EResultOK )
	{
		const char *pszResultDesc = GetResultDesc( pResult->m_eResult );

		Warning( "GamepadUIWorkshopPublishPanel::OnSteamUGCQueryCompleted() (%i) %s\n", pResult->m_eResult, pszResultDesc );
		GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->ReleaseQueryUGCRequest( pResult->m_handle );

		m_iState = State_Error;
		m_strResultDesc.SetRawUTF8( pszResultDesc );

		FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select | FooterButtons::Refresh;
		SetFooterButtons( buttons, FooterButtons::Select );
		return;
	}

	for ( uint32 i = 0; i < pResult->m_unNumResultsReturned; ++i )
	{
		SteamUGCDetails_t details;

		if ( !GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->GetQueryUGCResult( pResult->m_handle, i, &details ) )
		{
			Warning( "GetQueryUGCResult() failed\n" );
			continue;
		}

		if ( details.m_eResult != k_EResultOK )
		{
			Warning( "GetQueryUGCResult() failed [%i]\n", details.m_eResult );
			continue;
		}

		char command[32];
		Q_snprintf( command, sizeof( command ), "addon_%llu", details.m_nPublishedFileId );

		GamepadUIAddonButton *pAddonButton = new GamepadUIAddonButton(
            this, this,
			GAMEPADUI_ADDON_BUTTON_SCHEME, command,
			details.m_rgchTitle, "" );
		pAddonButton->SetPriority( i );
		pAddonButton->SetForwardToParent( true );

		if (details.m_rgchTitle[0] == '\0')
		{
			// Edge case: Blank name
			pAddonButton->m_strButtonText.SetText( "#GameUI_None" );
			pAddonButton->m_colTextColor = Color( 128, 128, 128, 150 );
		}
		else if (details.m_rgchTitle[strlen(details.m_rgchTitle)-1] == '\n')
		{
			// Edge case: Newline
			char szNewName[128];
			V_strncpy( szNewName, details.m_rgchTitle, strlen(details.m_rgchTitle) );
			pAddonButton->m_strButtonText.SetRawUTF8( szNewName );
		}

		pAddonButton->InitAddon( details.m_nPublishedFileId, details.m_rtimeUpdated, details.m_rtimeCreated, details.m_eVisibility, details.m_rgchTags );

		m_pAddonButtons.AddToTail( pAddonButton );
	}

	// Create addon button
	{
		m_pNewAddonButton = new GamepadUIButton(
		        this, this,
				GAMEPADUI_CREATE_ADDON_SCHEME, "new_addon",
			"#GameUI_Add", "" );
			m_pNewAddonButton->SetPriority( m_pAddonButtons.Count() );
			m_pNewAddonButton->SetForwardToParent( true );
	}

    if ( m_pAddonButtons.Count() > 0 )
	{
		m_pAddonButtons[0]->NavigateTo();
		
		m_pNewAddonButton->SetNavUp( m_pAddonButtons[m_pAddonButtons.Count()-1] );
		m_pAddonButtons[m_pAddonButtons.Count()-1]->SetNavDown( m_pNewAddonButton );
		m_pNewAddonButton->SetNavDown( m_pAddonButtons[0] );
		m_pAddonButtons[0]->SetNavUp( m_pNewAddonButton );
	}

	for ( int i = 1; i < m_pAddonButtons.Count(); i++ )
    {
		m_pAddonButtons[i]->SetNavUp( m_pAddonButtons[i-1] );
		m_pAddonButtons[i-1]->SetNavDown( m_pAddonButtons[i] );
    }

	GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->ReleaseQueryUGCRequest( pResult->m_handle );

	if ( pResult->m_unNumResultsReturned == kNumUGCResultsPerPage )
	{
		m_nQueryPage++;
		if ( QueryAll() )
			return;
	}

	Msg( "Completed query in %f seconds\n", (float)(system()->GetTimeMillis() - m_nQueryTime) / 1e3f );

	m_iState = State_Active;

	FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select | FooterButtons::Refresh;
	SetFooterButtons( buttons, FooterButtons::Select );
}

void GamepadUIWorkshopPublishPanel::CreateItem()
{
	SteamAPICall_t hSteamAPICall = GamepadUI::GetInstance().GetSteamAPIContext()->SteamUGC()->CreateItem( GamepadUI::GetInstance().GetSteamAPIContext()->SteamUtils()->GetAppID(), EWorkshopFileType::k_EWorkshopFileTypeCommunity);
	if ( hSteamAPICall == k_uAPICallInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::CreateItem() Steam API call failed\n" );
		return;
	}

	m_CreateItemResult.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnCreateItemResult );
}

void GamepadUIWorkshopPublishPanel::OnCreateItemResult( CreateItemResult_t *pResult, bool bIOFailure )
{
	Assert( !bIOFailure );

	if ( pResult->m_eResult != k_EResultOK )
	{
		Warning( "GamepadUIWorkshopPublishPanel::OnCreateItemResult() (%i) %s\n", pResult->m_eResult, GetResultDesc( pResult->m_eResult ) );
		return;
	}

	if ( pResult->m_bUserNeedsToAcceptWorkshopLegalAgreement )
	{
		Warning( "User needs to accept workshop legal agreement\n" );
	}

	// TODO
	//m_pItemPublishDialog->m_item = pResult->m_nPublishedFileId;

	SubmitItemUpdate( pResult->m_nPublishedFileId );

	// Update labels
	//m_pItemPublishDialog->m_pProgressUpload->OnTick();
}

void GamepadUIWorkshopPublishPanel::SubmitItemUpdate( PublishedFileId_t id )
{
	/*Assert( m_pItemPublishDialog );

	char pszTitle[ k_cchPublishedDocumentTitleMax - sizeof(wchar_t) ];
	m_pItemPublishDialog->m_pTitleInput->GetText( pszTitle, sizeof(pszTitle) );

	char pszDesc[ k_cchPublishedDocumentDescriptionMax - sizeof(wchar_t) ];
	m_pItemPublishDialog->m_pDescInput->GetText( pszDesc, sizeof(pszDesc) );

	char pszChanges[ k_cchPublishedDocumentChangeDescriptionMax - sizeof(wchar_t) ];
	m_pItemPublishDialog->m_pChangesInput->GetText( pszChanges, sizeof(pszChanges) );

	char pszContent[ MAX_PATH ];
	m_pItemPublishDialog->m_pContentInput->GetText( pszContent, sizeof(pszContent) );

	char pszPreview[ MAX_PATH ];
	m_pItemPublishDialog->m_pPreviewInput->GetText( pszPreview, sizeof(pszPreview) );

	int visibility = m_pItemPublishDialog->m_pVisibility->GetActiveItemUserData()->GetInt();

	CUtlStringList vecTags;
	SteamParamStringArray_t tags;
	tags.m_nNumStrings = 0;

	for ( int i = g_nTagsGridRowCount; i--; )
	{
		for ( int j = g_nTagsGridColumnCount; j--; )
		{
			CheckButton *pCheckBox = (CheckButton*)m_pItemPublishDialog->m_pTagsGrid->GetElement( i, j );
			if ( pCheckBox && pCheckBox->IsSelected() )
			{
				tags.m_nNumStrings++;
				vecTags.CopyAndAddToTail( pCheckBox->GetName() );
			}
		}
	}

	UGCUpdateHandle_t item = m_hItemUpdate = steamapicontext->SteamUGC()->StartItemUpdate( steamapicontext->SteamUtils()->GetAppID(), id );

	if ( pszTitle[0] )
		Verify( steamapicontext->SteamUGC()->SetItemTitle( item, pszTitle ) );

	if ( pszDesc[0] )
		Verify( steamapicontext->SteamUGC()->SetItemDescription( item, pszDesc ) );

	if ( pszContent[0] )
		Verify( steamapicontext->SteamUGC()->SetItemContent( item, pszContent ) );

	if ( pszPreview[0] )
		Verify( steamapicontext->SteamUGC()->SetItemPreview( item, pszPreview ) );

	if ( tags.m_nNumStrings )
	{
		tags.m_ppStrings = const_cast< const char ** >( vecTags.Base() );
		if ( !steamapicontext->SteamUGC()->SetItemTags( item, &tags ) )
		{
			Warning( "Failed to set item tags.\n" );
		}
	}

	steamapicontext->SteamUGC()->SetItemVisibility( item, (ERemoteStoragePublishedFileVisibility)visibility );

	SteamAPICall_t hSteamAPICall = steamapicontext->SteamUGC()->SubmitItemUpdate( item, pszChanges );
	if ( hSteamAPICall == k_uAPICallInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::SubmitItemUpdate() Steam API call failed\n" );
		return;
	}

	m_SubmitItemUpdateResult.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnSubmitItemUpdateResult );*/
}

void GamepadUIWorkshopPublishPanel::OnSubmitItemUpdateResult( SubmitItemUpdateResult_t *pResult, bool bIOFailure )
{
	/*Assert( !bIOFailure );

	if ( pResult->m_eResult != k_EResultOK )
	{
		Warning( "GamepadUIWorkshopPublishPanel::OnSubmitItemUpdateResult() (%i) %s\n", pResult->m_eResult, GetResultDesc( pResult->m_eResult ) );
	}
	else
	{
		if ( pResult->m_bUserNeedsToAcceptWorkshopLegalAgreement )
		{
			Warning( "User needs to accept workshop legal agreement\n" );
		}

		// Open the page in Steam
		system()->ShellExecute( "open", CFmtStrN< 64 >( "steam://url/CommunityFilePage/%llu", m_pItemPublishDialog->m_item ) );
	}

	m_hItemUpdate = k_UGCUpdateHandleInvalid;

	if ( m_pItemPublishDialog )
	{
		if ( m_pItemPublishDialog->m_pProgressUpload )
		{
			m_pItemPublishDialog->m_pProgressUpload->Close();
		}

		m_pItemPublishDialog->Close();
		m_pItemPublishDialog = NULL;
	}

	MoveToFront();

	Refresh();*/
}

void GamepadUIWorkshopPublishPanel::DeleteItem( PublishedFileId_t id )
{
#ifdef _DEBUG
	// Warn if the item is not in the list
	{
		PublishedFileId_t inID = 0;
		for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
		{
			if ( id == pButton->m_nID )
			{
				inID = pButton->m_nID;
				break;
			}
		}

		if ( !inID && id )
		{
			DevWarning( "Deleting file from outside of the list %llu\n", id );
		}
	}
#endif

	if ( !id )
	{
		Warning( "GamepadUIWorkshopPublishPanel::DeleteItem() Invalid item\n" );
		return;
	}

	// NOTE: ISteamRemoteStorage::DeletePublishedFile was deprecated and ISteamUGC::DeleteItem was added in Steamworks SDK 141
	SteamAPICall_t hSteamAPICall = GamepadUI::GetInstance().GetSteamAPIContext()->SteamRemoteStorage()->DeletePublishedFile( id );
	if ( hSteamAPICall == k_uAPICallInvalid )
	{
		Warning( "GamepadUIWorkshopPublishPanel::DeleteItem() Steam API call failed\n" );
		return;
	}

	m_DeleteItemResult.Set( hSteamAPICall, this, &GamepadUIWorkshopPublishPanel::OnDeleteItemResult );

	//m_pDelete->SetEnabled( false );
}

void GamepadUIWorkshopPublishPanel::OnDeleteItemResult( RemoteStorageDeletePublishedFileResult_t *pResult, bool bIOFailure )
{
	Assert( !bIOFailure );

	if ( pResult->m_eResult != k_EResultOK )
	{
		Warning( "GamepadUIWorkshopPublishPanel::OnDeleteItemResult() (%i) %s\n", pResult->m_eResult, GetResultDesc( pResult->m_eResult ) );

		// It was deleted elsewhere, refresh
		if ( pResult->m_eResult == k_EResultFileNotFound )
		{
			Refresh();
		}
	}
	else
	{
		Msg( "Deleted item %llu\n", pResult->m_nPublishedFileId );

		// Remove deleted item from the list
        for ( int i = 0; i < m_pAddonButtons.Count(); i++ )
		{
			if ( pResult->m_nPublishedFileId == m_pAddonButtons[i]->m_nID)
			{
				delete m_pAddonButtons[i];
				m_pAddonButtons.Remove( i );
				break;
			}
		}
	}

	//m_pDelete->SetEnabled( true );
}

void GamepadUIWorkshopPublishPanel::OnCommand( char const* pCommand )
{
    if ( !V_strcmp( pCommand, "action_back" ) )
    {
        Close();
    }
	else if ( !V_strcmp( pCommand, "action_refresh" ) )
    {
        Refresh();
    }
	else if ( !V_strncmp( pCommand, "header_", 7 ) )
	{
		const char *pszHeader = pCommand + 7;
		if ( !V_strcmp( pszHeader, "title" ) )
			SortColumn( HEADER_TITLE );
		else if ( !V_strcmp( pszHeader, "lastupdated" ) )
			SortColumn( HEADER_LAST_UPDATED );
		else if ( !V_strcmp( pszHeader, "datecreated" ) )
			SortColumn( HEADER_DATE_CREATED );
		else if ( !V_strcmp( pszHeader, "visibility" ) )
			SortColumn( HEADER_VISIBILITY );
		else if ( !V_strcmp( pszHeader, "id" ) )
			SortColumn( HEADER_ID );
	}
	else if ( !V_strncmp( pCommand, "addon_", 6 ) )
    {
		PublishedFileId_t nID = V_atoui64(pCommand + 6);
		for ( GamepadUIAddonButton *pButton : m_pAddonButtons )
		{
			if (pButton->m_nID == nID)
			{
				m_pItemPublishDialog = new GamepadUIAddonEditPanel( GamepadUIWorkshopPublishPanel::GetInstance(), "AddonEdit" );
				QueryItem( nID );
				break;
			}
		}
    }
	else if ( !V_strcmp( pCommand, "new_addon" ) )
    {
		m_pItemPublishDialog = new GamepadUIAddonEditPanel( GamepadUIWorkshopPublishPanel::GetInstance(), "AddonEdit" );
    }
    else
    {
        BaseClass::OnCommand( pCommand );
    }
}

void GamepadUIWorkshopPublishPanel::OnMouseWheeled( int nDelta )
{
    m_ScrollState.OnMouseWheeled( nDelta * 100.0f, GamepadUI::GetInstance().GetTime() );
}

GamepadUIAddonEditPanel::GamepadUIAddonEditPanel( vgui::Panel *pParent, const char *PanelName ) : BaseClass( pParent, PanelName )
{
    vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile( GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemeaddonedit.res", "SchemeAddonEdit" );
    SetScheme( hScheme );

    GetFrameTitle() = GamepadUIString( "#WorkshopMgr_ItemPublish" );
    FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select | FooterButtons::Delete | FooterButtons::Publish;
    SetFooterButtons( buttons, FooterButtons::Select );

	//--------------------------------------------------------

	m_strTitleInput.SetText( "#WorkshopMgr_Label_Title" );
	m_strDescInput.SetText( "#WorkshopMgr_Label_Description" );
	m_strChangesInput.SetText( "#WorkshopMgr_Label_UpdateDesc" );
	m_strPreviewInput.SetText( "#WorkshopMgr_Label_PreviewImage" );
	m_strContentInput.SetText( "#WorkshopMgr_Label_AddonContent" );
	m_strVisibility.SetText( "#WorkshopMgr_Label_Visibility" );
	m_strTags.SetText( "#WorkshopMgr_Label_Tags" );

	m_strLoading.SetRawUTF8( "Retrieving file information..." );
	m_strNoImage.SetText( "#WorkshopMgr_NoImage" );

	m_pTitleInput = new GamepadUITextEntry( this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry_title.res" );
	m_pTitleInput->SetMaximumCharCount( k_cchPublishedDocumentTitleMax - sizeof(wchar_t) );
	m_pTitleInput->SetMultiline( false );
	m_pTitleInput->SetAllowNonAsciiCharacters( true );

	m_pDescInput = new GamepadUITextEntry( this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry_desc.res" );
	m_pDescInput->SetMaximumCharCount( k_cchPublishedDocumentDescriptionMax - sizeof(wchar_t) );
	m_pDescInput->SetMultiline( true );
	m_pDescInput->SetVerticalScrollbar( true );
	m_pDescInput->SetCatchEnterKey( true );
	m_pDescInput->SetAllowNonAsciiCharacters( true );
	
	m_pChangesInput = new GamepadUITextEntry( this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry_changes.res" );
	m_pChangesInput->SetMaximumCharCount( k_cchPublishedDocumentChangeDescriptionMax - sizeof(wchar_t) );
	m_pChangesInput->SetMultiline( true );
	m_pChangesInput->SetVerticalScrollbar( true );
	m_pChangesInput->SetCatchEnterKey( true );
	m_pChangesInput->SetAllowNonAsciiCharacters( true );

	m_pPreviewInput = new GamepadUITextEntry( this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry_preview.res" );
	m_pPreviewInput->SetMaximumCharCount( MAX_PATH - 1 );
	m_pPreviewInput->SetMultiline( false );

	m_pContentInput = new GamepadUITextEntry( this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry_content.res" );
	m_pContentInput->SetMaximumCharCount( MAX_PATH - 1 );
	m_pContentInput->SetMultiline( false );

	m_pPreviewBrowse = new GamepadUIButton( this, this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemebuttonbrowse.res", "PreviewBrowse", "#WorkshopMgr_ButtonSearch", "" );
	m_pContentBrowse = new GamepadUIButton( this, this, GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemebuttonbrowse.res", "ContentBrowse", "#WorkshopMgr_ButtonSearch", "" );

	m_pVisibility = new GamepadUIComboBox( this, "visibility", 4, false );
	m_pVisibility->SetScheme( vgui::scheme()->LoadSchemeFromFile( GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemetextentry.res", "SchemePanel" ) );
	// NOTE: Add in the order of the raw values. This is read as int in UpdateFields
	m_pVisibility->AddItem( "#Workshop_Public", new KeyValues( "", NULL, k_ERemoteStoragePublishedFileVisibilityPublic ) );
	m_pVisibility->AddItem( "#Workshop_FriendsOnly", new KeyValues( "", NULL, k_ERemoteStoragePublishedFileVisibilityFriendsOnly ) );
	m_pVisibility->AddItem( "#Workshop_Private", new KeyValues( "", NULL, k_ERemoteStoragePublishedFileVisibilityPrivate ) );
	m_pVisibility->AddItem( "#Workshop_Unlisted", new KeyValues( "", NULL, k_ERemoteStoragePublishedFileVisibilityUnlisted_149 ) );
	m_pVisibility->SilentActivateItem( k_ERemoteStoragePublishedFileVisibilityPrivate );

	//--------------------------------------------------------

    Activate();

    UpdateGradients();
}

GamepadUIAddonEditPanel::~GamepadUIAddonEditPanel()
{
}

void GamepadUIAddonEditPanel::UpdateGradients()
{
    const float flTime = GamepadUI::GetInstance().GetTime();
    GamepadUI::GetInstance().GetGradientHelper()->ResetTargets( flTime );
    GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient( GradientSide::Up, { 1.0f, 1.0f }, flTime );
    GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient( GradientSide::Down, { 1.0f, 0.75f }, flTime );
}

void GamepadUIAddonEditPanel::OnThink()
{
	BaseClass::OnThink();
}

void GamepadUIAddonEditPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	int nFontTall = vgui::surface()->GetFontTall( m_pTitleInput->m_hTextFont ) + 8;
	m_pTitleInput->SetPos( m_AddonTitleOffsetX, m_AddonTitleOffsetY );
	//m_pTitleInput->m_flWidth = m_AddonTitleWidth;
	//m_pTitleInput->m_flHeight = nFontTall;

	m_pDescInput->SetPos( m_AddonDescOffsetX, m_AddonDescOffsetY );
	//m_pDescInput->m_flWidth = m_AddonDescWidth;
	//m_pDescInput->m_flHeight = m_AddonDescHeight;

	m_pChangesInput->SetPos( m_AddonChangesOffsetX, m_AddonChangesOffsetY );
	//m_pChangesInput->m_flWidth = m_AddonDescWidth;
	//m_pChangesInput->m_flHeight = m_AddonChangesHeight;

	m_pPreviewInput->SetPos( m_AddonPreviewPathOffsetX, m_AddonPreviewPathOffsetY );
	//m_pPreviewInput->m_flWidth = m_AddonPreviewPathWidth;
	//m_pPreviewInput->m_flHeight = nFontTall;

	m_pContentInput->SetPos( m_AddonPreviewPathOffsetX, m_AddonContentPathOffsetY );
	//m_pContentInput->m_flWidth = m_AddonPreviewPathWidth;
	//m_pContentInput->m_flHeight = nFontTall;

	m_pPreviewBrowse->SetPos( m_AddonPreviewPathOffsetX + m_pPreviewInput->GetWide() + 4, m_AddonPreviewPathOffsetY );
	m_pContentBrowse->SetPos( m_AddonPreviewPathOffsetX + m_pContentInput->GetWide() + 4, m_AddonContentPathOffsetY );

	m_pVisibility->SetPos( m_AddonVisibilityOffsetX, m_AddonVisibilityOffsetY );
	m_pVisibility->SetSize( m_AddonVisibilityWidth, nFontTall );

	//--------------------------------------------------------

	m_pTitleInput->SetNavDown( m_pDescInput );

	m_pDescInput->SetNavUp( m_pTitleInput );
	m_pDescInput->SetNavLeft( m_pVisibility );
	m_pDescInput->SetNavDown( m_pChangesInput );

	m_pChangesInput->SetNavUp( m_pDescInput );
	m_pChangesInput->SetNavLeft( m_pPreviewBrowse );

	m_pPreviewInput->SetNavDown( m_pContentInput );
	m_pPreviewInput->SetNavRight( m_pPreviewBrowse );

	m_pContentInput->SetNavUp( m_pPreviewInput );
	m_pContentInput->SetNavRight( m_pContentBrowse );

	m_pPreviewBrowse->SetNavUp( m_pVisibility );
	m_pPreviewBrowse->SetNavDown( m_pContentBrowse );
	m_pPreviewBrowse->SetNavLeft( m_pPreviewInput );
	m_pPreviewBrowse->SetNavRight( m_pChangesInput );

	m_pContentBrowse->SetNavUp( m_pPreviewBrowse );
	m_pContentBrowse->SetNavLeft( m_pContentInput );
	m_pContentBrowse->SetNavRight( m_pChangesInput );

	//m_pVisibility->SetNavUp( m_pVisibility );
	m_pVisibility->SetNavDown( m_pPreviewBrowse );
	m_pVisibility->SetNavRight( m_pDescInput );

	m_pTitleInput->NavigateTo();

	//--------------------------------------------------------
}

void GamepadUIAddonEditPanel::Paint()
{
	BaseClass::Paint();

	if (m_bLoadingInfo)
	{
		int nParentW, nParentH;
		GetParent()->GetSize( nParentW, nParentH );

		int nTextW, nTextH;
		vgui::surface()->GetTextSize( m_hGenericFont, m_strLoading.String(), nTextW, nTextH );

		int nX = (nParentW / 2);
		int nY = (nParentH / 2);

		nX -= (nTextW/2);
		nY -= (nTextH/2);

		vgui::surface()->DrawSetTextColor( Color( 255, 255, 255, 192 ) );
		vgui::surface()->DrawSetTextFont( m_hGenericFont );
		DrawPrintWrappedText( m_hGenericFont, nX, nY, m_strLoading.String(), m_strLoading.Length(), m_ImageWidth, true );
	}
	else
	{
		PaintFieldTitles();
	}

	if (m_PreviewImage.IsValid())
	{
		vgui::surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		vgui::surface()->DrawSetTexture( m_PreviewImage );
		vgui::surface()->DrawTexturedRect( m_ImageOffsetX, m_ImageOffsetY, m_ImageOffsetX + m_ImageWidth, m_ImageOffsetY + m_ImageHeight );
		vgui::surface()->DrawSetTexture( 0 );
	}
	else if (m_bLoadingImage)
	{
		vgui::surface()->DrawSetColor( m_colSpinnerImage );

		int nX = m_ImageOffsetX + (m_ImageWidth / 2);
		int nY = m_ImageOffsetY + (m_ImageHeight / 3);
		nX -= (m_flSpinnerSize / 2);
		nY -= (m_flSpinnerSize / 2);

		static_cast<GamepadUIWorkshopPublishPanel*>(GetParent())->PaintSpinner( nX, nY, nX + m_flSpinnerSize, nY + m_flSpinnerSize ); // m_ImageOffsetX, m_ImageOffsetY, m_ImageOffsetX + m_ImageWidth, m_ImageOffsetY + m_ImageHeight
	}
	else
	{
		int nTextW, nTextH;
		vgui::surface()->GetTextSize( m_hGenericFont, m_strNoImage.String(), nTextW, nTextH );

		int nX = m_ImageOffsetX + (m_ImageWidth / 2);
		int nY = m_ImageOffsetY + (m_ImageHeight / 2);

		nX -= (nTextW/2);
		nY -= (nTextH/2);

		vgui::surface()->DrawSetTextColor( Color( 255, 255, 255, 192 ) );
		vgui::surface()->DrawSetTextFont( m_hGenericFont );
		DrawPrintWrappedText( m_hGenericFont, nX, nY, m_strNoImage.String(), m_strNoImage.Length(), m_ImageWidth, true );
	}
}

void GamepadUIAddonEditPanel::PaintFieldTitle( GamepadUIString &str, int nOffsetX, int nOffsetY )
{
    int nTextW, nTextH;
    vgui::surface()->GetTextSize( m_hFieldTitleFont, str.String(), nTextW, nTextH );

    int nTextY = nOffsetY - nTextH - 4;

	vgui::surface()->DrawSetTextFont( m_hFieldTitleFont );
    vgui::surface()->DrawSetTextPos( nOffsetX, nTextY );
    vgui::surface()->DrawPrintText( str.String(), str.Length() );
}

void GamepadUIAddonEditPanel::PaintFieldTitles()
{
	PaintFieldTitle( m_strTitleInput, m_AddonTitleOffsetX, m_AddonTitleOffsetY );
	PaintFieldTitle( m_strDescInput, m_AddonDescOffsetX, m_AddonDescOffsetY );
	PaintFieldTitle( m_strChangesInput, m_AddonChangesOffsetX, m_AddonChangesOffsetY );
	PaintFieldTitle( m_strPreviewInput, m_AddonPreviewPathOffsetX, m_AddonPreviewPathOffsetY );
	PaintFieldTitle( m_strContentInput, m_AddonPreviewPathOffsetX, m_AddonContentPathOffsetY );

	PaintFieldTitle( m_strTags, m_AddonTagsOffsetX, m_AddonTagsOffsetY );
	PaintFieldTitle( m_strVisibility, m_AddonVisibilityOffsetX, m_AddonVisibilityOffsetY );
}

void GamepadUIAddonEditPanel::ApplySchemeSettings( vgui::IScheme* pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );

    m_hFieldTitleFont = pScheme->GetFont( "TextEntry.Title.Font", true );
}

void GamepadUIAddonEditPanel::OnCommand( char const* pCommand )
{
    if ( !V_strcmp( pCommand, "action_back" ) )
    {
        Close();
    }
	else if ( !V_strcmp( pCommand, "action_refresh" ) )
    {
        //Refresh();
    }
	else if ( !V_strcmp( pCommand, "action_publish" ) )
	{
		//OnPublish();
	}
	else if ( !V_strcmp( pCommand, "PreviewBrowse" ) )
	{
		FileOpenDialog *pFileBrowser = new FileOpenDialog( this, "Browse preview image", FileOpenDialogType_t::FOD_OPEN);
		pFileBrowser->AddFilter( "*.jpg;*.jpeg;*.png;*.gif;*.bmp", "Image files (*.jpg;*.jpeg;*.png;*.gif;*.bmp)", true );

		pFileBrowser->SetScheme( scheme()->LoadSchemeFromFile( GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemefiledialog.res", "SchemePanel" ) );
		pFileBrowser->SetPaintBackgroundEnabled( true );
		pFileBrowser->SetPaintBorderEnabled( true );
		pFileBrowser->SetConsoleStylePanel( true );

		pFileBrowser->AddActionSignalTarget( this );
		pFileBrowser->SetDeleteSelfOnClose( true );
		pFileBrowser->DoModal();
		pFileBrowser->SetVisible( true );
	}
	else if ( !V_strcmp( pCommand, "ContentBrowse" ) )
	{
		// NOTE: DirectorySelectDialog sucks, but FileOpenDialogType_t::FOD_SELECT_DIRECTORY does not work
		DirectorySelectDialog *pDirectoryBrowser = new DirectorySelectDialog( this, "Browse content directory" );
		pDirectoryBrowser->SetScheme( scheme()->LoadSchemeFromFile( GAMEPADUI_WORKSHOP_RESOURCE_FOLDER "schemefiledialog.res", "SchemePanel" ) );
		pDirectoryBrowser->SetPaintBackgroundEnabled( true );
		pDirectoryBrowser->SetPaintBorderEnabled( true );
		pDirectoryBrowser->SetConsoleStylePanel( true );
		pDirectoryBrowser->MakeReadyForUse();
		pDirectoryBrowser->AddActionSignalTarget( this );
		pDirectoryBrowser->SetDeleteSelfOnClose( true );
		pDirectoryBrowser->DoModal();
		pDirectoryBrowser->SetVisible( true );

		char pLocalPath[255];
		g_pFullFileSystem->GetCurrentDirectory( pLocalPath, 255 );
		pDirectoryBrowser->SetStartDirectory( pLocalPath );
	}
    else
    {
        BaseClass::OnCommand( pCommand );
    }
}

void GamepadUIAddonEditPanel::SetPreviewImage( const char *filename )
{
	const char *ext = V_GetFileExtension( filename );

	if ( !ext )
	{
		m_PreviewImage.Cleanup();
	}
	else if ( !V_stricmp( ext, "jpg" ) || !V_stricmp( ext, "jpeg" ) )
	{
		m_PreviewImage.SetJPEGImage( filename );
	}
	else if ( !V_stricmp( ext, "png" ) )
	{
		m_PreviewImage.SetPNGImage( filename );
	}
	else
	{
		m_PreviewImage.Cleanup();

		// if this is a type that cannot be loaded, always draw "loading" message
		if ( g_pFullFileSystem->FileExists( filename ) )
		{
			if ( !V_stricmp( ext, "gif" ) || !V_stricmp( ext, "bmp" ) )
			{
				m_bLoadingImage = true;
			}
		}
	}
}

void GamepadUIAddonEditPanel::SetPreviewImage( CUtlBuffer &file, const char *filename )
{
	const char *ext = V_GetFileExtension( filename );

	if ( !ext )
	{
		m_PreviewImage.Cleanup();
	}
	else if ( !V_stricmp( ext, "jpg" ) || !V_stricmp( ext, "jpeg" ) )
	{
		m_PreviewImage.SetJPEGImage( file );
	}
	else if ( !V_stricmp( ext, "png" ) )
	{
		m_PreviewImage.SetPNGImage( file );
	}
	else
	{
		m_PreviewImage.Cleanup();

		// if this is a type that cannot be loaded, always draw "loading" message
		m_bLoadingImage = true;
	}
}

void GamepadUIAddonEditPanel::OnTextChanged( Panel *panel )
{
	if ( panel == m_pPreviewInput )
	{
		// Try to load image from current text
		if ( m_pPreviewInput->GetTextLength() )
		{
			char path[ MAX_PATH ];
			m_pPreviewInput->GetText( path, sizeof(path) );
			SetPreviewImage( path );
		}
		else
		{
			m_PreviewImage.Cleanup();
		}
	}
#if 0
	else if ( panel == m_pContentInput )
	{
		// Get content size
		if ( m_pContentInput->GetTextLength() )
		{
			char path[ MAX_PATH ];
			m_pContentInput->GetText( path, sizeof(path) );
			SetAddonContent( path );
		}
		else
		{
			m_pContentSize->SetText( "" );
		}
	}
#endif
}

void GamepadUIAddonEditPanel::OnFileSelected( const char *fullpath )
{
	m_pPreviewInput->SetText( fullpath );
	SetPreviewImage( fullpath );

	// Set content to the same path as preview image if it was not already set
	if ( !m_pContentInput->GetTextLength() )
	{
		char path[MAX_PATH];
		V_StripExtension( fullpath, path, MAX_PATH );
		m_pContentInput->SetText( path );
	}
}

void GamepadUIAddonEditPanel::OnDirectorySelected( const char *dir )
{
	m_pContentInput->SetText( dir );

	// Set preview image to the same path as content if it was not already set
	if ( !m_pPreviewInput->GetTextLength() )
	{
		char path[MAX_PATH];
		int len = V_strlen(dir);

		if ( dir[len-1] == CORRECT_PATH_SEPARATOR || dir[len-1] == INCORRECT_PATH_SEPARATOR )
		{
			// ignore trailing slash
			V_strncpy( path, dir, len );
		}
		else
		{
			V_strncpy( path, dir, sizeof(path) );
		}

		V_strcat( path, ".jpg", sizeof(path) );
		m_pPreviewInput->SetText( path );

		// Load image if it exists
		if ( g_pFullFileSystem->FileExists( path ) )
		{
			m_PreviewImage.SetJPEGImage( path );
		}
	}
}

void GamepadUIAddonEditPanel::UpdateFields( const SteamUGCDetails_t &data )
{
	m_item = data.m_nPublishedFileId;

	wchar_t wtitle[128];
	V_snwprintf( wtitle, sizeof(wtitle), L"%ls (%llu)", g_pVGuiLocalize->Find("#WorkshopMgr_ItemPublish"), m_item );
	SetTitle( wtitle, false );
	//m_pPublish->SetText( "#WorkshopMgr_ButtonUpdate" );

	FooterButtonMask buttons = FooterButtons::Back | FooterButtons::Select | FooterButtons::Delete | FooterButtons::Update;
	SetFooterButtons( buttons, FooterButtons::Select );

	m_pTitleInput->SetText( data.m_rgchTitle );
	m_pDescInput->SetText( data.m_rgchDescription );
	//m_pVisibility->SilentActivateItem( data->GetInt("visibility_raw") );

	/*
	const char *ppTags = data->GetString("tags");
	if ( ppTags[0] )
	{
		CUtlStringList tags;
		V_SplitString( ppTags, ",", tags );

		FOR_EACH_VEC( tags, i )
		{
			const char *tag = tags[i];
			CheckButton *tagCheckBox = (CheckButton*)m_pTagsGrid->FindChildByName( tag );

			// Tag from the workshop item is not found in g_ppWorkshopTags
			AssertMsg( tagCheckBox, "Tag '%s' not found", tag );

			if ( tagCheckBox )
			{
				Assert( !V_stricmp( "CheckButton", tagCheckBox->GetClassName() ) );
				tagCheckBox->SetSelected( true );
			}
		}
	}
	*/
}

CON_COMMAND( gamepadui_openpublishdialog, "" )
{
    new GamepadUIWorkshopPublishPanel( GamepadUI::GetInstance().GetBasePanel(), "" );
}
