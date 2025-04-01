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
    FET_APP = 1, // CLICK LMB / RETURN - APPLY NEW SETTING
    FET_HRD = 2, // DEFAULT SETTINGS RESTORED
    FET_RSO = 3, // ORIGINAL SETTING RESTORED
    FEA_SCF = 4, // FAILED TO SCAN USER TRACKS
    FEA_SCS = 5, // USER TRACKS SCANNED SUCCESSFULLY
    FEA_STS = 6, // STATS SAVED TO 'STATS.HTML'
};

enum eRadarMode : int32 {
    MAPS_AND_BLIPS,
    BLIPS_ONLY,
    OFF
};

struct MPack {
    uint8 m_Id;
    char  m_Name[260];
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
    char      field_9[23];
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
    char      field_35;
    char      field_36[2];
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
    char      field_4E;
    int8      m_nSfxVolume;
    int8      m_nRadioVolume;
    bool      m_bRadioEq;

    eRadioID  m_nRadioStation;
    char      field_53;
    int32     m_nCurrentScreenItem;
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
    bool      field_8C;
    int32     m_ListSelection;      // controller related
    int32     field_94;      // unused
    uint8*    m_GalleryImgBuffer;   //!< +0x98  \see JPegCompress file
    char      field_9C[16];
    int32     m_nUserTrackIndex;
    int8      m_nRadioMode;

    bool      m_bInvertPadX1;
    bool      m_bInvertPadY1;
    bool      m_bInvertPadX2;
    bool      m_bInvertPadY2;
    bool      m_bSwapPadAxis1;
    bool      m_bSwapPadAxis2;

    bool      m_RedefiningControls;
    bool      m_bDrawMouse; // m_bMouseMoved
    int32     m_nMousePosX;
    int32     m_nMousePosY;
    bool      m_bPrefsMipMapping;
    bool      m_bTracksAutoScan;
    int32     m_nPrefsAntialiasing;
    int32     m_nDisplayAntialiasing;
    int32     m_ControlMethod; // TODO: Change to enum
    int32     m_nPrefsVideoMode;
    int32     m_nDisplayVideoMode;
    int32     m_nCurrentRwSubsystem; // initialized | not used

    int32     m_nMousePosWinX; // xPos = GET_X_LPARAM(lParam); 0x748323
    int32     m_nMousePosWinY; // yPos = GET_Y_LPARAM(lParam);

    bool      m_bSavePhotos;
    bool      m_bMainMenuSwitch;
    int8      m_nPlayerNumber;
    bool      m_bLanguageChanged; // useless?
    int32     field_EC;
    RsKeyCodes* m_pPressedKey; // any pressed key, in order of CKeyboardState; rsNULL means no key pressed
    bool      field_F4; // m_bPreInitialised

    union {
        struct {
            CSprite2d m_apRadioSprites[13];
            CSprite2d m_apBackgroundTextures[8];
            CSprite2d m_apAdditionalBackgroundTextures[2];
            CSprite2d m_apMouseTextures[2];
        };
        CSprite2d m_aFrontEndSprites[25];
    };

    bool  m_bTexturesLoaded;
    eMenuScreen m_nCurrentScreen;
    eMenuScreen m_nPrevScreen; // Used only in SwitchToNewScreen
    uint8 m_bSelectedSaveGame;
    uint8 m_nMissionPackGameId;
    MPack m_MissionPacks[25];
    bool  m_bDoVideoModeUpdate;
    RsKeyCodes m_nPressedMouseButton; // used in redefine controls
    int32 m_nJustDownJoyButton; // used in redefine controls; set via CControllerConfigManager::GetJoyButtonJustDown
    bool  m_MenuIsAbleToQuit;
    bool  m_bRadioAvailable;
    uint8 m_nControllerError;
    bool  m_bScanningUserTracks;
    int32 m_nHelperTextFadingAlpha;
    char  field_1AF0;
    char  field_1AF1;
    char  field_1AF2;
    char  field_1AF3;
    int32 field_1AF4;
    int32 m_nOldMousePosX;
    int32 m_nOldMousePosY;
    int32 m_MouseInBounds;
    int32 m_CurrentMouseOption;
    bool  m_bJustOpenedControlRedefWindow;
    bool  m_EditingControlOptions;
    bool  m_DeleteAllBoundControls;
    bool  m_DeleteAllNextDefine;
    int32 m_OptionToChange;
    char  field_1B10;
    char  field_1B11;
    char  field_1B12;
    char  field_1B13;
    bool  m_CanBeDefined;
    bool  m_JustExitedRedefine;
    char  field_1B16;
    char  field_1B17;
    eHelperText m_nHelperText;
    int32  field_1B1C;
    bool   m_bTexturesRound;
    uint8  m_nNumberOfMenuOptions;
    int16  field_1B22;
    int32  field_1B24;
    bool   m_bViewRadar;
    char   field_1B29;
    int16  field_1B2A;
    int32  m_iRadarVisibilityChangeTime;
    uint32 m_nBriefsArrowBlinkTimeMs;
    int16  field_1B34; // CPad::DisablePlayerControls
    int16  field_1B36;
    int32  field_1B38;
    char   field_1B3C;
    char   field_1B3D;
    char   field_1B3E; // mpack related
    char   field_1B3F;
    uint32 m_nUserTrackScanningTimeMs;
    char   field_1B44;
    char   field_1B45;
    int16  field_1B46;
    uint32 field_1B48;

    union {
        struct {
            uint32 field_1B4C_b1 : 1;
            uint32 bScanningUserTracks : 1;
        };
        int32 field_1B4C;
    };

    int8  m_nBackgroundSprite;
    bool  m_isTextBlinking;
    int16 field_1B52;
    int32 m_lastBlinkTime;
    uint32 m_nTimeHelperTextUpdated;
    bool  ColourSwitch;
    char  field_1B5D;
    int16 field_1B5E;
    int32 LastFlash;
    int32 field_1B64;
    int32 m_nTimeSlideLeftMove;
    int32 m_nTimeSlideRightMove;
    int32 field_1B70;
    int32 field_1B74;

    static int32& nLastMenuPage;

    static inline bool& bInvertMouseX = *(bool*)0xBA6744;
    static inline bool& bInvertMouseY = *(bool*)0xBA6745;

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
    void DrawStandardMenus(uint8);
    void DrawWindow(const CRect& coords, const char* key, uint8 color, CRGBA backColor, bool unused, bool background);
    void DrawWindowedText(float x, float y, float wrap, const char* title, const char* message, eFontAlignment alignment);
    void DrawQuitGameScreen();
    void DrawControllerScreenExtraText(int32); // Original name was 'DrawContollerScreenExtraText' (missing 'r').
    void DrawControllerBound(uint16, bool);
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

    void CheckSliderMovement(int8 value);
    [[nodiscard]] bool CheckFrontEndUpInput() const;
    [[nodiscard]] bool CheckFrontEndDownInput() const;
    [[nodiscard]] bool CheckFrontEndLeftInput() const;
    [[nodiscard]] bool CheckFrontEndRightInput() const;
    void CheckForMenuClosing();
    [[nodiscard]] bool CheckHover(int32 left, int32 right, int32 top, int32 bottom) const;
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
    void ProcessUserInput(bool downPressed, bool upPressed, bool acceptPressed, bool cancelPressed, int8 pressedLR);
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

    //! Simulate that we came into the menu and clicked to load game
    //! @param newGame If we should start a new game
    //! @param slot    Slot of the save-game to load (Ignored if `newGame`)
    void SimulateGameLoad(bool newGame, uint32 slot);
private:
    static void SetBrightness(float brightness, bool arg2);
};

VALIDATE_SIZE(CMenuManager, 0x1B78);

extern CMenuManager& FrontEndMenuManager;
