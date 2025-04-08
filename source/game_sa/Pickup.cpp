#include "StdInc.h"

#include "Pickup.h"
#include "ModelIndices.h"

using namespace ModelIndices;

void CPickup::InjectHooks() {
    RH_ScopedClass(CPickup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ExtractAmmoFromPickup, 0x454BE0);
    RH_ScopedInstall(FindStringForTextIndex, 0x455540);
    RH_ScopedInstall(FindTextIndexForString, 0x455500);
    RH_ScopedInstall(GetPosn, 0x4549A0);
    RH_ScopedInstall(GetRidOfObjects, 0x454CF0);
    RH_ScopedInstall(GiveUsAPickUpObject, 0x4567E0, {.reversed = false}); // <-- broken for now
    RH_ScopedInstall(IsVisible, 0x454C70); // <-- something broken about this one
    RH_ScopedInstall(PickUpShouldBeInvisible, 0x454D20);
    RH_ScopedInstall(ProcessGunShot, 0x4588B0);
    RH_ScopedInstall(Remove, 0x4556C0);
    RH_ScopedInstall(SetPosn, 0x454960);
    RH_ScopedInstall(Update, 0x457410);
}

// Give player an ammo from weapon pickup
// 0x454BE0
void CPickup::ExtractAmmoFromPickup(CPlayerPed* player) {
    const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex);

    if (m_nPickupType == PICKUP_IN_SHOP)
        return;

    switch (CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot) {
    case PICKUP_ONCE:
    case PICKUP_ONCE_TIMEOUT:
    case PICKUP_ONCE_TIMEOUT_SLOW:
        break;
    default:
        return;
    }

    const auto EmptyAmmo = [this] {
        m_nFlags.bEmpty = true;
        m_nAmmo = 0;
    };

    if (m_nAmmo) {
        player->GrantAmmo(weapon, m_nAmmo);
    } else {
        if (m_nFlags.bEmpty) {
            return EmptyAmmo();
        }

        player->GrantAmmo(weapon, AmmoForWeapon_OnStreet[weapon]);
    }

    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
    EmptyAmmo();
}

// 0x455540
const char* CPickup::FindStringForTextIndex(ePickupPropertyText index) {
    switch (index) {
    case PICKUP_PROPERTY_TEXT_CAN_BUY:
        return "PROP_3"; // Press ~k~~PED_ANSWER_PHONE~ to buy this property.

    case PICKUP_PROPERTY_TEXT_CANT_BUY:
        return "PROP_4"; // You cannot buy this property yet.

    case PICKUP_PROPERTY_TEXT_CANCEL:
    default:
        return "FESZ_CA"; // Cancel
    }
}

// 0x455500
ePickupPropertyText CPickup::FindTextIndexForString(const char* message) {
    if (!message)
        return PICKUP_PROPERTY_TEXT_CANCEL;

    if (!_stricmp("PROP_3", message)) {
        return PICKUP_PROPERTY_TEXT_CAN_BUY;
    } else if (!_stricmp("PROP_4", message)) {
        return PICKUP_PROPERTY_TEXT_CANT_BUY;
    }

    return PICKUP_PROPERTY_TEXT_CANCEL;
}

// Delete pickup's object (CObject)
// 0x454CF0
void CPickup::GetRidOfObjects() {
    if (m_pObject) {
        CWorld::Remove(m_pObject);
        delete m_pObject;
        m_pObject = nullptr;
    }
}

// Creates an object (CObject) for pickup. slotIndex - object to replace; use -1 (or any negative value) to create a new object
// 0x4567E0
void CPickup::GiveUsAPickUpObject(CObject** obj, int32 slotIndex) {
    const auto mi = CModelInfo::GetModelInfo(m_nModelIndex);
    auto& object = *obj;
    object = nullptr;

    if (CCutsceneMgr::HasLoaded()) {
        return;
    }

    if (mi->GetModelType() == MODEL_INFO_WEAPON) {
        CWeaponInfo::GetWeaponInfo(mi->AsWeaponModelInfoPtr()->m_weaponInfo);
    }

    if (slotIndex < 0) {
        object = CObject::Create(m_nModelIndex, false);
        return;
    }

    CPools::MakeSureSlotInObjectPoolIsEmpty(slotIndex);
    object = new (slotIndex << 8) CObject(m_nModelIndex, false);
    if (!object) {
        NOTSA_LOG_DEBUG("NO OBJECT ALLOCATED WHAT THE FUCK");
        return;
    }

    object->m_nObjectType = OBJECT_TYPE_DECORATION;
    object->GetPosition() = GetPosn();

    if (object->m_nModelIndex == MI_OYSTER) {
        object->m_bUnderwater = true;
    }

    object->SetOrientation(0.0f, 0.0f, -HALF_PI);
    object->SetHeading(-HALF_PI);
    object->UpdateRwFrame();
    object->physicalFlags.bApplyGravity = false;
    object->physicalFlags.bExplosionProof = true;
    object->m_bUsesCollision = false;
    object->objectFlags.bIsPickup = true;
    object->objectFlags.b0x02 = object->m_bCollisionProcessed;
    object->objectFlags.bDoNotRender = PickUpShouldBeInvisible();
    object->m_bHasPreRenderEffects = true;
    object->m_bTunnelTransition = true;
    object->RegisterReference((CEntity**)obj);

    if (m_nModelIndex == MI_PICKUP_BONUS || m_nModelIndex == MI_PICKUP_CLOTHES) {
        object->m_nBonusValue = m_nAmmo;
    } else {
        object->m_nBonusValue = 0;
    }

    switch (m_nPickupType) {
    case PICKUP_IN_SHOP_OUT_OF_STOCK:
        object->objectFlags.bPickupInShopOutOfStock = true;
        object->physicalFlags.bRenderScorched = true; // ?
        break;

    case PICKUP_PROPERTY_FORSALE:
        object->objectFlags.bPickupPropertyForSale = true;
        object->m_wCostValue = m_nAmmo / 5u;
        break;
    }
}

// Is pickup visible (checks if distance between pickup and camera is shorter than 100 units)
// 0x454C70
bool CPickup::IsVisible() {
    return DistanceBetweenPoints2D(GetPosn2D(), TheCamera.GetPosition()) < 100.0f;
}

// 0x454D20
bool CPickup::PickUpShouldBeInvisible() {
    const auto IsKillFrenzy = [this] {
        return CLocalisation::KillFrenzy() && m_nModelIndex == MI_PICKUP_KILLFRENZY && CTheScripts::IsPlayerOnAMission() && CDarkel::FrenzyOnGoing();
    };

    if (CCutsceneMgr::IsRunning() && !IsKillFrenzy())
        return true;

    CVector2D lspdPark{+1479.0f, -1658.0f}; // Pershing Square Park
    if (m_nModelIndex == MI_PICKUP_2P_KILLFRENZY && CLocalisation::GermanGame() && DistanceBetweenPoints2D(GetPosn2D(), lspdPark) < 10.0f)
        return true;

    if (TheCamera.m_bWideScreenOn && m_nPickupType != PICKUP_ASSET_REVENUE && m_nModelIndex != MODEL_FIRE_EX)
        return true;

    return CGameLogic::IsCoopGameGoingOn() && CWeapon::CanBeUsedFor2Player(CPickups::WeaponForModel(m_nModelIndex));
}

// Checks if pickup collides with line (origin;target), removes pickup and creates an explosion. Used in previous GTA games for mine pickup
// 0x4588B0
void CPickup::ProcessGunShot(const CVector& start, const CVector& end) {
    if (!m_pObject)
        return;

    if (const auto& posn = m_pObject->GetPosition(); CCollision::TestLineSphere({start, end}, {posn, 4.0f})) {
        CExplosion::AddExplosion(nullptr, nullptr, EXPLOSION_MINE, posn, 0, true, -1.0f, false);
        Remove();
    }
}

// 0x4556C0
void CPickup::Remove() {
    auto pickupRef = tPickupReference(*this);
    CRadar::ClearBlipForEntity(BLIP_PICKUP, pickupRef.num);
    GetRidOfObjects();
    m_nPickupType = PICKUP_NONE;
    m_nFlags.bDisabled = true;
}

// Updates the pickup. Returns TRUE if pickup was removed/disabled
// 0x457410
bool CPickup::Update(CPlayerPed* player, CVehicle* vehicle, int32 playerId) {
    auto& pt = m_nPickupType;

    if (m_pObject) {
        m_pObject->SetPosn(GetPosn());
    }

    if (pt == PICKUP_ASSET_REVENUE) {
        const auto elapsed = CTimer::GetTimeInMS() - m_nRegenerationTime;
        m_nRegenerationTime = CTimer::GetTimeInMS();

        if (DistanceBetweenPoints(GetPosn(), FindPlayerCoors()) > 10.0f) {
            m_fRevenueValue += ((float)elapsed * m_nMoneyPerDay) / 1'440'000.0f; // 1,440,000 = 1 game day (24mins) in ms
        }
        m_fRevenueValue = std::min(m_fRevenueValue, (float)m_nAmmo);

        if (m_pObject) {
            m_pObject->m_wCostValue = (uint16)m_fRevenueValue >= 10 ? (uint16)m_fRevenueValue / 5 : 0;
        }
    }

    if (m_nFlags.bDisabled) {
        if (CTimer::GetTimeInMS() > m_nRegenerationTime) {
            const auto dist = DistanceBetweenPoints(GetPosn(), FindPlayerCoors());
            if (dist > 10.0f || (pt == PICKUP_IN_SHOP && dist > 2.4f)) {
                GiveUsAPickUpObject(&m_pObject, -1);

                if (m_pObject) {
                    CWorld::Add(m_pObject);
                    m_nFlags.bDisabled = false;
                }
            }
        }

        return false;
    }

    bool isRemoved = false;
    const auto SetRemoved = [&] {
        isRemoved = true;
        Remove();
    };

    if (!m_pObject) {
        GiveUsAPickUpObject(&m_pObject, -1);
        if (m_pObject) {
            CWorld::Add(m_pObject);
        }
    }

    if (m_pObject) {
        const auto& mi = m_pObject->m_nModelIndex;

        const auto ObjectWaterLevelCheck = [this](float extra = 0.0f) {
            float level;
            if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition(), level, true)) {
                m_pObject->GetPosition().z = level + extra;
            }
            m_pObject->UpdateRW();
            m_pObject->UpdateRwFrame();
        };

        switch (pt) {
        case PICKUP_MINE_INACTIVE:
            if (vehicle && !vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) {
                pt = PICKUP_MINE_ARMED;
                m_nRegenerationTime = CTimer::GetTimeInMS() + 10'000;
            }
            break;

        case PICKUP_NAUTICAL_MINE_INACTIVE: {
            ObjectWaterLevelCheck(0.6f);

            bool isAnyVehicleTouching = false;
            for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
                if (vehicle.IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) {
                    isAnyVehicleTouching = true;
                    // break?
                }
            }

            if (!isAnyVehicleTouching) {
                pt = PICKUP_NAUTICAL_MINE_ARMED;
                m_nRegenerationTime = CTimer::GetTimeInMS() + 10'000;
            }

            break;
        }
        case PICKUP_NAUTICAL_MINE_ARMED:
            ObjectWaterLevelCheck(0.6f);
            [[fallthrough]];

        case PICKUP_MINE_ARMED: {
            bool explode = CTimer::GetTimeInMS() > m_nRegenerationTime;
            for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
                if (vehicle.IsSphereTouchingVehicle(m_pObject->GetPosition(), 1.5f)) {
                    explode = true;
                    // break?
                }
            }

            if (explode) {
                CExplosion::AddExplosion(nullptr, nullptr, EXPLOSION_MINE, m_pObject->GetPosition(), 0u, 1u, -1.0f, false);
                Remove();
            }
            break;
        }
        case PICKUP_FLOATINGPACKAGE: {
            m_pObject->GetMoveSpeed().z -= CTimer::GetTimeStep() / 1'000.0f;
            m_pObject->GetPosition() += m_pObject->GetMoveSpeed() * CTimer::GetTimeStep();
            m_pObject->UpdateRW();
            m_pObject->UpdateRwFrame();

            float level;
            if (CWaterLevel::GetWaterLevel(m_pObject->GetPosition(), level, true) && level >= m_pObject->GetPosition().z) {
                pt = PICKUP_FLOATINGPACKAGE_FLOATING;
            }
            break;
        }
        case PICKUP_FLOATINGPACKAGE_FLOATING:
            ObjectWaterLevelCheck();

            if (vehicle && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) {
                SetRemoved();
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_DRUGS);
            }
            break;
        default: {
            bool isPicked = false;
            const auto CheckObjectAndProcess = [&] {
                if (m_pObject->objectFlags.bDoNotRender) {
                    isPicked = false;
                }

                if ((!player->GetPadFromPlayer() || !player->GetPadFromPlayer()->ArePlayerControlsDisabled() || m_pObject->m_nModelIndex != MODEL_JETPACK) && isPicked) {
                    isPicked = [&] {
                        switch (pt) {
                        case PICKUP_COLLECTABLE1:
                        case PICKUP_MONEY:
                        case PICKUP_MINE_INACTIVE:
                        case PICKUP_MINE_ARMED:
                        case PICKUP_NAUTICAL_MINE_INACTIVE:
                        case PICKUP_FLOATINGPACKAGE:
                        case PICKUP_FLOATINGPACKAGE_FLOATING:
                        case PICKUP_ASSET_REVENUE:
                        case PICKUP_PROPERTY_LOCKED:
                        case PICKUP_PROPERTY_FORSALE:
                        case PICKUP_MONEY_DOESNTDISAPPEAR:
                            return isPicked;
                        default:
                            break;
                        }

                        if (const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex); weapon <= WEAPON_PARACHUTE) {
                            if (CDarkel::FrenzyOnGoing()) {
                                m_nFlags.bHelpMessageDisplayed = false;

                                return false;
                            }

                            if (weapon == WEAPON_UNARMED)
                                return isPicked;

                            const auto& weaponInfo = CWeaponInfo::GetWeaponInfo(weapon);
                            auto& pedWeapon = player->GetWeaponInSlot(weaponInfo->m_nSlot);

                            if (pedWeapon.m_Type != weapon) {
                                if (!CStreaming::GetInfo(m_pObject->m_nModelIndex).IsLoaded()) {
                                    return false;
                                }

                                const auto IsSlotExchangeable = [](int32 slot) {
                                    switch (slot) {
                                    case 3:
                                    case 4:
                                    case 5:
                                        return true;
                                    default:
                                        return false;
                                    }
                                };

                                if (pedWeapon.m_Type != WEAPON_UNARMED && (pedWeapon.m_TotalAmmo != 0 || IsSlotExchangeable(weaponInfo->m_nSlot))) {
                                    CPickups::PlayerOnWeaponPickup = 6;

                                    if (IsSlotExchangeable(weaponInfo->m_nSlot)) {
                                        switch (pt) {
                                        case PICKUP_ONCE_TIMEOUT:
                                        case PICKUP_ONCE:
                                        case PICKUP_ON_STREET:
                                        case PICKUP_ONCE_FOR_MISSION:
                                            ExtractAmmoFromPickup(player);
                                            pedWeapon.Reload();
                                        }
                                    }

                                    if (!CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(weapon)) {
                                        return false;
                                    }

                                    const auto pad = CPad::GetPad();
                                    if (!pad->GetTarget() && CPickups::DisplayHelpMessage && !m_nFlags.bHelpMessageDisplayed) {
                                        switch (pad->Mode) {
                                        case 0:
                                        case 1:
                                            CHud::SetHelpMessage(TheText.Get("PU_CF1"), false, false, false);
                                            break;
                                        case 2:
                                            CHud::SetHelpMessage(TheText.Get("PU_CF3"), false, false, false);
                                            break;
                                        case 3:
                                            CHud::SetHelpMessage(TheText.Get("PU_CF4"), false, false, false);
                                            break;
                                        default:
                                            break;
                                        }

                                        if (CPickups::DisplayHelpMessage) {
                                            CPickups::DisplayHelpMessage--;
                                        }
                                        m_nFlags.bHelpMessageDisplayed = true;
                                    }

                                    if (!CollectPickupBuffer || CTimer::GetTimeInMS() - player->m_pPlayerData->m_nLastHSMissileLOSTime < 1500) {
                                        return false;
                                    }
                                } else if (CTimer::GetTimeInMS() - player->m_pPlayerData->m_nLastHSMissileLOSTime < 1500 ||
                                           !CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(weapon)) {
                                    CPickups::PlayerOnWeaponPickup = 6;
                                    return false;
                                }
                            }
                        }

                        return isPicked;
                    }();

                    if (isPicked) {
                        const auto CanPlayerPick = [&] {
                            const auto& playerInfo = FindPlayerInfo(playerId);
                            const auto& playerWantedLevel = FindPlayerWanted(playerId)->GetWantedLevel();

                            if (mi == MI_PICKUP_BODYARMOUR && (float)playerInfo.m_nMaxArmour - 0.2f < player->m_fArmour)
                                return false;

                            if (mi == MI_PICKUP_HEALTH && (float)playerInfo.m_nMaxHealth - 0.2f < player->m_fHealth)
                                return false;

                            if (mi == MI_PICKUP_BRIBE && playerWantedLevel == 0)
                                return false;

                            if (mi == MI_PICKUP_KILLFRENZY && (CTheScripts::IsPlayerOnAMission() || CDarkel::FrenzyOnGoing() || !CLocalisation::KillFrenzy()))
                                return false;

                            // todo: check again, i found || instead of && when solving de morgan but it's bullshit, that should be
                            //       the correct expression.
                            if (m_pObject->m_nModelIndex == MODEL_JETPACK && player->GetIntelligence()->GetTaskJetPack())
                                return false;

                            if ((pt == PICKUP_ASSET_REVENUE && m_fRevenueValue < 10.0f) || (pt == PICKUP_2P && CTheScripts::IsPlayerOnAMission()))
                                return false;

                            return true;
                        };
                        if (CanPlayerPick()) {
                            if (mi != MI_PICKUP_PROPERTY && mi != MI_PICKUP_PROPERTY_FORSALE && pt != PICKUP_2P) {
                                CPad::GetPad()->StartShake(120, 100u, 0);
                            }

                            const auto weapon = CPickups::WeaponForModel(m_pObject->m_nModelIndex);
                            switch (pt) {
                            case PICKUP_ON_STREET:
                            case PICKUP_ON_STREET_SLOW: {
                                const auto SetPickupRegenTimeAndDisable = [&](uint32 regenTime) {
                                    switch (pt) {
                                    case PICKUP_ON_STREET:
                                        m_nRegenerationTime = regenTime + 30'000;
                                        break;
                                    case PICKUP_ON_STREET_SLOW:
                                        m_nRegenerationTime = regenTime + ((mi == MI_PICKUP_BRIBE) ? 30'000 : 36'000);
                                        break;
                                    default:
                                        break;
                                    }

                                    isRemoved = true;
                                    GetRidOfObjects();
                                    m_nFlags.bDisabled = true;
                                };

                                if (!CPickups::GivePlayerGoodiesWithPickUpMI(mi, playerId)) {
                                    if (!player->DoesPlayerWantNewWeapon(weapon, false)) {
                                        break;
                                    }

                                    if (weapon != WEAPON_UNARMED) {
                                        if (m_nAmmo) {
                                            player->GiveDelayedWeapon(weapon, m_nAmmo);
                                        } else {
                                            player->GiveDelayedWeapon(weapon, (!m_nFlags.bEmpty) ? AmmoForWeapon_OnStreet[weapon] : 0);
                                        }

                                        if (auto& chosen = player->m_pPlayerData->m_nChosenWeapon; chosen == player->GetWeaponSlot(WEAPON_UNARMED)) {
                                            chosen = player->GetWeaponSlot(weapon);
                                        }
                                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
                                    } else if (mi == MI_PICKUP_CAMERA && vehicle) {
                                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
                                        CPickups::bPickUpcamActivated = true;
                                        CPickups::pPlayerVehicle = FindPlayerVehicle();
                                        CPickups::StaticCamCoors = &m_pObject->GetPosition();
                                        CPickups::StaticCamStartTime = CTimer::GetTimeInMS();

                                        SetPickupRegenTimeAndDisable(CPickups::StaticCamStartTime);
                                        break;
                                    }
                                }
                                SetPickupRegenTimeAndDisable(CTimer::GetTimeInMS());
                                break;
                            }
                            case PICKUP_ONCE:
                            case PICKUP_ONCE_TIMEOUT:
                            case PICKUP_ONCE_TIMEOUT_SLOW:
                            case PICKUP_ONCE_FOR_MISSION:
                                if (!CPickups::GivePlayerGoodiesWithPickUpMI(mi, playerId)) {
                                    if (!player->DoesPlayerWantNewWeapon(weapon, false)) {
                                        ExtractAmmoFromPickup(player);
                                        break;
                                    }

                                    if (weapon != WEAPON_UNARMED) {
                                        if (m_nAmmo) {
                                            player->GiveDelayedWeapon(weapon, m_nAmmo);
                                        } else {
                                            player->GiveDelayedWeapon(weapon, (m_nFlags.bEmpty) ? 0 : AmmoForWeapon_OnStreet[weapon]);
                                        }

                                        if (auto& chosen = player->m_pPlayerData->m_nChosenWeapon; chosen == player->GetWeaponSlot(WEAPON_UNARMED)) {
                                            chosen = player->GetWeaponSlot(weapon);
                                        }
                                    }

                                    if (mi != MI_PICKUP_SAVEGAME) {
                                        CHud::SetHelpMessage(nullptr, true, false, false);
                                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON);
                                    }
                                }
                                SetRemoved();
                                break;
                            case PICKUP_MONEY:
                            case PICKUP_MONEY_DOESNTDISAPPEAR:
                                FindPlayerInfo().m_nMoney += m_nAmmo; // originally player 0
                                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
                                player->Say(CTX_GLOBAL_PICKUP_CASH);
                                SetRemoved();
                                break;
                            case PICKUP_ASSET_REVENUE:
                                FindPlayerInfo(playerId).m_nMoney += (int32)m_fRevenueValue;
                                m_fRevenueValue = 0.0f;
                                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
                                break;
                            case PICKUP_PROPERTY_LOCKED:
                                if (!m_nFlags.bHelpMessageDisplayed) {
                                    m_nFlags.bHelpMessageDisplayed = true;
                                    auto text = TheText.Get(FindStringForTextIndex((ePickupPropertyText)m_nFlags.nPropertyTextIndex));
                                    CHud::SetHelpMessage(text, false, false, false);
                                }
                                break;
                            case PICKUP_PROPERTY_FORSALE:
                                char labelKey[9];
                                strncpy_s(labelKey, FindStringForTextIndex((ePickupPropertyText)m_nFlags.nPropertyTextIndex), 9);
                                ModifyStringLabelForControlSetting(labelKey);
                                CMessages::InsertNumberInString(TheText.Get(labelKey), m_nAmmo, 0, 0, 0, 0, 0, gGxtString);

                                if (!CHud::HelpMessageDisplayed()) {
                                    CHud::SetHelpMessage(gGxtString, false, false, false);
                                }
                                if (CollectPickupBuffer) {
                                    const GxtChar* textToPrint = nullptr;
                                    if (CTheScripts::IsPlayerOnAMission()) {
                                        textToPrint = TheText.Get("PROP_2");
                                    } else if (FindPlayerInfo().m_nMoney < (int32)m_nAmmo) {
                                        textToPrint = TheText.Get("PROP_1");
                                    }

                                    CHud::SetHelpMessage(textToPrint, true, false, false);
                                }
                                break;
                            case PICKUP_2P:
                                if (const auto pad2 = CPad::GetPad(1); pad2->GetTouchedTimeDelta() >= 500) {
                                    if (!m_nFlags.bHelpMessageDisplayed) {
                                        CHud::SetHelpMessage(TheText.Get("PU_2PLN"), true, false, true);
                                    }
                                } else {
                                    CHud::m_Wants_To_Draw_Hud = true;
                                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
                                    SetRemoved();
                                    CHud::SetHelpMessage(nullptr, true, false, false);
                                    CGameLogic::vec2PlayerStartLocation = FindPlayerCoors();
                                    CGameLogic::f2PlayerStartHeading = FindPlayerPed()->GetHeading();
                                }
                                break;
                            }
                        }
                    }
                }
            };

            if (mi == MI_PICKUP_CAMERA) {
                isPicked = false;
            } else {
                const auto distXY = DistanceBetweenPoints2D(player->GetPosition2D(), m_pObject->GetPosition2D());
                const auto diffZ = std::abs(player->GetPosition().z - m_pObject->GetPosition().z);

                if (mi == MI_PICKUP_BRIBE) {
                    // bribes can be picked in vehicle.
                    if ((vehicle && vehicle->IsSphereTouchingVehicle(m_pObject->GetPosition(), 2.0f)) || (diffZ < 2.0f && distXY < sqrt(1.8f))) {
                        isPicked = true;
                    }
                } else {
                    // these can not be picked on missions
                    bool nonMissionPickup = mi == MI_PICKUP_SAVEGAME || mi == MI_PICKUP_2P_KILLFRENZY || mi == MI_PICKUP_2P_COOP;

                    if ((!nonMissionPickup || player->CanPlayerStartMission()) && (!vehicle && player->IsAlive())) {
                        if (diffZ < 2.0f && distXY < sqrt(1.8f)) {
                            isPicked = true;
                        }
                    }
                }
            }
            CheckObjectAndProcess();
        }
        }

        if (!m_nFlags.bDisabled) {
            switch (pt) {
            case PICKUP_ONCE_TIMEOUT:
            case PICKUP_ONCE_TIMEOUT_SLOW:
            case PICKUP_MONEY:
                if (CTimer::GetTimeInMS() > m_nRegenerationTime) {
                    SetRemoved();
                }
                break;
            }
        }
    }
    return isRemoved;
}
