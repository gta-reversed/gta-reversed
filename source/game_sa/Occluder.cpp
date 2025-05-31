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

    if (!CalcScreenCoors(center, CenterOnScreen)) {
        return false;
    }
    if (CenterOnScreen.z < -150.0F || CenterOnScreen.z > 300.0F) {
        return false;
    }
    const auto size = CVector{m_Width, m_Length, m_Height};

    out->m_DistToCam = (uint16)(CenterOnScreen.z - size.Magnitude());

    CMatrix transform{};
    transform.SetRotate(m_Rot);

    MinXInOccluder = 999999.88F;
    MinYInOccluder = 999999.88F;
    MaxXInOccluder = -999999.88F;
    MaxYInOccluder = -999999.88F;

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

        OccluderCoors[0] = center + directions[DUP]     + directions[DRIGHT] + directions[DBACK]; // back top right
        OccluderCoors[1] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DBACK]; // back bottom right
        OccluderCoors[2] = center + directions[DUP]     + directions[DLEFT]  + directions[DBACK]; // back top left
        OccluderCoors[3] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DBACK]; // back bottom left

        OccluderCoors[4] = center + directions[DUP]     + directions[DRIGHT] + directions[DFRONT]; // front top right
        OccluderCoors[5] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DFRONT]; // front bottom right
        OccluderCoors[6] = center + directions[DUP]     + directions[DLEFT]  + directions[DFRONT]; // front top left
        OccluderCoors[7] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DFRONT]; // front bottom left

        for (auto i = 0; i < 8; ++i) {
            OccluderCoorsValid[i] = CalcScreenCoors(OccluderCoors[i], OccluderCoorsOnScreen[i]);
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
            if (SCREEN_WIDTH * 0.15F <= MaxXInOccluder - MinXInOccluder
             && SCREEN_HEIGHT * 0.1F <= MaxYInOccluder - MinYInOccluder
            ) {
                out->m_NumFaces = 0;
                for (auto i = 0; i < 6; ++i) {
                    if (!onScreen[i]) {
                        continue;
                    }
                    const auto& dir = directions[i].Normalized();

                    out->m_FaceNormals[out->m_NumFaces] = dir;
                    out->m_FaceOffsets[out->m_NumFaces] = (center + dir).Dot(dir);

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
    OccluderCoors[0] = center + right + up; // top right -- top right
    OccluderCoors[1] = center - right + up; // top left -- bottom right
    OccluderCoors[2] = center - right - up; // bottom left -- bottom left
    OccluderCoors[3] = center + right - up; // bottom right -- top left

    for (auto i = 0; i < 4; ++i) {
        OccluderCoorsValid[i] = CalcScreenCoors(OccluderCoors[i], OccluderCoorsOnScreen[i]);
    }

    if (ProcessLineSegment(0, 1, out)
     || ProcessLineSegment(1, 2, out)
     || ProcessLineSegment(2, 3, out)
     || ProcessLineSegment(3, 0, out)
    )  {
        return false;
    }
    if (MaxXInOccluder - MinXInOccluder < SCREEN_WIDTH * 0.1F
     || MaxYInOccluder - MinYInOccluder < SCREEN_HEIGHT * 0.07F
    ) {
        return false;
    }

    const auto normal = right.Cross(up);
    out->m_FaceNormals[0] = normal;
    out->m_FaceOffsets[0] = normal.Dot(center);
    out->m_NumFaces       = 1;

    return true;
}


// 0x71E130
bool COccluder::ProcessLineSegment(int32 idxFrom, int32 idxTo, CActiveOccluder* activeOccluder) {
    if (!OccluderCoorsValid[idxFrom] && !OccluderCoorsValid[idxTo]) {
        return true;
    }

    // Calcualte/get on-screen coordinates of the occluder
    const auto GetScreenCoorsOf = [=](int32 i, int32 other) -> std::optional<CVector2D> {
        if (OccluderCoorsValid[i]) {
            return OccluderCoorsOnScreen[i];
        }

        // BUG/NOTE/TODO:
        // Originally they used `idxFrom` and `idxTo` for both calculations
        // but that doesn't make sense, so I changed it up to be like this
        // I haven't noticed any real-world difference, but we'll see....
        // (So, to undo the change, just change `i` to `idxFrom` and `other` to `idxTo`)
        const auto zA = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(OccluderCoors[i])).z - 1.1F);
        const auto zB = std::fabsf((TheCamera.m_mViewMatrix.TransformPoint(OccluderCoors[other])).z - 1.1F);
        const auto pt = lerp(OccluderCoors[other], OccluderCoors[i], zB / (zA + zB));

        if (CVector pos; CalcScreenCoors(pt, pos)) {
            return pos;
        }
        return std::nullopt;
    };
    const auto from = GetScreenCoorsOf(idxFrom, idxTo);
    if (!from) {
        return true;
    }
    const auto to = GetScreenCoorsOf(idxTo, idxFrom);
    if (!to) {
        return true;
    }

    MinXInOccluder = std::min({ MinXInOccluder, from->x, to->x });
    MaxXInOccluder = std::max({ MaxXInOccluder, from->x, to->x });
    MinYInOccluder = std::min({ MinYInOccluder, from->y, to->y });
    MaxYInOccluder = std::max({ MaxYInOccluder, from->y, to->y });

    // Now see if 
    auto* const l = &activeOccluder->m_Lines[activeOccluder->m_LinesUsed];

    // Calculate line origin and dir
    l->Origin = *from;
    l->Dir    = *to - *from;
    if (!IsPointInsideLine(l->Origin, l->Dir, CenterOnScreen, 0.f)) {
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
