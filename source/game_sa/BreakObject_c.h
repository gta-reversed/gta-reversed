#pragma once

#include "Base.h"

#include "Vector.h"
#include <rwplcore.h>

struct BreakInfo_t;

constexpr auto NUM_BREAK_GROUP_RENDER_INFO = 3;

struct BreakGroupRenderInfo_t {
    std::array<CVector, NUM_BREAK_GROUP_RENDER_INFO>     positions;
    std::array<RwTexCoords, NUM_BREAK_GROUP_RENDER_INFO> texCoords;
    std::array<CRGBA, NUM_BREAK_GROUP_RENDER_INFO>       colors;
};

enum class BreakGroupType : uint8 {
    RIGHT = 0,
    UP    = 1,
    AT    = 2,
};

struct BreakGroup_t {
    RwMatrix                m_Matrix;
    CVector                 m_Velocity;
    bool                    m_bStoppedMoving;
    int16                   m_NumTriangles;
    BreakGroupRenderInfo_t* m_RenderInfo;
    RwTexture*              m_Texture;
    BreakGroupType          m_Type;
    float                   m_BoundingSize;
    float                   m_RotationSpeed;
    CVector                 m_RotationAxis;
    int32                   m_FramesToLive;

    auto GetRenderInfos() const { return std::span{ m_RenderInfo, (size_t)m_NumTriangles }; }
};
VALIDATE_SIZE(BreakGroup_t, 0x74);

class BreakObject_c {
public:
    bool          m_JustFaces;
    bool          m_bActive;
    bool          m_AddSparks;
    bool          m_bDrawLast;
    int32         m_NumBreakGroups;
    BreakGroup_t* m_BreakGroups;
    int32         m_FramesActive;
    float         m_GroundZ;
    CVector       m_VecNormal;

public:
    static void InjectHooks();

    BreakObject_c();
    ~BreakObject_c() = default;

    bool Init(CObject* object, const CVector* velocity, float fVelocityRand, int32 bJustFaces);
    void Exit();

    void CalcGroupCenter(BreakGroup_t* group);
    void SetGroupData(const RwMatrix* matrix, const CVector* vecVelocity, float fVelocityRand);
    void SetBreakInfo(BreakInfo_t* info, int32 bJustFaces);

    void DoCollisionResponse(BreakGroup_t* group, float timeStep, const CVector* vecNormal, float groundZ) const;
    void DoCollision(BreakGroup_t* group, float timeStep);
    void Update(float timeStep);
    void Render(bool isDrawLast) const;

    auto GetBreakGroups() { return std::span{ m_BreakGroups, (size_t)m_NumBreakGroups }; }
    [[nodiscard]] auto GetBreakGroups() const { return std::span{ m_BreakGroups, (size_t)m_NumBreakGroups }; }
};
VALIDATE_SIZE(BreakObject_c, 0x20);
