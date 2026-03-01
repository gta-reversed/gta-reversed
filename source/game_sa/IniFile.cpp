#include "StdInc.h"

#include "IniFile.h"
#include "CarCtrl.h"

float& CIniFile::PedNumberMultiplier = *(float*)0x8CDF14;
float& CIniFile::CarNumberMultiplier = *(float*)0x8CDF18;

void CIniFile::InjectHooks() {
    RH_ScopedClass(CIniFile);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadIniFile, 0x56D070);
}

// 0x56D070
void CIniFile::LoadIniFile() {
    ZoneScoped;

    CFileMgr::SetDir("");
    auto file = CFileMgr::OpenFile("gta3.ini", "r");
    if (file) {
        CFileMgr::ReadLine(file, gString, 200);
        VERIFY(sscanf_s(gString, "%f", &PedNumberMultiplier) == 1);
        if (PedNumberMultiplier >= 0.5f) {
            if (PedNumberMultiplier > 3.0f)
                PedNumberMultiplier = 3.0f;
        } else {
            PedNumberMultiplier = 0.5f;
        }

        CFileMgr::ReadLine(file, gString, 200);
        VERIFY(sscanf_s(gString, "%f", &CarNumberMultiplier) == 1);
        if (CarNumberMultiplier >= 0.5f) {
            if (CarNumberMultiplier > 3.0f)
                CarNumberMultiplier = 3.0f;
        } else {
            CarNumberMultiplier = 0.5f;
        }
        CFileMgr::CloseFile(file);
    }
    CPopulation::MaxNumberOfPedsInUse = 25;
    CCarCtrl::MaxNumberOfCarsInUse = 12;
}
