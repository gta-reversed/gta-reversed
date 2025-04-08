/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#ifdef InsertMenu
#undef InsertMenu
#endif

typedef uint8 MenuId;

class CMenuSystem {
public:
    enum {
        MENU_COL_COUNT       = 4,
        MENU_ROW_COUNT       = 12,
        MENU_COUNT           = 2,
        MENU_CAR_COLOR_COUNT = 64,

        MENU_UNDEFINED       = -99,
    };

    enum eMenuType : uint8 {
        MENU_TYPE_DEFAULT = 0,
        MENU_TYPE_GRID    = 1,
    };

    enum eColumnInteractive {
        INTERACTIVE_DEFAULT = 0,
        INTERACTIVE_DPAD    = 1,
    };

    struct Menu {
        uint8          m_anUsedCarColors[MENU_CAR_COLOR_COUNT];
        eMenuType      m_nType;
        char           m_aaacRowTitles[MENU_COL_COUNT][MENU_ROW_COUNT][10];
        int32          m_aanNumberInRowTitle[MENU_COL_COUNT][MENU_ROW_COUNT];
        int32          m_aadw2ndNumberInRowTitle[MENU_COL_COUNT][MENU_ROW_COUNT];
        char           m_aacColumnHeaders[MENU_COL_COUNT][10];
        char           m_szTitle[10];
        bool           m_abRowSelectable[MENU_ROW_COUNT];
        uint8          m_abRowAlreadyBought[MENU_ROW_COUNT];
        eFontAlignment m_anColumnAlignment[MENU_COL_COUNT];
        eFontAlignment m_anColumnHeaderAlignment[MENU_COL_COUNT];
        uint8          m_nNumRows;
        uint8          m_nNumColumns;
        bool           m_abColumnInteractive[MENU_COL_COUNT];
        float          m_afColumnWidth[MENU_COL_COUNT];
        CVector2D      m_vPosn;
        bool           m_bColumnBackground;
        MenuId         m_nSelectedRow;
        MenuId         m_nAcceptedRow;
    };

    static std::array<bool, MENU_COUNT> (&MenuInUse);
    static int8&                        CurrentMenuInUse;
    static uint8&                       num_menus_in_use; // OG name

public:
    static void InjectHooks();

    static void Initialise();
    static void Process(int8 menu = MENU_UNDEFINED);

    static MenuId CheckForAccept(MenuId id);
    static MenuId CheckForSelected(MenuId id);

    static void Input(MenuId id);
    static void InputStandardMenu(MenuId id);
    static void InputGridMenu(MenuId id);
    static void Display(MenuId id, uint8 unk);
    static void DisplayStandardMenu(MenuId id, bool bRightFont);
    static void DisplayGridMenu(MenuId id, bool bFade);

    static MenuId CreateNewMenu(eMenuType type, const char* title, float x, float y, float width, uint8 columns, bool interactive, bool background, eFontAlignment alignment);
    static void ActivateItems(MenuId id, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10, bool b11, bool b12);
    static void ActivateOneItem(MenuId id, uint8 rowId, bool enable);
    static void InsertMenu(MenuId id, uint8 column, char* colHeader, char* row0 = nullptr, char* row1 = nullptr, char* row2 = nullptr, char* row3 = nullptr,
                           char* row4 = nullptr, char* row5 = nullptr, char* row6 = nullptr, char* row7 = nullptr, char* row8 = nullptr, char* row9 = nullptr,
                           char* row10 = nullptr, char* row11 = nullptr);
    static void FillGridWithCarColours(MenuId id);

    static void HighlightOneItem(MenuId id, uint8 item, bool bought);
    static void InsertOneMenuItem(MenuId id, uint8 column, uint8 row, const char* text);
    static void InsertOneMenuItemWithNumber(MenuId id, uint8 column, uint8 row, const char* text, int32 num1, int32 num2);
    static void SwitchOffMenu(MenuId id);

    static void SetActiveMenuItem(MenuId id, int8 item);
    static void SetColumnHeader(MenuId id, uint8 column, Const char* header);
    static void SetRowTitle(MenuId id, uint8 column, uint8 row, Const char* title);
    static void SetColumnOrientation(MenuId id, uint8 column, uint8 orientation);
    static void SetColumnWidth(MenuId id, uint8 column, uint16 width);
    static void SetHeaderOrientation(MenuId id, uint8 column, uint8 orientation);
    static void SetDPadInput(MenuId id, uint8 nbDPadInput);

    static uint8 GetCarColourFromGrid(MenuId id, uint8 colorId);
    static void  GetMenuPosition(MenuId id, float* outX, float* outY);

    static MenuId GetNumMenusInUse() noexcept { return num_menus_in_use; } // 0x558DC0

private:
    static void CalcNonEmptyRows(MenuId id);
};

VALIDATE_SIZE(CMenuSystem::Menu, 0x418);
