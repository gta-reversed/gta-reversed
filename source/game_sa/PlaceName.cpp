#include "StdInc.h"

#include "PlaceName.h"

#include "EntryExitManager.h"
#include "Hud.h"

void CPlaceName::InjectHooks() {
    RH_ScopedClass(CPlaceName);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetForMap, 0x571D90);
    RH_ScopedInstall(Process, 0x571F20);
}

// 0x571E90
CPlaceName::CPlaceName() {
    m_pZone = nullptr;
}

// 0x571F10
void CPlaceName::Init() {
    m_pZone = nullptr;
    m_nAdditionalTimer = 0;
}

// 0x571E40
void CPlaceName::ProcessAfterFrontEndShutDown() {
    CHud::m_ZoneState = NAME_DONT_SHOW;
    CHud::m_pLastZoneName = nullptr;
    m_nAdditionalTimer = 250;
}

// 0x571D90
const GxtChar* CPlaceName::GetForMap(float x, float y) {
    CVector posn = CalcPosition();
    CZone* zone = CTheZones::FindSmallestZoneForPosition({ x, y, posn.z }, false);
    return zone->GetTranslatedName();
}

// 0x571F20
void CPlaceName::Process() {
    CVector pos = CalcPosition();
    CZone* smallestZone = CTheZones::FindSmallestZoneForPosition(pos, false);
    if (!smallestZone) {
        Init();
    }

    if ((smallestZone == m_pZone || CGame::currArea == AREA_CODE_1) 
            && m_pZone
            || smallestZone
            && m_pZone // yep, checked twice (Pirulax: gotta account for those cosmis bit flipping rays?)
            && memcmp(smallestZone->m_TextLabel, m_pZone->m_TextLabel, sizeof(smallestZone->m_TextLabel)) == 0
    ) {
        if (m_nAdditionalTimer) {
            m_nAdditionalTimer -= 1;
        }
    } else {
        m_pZone = smallestZone;
        m_nAdditionalTimer = 250;
    }

    Display();
}

// 0x571E60
void CPlaceName::Display() const {
    if (m_pZone) {
        CHud::SetZoneName(m_pZone->GetTranslatedName(), false);
    } else {
        CHud::SetZoneName(nullptr, false);
    }
}

CVector CPlaceName::CalcPosition() {
    auto player = FindPlayerPed();
    if (player->bInVehicle) {
        return player->m_pVehicle->GetPosition();
    } else {
        auto posn = player->GetPosition();
        CEntryExitManager::GetPositionRelativeToOutsideWorld(posn);
        return posn;
    }
}
