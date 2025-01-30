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
bool COccluder::ProcessOneOccluder(CActiveOccluder* out) {
    out->m_LinesUsed = 0;
    const auto center = CVector{m_Center};

    if (!CalcScreenCoors(center, COcclusion::CenterOnScreen) || COcclusion::CenterOnScreen.z < -150.0F || COcclusion::CenterOnScreen.z > 300.0F) {
        return false;
    }
    const auto size = CVector{m_Width, m_Length, m_Height};

    out->m_DistToCam = static_cast<uint16>(COcclusion::CenterOnScreen.z - size.Magnitude());

    //auto matRotX      = CMatrix();
    //auto matRotY      = CMatrix();
    //auto matRotZ      = CMatrix();
    CMatrix transform{};

    //matRotX.SetRotateX(m_Rot.x);
    //matRotY.SetRotateY(m_Rot.y);
    //matRotZ.SetRotateZ(m_Rot.z);
    transform.SetRotate(m_Rot);
    //transform.SetRotate({m_Rot.x, m_Rot.z, m_Rot.y});
    //transform = matRotY * matRotX * matRotZ;

    COcclusion::MinXInOccluder = 999999.88F;
    COcclusion::MinYInOccluder = 999999.88F;
    COcclusion::MaxXInOccluder = -999999.88F;
    COcclusion::MaxYInOccluder = -999999.88F;

    if (size.x != 0 && size.y != 0.f && size.z != 0.f) {
        const auto right   = transform.TransformPoint(CVector(size.x / 2.0F, 0.0F, 0.0F));
        const auto up      = transform.TransformPoint(CVector(0.0F, size.y / 2.0F, 0.0F));
        const auto forward = transform.TransformPoint(CVector(0.0F, 0.0F, size.z / 2.0F));

        enum {
            DUP,
            DBOTTOM,

            DRIGHT,
            DLEFT,

            DBACK,
            DFRONT,
        };
        const std::array directions{
            up, -up,
            right, -right,
            forward, -forward
        };

        // Figure out if we see the front or back of a face
        const auto IsVertexOnScreen = [&, cam = TheCamera.GetPosition()](int32 i) {
            return (center + directions[i] - cam).Dot(directions[i]) < 0.0F;
        };
        const std::array onScreen{
            IsVertexOnScreen(DUP),
            IsVertexOnScreen(DBOTTOM),

            IsVertexOnScreen(DRIGHT),
            IsVertexOnScreen(DLEFT),

            IsVertexOnScreen(DBACK),
            IsVertexOnScreen(DFRONT),
        };

        COcclusion::OccluderCoors[0] = center + directions[DUP]     + directions[DRIGHT] + directions[DBACK]; // back top right
        COcclusion::OccluderCoors[1] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DBACK]; // back bottom right
        COcclusion::OccluderCoors[2] = center + directions[DUP]     + directions[DLEFT]  + directions[DBACK]; // back top left
        COcclusion::OccluderCoors[3] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DBACK]; // back bottom left

        COcclusion::OccluderCoors[4] = center + directions[DUP]     + directions[DRIGHT] + directions[DFRONT]; // front top right
        COcclusion::OccluderCoors[5] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DFRONT]; // front bottom right
        COcclusion::OccluderCoors[6] = center + directions[DUP]     + directions[DLEFT]  + directions[DFRONT]; // front top left
        COcclusion::OccluderCoors[7] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DFRONT]; // front bottom left

        for (auto i = 0; i < 8; ++i) {
            COcclusion::OccluderCoorsValid[i] = CalcScreenCoors(COcclusion::OccluderCoors[i], COcclusion::OccluderCoorsOnScreen[i]);
        }

        // Between two differently facing sides we see an edge, so process those
        if ((onScreen[DUP] == onScreen[DRIGHT] || !ProcessLineSegment(0, 4, out))
         && (onScreen[DUP] == onScreen[DLEFT] || !ProcessLineSegment(2, 6, out))
         && (onScreen[DUP] == onScreen[DBACK] || !ProcessLineSegment(0, 2, out))
         && (onScreen[DUP] == onScreen[DFRONT] || !ProcessLineSegment(4, 6, out))
         && (onScreen[DBOTTOM] == onScreen[DRIGHT] || !ProcessLineSegment(1, 5, out))
         && (onScreen[DBOTTOM] == onScreen[DLEFT] || !ProcessLineSegment(3, 7, out))
         && (onScreen[DBOTTOM] == onScreen[DBACK] || !ProcessLineSegment(1, 3, out))
         && (onScreen[DBOTTOM] == onScreen[DFRONT] || !ProcessLineSegment(5, 7, out))
         && (onScreen[DRIGHT] == onScreen[DBACK] || !ProcessLineSegment(0, 1, out))
         && (onScreen[DLEFT] == onScreen[DBACK] || !ProcessLineSegment(2, 3, out))
         && (onScreen[DLEFT] == onScreen[DFRONT] || !ProcessLineSegment(6, 7, out))
         && (onScreen[DRIGHT] == onScreen[DFRONT] || !ProcessLineSegment(4, 5, out))
        ) {
            if (SCREEN_WIDTH * 0.15F <= COcclusion::MaxXInOccluder - COcclusion::MinXInOccluder
             && SCREEN_HEIGHT * 0.1F <= COcclusion::MaxYInOccluder - COcclusion::MinYInOccluder
            ) {
                out->m_NumFaces = 0;
                for (auto i = 0; i < 6; ++i) {
                    if (!onScreen[i]) {
                        continue;
                    }
                    const auto& dir = directions[i].Normalized();

                    out->m_FaceNormals[out->m_NumFaces] = dir; // vecNormalised
                    out->m_FaceOffsets[out->m_NumFaces] = (center + dir).Dot(dir); // DotProduct(vecScreenPos, vecNormalised);

                    ++out->m_NumFaces;
                }
                return true;
            }
        }

        return false;
    }

    CVector right, up;
    if (size.y == 0.f) {
        right = CVector{size.x, 0.f, 0.f   };
        up    = CVector{0.f,   0.f, size.z };
    } else if (size.x == 0.f) {
        right = CVector{0.f, size.y, 0.f   };
        up    = CVector{0.f, 0.f,   size.z };
    } else if (size.z == 0.f) {
        right = CVector{0.f,    size.y, 0.f};
        up    = CVector{size.x , 0.f,   0.f};
    }
    right = transform.TransformPoint(right / 2.f);
    up    = transform.TransformPoint(up / 2.f);
    COcclusion::OccluderCoors[0] = center + right + up; // top right -- top right
    COcclusion::OccluderCoors[1] = center - right + up; // top left -- bottom right
    COcclusion::OccluderCoors[2] = center - right - up; // bottom left -- bottom left
    COcclusion::OccluderCoors[3] = center + right - up; // bottom right -- top left

    for (auto i = 0; i < 4; ++i) {
        COcclusion::OccluderCoorsValid[i] = CalcScreenCoors(COcclusion::OccluderCoors[i], COcclusion::OccluderCoorsOnScreen[i]);
    }

    if (ProcessLineSegment(0, 1, out)
     || ProcessLineSegment(1, 2, out)
     || ProcessLineSegment(2, 3, out)
     || ProcessLineSegment(3, 0, out)
    )  {
        return false;
    }
    if (COcclusion::MaxXInOccluder - COcclusion::MinXInOccluder < SCREEN_WIDTH * 0.1F
     || COcclusion::MaxYInOccluder - COcclusion::MinYInOccluder < SCREEN_HEIGHT * 0.07F
    ) {
        return false;
    }

    const auto normal = right.Cross(up);
    out->m_FaceNormals[0] = normal;
    out->m_FaceOffsets[0] = normal.Dot(center);
    out->m_NumFaces       = 1;

    return true;
       

    //if (   !ProcessLineSegment(0, 1, out)
    //    && !ProcessLineSegment(1, 2, out)
    //    && !ProcessLineSegment(2, 3, out)
    //    && !ProcessLineSegment(3, 0, out)
    //    && SCREEN_WIDTH * 0.1F <= COcclusion::MaxXInOccluder - COcclusion::MinXInOccluder
    //    && SCREEN_HEIGHT * 0.07F <= COcclusion::MaxYInOccluder - COcclusion::MinYInOccluder
    //    ) {
    //    auto vecCross = CrossProduct(offsetX, offsetY);
    //    vecCross.Normalise();
    //
    //    out->m_FaceNormals[0] = vecCross;
    //    out->m_FaceOffsets[0] = DotProduct(vecCross, pos);
    //    out->m_NumFaces       = 1;
    //
    //    return true;
    //}
    //
    //return false;
}


// 0x71E130
bool COccluder::ProcessLineSegment(int32 idxFrom, int32 idxTo, CActiveOccluder* activeOccluder) {
    if (!COcclusion::OccluderCoorsValid[idxFrom] && !COcclusion::OccluderCoorsValid[idxTo]) {
        return true;
    }

    const auto GetScreenCoorsOf = [=](int32 i) -> std::optional<CVector2D> {
        if (COcclusion::OccluderCoorsValid[i]) {
            return COcclusion::OccluderCoorsOnScreen[i];
        }

        // TODO/BUG:
        // Something's wrong here, the code for both `else` branches is the same...
        // Makes no sense, why would both coords be the same?
        const auto fromDepth = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxFrom])).z - 1.1F);
        const auto toDepth   = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxTo])).z - 1.1F);

        const auto t    = toDepth / (fromDepth + toDepth); // NOTE: I believe this is the culprit, should perhaps be `fromDepth / (fromDepth + toDepth)` if `i == idxFrom`?
        const auto from = (1.0F - t) * COcclusion::OccluderCoors[idxTo];
        const auto to   = from + t * COcclusion::OccluderCoors[idxFrom];

        if (CVector pos; CalcScreenCoors(to, pos)) {
            return pos;
        }
        return std::nullopt;
    };
    const auto from = GetScreenCoorsOf(idxFrom);
    if (!from) {
        return true;
    }
    const auto to = GetScreenCoorsOf(idxTo);
    if (!to) {
        return true;
    }
    

    //CVector screenFrom, screenTo;
    //if (COcclusion::OccluderCoorsValid[idxFrom]) {
    //    screenFrom = COcclusion::OccluderCoorsOnScreen[idxFrom];
    //} else {
    //    const auto fromDepth = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxFrom])).z - 1.1F);
    //    const auto toDepth   = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxTo])).z - 1.1F);
    //
    //    const auto t    = toDepth / (fromDepth + toDepth);
    //    const auto from = (1.0F - t) * COcclusion::OccluderCoors[idxTo];
    //    const auto to   = from + t * COcclusion::OccluderCoors[idxFrom];
    //
    //    if (!CalcScreenCoors(to, screenFrom)) {
    //        return true;
    //    }
    //}
    //
    //if (COcclusion::OccluderCoorsValid[idxTo]) {
    //    screenTo = COcclusion::OccluderCoorsOnScreen[idxTo];
    //} else {
    //    const auto fromDepth = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxFrom])).z - 1.1F);
    //    const auto toDepth   = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(COcclusion::OccluderCoors[idxTo])).z - 1.1F);
    //
    //    const auto t    = toDepth / (fromDepth + toDepth);
    //    const auto from = (1.0F - t) * COcclusion::OccluderCoors[idxTo];
    //    const auto to   = from + t * COcclusion::OccluderCoors[idxFrom];
    //
    //    if (!CalcScreenCoors(to, screenTo)) {
    //        return true;
    //    }
    //}
    //

    COcclusion::MinXInOccluder = std::min({ COcclusion::MinXInOccluder, from->x, to->x });
    COcclusion::MaxXInOccluder = std::max({ COcclusion::MaxXInOccluder, from->x, to->x });
    COcclusion::MinYInOccluder = std::min({ COcclusion::MinYInOccluder, from->y, to->y });
    COcclusion::MaxYInOccluder = std::max({ COcclusion::MaxYInOccluder, from->y, to->y });

    auto* const l = &activeOccluder->m_Lines[activeOccluder->m_LinesUsed];

    // Calculate line origin and dir
    l->Origin = *from;
    l->Dir    = *to - *from;
    if (!IsPointInsideLine(l->Origin, l->Dir, COcclusion::CenterOnScreen, 0.f)) {
        l->Origin = *to;
        l->Dir    = -l->Dir; // basically *from - *to
    }
    l->Dir = l->Dir.Normalized(&l->Length);

    if (!DoesInfiniteLineTouchScreen(l->Origin, l->Dir)) {
        return !IsPointInsideLine(
            l->Origin,
            l->Dir,
            {SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f},
            0.f
        );
    }

    ++activeOccluder->m_LinesUsed;
    return false;
}

// 0x71F960
bool COccluder::NearCamera() const {
    return CVector::Dist(m_Center, TheCamera.GetPosition()) - (std::max(m_Length, m_Width) / 2.0F) < 250.f;
}

int16 COccluder::SetNext(int16 next) {
    const auto old = m_NextIndex;
    m_NextIndex    = next;
    return old;
}
