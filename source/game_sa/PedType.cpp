#include "StdInc.h"

#include "PedType.h"

CAcquaintance*& CPedType::ms_apPedTypes = *(CAcquaintance**)0xC0BBE8;
CAcquaintance* CPedType::ms_apPedTypesOld = {}; // NOTSA

void CPedType::InjectHooks() {
    RH_ScopedClass(CPedType);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x608E40);
    RH_ScopedInstall(Shutdown, 0x608B00);
    RH_ScopedInstall(Save, 0x5D3CD0, {.enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(Load, 0x5D3D10, {.enabled = LOAD_HOOKS_ENABLED });
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
            uint32 value;
            CGenericGameStorage::LoadDataFromWorkBuffer_Org(&value, sizeof(uint32));
            GetPedTypeAcquaintances(static_cast<ePedType>(i)).SetAcquaintances(id, value);
        }
    }
}

// 0x5D3CD0
void CPedType::Save() {
    for (uint32 i = 0; i < PED_TYPE_COUNT; ++i ) {
        for (auto id = 0; id < ACQUAINTANCE_NUM; ++id) {
            uint32 value = GetPedTypeAcquaintances(static_cast<ePedType>(i)).GetAcquaintances(id);
            CGenericGameStorage::SaveDataToWorkBuffer(&value, 4);
        }
    }
}

// 0x608790
ePedType CPedType::FindPedType(const char* pedTypeName) {
    for (int16 pedType = 0; pedType < PED_TYPE_COUNT; pedType++) {
        if (strcmp(pedTypeName, aPedTypeNames[pedType]) != 0)
            continue;

        return static_cast<ePedType>(pedType);
    }

    if (strcmp(pedTypeName, "PLAYER_NETWORK") == 0) {
        return PED_TYPE_PLAYER_NETWORK;
    } else if (strcmp(pedTypeName, "PLAYER_UNUSED") == 0) {
        return PED_TYPE_PLAYER_UNUSED;
    } else {
        return PED_TYPE_MISSION8;
    }
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
