/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   rpnormmap.h                                                -*
 *-                                                                         -*
 *-  Purpose :   Normal Maps plugin public/internal API                     -*
 *-                                                                         -*
 *===========================================================================*/

#ifndef RPNORMMAP_H
#define RPNORMMAP_H

#include "rwcore.h"
#include "rpworld.h"

/* RWPUBLIC */

/**
 * \defgroup rpnormmap RpNormMap
 * \ingroup lighting
 *
 * Normal Maps Plugin for RenderWare Graphics.
 */

/**
 * \ingroup rpnormmap
 * \ref RpNormMapAtomicPipeline rendering pipelines available within the
 * \ref rpnormmap plugin. Use \ref RpNormMapGetAtomicPipeline to retrieve
 * the required \ref RxPipeline.
 */
enum RpNormMapAtomicPipeline
{
    rpNANORMMAPATOMICPIPELINE          = 0,
    rpNORMMAPATOMICSTATICPIPELINE      = 1, /**<Static atomics normal map rendering pipeline. */
    rpNORMMAPATOMICSKINNEDPIPELINE     = 2, /**<Skinned atomics normal map rendering pipeline. */
    rpNORMMAPATOMICPIPELINEMAX,
    rpNORMMAPATOMICPIPELINEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpNormMapAtomicPipeline RpNormMapAtomicPipeline;

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool
RpNormMapPluginAttach(void);

/**
 * \ingroup rpnormmap
 * \ref RpNormMapAtomicInitialize initializes the required normal map
 * rendering pipeline on an atomic.
 *
 * The world and normal maps plugins must be attached before using
 * this function. The header file rpnormmap.h is required.
 *
 * \param atomic Pointer to the \ref RpAtomic to initialize the normal
 *                    maps pipeline on.
 * \param pipeline Atomic rendering type.
 *
 * \see RpNormMapPluginAttach
 * \see RpNormMapAtomicIsInitialized
 * \see RpNormMapWorldEnable
 * \see RpNormMapWorldSectorInitialize
 * \see RpNormMapMaterialSetNormMapTexture
 */
extern void
RpNormMapAtomicInitialize(RpAtomic *atomic, RpNormMapAtomicPipeline pipeline);

extern RwBool
RpNormMapAtomicIsInitialized(const RpAtomic *atomic);

/**
 * \ingroup rpnormmap
 * \ref RpNormMapWorldEnable is used to enable the normal maps
 * object pipeline on a world.
 *
 * The world and normal maps plugins must be attached before using
 * this function. The header file rpnormmap.h is required.
 *
 * This function must be used \e before the \ref RpWorld is
 * rendered or pre-instanced.
 *
 * \param world Pointer to the world to enable the normal maps pipeline on.
 *
 * \return True if the normal maps plugin has been enabled on the world,
 * false otherwise.
 *
 * \see RpNormMapPluginAttach
 * \see RpNormMapAtomicInitialize
 * \see RpNormMapAtomicIsInitialized
 * \see RpNormMapWorldSectorInitialize
 * \see RpNormMapWorldIsEnabled
 * \see RpNormMapMaterialSetNormMapTexture
 */
extern void
RpNormMapWorldEnable(RpWorld *world);

extern void
RpNormMapWorldSectorInitialize(RpWorldSector *worldsector);

extern RwBool
RpNormMapWorldSectorIsInitialized(const RpWorldSector *worldsector);

extern RwBool
RpNormMapWorldIsEnabled(const RpWorld *world);

/**
 * \ingroup rpnormmap
 * \ref RpNormMapGetAtomicPipeline returns one of the \ref rpnormmap internal
 * atomic pipelines.
 *
 * The \ref rpnormmap plugin must be attached before using this function.
 *
 * \param pipeline Type of the requested pipeline.
 *
 * \return The \ref RxPipeline requested from the plugin, or
 * NULL if the pipeline wasn't constructed.
 *
 * \see RpNormMapPluginAttach
 * \see RpNormMapAtomicInitialize
 * \see RpNormMapWorldIsEnabled
 * \see RpNormMapGetWorldSectorPipeline
 */
extern RxPipeline *
RpNormMapGetAtomicPipeline(RpNormMapAtomicPipeline pipeline);

extern RxPipeline *
RpNormMapGetWorldSectorPipeline(void);

extern RpMaterial *
RpNormMapMaterialSetNormMapTexture(RpMaterial *material, RwTexture *normalmap);

extern RwTexture *
RpNormMapMaterialGetNormMapTexture(const RpMaterial *material);

extern RpMaterial *
RpNormMapMaterialSetEnvMapTexture(RpMaterial *material, RwTexture *envmap);

extern RwTexture *
RpNormMapMaterialGetEnvMapTexture(const RpMaterial *material);

extern RpMaterial *
RpNormMapMaterialSetEnvMapCoefficient(RpMaterial *material, RwReal coef);

extern RwReal
RpNormMapMaterialGetEnvMapCoefficient(const RpMaterial *material);

extern RpMaterial *
RpNormMapMaterialSetEnvMapFrame(RpMaterial *material, RwFrame *frame);

extern RwFrame *
RpNormMapMaterialGetEnvMapFrame(RpMaterial *material);

extern RpMaterial *
RpNormMapMaterialModulateEnvMap(RpMaterial *material, RwBool modulate);

extern RwBool
RpNormMapMaterialIsEnvMapModulated(const RpMaterial *material);

extern void
RpNormMapSetActiveLights(RpLight *mainLight, RpLight *ambientLight);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */

#endif /* RPNORMMAP_H */
