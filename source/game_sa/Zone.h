/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "rw/rwplcore.h" // for RwRGBA struct
#include "eLevelName.h"

enum eZoneType : uint8 {
    ZONE_TYPE_NAVI,
    ZONE_TYPE_LOCAL_NAVI,
    ZONE_TYPE_INFO,
    ZONE_TYPE_MAP,
};

class CZone {
public:
    auto GetTranslatedName() const -> const GxtChar*;
    auto GetInfoLabel()      const { return std::string_view{ m_InfoLabel }; }
    auto GetNaviLabel()      const { return std::string_view{ m_TextLabel }; }
    auto GetRect()           const { return CRect{ (float)m_fX1, (float)m_fY1, (float)m_fX2, (float)m_fY2 }; }
    auto GetBB()             const { return CBox{ {(float)m_fX1, (float)m_fY1, (float)m_fZ1}, {(float)m_fX2, (float)m_fY2, (float)m_fZ2} }; }

public:
    char       m_InfoLabel[8]{}; // Zone info `TheText.Get` key (Unsure)
    char       m_TextLabel[8]{}; // Display name `TheText.Get` key
    int16      m_fX1{};
    int16      m_fY1{};
    int16      m_fZ1{};
    int16      m_fX2{};
    int16      m_fY2{};
    int16      m_fZ2{};
    int16      m_nZoneExtraIndexInfo{};
    eZoneType  m_nType{};
    eLevelName m_nLevel{};


};
VALIDATE_SIZE(CZone, 0x20);
