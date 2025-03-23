/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <extensions/EntityRef.hpp>
#include <extensions/FixedFloat.hpp>
#include <common.h>
#include "Vector.h"

class CEntity;
class CPed;

class CCoverPoint {
public:
    using Dir = FixedFloat<uint8, 256.f / TWO_PI>; // [0, 2π)

    enum class eType : int8 {
        NONE       = 0, // 0x0
        OBJECT     = 1, // 0x1
        VEHICLE    = 2, // 0x2
        POINTONMAP = 3, // 0x3

        NUM
    };

    enum class eUsage : int8 {
        LOWCOVER    = 0, // 0x0
        WALLTOLEFT  = 1, // 0x1
        WALLTORIGHT = 2, // 0x2
    };

public:
    static void InjectHooks();

    CCoverPoint() = default;
    CCoverPoint(eType type, eUsage usage, Dir dir, CEntity* coverEntity, const CVector* pos);
     
    bool CanAccommodateAnotherPed() const noexcept;
    void ReleaseCoverPointForPed(CPed* ped);
    void ReserveCoverPointForPed(CPed* ped);
    bool FindCoordinatesCoverPoint(CPed* ped, const CVector& position, CVector& outPt) const noexcept;
    void Remove();
    void Update();
    bool CanBeRemoved() const;
    auto GetType() const { return m_Type; }
    auto GetUsage() const { return m_Usage; }
    auto GetDir() const { return m_Dir; }
    auto GetDirVector() const { return CVector{ std::sin(m_Dir), std::cos(m_Dir), 0.f }; } // Based on `CCover::FindVectorFromDir`
    auto GetCoverEntity() const { return m_CoverEntity.Get(); }
    auto GetPointOnMap() const { assert(m_Type == eType::POINTONMAP); return m_PointOnMap; }
    CVector GetPos() const;
    auto GetCoveredPeds(this auto&& self) { return self.m_CoveredPeds | rng::views::transform([](auto&& p) { return p.Get(); }); }

protected:
    eType                                 m_Type{};        //!< Type of this cover point
    eUsage                                m_Usage{};       //!< Usage of this cover point
    Dir                                   m_Dir{};         //!< [0, 2π)
    CVector                               m_PointOnMap{};  //!< Only used when `m_Type` is `POINTONMAP`
    notsa::EntityRef<CEntity>             m_CoverEntity{}; //!< Only used when `m_Type` is `OBJECT` or `VEHICLE`
    std::array<notsa::EntityRef<CPed>, 2> m_CoveredPeds{}; //!< Pes currently covered by this cover point
};

VALIDATE_SIZE(CCoverPoint, 0x1C);
