/******************************************
 *                                        *
 *    RenderWare(TM) Graphics Library     *
 *                                        *
 ******************************************/

/*
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1998. Criterion Software Ltd.
 * All Rights Reserved.
 */

/***************************************************************************
 *                                                                         *
 * Module  : rtcmpkey.h                                                    *
 *                                                                         *
 * Purpose : Hierarchical animation compressed keyframe system             *
 *                                                                         *
 **************************************************************************/

#ifndef RTCMPKEY_H
#define RTCMPKEY_H


/****************************************************************************
 Includes
 */

#include <rwcore.h>
#include <rpworld.h>

#include <rtanim.h>
#include <rtquat.h>

/**
 * \defgroup rtcmpkey RtCmpKey
 * \ingroup animtools
 *
 * Keyframe system supporting compressed matrix animation
 */

#define rtANIMCOMPRESSEDKEYFRAMETYPEID 0x2

/*
 * typedef for struct RtCompressedKeyFrame. 
 */
typedef struct RtCompressedKeyFrame RtCompressedKeyFrame;

/**
 * \ingroup rtcmpkey
 * \struct RtCompressedKeyFrame
 * A structure representing data for a compressed keyframe. Sequences of
 * such keyframes in an \ref RtAnimAnimation defines the animation of each
 * node in a hierarchy.
 *
 * Based on \ref RtAnimKeyFrameHeader.
 */
struct RtCompressedKeyFrame
{
    RtCompressedKeyFrame *prevFrame;    /**< Pointer to the previous keyframe */
    RwReal              time;           /**< Time at keyframe */
    RwUInt16            qx;             /**< Compressed rotation quat.imag.x at keyframe */
    RwUInt16            qy;             /**< Compressed rotation quat.imag.y at keyframe */
    RwUInt16            qz;             /**< Compressed rotation quat.imag.z at keyframe */
    RwUInt16            qr;             /**< Compressed rotation quat.real at keyframe */
    RwUInt16            tx;             /**< Compressed translation x at keyframe */
    RwUInt16            ty;             /**< Compressed translation y at keyframe */
    RwUInt16            tz;             /**< Compressed translation z at keyframe */
};

/**
 * \ingroup rtcmpkey
 * \ref RtCompressedInterpKeyFrame
 * typedef for RtCompressedInterpKeyFrame. Matches \ref RpHAnimInterpFrame.
 */
typedef RpHAnimInterpFrame RtCompressedInterpKeyFrame;

/*
 * typedef for struct RtCompressedKeyFrameCustomData.
 */
typedef struct RtCompressedKeyFrameCustomData RtCompressedKeyFrameCustomData;

/**
 * \ingroup rtcmpkey
 * \struct RtCompressedKeyFrameCustomData
 * A structure representing additional custom data for a compressed animation.
 * This data represents offsets and scalars to decompress translation values.
 * 
 */
struct RtCompressedKeyFrameCustomData
{
    RwV3d   offset; /**< Offset to center all translation values around 0,0 */
    RwV3d   scalar; /**< Scalar to normalize translation values */
    /* Note: This structure is used for streaming; please take care while
             modifying */
};

#if (!defined(DOXYGEN))

/* Internal use only */
typedef union RtCompressedKeyFloatIntTag
{
    float f;
    int   i;
} RtCompressedKeyFloatInt;

#endif /* (!defined(DOXYGEN)) */

#define RtCompressedKeyFrameDecompressFloat(I, O)     \
do                                                    \
{                                                     \
    RtCompressedKeyFloatInt tmp;                      \
    tmp.i = (int)((I) & 0x8000)<<16;                  \
    if ((I) & 0x7fff)                                 \
    {                                                 \
        tmp.i |= ((int)((I) & 0x7800)<<12) + 0x38000000; \
        tmp.i |= (int)((I) & 0x07ff)<<12;             \
    }                                                 \
    (O) = tmp.f;                                      \
}                                                     \
while(0)

/*--- Plugin API Functions ---*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwUInt16
RtCompressedKeyFrameCompressFloat(RwReal I);

extern void
RtCompressedKeyFrameApply(void *matrix, void *voidIFrame);

extern void
RtCompressedKeyFrameInterpolate(void *voidOut,
                                void *voidIn1,
                                void *voidIn2,
                                RwReal time,
                                void *customData);

extern void
RtCompressedKeyFrameBlend(void *voidOut,
                          void *voidIn1,
                          void *voidIn2,
                          RwReal alpha);

extern RtAnimAnimation *
RtCompressedKeyFrameStreamRead(RwStream *stream,
                               RtAnimAnimation *animation);

extern RwBool
RtCompressedKeyFrameStreamWrite(const RtAnimAnimation *animation,
                                RwStream *stream);

extern RwInt32
RtCompressedKeyFrameStreamGetSize(const RtAnimAnimation *animation);

extern void
RtCompressedKeyFrameMulRecip(void *voidFrame, void *voidStart);

extern void
RtCompressedKeyFrameAdd(void *voidOut,
                        void *voidIn1,
                        void *voidIn2);

extern RwBool
RtCompressedKeyFrameRegister(void);

extern RtAnimAnimation *
RtCompressedKeyConvertFromStdKey(RtAnimAnimation *stdkeyanim);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTCMPKEY_H */
