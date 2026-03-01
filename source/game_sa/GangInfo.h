/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CGangInfo {
public:
    CGangInfo(); // 0x5DE520
    ~CGangInfo() = default; // 0x5DE540

    //! Get the number unique weapon types the gang may have
    size_t GetNumOfWeaponChoices() const;

    //! Get a random weapon (out of the weapons the gang may have)
    eWeaponType GetRandomWeapon() const;

    //! Set the weapons this gang may use
    void SetWeapons(std::array<eWeaponType, 3> weapons) { m_nGangWeapons = weapons; }

public:
    int8                       m_nPedModelOverride = -1;
    std::array<eWeaponType, 3> m_nGangWeapons{};
};
VALIDATE_SIZE(CGangInfo, 0x10);
