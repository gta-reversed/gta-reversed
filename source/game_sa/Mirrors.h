/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Matrix.h"
#include "Vector.h"

enum eMirrorType : uint8 {
    MIRROR_TYPE_NONE = 0,
    MIRROR_TYPE_WALL = 1,
    MIRROR_TYPE_FLOOR = 2,
    MIRROR_TYPE_SPHERE_MAP = 3
};

class CMirrors {
public:
    static inline auto& pBuffer = StaticRef<RwRaster*>(0xC7C71C);
    static inline auto& pZBuffer = StaticRef<RwRaster*>(0xC7C720);
    static inline auto& bRenderingReflection = StaticRef<bool>(0xC7C728);
    static inline auto& d3dRestored = StaticRef<bool>(0xC7C729);
    static inline auto& TypeOfMirror = StaticRef<eMirrorType>(0xC7C724);
    static inline auto& MirrorFlags = StaticRef<uint8>(0xC7C618);
    static inline auto& MirrorV = StaticRef<float>(0xC7C61C);
    static inline auto& MirrorNormal = StaticRef<CVector>(0xC803D8);

public:
    static void InjectHooks();

    static void Init();
    static void ShutDown();

    static void BeforeConstructRenderList();
    static void BeforeMainRender();

    static void BuildCamMatrix(CMatrix& camMatrix, CVector source, CVector dest);
    static void BuildCameraMatrixForScreens(CMatrix& mat);

    static void CreateBuffer();

    static void RenderMirrorBuffer();
    static void RenderReflBuffer(bool);
    static void RenderReflections();

    static bool ShouldRenderPeds();
};
