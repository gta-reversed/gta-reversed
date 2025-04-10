#include "StdInc.h"

#include "BreakObject_c.h"
#include "Plugins\BreakablePlugin\BreakablePlugin.h"
#include "PostEffects.h"

void BreakObject_c::InjectHooks() {
    RH_ScopedClass(BreakObject_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CalcGroupCenter, 0x59D190);
    RH_ScopedInstall(SetGroupData, 0x59D570);
    RH_ScopedInstall(SetBreakInfo, 0x59D7F0);
    RH_ScopedInstall(Exit, 0x59DDD0);
    RH_ScopedInstall(DoCollisionResponse, 0x59DE40);
    RH_ScopedInstall(DoCollision, 0x59E1F0);
    RH_ScopedInstall(Update, 0x59E220);
    RH_ScopedInstall(Render, 0x59E480);
    RH_ScopedInstall(Init, 0x59E750);
}

// 0x59D170
BreakObject_c::BreakObject_c() {
    m_JustFaces = false;
    m_bActive = false;
    m_BreakGroups = nullptr;
}

// 0x59E750
bool BreakObject_c::Init(CObject* object, const CVector* velocity, float fVelocityRand, int32 bJustFaces) {
    if (!object->m_pRwObject || RwObjectGetType(object->m_pRwObject) != rpATOMIC)
        return false;

    auto* info = BREAKABLEPLG(RpAtomicGetGeometry(object->m_pRwAtomic), m_pBreakableInfo);
    if (!info)
        return false;

    SetBreakInfo(info, bJustFaces);
    auto ltm = RwFrameGetLTM(RpAtomicGetFrame(object->m_pRwAtomic));
    SetGroupData(ltm, velocity, fVelocityRand);

    m_JustFaces = bJustFaces;
    m_bActive = true;
    m_FramesActive = 0;
    m_AddSparks = object->m_pObjectInfo->m_nSparksOnImpact;
    m_bDrawLast = object->m_bDrawLast;

    CVector vecOrigin;
    auto* colModel = object->GetModelInfo()->GetColModel();
    if (info->m_uiPosRule != eBreakablePluginPositionRule::OBJECT_ORIGIN) {
        auto usedPoint = colModel->GetBoundingBox().GetCenter();
        usedPoint.z += 0.25f;
        vecOrigin = object->m_matrix->TransformPoint(usedPoint);
    } else {
        vecOrigin = *RwMatrixGetPos(ltm);
        vecOrigin.z += colModel->GetBoundingBox().m_vecMin.z + 0.25f;
    }

    CColPoint colPoint;
    CEntity* colEntity;
    if (CWorld::ProcessVerticalLine(vecOrigin, -1000.0f, colPoint, colEntity, true)) {
        m_GroundZ = colPoint.m_vecPoint.z;
        m_VecNormal = colPoint.m_vecNormal;
        if (std::fabs(m_VecNormal.x) < 0.01f &&
            std::fabs(m_VecNormal.y) < 0.01f &&
            std::fabs(m_VecNormal.z) < 0.01f
        ) {
            m_VecNormal.Set(0.0f, 0.0f, 1.0f);
        }
    } else {
        m_GroundZ = -1000.0f;
        m_VecNormal.Set(0.0f, 0.0f, 1.0f);
    }

    return true;
}

// 0x59DDD0
void BreakObject_c::Exit() {
    for (auto& group : GetBreakGroups()) {
        if (group.m_Texture) {
            RwTextureDestroy(group.m_Texture);
            group.m_Texture = nullptr;
        }
        delete[] group.m_RenderInfo;
    }
    delete[] m_BreakGroups;
    m_bActive = false;
}

// 0x59D190
void BreakObject_c::CalcGroupCenter(BreakGroup_t* group) {
    CBoundingBox bbox(
        { +9999999.0f, +9999999.0f, +9999999.0f }, // todo: replace one million - 1 with a more comprehensible
        { -9999999.0f, -9999999.0f, -9999999.0f }
    );
    auto& vMin = bbox.m_vecMin;
    auto& vMax = bbox.m_vecMax;

    for (auto& info : group->GetRenderInfos()) {
        for (auto& position : info.positions) {
            vMin.x = std::min(vMin.x, position.x);
            vMax.x = std::max(vMax.x, position.x);
            vMin.y = std::min(vMin.y, position.y);
            vMax.y = std::max(vMax.y, position.y);
            vMin.z = std::min(vMin.z, position.z);
            vMax.z = std::max(vMax.z, position.z);
        }
    }

    auto vecCenter = bbox.GetCenter();
    for (auto& info : group->GetRenderInfos()) {
        for (auto& position : info.positions) {
            position -= vecCenter;
        }
    }

    vMin -= vecCenter;
    vMax -= vecCenter;

    RwV3d vecTransformedCenter;
    RwV3dTransformVector(&vecTransformedCenter, &vecCenter, &group->m_Matrix);
    auto* pos = RwMatrixGetPos(&group->m_Matrix);
    RwV3dAdd(pos, pos, &vecTransformedCenter);

    auto width  = bbox.GetWidth();
    auto length = bbox.GetLength();
    auto height = bbox.GetHeight();

    if (width <= length && width <= height) {
        group->m_Type = BreakGroupType::RIGHT;
        group->m_BoundingSize = width / 2.0f;
    } else if (length <= width && length <= height) {
        group->m_Type = BreakGroupType::UP;
        group->m_BoundingSize = length / 2.0f;
    } else if (height <= length && height <= width) {
        group->m_Type = BreakGroupType::AT;
        group->m_BoundingSize = height / 2.0f;
    }
}

// 0x59D570
void BreakObject_c::SetGroupData(const RwMatrix* matrix, const CVector* vecVelocity, float fVelocityRand) {
    for (auto& group : GetBreakGroups()) {
        group.m_Matrix = *matrix;

        CalcGroupCenter(&group);

        if (vecVelocity) {
            group.m_Velocity = *vecVelocity;
            if (fVelocityRand != 0.0f) {
                group.m_Velocity.x += CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
                group.m_Velocity.y += CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
                group.m_Velocity.z += CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
            }
        } else {
            group.m_Velocity.x = CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
            group.m_Velocity.y = CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
            group.m_Velocity.z = CGeneral::GetRandomNumberInRange(-fVelocityRand, fVelocityRand);
        }

        group.m_RotationSpeed = CGeneral::GetRandomNumberInRange(3.0f, 6.0f);

        group.m_RotationAxis.x = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
        group.m_RotationAxis.y = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
        group.m_RotationAxis.z = CGeneral::GetRandomNumberInRange(-1.0f, 1.0f);
        group.m_RotationAxis.Normalise();

        group.m_bStoppedMoving = false;
    }
}

// 0x59D7F0
void BreakObject_c::SetBreakInfo(BreakInfo_t* info, int32 bJustFaces) {
    static float& ambientRed   = *(float*)0x8D0A0C;  // TODO | STATICREF // = 32.0f;
    static float& ambientGreen = *(float*)0x8D0A08;  // TODO | STATICREF // = 32.0f;
    static float& ambientBlue  = *(float*)0x8D0A04;  // TODO | STATICREF // = 32.0f;

    m_NumBreakGroups = bJustFaces ? info->m_usNumTriangles : info->m_usNumMaterials;
    m_BreakGroups = new BreakGroup_t[m_NumBreakGroups];

    for (auto i = 0; i < m_NumBreakGroups; ++i) {
        auto& group = m_BreakGroups[i];
        group.m_NumTriangles = 0;
        group.m_Texture = nullptr;

        auto numMaterials = 0u;
        if (bJustFaces)
            numMaterials = 1u;
        else {
            auto* pIndex = info->m_pTrianglesMaterialIndices;
            for (auto triInd = 0; triInd < info->m_usNumTriangles; ++triInd) {
                if (*pIndex == i)
                    ++numMaterials;

                ++pIndex;
            }
        }

        group.m_RenderInfo = new BreakGroupRenderInfo_t[numMaterials];
        group.m_FramesToLive = 256 + CGeneral::GetRandomNumberInRange(0, 32); // todo: magic numbers
    }

    for (auto i = 0; i < info->m_usNumTriangles; ++i) {
        auto curIndex = i;
        if (!bJustFaces)
            curIndex = info->m_pTrianglesMaterialIndices[curIndex];

        auto& triangle = info->m_pTriangles[i];
        auto& group = m_BreakGroups[curIndex];
        // BUG: (?) Compiler shows that invalid access can happen in next line, possibly something wrong with reversed code
        auto& curRenderInfo = group.m_RenderInfo[group.m_NumTriangles];
        for (auto ind = 0; ind < NUM_BREAK_GROUP_RENDER_INFO; ++ind) {
            curRenderInfo.positions[ind] = info->m_pVertexPos[triangle.vertIndex[ind]];
        }

        auto textureInd = bJustFaces ? info->m_pTrianglesMaterialIndices[i] : curIndex;
        group.m_Texture = info->m_pTextures[textureInd];

        if (!CPostEffects::IsVisionFXActive()) {
            ambientRed   = AmbientLightColourForFrame.red   * 255.0f;
            ambientGreen = AmbientLightColourForFrame.green * 255.0f;
            ambientBlue  = AmbientLightColourForFrame.blue  * 255.0f;
        }

        const auto& matColor = info->m_pMaterialProperties[textureInd];
        for (auto ind = 0; ind < NUM_BREAK_GROUP_RENDER_INFO; ++ind) {
            auto& vertColor = info->m_pVertexColors[triangle.vertIndex[ind]];
            auto red   = (uint8)std::min(255.0f, (float)vertColor.red   * matColor.red   + ambientRed);
            auto green = (uint8)std::min(255.0f, (float)vertColor.green * matColor.green + ambientGreen);
            auto blue  = (uint8)std::min(255.0f, (float)vertColor.blue  * matColor.blue  + ambientBlue);

            curRenderInfo.colors[ind].Set(red, green, blue);
            curRenderInfo.texCoords[ind] = info->m_pTexCoors[triangle.vertIndex[ind]];
        }

        ++group.m_NumTriangles;
    }

    for (auto& group : GetBreakGroups()) {
        if (group.m_Texture) {
            RwTextureAddRef(group.m_Texture);
        }
    }
}

// 0x59DE40
void BreakObject_c::DoCollisionResponse(BreakGroup_t* group, float timeStep, const CVector* vecNormal, float groundZ) const {
    static float& dotMultiplier = *(float*)0x8D0A18;   // TODO | STATICREF // = 0.85f;
    static float& timestepScaling = *(float*)0x8D0A14; // TODO | STATICREF // = 0.05f;
    static float& velocityScaling = *(float*)0x8D0A10; // TODO | STATICREF // = 0.8f;

    auto dotProd = DotProduct(group->m_Velocity, *vecNormal) * dotMultiplier;
    CVector velocityChange = *vecNormal;
    velocityChange *= dotProd * 2;

    CVector newVelocity = group->m_Velocity;
    newVelocity -= velocityChange;

    auto fTimeScale = timeStep * timestepScaling;
    CVector vecRand{
        CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
        CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
        CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
    };
    vecRand.Normalise();
    vecRand *= fTimeScale;

    auto fOriginalVelocityScale = newVelocity.Magnitude();
    newVelocity += vecRand;
    newVelocity.Normalise();
    newVelocity *= fOriginalVelocityScale;
    newVelocity *= velocityScaling;

    group->m_RotationSpeed = 0.0f;
    group->m_Velocity = newVelocity;
    RwMatrixGetPos(&group->m_Matrix)->z = groundZ + group->m_BoundingSize;

    if (fOriginalVelocityScale >= 0.05f) {
        if (m_JustFaces)
            return;
    } else {
        group->m_bStoppedMoving = true;
        if (m_JustFaces) {
            group->m_FramesToLive = 32 + CGeneral::GetRandomNumberInRange(0, 32); // todo: magic numbers
            return;
        }
    }

    const auto NUM_PARTICLES = 4;
    CVector groupPos = *RwMatrixGetPos(&group->m_Matrix);
    for (auto i = 0; i < NUM_PARTICLES; ++i) {
        CVector particlePos = groupPos;
        particlePos.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
        particlePos.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);

        CVector particleVelocity = {
            CGeneral::GetRandomNumberInRange(-0.15f, 0.15f),
            CGeneral::GetRandomNumberInRange(-0.15f, 0.15f),
            0.0f,
        };

        auto particle = FxPrtMult_c(1.0f, 1.0f, 1.0f, 0.1f, 0.3f, 0.0f, 0.15f);
        g_fx.m_SmokeII3expand->AddParticle(&particlePos, &particleVelocity, 0.0f, &particle, -1.0f, 1.2f, 0.6f, 0);
    }

    if (m_AddSparks) {
        CVector particleVelocity = { 0.0f, 0.0f, 1.0f };
        auto amount = static_cast<int32>(group->m_Velocity.Magnitude() * 100.0f);
        g_fx.AddSparks(groupPos, particleVelocity, 2.0f, amount, CVector(0.0f, 0.0f, 0.0f), SPARK_PARTICLE_SPARK, 0.4f, 1.0f);
    }
}

// 0x59E1F0
void BreakObject_c::DoCollision(BreakGroup_t* group, float timeStep) {
    if (RwMatrixGetPos(&group->m_Matrix)->z - group->m_BoundingSize < m_GroundZ) {
        DoCollisionResponse(group, timeStep, &m_VecNormal, m_GroundZ);
    }
}

// 0x59E220
void BreakObject_c::Update(float timeStep) {
    if (m_NumBreakGroups <= 0) {
        Exit();
        ++m_FramesActive;
        return;
    }

    auto bToBeRemoved = true;
    for (auto& group : GetBreakGroups()) {
        if (!group.m_bStoppedMoving) {
            group.m_Velocity.z -= timeStep / 125.0f;
            CVector vecVelocity = group.m_Velocity;
            vecVelocity *= timeStep;

            auto* pos = RwMatrixGetPos(&group.m_Matrix);
            RwV3dAdd(pos, pos, &vecVelocity);

            if (m_FramesActive >= 5) {
                RwV3d* vecFacing = [&]{
                    switch (group.m_Type) {
                    case BreakGroupType::UP: return RwMatrixGetUp(&group.m_Matrix);
                    case BreakGroupType::AT: return RwMatrixGetAt(&group.m_Matrix);
                    case BreakGroupType::RIGHT:
                    default:                 return RwMatrixGetRight(&group.m_Matrix);
                    }
                }();

                auto fAngleRad = std::acos(DotProduct(m_VecNormal, *vecFacing));
                if (std::fabs(fAngleRad) > 0.01f) {
                    CVector axis = CrossProduct(*vecFacing, m_VecNormal);
                    axis.Normalise();

                    auto fAngleDeg = RadiansToDegrees(fAngleRad) * timeStep / 20.0f;
                    CVector savedPos = *pos;
                    RwMatrixRotate(&group.m_Matrix, &axis, fAngleDeg, RwOpCombineType::rwCOMBINEPOSTCONCAT);
                    *pos = savedPos;
                }
            } else {
                float angle = timeStep * group.m_RotationSpeed;
                RwMatrixRotate(&group.m_Matrix, &group.m_RotationAxis, angle, RwOpCombineType::rwCOMBINEPRECONCAT);
            }

            DoCollision(&group, timeStep);
        }

        --group.m_FramesToLive;
        if (group.m_FramesToLive <= 0)
            group.m_FramesToLive = 0;
        else
            bToBeRemoved = false;
    }

    if (bToBeRemoved)
        Exit();

    ++m_FramesActive;
}

// 0x59E480
void BreakObject_c::Render(bool isDrawLast) const {
    if (isDrawLast != m_bDrawLast)
        return;

    RwRaster* lastRaster = nullptr;
    for (auto& group : GetBreakGroups()) {
        RwRaster* curRaster = nullptr;
        if (group.m_Texture)
            curRaster = RwTextureGetRaster(group.m_Texture);

        if (lastRaster != curRaster) {
            RenderEnd();
            lastRaster = curRaster;
            RenderBegin(curRaster, nullptr, RwIm3DTransformFlags::rwIM3D_VERTEXUV);
        }

        if (!curRaster && !lastRaster) {
            RenderEnd();
            lastRaster = nullptr;
            RenderBegin(nullptr, nullptr, RwIm3DTransformFlags::rwIM3D_VERTEXUV);
        }

        for (auto& renderInfo : group.GetRenderInfos()) {
            RwV3d aVecPos[NUM_BREAK_GROUP_RENDER_INFO];
            RwV3dTransformPoints(aVecPos, renderInfo.positions, NUM_BREAK_GROUP_RENDER_INFO, &group.m_Matrix);

            int32 alpha = group.m_FramesToLive * (m_JustFaces ? 8 : 2);
            alpha = std::min(alpha, 255);

            auto colors = std::to_array(renderInfo.colors);
            std::ranges::for_each(colors, [&](auto& color) { color.a = alpha; });

            RenderAddTri(
                aVecPos[0], aVecPos[1], aVecPos[2],
                renderInfo.texCoords[0], renderInfo.texCoords[1], renderInfo.texCoords[2],
                colors[0], colors[1], colors[2]
            );
        }
    }

    RenderEnd();
}
