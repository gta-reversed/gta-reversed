/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "LoadedCarGroup.h"

constexpr auto SENTINEL_VALUE_OF_UNUSED = (int16)(MODEL_INVALID);

void CLoadedCarGroup::InjectHooks() {
    RH_ScopedClass(CLoadedCarGroup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Clear, 0x611B90);
    RH_ScopedInstall(AddMember, 0x611BB0);
    RH_ScopedInstall(RemoveMember, 0x611BD0);
    RH_ScopedInstall(GetMember, 0x611C20);
    RH_ScopedInstall(CountMembers, 0x611C30);
    RH_ScopedInstall(PickRandomCar, 0x611C50);
    RH_ScopedInstall(SortBasedOnUsage, 0x611E10);
    RH_ScopedInstall(PickLeastUsedModel, 0x611E90);
}

// 0x611E10
void CLoadedCarGroup::SortBasedOnUsage() {
    // Sort from higher to lower usage
    rng::sort(GetAllModels(), std::greater<>{}, [](int16 modelid) {
        return CModelInfo::GetVehicleModelInfo(modelid)->m_nTimesUsed; }
    );
}

// 0x611BD0
void CLoadedCarGroup::RemoveMember(eModelID modelIndex) {
    if (notsa::remove_first(m_models, (int16)(modelIndex))) {
        m_models.back() = SENTINEL_VALUE_OF_UNUSED;
    }
}

// 0x611C50
eModelID CLoadedCarGroup::PickRandomCar(bool bNotTooManyInTheWorld, bool bOnlyPickNormalCars) {
    if (Empty()) {
        return MODEL_INVALID;
    }

    const auto PickRandom = [&](auto&& choices) {
        if (rng::empty(choices)) {
            return MODEL_INVALID;
        }

        const auto weightSum = notsa::accumulate(choices, 0, [](int16 model) {
            return CModelInfo::GetVehicleModelInfo(model)->m_nFrq;
        });

        for (auto tr{ 0 }; tr < 10; tr++) { // tr = tries
            // First, pick a model
            const auto pickedModel = [&] {
                auto pickedWeight = CGeneral::GetRandomNumberInRange(0, weightSum);
                for (auto modelId : choices) {
                    const auto thisModelFrq = CModelInfo::GetVehicleModelInfo(modelId)->m_nFrq;
                    if (thisModelFrq >= pickedWeight) {
                        return (eModelID)(modelId);
                    }
                    pickedWeight -= thisModelFrq;
                }
                // No frequency of any model in the array was `>=` than `pickedWeight`
                // Originally in this case the last value from `choices` was used, but most likely unintentionally
                NOTSA_UNREACHABLE();
            }();

            // Check if it's suitable
            if (   !CTheScripts::HasCarModelBeenSuppressed(pickedModel)
                && !CTheScripts::HasVehicleModelBeenBlockedByScript(pickedModel)
                && !CStreaming::WeAreTryingToPhaseVehicleOut(pickedModel)
                && (!bNotTooManyInTheWorld || CModelInfo::GetVehicleModelInfo(pickedModel)->m_nRefCount <= 2)
            ) {
                return pickedModel;
            }
        }

        // 10 tries, but no luck
        return MODEL_INVALID;
    };

    if (bOnlyPickNormalCars) {
        // Originally an array was created here, and that was used
        // That is more performant, but I doubt that this is so performance critical to care about that.
        return PickRandom(
            GetAllModels() | rng::views::filter([](int16 modelId) {
                switch (CModelInfo::GetVehicleModelInfo(modelId)->m_nVehicleClass) {
                case VEHICLE_CLASS_NORMAL:
                case VEHICLE_CLASS_POORFAMILY:
                case VEHICLE_CLASS_RICHFAMILY:
                case VEHICLE_CLASS_MOTORBIKE:
                    return true;
                }
                return false;
            })
        );
    } else {
#ifdef FIX_BUGS
        return PickRandom(GetAllModels());
#else
        return PickRandom(m_models);
#endif
    }
}

// 0x611E90
eModelID CLoadedCarGroup::PickLeastUsedModel(int32 maxTimesUsed) {
    if (Empty()) {
        return MODEL_INVALID;
    }

    const auto GetMI = [](auto model) { return CModelInfo::GetVehicleModelInfo(model); };
    const auto ret = rng::min(GetAllModels(), [&](int16 modelA, int16 modelB) {
        const auto miA = GetMI(modelA), miB = GetMI(modelA);
        if (miA->m_nRefCount < miB->m_nRefCount) { // Primary sort criteria is `m_nRefCount`
            return true;
        }
        if (miA->m_nRefCount == miB->m_nRefCount) { // If that fails, secondary is `m_nTimesUsed`
            return miA->m_nTimesUsed < miB->m_nTimesUsed;
        }
        return false;
    });

    if (GetMI(ret)->m_nTimesUsed <= maxTimesUsed) {
        return (eModelID)(ret);
    }

    return MODEL_INVALID;
}

// 0x611C20
eModelID CLoadedCarGroup::GetMember(uint32 idx) const {
    assert(idx < CountMembers());
    return (eModelID)(m_models[idx]);
}

// 0x611C30
uint32 CLoadedCarGroup::CountMembers() const {
    return (uint32)(rng::distance(m_models.begin(), rng::find(m_models, SENTINEL_VALUE_OF_UNUSED)));
}

// NOTSA
bool CLoadedCarGroup::Empty() const {
    return m_models.front() == SENTINEL_VALUE_OF_UNUSED;
}

// 0x611B90
void CLoadedCarGroup::Clear() {
    rng::fill(m_models, SENTINEL_VALUE_OF_UNUSED);
}

// 0x611BB0
void CLoadedCarGroup::AddMember(eModelID member) {
    const auto end = rng::find(m_models, SENTINEL_VALUE_OF_UNUSED);
    if (end != m_models.end()) {
        *end = (int16)(member);
    } else {
        NOTSA_LOG_DEBUG("Failed to add model to group [Out of memory]");
    }
}
