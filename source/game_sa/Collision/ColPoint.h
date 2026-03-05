/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "ColData.h"
#include "ColLighting.h"
#include "Enums/eSurfaceType.h"

class CColPoint {
private:
    /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CColPointSA.h */
    CVector m_Position; // 0x00
    float   pad;        // 0x0C
    CVector m_Normal;   // 0x10
    float   pad2;       // 0x1C

    // col shape 1 and 2 info
    tColData m_DataA; // 0x20-0x22
    tColData m_DataB; // 0x23-0x25

    float m_Depth; // 0x28

public:
    void SetPosition(float x, float y, float z) { m_Position.Set(x, y, z); }
    void SetPosition(const CVector& pos) { m_Position = pos; }
    CVector& GetPosition() { return m_Position; }

    void SetNormal(float x, float y, float z) { m_Normal.Set(x, y, z); }
    void SetNormal(const CVector& pos) { m_Normal = pos; }
    CVector& GetNormal() { return m_Normal; }

    void SetDataA(const tColData& data) { m_DataA = data; }
    tColData GetDataA() const { return m_DataA;}
    void SetSurfaceTypeA(eSurfaceType surfaceType) { m_DataA.SurfaceType = surfaceType; }
    eSurfaceType GetSurfaceTypeA() const { return m_DataA.SurfaceType; }
    void SetPieceTypeA(uint8 pieceType) { m_DataA.PieceType = pieceType; }
    uint8 GetPieceTypeA() const { return m_DataA.PieceType; }
    void SetLightingA(tColLighting lighting) { m_DataA.Lighting = lighting; }
    tColLighting GetLightingA() const { return m_DataA.Lighting; }

    void SetDataB(const tColData& data) { m_DataB = data; }
    tColData GetDataB() const { return m_DataB; }
    void SetSurfaceTypeB(eSurfaceType surfaceType) { m_DataB.SurfaceType = surfaceType; }
    eSurfaceType GetSurfaceTypeB() const { return m_DataB.SurfaceType; }
    void SetPieceTypeB(uint8 pieceType) { m_DataB.PieceType = pieceType; }
    uint8 GetPieceTypeB() const { return m_DataB.PieceType; }
    void SetLightingB(tColLighting lighting) { m_DataB.Lighting = lighting; }
    tColLighting GetLightingB() const { return m_DataB.Lighting; }

    void SetDepth(float depth) { m_Depth = depth; }
    float GetDepth() const { return m_Depth; }

    CColPoint& operator=(const CColPoint& p) = default;
};

VALIDATE_SIZE(CColPoint, 0x2C);
