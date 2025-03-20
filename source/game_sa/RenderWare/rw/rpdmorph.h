#ifndef RPDMORPH_H
#define RPDMORPH_H

#include <rwcore.h>
#include <rpworld.h>

#if ((defined(GCN_DRVMODEL_H) || \
      defined(XBOX_DRVMODEL_H)))
#include "dmorphplatformapi.h"
#endif

/*---- start: ./dmorph.h----*/
/**
 * \defgroup rpdmorph RpDMorph
 * \ingroup deltamorphing
 *
 * Delta Morphing Plugin for RenderWare Graphics.
 */
#ifdef     __cplusplus
extern "C"
{
#endif  /* __cplusplus */
extern RwBool RpDMorphPluginAttach( void );

#ifdef    __cplusplus
}
#endif /* __cplusplus */
/*---- end: ./dmorph.h----*/
/*---- start: ./dmphanim.h----*/
#define rpDMORPHNULLFRAME ((RwUInt32)~0)
/*--- rpDMorphFrame ---------------------------------------------------------
 *
 *  Animation frame.
 */
typedef struct rpDMorphFrame rpDMorphFrame;

#if (!defined(DOXYGEN))
struct rpDMorphFrame
{
    /* Ordering must be maintained here for stream compatibility */

    /*--- Interpolation data ---*/
    RwReal   startValue;
    RwReal   endValue;
    RwReal   duration;
    /*--- Utility data ---*/
    RwReal   recipDuration;
    /*--- Linking ---*/
    RwUInt32 nextFrame;
};
#endif /* (!defined(DOXYGEN)) */

/*--- rpDMorphFrameSequence -------------------------------------------------
 *
 *  A linked sequence of animation frames.
 */
typedef struct rpDMorphFrameSequence rpDMorphFrameSequence;

#if (!defined(DOXYGEN))
struct rpDMorphFrameSequence
{
    RwUInt32         numFrames;
    rpDMorphFrame    *frames;

};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpdmorph
 * \struct RpDMorphAnimation
 * Contains frame sequences for animating delta
 * morph target objects.
 * This should be considered an opaque type.
 * Use the RpDMorphAnimation API
 * functions to access.
 */
typedef struct RpDMorphAnimation RpDMorphAnimation;

/*--- RpDMorphAnimation -----------------------------------------------------
 *
 *  An array of animation sequences, one for each morph target.
 */
#if (!defined(DOXYGEN))
struct RpDMorphAnimation
{
    RwUInt32                numDMorphTargets;
    rpDMorphFrameSequence   *frameSequences;
};
#endif /* (!defined(DOXYGEN)) */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*===========================================================================*
 *--- Plugin API Functions --------------------------------------------------*
 *===========================================================================*/

/*--- Animation Functions --------------------------------------------------
 */
extern RpDMorphAnimation *
RpDMorphAnimationCreate(RwUInt32 numDMorphTargets);

extern RpDMorphAnimation *
RpDMorphAnimationDestroy(RpDMorphAnimation *anim);

extern RwUInt32
RpDMorphAnimationGetNumDMorphTargets(RpDMorphAnimation *animation);

/* Animation Frames */
extern RpDMorphAnimation *
RpDMorphAnimationCreateFrames(RpDMorphAnimation  *anim,
                              RwUInt32           dMorphTargetIndex,
                              RwUInt32           numFrames);

extern RpDMorphAnimation *
RpDMorphAnimationDestroyFrames(RpDMorphAnimation  *anim,
                               RwUInt32           dMorphTargetIndex);

extern RwUInt32
RpDMorphAnimationGetNumFrames(RpDMorphAnimation *animation,
                              RwUInt32          dMorphTargetIndex);

/* Stream I/O */
extern RpDMorphAnimation *
RpDMorphAnimationStreamRead(RwStream *stream);

extern RpDMorphAnimation *
RpDMorphAnimationStreamWrite(RpDMorphAnimation *animation,
                             RwStream *stream);

extern RwUInt32
RpDMorphAnimationStreamGetSize(RpDMorphAnimation *animation);

extern RpDMorphAnimation *
RpDMorphAnimationRead(const RwChar *filename);

extern RpDMorphAnimation *
RpDMorphAnimationWrite(RpDMorphAnimation *animation, const RwChar *filename);

/*--- Animation Frame Functions --------------------------------------------
 *
 * These functions work on the DMorphAnimationFrame level.
 * Each Frame can have a reference to the next Frame for the
 * DMorphTarget.
 */
extern RpDMorphAnimation *
RpDMorphAnimationFrameSetNext(RpDMorphAnimation *anim,
                              RwUInt32 dMorphTargetIndex,
                              RwUInt32 frameIndex,
                              RwUInt32 nextFrame );

extern RwUInt32
RpDMorphAnimationFrameGetNext(RpDMorphAnimation *anim,
                              RwUInt32 dMorphTargetIndex,
                              RwUInt32 frameIndex );

extern RpDMorphAnimation *
RpDMorphAnimationFrameSet(RpDMorphAnimation *anim,
                          RwUInt32   dMorphTargetIndex,
                          RwUInt32   frameIndex,
                          RwReal startValue,
                          RwReal endValue,
                          RwReal duration,
                          RwUInt32 nextFrame );

extern RpDMorphAnimation *
RpDMorphAnimationFrameSetStartValue(RpDMorphAnimation *anim,
                                    RwUInt32 dMorphTargetIndex,
                                    RwUInt32 frameIndex,
                                    RwReal   startValue );

extern RwReal
RpDMorphAnimationFrameGetStartValue(RpDMorphAnimation *anim,
                                    RwUInt32 dMorphTargetIndex,
                                    RwUInt32 frameIndex );

extern RpDMorphAnimation *
RpDMorphAnimationFrameSetEndValue(RpDMorphAnimation *anim,
                                  RwUInt32  dMorphTargetIndex,
                                  RwUInt32  frameIndex,
                                  RwReal    endValue );

extern RwReal
RpDMorphAnimationFrameGetEndValue(RpDMorphAnimation *anim,
                                  RwUInt32  dMorphTargetIndex,
                                  RwUInt32  frameIndex );

extern RpDMorphAnimation *
RpDMorphAnimationFrameSetDuration(RpDMorphAnimation *anim,
                                  RwUInt32  dMorphTargetIndex,
                                  RwUInt32  frameIndex,
                                  RwReal    duration );

extern RwReal
RpDMorphAnimationFrameGetDuration(RpDMorphAnimation *anim,
                                  RwUInt32 dMorphTargetIndex,
                                  RwUInt32 frameIndex );

#ifdef __cplusplus
}
#endif /* __cplusplus */
/*---- end: ./dmphanim.h----*/
/*---- start: ./dmphintp.h----*/
/*--- RpDMorphInterpolator --------------------------------------------------
 *
 *  Data holding current position in an animation.
 */
typedef struct rpDMorphInterpolator rpDMorphInterpolator;

#if (!defined(DOXYGEN))
struct rpDMorphInterpolator
{
    RpDMorphAnimation  *animation;
    RwReal              animTime;
    rpDMorphFrame     **currFrames;
    RwReal             *currTimes;
    RpAtomicCallBack    loopCallBack;
    void               *loopCallBackData;
};
#endif /* (!defined(DOXYGEN)) */

/*--- rpDMorphAtomicData ---------------------------------------------------
 *
 * This is an extension to RpAtomic.
 */
typedef struct rpDMorphAtomicData rpDMorphAtomicData;

#if (!defined(DOXYGEN))
struct rpDMorphAtomicData
{
    /*--- Morph state data ---*/
    RwReal                  *values;

    /*--- Animation interpolator ---*/
    rpDMorphInterpolator    *interpolator;

    /*--- Render callback chaining ---*/
    RpAtomicCallBackRender  defRenderCallBack;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpdmorph
 * \ref RpDMorphType defines the different ways a dmorph atomic can
 * be rendered. Once a dmorph \ref RpGeometry has been attached to
 * an \ref RpAtomic the atomic must be setup with the correct dmorph
 * rendering pipeline with \ref RpDMorphAtomicSetType.
 */
enum RpDMorphType
{
    rpNADMORPHTYPE        = 0, /**<Invalid DMorph pipeline.           */
    rpDMORPHTYPEGENERIC   = 1, /**<Generic DMorph rendering.          */
    rpDMORPHTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpDMorphType RpDMorphType;

/*===========================================================================*
 *--- Macros ----------------------------------------------------------------*
 *===========================================================================*/

/*--- Atomic data ---*/
#define RPDMORPHATOMICGETDATA(atomic)                               \
    ((rpDMorphAtomicData *)(((RwUInt8 *)atomic)+                      \
                          (rpDMorphAtomicDataOffset)))

#define RPDMORPHATOMICGETCONSTDATA(atomic)                          \
    ((const rpDMorphAtomicData *)(((const RwUInt8 *)atomic)+          \
                                (rpDMorphAtomicDataOffset)))
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*===========================================================================*
 *--- Global variables ------------------------------------------------------*
 *===========================================================================*/

extern RwInt32 rpDMorphAtomicDataOffset;

/*===========================================================================*
 *--- Plugin API Functions --------------------------------------------------*
 *===========================================================================*/

/*--- DMorphAtomic functions ------------------------------------------------
 *
 * These functions work at the DMorphAtomic level.
 */
extern RpAtomic *
RpDMorphAtomicInitialize( RpAtomic *atomic );

extern RpAtomic *
RpDMorphAtomicSetType(RpAtomic *atomic,
                      RpDMorphType type);
extern RpDMorphType
RpDMorphAtomicGetType(RpAtomic *atomic);

extern RwReal *
RpDMorphAtomicGetDMorphValues( RpAtomic *atomic );

extern RpAtomic *
RpDMorphAtomicSetAnimation( RpAtomic *atomic,
                            RpDMorphAnimation *animation );

extern RpDMorphAnimation *
RpDMorphAtomicGetAnimation( const RpAtomic *atomic );

extern RpAtomic *
RpDMorphAtomicAddTime( RpAtomic *atomic,
                       RwReal time );

extern RwReal
RpDMorphAtomicGetAnimTime( const RpAtomic *atomic );

extern RpAtomic *
RpDMorphAtomicSetAnimLoopCallBack( RpAtomic *atomic,
                                   RpAtomicCallBack callBack,
                                   void *data );

extern RpAtomicCallBack
RpDMorphAtomicGetAnimLoopCallBack( const RpAtomic *atomic,
                                   void **callBackData );

extern RpAtomic *
RpDMorphAtomicSetAnimFrame( RpAtomic *atomic,
                            RwUInt32 dMorphTargetIndex,
                            RwUInt32 index );

extern RwUInt32
RpDMorphAtomicGetAnimFrame( const RpAtomic *atomic,
                            RwUInt32 dMorphTargetIndex );


extern RpAtomic *
RpDMorphAtomicSetAnimFrameTime( RpAtomic *atomic,
                                RwUInt32 dMorphTargetIndex,
                                RwReal time );

extern RwReal
RpDMorphAtomicGetAnimFrameTime( const RpAtomic *atomic,
                                RwUInt32 dMorphTargetIndex );

#ifdef __cplusplus
}
#endif /* __cplusplus */
/*---- end: ./dmphintp.h----*/
/*---- start: ./dmphgeom.h----*/
/*--- rpDMorphRLE --------------------------------------------------------
 *
 * Contains the run length encoding of the delta data.
 */
typedef struct rpDMorphRLE rpDMorphRLE;

#if (!defined(DOXYGEN))
struct rpDMorphRLE
{
    RwUInt8            *code;
    RwUInt32            numCodeElements;
    RwUInt32            numDataElements;
    void               *data;
};
#endif /* (!defined(DOXYGEN)) */

/*--- _RpDMorphTarget -------------------------------------------------------*/

/**
 * \ingroup rpdmorph
 * \struct RpDMorphTarget
 * Delta morph target object for defining a target for
 * a base geometry.
 * This should be considered an opaque type.
 * Use the RpDMorphGeometry and RpDMorphTarget API
 * functions to access.
 */
typedef struct RpDMorphTarget RpDMorphTarget;

#if (!defined(DOXYGEN))
struct RpDMorphTarget
{
    /*--- DMorphTarget ---*/
    RpGeometry            *geometry;
    RwChar                *name;
    RwUInt32              flags;
    RwUInt32              lockFlags;
    rpDMorphRLE           rle;
    /*--- DMorphGeometry ---*/
    RwV3d                 *vertices;
    RwV3d                 *normals;
    RwRGBA                *preLightColors;
    RwTexCoords           *texCoords;
    /*--- Bounding sphere ---*/
    RwSphere              boundingSphere;
};
#endif /* (!defined(DOXYGEN)) */

/*--- rpDMorphGeometryData -------------------------------------------------
 *
 * This is an extension to RpGeometry. This defines the number of
 * DMorphTargets.
 */
typedef struct rpDMorphGeometryData rpDMorphGeometryData;

#if (!defined(DOXYGEN))
struct rpDMorphGeometryData
{
    /*--- DMorphTargets ---*/
    RwUInt32            numDMorphTargets;
    RpDMorphTarget      *dMorphTargets;

    /*--- Current morph state of the geometry ---*/
    RwReal              *currValues;

#if defined(PLATFORM_DATA)
    /*--- Render callback chaining ---*/
    DMorphPlatformData  platformData;
#endif /* defined(PLATFORM_DATA) */
};
#endif /* (!defined(DOXYGEN)) */

/*===========================================================================*
 *--- Macros ----------------------------------------------------------------*
 *===========================================================================*/

/*--- Geometry data ---*/
#define RPDMORPHGEOMETRYGETDATA(geometry)                               \
    ((rpDMorphGeometryData **)                                          \
     (((RwUInt8 *)geometry)+                                            \
      (rpDMorphGeometryDataOffset)))

#define RPDMORPHGEOMETRYGETCONSTDATA(geometry)                          \
    ((const rpDMorphGeometryData * const *)                             \
     (((const RwUInt8 *)geometry)+                                      \
      (rpDMorphGeometryDataOffset)))
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*===========================================================================*
 *--- Global variables ------------------------------------------------------*
 *===========================================================================*/

extern RwInt32 rpDMorphGeometryDataOffset;
/*===========================================================================*
 *--- Plugin API Functions --------------------------------------------------*
 *===========================================================================*/

/*--- DMorphGeometry functions ----------------------------------------------
 *
 * These functios work on the DMorphGeometry level.
 * Each DMorphGeometry has a list of DMorphTargets.
 */
extern RpGeometry *
RpDMorphGeometryCreateDMorphTargets( RpGeometry *geometry,
                                     RwUInt32 number );

extern RpGeometry *
RpDMorphGeometryDestroyDMorphTargets( RpGeometry *geometry );

extern RpGeometry *
RpDMorphGeometryAddDMorphTarget( RpGeometry  *geometry,
                                 RwUInt32    index,
                                 RwV3d       *vertices,
                                 RwV3d       *normals,
                                 RwRGBA      *preLightColors,
                                 RwTexCoords *texCoords,
                                 RwUInt32    flags );

extern RpGeometry *
RpDMorphGeometryRemoveDMorphTarget( RpGeometry *geometry,
                                    RwUInt32 index );

extern RpDMorphTarget *
RpDMorphGeometryGetDMorphTarget( const RpGeometry *geometry,
                                 RwUInt32 index );

extern RwUInt32
RpDMorphGeometryGetNumDMorphTargets( const RpGeometry *geometry );

extern RpGeometry *
RpDMorphGeometryTransformDMorphTargets( RpGeometry *geometry,
                                        const RwMatrix *matrix );

extern const RwSphere *
RpDMorphTargetGetBoundingSphere( const RpDMorphTarget *dMorphTarget );

extern RpDMorphTarget *
RpDMorphTargetSetName( RpDMorphTarget *dMorphTarget,
                       RwChar *name );

extern RwChar *
RpDMorphTargetGetName( RpDMorphTarget *dMorphTarget );

extern RpGeometryFlag
RpDMorphTargetGetFlags( RpDMorphTarget *dMorphTarget );

#ifdef __cplusplus
}
#endif /* __cplusplus */
/*---- end: ./dmphgeom.h----*/

#endif /* RPDMORPH_H */

