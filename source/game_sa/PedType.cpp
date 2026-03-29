#include "StdInc.h"

#include "PedType.h"
CAcquaintance* CPedType::ms_apPedTypesOld = {}; // NOTSA

void CPedType::InjectHooks() {
    RH_ScopedClass(CPedType);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x608E40);
    RH_ScopedInstall(Shutdown, 0x608B00);
    RH_ScopedInstall(Save, 0x5D3CD0);
    RH_ScopedInstall(Load, 0x5D3D10);
    RH_ScopedInstall(LoadPedData, 0x608B30);
    RH_ScopedInstall(FindPedType, 0x608790);
    RH_ScopedInstall(GetPedFlag, 0x608830);
    RH_ScopedOverloadedInstall(GetPedTypeAcquaintances, "CAcquaintance", 0x6089B0, CAcquaintance&(*)(ePedType));
    RH_ScopedOverloadedInstall(GetPedTypeAcquaintances, "uint32", 0x6089D0, uint32(*)(AcquaintanceId, ePedType));
    RH_ScopedInstall(SetPedTypeAsAcquaintance, 0x608E20);
    RH_ScopedInstall(ClearPedTypeAcquaintances, 0x608A20);
    RH_ScopedInstall(ClearPedTypeAsAcquaintance, 0x6089F0);
    RH_ScopedInstall(PoliceDontCareAboutCrimesAgainstPedType, 0x608A40);
}

// 0x608E40
void CPedType::Initialise() {
    ms_apPedTypes = new CAcquaintance[PED_TYPE_COUNT]();
    ms_apPedTypesOld = new CAcquaintance[PED_TYPE_COUNT](); // NOTSA
    LoadPedData();
}

// 0x608B00
void CPedType::Shutdown() {
    delete[] ms_apPedTypes;
    delete[] ms_apPedTypesOld; // NOTSA
}

// 0x608B30
void CPedType::LoadPedData() {
    ePedType pedType = PED_TYPE_MISSION8;
    auto file = CFileMgr::OpenFile("DATA\\PED.DAT", "r");
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        char buf[32];
        VERIFY(sscanf_s(line, "%s", SCANF_S_STR(buf)) == 1);

        char* nextToken{};
        const auto GetAcquaintance = [=, &nextToken]() -> uint32 {
            uint32 value = 0;
            RET_IGNORED(strtok_s(line, " ,\t", &nextToken));
            for (const char* pedTypeName = strtok_s(nullptr, " ,\t", &nextToken); pedTypeName; pedTypeName = strtok_s(nullptr, " ,\t", &nextToken)) {
                value |= GetPedFlag(FindPedType(pedTypeName));
            }
            return value;
        };

        using namespace std::string_view_literals;
        if (std::string_view{buf, 4u} == "Hate")
        {
            GetPedTypeAcquaintances(pedType).m_nHate = GetAcquaintance();
        }
        else if (std::string_view{buf, 7u} == "Dislike")
        {
            GetPedTypeAcquaintances(pedType).m_nDislike = GetAcquaintance();
        }
        else if (std::string_view{buf, 4u} == "Like")
        {
            GetPedTypeAcquaintances(pedType).m_nLike = GetAcquaintance();
        }
        else if (std::string_view{buf, 7u} == "Respect")
        {
            GetPedTypeAcquaintances(pedType).m_nRespect = GetAcquaintance();
        }
        else
        {
            pedType = FindPedType(buf);
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5D3D10
void CPedType::Load() {
    for (uint32 i = 0; i < PED_TYPE_COUNT; ++i ) {
        for (auto id = 0; id < ACQUAINTANCE_NUM; ++id) {
            auto value = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
            GetPedTypeAcquaintances(static_cast<ePedType>(i)).SetAcquaintances(id, value);
        }
    }
}

// 0x5D3CD0
void CPedType::Save() {
    for (uint32 i = 0; i < PED_TYPE_COUNT; ++i ) {
        for (auto id = 0; id < ACQUAINTANCE_NUM; ++id) {
            uint32 value = GetPedTypeAcquaintances(static_cast<ePedType>(i)).GetAcquaintances(id);
            CGenericGameStorage::SaveDataToWorkBuffer(value);
        }
    }
}

// 0x608790
ePedType CPedType::FindPedType(const char* pedTypeName) {
    static auto s_PedTypeByNameMapping = notsa::make_mapping<ePedType, std::string_view>({
        { PED_TYPE_PLAYER1,        "PLAYER1"        },
        { PED_TYPE_PLAYER2,        "PLAYER2"        },
        { PED_TYPE_PLAYER_NETWORK, "PLAYER_NETWORK" },
        { PED_TYPE_PLAYER_UNUSED,  "PLAYER_UNUSED"  },

        { PED_TYPE_CIVMALE,        "CIVMALE"        },
        { PED_TYPE_CIVFEMALE,      "CIVFEMALE"      },

        { PED_TYPE_COP,            "COP"            },

        { PED_TYPE_GANG1,          "GANG1"          },
        { PED_TYPE_GANG2,          "GANG2"          },
        { PED_TYPE_GANG3,          "GANG3"          },
        { PED_TYPE_GANG4,          "GANG4"          },
        { PED_TYPE_GANG5,          "GANG5"          },
        { PED_TYPE_GANG6,          "GANG6"          },
        { PED_TYPE_GANG7,          "GANG7"          },
        { PED_TYPE_GANG8,          "GANG8"          },
        { PED_TYPE_GANG9,          "GANG9"          },
        { PED_TYPE_GANG10,         "GANG10"         },

        { PED_TYPE_DEALER,         "DEALER"         },
        { PED_TYPE_MEDIC,          "MEDIC"          },
        { PED_TYPE_FIREMAN,        "FIREMAN"        },
        { PED_TYPE_CRIMINAL,       "CRIMINAL"       },
        { PED_TYPE_BUM,            "BUM"            },
        { PED_TYPE_PROSTITUTE,     "PROSTITUTE"     },
        { PED_TYPE_SPECIAL,        "SPECIAL"        },

        { PED_TYPE_MISSION1,       "MISSION1"       },
        { PED_TYPE_MISSION2,       "MISSION2"       },
        { PED_TYPE_MISSION3,       "MISSION3"       },
        { PED_TYPE_MISSION4,       "MISSION4"       },
        { PED_TYPE_MISSION5,       "MISSION5"       },
        { PED_TYPE_MISSION6,       "MISSION6"       },
        { PED_TYPE_MISSION7,       "MISSION7"       },
        { PED_TYPE_MISSION8,       "MISSION8"       },
    });
    /* NOTE(pirulax): Original code returned `PED_TYPE_COUNT` as an `invalid value` sentinel, but we don't want to have quiet errors */
    return notsa::find_value(s_PedTypeByNameMapping, pedTypeName); 
    /* NOTE(pirulax): There are 2 extra `strcmp`'s after this, but they both check values already present in the mapping above, so they're ok to omit */
}

// 0x608830
// TODO: Stuff related to this should use `size_t`'s
uint32 CPedType::GetPedFlag(ePedType pedType) {
    if ((size_t)pedType < (sizeof(uint32) * 8)) { // Make sure we aren't shifting more than 31 bits, otherwise it's U.B.
        return 1 << (size_t)pedType;
    } else {
        return 0;
    }
}

// 0x6089B0
CAcquaintance& CPedType::GetPedTypeAcquaintances(ePedType pedType) {
    return ms_apPedTypes[static_cast<int32>(pedType)];
}

// 0x6089D0
uint32 CPedType::GetPedTypeAcquaintances(AcquaintanceId id, ePedType pedType) {
    return GetPedTypeAcquaintances(pedType).GetAcquaintances(id);
}

// 0x608E20
void CPedType::SetPedTypeAsAcquaintance(AcquaintanceId id, ePedType pedType, int32 pedTypeBitNum) {
    GetPedTypeAcquaintances(pedType).SetAsAcquaintance(id, pedTypeBitNum);
}

// 0x608A20
void CPedType::ClearPedTypeAcquaintances(AcquaintanceId id, ePedType pedType) {
    GetPedTypeAcquaintances(pedType).ClearAcquaintances(id);
}

// 0x6089F0
void CPedType::ClearPedTypeAsAcquaintance(AcquaintanceId id, ePedType pedType, int32 pedTypeBitNum) {
    GetPedTypeAcquaintances(pedType).ClearAsAcquaintance(id, pedTypeBitNum);
}

// 0x608A40
bool CPedType::PoliceDontCareAboutCrimesAgainstPedType(ePedType pedType) {
    return pedType >= PED_TYPE_GANG1 && pedType <= PED_TYPE_GANG10 || pedType == PED_TYPE_CRIMINAL || pedType == PED_TYPE_DEALER;
}
