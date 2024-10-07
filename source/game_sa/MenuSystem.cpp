#include "StdInc.h"

#include "MenuSystem.h"

uint8& CMenuSystem::num_menus_in_use = *reinterpret_cast<uint8*>(0xBA82E3);
int8& CMenuSystem::CurrentMenuInUse = *(int8*)0xBA82E2;
std::array<CMenuSystem::Menu*, 2> (&MenuNumber) = *(std::array<CMenuSystem::Menu*, 2>(*))0xBA82D8;
std::array<bool, CMenuSystem::MENU_COUNT> (&CMenuSystem::MenuInUse) = *(std::array<bool, CMenuSystem::MENU_COUNT> (*))0xBA82E0;

void CMenuSystem::InjectHooks() {
    RH_ScopedClass(CMenuSystem);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5822D0);
    RH_ScopedInstall(SetActiveMenuItem, 0x5820C0);
    RH_ScopedInstall(SetColumnOrientation, 0x582080);
    RH_ScopedInstall(SetColumnWidth, 0x582050);
    RH_ScopedInstall(GetCarColourFromGrid, 0x5822B0);
    RH_ScopedInstall(CheckForAccept, 0x5807C0);
    RH_ScopedInstall(CheckForSelected, 0x5807E0);
    RH_ScopedInstall(Input, 0x5825D0);
    RH_ScopedInstall(InputStandardMenu, 0x580800, { .reversed = false }); // bad
    RH_ScopedInstall(InputGridMenu, 0x580BD0);
    RH_ScopedInstall(DisplayStandardMenu, 0x580E00, { .reversed = false }); // bad
    RH_ScopedInstall(DisplayGridMenu, 0x5816E0);
    RH_ScopedInstall(Process, 0x582630);
    RH_ScopedInstall(HighlightOneItem, 0x581C10);
    RH_ScopedInstall(InsertOneMenuItemWithNumber, 0x581D70);
    RH_ScopedInstall(CreateNewMenu, 0x582300);
    RH_ScopedInstall(ActivateItems, 0x581990);
    RH_ScopedInstall(ActivateOneItem, 0x581B30);
    RH_ScopedInstall(FillGridWithCarColours, 0x5820E0, { .reversed = false });
    RH_ScopedInstall(InsertMenu, 0x581E00);
    RH_ScopedInstall(SwitchOffMenu, 0x580750);
}

// 0x5822D0
void CMenuSystem::Initialise() {
    for (auto i = 0; i < MENU_COUNT; ++i) {
        if (MenuInUse[i])
            SwitchOffMenu(i);
    }
    CurrentMenuInUse = 0;
}

// 0x5820C0
void CMenuSystem::SetActiveMenuItem(MenuId id, int8 item) {
    MenuNumber[id]->m_nSelectedRow = item;
}

// 0x581C40
void CMenuSystem::SetColumnHeader(MenuId id, uint8 column, Const char* header) {
    assert(column < MENU_COL_COUNT);
    strcpy_s(MenuNumber[id]->m_aacColumnHeaders[column], header ? header : "\0");
}

// NOTSA
void CMenuSystem::SetRowTitle(MenuId id, uint8 column, uint8 row, Const char* title) {
    assert(column < MENU_COL_COUNT);
    strcpy_s(MenuNumber[id]->m_aaacRowTitles[column][row], title ? title : "\0");
}

// 0x582080
void CMenuSystem::SetColumnOrientation(MenuId id, uint8 column, uint8 orientation) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_anColumnAlignment[column] = static_cast<eFontAlignment>(orientation);
}

// 0x582050
void CMenuSystem::SetColumnWidth(MenuId id, uint8 column, uint16 width) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_afColumnWidth[column] = float(width);
}

// 0x5820A0, unused
void CMenuSystem::SetHeaderOrientation(MenuId id, uint8 column, uint8 orientation) {
    assert(column < MENU_COL_COUNT);
    MenuNumber[id]->m_anColumnHeaderAlignment[column] = static_cast<eFontAlignment>(orientation);
}

// 0x581C90, unused
void CMenuSystem::SetDPadInput(MenuId id, uint8 input) {
    MenuNumber[id]->m_abColumnInteractive[INTERACTIVE_DPAD] = input;
}

// 0x5822B0
uint8 CMenuSystem::GetCarColourFromGrid(MenuId id, uint8 colorId) {
    assert(colorId < MENU_CAR_COLOR_COUNT);
    return MenuNumber[id]->m_anUsedCarColors[colorId];
}

// 0x581CB0, unused
void CMenuSystem::GetMenuPosition(MenuId id, float* outX, float* outY) {
    assert(outX && outY);
    *outX = MenuNumber[id]->m_vPosn.x;
    *outY = MenuNumber[id]->m_vPosn.y;
}

// 0x5807C0
MenuId CMenuSystem::CheckForAccept(MenuId id) {
    return MenuInUse[id] ? MenuNumber[id]->m_nAcceptedRow : MENU_UNDEFINED;
}

// 0x5807E0
MenuId CMenuSystem::CheckForSelected(MenuId id) {
    return MenuInUse[id] ? MenuNumber[id]->m_nSelectedRow : MENU_UNDEFINED;
}

// 0x5825D0
void CMenuSystem::Input(MenuId id) {
    switch (MenuNumber[id]->m_nType) {
    case MENU_TYPE_DEFAULT:
        return InputStandardMenu(id);
    case MENU_TYPE_GRID:
        return InputGridMenu(id);
    }
}

// 0x580800
void CMenuSystem::InputStandardMenu(MenuId id) {
    return plugin::Call<0x580800, MenuId>(id);

    auto menu = MenuNumber[id];

    if (CurrentMenuInUse < 0) {
        CurrentMenuInUse = 1;
    }

    if (!MenuInUse[CurrentMenuInUse]) {
        do {
            if (CurrentMenuInUse <= 0)
                break;
            --CurrentMenuInUse;
        } while (!MenuInUse[CurrentMenuInUse]);
    }

    if (CurrentMenuInUse >= 2) {
        CurrentMenuInUse = 0;
    }

    if (!MenuInUse[CurrentMenuInUse]) {
        do {
            if (CurrentMenuInUse >= 2)
                break;
            ++CurrentMenuInUse;
        } while (!MenuInUse[CurrentMenuInUse]);
    }

    auto pad = CPad::GetPad();

    if (pad->IsTrianglePressed() && !CTimer::GetIsPaused()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK, 0.0f, 1.0f);
    }

    if (pad->IsCrossPressed() || CTimer::GetIsPaused() && CPad::IsReturnJustPressed()) {
        if (!CTimer::GetIsPaused())
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0f, 1.0f);

        if (menu->m_abRowSelectable[menu->m_nSelectedRow])
            menu->m_nAcceptedRow = menu->m_nSelectedRow;
    }

    if (menu->m_nAcceptedRow != menu->m_nSelectedRow)
        menu->m_nAcceptedRow = MENU_UNDEFINED;

    if (menu->m_nNumRows <= 1)
        return;

    if (CPad::GetAnaloguePadLeft() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadUpPressed() || CTimer::GetIsPaused() && CPad::IsUpPressed()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);

        do {
            menu->m_nSelectedRow -= 1;
        } while ((!menu->m_abRowSelectable[menu->m_nSelectedRow] || !menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0]) && menu->m_nSelectedRow >= 0);
    }

    if (CPad::GetAnaloguePadDown() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadDownPressed() || CTimer::GetIsPaused() && CPad::IsDownPressed()) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
        ++menu->m_nSelectedRow;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }

            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;

            menu->m_nSelectedRow += 1;
        }
    }

    if (menu->m_nSelectedRow < 0) {
        auto m_nNumRows = menu->m_nNumRows;
        do {
            menu->m_nSelectedRow -= 1;
        } while ((!menu->m_abRowSelectable[m_nNumRows] || !menu->m_aaacRowTitles[0][m_nNumRows][0]) && m_nNumRows >= 0);
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows) {
        menu->m_nSelectedRow = 0;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }

            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;

            menu->m_nSelectedRow += 1;
        }
    }
}

// Here we optimized calls of CPad::GetPad
// 0x580BD0
void CMenuSystem::InputGridMenu(MenuId id) {
    auto menu = MenuNumber[id];
    auto pad = CPad::GetPad();

    if (pad->IsCrossPressed() || CTimer::GetIsPaused() && CPad::IsReturnJustPressed()) {
        if (menu->m_abRowSelectable[menu->m_nSelectedRow])
            menu->m_nAcceptedRow = menu->m_nSelectedRow;
    }

    if (menu->m_nAcceptedRow != menu->m_nSelectedRow)
        menu->m_nAcceptedRow = UNDEFINED;

    if (CPad::GetAnaloguePadLeft() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadUpPressed()) {
        if (menu->m_nSelectedRow >= menu->m_nNumColumns)
            menu->m_nSelectedRow -= menu->m_nNumColumns;
    }

    if (CPad::GetAnaloguePadDown() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadDownPressed()) {
        if (menu->m_nSelectedRow < menu->m_nNumRows - menu->m_nNumColumns)
            menu->m_nSelectedRow += menu->m_nNumColumns;
    }

    if (CPad::GetAnaloguePadUp() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadLeftPressed()) {
        if (menu->m_nSelectedRow > 0)
            menu->m_nSelectedRow -= 1;
    }

    if (CPad::GetAnaloguePadRight() || menu->m_abColumnInteractive[INTERACTIVE_DPAD] && pad->IsDPadRightPressed()) {
        if (menu->m_nSelectedRow < menu->m_nNumRows - 1)
            menu->m_nSelectedRow += 1;
    }
}

// 0x582600, unused
void CMenuSystem::Display(MenuId id, uint8 unk) {
    switch (MenuNumber[id]->m_nType) {
    case MENU_TYPE_DEFAULT:
        return DisplayStandardMenu(id, unk);
    case MENU_TYPE_GRID:
        return DisplayGridMenu(id, unk);
    }
}

// 0x580E00
void CMenuSystem::DisplayStandardMenu(MenuId id, bool bRightFont /*bBrightFont*/) {
    return plugin::Call<0x580E00, MenuId, bool>(id, bRightFont);

    auto menu = MenuNumber[id];
    uint8 titleDarkness = bRightFont ? 0 : 120;

    if (menu->m_bColumnBackground) {
        float width = 0, height = 50;
        for (auto i = 0; i < menu->m_nNumColumns; i++) {
            width += menu->m_afColumnWidth[i];
            if (menu->m_aacColumnHeaders[i][0]) {
                height = 70;
            }
        }
        width  = width;
        height = SCREEN_STRETCH_Y(height) + SCREEN_STRETCH_Y(float(menu->m_nNumRows) * 16.f);

        CRect rect(menu->m_vPosn.x, menu->m_vPosn.y, menu->m_vPosn.x + width, menu->m_vPosn.y + height);
        FrontEndMenuManager.DrawWindow(rect, menu->m_szTitle, titleDarkness, CRGBA(0, 0, 0, 190), false, true);
    }
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetScale(SCREEN_STRETCH_X(0.52f), SCREEN_STRETCH_Y(1.0f));
    CFont::SetDropShadowPosition(2);
    CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);

    // 0x580FDD
    for (auto column = 0; column < menu->m_nNumColumns; column++) {
        if (!menu->m_aacColumnHeaders[0][column])
            continue;

        auto alignment = static_cast<int32>(menu->m_anColumnHeaderAlignment[column]) < 0 ? eFontAlignment::ALIGN_CENTER : menu->m_anColumnHeaderAlignment[0];
        CFont::SetOrientation(alignment);
        CFont::SetColor(CRGBA(225 - titleDarkness, 225 - titleDarkness, 225 - titleDarkness, 255));

        float width = column ? menu->m_afColumnWidth[column] : 0.0f;
        float x = SCREEN_STRETCH_X(10.0f) + menu->m_vPosn.x + width;
        switch (alignment) {
        case eFontAlignment::ALIGN_CENTER: {
            x += menu->m_afColumnWidth[column] / 2.0f;
            break;
        }
        case eFontAlignment::ALIGN_LEFT: {
            break;
        }
        case eFontAlignment::ALIGN_RIGHT: {
            x += menu->m_afColumnWidth[column];
            break;
        }
        default:
            NOTSA_UNREACHABLE();
        }
        float y = SCREEN_STRETCH_Y(20.0f) + menu->m_vPosn.y;

        const auto columnText = menu->m_aacColumnHeaders[column];
        const auto text = TheText.Get(columnText);
        CFont::PrintString(x, y, text);
    }

    auto fBaseY = SCREEN_STRETCH_Y(20.0f);

    const auto GetColor = [=](auto row) -> CRGBA {
        if (menu->m_abColumnInteractive[INTERACTIVE_DEFAULT]) {
            if (row == menu->m_nSelectedRow && menu->m_abRowSelectable[row]) {
                return HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE);
            } else if (menu->m_abRowAlreadyBought[row]) {
                return HudColour.GetRGB(HUD_COLOUR_GREEN);
            } else if (menu->m_abRowSelectable[row]) {
                return HudColour.GetRGB(HUD_COLOUR_NIGHT_BLUE);
            }
            return HudColour.GetRGB(HUD_COLOUR_DARK_GRAY);
        }

        if (menu->m_abRowAlreadyBought[row]) {
            return HudColour.GetRGB(HUD_COLOUR_GREEN);
        } else if (menu->m_abRowSelectable[row]) {
            return HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE);
        }
        return HudColour.GetRGB(HUD_COLOUR_NIGHT_BLUE);
    };

    // 0x581216
    GxtChar buffer[400];
    // draw rows
    for (auto row = 0; row < menu->m_nNumRows; row++) {
        for (auto column = 0; column < menu->m_nNumColumns; column++) {
            if (!menu->m_aaacRowTitles[0][row][0])
                continue;

            CFont::SetColor(GetColor(row)); // NOTSA | optimized
            // formatting numbers in row titles
            CMessages::InsertNumberInString(TheText.Get(menu->m_aaacRowTitles[0][row]), menu->m_aanNumberInRowTitle[0][row], menu->m_aadw2ndNumberInRowTitle[0][row], -1, -1, -1, -1, buffer);
            CFont::SetOrientation(menu->m_anColumnAlignment[row]);
            CMessages::InsertPlayerControlKeysInString(buffer);

            auto _panelWidthTotal = 0.52f;
            CFont::SetScale(SCREEN_STRETCH_X(0.52f), SCREEN_STRETCH_Y(1.0f));
            while (CFont::GetStringWidth(buffer, true, false) > menu->m_afColumnWidth[row]) {
                _panelWidthTotal -= 0.05f;
                CFont::SetScale(SCREEN_STRETCH_X(_panelWidthTotal), SCREEN_STRETCH_Y(1.0f)); // make text smaller :)
            }

            // add previous columns width
            /*
            auto v41 = 0;
            if (column) //
            {
                v42 = menu->m_afColumnWidth;
                v43 = v34;
                do {
                    v41 = (unsigned __int64)((double)(unsigned __int16)v41 + *v42++);
                    --v43;
                } while (v43);
            }
            */
            fBaseY += SCREEN_STRETCH_Y(16.0f);
            float x1, y2;

            switch (menu->m_anColumnAlignment[row]) {
            case eFontAlignment::ALIGN_CENTER:
                x1 = SCREEN_STRETCH_X(10.0f) + menu->m_afColumnWidth[row] + menu->m_vPosn.x + menu->m_afColumnWidth[column];
                y2 = fBaseY + menu->m_vPosn.y;
                break;
            case eFontAlignment::ALIGN_LEFT:
                x1 = SCREEN_STRETCH_X(10.0f) + menu->m_afColumnWidth[column] + menu->m_vPosn.x;
                y2 = fBaseY + menu->m_vPosn.y;
                break;
            case eFontAlignment::ALIGN_RIGHT:
                x1 = SCREEN_STRETCH_X(10.0f) + menu->m_afColumnWidth[row] / 2.0f + menu->m_vPosn.x + menu->m_afColumnWidth[column];
                y2 = fBaseY + menu->m_vPosn.y;
                break;
            default:
                NOTSA_UNREACHABLE();
            }
            CFont::PrintString(x1, y2, buffer);
        }
    }
}

// 0x5816E0
void CMenuSystem::DisplayGridMenu(MenuId id, bool bFade) {
    auto menu = MenuNumber[id];

    auto columnWidth = menu->m_afColumnWidth[0];
    if (menu->m_bColumnBackground) {
        CRect rect;
        rect.left   = menu->m_vPosn.x;
        rect.bottom    = menu->m_vPosn.y;
        rect.right  = (float)menu->m_nNumColumns * columnWidth + menu->m_vPosn.x;
        rect.top = (float)menu->m_nNumColumns * columnWidth + menu->m_vPosn.y;
        FrontEndMenuManager.DrawWindow(rect, nullptr, bFade ? 0 : 120, { 0, 0, 0, 190 }, false, true);
    }

    auto index = 0;
    for (auto c = 0; c < menu->m_nNumColumns; c++) {
        for (auto r = 0; r < menu->m_nNumColumns; r++) {
            // white border
            if (index == menu->m_nSelectedRow) {
                CRect rect;
                rect.left   = (float)r * columnWidth + menu->m_vPosn.x;
                rect.bottom    = (float)c * columnWidth + menu->m_vPosn.y;
                rect.right  = ((float)r + 1.0f) * columnWidth + menu->m_vPosn.x;
                rect.top = ((float)c + 1.0f) * columnWidth + menu->m_vPosn.y;
                CSprite2d::DrawRect(rect, { 225, 225, 225, 255 });
            }

            // color
            auto  colorIndex = GetCarColourFromGrid(id, index);
            auto& color = CVehicleModelInfo::ms_vehicleColourTable[colorIndex];
            CRect rect;
            rect.left   = (float)r * columnWidth + menu->m_vPosn.x + SCREEN_STRETCH_X(3.0f);
            rect.bottom    = (float)c * columnWidth + menu->m_vPosn.y + SCREEN_STRETCH_Y(3.0f);
            rect.right  = (float)r * columnWidth + menu->m_vPosn.x + columnWidth - SCREEN_STRETCH_X(3.0f);
            rect.top = (float)c * columnWidth + menu->m_vPosn.y + columnWidth - SCREEN_STRETCH_Y(3.0f);
            CSprite2d::DrawRect(rect, { color.r, color.g, color.b, 255 });
            index++;
        }
    }
}

// 0x582630
void CMenuSystem::Process(int8 id) {
    if (id != (int8)MENU_UNDEFINED) {
        if (MenuInUse[id]) {
            Display(id, 1);
            if (MenuNumber[id]->m_abColumnInteractive[INTERACTIVE_DEFAULT])
                Input(id);
        }
        return;
    }

    // 0x58263F :thinking
    for (auto i = 0; i < MENU_COUNT; i++) {
        if (i != CurrentMenuInUse && MenuInUse[i]) {
            Display(id, false);
        }
    }

    // 0x5826A4
    if (MenuInUse[CurrentMenuInUse]) {
        Display(CurrentMenuInUse, true);

        if (MenuNumber[CurrentMenuInUse]->m_abColumnInteractive[INTERACTIVE_DEFAULT]) {
            Input(CurrentMenuInUse);
        }
    }
}

// 0x581C10
void CMenuSystem::HighlightOneItem(MenuId id, uint8 item, bool bought) {
    MenuNumber[id]->m_abRowAlreadyBought[item] = bought;
    MenuNumber[id]->m_abRowSelectable[item] = true;
}

// 0x581CE0, unused
void CMenuSystem::InsertOneMenuItem(MenuId id, uint8 column, uint8 row, const char* text) {
    auto* menu = MenuNumber[id];
    SetRowTitle(id, column, row, text);
    menu->m_aanNumberInRowTitle[column][row] = -1;
    menu->m_aadw2ndNumberInRowTitle[column][row] = -1;

    CalcNonEmptyRows(id);
}

// 0x581D70
void CMenuSystem::InsertOneMenuItemWithNumber(MenuId id, uint8 column, uint8 row, const char* text, int32 num1, int32 num2) {
    auto* menu = MenuNumber[id];
    SetRowTitle(id, column, row, text);
    menu->m_aanNumberInRowTitle[column][row] = num1;
    menu->m_aadw2ndNumberInRowTitle[column][row] = num2;

    CalcNonEmptyRows(id);
}

// 0x582300
MenuId CMenuSystem::CreateNewMenu(eMenuType type, const char* title, float x, float y, float width, uint8 columns, bool interactive, bool background, eFontAlignment alignment) {
    assert(num_menus_in_use < MENU_COUNT);

    MenuId menuId = GetNumMenusInUse();
    assert(!MenuInUse[menuId]);
    MenuNumber[menuId] = new Menu();
    MenuInUse[menuId] = true;
    Menu* menu = MenuNumber[menuId];
    menu->m_nType = type;

    switch (type) {
    case MENU_TYPE_DEFAULT:
        menu->m_nNumRows = 0;
        menu->m_nNumColumns = std::min(columns, (uint8)MENU_COL_COUNT);
        menu->m_nSelectedRow = 0;
        menu->m_abColumnInteractive[INTERACTIVE_DEFAULT] = interactive;
        menu->m_abColumnInteractive[INTERACTIVE_DPAD] = true; // DPAD
        menu->m_vPosn = { x, y };
        menu->m_bColumnBackground = background != 0;
        if (title) {
            strcpy_s(menu->m_szTitle, title);
        } else {
            menu->m_szTitle[0] = '\0';
        }
        menu->m_nAcceptedRow = MENU_UNDEFINED;
        ActivateItems(menuId, true, true, true, true, true, true, true, true, true, true, true, true);
        for (auto i = 0; i < MENU_COL_COUNT; ++i) {
            menu->m_afColumnWidth[i] = width;
            menu->m_anColumnAlignment[i] = alignment;
            menu->m_anColumnHeaderAlignment[i] = eFontAlignment::ALIGN_UNDEFINED;
            menu->m_aacColumnHeaders[i][0] = '\0';
            for (auto j = 0; j < MENU_ROW_COUNT; ++j) {
                menu->m_aaacRowTitles[i][j][0] = '\0';
                menu->m_abRowAlreadyBought[j] = 0;
            }
        }
        break;
    case MENU_TYPE_GRID:
        menu->m_afColumnWidth[0] = width;
        menu->m_bColumnBackground = background != 0;
        menu->m_abColumnInteractive[INTERACTIVE_DEFAULT] = interactive;
        menu->m_abColumnInteractive[INTERACTIVE_DPAD] = true;
        menu->m_nNumColumns = std::min(columns, (uint8)8);
        menu->m_vPosn = { x, y };
        menu->m_nAcceptedRow = MENU_UNDEFINED;
        menu->m_nSelectedRow = 0;
        menu->m_nNumRows = menu->m_nNumColumns * menu->m_nNumColumns;
        FillGridWithCarColours(menuId);
        break;
    }

    ++num_menus_in_use;
    return menuId;
}

// 0x581990
void CMenuSystem::ActivateItems(MenuId id, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10, bool b11) {
    auto* menu = MenuNumber[id];

    const bool rows[] = { b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11 };
    for (auto rowId = 0u; rowId < std::size(rows); rowId++) {
        menu->m_abRowSelectable[rowId] = rows[rowId];
    }

    for (auto i = 0; i < MENU_COUNT; ++i) {
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }
            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;
            ++menu->m_nSelectedRow;
        }
        if (menu->m_nSelectedRow >= menu->m_nNumRows) {
            menu->m_nSelectedRow = 0;
        }
    }
}

// 0x581B30, untested
void CMenuSystem::ActivateOneItem(MenuId id, uint8 row, bool enable) {
    // plugin::Call<0x581B30, MenuId, uint8, uint8>(id, row, enable);

    auto* menu = MenuNumber[id];

    menu->m_abRowSelectable[row] = enable;
    while (true) {
        if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
            if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                break;
        }
        if (menu->m_nSelectedRow >= menu->m_nNumRows)
            break;
        menu->m_nSelectedRow += 1;
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows) {
        menu->m_nSelectedRow = 0;
        while (true) {
            if (menu->m_abRowSelectable[menu->m_nSelectedRow]) {
                if (menu->m_aaacRowTitles[0][menu->m_nSelectedRow][0])
                    break;
            }
            if (menu->m_nSelectedRow >= menu->m_nNumRows)
                break;
            menu->m_nSelectedRow += 1;
        }
    }

    if (menu->m_nSelectedRow >= menu->m_nNumRows)
        menu->m_nSelectedRow = 0;
}

// 0x5820E0
void CMenuSystem::FillGridWithCarColours(MenuId id) {
    plugin::Call<0x5820E0, MenuId>(id);
}

// Insert menu column
// 0x581E00
void CMenuSystem::InsertMenu(MenuId id, uint8 column, char* colHeader, char* row0, char* row1, char* row2, char* row3, char* row4, char* row5, char* row6, char* row7, char* row8, char* row9, char* row10, char* row11) {
    assert(column < MENU_COL_COUNT);
    auto* menu = MenuNumber[id];

    SetColumnHeader(id, column, colHeader);

    const auto rows = std::array{ row0, row1, row2, row3, row4, row5, row6, row7, row8, row9, row10, row11 };
    for (auto rowId = 0u; rowId < std::size(rows); rowId++) {
        SetRowTitle(id, column, rowId, rows[rowId]);
    }

    for (auto i = 0; i < MENU_COL_COUNT; ++i) {
        for (auto j = 0; j < MENU_ROW_COUNT; ++j) {
            menu->m_aanNumberInRowTitle[i][j] = -1;
            menu->m_aadw2ndNumberInRowTitle[i][j] = -1;
        }
    }

    if (column)
        return;

    CalcNonEmptyRows(id);
}

// 0x580750
void CMenuSystem::SwitchOffMenu(MenuId id) {
    if (!MenuInUse[id])
        return;

    delete MenuNumber[id];
    MenuInUse[id] = false;
    num_menus_in_use--;
    CurrentMenuInUse = 0;

    for (auto i = 0; i < MENU_COUNT; ++i) {
        if (MenuInUse[i]) {
            CurrentMenuInUse = i;
            break;
        }
    }
}

// NOTSA
void CMenuSystem::CalcNonEmptyRows(MenuId id) {
    auto* menu = MenuNumber[id];
    menu->m_nNumRows = 0;
    for (auto& row : menu->m_aaacRowTitles[0]) { // they used reverse loop from MENU_ROW_COUNT to 0
        if (row[0]) {
            menu->m_nNumRows += 1;
        }
    }
}
