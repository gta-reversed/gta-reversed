/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Localisation.h"

bool& CLocalisation::nastyGame = *(bool*)0xB9B7EC;
bool& CLocalisation::germanGame = *(bool*)0xB9B7ED;
bool& CLocalisation::frenchGame = *(bool*)0xB9B7EE;

void CLocalisation::InjectHooks() {
    RH_ScopedClass(CLocalisation);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x56D180);
    RH_ScopedInstall(SetNormalGame, 0x56D1A0);
    RH_ScopedInstall(SetFrenchGame, 0x56D1C0);
    RH_ScopedInstall(SetGermanGame, 0x56D1E0);
    RH_ScopedInstall(GermanGame, 0x56D200);
    RH_ScopedInstall(FrenchGame, 0x56D210);
    RH_ScopedInstall(Metric, 0x56D220);
    RH_ScopedInstall(Blood, 0x56D230);
    RH_ScopedInstall(Porn, 0x56D240);
    RH_ScopedInstall(ScreamsFromKills, 0x56D250);
    RH_ScopedInstall(Prostitutes, 0x56D260);
    RH_ScopedInstall(KickingWhenDown, 0x56D270);
    RH_ScopedInstall(ShootLimbs, 0x56D280);
    RH_ScopedInstall(KnockDownPeds, 0x56D290);
    RH_ScopedInstall(KillFrenzy, 0x56D2A0);
    RH_ScopedInstall(StealFromDeadPed, 0x56D2B0);
    RH_ScopedInstall(KillPeds, 0x56D2C0);
    RH_ScopedInstall(PedsOnFire, 0x56D2D0);
}

// 0x56D180
void CLocalisation::Initialise() {
    nastyGame = true;
    germanGame = false;
    frenchGame = false;
}

// 0x56D1A0
void CLocalisation::SetNormalGame() {
    nastyGame = true;
    frenchGame = false;
    germanGame = false;
}

// 0x56D1C0
void CLocalisation::SetFrenchGame() {
    frenchGame = true;
    germanGame = false;
    nastyGame = true;
}

// 0x56D1E0
void CLocalisation::SetGermanGame() {
    germanGame = true;
    frenchGame = false;
    nastyGame = false;
}

// 0x56D200
bool CLocalisation::GermanGame() {
    return germanGame;
}

// 0x56D210
bool CLocalisation::FrenchGame() {
    return frenchGame;
}

// 0x56D220
bool CLocalisation::Metric() {
    return FrontEndMenuManager.m_nPrefsLanguage != eLanguage::ENGLISH;
}

// 0x56D230
bool CLocalisation::Blood() {
    return true;
}

// 0x56D240
bool CLocalisation::Porn() {
    return true;
}

// 0x56D250
bool CLocalisation::ScreamsFromKills() {
    return !germanGame;
}

// 0x56D260
bool CLocalisation::Prostitutes() {
    return true;
}

// 0x56D270
bool CLocalisation::KickingWhenDown() {
    return true;
}

// 0x56D280
bool CLocalisation::ShootLimbs() {
    return nastyGame;
}

// 0x56D290
bool CLocalisation::KnockDownPeds() {
    return nastyGame;
}

// 0x56D2A0
bool CLocalisation::KillFrenzy() {
    return nastyGame;
}

// 0x56D2B0
bool CLocalisation::StealFromDeadPed() {
    return nastyGame;
}

// 0x56D2C0
bool CLocalisation::KillPeds() {
    return nastyGame;
}

// 0x56D2D0
bool CLocalisation::PedsOnFire() {
    return nastyGame;
}
