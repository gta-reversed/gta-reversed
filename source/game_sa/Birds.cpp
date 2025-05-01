#include "StdInc.h"

#include "Birds.h"
#include "Clouds.h"

bool& CBirds::bHasBirdBeenShot = *(bool*)0xC6A8A0;
uint32& CBirds::uiNumberOfBirds = *(uint32*)0xC6A8A4;
CVector& CBirds::vecBirdShotAt = *(CVector*)0xC6AA48;

/*
These spawn in a formation like this:
    4           5
             1
          2
       6           3
*/
float CBirds::faCreationCoorsX[6] = { 0.0f, -1.0f, 2.0f, -3.0f, 1.0f, -2.0f }; // 0x8D5250
float CBirds::faCreationCoorsY[6] = { 0.0f, -1.0f, -2.0f, 1.0f, 1.0f, -2.0f }; // 0x8D5268
float CBirds::faCreationCoorsZ[6] = { 0.0f, 0.5f, 1.0f, 0.7f, 2.0f, 1.2f };    // 0x8D5280

float CBirds::faRenderCoorsU[8] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.5f }; // 0x8D52B8
float CBirds::faRenderCoorsV[8] = { 0.5f, 0.5f, 0.75f, 0.75f, 1.0f, 1.0f, 1.0f, 0.5f }; // 0x8D5298
float CBirds::faRenderPosY[8] = { 0.25f, -0.25f, 0.25f, -0.25f, 0.0f, -0.25f, -0.35f, -0.35f }; // 0x8D52D8;

uint32 CBirds::auRenderIndices[30] = { 0, 3, 1, 0, 2, 3, 2, 4, 5, 2, 5, 3, 0, 1, 3, 0, 3, 2, 2, 5, 4, 2, 3, 5, 0, 6, 7, 0, 7, 6 }; // 0x8D52F8

void CBirds::InjectHooks() {
    RH_ScopedClass(CBirds);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x711EC0);
    RH_ScopedInstall(Shutdown, 0x712300);
    RH_ScopedInstall(HandleGunShot, 0x712E40);
    RH_ScopedInstall(Update, 0x712330);
    RH_ScopedInstall(CreateNumberOfBirds, 0x711EF0);
    RH_ScopedInstall(Render, 0x712810);
}

// 0x711EC0
void CBirds::Init() {
    ZoneScoped;

    for (auto& bird : aBirds) {
        bird.m_bCreated = false;
    }
    uiNumberOfBirds = 0;
    bHasBirdBeenShot = false;
}

inline float GetMaxDistance(eBirdsBiome biome) {
    switch (biome) {
    case eBirdsBiome::BIOME_WATER:  return 45.0F;
    case eBirdsBiome::BIOME_DESERT: return 80.0F;
    case eBirdsBiome::BIOME_NORMAL:
    default:                        return 40.0F;
    }
};

// 0x711EF0
void CBirds::CreateNumberOfBirds(CVector vecStartPos, CVector vecTargetPos, int32 iBirdCount, eBirdsBiome biome, bool bCheckObstacles) {
    if (iBirdCount <= 0)
        return;

    const float fMaxDistance = GetMaxDistance(biome);
    for (auto i = 0; i < iBirdCount; ++i) {
        auto iFreeBirdIndex = 0;
        while (iFreeBirdIndex < (int32)std::size(aBirds)) {
            if (!aBirds[iFreeBirdIndex].m_bCreated)
                break;
            ++iFreeBirdIndex;
        }
        if (iFreeBirdIndex >= (int32)std::size(aBirds))
            return;

        auto& bird = aBirds[iFreeBirdIndex];
        bird.m_vecPosn = vecStartPos;

        auto vecBirdDirection = vecTargetPos - bird.m_vecPosn;
        vecBirdDirection.Normalise();
        auto vecCheckPos = bird.m_vecPosn + (vecBirdDirection * fMaxDistance * 2.4F);
        if (bCheckObstacles && !CWorld::GetIsLineOfSightClear(bird.m_vecPosn, vecCheckPos, true, false, false, false, false, false, false))
            continue;

        assert(i < (int32)std::size(faCreationCoorsX));

        ++uiNumberOfBirds;
        bird.m_bCreated = true;
        bird.m_fMaxBirdDistance = fMaxDistance;
        bird.m_eBirdMode = eBirdMode::BIRD_DRAW_UPDATE;
        bird.m_nUpdateAfterMS = 0;
        bird.m_bMustDoCurves = false;
        bird.m_fAngle = std::atan2(vecBirdDirection.x, vecBirdDirection.y);
        bird.m_vecPosn.x += faCreationCoorsX[i];
        bird.m_vecPosn.y += faCreationCoorsY[i];
        bird.m_vecPosn.z += faCreationCoorsZ[i];

        auto iSpeedRandFactor = CGeneral::GetRandomNumber() % 31;
        float fSpeedMult;
        switch (biome) {
        case eBirdsBiome::BIOME_WATER:
            fSpeedMult = (float)(iSpeedRandFactor) / 50.0f + 4.0f;          // [4.0 : 4.6]
            bird.m_BodyColor.Set(CGeneral::GetRandomNumber() % 64 + 80);    // [80 : 143]
            bird.m_WingsColor.Set(CGeneral::GetRandomNumber() % 64 - 76);   // [166 : 242]
            bird.m_fSize = CGeneral::GetRandomNumberInRange(0.8F, 1.2F);
            bird.m_nWingStillness = 1000 - 12 * iSpeedRandFactor;           // [640 : 1000]
            break;
        case eBirdsBiome::BIOME_DESERT:
            fSpeedMult = (float)(iSpeedRandFactor) / 50.0f + 3.0f; // [3.0 : 3.6]
            bird.m_BodyColor.Set(30, 15, 10);
            bird.m_WingsColor.Set(80, 15, 10);
            bird.m_fSize = CGeneral::GetRandomNumberInRange(2.0F, 2.5F);
            bird.m_nWingStillness = 12 * (125 - iSpeedRandFactor); // [1140 : 1500]
            if (CGeneral::GetRandomNumber() & 4)
                bird.m_nWingStillness = 1'000'000; // Not moving it's wings

            if (CGeneral::GetRandomNumber() & 8)
                bird.m_bMustDoCurves = true;

            break;
        case eBirdsBiome::BIOME_NORMAL:
        default:
            fSpeedMult = (float)(iSpeedRandFactor) / 50.0f + 5.0F; // [5.0 : 5.6]
            bird.m_BodyColor.Set(CGeneral::GetRandomNumberInRange(128, 256));
            bird.m_WingsColor.Set(CGeneral::GetRandomNumberInRange(128, 256));
            bird.m_fSize = CGeneral::GetRandomNumberInRange(0.5F, 0.6F);
            bird.m_nWingStillness = 500 - 6 * iSpeedRandFactor;    // [320 : 500]
            break;
        }

        bird.m_vecTargetVelocity = vecBirdDirection * fSpeedMult;
        bird.m_vecCurrentVelocity = bird.m_vecTargetVelocity;
    }
}

// 0x712300
void CBirds::Shutdown() {
    for (auto& bird : aBirds) {
        if (bird.m_bCreated) {
            bird.m_bCreated = false;
        }
    }
    uiNumberOfBirds = 0;
}

// 0x712330
void CBirds::Update() {
    ZoneScoped;

    const auto& vecCamPos = TheCamera.GetPosition();

    if (!CGame::currArea
        && uiNumberOfBirds < std::size(aBirds)
        && CClock::ClockHoursInRange(5, 22)
        && (CTimer::GetFrameCounter() % 512) == std::size(aBirds)
    ) {
        auto iNumBirdsToCreate = CGeneral::GetRandomNumberInRange(std::size(aBirds) - uiNumberOfBirds) + 1;

        eBirdsBiome biome = eBirdsBiome::BIOME_WATER;

        if (TheCamera.m_fDistanceToWater > 30.0F) {
            if (CWeather::WeatherRegion == eWeatherRegion::WEATHER_REGION_DESERT) {
                biome = eBirdsBiome::BIOME_DESERT;
                iNumBirdsToCreate = std::min(iNumBirdsToCreate, uiNumberOfBirds);
            }
            else {
                biome = eBirdsBiome::BIOME_NORMAL;
            }
        }

        if (iNumBirdsToCreate > 0) {
            CGeneral::GetRandomNumber(); // Called 2 times for some reason
            CGeneral::GetRandomNumber();

            float fSpawnDistance = GetMaxDistance(biome);
            float fFlightHeight = [&biome]() {
                switch (biome) {
                case eBirdsBiome::BIOME_WATER:  return CGeneral::GetRandomNumberInRange(4.0F, 13.0F);
                case eBirdsBiome::BIOME_DESERT: return CGeneral::GetRandomNumberInRange(15.0F,25.0F);
                case eBirdsBiome::BIOME_NORMAL:
                default:                        return CGeneral::GetRandomNumberInRange(2.0F, 10.0F);
                }
            }();

            if (fFlightHeight > 5.0F) {
                float fBirdSpawnZ = fFlightHeight + vecCamPos.z;
                float fSpawnAngleCamRelative;

                if (CGeneral::GetRandomNumber() % 2)
                    fSpawnAngleCamRelative = (float)(uint8)(CGeneral::GetRandomNumber() % 256) * 0.024531251F; // [0 : 255] mapped to [0 : 2π]
                else {
                    auto vecForward = TheCamera.m_mCameraMatrix.GetForward();
                    vecForward.z = 0.0F;
                    if (vecForward.x == 0.0F)
                        vecForward.x = 0.01F;

                    vecForward.Normalise();
                    uint8 cRand = (uint8)(CGeneral::GetRandomNumber() % 256) - 128;
                    fSpawnAngleCamRelative = ((float)cRand) / 256.0F + std::atan2(vecForward.x, vecForward.y); // [0 : 1] + atan2(...)
                }

                auto vecBirdSpawnPos = CVector(
                    std::sin(fSpawnAngleCamRelative) * fSpawnDistance + vecCamPos.x,
                    std::cos(fSpawnAngleCamRelative) * fSpawnDistance + vecCamPos.y,
                    fBirdSpawnZ
                );

                float fWaterLevel;
                if (!CWaterLevel::GetWaterLevelNoWaves(vecBirdSpawnPos, &fWaterLevel, nullptr, nullptr) || fWaterLevel + 4.0F < fBirdSpawnZ) {
                    auto vecForward = TheCamera.m_mCameraMatrix.GetForward();
                    vecForward.z = 0.0F;
                    vecForward.Normalise();

                    auto vecFlightTargetPos = vecCamPos + (vecForward * 8.0F);
                    vecFlightTargetPos.z = fBirdSpawnZ;

                    CreateNumberOfBirds(vecBirdSpawnPos, vecFlightTargetPos, iNumBirdsToCreate, biome, true);
                }
            }
        }
    }

    auto iBirdIndex = CTimer::GetFrameCounter() % std::size(aBirds);
    auto& checkedBird = aBirds[iBirdIndex];
    if (checkedBird.m_bCreated && DistanceBetweenPoints2D(vecCamPos, checkedBird.m_vecPosn) > checkedBird.m_fMaxBirdDistance)
    {
        checkedBird.m_bCreated = false;
        uiNumberOfBirds--;
    }

    for (auto & bird : aBirds) {
        if (!bird.m_bCreated)
            continue;

        if (bird.m_eBirdMode != eBirdMode::BIRD_DRAW_UPDATE || bird.m_nUpdateAfterMS > CTimer::GetTimeInMS())
            continue;

        if (bird.m_bMustDoCurves) {
            auto fCircleProgress = CTimer::GetTimeStepInSeconds() / 10.0f;
            auto vecCurTarget = CVector2D(bird.m_vecTargetVelocity);
            auto fSinAngle = std::sin(fCircleProgress);
            auto fCosAngle = std::cos(fCircleProgress);

            bird.m_vecTargetVelocity.x = fCosAngle * vecCurTarget.x + fSinAngle * vecCurTarget.y;
            bird.m_vecTargetVelocity.y = fCosAngle * vecCurTarget.y - fSinAngle * vecCurTarget.x;
        }

        auto fTimeStep = CTimer::GetTimeStepInSeconds();
        bird.m_vecCurrentVelocity = Lerp(bird.m_vecCurrentVelocity, bird.m_vecTargetVelocity, std::min(fTimeStep / 2.0F, 1.0F));
        bird.m_vecPosn += (fTimeStep * bird.m_vecCurrentVelocity);
        bird.m_fAngle = std::atan2(bird.m_vecTargetVelocity.x, bird.m_vecTargetVelocity.y);
    }
}

// 0x712810
void CBirds::Render() {
    ZoneScoped;

    if (uiNumberOfBirds == 0)
        return;

    uint32 uiWingMoveTimeOffset = 0;
    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    for (auto& bird : aBirds) {
        if (!bird.m_bCreated) {
            uiWingMoveTimeOffset += 100;
            continue;
        }

        CMatrix matBirdTransform;
        matBirdTransform.SetTranslate(bird.m_vecPosn);

        if (bird.m_eBirdMode == eBirdMode::BIRD_DRAW_NOUPDATE || bird.m_eBirdMode == eBirdMode::BIRD_DRAW_UPDATE) {
            auto& vecPos = matBirdTransform.GetPosition();
            auto uiTime = CTimer::GetTimeInMS() + uiWingMoveTimeOffset;
            vecPos.z += std::sin((float)(uiTime % bird.m_nWingStillness) * (TWO_PI / (float)bird.m_nWingStillness)) * 0.1F;
        }

        CVector vecScreenPos;
        float fScreenWidth;
        float fScreenHeight;
        if (CSprite::CalcScreenCoors(matBirdTransform.GetPosition(), &vecScreenPos, &fScreenWidth, &fScreenHeight, false, true)) {
            auto fCosSize = std::cos(bird.m_fAngle) * bird.m_fSize;
            auto fSinSize = std::sin(bird.m_fAngle) * bird.m_fSize;

            matBirdTransform.GetRight().Set(fCosSize, -fSinSize, 0.0F);
            matBirdTransform.GetForward().Set(fSinSize, fCosSize, 0.0F);
            matBirdTransform.GetUp().Set(0.0F, 0.0F, bird.m_fSize);

            if (bird.m_eBirdMode == eBirdMode::BIRD_DRAW_NOUPDATE || bird.m_eBirdMode == eBirdMode::BIRD_DRAW_UPDATE) {
                for (auto index = 0u; index < std::size(auRenderIndices); ++index) {
                    auto uiVertInd = static_cast<RxVertexIndex>(uiTempBufferVerticesStored + auRenderIndices[index]);
                    aTempBufferIndices[uiTempBufferIndicesStored + index] = uiVertInd;
                    aTempBufferIndices[uiTempBufferIndicesStored + index + std::size(auRenderIndices)] = uiVertInd + 8;
                }

                auto uiTime = CTimer::GetTimeInMS() + uiWingMoveTimeOffset;
                auto fSin1 = -std::sin((float)((uiTime + bird.m_nWingStillness / 6) % bird.m_nWingStillness) * TWO_PI / (float)bird.m_nWingStillness);
                auto fSin2 = -std::sin((float)((uiTime + bird.m_nWingStillness) % bird.m_nWingStillness) * TWO_PI / (float)bird.m_nWingStillness);

                auto fSizeUsed = std::max(1.0F, bird.m_fSize);
                auto fSin1Factor = fSin1 / fSizeUsed;
                auto fSin2Factor = fSin2 / fSizeUsed;

                auto fOffX1 = std::cos(fSin1Factor) * 0.5F;
                auto fOffZ1 = std::sin(fSin1Factor) * 0.5F;
                auto fOffX2 = std::cos(fSin2Factor) * 0.5F;
                auto fOffZ2 = std::sin(fSin2Factor) * 0.5F;

                RwUInt8 cAlpha = 255;
                const auto& vecCameraPos = TheCamera.GetPosition();
                auto fCameraDist = DistanceBetweenPoints(vecCameraPos, bird.m_vecPosn);
                auto fAlphaCutoffDist = bird.m_fMaxBirdDistance * 0.7F;
                if (fCameraDist > fAlphaCutoffDist) { // 0x712AF3 todo: review
                    auto fTransparency = 1.0F - invLerp(fAlphaCutoffDist, bird.m_fMaxBirdDistance, fCameraDist);
                    fTransparency = std::max(fTransparency, 0.0f);
                    cAlpha = static_cast<RwUInt8>(fTransparency * 255.0F);
                }

                for (auto uiVertInd = 0u; uiVertInd < std::size(faRenderPosY); ++uiVertInd) {
                    auto [point, color] = [&] {
                        switch (uiVertInd) {
                        case 0:
                        case 1:
                        case 7:
                            return std::make_pair(
                                CVector{ 0.0f, faRenderPosY[uiVertInd], 0.0f },
                                bird.m_BodyColor
                            );
                        case 2:
                        case 3:
                            return std::make_pair(
                                CVector{ fOffX1, faRenderPosY[uiVertInd], fOffZ1 },
                                bird.m_BodyColor
                            );
                        case 4:
                        case 5:
                            return std::make_pair(
                                CVector{ fOffX1 + fOffX2, faRenderPosY[uiVertInd], fOffZ1 + fOffZ2 },
                                bird.m_WingsColor
                            );
                        case 6:
                            return std::make_pair(
                                CVector{ 0.1f, faRenderPosY[uiVertInd], 0.0f },
                                bird.m_BodyColor
                            );
                        }
                        NOTSA_UNREACHABLE("CBirds::Render::lambda suppress warning");
                        return std::make_pair(CVector(), CBirdColor());
                    }();
                    auto vecWorldPos = matBirdTransform.TransformPoint(point);

                    auto iBufferInd = uiTempBufferVerticesStored + uiVertInd;
                    auto vert1 = &TempBufferVertices.m_3d[iBufferInd];
                    RwRGBA rwColor = CRGBA(color.cRed, color.cGreen, color.cBlue, cAlpha).ToRwRGBA();
                    RxObjSpace3DVertexSetPreLitColor(vert1, &rwColor);
                    RxObjSpace3DVertexSetPos(vert1, &vecWorldPos);
                    RxObjSpace3DVertexSetU(vert1, faRenderCoorsU[uiVertInd]);
                    RxObjSpace3DVertexSetV(vert1, faRenderCoorsV[uiVertInd]);

                    // Mirror on the other side with slightly changed colors
                    point.x = -point.x;
                    vecWorldPos = matBirdTransform.TransformPoint(point);
                    color.Scale(0.8F);
                    rwColor = CRGBA(color.cRed, color.cGreen, color.cBlue, cAlpha).ToRwRGBA();

                    auto vert2 = &TempBufferVertices.m_3d[iBufferInd + 8];
                    RxObjSpace3DVertexSetPreLitColor(vert2, &rwColor);
                    RxObjSpace3DVertexSetPos(vert2, &vecWorldPos);
                    RxObjSpace3DVertexSetU(vert2, faRenderCoorsU[uiVertInd]);
                    RxObjSpace3DVertexSetV(vert2, faRenderCoorsV[uiVertInd]);
                }

                uiTempBufferIndicesStored += 60;
                uiTempBufferVerticesStored += 16;
            }
        }
        uiWingMoveTimeOffset += 100;
    }

    if (uiTempBufferVerticesStored) {
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(gpCloudMaskTex->raster));
        CBrightLights::RenderOutGeometryBuffer();
    }
}

// 0x712E40
void CBirds::HandleGunShot(const CVector* pointA, const CVector* pointB) {
    for (auto& bird : aBirds) {
        if (!bird.m_bCreated)
            continue;

        CColLine line{ *pointA, *pointB };
        CColSphere sphere{ bird.m_vecPosn, 0.5f };

        if (CCollision::TestLineSphere(line, sphere)) {
            vecBirdShotAt = bird.m_vecPosn;
            bHasBirdBeenShot = true;
            bird.m_bCreated = false;
            uiNumberOfBirds--;
        }
    }
}
