#include "StdInc.h"

#include <unordered_map>
#include "CDarkelDebugModule.h"
#include "imgui.h"
#include "Timer.h"

using namespace ImGui;

constexpr const char* WEAPON_TYPE_STRING[] = {
    "UNARMED", "BRASSKNUCKLE", "GOLFCLUB", "NIGHTSTICK", "KNIFE", "BASEBALLBAT", "SHOVEL", "POOL_CUE", "KATANA", "CHAINSAW", "DILDO1", "DILDO2", "VIBE1", "VIBE2", "FLOWERS", "CANE", "GRENADE",
    "TEARGAS", "MOLOTOV", "ROCKET", "ROCKET_HS", "FREEFALL_BOMB", "PISTOL", "PISTOL_SILENCED", "DESERT_EAGLE", "SHOTGUN", "SAWNOFF_SHOTGUN", "SPAS12_SHOTGUN", "MICRO_UZI", "MP5", "AK47", "M4",
    "TEC9", "COUNTRYRIFLE", "SNIPERRIFLE", "RLAUNCHER", "RLAUNCHER_HS", "FLAMETHROWER", "MINIGUN", "REMOTE_SATCHEL_CHARGE", "DETONATOR", "SPRAYCAN", "EXTINGUISHER", "CAMERA", "NIGHTVISION",
    "INFRARED", "PARACHUTE", "LAST_WEAPON", "ARMOUR", "RAMMEDBYCAR", "RUNOVERBYCAR", "EXPLOSION", "UZI_DRIVEBY", "DROWNING", "FALL", "UNIDENTIFIED", "ANYMELEE", "ANYWEAPON", "FLARE" "UNARMED",
    "BRASSKNUCKLE", "GOLFCLUB", "NIGHTSTICK", "KNIFE", "BASEBALLBAT", "SHOVEL", "POOL_CUE", "KATANA", "CHAINSAW", "DILDO1", "DILDO2", "VIBE1", "VIBE2", "FLOWERS", "CANE", "GRENADE", "TEARGAS",
    "MOLOTOV", "ROCKET", "ROCKET_HS", "FREEFALL_BOMB", "PISTOL", "PISTOL_SILENCED", "DESERT_EAGLE", "SHOTGUN", "SAWNOFF_SHOTGUN", "SPAS12_SHOTGUN", "MICRO_UZI", "MP5", "AK47", "M4", "TEC9",
    "COUNTRYRIFLE", "SNIPERRIFLE", "RLAUNCHER", "RLAUNCHER_HS", "FLAMETHROWER", "MINIGUN", "REMOTE_SATCHEL_CHARGE", "DETONATOR", "SPRAYCAN", "EXTINGUISHER", "CAMERA", "NIGHTVISION", "INFRARED",
    "PARACHUTE", "LAST_WEAPON", "ARMOUR", "RAMMEDBYCAR", "RUNOVERBYCAR", "EXPLOSION", "UZI_DRIVEBY", "DROWNING", "FALL", "UNIDENTIFIED", "ANYMELEE", "ANYWEAPON", "FLARE"
};

void DarkelDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Darkel Debug", {860.f, 290.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    BeginGroup();
    Text(
        "pStartMessage                 = '%s'\n"
        "AmmoInterruptedWeapon         = %u\n"
        "InterruptedWeaponType         = %s\n"
        "InterruptedWeaponTypeSelected = %s\n"
        "TimeOfFrenzyStart             = %u\n"
        "PreviousTime                  = %d\n"
        "TimeLimit                     = %d\n"
        "KillsNeeded                   = %d\n"
        "ModelToKill                   = { %d, %d, %d, %d }\n"
        "WeaponType                    = %s\n"
        "Status                        = %d\n"
        "bHeadShotRequired             = %d\n"
        "bStandardSoundAndMessages     = %d\n"
        "bProperKillFrenzy             = %d",
        CDarkel::pStartMessage ? GxtCharToUTF8(CDarkel::pStartMessage) : "<nullptr>", CDarkel::AmmoInterruptedWeapon, WEAPON_TYPE_STRING[CDarkel::InterruptedWeaponType],
        WEAPON_TYPE_STRING[CDarkel::InterruptedWeaponTypeSelected], CDarkel::TimeOfFrenzyStart, CDarkel::PreviousTime, CDarkel::TimeLimit, CDarkel::KillsNeeded,
        CDarkel::ModelToKill[0], CDarkel::ModelToKill[1], CDarkel::ModelToKill[2], CDarkel::ModelToKill[3], WEAPON_TYPE_STRING[CDarkel::WeaponType], CDarkel::Status,
        CDarkel::bHeadShotRequired, CDarkel::bStandardSoundAndMessages, CDarkel::bProperKillFrenzy
    );
    Separator();

    SetNextItemWidth(80.f);
    if (InputInt("ID", &m_WeaponType)) {
        m_WeaponType = std::clamp(m_WeaponType, 0, (int32)(std::size(WEAPON_TYPE_STRING) - 1));
    }
    SameLine();
    if (BeginCombo("Weapon Type", WEAPON_TYPE_STRING[m_WeaponType])) {
        for (auto&& [typeId, wepName] : notsa::enumerate(WEAPON_TYPE_STRING)) {
            const notsa::ui::ScopedID id{ typeId };
            if (Selectable(wepName, m_WeaponType == typeId)) {
                m_WeaponType = typeId;
            }
            if (m_WeaponType == typeId) {
                SetItemDefaultFocus();
            }
        }
        EndCombo();
    }
    
    InputInt("Time Limit [Seconds]", &m_TimeLimit, 0);
    InputInt("Kills Needed", &m_NumKillsNeeded, 0);

    SetNextItemWidth(120.0f);
    InputInt4("Models To Kill (Use -1/-2/-3 for any ped/vehicle/bike)", m_ModelsToKill, 0);

    Checkbox("Standard Sound And Msg", &m_StandardSoundAndMsg);
    Checkbox("Headshot Required", &m_IsHeadshotRequired);

    if (Button("Start Frenzy")) {
        // Make sure model is loaded (otherwise bad things happen)
        CWeaponInfo::GetWeaponInfo((eWeaponType)(m_WeaponType))->StreamModelsForWeapon(STREAMING_GAME_REQUIRED);

        // Now start it
        CDarkel::StartFrenzy(
            (eWeaponType)m_WeaponType,
            m_TimeLimit * 1000,
            m_NumKillsNeeded,
            m_ModelsToKill[0],
            TheText.Get("PAGE_00"),
            m_ModelsToKill[1],
            m_ModelsToKill[2],
            m_ModelsToKill[3],
            m_StandardSoundAndMsg,
            m_IsHeadshotRequired
        );
    }

    EndGroup();
}

void DarkelDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Darkel", nullptr, &m_IsOpen);
    });
}
