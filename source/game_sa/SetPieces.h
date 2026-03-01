#pragma once

class CSetPiece;

static constexpr auto MAX_SET_PIECES = 210;

class CSetPieces {
public:
    static inline auto& aSetPieces = StaticRef<std::array<CSetPiece, MAX_SET_PIECES>>(0xA95818);
    static inline auto& NumSetPieces = StaticRef<uint32>(0xA957FC);
    static inline auto& bDebug = StaticRef<bool>(0xA957F8);

public:
    static void InjectHooks();

    static void AddOne(eSetPieceType type, CVector2D cornerA, CVector2D cornerB, CVector2D spawnCoord1, CVector2D targetCoord1, CVector2D spawnCoord2, CVector2D targetCoord2);
    static void Init();
    static bool Load();
    static bool Save();
    static void Update();
};
