
/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h" // important

class PedClothesDesc;

class CClothesBuilder {
public:
    static void  InjectHooks();

    static void LoadCdDirectory();
    static void RequestGeometry(int32 modelId, uint32 crc);
    static int32 RequestTexture(uint32 crc);
    static void PreprocessClothesDesc(CPedClothesDesc& desc, bool a2);
    static void ReleaseGeometry(int32 numToRelease);
    static void AddWeightToBoneVertex(float(&out_weights)[8], uint8(&bone_vertex_indices)[8], float weight_to_add, RwUInt32 target_vertex);
    static void StoreBoneArray(RpClump* clump, int32 a2);

    /*!
    * Blend 3 geometries together and store the result in the first one
    *
    * @addr 0x5A4940
    *
    * @arg clump      The clump of the named frames [which in turn contain the geometries]
    * @arg frameName0 The 0th frame - This is where the result is stored to
    * @arg frameName1 The 1st frame
    * @arg frameName2 The 2nd frame
    * @arg r0         Blend ratio of the corresponding frame
    * @arg r1         Blend ratio of the corresponding frame
    * @arg r2         Blend ratio of the corresponding frame
    */
    static RpGeometry* BlendGeometry(RpClump* pClump, const char* frameName0, const char* frameName1, const char* frameName2, float r0, float r1, float r2);

    /*!
    * Blend 2 geometries together and store the result in the first one
    *
    * @addr 0x5A4F10
    *
    * @arg clump      The clump of the named frames [which in turn contain the geometries]
    * @arg frameName0 The 0th frame - This is where the result is stored to
    * @arg frameName1 The 1st frame
    * @arg r0         Blend ratio of the corresponding frame
    * @arg r1         Blend ratio of the corresponding frame
    */
    static RpGeometry* BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, float r0, float r1);

    static RpGeometry* CopyGeometry(RpClump* clump, const char* a2, const char* a3);
    static void ConstructGeometryArray(RpGeometry** ppGeometry, uint32* pModelKeys, float normal, float fatness, float strength);
    static void DestroySkinArrays(RwMatrixWeights* weights, RwUInt32* bones);
    static void BuildBoneIndexConversionTable(uint8* a1, RpHAnimHierarchy* a2, int32 a3);

    /*!
    * Create a copy of a texture
    * 
    * @addr 0x5A5730
    */
    static RwTexture* CopyTexture(RwTexture* texture);
    static void PlaceTextureOnTopOfTexture(RwTexture* texture1, RwTexture* texture2);

    /*!
    * Blend textures with a given blend ratio
    *
    * @addr 0x5A5820
    * 
    * @arg dst The texture to which the blended data is written to, and read from
    * @arg src The texture that should be blended with the other one
    * @arg r1 Ratio of the `dst` texture
    * @arg r2 Ratio of the `src` texture
    */
    static void BlendTextures(RwTexture* dst, RwTexture* src, float r1, float r2, int32);

    /*!
    * Blend textures with a given blend ratio
    *
    * @addr 0x5A59C0
    *
    * @arg dst The texture to which the blended data is written to, and read from
    * @arg src1 The texture that should be blended with the other one
    * @arg src2 The texture that should be blended with the other one
    * @arg r1 Ratio of the `dst` texture
    * @arg r2 Ratio of the `src1` texture
    * @arg r3 Ratio of the `src2` texture
    */
    static void BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32);

    /*!
    * Blend textures with a given blend ratio
    *
    * @addr 0x5A5BC0
    *
    * @arg dst The texture to which the blended data is written to, and read from
    * @arg src1 The texture that should be blended with the other one
    * @arg src2 The texture that should be blended with the other one
    * @arg r1 Ratio of the `dst` texture
    * @arg r2 Ratio of the `src1` texture
    * @arg r3 Ratio of the `src2` texture
    */
    static void BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32 numColors, RwTexture* tattoos);
    static void ConstructTextures(RwTexDictionary* dict, uint32* hashes, float factorA, float factorB, float factorC);
    static void ConstructGeometryAndSkinArrays(RpHAnimHierarchy* animHierarchy, RpGeometry** geometry1, RwMatrixWeights** weights, uint32** a4, uint32 a5, RpGeometry** geometry2, RpMaterial** material);
    static RpClump* CreateSkinnedClump(RpClump* clump, RwTexDictionary* dict, CPedClothesDesc& newClothes, const CPedClothesDesc* oldClothes, bool CutscenePlayer);
};

RpAtomic* GetAtomicWithName(RpClump* clump, const char* name);
RwTexture* GetTextureFromTxdAndLoadNextTxd(RwTexture* destTexture, int32 txdId_withTexture, int32 CRC_nextTxd, int32* nextTxdId);
