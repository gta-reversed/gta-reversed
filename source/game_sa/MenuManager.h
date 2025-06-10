/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "MenuManager_Internal.h"

#include "RGBA.h"
#include "Sprite2d.h"
#include "Vector2D.h"
#include "eLanguage.h"

class CRect;

enum eHelperText : int32 {
    HELPER_NONE,
    FET_APP = 1,
    FET_HRD = 2,
    FET_RSO = 3,
    FEA_SCF = 4,
    FEA_SCS = 5,
    FET_STS = 6,
};

enum eRadarMode : int32 {
    RADAR_MODE_MAPS_AND_BLIPS = 0,
    RADAR_MODE_BLIPS_ONLY     = 1,
    RADAR_MODE_OFF            = 2,

    RADAR_MODE_COUNT
};

enum eRadioMode : int8 {
    RADIO_MODE_RADIO      = 0,
    RADIO_MODE_RANDOM     = 1,
    RADIO_MODE_SEQUENTIAL = 2,

    RADIO_MODE_COUNT
};

struct MPack {
    uint8 m_Id;
    char  m_Name[260];
};

enum class eController : int8 {
    MOUSE_PLUS_KEYS = 0,
    JOYPAD          = 1
};

enum class eControlMode : uint8 {
    FOOT    = 0,
    VEHICLE = 1
};

enum class eControllerError : int8 {
    NONE     = 0,
    VEHICLE  = 1,
    FOOT     = 2,
    NOT_SETS = 3
};

enum class eMouseInBounds {
    /*
    LEFT             = -1,
    NONE_SIDE        = 0,
    RIGNT            = 1,
    */

    MENU_ITEM        = 2,
    BACK_BUTTON      = 3,
    ENTER_MENU       = 4,
    SELECT           = 5,
    SLIDER_RIGHT     = 6,
    SLIDER_LEFT      = 7,
    DRAW_DIST_RIGHT  = 8,
    DRAW_DIST_LEFT   = 9,
    RADIO_VOL_RIGHT  = 10,
    RADIO_VOL_LEFT   = 11,
    SFX_VOL_RIGHT    = 12,
    SFX_VOL_LEFT     = 13,
    MOUSE_SENS_RIGHT = 14,
    MOUSE_SENS_LEFT  = 15,
    NONE             = 16
};

constexpr auto FRONTEND_MAP_RANGE_MIN = 300.0f;
constexpr auto FRONTEND_MAP_RANGE_MAX = 1100.0f;

class CMenuManager {
    enum {
        MPACK_COUNT  = 25,
        SPRITE_COUNT = 25,
    };

public:
    static constexpr uint32 SETTINGS_FILE_VERSION = 6u;

    int8      m_nStatsScrollDirection;
    float     m_fStatsScrollSpeed;
    uint8     m_nSelectedRow; // CMenuSystem
    char      field_9[23]; // unused
    bool      m_PrefsUseVibration;
    bool      m_bHudOn;
    char      field_22[2]; // pad
    eRadarMode m_nRadarMode;
    char      field_28[4];
    int32     m_nTargetBlipIndex; // blip script handle
    int8      m_nSysMenu; // CMenuSystem
    bool      m_DisplayControllerOnFoot;
    bool      m_bDontDrawFrontEnd;
    bool      m_bActivateMenuNextFrame;
    bool      m_bMenuAccessWidescreen;
    bool      field_35;
    char      field_36[2]; // unused
    RsKeyCodes m_KeyPressedCode;
    int32     m_PrefsBrightness;
    float     m_fDrawDistance;

    bool      m_bShowSubtitles;
    union {
        struct {
            bool m_ShowLocationsBlips;
            bool m_ShowContactsBlips;
            bool m_ShowMissionBlips;
            bool m_ShowOtherBlips;
            bool m_ShowGangAreaBlips;
        };
        bool m_abPrefsMapBlips[5];
    };
    bool      m_bMapLegend;
    bool      m_bWidescreenOn;
    bool      m_bPrefsFrameLimiter;
    bool      m_bRadioAutoSelect;
    bool      m_PrefsAudioOutputMode;
    int8      m_nSfxVolume;
    int8      m_nRadioVolume;
    bool      m_bRadioEq;

    eRadioID  m_nRadioStation;
    char      field_53; // unused
    int32     m_nCurrentScreenItem; // CurrentOption
    bool      m_bQuitGameNoDVD; // CMenuManager::WaitForUserCD 0x57C5E0

    bool      m_bDrawingMap;
    bool      m_bStreamingDisabled;
    bool      m_bAllStreamingStuffLoaded;

    bool      m_bMenuActive;
    bool      m_bStartGameLoading;
    int8      m_nGameState;
    bool      m_bIsSaveDone;
    bool      m_bLoadingData;
    float     m_fMapZoom;
    CVector2D m_vMapOrigin;
    CVector2D m_vMousePos;  // Red marker position (world coordinates)
    bool      m_bMapLoaded;

    int32     m_nTitleLanguage; // Value is PRIMARYLANGID(GetSystemDefaultLCID())
    int32     m_nTextLanguage; // TODO: Change to `eLanguage`
    eLanguage m_nPrefsLanguage;
    eLanguage m_nPreviousLanguage;
    int32     m_SystemLanguage;
    bool      m_bLoadedLanguage;
    int32     m_ListSelection;      // controller related
    int32     field_94;      // unused
    uint8*    m_GalleryImgBuffer;   //!< +0x98  \see JPegCompress file
    char      field_9C[16]; // unused
    uint32    m_nUserTrackIndex;
    eRadioMode m_nRadioMode;

    bool      m_bInvertPadX1;
    bool      m_bInvertPadY1;
    bool      m_bInvertPadX2;
    bool      m_bInvertPadY2;
    bool      m_bSwapPadAxis1;
    bool      m_bSwapPadAxis2;

    eControlMode m_RedefiningControls;
    bool      m_DisplayTheMouse; // m_bMouseMoved
    int32     m_nMousePosX;
    int32     m_nMousePosY;
    bool      m_bPrefsMipMapping;
    bool      m_bTracksAutoScan;
    int32     m_nPrefsAntialiasing;
    int32     m_nDisplayAntialiasing;
    eController m_ControlMethod;
    int32     m_nPrefsVideoMode;
    int32     m_nDisplayVideoMode;
    int32     m_nCurrentRwSubsystem; // initialized | not used

    int32     m_nMousePosWinX; // xPos = GET_X_LPARAM(lParam); 0x748323
    int32     m_nMousePosWinY; // yPos = GET_Y_LPARAM(lParam);

    bool      m_bSavePhotos;
    bool      m_bMainMenuSwitch;
    uint8     m_nPlayerNumber;
    bool      m_bLanguageChanged; // useless?
    int32     field_EC;
    RsKeyCodes* m_pPressedKey; // any pressed key, in order of CKeyboardState; rsNULL means no key pressed
    bool      m_isPreInitialised;

    union {
        struct {
            CSprite2d m_apRadioSprites[FRONTEND2_START];
            CSprite2d m_apBackgroundTextures[FRONTEND3_START - FRONTEND2_START];
            CSprite2d m_apAdditionalBackgroundTextures[FRONTEND4_START - FRONTEND3_START];
            CSprite2d m_apMouseTextures[FRONTEND_SPRITE_COUNT - FRONTEND4_START];
        };
        CSprite2d m_aFrontEndSprites[FRONTEND_SPRITE_COUNT];
    };

    bool        m_bTexturesLoaded;
    eMenuScreen m_nCurrentScreen;
    eMenuScreen m_nPrevScreen; // Used only in SwitchToNewScreen
    uint8       m_SelectedSlot;
    uint8       m_nMissionPackGameId;
    MPack       m_MissionPacks[MPACK_COUNT];
    bool        m_bDoVideoModeUpdate;
    RsKeyCodes  m_nPressedMouseButton; // used in redefine controls
    int32       m_nJustDownJoyButton;  // used in redefine controls; set via CControllerConfigManager::GetJoyButtonJustDown
    bool        m_MenuIsAbleToQuit;
    bool        m_bRadioAvailable;
    eControllerError m_nControllerError;
    bool        m_bScanningUserTracks;
    int32       m_nHelperTextFadingAlpha;
    bool        m_KeyPressed[5];
    int32       m_nOldMousePosX;
    int32       m_nOldMousePosY;
    eMouseInBounds m_MouseInBounds;
    int32       m_CurrentMouseOption;
    bool        m_bJustOpenedControlRedefWindow;
    bool        m_EditingControlOptions;
    bool        m_DeleteAllBoundControls;
    bool        m_DeleteAllNextDefine;
    int32       m_OptionToChange;
    int32       m_OptionProcessing; // unused
    bool        m_CanBeDefined;
    bool        m_JustExitedRedefine;
    char        field_1B16[2]; // unused
    eHelperText m_nHelperText;
    uint32      m_TimeToStopPadShaking; // useless

    bool        m_TexturesSwapped;
    uint8       m_nNumberOfMenuOptions;
    char        field_1B22[2]; // unused
    uint32      m_StatsScrollTime;
    bool        m_bViewRadar;
    char        field_1B29[3]; // unused
    uint32      m_RadarVisibilityChangeTime;
    uint32      m_BriefsArrowBlinkTime;
    uint16      m_StatusDisablePlayerControls;
    char        field_1B36[2]; // unused
    int32       m_LastActionTime;
    bool        m_CurrentlyLoading;
    bool        m_CurrentlyDeleting;
    bool        m_CurrentlySaving; // mpack related
    char        field_1B3F;        // unused
    uint32      m_UserTrackScanningTime;
    bool        m_ErrorPendingReset;
    char        field_1B45[3]; // unused
    uint32      m_ErrorStartTime;

    union {
        struct {
            uint32 field_1B4C_b1 : 1;
            uint32 bScanningUserTracks : 1;
        };
        int32 field_1B4C;
    };

    int8   m_nBackgroundSprite;
    bool   m_isTextBlinking;
    char   field_1B52[2]; // unused
    int32  m_lastBlinkTime;
    uint32 m_HelperTextUpdatedTime;
    bool   m_OptionFlashColorState;
    char   field_1B5D[3]; // unused
    uint32 m_LastHighlightToggleTime;
    uint32 m_LastTransitionTime;
    uint32 m_SlideLeftMoveTime;
    uint32 m_SlideRightMoveTime;
    int32  field_1B70;
    int32  field_1B74; // ???

    static int32& nLastMenuPage;

    static inline bool& bInvertMouseX = *(bool*)0xBA6744;
    static inline bool& bInvertMouseY = *(bool*)0xBA6745;

    // notsa colors

    static constexpr CRGBA MENU_BG              = CRGBA(0, 0, 0, 255);       // Black background
    static constexpr CRGBA MENU_BUILD_INFO_TEXT = CRGBA(255, 255, 255, 100);
    static constexpr CRGBA MENU_CONTROLLER_BG   = CRGBA(49, 101, 148, 100);
    static constexpr CRGBA MENU_CURSOR_SHADOW   = CRGBA(100, 100, 100, 50);
    static constexpr CRGBA MENU_ERROR           = CRGBA(200, 50, 50, 255);   // Error/Warning color
    static constexpr CRGBA MENU_MAP_BACKGROUND  = CRGBA(111, 137, 170, 255); // Map background
    static constexpr CRGBA MENU_MAP_BORDER      = CRGBA(100, 100, 100, 255); // Map border
    static constexpr CRGBA MENU_MAP_FOG         = CRGBA(111, 137, 170, 200);
    static constexpr CRGBA MENU_PROGRESS_BG     = CRGBA(50, 50, 50, 255);
    static constexpr CRGBA MENU_SHADOW          = CRGBA(0, 0, 0, 200);       // Semi-transparent shadow
    static constexpr CRGBA MENU_TEXT_DISABLED   = CRGBA(14, 30, 47, 255);
    static constexpr CRGBA MENU_TEXT_INACTIVE   = CRGBA(255, 255, 255, 30);
    static constexpr CRGBA MENU_TEXT_LIGHT_GRAY = CRGBA(225, 225, 225, 255); // Light gray text
    static constexpr CRGBA MENU_TEXT_NORMAL     = CRGBA(74, 90, 107, 255);   // Plain text (not selected)
    static constexpr CRGBA MENU_TEXT_SELECTED   = CRGBA(172, 203, 241, 255); // Highlighted text
    static constexpr CRGBA MENU_TEXT_WHITE      = CRGBA(255, 255, 255, 255); // White text

public:
    static void InjectHooks();

    CMenuManager();
    ~CMenuManager();
    CMenuManager* Constructor();
    CMenuManager* Destructor();

    void Initialise();

    void LoadAllTextures();
    void SwapTexturesRound(bool slot);
    void UnloadTextures();

    void InitialiseChangedLanguageSettings(bool reinitControls);
    bool HasLanguageChanged();

    void DoSettingsBeforeStartingAGame();
    float StretchX(float x);
    float StretchY(float y);
    void SwitchToNewScreen(eMenuScreen screen);
    void ScrollRadioStations(int8 numStations);
    void SetFrontEndRenderStates();
    void SetDefaultPreferences(eMenuScreen screen);
    uint32 GetNumberOfMenuOptions();

    void JumpToGenericMessageScreen(eMenuScreen screen, const char* titleKey, const char* textKey);

    void DrawFrontEnd();
    void DrawBuildInfo();
    void DrawBackground();
    void DrawStandardMenus(bool);
    void DrawWindow(const CRect& coords, const char* key, uint8 color, CRGBA backColor, bool unused, bool background);
    void DrawWindowedText(float x, float y, float wrap, const char* title, const char* message, eFontAlignment alignment);
    void DrawQuitGameScreen();
    void DrawControllerScreenExtraText(int32);
    void DrawControllerBound(uint16 verticalOffset, bool isOppositeScreen);
    void DrawControllerSetupScreen();
#ifdef USE_GALLERY
    void DrawGallery();
    void DrawGallerySaveMenu();
#endif

    void CentreMousePointer();

    void LoadSettings();
    void SaveSettings();
    void SaveStatsToFile();
    void SaveLoadFileError_SetUpErrorScreen();

    void CheckSliderMovement(int32 value);
    [[nodiscard]] bool CheckFrontEndUpInput() const;
    [[nodiscard]] bool CheckFrontEndDownInput() const;
    [[nodiscard]] bool CheckFrontEndLeftInput() const;
    [[nodiscard]] bool CheckFrontEndRightInput() const;
    void CheckForMenuClosing();
    [[nodiscard]] bool CheckHover(float left, float right, float top, float bottom) const;
    bool CheckMissionPackValidMenu();
    void CheckCodesForControls(eControllerType type);

    int32 DisplaySlider(float x, float y, float h1, float h2, float length, float value, int32 spacing);

    void DisplayHelperText(const char* key);
    void SetHelperText(eHelperText messageId);
    void ResetHelperText();
    void NoDiskInDriveMessage();

    void MessageScreen(const char* key, bool blackBackground, bool cameraUpdateStarted);
    void SmallMessageScreen(const char* key);

    void CalculateMapLimits(float& bottom, float& top, float& left, float& right);

    void PlaceRedMarker();
    void RadarZoomIn();

    void PrintMap();
    void PrintStats();
    void PrintBriefs();
    void PrintRadioStationList();

    void UserInput();
    void AdditionalOptionInput(bool* upPressed, bool* downPressed);
    bool CheckRedefineControlInput();
    void RedefineScreenUserInput(bool* accept, bool* cancel);

    void Process();
    void ProcessStreaming(bool streamAll);
    void ProcessFileActions();
    void ProcessUserInput(bool GoDownMenu, bool GoUpMenu, bool EnterMenuOption, bool GoBackOneMenu, int8 LeftRight);
    void ProcessMenuOptions(int8 pressedLR, bool& cancelPressed, bool acceptPressed);
    bool ProcessPCMenuOptions(int8 pressedLR, bool acceptPressed);
    void ProcessMissionPackNewGame();

    // NOTSA
    const char* GetMovieFileName() const {
        switch (m_nTitleLanguage) {
        case 12:
        case 7:
            return "movies\\GTAtitlesGER.mpg";
        }
        return "movies\\GTAtitles.mpg";
    }
    uint32 GetMaxAction();
    uint32 GetVerticalSpacing();

    //! Simulate that we came into the menu and clicked to load game
    //! @param newGame If we should start a new game
    //! @param slot    Slot of the save-game to load (Ignored if `newGame`)
    void SimulateGameLoad(bool newGame, uint32 slot);
private:
    static void SetBrightness(float brightness, bool arg2);
};

VALIDATE_SIZE(CMenuManager, 0x1B78);

extern CMenuManager& FrontEndMenuManager;
