
#ifndef RPSKIN_H
#define RPSKIN_H

/**
 * \defgroup rpskin RpSkin
 * \ingroup skinning
 *
 * Skin Plugin for RenderWare Graphics.
 */

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/
#include "rwcore.h"
#include "rpworld.h"

#include "rpcriter.h"
////#include "rpskin.rpe"

#include "rphanim.h"

/*===========================================================================*
 *--- Global Types ----------------------------------------------------------*
 *===========================================================================*/
typedef struct RwMatrixWeights RwMatrixWeights;

/**
 * \ingroup rpskin
 * \struct RwMatrixWeights
 * A structure for defining up to four matrix weights per vertex.
 * Not all entries need to be used.
 *
 * \note
 * Values should be sorted, such that any zero 0.0f entries appear
 * after the valid weights. Any weights that appear after a zero
 * entry will be ignored.
 *
 * \see RpSkinCreate
 */
struct RwMatrixWeights
{
    RwReal w0; /**< The first matrix weight.  */
    RwReal w1; /**< The second matrix weight. */
    RwReal w2; /**< The third matrix weight.  */
    RwReal w3; /**< The fourth matrix weight. */
};

/**
 * \ingroup rpskin
 * \struct RpSkin
 *
 * Skin object. This should be considered an opaque type.
 * Use the RpSkin API functions to access.
 *
 * \see RpSkinCreate
 * \see RpSkinDestroy
 */
typedef struct RpSkin RpSkin;

/*===========================================================================*
 *--- Plugin API Functions --------------------------------------------------*
 *===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*---------------------------------------------------------------------------*
 *-   Plugin functions                                                      -*
 *---------------------------------------------------------------------------*/
extern void RpSkinSetFreeListCreateParams(
      RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

extern RwBool
RpSkinPluginAttach(void);

/*---------------------------------------------------------------------------*
 *-   Skin Atomic functions                                                 -*
 *---------------------------------------------------------------------------*/
extern RpAtomic *
RpSkinAtomicSetHAnimHierarchy( RpAtomic *atomic,
                               RpHAnimHierarchy *hierarchy );

extern RpHAnimHierarchy *
RpSkinAtomicGetHAnimHierarchy( const RpAtomic *atomic );

/*---------------------------------------------------------------------------*
 *-   Skin Geometry functions                                               -*
 *---------------------------------------------------------------------------*/
extern RpGeometry *
RpSkinGeometrySetSkin( RpGeometry *geometry,
                       RpSkin *skin );

extern RpSkin *
RpSkinGeometryGetSkin( RpGeometry *geometry );

extern RpSkin *
RpSkinCreate( RwUInt32 numVertices,
              RwUInt32 numBones,
              RwMatrixWeights *vertexWeights,
              RwUInt32 *vertexIndices,
              RwMatrix *inverseMatrices );

extern RpSkin *
RpSkinDestroy( RpSkin *skin );

extern RwUInt32
RpSkinGetNumBones( RpSkin *skin );

extern /*const*/ RwMatrixWeights*
RpSkinGetVertexBoneWeights( RpSkin *skin );

extern const RwUInt32 *
RpSkinGetVertexBoneIndices( RpSkin *skin );

extern const RwMatrix *
RpSkinGetSkinToBoneMatrices( RpSkin *skin );

extern RwBool
RpSkinIsSplit( RpSkin *skin );

/*---------------------------------------------------------------------------*
 *-   Skin pipeline                                                         -*
 *---------------------------------------------------------------------------*/

/**
 * \ingroup rpskin
 * \ref RpSkinType defines the different ways a skinned atomic can
 * be rendered. Once a skinned \ref RpGeometry has been attached to
 * an \ref RpAtomic the atomic must be setup with the correct skin
 * rendering pipeline with \ref RpSkinAtomicSetType.
 */
enum RpSkinType
{
    rpNASKINTYPE        = 0, /**<Invalid skin pipeline.           */
    rpSKINTYPEGENERIC   = 1, /**<Generic skin rendering.          */
    rpSKINTYPEMATFX     = 2, /**<Material effects skin rendering. */
    rpSKINTYPETOON      = 3, /**<Toon skin rendering.   To use this 
                               * flag the \ref rptoon plugin (part 
                               * of the FX Pack) needs to be 
                               * installed.          */
    rpSKINTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpSkinType RpSkinType;

extern RpAtomic *
RpSkinAtomicSetType( RpAtomic *atomic,
                     RpSkinType type );

extern RpSkinType
RpSkinAtomicGetType( RpAtomic *atomic );

/*---------------------------------------------------------------------------*
 *-   Internal API                                                          -*
 *---------------------------------------------------------------------------*/
extern RpGeometry *
_rpSkinInitialize(RpGeometry *geometry);

extern RpGeometry *
_rpSkinDeinitialize(RpGeometry *geometry);

extern RwUInt8 *
_rpSkinGetMeshBoneRemapIndices( RpSkin *skin );

extern RwUInt8 *
_rpSkinGetMeshBoneRLECount( RpSkin *skin );

extern RwUInt8 *
_rpSkinGetMeshBoneRLE( RpSkin *skin );

extern RpSkin *
_rpSkinSplitDataCreate( RpSkin *skin, RwUInt32 boneLimit,
                        RwUInt32 numMatrices, RwUInt32 numMeshes,
                        RwUInt32 numRLE );

extern RwBool
_rpSkinSplitDataDestroy( RpSkin *skin );

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*---------------------------------------------------------------------------*
 *-   Backwards macros                                                      -*
 *---------------------------------------------------------------------------*/

#define RpSkinAtomicGetSkin(_a)                                         \
    RpSkinGeometryGetSkin(RpAtomicGetGeometry(_a))

/*---------------------------------------------------------------------------*/

/*---- start: ./d3d9/skinplatform.h----*/

/**
 * \defgroup rpskind3d9 D3D9
 * \ingroup rpskin
 *
 * D3D9 skin pipeline extension.
 */

/**
 * \defgroup rpskind3d9features Features
 * \ingroup rpskind3d9
 *
 * D3D9 skin pipeline features.
 */

/**
 * \defgroup rpskind3d9restrictions Restrictions
 * \ingroup rpskind3d9
 *
 * D3D9 skin pipeline restrictions.
 */


/*===========================================================================*
 *--- D3D9 Defines -----------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- D3D9 Global Types ------------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rpskind3d9
 * \ref RpSkinD3D9Pipeline rendering pipelines available within
 * the \ref RpSkin plugin. Use \ref RpSkinGetD3D9Pipeline to
 * retrieve the \ref RxPipeline's.
 */
enum RpSkinD3D9Pipeline
{
    rpNASKIND3D9PIPELINE              = 0,
    rpSKIND3D9PIPELINEGENERIC         = 1,
    /**<D3D9 generic skin rendering pipeline.                           */
    rpSKIND3D9PIPELINEMATFX           = 2,
    /**<D3D9 material effect skin rendering pipeline.                   */
    rpSKIND3D9PIPELINETOON            = 3,
    /**<D3D9 toon skin rendering pipeline.                              */
    rpSKIND3D9PIPELINEMATFXTOON       = 4,
    /**<D3D9 toon matfx skin rendering pipeline not supported           */
    rpSKIND3D9PIPELINEMAX,
    rpSKIND3D9PIPELINEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpSkinD3D9Pipeline RpSkinD3D9Pipeline;

/*===========================================================================*
 *--- D3D9 Plugin API Functions ----------------------------------------------*
 *===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*---------------------------------------------------------------------------*/

extern RxPipeline *
RpSkinGetD3D9Pipeline( RpSkinD3D9Pipeline D3D9Pipeline );

extern RxNodeDefinition *
RxNodeDefinitionGetD3D9SkinAtomicAllInOne(void);

/* Vertex shader pipeline */
extern void
_rxD3D9SkinVertexShaderSetBeginCallBack(RxPipelineNode *node,
                                _rxD3D9VertexShaderBeginCallBack beginCallback);
extern void
_rxD3D9SkinVertexShaderSetLightingCallBack(RxPipelineNode *node,
                                _rxD3D9VertexShaderLightingCallBack lightingCallback);
extern void
_rxD3D9SkinVertexShaderSetGetMaterialShaderCallBack(RxPipelineNode *node,
                                _rxD3D9VertexShaderGetMaterialShaderCallBack getmaterialshaderCallback);
extern void
_rxD3D9SkinVertexShaderSetMeshRenderCallBack(RxPipelineNode *node,
                                _rxD3D9VertexShaderMeshRenderCallBack meshRenderCallback);
extern void
_rxD3D9SkinVertexShaderSetEndCallBack(RxPipelineNode *node,
                                _rxD3D9VertexShaderEndCallBack endCallback);


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./d3d9/skinplatform.h----*/

#endif /* RPSKIN_H */


