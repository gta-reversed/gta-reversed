#include "StdInc.h"

#include "ActiveOccluder.h"
#include "Occluder.h"
#include "Occlusion.h"

void COccluder::InjectHooks() {
    RH_ScopedClass(COccluder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ProcessOneOccluder, 0x71E5D0);
    RH_ScopedInstall(ProcessLineSegment, 0x71E130);
    RH_ScopedInstall(NearCamera, 0x71F960);
}

// 0x71E5D0
bool COccluder::ProcessOneOccluder(CActiveOccluder* activeOccluder)
{
    activeOccluder->m_cLinesCount = 0;
    auto vecPos = CVector(m_wMidX, m_wMidY, m_wMidZ) / 4.0F;
    float temp1, temp2;

    if (!CalcScreenCoors(vecPos, COcclusion::CenterOnScreen, temp1, temp2) || COcclusion::CenterOnScreen.z < -150.0F || COcclusion::CenterOnScreen.z > 300.0F)
        return false;

    auto fMagnitude = (CVector(m_wWidth, m_wLength, m_wHeight)  / 4.0F ).Magnitude();
    activeOccluder->m_wDepth = static_cast<uint16>(COcclusion::CenterOnScreen.z - fMagnitude);

    auto matRotX = CMatrix();
    auto matRotY = CMatrix();
    auto matRotZ = CMatrix();
    auto matTransform = CMatrix();

    matRotX.SetRotateX(static_cast<float>(m_cRotX) * PI / 128.0F);
    matRotY.SetRotateY(static_cast<float>(m_cRotY) * PI / 128.0F);
    matRotZ.SetRotateZ(static_cast<float>(m_cRotZ) * PI / 128.0F);
    matTransform = (matRotY * matRotX) * matRotZ;

    COcclusion::MinXInOccluder = 999999.88F;
    COcclusion::MinYInOccluder = 999999.88F;
    COcclusion::MaxXInOccluder = -999999.88F;
    COcclusion::MaxYInOccluder = -999999.88F;

    auto fLength = static_cast<float>(m_wLength);
    auto fWidth = static_cast<float>(m_wWidth);
    auto fHeight = static_cast<float>(m_wHeight);

    if (   fLength / 4.0F != 0.0F
        && fWidth  / 4.0F != 0.0F
        && fHeight / 4.0F != 0.0F) {
        auto vecWidth = CVector(fWidth / 8.0F, 0.0F, 0.0F);
        auto vecTransWidth = matTransform.TransformPoint(vecWidth);

        auto vecLength = CVector(0.0F, fLength / 8.0F, 0.0F);
        auto vecTransLength = matTransform.TransformPoint(vecLength);

        auto vecHeight = CVector(0.0F, 0.0F, fHeight / 8.0F);
        auto vecTransHeight = matTransform.TransformPoint(vecHeight);

        CVector aVecArr[6]{
            vecTransLength, -vecTransLength,
            vecTransWidth, -vecTransWidth,
            vecTransHeight, -vecTransHeight
        };

        // Figure out if we see the front or back of a face
        const auto& vecCamPos = TheCamera.GetPosition();
        bool abOnScreen[6]{
            DotProduct((vecPos + aVecArr[0] - vecCamPos), aVecArr[0]) < 0.0F,
            DotProduct((vecPos + aVecArr[1] - vecCamPos), aVecArr[1]) < 0.0F,
            DotProduct((vecPos + aVecArr[2] - vecCamPos), aVecArr[2]) < 0.0F,
            DotProduct((vecPos + aVecArr[3] - vecCamPos), aVecArr[3]) < 0.0F,
            DotProduct((vecPos + aVecArr[4] - vecCamPos), aVecArr[4]) < 0.0F,
            DotProduct((vecPos + aVecArr[5] - vecCamPos), aVecArr[5]) < 0.0F
        };

        // Calculating vertices of a box
        COcclusion::OccluderCoors[0] = vecPos + aVecArr[0] + aVecArr[2] + aVecArr[4];
        COcclusion::OccluderCoors[1] = vecPos + aVecArr[1] + aVecArr[2] + aVecArr[4];
        COcclusion::OccluderCoors[2] = vecPos + aVecArr[0] + aVecArr[3] + aVecArr[4];
        COcclusion::OccluderCoors[3] = vecPos + aVecArr[1] + aVecArr[3] + aVecArr[4];
        COcclusion::OccluderCoors[4] = vecPos + aVecArr[0] + aVecArr[2] + aVecArr[5];
        COcclusion::OccluderCoors[5] = vecPos + aVecArr[1] + aVecArr[2] + aVecArr[5];
        COcclusion::OccluderCoors[6] = vecPos + aVecArr[0] + aVecArr[3] + aVecArr[5];
        COcclusion::OccluderCoors[7] = vecPos + aVecArr[1] + aVecArr[3] + aVecArr[5];

        for (auto i = 0; i < 8; ++i) {
            COcclusion::OccluderCoorsValid[i] = CalcScreenCoors(COcclusion::OccluderCoors[i], COcclusion::OccluderCoorsOnScreen[i], temp1, temp2);
        }

        // Between two differently facing sides we see an edge, so process those
        if (   (abOnScreen[0] == abOnScreen[2] || !ProcessLineSegment(0, 4, activeOccluder))
            && (abOnScreen[0] == abOnScreen[3] || !ProcessLineSegment(2, 6, activeOccluder))
            && (abOnScreen[0] == abOnScreen[4] || !ProcessLineSegment(0, 2, activeOccluder))
            && (abOnScreen[0] == abOnScreen[5] || !ProcessLineSegment(4, 6, activeOccluder))
            && (abOnScreen[1] == abOnScreen[2] || !ProcessLineSegment(1, 5, activeOccluder))
            && (abOnScreen[1] == abOnScreen[3] || !ProcessLineSegment(3, 7, activeOccluder))
            && (abOnScreen[1] == abOnScreen[4] || !ProcessLineSegment(1, 3, activeOccluder))
            && (abOnScreen[1] == abOnScreen[5] || !ProcessLineSegment(5, 7, activeOccluder))
            && (abOnScreen[2] == abOnScreen[4] || !ProcessLineSegment(0, 1, activeOccluder))
            && (abOnScreen[3] == abOnScreen[4] || !ProcessLineSegment(2, 3, activeOccluder))
            && (abOnScreen[3] == abOnScreen[5] || !ProcessLineSegment(6, 7, activeOccluder))
            && (abOnScreen[2] == abOnScreen[5] || !ProcessLineSegment(4, 5, activeOccluder))
            && SCREEN_WIDTH * 0.15F <= COcclusion::MaxXInOccluder - COcclusion::MinXInOccluder
            && SCREEN_HEIGHT * 0.1F <= COcclusion::MaxYInOccluder - COcclusion::MinYInOccluder) {

            activeOccluder->m_cNumVectors = 0;
            for (auto i = 0; i < 6; ++i) {
                if (abOnScreen[i]) {
                    auto vecNormalised = CVector(aVecArr[i]);
                    vecNormalised.Normalise();
                    auto vecScreenPos = vecPos + aVecArr[i];

                    activeOccluder->m_aVectors[activeOccluder->m_cNumVectors] = vecNormalised;
                    activeOccluder->m_afRadiuses[activeOccluder->m_cNumVectors] = DotProduct(vecScreenPos, vecNormalised);
                    ++activeOccluder->m_cNumVectors;
                }
            }
            return true;
        }

        return false;
    }

    CVector vec1, vec2;
    if (fLength / 4.0F == 0.0F) {
        auto vecWidth = CVector(fWidth / 8.0F, 0.0F, 0.0F);
        vec1 = matTransform.TransformPoint(vecWidth);

        auto vecHeight = CVector(0.0F, 0.0F, fHeight / 8.0F);
        vec2 = matTransform.TransformPoint(vecHeight);
    }
    else if (fWidth / 4.0F == 0.0F) {
        auto vecLength = CVector(0.0F, fLength / 8.0F, 0.0F);
        vec1 = matTransform.TransformPoint(vecLength);

        auto vecHeight = CVector(0.0F, 0.0F, fHeight / 8.0F);
        vec2 = matTransform.TransformPoint(vecHeight);
    }
    else if (fHeight / 4.0F == 0.0F) {
        auto vecLength = CVector(0.0F, fLength / 8.0F, 0.0F);
        vec1 = matTransform.TransformPoint(vecLength);

        auto vecWidth = CVector(fWidth / 8.0F, 0.0F, 0.0F);
        vec2 = matTransform.TransformPoint(vecWidth);
    }

    COcclusion::OccluderCoors[0] = vecPos + vec1 + vec2;
    COcclusion::OccluderCoors[1] = vecPos - vec1 + vec2;
    COcclusion::OccluderCoors[2] = vecPos - vec1 - vec2;
    COcclusion::OccluderCoors[3] = vecPos + vec1 - vec2;

    for (auto i = 0; i < 4; ++i) {
        COcclusion::OccluderCoorsValid[i] = CalcScreenCoors(COcclusion::OccluderCoors[i], COcclusion::OccluderCoorsOnScreen[i], temp1, temp2);
    }

    if (   !ProcessLineSegment(0, 1, activeOccluder)
        && !ProcessLineSegment(1, 2, activeOccluder)
        && !ProcessLineSegment(2, 3, activeOccluder)
        && !ProcessLineSegment(3, 0, activeOccluder)
        && SCREEN_WIDTH  * 0.1F  <= COcclusion::MaxXInOccluder - COcclusion::MinXInOccluder
        && SCREEN_HEIGHT * 0.07F <= COcclusion::MaxYInOccluder - COcclusion::MinYInOccluder){

        auto vecCross = CrossProduct(vec1, vec2);
        vecCross.Normalise();

        activeOccluder->m_aVectors[0] = vecCross;
        activeOccluder->m_afRadiuses[0] = DotProduct(vecCross, vecPos);
        activeOccluder->m_cNumVectors = 1;

        return true;
    }

    return false;
}

// 0x71E130
bool COccluder::ProcessLineSegment(int32 iIndFrom, int32 iIndTo, CActiveOccluder* activeOccluder)
{
    if (!COcclusion::OccluderCoorsValid[iIndFrom] && !COcclusion::OccluderCoorsValid[iIndTo])
        return true;

    CVector vecScreenFrom, vecScreenTo;
    if (COcclusion::OccluderCoorsValid[iIndFrom]) {
        vecScreenFrom = COcclusion::OccluderCoorsOnScreen[iIndFrom];
    }
    else {
        auto fFromDepth = fabs((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[iIndFrom])).z - 1.1F);
        auto fToDepth = fabs((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[iIndTo])).z - 1.1F);

        auto fProgress = fToDepth / (fFromDepth + fToDepth);
        vecScreenFrom = (1.0F - fProgress) * COcclusion::OccluderCoors[iIndTo];
        vecScreenTo = vecScreenFrom + fProgress * COcclusion::OccluderCoors[iIndFrom];

        float fTemp1, fTemp2;
        if (!CalcScreenCoors(vecScreenTo, vecScreenFrom, fTemp1, fTemp2))
            return true;
    }

    if (COcclusion::OccluderCoorsValid[iIndTo]) {
        vecScreenTo = COcclusion::OccluderCoorsOnScreen[iIndTo];
    }
    else {
        auto fFromDepth = fabs((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[iIndFrom])).z - 1.1F);
        auto fToDepth = fabs((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[iIndTo])).z - 1.1F);

        auto fProgress = fToDepth / (fFromDepth + fToDepth);
        auto vecFrom = (1.0F - fProgress) * COcclusion::OccluderCoors[iIndTo];
        auto vecTo = vecFrom + fProgress * COcclusion::OccluderCoors[iIndFrom];

        float fTemp1, fTemp2;
        if (!CalcScreenCoors(vecTo, vecScreenTo, fTemp1, fTemp2))
            return true;
    }

    COcclusion::MinXInOccluder = std::min({ COcclusion::MinXInOccluder, vecScreenFrom.x, vecScreenTo.x });
    COcclusion::MaxXInOccluder = std::max({ COcclusion::MaxXInOccluder, vecScreenFrom.x, vecScreenTo.x });
    COcclusion::MinYInOccluder = std::min({ COcclusion::MinYInOccluder, vecScreenFrom.y, vecScreenTo.y });
    COcclusion::MaxYInOccluder = std::max({ COcclusion::MaxYInOccluder, vecScreenFrom.y, vecScreenTo.y });

    auto fXSize = vecScreenTo.x - vecScreenFrom.x;
    auto fYSize = vecScreenTo.y - vecScreenFrom.y;
    auto fFromX = vecScreenFrom.x;
    auto fFromY = vecScreenFrom.y;

    if (!IsPointInsideLine(vecScreenFrom.x, vecScreenFrom.y, fXSize, fYSize, COcclusion::CenterOnScreen.x, COcclusion::CenterOnScreen.y, 0.0F)) {
        fFromX = vecScreenFrom.x + fXSize;
        fFromY = vecScreenFrom.y + fYSize;
        fXSize = -fXSize;
        fYSize = -fYSize;
    }

    auto& pCurLine = activeOccluder->m_aLines[activeOccluder->m_cLinesCount];
    pCurLine.m_fLength = CVector2D(fXSize, fYSize).Magnitude();

    auto fRecip = 1.0F / pCurLine.m_fLength;
    pCurLine.m_vecOrigin.Set(fFromX, fFromY);
    pCurLine.m_vecDirection.Set(fRecip * fXSize, fRecip * fYSize);

    if (DoesInfiniteLineTouchScreen(fFromX, fFromY, pCurLine.m_vecDirection.x, pCurLine.m_vecDirection.y)) {
        ++activeOccluder->m_cLinesCount;
        return false;
    }

    return !IsPointInsideLine(fFromX, fFromY, pCurLine.m_vecDirection.x, pCurLine.m_vecDirection.y, SCREEN_WIDTH * 0.5F, SCREEN_HEIGHT * 0.5F, 0.0F);
}

// 0x71F960
bool COccluder::NearCamera() const
{
    auto fSize = std::max(
        static_cast<float>(m_wLength) / 4.0F,
        static_cast<float>(m_wWidth) / 4.0F
    );
    auto vecPos = CVector(m_wMidX, m_wMidY, m_wMidZ) / 4.0F;
    auto fDist = DistanceBetweenPoints(vecPos, TheCamera.GetPosition());
    return (fDist - fSize / 2.0F) < 250.0F;
}

int16 COccluder::SetNext(int16 next) {
    const auto old = m_nNextIndex;
    m_nNextIndex = next;
    return old;
}
