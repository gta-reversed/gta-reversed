/*****************************************************************************
 *
 * File :     rpuvanim.h
 *
 * Abstract : UV animation
 *
 *****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd. or
 * Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. will not, under any
 * circumstances, be liable for any lost revenue or other damages arising
 * from the use of this file.
 *
 * Copyright (c) 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 * RenderWare is a trademark of Canon Inc.
 *
 *****************************************************************************/

#ifndef RPUVANIM_H
#define RPUVANIM_H

/**
 * \defgroup rpuvanim RpUVAnim
 * \ingroup animtools
 *
 * UV Animation Plugin for RenderWare Graphics.
 */

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/
#include <rwcore.h>
#include <rpworld.h>
#include <rtanim.h>
#include <rtdict.h>

#include <rpcriter.h>
//#include "rpuvanim.rpe" /* automatically generated header file */

/*===========================================================================*
 *--- Global Types ----------------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rpuvanim
 * \def RP_UVANIM_MAXSLOTS is the maximum number of separate UV animations
 * that can be applied to a single material
 */
#define RP_UVANIM_MAXSLOTS (8)
/**
 * \ingroup rpuvanim
 * \def RP_UVANIM_APPLIEDCHANNELS is the number of UV channels that will be
 * updated via the default implementation of \ref RpMaterialUVAnimApplyUpdate 
 */
#define RP_UVANIM_APPLIEDCHANNELS (2)

#define RP_UVANIM_FIRST_PASS_CHANNEL (0)
#define RP_UVANIM_DUAL_PASS_CHANNEL (1)
/**
 * \ingroup rpuvanim
 * \def RP_UVANIM_MAXNAME is the maximum number of chars stored in a UV
 * animation name, including the terminating NULL
 */
#define RP_UVANIM_MAXNAME (32)

/*===========================================================================*
 *--- Global variables ------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Plugin API Functions --------------------------------------------------*
 *===========================================================================*/


/*--- UVAnimPlugin functions ------------------------------------------------*/

/****************************************************************************
 Includes
 */

#if (!defined(DOXYGEN))
typedef struct RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobalVars;

struct RpUVAnimMaterialGlobalVars
{
    RwInt32   engineOffset;
};

typedef struct _rpUVAnimCustomData _rpUVAnimCustomData;

struct _rpUVAnimCustomData   /* Warning - streaming depends on layout */
{
    RwChar name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwUInt32 refCount;
};

extern _rpUVAnimCustomData *
_rpUVAnimCustomDataStreamRead(RwStream *stream);

extern const _rpUVAnimCustomData *
_rpUVAnimCustomDataStreamWrite(const _rpUVAnimCustomData *customData,
                               RwStream *stream);

extern RwUInt32
_rpUVAnimCustomDataStreamGetSize(const _rpUVAnimCustomData *customData);
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpuvanim
 * \ref RpUVAnim
 * typedef for struct RpUVAnim
 */
typedef RtAnimAnimation RpUVAnim;

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimInterpolator
 * typedef for struct RpUVAnimInterpolator
 */
typedef RtAnimInterpolator RpUVAnimInterpolator;

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimKeyFrameType
 * enum delineating the keyframe schemes that may be used
 */
enum RpUVAnimKeyFrameType
{
    rpUVANIMLINEARKEYFRAMES = 0, /**<Linearly interpolated keyframes       */
    rpUVANIMPARAMKEYFRAMES  = 1, /**<Parametrically interpolated keyframes;
                                  **<better for rotations */
    rpUVANIMFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimKeyFrameType
 * typedef for enum RpUVAnimKeyFrameType
 */
typedef enum RpUVAnimKeyFrameType RpUVAnimKeyFrameType;

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimLinearKeyFrameData
 * typedef for struct RpUVAnimLinearKeyFrameData
 */
typedef struct RpUVAnimLinearKeyFrameData RpUVAnimLinearKeyFrameData;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimLinearKeyFrameData
 * A structure representing linear keyframe data.
 */
struct RpUVAnimLinearKeyFrameData
{
    RwReal              uv[6];
                                    /**< UV matrix at keyframe  */
                                    /**<   (matrix) | uv[0] uv[1] - | (right) */
                                    /**<            | uv[2] uv[3] - | (up) */
                                    /**<            |  -     -    - | (at) */
                                    /**<            | uv[4] uv[5] - | (pos) */
                                    /**<The transformed UV texture coordinates will be: */ 
                                    /**<   u' = rx * u + ux * v + px */
                                    /**<   v' = ry * u + uy * v + py */
};

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimParamKeyFrameData
 * typedef for struct RpUVAnimParamKeyFrameData
 */
typedef struct RpUVAnimParamKeyFrameData RpUVAnimParamKeyFrameData;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimParamKeyFrameData
 * A structure representing paramaterized keyframe data.
 */
struct RpUVAnimParamKeyFrameData
{
    RwReal              theta;  /**< Angle of rotation at keyframe  */
    RwReal              s0;     /**< x-scaling at keyframe  */
    RwReal              s1;     /**< y-scaling at keyframe  */
    RwReal              skew;   /**< skew at keyframe       */
    RwReal              x;      /**< x-position at keyframe */
    RwReal              y;      /**< y-position at keyframe */
};

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimKeyFrameData
 * typedef for union RpUVAnimKeyFrameData
 */
typedef union RpUVAnimKeyFrameData RpUVAnimKeyFrameData;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimKeyFrameData
 * A union representing linear or paramaterized keyframe data.
 */
union RpUVAnimKeyFrameData
{
    RpUVAnimLinearKeyFrameData linear; /**< Linear format keyframe */
    RpUVAnimParamKeyFrameData param;   /**< Parameterized format keyframe */
};

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimKeyFrame
 * typedef for struct RpUVAnimKeyFrame. Based on \ref RtAnimKeyFrameHeader.
 */
typedef struct RpUVAnimKeyFrame RpUVAnimKeyFrame;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimKeyFrame
 * A structure representing the standard keyframe data. Sequences of
 * such keyframes in an \ref RtAnimAnimation define the entire UV animation.
 */
struct RpUVAnimKeyFrame
{
    RpUVAnimKeyFrame     *prevFrame; /**< Pointer to the previous keyframe */
    RwReal               time;       /**< Time at keyframe */
    RpUVAnimKeyFrameData data;       /**< Linear or parameterized keyframe data */
};

/**
 * \ingroup rpuvanim
 * \ref RpUVAnimInterpFrame
 * typedef for struct RpUVAnimInterpFrame. Based on \ref RtAnimInterpFrameHeader.
 */
typedef struct RpUVAnimInterpFrame RpUVAnimInterpFrame;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimInterpFrame
 * A structure representing an interpolated keyframe. The initial part of the
 * structure matches \ref RtAnimInterpFrameHeader.
 */
struct RpUVAnimInterpFrame
{
    RpUVAnimKeyFrame    *keyFrame1;
        /**< Pointer to 1st keyframe of current interpolation pair */
    RpUVAnimKeyFrame    *keyFrame2;
        /**< Pointer to 2nd keyframe of current interpolation pair */
    RpUVAnimKeyFrameData data;  /**< Interpolated keyframe data in a linear
                                  * or parameterized format */
};


/**
 * \ingroup rpuvanim
 * \ref RpUVAnimMaterialExtension typedef for struct RpUVAnimMaterialExtension
 */

typedef struct RpUVAnimMaterialExtension RpUVAnimMaterialExtension;

/**
 * \ingroup rpuvanim
 * \struct RpUVAnimMaterialExtension
 *
 * Used to extend \ref RpMaterial objects, and thus
 * allow the mapping between UV animation and \ref RpMaterial.
 *
 */
struct RpUVAnimMaterialExtension
{
    RwMatrix *uv[RP_UVANIM_APPLIEDCHANNELS];
                /**< Matrices used by the default implementation of */
                /**< \ref RpMaterialUVAnimApplyUpdates to store the updated */
                /**< matrices for the single and dual pass channels */
    RtAnimInterpolator *interp[RP_UVANIM_MAXSLOTS];
                /**< Animations on each UV range. Note that the default  */
                /**< animation functions apply animations to the first   */
                /**< pass and dual pass UV transformations only */ 
};

/*--- Plugin API Functions ---*/

#define RpUVAnimLinearKeyFrameToMatrixMacro(_matrix,_voidIFrame)        \
MACRO_START                                                             \
{                                                                       \
    RpUVAnimInterpFrame *iFrame = (RpUVAnimInterpFrame *)(_voidIFrame); \
    RpUVAnimLinearKeyFrameData *lkf = &iFrame->data.linear;             \
                                                                        \
    (_matrix)->right.x = lkf->uv[0];                                    \
    (_matrix)->right.y = lkf->uv[1];                                    \
    (_matrix)->right.z = 0.0f;                                          \
    (_matrix)->up.x    = lkf->uv[2];                                    \
    (_matrix)->up.y    = lkf->uv[3];                                    \
    (_matrix)->up.z    = 0.0f;                                          \
    (_matrix)->at.x    = 0.0f;                                          \
    (_matrix)->at.y    = 0.0f;                                          \
    (_matrix)->at.z    = 0.0f;                                          \
    (_matrix)->pos.x   = lkf->uv[4];                                    \
    (_matrix)->pos.y   = lkf->uv[5];                                    \
    (_matrix)->pos.z   = 0.0f;                                          \
    RwMatrixUpdate(_matrix);                                            \
}                                                                       \
MACRO_STOP

#define RpUVAnimParamKeyFrameToMatrixMacro(_matrix,_voidIFrame, _Pivot, _InvPivot) \
MACRO_START                                                             \
{                                                                       \
    /* RwV3d _InvPivot; */                                              \
    RpUVAnimParamKeyFrameData *param                                    \
       = &((RpUVAnimKeyFrame *)(_voidIFrame))->data.param;              \
                                                                        \
    RwV3d zAxis = {0.0f, 0.0f, 1.0f};                                   \
                                                                        \
    (_matrix)->right.x = param->s0;                          \
    (_matrix)->right.y = param->skew; \
    (_matrix)->right.z = 0.0f;                                          \
    (_matrix)->up.x    = 0.0f;                          \
    (_matrix)->up.y    = param->s1; \
    (_matrix)->up.z    = 0.0f;                                          \
    (_matrix)->at.x    = 0.0f;                                          \
    (_matrix)->at.y    = 0.0f;                                          \
    (_matrix)->at.z    = 0.0f;                                          \
    (_matrix)->pos.x   = param->x;                          \
    (_matrix)->pos.y   = param->y;                           \
    (_matrix)->pos.z   = 0.0f;                                          \
    RwMatrixUpdate((_matrix));                                          \
    RwMatrixTranslate((_matrix), _Pivot, rwCOMBINEPOSTCONCAT);  \
    RwMatrixRotate((_matrix), &zAxis, param->theta/rwPI*180.0f, rwCOMBINEPOSTCONCAT); \
    RwMatrixTranslate((_matrix), _InvPivot, rwCOMBINEPOSTCONCAT);    \
}                                                                       \
MACRO_STOP

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobals;

/* Module management */
void
RpUVAnimSetFreeListCreateParams(RwInt32 blockSize,
                                RwInt32 numBlocksToPrealloc);

/* Plugin support */

extern RwBool
RpUVAnimPluginAttach(void);

/* uvanim interpolator info */
extern RtAnimInterpolatorInfo _rpUVAnimLinearInterpolatorInfo;
extern RtAnimInterpolatorInfo _rpUVAnimParamInterpolatorInfo;

/* uvanim keyframe functions */

extern void
RpUVAnimLinearKeyFrameApply(void *matrix,
                      void *voidIFrame);

extern void
RpUVAnimLinearKeyFrameBlend(void *voidOut,
                        void *voidIn1,
                        void *voidIn2,
                        RwReal alpha);

extern void
RpUVAnimLinearKeyFrameInterpolate(void *voidOut,
                           void *voidIn1,
                           void *voidIn2,
                           RwReal time,
                           void *customData);

extern void
RpUVAnimLinearKeyFrameAdd(void *voidOut,
                      void *voidIn1,
                      void *voidIn2);

extern void
RpUVAnimLinearKeyFrameMulRecip(void *voidFrame,
                           void *voidStart);


extern void
RpUVAnimParamKeyFrameApply(void *matrix,
                      void *voidIFrame);

extern void
RpUVAnimParamKeyFrameBlend(void *voidOut,
                        void *voidIn1,
                        void *voidIn2,
                        RwReal alpha);

extern void
RpUVAnimParamKeyFrameInterpolate(void *voidOut,
                           void *voidIn1,
                           void *voidIn2,
                           RwReal time,
                           void *customData);

extern void
RpUVAnimParamKeyFrameAdd(void *voidOut,
                      void *voidIn1,
                      void *voidIn2);

extern void
RpUVAnimParamKeyFrameMulRecip(void *voidFrame,
                           void *voidStart);

extern RtAnimAnimation *
RpUVAnimKeyFrameStreamRead(RwStream *stream,
                             RtAnimAnimation *animation);

extern RwBool
RpUVAnimKeyFrameStreamWrite(const RtAnimAnimation *animation,
                              RwStream *stream);

extern RwInt32
RpUVAnimKeyFrameStreamGetSize(const RtAnimAnimation *animation);

extern RpUVAnimLinearKeyFrameData *
RpUVAnimLinearKeyFrameDataInitFromMatrix(
                              RpUVAnimLinearKeyFrameData *data,
                              const RwMatrix *matrix);

extern RpUVAnimParamKeyFrameData *
RpUVAnimParamKeyFrameDataInitFromMatrix(
                              RpUVAnimParamKeyFrameData *data,
                              const RwMatrix *matrix);

extern RpUVAnimKeyFrame *
RpUVAnimKeyFrameInit(const RtAnimAnimation *animation,
                    RpUVAnimKeyFrame *keyFrame,
                    RpUVAnimKeyFrame *prevFrame,
                    RwReal time,
                    const RwMatrix *matrix);

/*
 * Schema Declaration
 */
RTDICTSCHEMADECLARE(_rpUVAnimDictSchema)

#if defined(DOXYGEN)
/**
 * \ingroup rpuvanim
 * \ref RpUVAnimGetDictSchema
 * returns the dictionary schema used for dictionaries of UV animations
 *
 * \return The UV animation dictionary schema
 * \see RtDictSchemaCreateDict
 */
extern RtDictSchema *
RpUVAnimGetDictSchema();
#else
#define RpUVAnimGetDictSchema()\
        (&_rpUVAnimDictSchema)
#endif

/* General API */
extern RpUVAnim *
RpUVAnimCreate(const RwChar *name, RwUInt32 numNodes, RwUInt32 numFrames,
               RwReal duration, RwUInt32 *nodeIndexToUVChannelMap,
               RpUVAnimKeyFrameType keyframeType);

extern RwBool
RpUVAnimDestroy(RpUVAnim *anim);

extern RpUVAnim *
RpUVAnimAddRef(RpUVAnim *anim);

extern const RwChar *
RpUVAnimGetName(const RpUVAnim *anim);

#ifdef RWDEBUG
extern const RpUVAnim *
_rpUVAnimDump(const RpUVAnim *anim);
#endif

#define RpUVAnimStreamGetSize(anim)\
            RtAnimAnimationStreamGetSize((anim))

#define RpUVAnimStreamRead(stream)\
            RtAnimAnimationStreamRead((stream))

#define RpUVAnimStreamWrite(anim, stream)\
            RtAnimAnimationStreamWrite((anim), (stream))

#define RpUVAnimGetUVNodeToChannelMap(anim)\
            (((_rpUVAnimCustomData *)(anim)->customData)->nodeToUVChannelMap)

extern RpMaterial *
RpMaterialSetUVAnim(RpMaterial *material, RpUVAnim *anim, RwUInt32 slot);

extern RpUVAnimInterpolator *
RpMaterialUVAnimGetInterpolator(RpMaterial *material, RwUInt32 slot);

extern RpMaterial *
RpMaterialUVAnimSetInterpolator(RpMaterial *material, RpUVAnimInterpolator *interp, RwUInt32 slot);

extern RpMaterial *
RpMaterialUVAnimSetCurrentTime(RpMaterial *material, RwReal time);

extern RpMaterial *
RpMaterialUVAnimAddAnimTime(RpMaterial *material, RwReal deltaTime);

extern RpMaterial *
RpMaterialUVAnimSubAnimTime(RpMaterial *material, RwReal deltaTime);

extern RwBool
RpMaterialUVAnimExists(const RpMaterial *material);

RpMaterial *
RpMaterialUVAnimApplyUpdate(RpMaterial *material);

extern const RwV3d rpUVAnimPivot;
extern const RwV3d rpUVAnimInvPivot;

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* RPUVANIM_H */

