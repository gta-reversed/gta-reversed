#pragma once

// Helper for loading col binary files

// NOTE:
// The term `face` and `triangle` are used interchangably here.
#include "Vector.h"
#include "Sphere.h"
#include "Box.h"
#include "CompressedVector.h"

// Based on https://gtamods.com/wiki/Collision_File

// The term `Face` and `Tri` are interchangeable. (Mean the same thing)

namespace ColHelpers {
enum class ColModelVersion {
    COLL = 1, // NOTSA - But we want COLX => X, instead of X - 1
    COL2,
    COL3,
    COL4,

    NONE = -1 // Placeholder for invalid values (not that it should ever happen)
};

// Top-most header for every file
struct FileHeader {
    struct FileInfo {
        char fourcc[4]{}; // Not null terminated. Either: COLL, COL2, COL3, COL4
        uint32 size{};

        // Get version based on fourcc
        [[nodiscard]] auto GetVersion() const {
            switch (MakeFourCC(fourcc)) {
            case MakeFourCC("COLL"): return ColModelVersion::COLL;
            case MakeFourCC("COL2"): return ColModelVersion::COL2;
            case MakeFourCC("COL3"): return ColModelVersion::COL3;
            case MakeFourCC("COL4"): return ColModelVersion::COL4;
            default:
                // It's ok if this happens - Since the buffer it was read from might not contain more col data, and we've just read padding.
                return ColModelVersion::NONE;
            }
        }

        [[nodiscard]] auto IsValid() const {
            return GetVersion() != ColModelVersion::NONE;
        }
    } info;

    char   modelName[22]{};
    uint16 modelId{};

public:
    // Total col data size, including header
    [[nodiscard]] auto GetTotalSize() const {
        return info.size + sizeof(FileInfo); // `size` doesn't `FileInfo`s size
    }

    // Size of data after header
    [[nodiscard]] auto GetDataSize() const {
        return GetTotalSize() - sizeof(FileHeader); // Basically boils down to info.size - (sizeof(FileHeader) - sizeof(FileInfo)) = info.size - 24
    }

    // Get version based on fourcc
    [[nodiscard]] auto GetVersion() const {
        return info.GetVersion();
    }
};
static_assert(std::is_trivially_copyable_v<FileHeader>);
VALIDATE_SIZE(FileHeader::FileInfo, 0x8);
VALIDATE_SIZE(FileHeader, 0x20);

struct TSurface {
    eSurfaceType material;
    uint8 flag, brightness;
    tColLighting light;
};
static_assert(std::is_trivially_copyable_v<TSurface>);
VALIDATE_SIZE(TSurface, 4);

struct TBox : CBox {
    TSurface surface{};

    operator CColBox() const {
        return {
            *reinterpret_cast<const CBox*>(this),
            surface.material,
            surface.flag,
            surface.light
        };
    }
};
//static_assert(std::is_trivially_copyable_v<TBox>);
VALIDATE_SIZE(TBox, 28);

// NOTE: Face = triangle
struct TFaceGroup {
    // Bounding box of all faces in this group.
    // TODO: Check if all vertices of these triangles are within the BB or not - It might be a useful information to know.
    CBoundingBox bb{};
    uint16       first{}, last{}; // First and last face index (Inclusive: [first, last])
};
//static_assert(std::is_trivially_copyable_v<TFaceGroup>);
VALIDATE_SIZE(TFaceGroup, 28);

namespace V1 {

using TVector = CVector;
using TVertex = TVector;

struct TSphere {
    // Unfortunately can't use CSphere, because `center` and `radius` are swapped
    float radius{};
    TVector center{};
    TSurface surface{};

    operator CColSphere() const {
        return {
            { center, radius },
            surface.material,
            surface.flag,
            surface.light
        };
    }
};
//static_assert(std::is_trivially_copyable_v<TSphere>);
VALIDATE_SIZE(TSphere, 20);

struct TFace {
    uint32 a{}, b{}, c{};
    TSurface surface{};

    operator CColTriangle() {
        return {
            (uint16)a, (uint16)b, (uint16)c,
            surface.material, surface.light
        };
    }
};
static_assert(std::is_trivially_copyable_v<TFace>);
VALIDATE_SIZE(TFace, 16);

struct TBounds {
    // Unfortunately can't use CSphere, because `center` and `radius` are swapped
    struct {
        float radius{};
        TVector center{};

        operator CSphere() const { return { center, radius }; } // Implicitly convert to an actual CSphere
    } sphere;
    CBoundingBox box{};
};
//static_assert(std::is_trivially_copyable_v<TBounds>);
VALIDATE_SIZE(TBounds, 40);

// Version specific header after FileHeader
struct Header {
    TBounds bounds{};
};

}; // namespace V1

namespace V2 {
using TVertex = CompressedVector;
static_assert(std::is_trivially_copyable_v<TVertex>);
VALIDATE_SIZE(TVertex, 6);

struct TSphere : CSphere {
    TSurface surface{};
};
//static_assert(std::is_trivially_copyable_v<TSphere>);
VALIDATE_SIZE(TSphere, 20);

struct TBounds {
    CBoundingBox box{};
    CSphere sphere{};
};
//static_assert(std::is_trivially_copyable_v<TBounds>);
VALIDATE_SIZE(TBounds, 40);

struct TFace {
    uint16 a{}, b{}, c{};
    uint8 material{}, light{};
};
static_assert(std::is_trivially_copyable_v<TFace>);
VALIDATE_SIZE(TFace, 8);

// Version specific header after FileHeader
struct Header {
    TBounds bounds{};

    uint16 nSpheres{}, nBoxes{}, nFaces{};
    uint8 nLines{};

    // Quote from https://gtamods.com/wiki/Collision_File :
    // 1 - collision uses cones instead of lines(flag forced to false by engine upon loading)
    // 2 - not empty(collision model has spheres or boxes or a mesh)
    // 8 - has face groups(if not empty)
    // 16 - has shadow mesh(col 3)
    uint32 flags{};

    uint32 offSpheres{}, offBoxes{}, offLines{}, offVerts{}, offFaces{}, offPlanes{};

    [[nodiscard]] bool IsEmpty() const { return !(flags & 2); }
    [[nodiscard]] bool HasFaceGroups() const { return flags & 8; }
};
//static_assert(std::is_trivially_copyable_v<Header>);
VALIDATE_SIZE(Header, 0x4C);

}; // namespace V2

namespace V3 {
using namespace V2; // Inherit all others stuff

// Header for V3
struct Header : V2::Header {
    // NOTE: Face <=> Triangle

    uint32 nShdwFaces{};
    uint32 offShdwVerts{}, offShdwFaces{};

    // Basically just find the highest shadow vertex index, 0x537510
    uint32 GetNoOfShdwVerts(CCollisionData* cd) const {
        assert(cd->m_nNumShadowTriangles == nShdwFaces);
        if (!nShdwFaces) {
            return 0;
        }
        uint32 maxVert{0};
        for (auto& tri : cd->GetShdwTris()) {
            maxVert = std::max<uint32>(maxVert, rng::max(tri.m_vertIndices));
        }
        return maxVert + 1;
    }

    [[nodiscard]] bool HasShadowMesh() const { return flags & 16; }
};
//static_assert(std::is_trivially_copyable_v<Header>);
VALIDATE_SIZE(Header, 88);
}; // namespace V3

namespace V4 {
using namespace V3; // Inherit all others stuff

// Header for V4
struct Header : V3::Header {
    uint32 unk{};
};
//static_assert(std::is_trivially_copyable_v<Header>);
VALIDATE_SIZE(Header, 92);
}; // namespace V4

}; // namespace ColHelpers
