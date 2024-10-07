/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
/******************************************/
/*                                        */
/*    RenderWare(TM) Graphics Library     */
/*                                        */
/******************************************/

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
 * Copyright (c) 1999. Criterion Software Ltd.
 * All Rights Reserved.
 */

/*************************************************************************
 *
 * Filename: <C:/daily/rwsdk/include/d3d9/rwplcore.h>
 * Automatically Generated on: Fri Oct 03 09:52:43 2003
 *
 ************************************************************************/

/*--- System Header Files ---*/
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


/*--- Automatically derived from: C:/daily/rwsdk/os/win/ostypes.h ---*/
#ifndef WIN_OSTYPES_H
#define WIN_OSTYPES_H

#define rwLITTLEENDIAN         /* This is a little endian machine */

typedef long RwFixed;
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef signed char RwInt8;

#ifdef RWUNICODE
typedef wchar_t RwChar;
#else /* RWUNICODE */
typedef char RwChar;
#endif /* RWUNICODE */
typedef float RwReal;
typedef RwInt32 RwBool;

#if defined(_MSC_VER)
typedef long long RwInt64;
typedef unsigned long long RwUInt64;
#define RWZERO64 ((RwUInt64)0)
#elif defined(__GNUC__)
typedef long long RwInt64;
typedef unsigned long long RwUInt64;
#define RWZERO64 ((RwUInt64)0) 
#else /* defined(_MSC_VER) || defined(__GNUC__) */

typedef struct _RwUInt64 RwUInt64;
typedef struct _RwInt64 RwInt64;

/* We'll do it with structures (can't do maths on these, but OK for allocation): */
#if (!defined(DOXYGEN))
#ifdef rwBIGENDIAN
struct _RwUInt64
{
    RwUInt32 top;
    RwUInt32 bottom;
};

struct _RwInt64
{
    RwInt32 top;
    RwUInt32 bottom;
};

#else /* rwBIGENDIAN */
#ifdef rwLITTLEENDIAN
struct _RwUInt64
{
    RwUInt32 bottom;
    RwUInt32 top;
};

struct _RwInt64
{
    RwUInt32 bottom;
    RwInt32 top;
};

#else /* rwLITTLEENDIAN */
#error "ENDIAN-ness undefined!"
#endif /* rwLITTLEENDIAN */
#endif /* rwBIGENDIAN */
#endif /* (!defined(DOXYGEN)) */

#define RWZERO64 { (RwUInt32)0, (RwUInt32)0 }
#endif /* defined(_MSC_VER) || defined(__GNUC__) */

typedef struct _RwUInt128 RwUInt128;
typedef struct _RwInt128 RwInt128;

/* We'll do it with structures
 * (can't do maths on these, but OK for allocation): */
#if (!defined(DOXYGEN))
#ifdef rwBIGENDIAN 
struct _RwUInt128
{
    RwUInt64 top;
    RwUInt64 bottom;
};

struct _RwInt128
{
    RwInt64 top;
    RwUInt64 bottom;
};

#else /* rwBIGENDIAN */
#ifdef rwLITTLEENDIAN
struct _RwUInt128
{
    RwUInt64 bottom;
    RwUInt64 top;
};

struct _RwInt128
{
    RwUInt64 bottom;
    RwInt64 top;
};

#else /* rwLITTLEENDIAN */
#error "ENDIAN-ness undefined!"
#endif /* rwLITTLEENDIAN */
#endif /* rwBIGENDIAN */
#endif /* (!defined(DOXYGEN)) */

#define RWZERO128 { RWZERO64, RWZERO64 }

/* Limits of types */
#define RwInt32MAXVAL       0x7FFFFFFF
#define RwInt32MINVAL       0x80000000
#define RwUInt32MAXVAL      0xFFFFFFFF
#define RwUInt32MINVAL      0x00000000
#define RwRealMAXVAL        (RwReal)(3.40282347e+38)
#define RwRealMINVAL        (RwReal)(1.17549435e-38)
#define RwInt16MAXVAL       0x7FFF
#define RwInt16MINVAL       0x8000
#define RwUInt16MAXVAL      0xFFFF
#define RwUInt16MINVAL      0x0000

/* Structure alignment */
#define RWALIGN(type, x)   type /* nothing */
#define rwMATRIXALIGNMENT sizeof(RwUInt32)
#define rwFRAMEALIGNMENT sizeof(RwUInt32)
#define rwV4DALIGNMENT sizeof(RwUInt32)

#if (!defined(rwMALLOCALIGNMENT))
#define rwMALLOCALIGNMENT sizeof(RwUInt32)
#endif /* (!defined(rwMALLOCALIGNMENT) */

#if (defined(_MSC_VER))

#include <math.h>
/*
 * Keep true calls to these functions since
 * some x86 runtime libraries do not support _CIpow() etc
 */
#pragma function( acos, asin, cosh, fmod, pow, sinh , tanh ) 

#if (!defined(RWINT32FROMFLOAT))

static __inline RwInt32
int32fromreal(RwReal x)
{
    RwInt16 savemode;
    RwInt16 workmode;
    RwInt32 res;
    
    _asm
    {
        fnstcw    savemode      ; get fpu mode
        fld dword ptr[x]        ; load rwreal x  
  
        mov       ax,savemode   ; put fpu mode in register
        or        ah,0ch        ; or-in truncate mode
  
        mov       workmode,ax   ; make ready to set fpu mode
        fldcw     workmode      ; set fpu to truncate mode
        fistp     dword ptr[res]; store the rwint32eger result 
        fldcw     savemode      ; restore fpu mode
    }

    return res;
}
#define RwInt32FromRealMacro(x) int32fromreal(x)

#endif /* (!defined(RWINT32FROMFLOAT)) */

#if (!defined(NOASM) && !defined(__GNUC__) && !defined(__clang__))
static __inline RwUInt32 
RwFastRealToUInt32Inline(RwReal x)
{
    RwUInt32 res;

    __asm FLD DWord Ptr[x];
    __asm FISTP DWord Ptr[res];
    
    return(res);
}

#define RwFastRealToUInt32 RwFastRealToUInt32Inline

#endif /* (defined(NOASM)) */

#endif /* (defined(_MSC_VER)) */

#endif /* WIN_OSTYPES_H */


/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bamath.h ---*/

/****************************************************************************
 Defines
 */

#if (!defined(RwInt32FromRealMacro))
#define RwInt32FromRealMacro(x)                 \
    ((RwInt32)(x))
#endif /* (!defined(RwInt32FromRealMacro)) */

#if (!defined(RwFastRealToUInt32))
#define RwFastRealToUInt32(_x)                  \
   ((RwUInt32)RwInt32FromRealMacro(((RwReal)(_x))))
#endif /* (!defined(RwFastRealToUInt32)) */

/*
 * Ensure inclusion of prototypes for single precison maths functions
 * e.g. from
 * /usr/local/sce/ee/gcc/ee/include/math.h
 * /Program Files/Intel/Compiler4.0/include/mathf.h
 */

#if (defined(__ICL))

#if (defined(RWVERBOSE))

/*
 * See
 * http://www.eskimo.com/~scs/C-faq/q11.17.html
 */

#define _STRINGIFY(X) #X
#define _STRINGIFY_EXP(X) _STRINGIFY(X)

#pragma message ("Intel Compiler Version " _STRINGIFY_EXP(__ICL) ":" __FILE__ "(" _STRINGIFY_EXP(__LINE__) ")\n")
#pragma comment ( user, "comment:" "Intel Compiler Version " _STRINGIFY_EXP(__ICL) ":" __FILE__ "(" _STRINGIFY_EXP(__LINE__) ")\n")

#endif /* (defined(RWVERBOSE)) */

#if (400 <= __ICL)
#if (defined(__cplusplus))
#define _INC_MATH
#endif /* (defined(__cplusplus)) */
#include <mathf.h>
#else /* (400 < __ICL) */
#undef RW_USE_SPF
#endif /* (400 < __ICL) */

#endif /* (defined(__ICL)) */

#include <math.h>

#define   _RW_C1      ( (float) 4.1666667908e-02 )
#define   _RW_C2      ( (float)-1.3888889225e-03 )
#define   _RW_C3      ( (float) 2.4801587642e-05 )
#define   _RW_C4      ( (float)-2.7557314297e-07 )
#define   _RW_C5      ( (float) 2.0875723372e-09 )
#define   _RW_C6      ( (float)-1.1359647598e-11 )
#define   _RW_S1      ( (float)-1.6666667163e-01 )
#define   _RW_S2      ( (float) 8.3333337680e-03 )
#define   _RW_S3      ( (float)-1.9841270114e-04 )
#define   _RW_S4      ( (float) 2.7557314297e-06 )
#define   _RW_S5      ( (float)-2.5050759689e-08 )
#define   _RW_S6      ( (float) 1.5896910177e-10 )
#define   _RW_one     ( (float) 1.0000000000e+00 )
#define   _RW_pS0     ( (float) 1.6666667163e-01 )
#define   _RW_pS1     ( (float)-3.2556581497e-01 )
#define   _RW_pS2     ( (float) 2.0121252537e-01 )
#define   _RW_pS3     ( (float)-4.0055535734e-02 )
#define   _RW_pS4     ( (float) 7.9153501429e-04 )
#define   _RW_pS5     ( (float) 3.4793309169e-05 )
#define   _RW_pi      ( (float) 3.1415925026e+00 )
#define   _RW_pi_tol  ( (float) 0.0312500000e+00 )
#define   _RW_pio2_hi ( (float) 1.5707962513e+00 )
#define   _RW_pio2_lo ( (float) 7.5497894159e-08 )
#define   _RW_qS1     ( (float)-2.4033949375e+00 )
#define   _RW_qS2     ( (float) 2.0209457874e+00 )
#define   _RW_qS3     ( (float)-6.8828397989e-01 )
#define   _RW_qS4     ( (float) 7.7038154006e-02 )

#define RwCosMinusPiToPiMacro(result, x)                          \
MACRO_START                                                       \
{                                                                 \
    const float z = x * x;                                        \
    const float r = ( z * (_RW_C1  +                              \
                           z * (_RW_C2  +                         \
                                z * (_RW_C3  +                    \
                                     z * (_RW_C4  +               \
                                          z * (_RW_C5  +          \
                                               z * _RW_C6))))));  \
    result = (_RW_one - ((float) 0.5 * z - (z * r )));            \
}                                                                 \
MACRO_STOP

#define RwSinMinusPiToPiMacro(result, x)                          \
do                                                                \
{                                                                 \
    const float z = x * x;                                        \
    const float v = z * x;                                        \
    const float r = ( _RW_S2 +                                    \
                      z * (_RW_S3 +                               \
                           z * (_RW_S4 +                          \
                                z * (_RW_S5 +                     \
                                     z * _RW_S6))) );             \
    result = x + v * (_RW_S1 + z * r);                            \
}                                                                 \
while(0)                                                                  

typedef union _rwIEEEFloatShapeType _rwIEEEFloatShapeType;

#if (!defined(DOXYGEN))
union _rwIEEEFloatShapeType
{
    float               value;
    unsigned int        word;
};
#endif /* (!defined(DOXYGEN)) */

#define _RW_GET_FLOAT_WORD(i,d)                 \
do {                                            \
  _rwIEEEFloatShapeType gf_u;                   \
  gf_u.value = (d);                             \
  (i) = gf_u.word;                              \
} while (0)

/* Set a float from a 32 bit int.  */

#define _RW_SET_FLOAT_WORD(d,i)                 \
do {                                            \
  _rwIEEEFloatShapeType sf_u;                   \
  sf_u.word = (i);                              \
  (d) = sf_u.value;                             \
} while (0)

#define RwIEEEACosfMacro(result, x)                                      \
do                                                                       \
{                                                                        \
    float               z, p, q, r, w, s, c, df;                         \
    int                 hx, ix;                                          \
                                                                         \
    _RW_GET_FLOAT_WORD(hx, x);                                           \
    ix = hx & 0x7fffffff;                                                \
    if (ix >= 0x3f800000)                                                \
    {                          /* |x|>=1 */                              \
        if (hx > 0)                                                      \
        {                                                                \
            /* acos(1) = 0  */                                           \
            result = (0.0f);                                              \
        }                                                                \
        else                                                             \
        {                                                                \
            /* acos(-1)= _RW_pi */                                       \
            result = (_RW_pi + (float) 2.0 * _RW_pio2_lo);               \
        }                                                                \
                                                                         \
    }                                                                    \
    else if (ix < 0x3f000000)                                            \
    {                          /* |x| < 0.5 */                           \
        if (ix <= 0x23000000)                                            \
        {                                                                \
            /*if |x|<2**-57 */                                           \
            result = (_RW_pio2_hi + _RW_pio2_lo);                        \
        }                                                                \
        else                                                             \
        {                                                                \
            z = x * x;                                                   \
            p = z * (_RW_pS0 +                                           \
                     z * (_RW_pS1 +                                      \
                          z * (_RW_pS2 +                                 \
                               z * (_RW_pS3 +                            \
                                    z * (_RW_pS4 + z * _RW_pS5)))));     \
            q = _RW_one + z * (_RW_qS1 +                                 \
                               z * (_RW_qS2 +                            \
                                    z * (_RW_qS3 + z * _RW_qS4)));       \
            r = p / q;                                                   \
            result = (_RW_pio2_hi - (x - (_RW_pio2_lo - x * r)));        \
        }                                                                \
                                                                         \
    }                                                                    \
    else if (hx < 0)                                                     \
    {                          /* x < -0.5 */                            \
        z = (_RW_one + x) * (float) 0.5;                                 \
        p = z * (_RW_pS0 +                                               \
                 z * (_RW_pS1 +                                          \
                      z * (_RW_pS2 +                                     \
                           z * (_RW_pS3 +                                \
                                z * (_RW_pS4 + z * _RW_pS5)))));         \
        q = _RW_one + z * (_RW_qS1 +                                     \
                           z * (_RW_qS2 + z * (_RW_qS3 + z * _RW_qS4))); \
        rwSqrtMacro(&s, z);                                              \
        r = p / q;                                                       \
        w = r * s - _RW_pio2_lo;                                         \
        result = (_RW_pi - (float) 2.0 * (s + w));                       \
    }                                                                    \
    else                                                                 \
    {                          /* x > 0.5 */                             \
        int                 idf;                                         \
                                                                         \
        z = (_RW_one - x) * (float) 0.5;                                 \
        rwSqrtMacro(&s, z);                                              \
        df = s;                                                          \
        _RW_GET_FLOAT_WORD(idf, df);                                     \
        _RW_SET_FLOAT_WORD(df, idf & 0xfffff000);                        \
        c = (z - df * df) / (s + df);                                    \
        p = z * (_RW_pS0 +                                               \
                 z * (_RW_pS1 +                                          \
                      z * (_RW_pS2 +                                     \
                           z * (_RW_pS3 +                                \
                                z * (_RW_pS4 + z * _RW_pS5)))));         \
        q = _RW_one + z * (_RW_qS1 +                                     \
                           z * (_RW_qS2 + z * (_RW_qS3 + z * _RW_qS4))); \
        r = p / q;                                                       \
        w = r * s + c;                                                   \
        result = ((float) 2.0 * (df + w));                               \
    }                                                                    \
}                                                                        \
while(0)

#if (defined(RW_USE_SPF))

#define RwACos(_x)              acosf(_x)
#define RwACosh(_x)             acoshf(_x)
#define RwASin(_x)              asinf(_x)
#define RwASinh(_x)             asinhf(_x)

#if (!defined(__ICL))
/*
 * No SPF version in
 * Program Files/Intel/compilerXXX/include/mathf.h
 * of atan2()
 */
#define RwATan2(_x, _y)         atan2f(_x, _y)
#endif /* (!defined(__ICL)) */

#define RwATan(_x)              atanf(_x)
#define RwATanh(_x)             atanhf(_x)
#define RwCabs()                cabsf()
#define RwCbrt(_x)              cbrtf(_x)
#define RwCeil(_x)              ceilf(_x)
#define RwCopysign(_x, _y)      copysignf(_x, _y)
#define RwCos(_x)               cosf(_x)
#define RwCosh(_x)              coshf(_x)
#define RwDrem(_x, _y)          dremf(_x, _y)
#define RwErfc(_x)              erfcf(_x)
#define RwErf(_x)               erff(_x)
#define RwExp(_x)               expf(_x)
#define RwExpm1(_x)             expm1f(_x)
#define RwFinite(_x)            finitef(_x)
#define RwIlogb(_x)             ilogbf(_x)
#define RwIsinf(_x)             isinff(_x)
#define RwIsnan(_x)             isnanf(_x)
#define RwFabs(_x)              fabsf(_x)
#define RwFloor(_x)             floorf(_x)
#define RwFmod(_x, _y)          fmodf(_x, _y)
#define RwFrexp(_x, _iptr)      frexpf(_x, _iptr)
#define RwGamma(_x)             gammaf(_x)
#define RwGammaf_(_x, _iptr)    gammaf_r(_x, _iptr)
#define RwHypot(_x, _y)         hypotf(_x, _y)
#define RwInfinity()            infinityf()
#define RwJ0(_x)                j0f(_x)
#define RwJ1(_x)                j1f(_x)
#define RwJn(_i, _x)            jnf(_i, _x)
#define RwLdexp(_x, _i)         ldexpf(_x, _i)
#define RwLgamma(_x)            lgammaf(_x)
#define RwLgammaf_(_x, _iptr)   lgammaf_r(_x, _iptr)
#define RwLog10(_x)             log10f(_x)
#define RwLog1p(_x)             log1pf(_x)
#define RwLog(_x)               logf(_x)
#define RwModf(_x, _y)          modff(_x, _y)
#define RwNan()                 nanf()
#define RwNextafter(_x, _y)     nextafterf(_x, _y)
#define RwPow(_x, _y)           powf(_x, _y)
#define RwRemainder(_x, _y)     remainderf(_x, _y)
#define RwRint(_x)              rintf(_x)
#define RwScalbn(_x, _i)        scalbnf(_x, _i)
#define RwSin(_x)               sinf(_x)
#define RwSinh(_x)              sinhf(_x)
/* rwSqrtMacro/rwInvSqrtMacro are overloaded in drvmodel.h
 * (if they are at all) and wrapped as func/macro below */
#define RwTan(_x)               tanf(_x)
#define RwTanh(_x)              tanhf(_x)
#define RwY0(_x)                y0f(_x)
#define RwY1(_x)                y1f(_x)
#define RwYn(_i, _x)            ynf(_i, _x)

#endif /* (defined(RW_USE_SPF)) */

#if (!defined(RwACos))
#define RwACos(_x)              acos(_x)
#endif /* (!defined(RwACos)) */
#if (!defined(RwACosh))
#define RwACosh(_x)             acosh(_x)
#endif /* (!defined(RwACosh)) */
#if (!defined(RwASin))
#define RwASin(_x)              asin(_x)
#endif /* (!defined(RwASin)) */
#if (!defined(RwASinh))
#define RwASinh(_x)             asinh(_x)
#endif /* (!defined(RwASinh)) */
#if (!defined(RwATan2))
#define RwATan2(_x, _y)         atan2(_x, _y)
#endif /* (!defined(RwATan2)) */
#if (!defined(RwATan))
#define RwATan(_x)              atan(_x)
#endif /* (!defined(RwATan)) */
#if (!defined(RwATanh))
#define RwATanh(_x)             atanh(_x)
#endif /* (!defined(RwATanh)) */
#if (!defined(RwCabs))
#define RwCabs()                cabs()
#endif /* (!defined(RwCabs)) */
#if (!defined(RwCbrt))
#define RwCbrt(_x)              cbrt(_x)
#endif /* (!defined(RwCbrt)) */
#if (!defined(RwCeil))
#define RwCeil(_x)              ceil(_x)
#endif /* (!defined(RwCeil)) */
#if (!defined(RwCopysign))
#define RwCopysign(_x, _y)      copysign(_x, _y)
#endif /* (!defined(RwCopysign)) */
#if (!defined(RwCos))
#define RwCos(_x)               cos(_x)
#endif /* (!defined(RwCos)) */
#if (!defined(RwCosh))
#define RwCosh(_x)              cosh(_x)
#endif /* (!defined(RwCosh)) */
#if (!defined(RwDrem))
#define RwDrem(_x, _y)          drem(_x, _y)
#endif /* (!defined(RwDrem)) */
#if (!defined(RwErfc))
#define RwErfc(_x)              erfc(_x)
#endif /* (!defined(RwErfc)) */
#if (!defined(RwEr))
#define RwEr(_x)               erf(_x)
#endif /* (!defined(RwEr)) */
#if (!defined(RwExp))
#define RwExp(_x)               exp(_x)
#endif /* (!defined(RwExp)) */
#if (!defined(RwExpm1))
#define RwExpm1(_x)             expm1(_x)
#endif /* (!defined(RwExpm1)) */
#if (!defined(RwFinite))
#define RwFinite(_x)            finite(_x)
#endif /* (!defined(RwFinite)) */
#if (!defined(RwIlogb))
#define RwIlogb(_x)             ilogb(_x)
#endif /* (!defined(RwIlogb)) */
#if (!defined(RwIsin))
#define RwIsin(_x)             isinf(_x)
#endif /* (!defined(RwIsin)) */
#if (!defined(RwIsnan))
#define RwIsnan(_x)             isnan(_x)
#endif /* (!defined(RwIsnan)) */
#if (!defined(RwFabs))
#define RwFabs(_x)              fabs(_x)
#endif /* (!defined(RwFabs)) */
#if (!defined(RwFloor))
#define RwFloor(_x)             floor(_x)
#endif /* (!defined(RwFloor)) */
#if (!defined(RwFmod))
#define RwFmod(_x, _y)          fmod(_x, _y)
#endif /* (!defined(RwFmod)) */
#if (!defined(RwFrexp))
#define RwFrexp(_x, _iptr)      frexp(_x, _iptr)
#endif /* (!defined(RwFrexp)) */
#if (!defined(RwGamma))
#define RwGamma(_x)             gamma(_x)
#endif /* (!defined(RwGamma)) */
#if (!defined(RwGammaf_))
#define RwGammaf_(_x, _iptr)    gammaf_r(_x, _iptr)
#endif /* (!defined(RwGammaf_)) */
#if (!defined(RwHypot))
#define RwHypot(_x, _y)         hypot(_x, _y)
#endif /* (!defined(RwHypot)) */
#if (!defined(RwInfinity))
#define RwInfinity()            infinity()
#endif /* (!defined(RwInfinity)) */
#if (!defined(RwJ0))
#define RwJ0(_x)                j0(_x)
#endif /* (!defined(RwJ0)) */
#if (!defined(RwJ1))
#define RwJ1(_x)                j1(_x)
#endif /* (!defined(RwJ1)) */
#if (!defined(RwJn))
#define RwJn(_i, _x)            jn(_i, _x)
#endif /* (!defined(RwJn)) */
#if (!defined(RwLdexp))
#define RwLdexp(_x, _i)         ldexp(_x, _i)
#endif /* (!defined(RwLdexp)) */
#if (!defined(RwLgamma))
#define RwLgamma(_x)            lgamma(_x)
#endif /* (!defined(RwLgamma)) */
#if (!defined(RwLgammaf_))
#define RwLgammaf_(_x, _iptr)   lgammaf_r(_x, _iptr)
#endif /* (!defined(RwLgammaf_)) */
#if (!defined(RwLog10))
#define RwLog10(_x)             log10(_x)
#endif /* (!defined(RwLog10)) */
#if (!defined(RwLog1p))
#define RwLog1p(_x)             log1p(_x)
#endif /* (!defined(RwLog1p)) */
#if (!defined(RwLog))
#define RwLog(_x)               log(_x)
#endif /* (!defined(RwLog)) */
#if (!defined(RwMod))
#define RwMod(_x, _y)           mod(_x, _y )
#endif /* (!defined(RwMod)) */
#if (!defined(RwNan))
#define RwNan()                 nan()
#endif /* (!defined(RwNan)) */
#if (!defined(RwNextafter))
#define RwNextafter(_x, _y)     nextafter(_x, _y)
#endif /* (!defined(RwNextafter)) */
#if (!defined(RwPow))
#define RwPow(_x, _y)           pow(_x, _y)
#endif /* (!defined(RwPow)) */
#if (!defined(RwRemainder))
#define RwRemainder(_x, _y)     remainder(_x, _y)
#endif /* (!defined(RwRemainder)) */
#if (!defined(RwRint))
#define RwRint(_x)              rint(_x)
#endif /* (!defined(RwRint)) */
#if (!defined(RwScalbn))
#define RwScalbn(_x, _i)        scalbn(_x, _i)
#endif /* (!defined(RwScalbn)) */
#if (!defined(RwSin))
#define RwSin(_x)               sin(_x)
#endif /* (!defined(RwSin)) */
#if (!defined(RwSinh))
#define RwSinh(_x)              sinh(_x)
#endif /* (!defined(RwSinh)) */
#if (!defined(rwSqrt))
/* NOTE: this is overloaded in drvmodel.h for some targets (SKY2 and XBOX atm)
 * [we do in fact do overload w/ sqrtf there, if RW_USE_SPF,
 *  for D3D8, OpenGL and SoftRas] */
#define rwSqrt(_result, _x)     rwSqrtMacro(_result, _x)
#endif /* (!defined(rwSqrt)) */
#if (!defined(rwInvSqrt))
/* NOTE: this is overloaded in drvmodel.h for some targets (SKY2 and XBOX atm)
 * [we do in fact do overload w/ (1 / sqrtf) there, if RW_USE_SPF,
 *  for D3D8, OpenGL and SoftRas] */
#define rwInvSqrt(_recip, _x)   rwInvSqrtMacro(_recip, _x)
#endif /* (!defined(rwInvSqrt)) */
#if (!defined(RwTan))
#define RwTan(_x)               tan(_x)
#endif /* (!defined(RwTan)) */
#if (!defined(RwTanh))
#define RwTanh(_x)              tanh(_x)
#endif /* (!defined(RwTanh)) */
#if (!defined(RwY0))
#define RwY0(_x)                y0(_x)
#endif /* (!defined(RwY0)) */
#if (!defined(RwY1))
#define RwY1(_x)                y1(_x)
#endif /* (!defined(RwY1)) */
#if (!defined(RwYn))
#define RwYn(_i, _x)            yn(_i, _x)
#endif /* (!defined(RwYn)) */


/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/batypes.h ---*/
#define rwLIBRARYBASEVERSION    0x34000
#define rwLIBRARYNOWARNVERSION  0x35000
#define rwLIBRARYCURRENTVERSION 0x36003

#define rwLIBRARYVERSION34001   0x34001
#if (rwLIBRARYVERSION34001 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION34001 code"
#endif

#define rwLIBRARYVERSION34002   0x34002
#if (rwLIBRARYVERSION34002 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION34002 code"
#endif

#define rwLIBRARYVERSION34004   0x34004
#if (rwLIBRARYVERSION34004 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION34004 code"
#endif

#define rwLIBRARYVERSION35000   0x35000
#if (rwLIBRARYVERSION35000 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION35000 code"
#endif

#define rwLIBRARYVERSION36000   0x36000
#if (rwLIBRARYVERSION36000 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION36000 code"
#endif

#define rwLIBRARYVERSION36002   0x36002
#if (rwLIBRARYVERSION36002 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION36002 code"
#endif

#define rwLIBRARYVERSION36003   0x36003
#if (rwLIBRARYVERSION36003 < rwLIBRARYBASEVERSION)
#error "Time to remove all rwLIBRARYVERSION36003 code"
#endif

/*
 * RWBUILDNUMBER
 * This 16-bit int will be externally defined in an official build, and
 * is used to construct chunk header library ID when streaming out. All
 * unofficial builds will be stamped with the following:-
 */
#if !defined(RWBUILDNUMBER)
#define RWBUILDNUMBER 0xffff
#endif

/* IMPORTANT:
 * The following Doxygen comment MUST be copied into RwCore.h,
 * so don't move it from here. */

#if (!defined(RWFORCEENUMSIZEINT))
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#endif /* (!defined(RWFORCEENUMSIZEINT)) */

/*
 * See
 * http://www.eskimo.com/~scs/C-faq/q11.17.html
 */

#define RW_STRINGIFY(X) #X
#define RW_STRINGIFY_EXPANDED(X) RW_STRINGIFY(X)

/****************************************************************************
 Attributes
 */

#if (defined(__GNUC__))

/* See http://www.gnu.org/software/gcc/onlinedocs/gcc_4.html#SEC91 */

#if (!(defined(__cplusplus) || defined(__MWERKS__) || defined(__RWUNUSED__)))
#define __RWUNUSED__ __attribute__ ((unused))
#endif /* (!(defined(__cplusplus) || defined(__MWERKS__) || defined(__RWUNUSED__))) */

#if (!(defined(__RWUNUSEDRELEASE__) || defined(RWVALIDATEPARAM)))
#if (!( defined(__cplusplus) || defined(__MWERKS__) || defined(RWDEBUG)))
#define __RWUNUSEDRELEASE__ __attribute__ ((unused))
#endif /* (!(defined(__cplusplus) || defined(__MWERKS__) || defined(RWDEBUG))) */
#endif /* (!(defined(__RWUNUSEDRELEASE__) || defined(RWVALIDATEPARAM))) */

#if (!defined(__RWFORMAT__))
#define __RWFORMAT__(_archetype, _string_index, _first_to_check)        \
    __attribute__ ((format (_archetype, _string_index, _first_to_check)))
#endif /* (!defined(__RWFORMAT__)) */

#endif /* (defined(__GNUC__)) */

#if (!defined(__RWUNUSED__))
#define __RWUNUSED__           /* No op */
#endif /* (!defined(__RWUNUSED__)) */

#if (!defined(__RWUNUSEDRELEASE__))
#define __RWUNUSEDRELEASE__           /* No op */
#endif /* (!defined(__RWUNUSEDRELEASE__)) */

#if (!defined(__RWFORMAT__))
#define __RWFORMAT__(_archetype, _string_index, _first_to_check)  /* No op */
#endif /* (!defined(__RWFORMAT__)) */

/****************************************************************************
 Calling conventions
 */

#if (defined(WIN32))
#define RWASMCALL   __cdecl
#define RWASMAPI(TYPE) TYPE RWASMCALL
#endif /* (defined(WIN32)) */

#if (!defined(RWASMCALL))
#define RWASMCALL              /* No op */
#endif /* (!defined(RWASMCALL)) */

#if (!defined(RWASMAPI))
#define RWASMAPI(TYPE) TYPE
#endif /* (!defined(RWASMAPI)) */


/* Maximum number of nested contexts */
#define rwMAXPIPECONTEXT 10


/****************************************************************************
 Macro wrappers. These are needed everywhere.
 */

#ifndef MACRO_START
#define MACRO_START do
#endif /* MACRO_START */

#ifndef MACRO_STOP
#define MACRO_STOP while(0)
#endif /* MACRO_STOP */

/****************************************************************************
 Types needed everywhere
 */

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE !FALSE

/****************************************************************************
 MS VC/C++ Specific
 */

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)


/*
 * Check for correct compiler version
 */
#define RW_MSC_VER 1200

#if (0 && !defined(RW_NO_COMPILER_CHECK))
#if (_MSC_VER != RW_MSC_VER )
# pragma message (__FILE__ "(" RW_STRINGIFY_EXPANDED(__LINE__) "):" "\n  This compiler is a different version (" RW_STRINGIFY_EXPANDED(_MSC_VER) ")\n  to the compiler used to build the RenderWare product libraries (" RW_STRINGIFY_EXPANDED(RW_MSC_VER) ") \n  To turn off this warning please define RW_NO_COMPILER_CHECK " )
# pragma comment ( user, "comment:" __FILE__ "(" RW_STRINGIFY_EXPANDED(__LINE__) "):" "\n  This compiler is a different version (" RW_STRINGIFY_EXPANDED(_MSC_VER) ")\n  to the compiler used to build the RenderWare product libraries (" RW_STRINGIFY_EXPANDED(RW_MSC_VER) ") \n  To turn off this warning please define RW_NO_COMPILER_CHECK " )
#endif /* (_MSC_VER != RW_MSC_VER ) */
#endif /* (0 && !defined(RW_NO_COMPILER_CHECK)) */

/*
 * Output some compiler messages and object file comments
 */

#pragma comment ( compiler )

#pragma comment ( user, "comment:" __DATE__" "  __TIME__ " - " __FILE__ "(" RW_STRINGIFY_EXPANDED(__LINE__) ")")
#pragma comment ( user, "comment:" " _MSC_VER==" RW_STRINGIFY_EXPANDED(_MSC_VER) "; _M_IX86==" RW_STRINGIFY_EXPANDED(_M_IX86))
#if (defined(rwLIBRARYCURRENTVERSION))
#pragma comment ( user, "comment:" "rwLIBRARYCURRENTVERSION:" RW_STRINGIFY_EXPANDED(rwLIBRARYCURRENTVERSION) )
#endif /* (defined(rwLIBRARYCURRENTVERSION)) */

#if (defined(RWDEBUG) && defined(RWVERBOSE))

#if (defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC))
#define _CRTDBG_MAP_ALLOC
#endif /* defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC)) */
#include <crtdbg.h>

#pragma message (__DATE__" "  __TIME__ " - " __FILE__ "(" RW_STRINGIFY_EXPANDED(__LINE__) ")" )
#pragma message ("_MSC_VER==" RW_STRINGIFY_EXPANDED(_MSC_VER) "; _M_IX86==" RW_STRINGIFY_EXPANDED(_M_IX86))

#if (defined(rwLIBRARYCURRENTVERSION))
#pragma message ( "rwLIBRARYCURRENTVERSION:" RW_STRINGIFY_EXPANDED(rwLIBRARYCURRENTVERSION) )
#endif /* (defined(rwLIBRARYCURRENTVERSION)) */

#endif /* (defined(RWDEBUG) && defined(RWVERBOSE) ) */

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

/*******************/
/* Primitive types */
/*******************/

/* String construction stuff (gets us UNICODE support) */
#ifdef RWUNICODE
#define _RWSTRING(x) L ## x
#else /* RWUNICODE */
#define _RWSTRING(x) x
#endif /* RWUNICODE */
#define RWSTRING(x) _RWSTRING(x)

/* NB volatile keyword required for VC5.0 to ensure a reload - AMB */
typedef union RwSplitBits RwSplitBits;

#if (!defined(DOXYGEN))
union RwSplitBits
{
    RwReal nReal;
    volatile RwInt32 nInt;
    volatile RwUInt32 nUInt;
};

typedef struct RwSplitFixed RwSplitFixed;

#ifdef rwBIGENDIAN
struct RwSplitFixed
{
    RwInt16 integral;
    RwUInt16 fractional;
};

#else /* rwBIGENDIAN */
#ifdef rwLITTLEENDIAN
struct RwSplitFixed
{
    RwUInt16 fractional;
    RwInt16 integral;
};

#else /* rwLITTLEENDIAN */
#error "ENDIAN-ness undefined!"
#endif /* rwLITTLEENDIAN */
#endif /* rwBIGENDIAN */

typedef union RwUnionReal RwUnionReal;

union RwUnionReal /* MSB is sign bit in any circumstance */
{
    RwReal real; /* 4 bytes interpreted as RwReal */
    float floating; /* 4 bytes interpreted as float */
    RwFixed fixed; /* 4 bytes interpreted as 16:16 fixed */
    RwSplitFixed splitfixed; /* 4 bytes interpreted as 16:16 fixed */
};
#endif /* (!defined(DOXYGEN)) */

/*****************/

/* Complex types */

/*****************/

typedef struct RwV2d RwV2d;
/**
 * \ingroup rwv2d
 * \struct RwV2d
 * This type represents points in a 2D space, such as device
 * space, specified by the (x, y) coordinates of the point.
 */
struct RwV2d
{
    RwReal x;   /**< X value*/
    RwReal y;   /**< Y value */
};

typedef struct RwV3d RwV3d;
/**
 * \ingroup rwv3d
 * \struct RwV3d
 *  This type represents 3D points and vectors specified by
 * the (x, y, z) coordinates of a 3D point or the (x, y, z) components of a
 * 3D vector.
 */
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};

#define RWV4DALIGNMENT(_v4d) \
   (! (((rwV4DALIGNMENT)-1) & ((RwUInt32)(_v4d))))


#if (!defined(DOXYGEN))

struct RwV4dTag
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
    RwReal w;   /**< W value */
};

/*
 * RwV4d & RwV4dTag must be different otherwise the alignment
 * directive is ignored when compiling under C++ on Visual C
 */
typedef struct RwV4dTag RWALIGN(RwV4d, rwV4DALIGNMENT);

#else /* (!defined(DOXYGEN)) */

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwv4d
 * \struct RwV4d
 *  This type represents 4D points and vectors specified by
 * the (x, y, z, w) coordinates of a 4D point or the (x, y, z, w) components of a
 * 4D vector.
 */
#endif /* RWADOXYGENEXTERNAL */

typedef struct RwV4d RWALIGN(RwV4d, rwV4DALIGNMENT);

#endif /* (!defined(DOXYGEN)) */



typedef struct RwRect RwRect;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup geometricaltypes
 * \struct RwRect
 * This type represents a 2D device space rectangle specified
 * by the position of the top-left corner (the offset x, y) and its width (w)
 * and height (h).
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwRect
{
    RwInt32 x;  /**< X value of the top-left corner */
    RwInt32 y;  /**< Y value of the top-left corner */
    RwInt32 w;  /**< Width of the rectangle */
    RwInt32 h;  /**< Height of the rectangle */
};

typedef struct RwSphere RwSphere;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup geometricaltypes
 * \struct RwSphere
 * This type represents a sphere specified by the position
 * of its center and its radius.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwSphere
{
    RwV3d center;   /**< Sphere center */
    RwReal radius;  /**< Sphere radius */
};

#if (!defined(RwSphereAssign))
#define RwSphereAssign(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwSphereAssign)) */

typedef struct RwLine RwLine;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup geometricaltypes
 * \struct RwLine
 * This type represents a 3D line specified by the position
 * of its start and end points.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwLine
{
    RwV3d start;    /**< Line start */
    RwV3d end;      /**< Line end */
};

#if (!defined(RwLineAssign))
#define RwLineAssign(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwLineAssign)) */

/* The maximum number of texture coordinates */
#define rwMAXTEXTURECOORDS 8
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup fundtypesdatatypes
 * RwTextureCoordinateIndex
 *  This type represents the index for texture coordinates.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureCoordinateIndex
{
    rwNARWTEXTURECOORDINATEINDEX = 0,
    rwTEXTURECOORDINATEINDEX0,
    rwTEXTURECOORDINATEINDEX1,
    rwTEXTURECOORDINATEINDEX2,
    rwTEXTURECOORDINATEINDEX3,
    rwTEXTURECOORDINATEINDEX4,
    rwTEXTURECOORDINATEINDEX5,
    rwTEXTURECOORDINATEINDEX6,
    rwTEXTURECOORDINATEINDEX7,
    rwTEXTURECOORDINATEINDEXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureCoordinateIndex RwTextureCoordinateIndex;

typedef struct RwTexCoords RwTexCoords;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup fundtypesdatatypes
 * \struct RwTexCoords
 * This type represents the u and v texture
 * coordinates of a particular vertex.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwTexCoords
{
    RwReal u;   /**< U value */
    RwReal v;   /**< V value */
};


/* Singley linked list macros. End marked as NULL */

typedef struct RwSLLink RwSLLink;    /*** RwSLLink ***/

#if (!defined(DOXYGEN))
struct RwSLLink
{
    RwSLLink  *next;
};
#endif /* (!defined(DOXYGEN)) */

#define rwSLLinkGetData(link,type,entry)                                \
    ((type *)(((RwUInt8 *)(link))-offsetof(type,entry)))

#define rwSLLinkGetConstData(link,type,entry)                           \
    ((const type *)(((const RwUInt8 *)(link))-offsetof(type,entry)))

#define rwSLLinkInitialize(linkvar)                                     \
    (linkvar)->next = NULL;

#define rwSLLinkGetNext(linkvar)                                        \
    ((linkvar)->next)

typedef struct RwSingleList RwSingleList;

#if (!defined(DOXYGEN))
struct RwSingleList
{
    RwSLLink link;
};
#endif /* (!defined(DOXYGEN)) */

#define rwSingleListInitialize(list)                                    \
    (list)->link.next= NULL;
#define rwSingleListEmpty(list)                                         \
    (((list)->link.next)==NULL)
#define rwSingleListAddSLLink(list,linkvar)                             \
    ( (linkvar)->next = (list)->link.next,                              \
      (list)->link.next = (linkvar) )
#define rwSingleListGetFirstSLLink(list)                                \
    ((list)->link.next)
#define rwSingleListGetTerminator(list) (NULL)

/* Doubly linked list. End marked as start (its a ring) */

typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/

#if (!defined(DOXYGEN))
struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};
#endif /* (!defined(DOXYGEN)) */

#define rwLLLinkGetData(linkvar,type,entry)                             \
    ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))

#define rwLLLinkGetConstData(linkvar,type,entry)                        \
    ((const type *)(((const RwUInt8 *)(linkvar))-offsetof(type,entry)))

#define rwLLLinkGetNext(linkvar)                                        \
    ((linkvar)->next)

#define rwLLLinkGetPrevious(linkvar)                                    \
    ((linkvar)->prev)

#define rwLLLinkInitialize(linkvar)                                     \
    ( (linkvar)->prev = (RwLLLink *)NULL,                               \
      (linkvar)->next = (RwLLLink *)NULL )

#define rwLLLinkAttached(linkvar)                                       \
    ((linkvar)->next)

typedef struct RwLinkList RwLinkList;

#if (!defined(DOXYGEN))
struct RwLinkList
{
    RwLLLink link;
};
#endif /* (!defined(DOXYGEN)) */

#define rwLinkListInitialize(list)                                      \
    ( (list)->link.next = ((RwLLLink *)(list)),                         \
      (list)->link.prev = ((RwLLLink *)(list)) )
#define rwLinkListEmpty(list)                                           \
    (((list)->link.next) == (&(list)->link))
#define rwLinkListAddLLLink(list, linkvar)                              \
    ( (linkvar)->next = (list)->link.next,                              \
      (linkvar)->prev = (&(list)->link),                                \
      ((list)->link.next)->prev = (linkvar),                            \
      (list)->link.next = (linkvar) )
#define rwLinkListRemoveLLLink(linkvar)                                 \
    ( ((linkvar)->prev)->next = (linkvar)->next,                        \
      ((linkvar)->next)->prev = (linkvar)->prev )
#define rwLinkListGetFirstLLLink(list)                                  \
    ((list)->link.next)
#define rwLinkListGetLastLLLink(list)                                   \
    ((list)->link.prev)
#define rwLinkListGetTerminator(list)                                   \
    (&((list)->link))

typedef struct RwSurfaceProperties RwSurfaceProperties;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup fundtypesdatatypes
 * \struct RwSurfaceProperties
 *  This type represents the ambient, diffuse and
 * specular reflection coefficients of a particular geometry. Each coefficient
 * is specified in the range 0.0 (no reflection) to 1.0 (maximum reflection). 
 * Note that currently the specular element is not used.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwSurfaceProperties
{
    RwReal ambient;   /**< ambient reflection coefficient */
    RwReal specular;  /**< specular reflection coefficient */
    RwReal diffuse;   /**< reflection coefficient */
};

#if (!defined(RwSurfacePropertiesAssign))
#define RwSurfacePropertiesAssign(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwSurfacePropertiesAssign)) */

/**********
 * Macros *
 **********/

/* ANSI C defines the offsetof(type,member) macro; should be in <stddef.h> */

/* If not, fall back to this: */
#ifndef offsetof
#define offsetof(type, member)                                          \
    ((size_t)((RwUInt8 *)&((type *) 0)->member - (RwUInt8 *)((type *) 0)))
#endif /* offsetof */

/*
 *
 * Numeric Macros to handle Fixed/Floating point versions of RenderWare
 *
 */
#define RWFIX_MIN     (1)
#define RWFIX_MAX     (0x7fffffff)
#define RwFixedToInt(a)    ((a) >> 16)
#define RwFixedToFloat(a)  ((float)(((float)(a)) * (1.0f / 65536.0f)))
#define RwFixedToReal(a)   ((RwReal)(((RwReal)(a)) * (1.0f / 65536.0f)))
#define RwIntToFixed(a)    ((a) << 16)
#define RwRealToFixed(a)   (RwInt32FromRealMacro((a) * 65536.0f))
#define RwRealAbs(a)       ((RwReal)((a) >= (RwReal)(0.0) ? (a) : (-(a))))
#define RwRealMin2(a,b)    ((RwReal)( ((a) <= (b)) ?  (a) : (b)))
#define RwRealMax2(a,b)    ((RwReal)( ((a) >= (b)) ?  (a) : (b)))
#define RwRealMin3(a,b,c)  RwRealMin2(a,RwRealMin2(b,c))
#define RwRealMax3(a,b,c)  RwRealMax2(a,RwRealMax2(b,c))

#ifndef NORWREALSHORTCUT
#define RToFixed RwRealToFixed
#define RAbs     RwRealAbs
#define FxToInt   RwFixedToInt
#define FxToFloat RwFixedToFloat
#define FxToReal  RwFixedToFloat

#endif

#ifndef rwPI
#define rwPI ((RwReal)(3.1415926535f))
#define rwPIOVER2 (rwPI / (RwReal)(2.0f))
#endif
#define RWRGBALONG(r,g,b,a)                                             \
    ((RwUInt32) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                             RwPlane

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


/*
 * typedef for struct RwPlane
 */
typedef struct RwPlane RwPlane;

#if (!defined(DOXYGEN))
/*
 * This type represents a plane
 */
struct RwPlane
{
    RwV3d normal;    /**< Normal to the plane */
    RwReal distance; /**< Distance to plane from origin in normal direction*/
};
#endif /* (!defined(DOXYGEN)) */

/****************************************************************************
 Defines
 */

enum RwPlaneType
{
    rwXPLANE = 0, /* These are deliberately multiples of sizeof(RwReal) */
    rwYPLANE = 4,
    rwZPLANE = 8,
    rwPLANETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPlaneType RwPlaneType;

#define rwSECTORATOMIC -1
#define rwSECTORBUILD -2       /* Only used when building a world */

/* vect is a RwV3d, y is the component */
#define GETCOORD(vect,y)                                                \
    (*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define GETCONSTCOORD(vect,y)                                           \
    (*(const RwReal *)(((const RwUInt8 *)(&((vect).x)))+(RwInt32)(y)))
#define SETCOORD(vect,y,value)                                          \
    (((*(RwReal *)(((RwUInt8 *)(&((vect).x)))+(RwInt32)(y))))=(value))
#define SETCONSTCOORD(vect,y,value)                                      \
    (((*(const RwReal *)                                                \
       (((const RwUInt8 *)                                              \
         (&((vect).x)))+(RwInt32)(y))))=(value))
#define GETCOORDINT(vect,y)                                             \
    (*(RwInt32 *)(((RwUInt8 *)(&((vect).x)))+(y)))
#define GETCONSTCOORDINT(vect,y)                                        \
    (*(const RwInt32 *)(((const RwUInt8 *)(&((vect).x)))+(y)))


/**
 * \ingroup integertypes
 * \page inttypes Integer Types
 *
 * RenderWare supports a number of integer types:
 *
 * RwInt8 8-bit signed integer.
 *    \li RwUInt8 8-bit unsigned integer.
 *    \li RwChar Character type.
 *    \li RwInt16 16-bit signed integer.
 *    \li RwUInt16 16-bit unsigned integer.
 *    \li RwInt32 32-bit signed integer.
 *    \li RwUInt32 32-bit unsigned integer.
 *    \li RwInt64 64-bit signed integer.
 *    \li RwUInt64 64-bit unsigned integer.
 *    \li RwInt128 128-bit signed integer.
 *    \li RwUInt128 128-bit unsigned integer.
 *    \li RwBool Boolean type (in 32 bits).
 *
 * These types should be used in applications in preference to the underlying
 * native types.
 *
 * The following constants indicate the maximum and minimum values possible
 * for the various RenderWare integer types:
 *
 *    \li RwInt32MAXVAL   Maximum RwInt32 value.
 *    \li RwInt32MINVAL   Minimum RwInt32 value.
 *    \li RwUInt32MAXVAL   Maximum RwUInt32 value.
 *    \li RwUInt32MINVAL   Minimum RwUInt32 value.
 *    \li RwInt16MAXVAL   Maximum RwInt16 value.
 *    \li RwInt16MINVAL   Minimum RwInt16 value.
 *    \li RwUInt16MAXVAL   Maximum RwUInt16 value.
 *    \li RwUInt16MINVAL   Minimum RwUInt16 value.
 *
 * \see RwReal
 */

/**
 * \ingroup realtypes
 * \typedef RwReal
 *
 * RenderWare supports a single RwReal floating-point type to aid portability
 * across platforms. This type should be used in applications in preference to
 * the underlying native type.
 *
 * The constants RwRealMAXVAL and RwRealMINVAL are provided for determining
 * the maximum and minimum values possible using the RwReal type.
 *
 * In addition, the following macros are available for operations on RwReal
 * types:
 *      \li RwRealMin2(a, b)    Find the minimum of two RwReal values.
 *      \li RwRealMax2(a, b)    Find the maximum of two RwReal values.
 *      \li RwRealMin3(a, b, c)    Find the minimum of three RwReal values.
 *      \li RwRealMax3(a, b, c)    Find the maximum of three RwReal values.
 *      \li RwRealAbs(x)    Find the absolute value of a RwReal value.
 *
 * \see \ref inttypes
 */

/**
 * \ingroup realtypes
 * \typedef RwFixed
 *
 * RenderWare supports a single RwFixed fixed-point type.
 *
 * Although popular in the days when integer mathematics was much faster than
 * floating point mathematics, fixed-point math is now rarely used. It is
 * provided because it is still useful for some processes.
 *
 * The maximum and minimum size of an RwFixed value are defined by the constants
 * RWFIX_MAX and RWFIX_MIN respectively.
 *
 * The following macros are provided to help you work with RwFixed datatypes:
 *    \li RwFixedToInt(x)       Convert an RwFixed to an integer. (The fractional portion is lost.)
 *    \li RwFixedToFloat(x)     Convert an RwFixed to a float.
 *    \li RwFixedToReal(x)      Convert an RwFixed to an RwReal.
 *    \li RwRealToFixed(x)      Convert an RwReal to an RwFixed. (Some precision may be lost.)
 *    \li RwIntToFixed(x)       Convert an RwInt32 to an RwFixed. (Some precision may be lost.)
 */

/**
 * \ingroup integertypes
 * \typedef RwInt8
 *
 * Signed 8 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwUInt8
 *
 * Unsigned 8bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwChar
 *
 * Character type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwInt16
 *
 * Signed 16 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwUInt16
 *
 * Unsigned 16 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwInt32
 *
 * Signed 32 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwUInt32
 *
 * Unsigned 32 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwInt64
 *
 * Signed 64 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwUInt64
 *
 * Unsigned 64 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwInt128
 *
 * Signed 128 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwUInt128
 *
 * Unsigned 128 bit integer type.
 * \see \ref inttypes
 */

/**
 * \ingroup integertypes
 * \typedef RwBool
 *
 * Boolean type.
 * \see \ref inttypes
 */


/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/batype.h ---*/
/****************************************************************************
 Defines
*/

/*
 * Object Types - these are used in the binary object
 * representations and in the debug library. They must
 * be unique.  They are the old system.
 */

#define rwID_DATABASE 0x64617462     /* datb */

#define MAKECHUNKID(vendorID, chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))
#define GETOBJECTID(chunkID) (chunkID & 0xFF)
#define GETVENDORID(chunkID) ((chunkID >> 8) & 0xFFFFFF)

/***
 *** These are the vendor IDs.  A customer must reserve a vendor ID in order
 *** to be able to write toolkits (this prevents clashes between toolkits).
 *** We reserve some for our own use as shown below.  These are all 24 bit.
 ***
 *** IMPORTANT NOTE: DO NOT UNDER ANY CIRCUMSTANCES CHANGE THESE VALUES. IF
 ***                 YOU ARE ADDING A NEW ONE, APPEND IT!
 ***
 *** They must all be unique.
 ***/

enum RwPluginVendor
{
    rwVENDORID_CORE             = 0x000000L,
    rwVENDORID_CRITERIONTK      = 0x000001L,
    rwVENDORID_REDLINERACER     = 0x000002L,
    rwVENDORID_CSLRD            = 0x000003L,
    rwVENDORID_CRITERIONINT     = 0x000004L,
    rwVENDORID_CRITERIONWORLD   = 0x000005L,
    rwVENDORID_BETA             = 0x000006L,
    rwVENDORID_CRITERIONRM      = 0x000007L,
    rwVENDORID_CRITERIONRWA     = 0x000008L, /* RenderWare Audio */
    rwVENDORID_CRITERIONRWP     = 0x000009L, /* RenderWare Physics */
    rwPLUGINVENDORFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPluginVendor RwPluginVendor;

/***
 *** These are the core objects (8 bit IDs).  They must all be unique.
 *** We can get away without using the MAKECHUNKID macro because the
 *** vendor ID in all cases will be zero (rwVENDORID_CORE).
 ***
 *** IMPORTANT NOTE: DO NOT UNDER ANY CIRCUMSTANCES CHANGE THESE VALUES. IF
 ***                 YOU ARE ADDING A NEW ONE, APPEND IT!
 ***/

/* These are the internal ones.  Because the core ID is 0, we can get away without
 * using the MAKECHUNKID macro for the CORE chunks.
 */

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwstream
 * \ref RwCorePluginID
 *
 * A chunk is a block of binary stream data containing objects. These objects
 * are referenced using object IDs which are listed below. Before an object
 * stream is read, a chunk needs to be obtained containing the object ID.
 * These IDs are used with the function \ref RwStreamFindChunk.
 *
 * To write a chunk into a stream \ref RwStreamWriteChunkHeader is used
 * with one of the following object IDs.
 *
 * \note \ref rt2d and \ref rt2danim use different object IDs. These
 * are listed in \ref rt2dobjectids and \ref rt2danimobjectids.
 *
 * \see RwStreamFindChunk
 * \see RwStreamWriteChunkHeader
 */
enum RwCorePluginID
{
    rwID_NAOBJECT               = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT                 = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING                 = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION              = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA                 = MAKECHUNKID(rwVENDORID_CORE, 0x05),
/**< RwCamera chunk. See \ref RwCameraStreamRead */

    rwID_TEXTURE                = MAKECHUNKID(rwVENDORID_CORE, 0x06),
/**< RwTexture chunk. See \ref RwTextureStreamRead */

    rwID_MATERIAL               = MAKECHUNKID(rwVENDORID_CORE, 0x07),
/**< RpMaterial chunk. See \ref RpMaterialStreamRead. */

    rwID_MATLIST                = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT             = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT              = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD                  = MAKECHUNKID(rwVENDORID_CORE, 0x0B),
/**< RpWorld chunk. See \ref RpWorldStreamRead. */

    rwID_SPLINE                 = MAKECHUNKID(rwVENDORID_CORE, 0x0C),
/**< RpSpline chunk. See \ref RpSplineStreamRead */

    rwID_MATRIX                 = MAKECHUNKID(rwVENDORID_CORE, 0x0D),
/**< RwMatrix chunk. See \ref RwMatrixStreamRead */

    rwID_FRAMELIST              = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY               = MAKECHUNKID(rwVENDORID_CORE, 0x0F),
/**< RpGeometry chunk. See \ref RpGeometryStreamRead. */

    rwID_CLUMP                  = MAKECHUNKID(rwVENDORID_CORE, 0x10),
/**< RpClump chunk. See \ref RpClumpStreamRead. */

    rwID_LIGHT                  = MAKECHUNKID(rwVENDORID_CORE, 0x12),
/**< RpLight chunk. See \ref RpLightStreamRead. */

    rwID_UNICODESTRING          = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC                 = MAKECHUNKID(rwVENDORID_CORE, 0x14),
/**< RpAtomic chunk. See \ref RpAtomicStreamRead */

    rwID_TEXTURENATIVE          = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY          = MAKECHUNKID(rwVENDORID_CORE, 0x16),
/**< RwTexDictionary - platform specific texture dictionary.
 * See \ref RwTexDictionaryStreamRead. */

    rwID_ANIMDATABASE           = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE                  = MAKECHUNKID(rwVENDORID_CORE, 0x18),
/**< RwImage chunk. See \ref RwImageStreamRead */

    rwID_SKINANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST           = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION          = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
/**< RtAnimAnimation chunk. See \ref RtAnimAnimationStreamRead. */
    rwID_HANIMANIMATION         = MAKECHUNKID(rwVENDORID_CORE, 0x1B),
/**< RtAnimAnimation chunk. For backwards compatibility. See \ref rwID_ANIMANIMATION. */

    rwID_TEAM                   = MAKECHUNKID(rwVENDORID_CORE, 0x1C),
/**< \ref RpTeam chunk. See \ref RpTeamStreamRead */
    rwID_CROWD                  = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION        = MAKECHUNKID(rwVENDORID_CORE, 0x1E),
/**< RpDMorphAnimation - delta morph animation chunk. See \ref RpDMorphAnimationStreamRead */

    rwID_RIGHTTORENDER          = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE         = MAKECHUNKID(rwVENDORID_CORE, 0x20),
/**< \if xbox RpMTEffect - multi-texture effect chunk. See \ref RpMTEffectStreamRead \endif */
/**< \if gcn  RpMTEffect - multi-texture effect chunk. See \ref RpMTEffectStreamRead \endif */

    rwID_MTEFFECTDICT           = MAKECHUNKID(rwVENDORID_CORE, 0x21),
/**< \if xbox RpMTEffectDict - multi-texture effect dictionary chunk. See \ref RpMTEffectDictStreamRead \endif */
/**< \if gcn  RpMTEffectDict - multi-texture effect dictionary chunk. See \ref RpMTEffectDictStreamRead \endif */

    rwID_TEAMDICTIONARY         = MAKECHUNKID(rwVENDORID_CORE, 0x22),
/**< \ref RpTeamDictionary chunk. See \ref RpTeamDictionaryStreamRead */

    rwID_PITEXDICTIONARY        = MAKECHUNKID(rwVENDORID_CORE, 0x23),
/**< RwTexDictionary - platform independent texture dictionary. See \ref RtPITexDictionaryStreamRead. */

    rwID_TOC                    = MAKECHUNKID(rwVENDORID_CORE, 0x24),
/**< RtTOC chunk. See \ref RtTOCStreamRead */

    rwID_PRTSTDGLOBALDATA       = MAKECHUNKID(rwVENDORID_CORE, 0x25),
/**< RpPrtStdEmitterClass, RpPrtStdParticleClass and RpPrtStdPropertyTable chunks.
 *   See \ref RpPrtStdEClassStreamRead, \ref RpPrtStdPClassStreamRead
 *   \ref RpPrtStdPropTabStreamRead and \ref RpPrtStdGlobalDataStreamRead */

    rwID_ALTPIPE                = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS                 = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH              = MAKECHUNKID(rwVENDORID_CORE, 0x28),
/**< RpPatchMesh chunk. See \ref RpPatchMeshStreamRead */

    rwID_CHUNKGROUPSTART        = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND          = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT             = MAKECHUNKID(rwVENDORID_CORE, 0x2B),
/**< UV anim dictionary chunk. See \ref RpUVAnimGetDictSchema */

    rwID_COLLTREE               = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

/* Insert before MAX and increment MAX */
    rwID_COREPLUGINIDMAX        = MAKECHUNKID(rwVENDORID_CORE, 0x2D),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
#endif /* RWADOXYGENEXTERNAL */

/***
 *** These are the Criterion internal plugin extensions.  Use with rwVENDORID_CRITERIONINT.
 ***
 *** IMPORTANT NOTE: DO NOT UNDER ANY CIRCUMSTANCES CHANGE THESE VALUES. IF
 ***                 YOU ARE ADDING A NEW ONE, APPEND IT!
 ***/

enum RwCriterionPluginID
{
    rwID_COREPLUGIN             = 0x01,
    rwID_WORLDPLUGIN            = 0x02,
    rwID_TOOLPLUGIN             = 0x03,
    rwID_TOOL2PLUGIN            = 0x04,
    rwCRITERIONPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionPluginID RwCriterionPluginID;

/**
 * \ingroup rwengine
 * The Core Library is consisted from a collection of plugins that constitute
 * it. This is a list of the pluginIDs that belong to the Core. See
 * \ref RwEngineRegisterPlugin
 */
enum RwCriterionCoreID
{
    /* Guard value that should not be used. */
    rwID_NACOREID               = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    /* The valid plugin IDs */
    /**< Vector module pluginID. See \ref rwv2d and \ref rwv3d */
    rwID_VECTORMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),
    /**< Matrix module pluginID. See \ref rwmatrix */
    rwID_MATRIXMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),
    /**< Frame module pluginID. See \ref rwframe */
    rwID_FRAMEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),
    /**< Stream module pluginID. See \ref rwstream */
    rwID_STREAMMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),
    /**< Camera module pluginID. See \ref rwcamera */
    rwID_CAMERAMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),
    /**< Image module pluginID. See \ref rwimage */
    rwID_IMAGEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),
    /**< Raster module pluginID. See \ref rwraster */
    rwID_RASTERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),
    /**< Texture module pluginID. See \ref rwtexture */
    rwID_TEXTUREMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),
    /**< Pipeline module pluginID. See \ref RxPipeline */
    rwID_PIPEMODULE             = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),
    /**< Immediate module pluginID. See \ref rwim3d */
    rwID_IMMEDIATEMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),
    /**< Resource module pluginID. See \ref rwresources */
    rwID_RESOURCESMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),
    /**< Device module pluginID */
    rwID_DEVICEMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),
    /**< Color module pluginID. See \ref rwrgba */
    rwID_COLORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),
    /* Not used */
    rwID_POLYPIPEMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),
    /**< Error module pluginID. See \ref rwerror */
    rwID_ERRORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),
    /**< Metrics module pluginID. See \ref RwMetrics */
    rwID_METRICSMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),
    /**< Driver module pluginID */
    rwID_DRIVERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),
    /**< Chunk group module pluginID. See \ref rwchunkgroup */
    rwID_CHUNKGROUPMODULE       = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;

/***
 *** These are the Criterion internal platform identifies.
 ***
 *** IMPORTANT NOTE: DO NOT UNDER ANY CIRCUMSTANCES CHANGE THESE VALUES. IF
 ***                 YOU ARE ADDING A NEW ONE, APPEND IT!
 ***/
enum RwPlatformID
{
    rwID_PCD3D7 = 1,
    rwID_PCOGL,
    rwID_MAC,
    rwID_PS2,
    rwID_XBOX,
    rwID_GAMECUBE,
    rwID_SOFTRAS,
    rwID_PCD3D8,
    rwID_PCD3D9,
    rwPLATFROMIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPlatformID RwPlatformID;


/****************************************************************************
 Global Types
 */

typedef struct RwObject RwObject;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwobject
 * \struct RwObject
 * This should be considered an opaque type. Use
 * the RwObject API functions to access.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwObject
{
        RwUInt8 type;                /**< Internal Use */
        RwUInt8 subType;             /**< Internal Use */
        RwUInt8 flags;               /**< Internal Use */
        RwUInt8 privateFlags;        /**< Internal Use */
        void   *parent;              /**< Internal Use */
                                     /* Often a Frame  */
};
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwobject
 * \ref RwObjectCallBack
 * callback function supplied for object callback functions.
 *
 * \param  object   Pointer to the current object, supplied by
 *                  iterator.
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Pointer to the current object
 *
 * \see RwFrameForAllObjects
 *
 */
#endif /* RWADOXYGENEXTERNAL */
typedef RwObject *(*RwObjectCallBack)(RwObject *object, void *data);

/****************************************************************************
Function prototypes
*/

    /* TYPE METHODS */

    /* Creation/cloning */

#define rwObjectCopy(d,s)                               \
MACRO_START                                             \
{                                                       \
    ((RwObject *)(d))->type =                           \
        ((const RwObject *)(s))->type;                  \
    ((RwObject *)(d))->subType =                        \
        ((const RwObject *)(s))->subType;               \
    ((RwObject *)(d))->flags =                          \
        ((const RwObject *)(s))->flags;                 \
    ((RwObject *)(d))->privateFlags =                   \
        ((const RwObject *)(s))->privateFlags;          \
    ((RwObject *)(d))->parent =                         \
        NULL;                                           \
}                                                       \
MACRO_STOP

#define rwObjectInitialize(o, t, s)                     \
MACRO_START                                             \
{                                                       \
    ((RwObject *)(o))->type = (RwUInt8)(t);             \
    ((RwObject *)(o))->subType = (RwUInt8)(s);          \
    ((RwObject *)(o))->flags = 0;                       \
    ((RwObject *)(o))->privateFlags = 0;                \
    ((RwObject *)(o))->parent = NULL;                   \
}                                                       \
MACRO_STOP

    /* Debug */
#define RwObjectGetType(o)                  (((const RwObject *)(o))->type)

#define rwObjectSetType(o, t)               (((RwObject *)(o))->type) = (RwUInt8)(t)

    /* Sub type */
#define rwObjectGetSubType(o)               (((const RwObject *)(o))->subType)
#define rwObjectSetSubType(o, t)            (((RwObject *)(o))->subType) = (RwUInt8)(t)

    /* Flags */
#define rwObjectGetFlags(o)                 (((const RwObject *)(o))->flags)
#define rwObjectSetFlags(o, f)              (((RwObject *)(o))->flags) = (RwUInt8)(f)
#define rwObjectTestFlags(o, f)             ((((const RwObject *)(o))->flags) & (RwUInt8)(f))

    /* Private flags */
#define rwObjectGetPrivateFlags(c)          (((const RwObject *)(c))->privateFlags)
#define rwObjectSetPrivateFlags(c,f)        (((RwObject *)(c))->privateFlags) = (RwUInt8)(f)
#define rwObjectTestPrivateFlags(c,flag)    ((((const RwObject *)(c))->privateFlags) & (RwUInt8)(flag))

    /* Hierarchy */
#define rwObjectGetParent(object)           (((const RwObject *)(object))->parent)
#define rwObjectSetParent(c,p)              (((RwObject *)(c))->parent) = (void *)(p)

/*--- Automatically derived from: C:/daily/rwsdk/os/win/osintf.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/rwdbgerr.h ---*/
#define RWECODE(a,b) a,

/* Construct an enum type with all the plugin error codes (for the app to use) */
enum RwErrorCodePlugin_errcore 
{
#include "errcore.def"
    rwLASTERROR_errcore = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodePlugin_errcore RwErrorCodePlugin_errcore;


#undef RWECODE


/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/resmem.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bamemory.h ---*/
#if (defined(RWMEMDEBUG))
#ifdef _XBOX
/* Need OutputDebugString macros */
#include <xtl.h>
#endif
#endif

/****************************************************************************
 Defines
 */

/*
 * Debug fill bytes for compatibility with MSVC/C++ debug heap
 * See
 * \Program Files\Microsoft Visual Studio\VC98\CRT\SRC\DBGHEAP.C:
 * static unsigned char _bNoMansLandFill = 0xFD;
 *   // fill no-man's land with this
 * static unsigned char _bDeadLandFill   = 0xDD;
 *   // fill free objects with this
 * static unsigned char _bCleanLandFill  = 0xCD;
 *   // fill new objects with this
 */

#if (!defined(rwFREELISTNOMANSLANDFILL))
#define rwFREELISTNOMANSLANDFILL    0xFD
#endif /* (!defined(rwFREELISTNOMANSLANDFILL)) */

#if (!defined(rwFREELISTDEADLANDFILL))
#define rwFREELISTDEADLANDFILL      0xDD
#endif /* (!defined(rwFREELISTDEADLANDFILL)) */

#if (!defined(rwFREELISTCLEANLANDFILL))
#define rwFREELISTCLEANLANDFILL     0xCD
#endif /* (!defined(rwFREELISTCLEANLANDFILL)) */

#define RWFREELISTALIGNED(_pData, _freelist) \
  (! (((RwUInt32)(_pData)) & ((_freelist)->alignment - 1)) )

/*****************************
 * REGULAR MEMORY ALLOCATION *
 *****************************/

/**
 * \ingroup memoryfileinterface
 * \def RwMalloc
 * RwMalloc(_s, _h) is a macro that calls the currently registered RwMalloc function.
 * You can access all registered memory functions through an
 * \ref RwOsGetMemoryInterface() call which returns a pointer to an
 * \ref RwMemoryFunctions structure. You can also override the default
 * RenderWare Graphics functions by passing a structure populated with
 * your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwFree
 * RwFree(_p) is a macro that calls the currently registered RwFree function.
 * You can access all registered memory functions through an
 * \ref RwOsGetMemoryInterface() call which returns a pointer to an
 * \ref RwMemoryFunctions structure. You can also override the default
 * RenderWare Graphics functions by passing a structure populated with
 * your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwCalloc
 * RwCalloc(_n, _s, _h) is a macro that calls the currently registered RwCalloc
 * function. You can access all registered memory functions
 * through an \ref RwOsGetMemoryInterface() call which returns a pointer
 * to an \ref RwMemoryFunctions structure. You can also override the
 * default RenderWare Graphics functions by passing a structure populated
 * with your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwRealloc
 * RwRealloc(_p, _s, _h) is a macro that calls the currently registered RwRealloc
 * function. You can access all registered memory functions
 * through an \ref RwOsGetMemoryInterface() call which returns a pointer
 * to an \ref RwMemoryFunctions structure. You can also override the
 * default RenderWare Graphics functions by passing a structure populated
 * with your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

#if ( (defined(RWMEMDEBUG)) && defined(RWDEBUG) )

#if (!defined(RWNOFREELISTS))
#define RWNOFREELISTS
#endif /* (!defined(RWNOFREELISTS)) */

#if (defined(rwPLUGIN_ID))
#define _CLIENT_TAG \
    ( 0xFFFF & (rwPLUGIN_ID) )
#endif /* (defined(rwPLUGIN_ID)) */

#if (!defined(_CLIENT_TAG))
#define _CLIENT_TAG \
    ( 0xFFFF & (MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x00) ) )
#endif /* (!defined(_CLIENT_TAG)) */

# if (defined(_MSC_VER))
#  if ((_MSC_VER>=1000) && defined(_DEBUG))

#if (defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC))
#define _CRTDBG_MAP_ALLOC
#endif /* defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC)) */
#include <crtdbg.h>

#define RwMalloc(_s, _h)                                \
    _malloc_dbg((_s),                                   \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define RwFree(_p)                                      \
   _free_dbg((_p),                                      \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16))

#define RwCalloc(_n, _s, _h)                            \
   _calloc_dbg((_n), (_s),                              \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define RwRealloc(_p, _s, _h)                           \
   _realloc_dbg((_p),                                   \
                (_s),                                   \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define     RWCRTORDBGFLAG(_flag)                       \
    do                                                  \
    {                                                   \
        int            _DbgFlag;                        \
                                                        \
        _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
        _DbgFlag |= (_flag);                            \
        _CrtSetDbgFlag(_DbgFlag);                       \
    }   while(0)

#define VALID_HEAP_STR \
  __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): valid heap\n"

#define     RWCRTCHECKMEMORY()                          \
    do                                                  \
    {                                                   \
        int             valid_heap;                     \
                                                        \
        valid_heap = _CrtCheckMemory();                 \
        _ASSERTE(valid_heap);                           \
    }   while(0)

/*
 *      if (valid_heap)                                 \
 *          OutputDebugString(VALID_HEAP_STR);          \
 */

#define NO_LEAKS_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap leaks found\n"

#define     RWCRTDUMPMEMORYLEAKS()                      \
    do                                                  \
    {                                                   \
        int             leaks_found;                    \
                                                        \
        leaks_found = _CrtDumpMemoryLeaks();            \
        _ASSERTE(!leaks_found);                         \
        if (!leaks_found)                               \
            OutputDebugString(NO_LEAKS_FOUND_STR);      \
    }   while(0)

#define HEAP_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): heap differences found\n"

#define NO_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap differences found\n"

#define RWCRTHEAPDIFFERENCESINCE(_Then)                                   \
    do                                                                    \
    {                                                                     \
        /* only dump differences when                                     \
         * there are in fact differences */                               \
        _CrtMemState _Now;                                                \
        _CrtMemState _Delta;                                              \
        const int _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);         \
        int Differences;                                                  \
                                                                          \
        _CrtMemCheckpoint(&_Now);                                         \
        _CrtMemDifference(&_Delta, _Then, &_Now);                         \
                                                                          \
        (Differences) = ( ( 0 != _Delta.lCounts[_CLIENT_BLOCK] ) ||       \
                          ( 0 != _Delta.lCounts[_NORMAL_BLOCK] ) ||       \
                          ( (_DbgFlag & _CRTDBG_CHECK_CRT_DF) &&          \
                            ( 0 != _Delta.lCounts[_CRT_BLOCK]) ) );       \
                                                                          \
        if ( (Differences) )                                              \
        {                                                                 \
            /* difference detected: dump objects since _Then. */          \
            OutputDebugString(HEAP_DIFFERENCES_FOUND_STR);                \
            _CrtMemDumpAllObjectsSince(_Then);                            \
            _CrtMemDumpStatistics(&_Delta);                               \
        }                                                                 \
        else                                                              \
        {                                                                 \
            OutputDebugString(NO_DIFFERENCES_FOUND_STR);                  \
        }                                                                 \
    }   while (0)

#define RWCRTDBGBREAK() \
    _CrtDbgBreak()

#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) \
    _CrtDoForAllClientObjects(_f, _c)

#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l) \
    _CrtIsMemoryBlock(_p, _t, _r, _f, _l)

#define RWCRTISVALIDHEAPPOINTER(_p) \
    _CrtIsValidHeapPointer(_p)

#define RWCRTISVALIDPOINTER(_p, _n, _r) \
    _CrtIsValidPointer(_p, _n, _r)

#define RWCRTMEMCHECKPOINT(_s) \
    _CrtMemCheckpoint(_s)

#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) \
    _CrtMemDifference(_s1, _s2, _s3)

#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) \
    _CrtMemDumpAllObjectsSince(_s)

#define RWCRTMEMDUMPSTATISTICS(_s) \
    _CrtMemDumpStatistics(_s)

#define RWCRTSETALLOCHOOK(_f) \
    _CrtSetAllocHook(_f)

#define RWCRTSETBREAKALLOC(_a) \
    _CrtSetBreakAlloc(_a)

#define RWCRTSETDBGFLAG(_f) \
    _CrtSetDbgFlag(_f)

#define RWCRTSETDUMPCLIENT(_f) \
    _CrtSetDumpClient(_f)

#define RWCRTSETREPORTFILE(_t, _f) \
    _CrtSetReportFile(_t, _f)

#define RWCRTSETREPORTHOOK(_f) \
    _CrtSetReportHook(_f)

#define RWCRTSETREPORTMODE(_t, _f) \
    _CrtSetReportMode(_t, _f)

#if (!defined(_CRTDBG_FLAGS))
#define _CRTDBG_FLAGS                                           \
    ( (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF |       \
       _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF) &          \
     ~(_CRTDBG_CHECK_ALWAYS_DF |_CRTDBG_RESERVED_DF) )
#endif /* (!defined(_CRTDBG_FLAGS)) */

#  endif /* ((_MSC_VER>=1000) && defined(_DEBUG)) */
# endif /* (defined(_MSC_VER)) */



#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      ((void *)0xDDDDDDDD)
#endif /* (!defined(rwDEADPTRFILL)) */

#endif /* (defined(RWDEBUG) && (defined(RWMEMDEBUG))) */

#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      (NULL)
#endif /* (!defined(rwDEADPTRFILL)) */

#if (!defined(RwMalloc))
#define RwMalloc(_s, _h)                                    \
    ((RWSRCGLOBAL(memoryFuncs).rwmalloc)((_s), (_h)))
#endif /* (!defined(RwMalloc)) */

#if (!defined(RwFree))
#define RwFree(_p)                                          \
    ((RWSRCGLOBAL(memoryFuncs).rwfree)((_p)))
#endif /* (!defined(RwFree)) */

#if (!defined(RwCalloc))
#define RwCalloc(_n, _s, _h)                                \
    ((RWSRCGLOBAL(memoryFuncs).rwcalloc)((_n), (_s), (_h)))
#endif /* (!defined(RwCalloc)) */

#if (!defined(RwRealloc))
#define RwRealloc(_p, _s, _h)                               \
    ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s), (_h)))
#endif /* (!defined(RwRealloc)) */

#if (!defined(RWCRTORDBGFLAG))
#define     RWCRTORDBGFLAG(_flag) /* No op */
#endif /* (!defined(RWCRTORDBGFLAG)) */

#if (!defined(RWCRTCHECKMEMORY))
#define RWCRTCHECKMEMORY()     /* No Op */
#endif /* (!defined(RWCRTCHECKMEMORY)) */

#if (!defined(RWCRTDBGBREAK))
#define RWCRTDBGBREAK()        /* No Op */
#endif /* (!defined(RWCRTDBGBREAK)) */

#if (!defined(RWCRTDOFORALLCLIENTOBJECTS))
#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) /* No Op */
#endif /* (!defined(RWCRTDOFORALLCLIENTOBJECTS)) */

#if (!defined(RWCRTDUMPMEMORYLEAKS))
#define RWCRTDUMPMEMORYLEAKS() /* No Op */
#endif /* (!defined(RWCRTDUMPMEMORYLEAKS)) */

#if (!defined(RWCRTHEAPDIFFERENCESINCE))
#define RWCRTHEAPDIFFERENCESINCE(_Then)           /* No Op */
#endif /* (!defined(RWCRTHEAPDIFFERENCESINCE)) */

#if (!defined(RWCRTISMEMORYBLOCK))
#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l)    (NULL != (_p))
#endif /* (!defined(RWCRTISMEMORYBLOCK)) */

#if (!defined(RWCRTISVALIDHEAPPOINTER))
#define RWCRTISVALIDHEAPPOINTER(_p)               (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDHEAPPOINTER)) */

#if (!defined(RWCRTISVALIDPOINTER))
#define RWCRTISVALIDPOINTER(_p, _n, _r)           (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDPOINTER)) */

#if (!defined(RWCRTMEMCHECKPOINT))
#define RWCRTMEMCHECKPOINT(_s) /* No Op */
#endif /* (!defined(RWCRTMEMCHECKPOINT)) */

#if (!defined(RWCRTMEMDIFFERENCE))
#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) /* No Op */
#endif /* (!defined(RWCRTMEMDIFFERENCE)) */

#if (!defined(RWCRTMEMDUMPALLOBJECTSSINCE))
#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) /* No Op */
#endif /* (!defined(RWCRTMEMDUMPALLOBJECTSSINCE)) */

#if (!defined(RWCRTMEMDUMPSTATISTICS))
#define RWCRTMEMDUMPSTATISTICS(_s)                (NULL)
#endif /* (!defined(RWCRTMEMDUMPSTATISTICS)) */

#if (!defined(RWCRTSETALLOCHOOK))
#define RWCRTSETALLOCHOOK(_f)                     (NULL)
#endif /* (!defined(RWCRTSETALLOCHOOK)) */

#if (!defined(RWCRTSETBREAKALLOC))
#define RWCRTSETBREAKALLOC(_a)                    (0)
#endif /* (!defined(RWCRTSETBREAKALLOC)) */

#if (!defined(RWCRTSETDBGFLAG))
#define RWCRTSETDBGFLAG(_f)                       (0)
#endif /* (!defined(RWCRTSETDBGFLAG)) */

#if (!defined(RWCRTSETDUMPCLIENT))
#define RWCRTSETDUMPCLIENT(_f)                    (NULL)
#endif /* (!defined(RWCRTSETDUMPCLIENT)) */

#if (!defined(RWCRTSETREPORTFILE))
#define RWCRTSETREPORTFILE(_t, _f)                (NULL)
#endif /* (!defined(RWCRTSETREPORTFILE)) */

#if (!defined(RWCRTSETREPORTHOOK))
#define RWCRTSETREPORTHOOK(_f)                    (NULL)
#endif /* (!defined(RWCRTSETREPORTHOOK)) */

#if (!defined(RWCRTSETREPORTMODE))
#define RWCRTSETREPORTMODE(_t, _f)                (0)
#endif /* (!defined(RWCRTSETREPORTMODE)) */

#if (!defined(RWREGSETBREAKALLOC))
#define RWREGSETBREAKALLOC(_name) /* No op */
#endif /* (!defined(RWREGSETBREAKALLOC)) */

#if (!defined(RWREGSETASSERTPRINT))
#define RWREGSETASSERTPRINT(_name) /* No op */
#endif /* (!defined(RWREGSETASSERTPRINT)) */

#if (!defined(RWGETWINREGDWORD))
#define  RWGETWINREGDWORD(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGDWORD)) */

#if (!defined(RWGETWINREGBINARY))
#define  RWGETWINREGBINARY(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGBINARY)) */

#if (!defined(RWGETWINREGSTRING))
#define  RWGETWINREGSTRING(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGSTRING)) */

#if (!defined(_CRTDBG_FLAGS))
#define  _CRTDBG_FLAGS 0x33
#endif /* (!defined(_CRTDBG_FLAGS)) */

/**
 * \ingroup memoryfileinterface
 * Macro for extracting chunkID from a hint. This is the id of either
 * the object for which a memory request came, or of the module a call
 * for allocation belongs to. Refer to \ref RwCorePluginID for a list of
 * all object IDs and to \ref rwengine, \ref RwCriterionCoreID
 * and \ref RwCriterionWorldID for a list of all module IDs. Refer to
 * RwMemoryFunctions for more details about the hints.
 */
#define  RwMemoryHintGetChunkID(_h) ((_h) & 0xFFFF)
/**
 * \ingroup memoryfileinterface
 * Macro for extracting duration from a hint. Refer to RwMemoryFunctions
 * for more details about the hints.
 * \see RwMemoryHintDuration
 */
#define  RwMemoryHintGetDuration(_h) ((_h) & rwMEMHINTDUR_MASK)
/**
 * \ingroup memoryfileinterface
 * Macro for extracting flags from a hint. Refer to RwMemoryFunctions
 * for more details about the hints.
 * \see RwMemoryHintFlag
 */
#define  RwMemoryHintGetFlags(_h)    ((_h) & rwMEMHINTFLAG_MASK)

/****************************************************************************
 Global Types
 */

/**
 * \ingroup memoryfileinterface
 * This type represents the memory hint flags.
 */
enum RwMemoryHintFlag
{
    /** Indicates that memory being allocated will potentially be
     *  resized with calls to \ref RwRealloc
     */
    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK      = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;

/**
 * \ingroup memoryfileinterface
 * This type represents the duration of a memory.
 */
enum RwMemoryHintDuration
{
    rwMEMHINTDUR_NADURATION = 0x00000000,
    /** Is used for allocating some temporary memory which
     *  will be freed inside the function scope.
     */
    rwMEMHINTDUR_FUNCTION   = 0x00010000,
    /** Is used for per frame memory allocations.
     */
    rwMEMHINTDUR_FRAME      = 0x00020000,
    /** Is used for allocations which persist for longer
     *  than per frame, but are not global. This can be per
     *  level or per event.
     */
    rwMEMHINTDUR_EVENT      = 0x00030000,
    /** Is used for all global allocations, happening on and
     *  before RwEngineStart() and which are freed on and after
     *  RwEngineStop()
     */
    rwMEMHINTDUR_GLOBAL     = 0x00040000,
    rwMEMHINTDUR_MASK       = 0x00FF0000,
    rwMEMHINTDURFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintDuration RwMemoryHintDuration;

/**
 * \ingroup memoryfileinterface
 * \struct RwMemoryFunctions
 * This type represents the memory functions used by RenderWare. The
 * application may override the default functions by passing a
 * RwMemoryFunctions structure populated with a user defined functions
 * to \ref RwEngineInit().
 *
 * All RenderWare Graphics memory-management functions that allocate
 * memory take an additional RwUInt32 parameter which is a memory hint.
 * The main purpose of the hints is to help improve memory management,
 * such as preventing memory fragmentation. To use the memory hints, you
 * should have provided your own memory-management function through the
 * RwEngineInit() function. For example, you might want to have a
 * separate memory heap just for the RwMatrix or RpGeometry objects, or
 * a separate heap for all temporary allocations inside RenderWare
 * Graphics. As always, memory-management is very specific to each
 * application and has to be organized and tuned for each one.

 * A memory hint contains the following information:
 * RwMemoryHintDuration, RwMemoryHintFlag and chunkID. ChunkID is the id
 * of either the object for which a memory request came, or of the module
 * a call for allocation belongs to. Refer to \ref RwCorePluginID for a
 * list of all object IDs and to \ref rwengine, \ref RwCriterionCoreID
 * and \ref RwCriterionWorldID for a list of all module IDs. You can extend
 * all of these fields using specific values for duration, object IDs,
 * flags and create custom hints that can be passed to all the memory
 * allocation calls you make. A tipical call to RwMalloc looks like:
 * \code RwMalloc(size, rwID_MATRIX | rwMEMHINTDUR_FUNCTION);\endcode
 *
 * \note Note that once you override these function you will be responsible
 * for aligning the memory properly. Memory alignment differs between the
 * various platforms and it can also affect the application performance.
 *
 * \see RwMemoryHintGetDuration, RwMemoryHintGetChunkID,
 *      RwMemoryHintGetFlags, RwEngineInit.
 */
struct RwMemoryFunctions
{
    /* c.f.
     * Program Files/Microsoft Visual Studio/VC98/Include/MALLOC.H
     */
    void *(*rwmalloc)(size_t size, RwUInt32 hint);
        /**< Allocates memory blocks.
         *  \param size Number of bytes to allocate. Should be greater
         *         then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void  (*rwfree)(void *mem);
        /**< Deallocates or frees a memory block.
         *  \param mem Previously allocated memory block to be freed.
         *         Shouldn't be NULL pointer.
         */
    void *(*rwrealloc)(void *mem, size_t newSize, RwUInt32 hint);
        /**< Reallocate memory blocks.
         *  \param mem Pointer to previously allocated memory block.
         *  \param size New size in bytes. Should be greater then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void *(*rwcalloc)(size_t numObj, size_t sizeObj, RwUInt32 hint);
        /**< Allocates an array in memory with elements initialized to 0.
         *  \param numObj Non-zero number of elements.
         *  \param sizeObj Non-zero length in bytes of each element.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
};
typedef struct RwMemoryFunctions RwMemoryFunctions;

 /**
 * \ingroup rwfreelist
 * The free list was statically allocated
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_STATIC 0x00000001

/**
 * \ingroup rwfreelist
 * \hideinitializer
 * Free blocks as soon as they are empty.
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_FREEBLOCKS 0x00000002


typedef struct RwFreeList RwFreeList;

/**
 * \ingroup rwfreelist
 * Holds free list info, should be considered opaque. Use API functions to access.
 */
struct RwFreeList
{
    RwUInt32   entrySize;       /**<Size of an entry in the free list. */
#if (defined(RWDEBUG) && !defined(DOXYGEN))
    RwUInt32   nonAlignedEntrySize;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */
    RwUInt32   entriesPerBlock; /**<Number of entries per free list block. */
    RwUInt32   heapSize;        /**<Size of the heap. */
    RwUInt32   alignment;       /**<Alignment of a free list entry. */
    RwLinkList blockList;       /**<List of data blocks. */
    RwUInt32   flags;           /**<Flags which affect the behavior of the
                                    free list. <BR>
                                    rwFREELISTFLAG_FREEBLOCKS */
    RwLLLink   link;            /**<Link to the free list linked list. */

#if (defined(RWDEBUG) && !defined(DOXYGEN))
    const RwChar       *fileCreate;
    RwUInt32            lineCreate;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */
};
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwfreelist
 * \ref RwFreeListCallBack represents
 * the function called from \ref RwFreeListForAllUsed for all used entries in a
 * given free list.
 *
 * \param  pMem   Pointer to the start of the current entries.
 *
 * \param  data   User-defined data pointer.
 *
 * \see RwFreeListForAllUsed
 *
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void        (*RwFreeListCallBack) (void* mem, void *data);
typedef void       *(*RwMemoryAllocFn)    (RwFreeList* fl, RwUInt32 hint);
typedef RwFreeList *(*RwMemoryFreeFn)     (RwFreeList* fl, void *data);

#if (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS))

#if ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__))

/*
 * for more on memalign, see
 * http://www.gnu.org/manual/glibc-2.0.6/html_chapter/libc_3.html#SEC28
 */
#include <rtdbmalloc.h>

#define RwFreeListAlloc(_f, _h)     \
    memalign(((_f)->alignment),  (_f)->entrySize)

#else /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListAlloc(_f, _h)     \
    RwMalloc(((_f)->entrySize), (_h))

#endif /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListFree(_f, _p)  \
    RwFree((_p))

#endif /* (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS)) */

#if (!defined(RwFreeListAlloc))
#define RwFreeListAlloc(_f, _h)     \
    RWSRCGLOBAL(memoryAlloc)((_f), (_h))
#endif /* (!defined(RwFreeListAlloc)) */

#if (!defined(RwFreeListFree))
#define RwFreeListFree(_f, _p)  \
    RWSRCGLOBAL(memoryFree)(_f, _p)
#endif /* (!defined(RwFreeListFree)) */


/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bastream.h ---*/

/****************************************************************************
 Defines
 */

#define rwSTREAMSTACKSIZE    512

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwstream
 * \ref RwStreamType 
 * This type represents the different types of stream that 
 * can be used. 
 * See API section \ref rwstream 
 */
enum RwStreamType
{
    rwNASTREAM = 0,     /**<Invalid stream type */
    rwSTREAMFILE,       /**<File */
    rwSTREAMFILENAME,   /**<File name */
    rwSTREAMMEMORY,     /**<Memory*/
    rwSTREAMCUSTOM,     /**<Custom */
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;

/**
 * \ingroup rwstream
 * \ref RwStreamAccessType 
 * This type represents the options available for 
 * accessing a stream when it is opened.
 * See API section \ref rwstream */
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,   /**<Invalid stream access */
    rwSTREAMREAD,           /**<Read */
    rwSTREAMWRITE,          /**<Write */
    rwSTREAMAPPEND,         /**<Append */
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;

/* Memory stream */
/**
 * \ingroup rwstream
 * \struct RwStreamMemory
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStreamMemory RwStreamMemory;
#if (!defined(DOXYGEN))
struct RwStreamMemory
{
    RwUInt32            position; /* Current 'memory' position 0 is first byte */
    RwUInt32            nSize;  /* Space allocated currently */
    RwUInt8            *memBlock; /* Current memory block pointer */
};
#endif /* (!defined(DOXYGEN)) */


typedef union RwStreamFile RwStreamFile;
/**
 * \ingroup rwstream
 * \union RwStreamFile 
 * This type is used to represent a file pointer for
 * accessing data on disk through the stream mechanism.
 * See API section \ref rwstream. */
union RwStreamFile
{
    void    *fpFile;               /**< file pointer */
    const void    *constfpFile;    /**< const file pointer */
};


/* Custom stream function pointer types */
typedef             RwBool(*rwCustomStreamFnClose) (void *data);
typedef             RwUInt32(*rwCustomStreamFnRead) (void *data, void *buffer,
                                                     RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnWrite) (void *data,
                                                    const void *buffer,

                                                    RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnSkip) (void *data,

                                                   RwUInt32 offset);

/* Custom stream */
/**
 * \ingroup rwstream
 * \struct  RwStreamCustom
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStreamCustom RwStreamCustom;
#if (!defined(DOXYGEN))
struct RwStreamCustom
{
    rwCustomStreamFnClose sfnclose;
    rwCustomStreamFnRead sfnread;
    rwCustomStreamFnWrite sfnwrite;
    rwCustomStreamFnSkip sfnskip;
    void               *data;
};
#endif /* (!defined(DOXYGEN)) */

/* Stream */

typedef union RwStreamUnion RwStreamUnion;
/**
 * \ingroup rwstream
 * \union RwStreamUnion
 * The union of all supported stream types
 */
union RwStreamUnion
{
    RwStreamMemory      memory; /**< memory */
    RwStreamFile        file; /**< file */
    RwStreamCustom      custom; /**< custom */
};

/**
 * \ingroup rwstream
 * \struct RwStream 
 * Binary stream for reading or writing object data. 
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStream RwStream;
#if (!defined(DOXYGEN))
struct RwStream
{
    RwStreamType        type;
    RwStreamAccessType  accessType;
    RwInt32             position;
    RwStreamUnion       Type;
    RwBool              rwOwned;
};
#endif /* (!defined(DOXYGEN)) */


typedef struct RwMemory RwMemory;
/**
 * \ingroup rwstream
 * \struct RwMemory
 * This type represents a block of allocated memory.
 * It is used to specify an area of memory connected to a stream of type
 * rwSTREAMMEMORY.
 * \see \ref rwstream */
struct RwMemory
{
    RwUInt8     *start; /**< Starting address. */
    RwUInt32    length; /**< Length in bytes. */
};

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/rwstring.h ---*/

/****************************************************************************
 Global Types
 */

typedef int (*vecSprintfFunc)(RwChar *buffer,
                              const RwChar *format,
                              ...) /* __RWFORMAT__(printf, 2, 3) */; 
typedef int (*vecVsprintfFunc)(RwChar *buffer,
                               const RwChar *format,
                               va_list argptr); 
typedef RwChar *(*vecStrcpyFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncpyFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrcatFunc)(RwChar *dest,
                                 const RwChar *srce);
typedef RwChar *(*vecStrncatFunc)(RwChar *dest,
                                  const RwChar *srce,
                                  size_t size);
typedef RwChar *(*vecStrrchrFunc)(const RwChar *string,
                                  int findThis);
typedef RwChar *(*vecStrchrFunc)(const RwChar *string,
                                 int findThis);
typedef RwChar *(*vecStrstrFunc)(const RwChar *string,
                                 const RwChar *findThis);
typedef int (*vecStrcmpFunc)(const RwChar *string1,
                             const RwChar *string2);
typedef int (*vecStrncmpFunc)(const RwChar *string1,
                              const RwChar *string2,
                              size_t max_size);
typedef int (*vecStricmpFunc)(const RwChar *string1,
                              const RwChar *string2);
typedef size_t (*vecStrlenFunc)(const RwChar *string);
typedef RwChar *(*vecStruprFunc)(RwChar *string);
typedef RwChar *(*vecStrlwrFunc)(RwChar *string);
typedef RwChar *(*vecStrtokFunc)(RwChar *string, const RwChar *delimit);
typedef int (*vecSscanfFunc)(const RwChar *buffer,
                             const RwChar *format,
                             ...) /* __RWFORMAT__(scanf, 2, 3) */;

typedef struct RwStringFunctions RwStringFunctions;

#if (!defined(DOXYGEN))
struct RwStringFunctions
{
    vecSprintfFunc vecSprintf ;
    vecVsprintfFunc vecVsprintf;
    vecStrcpyFunc vecStrcpy;
    vecStrncpyFunc vecStrncpy;
    vecStrcatFunc vecStrcat;
    vecStrncatFunc vecStrncat;
    vecStrrchrFunc vecStrrchr;
    vecStrchrFunc vecStrchr;
    vecStrstrFunc vecStrstr;
    vecStrcmpFunc vecStrcmp;
    vecStrncmpFunc vecStrncmp;
    vecStricmpFunc vecStricmp;
    vecStrlenFunc vecStrlen;
    vecStruprFunc vecStrupr;
    vecStrlwrFunc vecStrlwr;
    vecStrtokFunc vecStrtok;
    vecSscanfFunc vecSscanf;
};
#endif /* (!defined(DOXYGEN)) */

/****************************************************************************
 Defines
 */
#define rwCHUNKGROUPMAXNAME          32

#define rwCHUNKGROUPVERSION          0x01

/****************************************************************************
 Global Types
 */

typedef struct RwChunkGroup RwChunkGroup;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwchunkgroup
 * \struct RwChunkGroup
 * Holds the RenderWare stream group data.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwChunkGroup
{
    RwChar      name[rwCHUNKGROUPMAXNAME + 1];  /**< Name identifier for the group. */
};

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/batkreg.h ---*/

/****************************************************************************
 Global Types
 */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkWriteCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * writes extension data to a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be written to the binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkReadCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * reads extension data from a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be read from a binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkGetSizeCallBack represents the callback
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * determines the binary size of the extension data.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return A RwInt32 value equal to the size in bytes of the plugin extension data.
 */
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkAlwaysCallBack represents the callback
 * registered by \ref RwCameraSetStreamAlwaysCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished (useful to set up plugin data for plugins that found no
 * data in the stream, but that cannot set up the data during the
 * \ref RwPluginObjectConstructor callback).
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkRightsCallBack represents the callback
 * registered by RwCameraSetStreamRightsCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished, and the object finalised, if and only if the object's rights
 * id was equal to that of the plugin registering the call.
 * For convience the extension data is passed to the callback.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \param  extraData     An RwUInt32 writen with the plugin id.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectConstructor represents the callback
 * registered by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc.
 * as the function that initializes either the global extension data (in the
 * case of \ref RwEngineRegisterPlugin) or the object extension data (in all
 * other cases). Registered by \ref RwCameraSetStreamAlwaysCallBack, etc.
 *
 * \param  object   Pointer to the object (global or otherwise)
 * that contains the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the
 * byte offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectCopy represents the callback registered by
 * \ref RwCameraRegisterPlugin, etc. as the function that copies the object
 * extension data when an object is duplicated.
 *
 * \param  dstObject   Pointer to the destination object that will
 * receive the extension data.
 *
 * \param  srcObject   Pointer to the source object containing
 * extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte offset
 * of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectDestructor represents the callback registered
 * by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc. as the
 * function that destroys either the global extension data (in the case of
 * \ref RwEngineRegisterPlugin) or the object extension data (in all other
 * cases).
 *
 * \param  object   Pointer to the object (global or otherwise)
 * containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

typedef void *(*RwPluginErrorStrCallBack)(void *);

typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;

#if (!defined(DOXYGEN))
struct RwPluginRegistry
{
    RwInt32          sizeOfStruct;
    RwInt32          origSizeOfStruct;
    RwInt32          maxSizeOfStruct;
    RwInt32          staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};

struct RwPluginRegEntry
{
    RwInt32         offset;
    RwInt32         size;
    RwUInt32        pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bamatrix.h ---*/

/****************************************************************************
 Defines
 */

#define RWMATRIXALIGNMENT(_mat) \
   (! (((rwMATRIXALIGNMENT)-1) & ((RwUInt32)(_mat))))

#if (defined(RWMATRIXMONITOR))
# if (defined(_MSC_VER))
#  if ((_MSC_VER>=1000) && defined(_DEBUG))

typedef char        MatrixString[1024];

#define RWMATRIXPRINT(_matrix)                             \
MACRO_START                                                \
{                                                          \
    MatrixString        message;                           \
    MatrixString        output;                            \
                                                           \
    if (NULL != (_matrix))                                 \
    {                                                      \
        const RwV3d * const _x = &(_matrix)->right;        \
        const RwV3d * const _y = &(_matrix)->up;           \
        const RwV3d * const _z = &(_matrix)->at;           \
        const RwV3d * const _w = &(_matrix)->pos;          \
                                                           \
        _snprintf(message, sizeof(MatrixString),           \
                  "[ [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"     \
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"     \
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ]\n"     \
                  "  [ %8.4f, %8.4f, %8.4f, %8.4f ] ]\n"   \
                  "  %08x == flags\n",                     \
                  _x->x, _x->y, _x->z, (RwReal) 0,         \
                  _y->x, _y->y, _y->z, (RwReal) 0,         \
                  _z->x, _z->y, _z->z, (RwReal) 0,         \
                  _w->x, _w->y, _w->z, (RwReal) 1,         \
                  (_matrix)->flags);                       \
    }                                                      \
    else                                                   \
    {                                                      \
        _snprintf(message, sizeof(MatrixString),           \
                  "NULL");                                 \
    }                                                      \
                                                           \
    _snprintf(output, sizeof(MatrixString),                \
              "%s(%d): %s [%p] ==\n%s\n",                  \
              __FILE__, __LINE__,                          \
              #_matrix, _matrix, message);                 \
                                                           \
    OutputDebugString(RWSTRING(output));                   \
}                                                          \
MACRO_STOP

#  endif /* ((_MSC_VER>=1000) && defined(_DEBUG)) */
# endif /* (defined(_MSC_VER)) */
#endif /* (defined(RWMATRIXMONITOR)) */

#if (!(defined(RWMATRIXPRINT)))
#define RWMATRIXPRINT(_matrix) /* No op */
#endif /* (!(defined(RWMATRIXPRINT))) */

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwmatrix
 * enum RwOpCombineType 
 * This type represents a combination operator which 
 * can be applied to frames and matrices.
 * The operator determines the order 
 * in which one object is combined with another 
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwOpCombineType
{
    rwCOMBINEREPLACE = 0,   /**<Replace - 
                                all previous transformations are lost */
    rwCOMBINEPRECONCAT,     /**<Pre-concatenation - 
                                the given transformation is applied 
                                before all others */
    rwCOMBINEPOSTCONCAT,    /**<Post-concatenation - 
                                the given transformation is applied 
                                after all others */
    rwOPCOMBINETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/*
 * RwOpCombineType typedef for enum RwOpCombineType
 */
typedef enum RwOpCombineType RwOpCombineType;

/* External flags (bits 0-15) */

/* Internal flags (bits 16-31) */
enum RwMatrixType
{
    rwMATRIXTYPENORMAL = 0x00000001,
    rwMATRIXTYPEORTHOGONAL = 0x00000002,
    rwMATRIXTYPEORTHONORMAL = 0x00000003,
    rwMATRIXTYPEMASK = 0x00000003,
    rwMATRIXTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixType RwMatrixType;

enum RwMatrixFlag
{
    rwMATRIXINTERNALIDENTITY = 0x00020000,
    rwMATRIXFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixFlag RwMatrixFlag;

/* Flags describing what will optimize for */
enum RwMatrixOptimizations
{
    rwMATRIXOPTIMIZE_IDENTITY = 0x00020000,
    rwMATRIXOPTIMIZATIONSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixOptimizations RwMatrixOptimizations;

/****************************************************************************
 Global Types
 */

#if (!defined(DOXYGEN))
struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};

typedef RwMatrixTag RwMatrix;

#else

/*
 * RwMatrix must appear as the struct name & the typedef,
 * otherwise Doxygen puts RwMatrixTag in the API ref instead of RwMatrix!
 */

/**
 * \ingroup rwmatrix
 * \struct RwMatrix
 * Matrix to define transformations. 
 * This should be considered an opaque type.
 * Use the RwMatrix API functions to access.
 */
#endif /* (!defined(DOXYGEN)) */

#define rwMatrixSetFlags(m, flagsbit)     ((m)->flags = (flagsbit))
#define rwMatrixGetFlags(m)               ((m)->flags)
#define rwMatrixTestFlags(m, flagsbit)    ((m)->flags & (RwInt32)(flagsbit))

#if (!defined(RwMatrixCopyMacro))
#define RwMatrixCopyMacro(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwMatrixCopyMacro)) */

#if (!defined(RwMatrixSetIdentityMacro))
#define RwMatrixSetIdentityMacro(m)                                     \
MACRO_START                                                             \
{                                                                       \
    (m)->right.x = (m)->up.y    = (m)->at.z    = (RwReal)((1.0));       \
    (m)->right.y = (m)->right.z = (m)->up.x    = (RwReal)((0.0));       \
    (m)->up.z    = (m)->at.x    = (m)->at.y    = (RwReal)((0.0));       \
    (m)->pos.x   = (m)->pos.y   = (m)->pos.z   = (RwReal)((0.0));       \
    rwMatrixSetFlags((m),                                               \
                     rwMatrixGetFlags(m) |                              \
                     (rwMATRIXINTERNALIDENTITY |                        \
                      rwMATRIXTYPEORTHONORMAL));                        \
}                                                                       \
MACRO_STOP
#endif /* (!defined(RwMatrixSetIdentityMacro)) */

#define RwMatrixCopy(dst, src)   RwMatrixCopyMacro(dst, src)
#define RwMatrixSetIdentity(m)   RwMatrixSetIdentityMacro(m)

typedef void (RWASMCALL * rwMatrixMultFn) (RwMatrix * dstMat,
                                           const RwMatrix * matA,
                                           const RwMatrix * matB);

typedef struct RwMatrixTolerance RwMatrixTolerance;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwmatrix
 * \struct RwMatrixTolerance
 * Holds tolerances for matrix optimizations with \ref RwMatrixOptimize
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwMatrixTolerance
{
    RwReal              Normal;
        /**< Tolerance within which matrix is deemed to be normal   */
    RwReal              Orthogonal;
        /**< Tolerance within which matrix is deemed to be orthogonal */
    RwReal              Identity;
        /**< Tolerance within which matrix is deemed to be identity */
};

/* Get components */
#define RwMatrixGetRight(m)    (&(m)->right)
#define RwMatrixGetUp(m)       (&(m)->up)
#define RwMatrixGetAt(m)       (&(m)->at)
#define RwMatrixGetPos(m)      (&(m)->pos)

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/drvmodel.h ---*/
#ifndef D3D9_DRVMODEL_H
#define D3D9_DRVMODEL_H

#if (defined(__ICL))
/* Avoid voluminous
 *   'warning #344: typedef name has already been declared (with same type)'
 * warnings from MS include files
 */
#pragma warning( disable : 344 )
#endif /* (defined(__ICL)) */


#if (defined(RWDEBUG))
#if (defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC))
#define _CRTDBG_MAP_ALLOC
#endif /* defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC)) */
#include <crtdbg.h>
#define ERR_WRAP(A) (_rwRePrintErrorDDD3D((A), __FILE__, __LINE__))
#endif /* (defined(RWDEBUG)) */

#if (!defined(ERR_WRAP))
#define ERR_WRAP(A) (A)
#endif /* (!defined(ERR_WRAP)) */

/****************************************************************************
 Defines
 */

/* Set true depth information (for fogging, eg) */
#define RwIm2DVertexSetCameraX(vert, camx)          /* Nothing */
#define RwIm2DVertexSetCameraY(vert, camy)          /* Nothing */
#define RwIm2DVertexSetCameraZ(vert, camz)          /* Nothing */

#define RwIm2DVertexSetRecipCameraZ(vert, recipz)   ((vert)->rhw = recipz)

#define RwIm2DVertexGetCameraX(vert)                (cause an error)
#define RwIm2DVertexGetCameraY(vert)                (cause an error)
#define RwIm2DVertexGetCameraZ(vert)                (cause an error)
#define RwIm2DVertexGetRecipCameraZ(vert)           ((vert)->rhw)

/* Set screen space coordinates in a device vertex */
#define RwIm2DVertexSetScreenX(vert, scrnx)         ((vert)->x = (scrnx))
#define RwIm2DVertexSetScreenY(vert, scrny)         ((vert)->y = (scrny))
#define RwIm2DVertexSetScreenZ(vert, scrnz)         ((vert)->z = (scrnz))
#define RwIm2DVertexGetScreenX(vert)                ((vert)->x)
#define RwIm2DVertexGetScreenY(vert)                ((vert)->y)
#define RwIm2DVertexGetScreenZ(vert)                ((vert)->z)

/* Set texture coordinates in a device vertex */
#define RwIm2DVertexSetU(vert, texU, recipz)        ((vert)->u = (texU))
#define RwIm2DVertexSetV(vert, texV, recipz)        ((vert)->v = (texV))
#define RwIm2DVertexGetU(vert)                      ((vert)->u)
#define RwIm2DVertexGetV(vert)                      ((vert)->v)

/* Modify the luminance stuff */
#define RwIm2DVertexSetRealRGBA(vert, red, green, blue, alpha)  \
    ((vert)->emissiveColor =                                    \
     (((RwFastRealToUInt32(alpha)) << 24) |                        \
      ((RwFastRealToUInt32(red)) << 16) |                          \
      ((RwFastRealToUInt32(green)) << 8) |                         \
      ((RwFastRealToUInt32(blue)))))

#define RwIm2DVertexSetIntRGBA(vert, red, green, blue, alpha)   \
    ((vert)->emissiveColor =                                    \
     ((((RwUInt32)(alpha)) << 24) |                             \
      (((RwUInt32)(red)) << 16) |                               \
      (((RwUInt32)(green)) << 8) |                              \
      (((RwUInt32)(blue)))))

#define RwIm2DVertexGetRed(vert)    \
    (((vert)->emissiveColor >> 16) & 0xFF)

#define RwIm2DVertexGetGreen(vert)  \
    (((vert)->emissiveColor >> 8) & 0xFF)

#define RwIm2DVertexGetBlue(vert)   \
    ((vert)->emissiveColor & 0xFF)

#define RwIm2DVertexGetAlpha(vert)  \
    (((vert)->emissiveColor >> 24) & 0xFF)

#define RwIm2DVertexCopyRGBA(dst, src)  \
    ((dst)->emissiveColor = (src)->emissiveColor)

/* Clipper stuff */

#define RwIm2DVertexClipRGBA(o, i, n, f)                                \
MACRO_START                                                             \
{                                                                       \
    const RwInt32        _factor =                                      \
        (RwFastRealToUInt32(i * (RwReal)(255))) & 255;                  \
                                                                        \
    (o)->emissiveColor =                                                \
        (((((RwIm2DVertexGetAlpha(f) - RwIm2DVertexGetAlpha(n)) *       \
            _factor) >> 8) + RwIm2DVertexGetAlpha(n)) << 24) |          \
        (((((RwIm2DVertexGetRed(f) - RwIm2DVertexGetRed(n)) *           \
            _factor) >> 8) + RwIm2DVertexGetRed(n)) << 16) |            \
        (((((RwIm2DVertexGetGreen(f) - RwIm2DVertexGetGreen(n)) *       \
            _factor) >> 8) + RwIm2DVertexGetGreen(n)) << 8) |           \
        (((((RwIm2DVertexGetBlue(f) - RwIm2DVertexGetBlue(n)) *         \
            _factor) >> 8) + RwIm2DVertexGetBlue(n)));                  \
}                                                                       \
MACRO_STOP

/****************************************************************************
 Global Types
 */

/* We use RwD3D9Vertex to drive the hardware in 2D mode */

/*
 * D3D9 vertex structure definition for 2D geometry
 */
#if !defined(RWADOXYGENEXTERNAL)
typedef struct RwD3D9Vertex RwD3D9Vertex;
/**
 * \ingroup rwcoredriverd3d9
 * \struct RwD3D9Vertex
 * D3D9 vertex structure definition for 2D geometry
 */
struct RwD3D9Vertex
{
    RwReal      x;              /**< Screen X */
    RwReal      y;              /**< Screen Y */
    RwReal      z;              /**< Screen Z */
    RwReal      rhw;            /**< Reciprocal of homogeneous W */

    RwUInt32    emissiveColor;  /**< Vertex color */

    RwReal      u;              /**< Texture coordinate U */
    RwReal      v;              /**< Texture coordinate V */
};
#endif /* !defined(RWADOXYGENEXTERNAL) */

/* Define types used */

#if !defined(RWADOXYGENEXTERNAL)
/**
 * \ingroup rwcoredriverd3d9
 * \ref RwIm2DVertex
 * Typedef for a RenderWare Graphics Immediate Mode 2D Vertex
 */
typedef RwD3D9Vertex    RwIm2DVertex;
#endif /* !defined(RWADOXYGENEXTERNAL) */

#if !defined(RWADOXYGENEXTERNAL)
/**
 * \ingroup rwcoredriverd3d9
 * \ref RxVertexIndex
 *
 * Typedef for a RenderWare Graphics Powe   rPipe Immediate
 * Mode Vertex
 */
typedef RwUInt16        RxVertexIndex;
#endif /* !defined(RWADOXYGENEXTERNAL) */

#if !defined(RWADOXYGENEXTERNAL)
/**
 * \ingroup rwcoredriverd3d9
 * \ref RwImVertexIndex
 * Typedef for a RenderWare Graphics Immediate Mode Vertex.
 */
typedef RxVertexIndex   RwImVertexIndex;
#endif /* !defined(RWADOXYGENEXTERNAL) */

#if !defined(RWADOXYGENEXTERNAL)
/**
 * \ingroup rwcoredriverd3d9
 * \struct RwD3D9Metrics
 * Structure containing metrics counters
 */
typedef struct
{
    RwUInt32    numRenderStateChanges;          /**< Number of Render States changed */
    RwUInt32    numTextureStageStateChanges;    /**< Number of Texture Stage States changed */
    RwUInt32    numSamplerStageStateChanges;    /**< Number of Sampler States changed */
    RwUInt32    numMaterialChanges;             /**< Number of Material changes */
    RwUInt32    numLightsChanged;               /**< Number of Lights changed */
    RwUInt32    numVBSwitches;                  /**< Number of Vertex Buffer switches */
}
RwD3D9Metrics;
#endif /* !defined(RWADOXYGENEXTERNAL) */

#endif /* D3D9_DRVMODEL_H */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bavector.h ---*/

/* RWPUBLIC */
/****************************************************************************
 Defines
 */

 /* If sqrt is overloaded for this platform, we will remove
  * all the sqrt table stuff from the build entirely
  * currently applies to SKY2 and XBOX - IDBS [2/11/2001]
  * [and, if using the intel compiler version 400 or above,
  *  we will use the single-precision float "sqrtf" under
  *  D3D8, OpenGL or SoftRas] */

#if (defined(rwSqrtMacro))
#define RWNOSQRTTABLE
#endif /* (defined(rwSqrtMacro)) */
#if (defined(rwInvSqrtMacro))
#define RWNOINVSQRTTABLE
#endif /* (defined(rwSqrtMacro)) */

#if (!defined(rwSqrtMacro))
#define rwSqrtMacro(_root, _input) \
    ( *(_root) = _rwSqrt(_input) )
#endif /* (!defined(rwSqrtMacro)) */

#if (!defined(rwInvSqrtMacro))
#define rwInvSqrtMacro(_recip, _input) \
    ( *(_recip) = _rwInvSqrt(_input) )
#endif /* (!defined(rwInvSqrtMacro)) */

#if (!defined(rwSqrtInvSqrtMacro))
#define rwSqrtInvSqrtMacro(_root, _recip, _input)       \
MACRO_START                                             \
{                                                       \
    RwReal _tmp = _input;                               \
    rwSqrt((_root), _tmp);                              \
    rwInvSqrt((_recip), _tmp);                          \
}                                                       \
MACRO_STOP
#endif /* (!defined(rwSqrtInvSqrtMacro)) */

/* Vector operations Macros */

#if (!defined(RwV2dAssignMacro))
#define RwV2dAssignMacro(_target, _source)                      \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwV2dAssignMacro)) */

#define RwV2dAddMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) + ( (b)->x));                            \
    (o)->y = (((a)->y) + ( (b)->y));                            \
}                                                               \
MACRO_STOP

#define RwV2dSubMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
}                                                               \
MACRO_STOP

#define RwV2dScaleMacro(o, i, s)                                \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((i)->x) * ( (s)));                               \
    (o)->y = (((i)->y) * ( (s)));                               \
}                                                               \
MACRO_STOP

#define RwV2dDotProductMacro(a,b)                               \
    (( ((((a)->x) * ( (b)->x))) +                               \
      ( (((a)->y) * ( (b)->y)))))

#define _rwV2dNormalizeMacro(_result, _out, _in)                \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV2dDotProductMacro((_in), (_in));        \
    rwInvSqrtMacro(&(_result), length2);                        \
    RwV2dScaleMacro((_out), (_in), (_result));                  \
}                                                               \
MACRO_STOP

#define RwV2dNormalizeMacro(_result, _out, _in)                 \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV2dDotProductMacro((_in), (_in));        \
    RwReal recip;                                               \
                                                                \
    rwSqrtInvSqrtMacro(&(_result), &recip, length2);            \
    RwV2dScaleMacro((_out), (_in), recip);                      \
}                                                               \
MACRO_STOP

#define RwV2dLengthMacro(_result, _in)                          \
MACRO_START                                                     \
{                                                               \
    (_result) = RwV2dDotProductMacro(_in, _in);                 \
    rwSqrtMacro(&(_result), (_result));                         \
}                                                               \
MACRO_STOP

#define RwV2dLineNormalMacro(_o, _a, _b)                        \
MACRO_START                                                     \
{                                                               \
    RwReal recip;                                               \
                                                                \
    (_o)->y = (((_b)->x) - ( (_a)->x));                         \
    (_o)->x = (((_a)->y) - ( (_b)->y));                         \
    _rwV2dNormalizeMacro(recip, _o,_o);                         \
}                                                               \
MACRO_STOP

#define RwV2dPerpMacro(o, a)                                    \
MACRO_START                                                     \
{                                                               \
    (o)->x = -(a)->y;                                           \
    (o)->y = (a)->x;                                            \
}                                                               \
MACRO_STOP

/* RwV3d */

#if (!defined(RwV3dAssignMacro))
#define RwV3dAssignMacro(_target, _source)                     \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwV3dAssignMacro)) */


#define RwV3dAddMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) + ( (b)->x));                            \
    (o)->y = (((a)->y) + ( (b)->y));                            \
    (o)->z = (((a)->z) + ( (b)->z));                            \
}                                                               \
MACRO_STOP

#define RwV3dSubMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
    (o)->z = (((a)->z) - ( (b)->z));                            \
}                                                               \
MACRO_STOP

#define RwV3dScaleMacro(o, a, s)                                \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) * ( (s)));                               \
    (o)->y = (((a)->y) * ( (s)));                               \
    (o)->z = (((a)->z) * ( (s)));                               \
}                                                               \
MACRO_STOP

#define RwV3dIncrementScaledMacro(o, a, s)                      \
MACRO_START                                                     \
{                                                               \
    (o)->x += (((a)->x) * ( (s)));                              \
    (o)->y += (((a)->y) * ( (s)));                              \
    (o)->z += (((a)->z) * ( (s)));                              \
}                                                               \
MACRO_STOP

#define RwV3dNegateMacro(o, a)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = -(a)->x;                                           \
    (o)->y = -(a)->y;                                           \
    (o)->z = -(a)->z;                                           \
}                                                               \
MACRO_STOP

#define RwV3dDotProductMacro(a, b)                              \
    ((((( (((a)->x) * ((b)->x))) +                              \
        ( (((a)->y) * ((b)->y))))) +                            \
        ( (((a)->z) * ((b)->z)))))

#define RwV3dCrossProductMacro(o, a, b)                         \
MACRO_START                                                     \
{                                                               \
    (o)->x =                                                    \
        (( (((a)->y) * ( (b)->z))) -                            \
         ( (((a)->z) * ( (b)->y))));                            \
    (o)->y =                                                    \
        (( (((a)->z) * ( (b)->x))) -                            \
         ( (((a)->x) * ( (b)->z))));                            \
    (o)->z =                                                    \
        (( (((a)->x) * ( (b)->y))) -                            \
         ( (((a)->y) * ( (b)->x))));                            \
}                                                               \
MACRO_STOP

#define _rwV3dNormalizeMacro(_result, _out, _in)                \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV3dDotProductMacro(_in, _in);            \
    rwInvSqrtMacro(&(_result), length2);                        \
    RwV3dScaleMacro(_out, _in, _result);                        \
}                                                               \
MACRO_STOP

#define RwV3dNormalizeMacro(_result, _out, _in)                 \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV3dDotProductMacro((_in), (_in));        \
    RwReal recip;                                               \
                                                                \
    rwSqrtInvSqrtMacro(&(_result), &recip, length2);            \
    RwV3dScaleMacro((_out), (_in), recip);                      \
}                                                               \
MACRO_STOP

#define RwV3dLengthMacro(_result, _in)                          \
MACRO_START                                                     \
{                                                               \
    (_result) = RwV3dDotProductMacro(_in, _in);                 \
    rwSqrtMacro(&(_result), _result);                           \
}                                                               \
MACRO_STOP

#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))

#define RwV2dAssign(o, a)               RwV2dAssignMacro(o, a)
#define RwV2dAdd(o, a, b)               RwV2dAddMacro(o, a, b)
#define RwV2dSub(o, a, b)               RwV2dSubMacro(o, a, b)
#define RwV2dLineNormal(_o, _a, _b)     RwV2dLineNormalMacro(_o, _a, _b)
#define RwV2dScale(o, i, s)             RwV2dScaleMacro(o, i, s)
#define RwV2dDotProduct(a,b)            RwV2dDotProductMacro(a,b)
#define RwV2dPerp(o, a)                 RwV2dPerpMacro(o, a)
#define RwV3dAssign(o, a)               RwV3dAssignMacro(o, a)
#define RwV3dAdd(o, a, b)               RwV3dAddMacro(o, a, b)
#define RwV3dSub(o, a, b)               RwV3dSubMacro(o, a, b)
#define RwV3dScale(o, a, s)             RwV3dScaleMacro(o, a, s)
#define RwV3dIncrementScaled(o, a, s)   RwV3dIncrementScaledMacro(o, a, s)
#define RwV3dNegate(o, a)               RwV3dNegateMacro(o, a)
#define RwV3dDotProduct(a, b)           RwV3dDotProductMacro(a, b)
#define RwV3dCrossProduct(o, a, b)      RwV3dCrossProductMacro(o, a, b)

#endif /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

#define RWRAD2DEG(_x) ((_x) * (((RwReal)180)/(rwPI)))
#define RWDEG2RAD(_x) ((_x) * (rwPI/((RwReal)180)))

#if (!defined(rw4OVERPISQ))
#define rw4OVERPISQ  ( ((RwReal)4) / ( rwPI * rwPI ))
#endif /* (!defined(rw4OVERPISQ)) */

#if (!defined(rwPI3))
#define rwPI3  (rwPI * (RwReal)3)
#endif /* (!defined(rwPI3)) */

#if (!defined(rwPI3OVER2))
#define rwPI3OVER2  ( rwPI3 / (RwReal)2 )
#endif /* (!defined(rwPI3OVER2)) */

#if (!defined(rwPI3OVER8))
#define rwPI3OVER8  (rwPI3 / (RwReal)8 )
#endif /* (!defined(rwPI3OVER8)) */

#define RwQuadSin(_x)                                                       \
    ( rw4OVERPISQ *                                                         \
      ( ( (_x) < 0 ) ?                                                      \
        ( ( rwPI + (_x) ) * (_x) ) :                                        \
        ( ( rwPI - (_x) ) * (_x) ) ) )

#define RwQuadASin(_result, _s)                                             \
    ( rwPIOVER2 * ( ((_s)<0) ?                                              \
                    ( rwSqrtMacro((_result), 1.0f + (_s)) - 1 ) :           \
                    ( 1 - rwSqrtMacro((_result), 1.0f - (_s)) ) ) )

#define RwQuadCos(_x)                                                       \
    ( rw4OVERPISQ *                                                         \
      ( ( (_x) < -rwPIOVER2 ) ?                                             \
        ( ( -rwPI3OVER2 - (_x) ) * ( -rwPIOVER2 - (_x) ) ) :                \
        ( ( (_x) < rwPIOVER2) ?                                             \
          ( ( rwPIOVER2 + (_x) ) * ( rwPIOVER2 - (_x) ) ) :                 \
          ( ( rwPIOVER2 - (_x) ) * ( rwPI3OVER2 - (_x) ) ) ) ) )

#define RwQuadACos(_result, _c)                                             \
    ( rwPIOVER2 * ( ((_c)<0) ?                                              \
                    (2.0f - rwSqrtMacro((_result), 1.0f + (_c))):           \
                    rwSqrtMacro((_result), 1.0f - (_c))) )

#define RwQuadTan(_x)                                                       \
    ( rwPI3 * (_x) / ( rwPI * rwPI - (_x) * (_x) * 4.0f ) )

#define RwQuadATan(_result, _t)                                             \
    ( ( rwSqrtMacro((_result), (rwPI3OVER8 * rwPI3OVER8) +                  \
                    (_t) * (_t) * (rwPIOVER2 * rwPIOVER2) ) - rwPI3OVER8 )  \
      / ( _t) )

#define RwQuadATan2(_result, _s, _c)                                        \
    ( ( rwSqrtMacro((_result), (_c) * (_c) * (rwPI3OVER8 * rwPI3OVER8) +    \
                    (_s) * (_s) * (rwPIOVER2 * rwPIOVER2) )                 \
        - (_c) * rwPI3OVER8 ) / ( _s)  )
/****************************************************************************
 Global Types
 */
#if !defined(RWNOVECMULTFUNCS)
/*
 * Typedef for pointer to Vector multiplication by Matrix function
 */
typedef RwV3d *(*rwVectorMultFn) (RwV3d * pointOut,
                                  const RwV3d * pointIn,
                                  const RwMatrix * matrix);

typedef RwV3d *(*rwVectorsMultFn) (RwV3d * pointsOut,
                                   const RwV3d * pointsIn,
                                   RwInt32 numPoints,
                                   const RwMatrix * matrix);
#endif /* !defined(RWNOVECMULTFUNCS) */

/* Other useful stuff */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/balist.h ---*/
/****************************************************************************
 Global Types
 */

typedef struct RwSList RwSList;

#if (!defined(DOXYGEN))
struct RwSList
{
    RwUInt8    *listElements;
    RwInt32     numElementsFilled;
    RwInt32     numElementsAlloced;
    RwInt32     entrySize;
};
#endif /* (!defined(DOXYGEN) */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/baimmedi.h ---*/

/****************************************************************************
 Defines
 */


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                        Immediate mode interface V2.0

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwRenderState represents the global state variables that control
 * rendering. These may be set and queried using the
 * \ref RwRenderStateSet and \ref RwRenderStateGet functions respectively.
 *
 * Refer to the \ref rwrenderstateoverview for an overview of this system.
 *
 * \note The texture render states (raster, address & filter modes) would
 * normally just be used when rendering in immediate mode and should be
 * specificied completely every time a texture is used. Retained mode
 * pipelines will frequently set theses states internally, usually based on
 * \ref RwTexture objects.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER,
        /**<Raster used for texturing (normally used in immediate mode). 
         *  The value is a pointer to an \ref RwRaster.
         * Default: NULL.
         */
    rwRENDERSTATETEXTUREADDRESS,
        /**<\ref RwTextureAddressMode: wrap, clamp, mirror or border.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSU,
        /**<\ref RwTextureAddressMode in u only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSV,
        /**<\ref RwTextureAddressMode in v only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREPERSPECTIVE,
        /**<Perspective correction on/off (always enabled on many platforms).
         */
    rwRENDERSTATEZTESTENABLE,
        /**<Z-buffer test on/off.
         * Default: TRUE.
         */
    rwRENDERSTATESHADEMODE,
        /**<\ref RwShadeMode: flat or gouraud shading.
         * Default: rwSHADEMODEGOURAUD.
         */
    rwRENDERSTATEZWRITEENABLE,
        /**<Z-buffer write on/off.
         * Default: TRUE.
         */
    rwRENDERSTATETEXTUREFILTER,
        /**<\ref RwTextureFilterMode: point sample, bilinear, trilinear, etc.
         * Default: rwFILTERLINEAR.
         */
    rwRENDERSTATESRCBLEND,
        /**<\ref RwBlendFunction used to modulate the source pixel color
         *  when blending to the frame buffer.
         * Default: rwBLENDSRCALPHA.
         */
    rwRENDERSTATEDESTBLEND,
        /**<\ref RwBlendFunction used to modulate the destination pixel
         *  color in the frame buffer when blending. The resulting pixel
         *  color is given by the formula 
         *  (SRCBLEND * srcColor + DESTBLEND * destColor) for each RGB
         *  component. For a particular platform, not all combinations
         *  of blend function are allowed (see platform specific
         *  restrictions).
         * Default: rwBLENDINVSRCALPHA.
         */
    rwRENDERSTATEVERTEXALPHAENABLE,
        /**<Alpha blending on/off (always enabled on some platforms). 
         *  This is normally used in immediate mode to enable alpha blending 
         *  when vertex colors or texture rasters have transparency. Retained
         *  mode pipelines will usually set this state based on material colors 
         *  and textures.
         * Default: FALSE.
         */
    rwRENDERSTATEBORDERCOLOR,
        /**<Border color for \ref RwTextureAddressMode 
         *  \ref rwTEXTUREADDRESSBORDER. The value should be a packed 
         *  RwUInt32 in a platform specific format. The macro 
         *  RWRGBALONG(r, g, b, a) may be used to construct this using 
         *  8-bit color components.
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGENABLE,
        /**<Fogging on/off (all polygons will be fogged).  
         * Default: FALSE.
         */
    rwRENDERSTATEFOGCOLOR,
        /**<Color used for fogging. The value should be a packed RwUInt32 
         *  in a platform specific format. The macro RWRGBALONG(r, g, b, a)
         *  may be used to construct this using 8-bit color components. 
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGTYPE,
        /**<\ref RwFogType, the type of fogging to use. 
         * Default: rwFOGTYPELINEAR.
         */
    rwRENDERSTATEFOGDENSITY,
        /**<Fog density for \ref RwFogType of 
         *  \ref rwFOGTYPEEXPONENTIAL or \ref rwFOGTYPEEXPONENTIAL2. 
         *  The value should be a pointer to an RwReal in the 
         *  range 0 to 1.  
         * Default: 1.
         */
    rwRENDERSTATECULLMODE = 20,
        /**<\ref RwCullMode, for selecting front/back face culling, or
         *  no culling.
         * Default: rwCULLMODECULLBACK.
         */
    rwRENDERSTATESTENCILENABLE,
        /**<Stenciling on/off.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: FALSE.
         */
    rwRENDERSTATESTENCILFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILZFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes and 
         *  the depth test (z-test) fails. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILPASS,
        /**<\ref RwStencilOperation used when both the stencil and the depth 
         *  (z) tests pass. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILFUNCTION,
        /**<\ref RwStencilFunction for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILFUNCTIONALWAYS.
         */
    rwRENDERSTATESTENCILFUNCTIONREF,
        /**<Integer reference value for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0.
         */
    rwRENDERSTATESTENCILFUNCTIONMASK,
        /**<Mask applied to the reference value and each stencil buffer 
         *  entry to determine the significant bits for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,
        /**<Write mask applied to values written into the stencil buffer. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATEALPHATESTFUNCTION,
        /**<\ref RwAlphaTestFunction for the alpha test. When a pixel fails,
         * neither the frame buffer nor the Z-buffer are updated.
         * Default: rwALPHATESTFUNCTIONGREATER (GameCube, Xbox, D3D8, D3D9
         * and OpenGL). The default PS2 behaviour is to always update the
         * frame buffer and update the Z-buffer only if a greater than or
         * equal test passes.
         */
    rwRENDERSTATEALPHATESTFUNCTIONREF,
        /**<Integer reference value for the alpha test. 
         *  <i> Range is 0 to 255, mapped to the platform's actual range </i>
         * Default: 128 (PS2) 0 (GameCube, Xbox, D3D8, D3D9 and OpenGL).
         */

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRenderState RwRenderState;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwShadeMode represents the available shading modes that may be
 * set using the \ref RwRenderState \ref rwRENDERSTATESHADEMODE.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwShadeMode
{
    rwSHADEMODENASHADEMODE = 0,
    rwSHADEMODEFLAT,                /**<Flat shading */
    rwSHADEMODEGOURAUD,             /**<Gouraud shading */
    rwSHADEMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwShadeMode RwShadeMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwTextureFilterMode represents the texture filtering modes that may
 * be set using the \ref RwRenderState \ref rwRENDERSTATETEXTUREFILTER in
 * immediate mode, or \ref RwTextureSetFilterMode in retained mode.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureFilterMode
{
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,                /**<Point sampled */
    rwFILTERLINEAR,                 /**<Bilinear */
    rwFILTERMIPNEAREST,             /**<Point sampled per pixel mip map */
    rwFILTERMIPLINEAR,              /**<Bilinear per pixel mipmap */
    rwFILTERLINEARMIPNEAREST,       /**<MipMap interp point sampled */
    rwFILTERLINEARMIPLINEAR,        /**<Trilinear */
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureFilterMode RwTextureFilterMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwFogType represents the fog modes available when setting the
 * \ref RwRenderState \ref rwRENDERSTATEFOGTYPE. Note that a particular
 * platform may not support all of these modes (see platform specific
 * restrictions).
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwFogType
{
    rwFOGTYPENAFOGTYPE = 0,
    rwFOGTYPELINEAR,            /**<Linear fog */
    rwFOGTYPEEXPONENTIAL,       /**<Exponential fog */
    rwFOGTYPEEXPONENTIAL2,      /**<Exponential^2 fog */
    rwFOGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFogType RwFogType;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwBlendFunction represents the options available when changing the
 * \ref RwRenderState setting for \ref rwRENDERSTATESRCBLEND and 
 * \ref rwRENDERSTATEDESTBLEND. The values are factors used to modulate
 * either the source or destination pixel color when blending to
 * the frame buffer. Note the some  combinations are not allowed on
 * certain platforms (see platform specific restrictions).
 *
 * In the following list, the factors applied to each color component are 
 * listed explicitly. A subscript s refers to a source value while a subscript 
 * d refers to a destination value. Note that blending in the alpha channel is 
 * only applicable when the frame buffer actually contains alpha, and that
 * the precise operation is platform specific. For instance, the
 * source alpha value might simply be written to the alpha channel with
 * no blending.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwBlendFunction
{
    rwBLENDNABLEND = 0,
    rwBLENDZERO,            /**<(0,    0,    0,    0   ) */
    rwBLENDONE,             /**<(1,    1,    1,    1   ) */
    rwBLENDSRCCOLOR,        /**<(Rs,   Gs,   Bs,   As  ) */
    rwBLENDINVSRCCOLOR,     /**<(1-Rs, 1-Gs, 1-Bs, 1-As) */
    rwBLENDSRCALPHA,        /**<(As,   As,   As,   As  ) */
    rwBLENDINVSRCALPHA,     /**<(1-As, 1-As, 1-As, 1-As) */
    rwBLENDDESTALPHA,       /**<(Ad,   Ad,   Ad,   Ad  ) */
    rwBLENDINVDESTALPHA,    /**<(1-Ad, 1-Ad, 1-Ad, 1-Ad) */
    rwBLENDDESTCOLOR,       /**<(Rd,   Gd,   Bd,   Ad  ) */
    rwBLENDINVDESTCOLOR,    /**<(1-Rd, 1-Gd, 1-Bd, 1-Ad) */
    rwBLENDSRCALPHASAT,     /**<(f,    f,    f,    1   )  f = min (As, 1-Ad) */
    rwBLENDFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwBlendFunction RwBlendFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwTextureAddressMode represents the addressing modes available 
 * when mapping textures to polygons using UV texture coordinates.
 * This may be set in immediate mode via the \ref RwRenderState
 * \ref rwRENDERSTATETEXTUREADDRESS, or via \ref RwTextureSetAddressing
 * for an \ref RwTexture object.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureAddressMode
{
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,      /**<UV wraps (tiles) */
    rwTEXTUREADDRESSMIRROR,    /**<Alternate UV is flipped */
    rwTEXTUREADDRESSCLAMP,     /**<UV is clamped to 0-1 */
    rwTEXTUREADDRESSBORDER,    /**<Border color takes effect outside of 0-1 */
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureAddressMode RwTextureAddressMode;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwStencilOperation represents the stencil operations that may be
 * performed depending on the results of stencil/z-buffer tests.
 * The operation may be set for the various tests 
 * via the \ref RwRenderState settings \ref rwRENDERSTATESTENCILFAIL,
 * \ref rwRENDERSTATESTENCILZFAIL and \ref rwRENDERSTATESTENCILPASS.
 */ 
#endif /* RWADOXYGENEXTERNAL */
enum RwStencilOperation
{
    rwSTENCILOPERATIONNASTENCILOPERATION = 0,

    rwSTENCILOPERATIONKEEP,     
        /**<Do not update the entry in the stencil buffer */
    rwSTENCILOPERATIONZERO,     
        /**<Set the stencil-buffer entry to 0 */
    rwSTENCILOPERATIONREPLACE,  
        /**<Replace the stencil-buffer entry with reference value */
    rwSTENCILOPERATIONINCRSAT,  
        /**<Increment the stencil-buffer entry, clamping to the 
         *  maximum value */
    rwSTENCILOPERATIONDECRSAT,  
        /**<Decrement the stencil-buffer entry, clamping to zero */    
    rwSTENCILOPERATIONINVERT,   
        /**<Invert the bits in the stencil-buffer entry */
    rwSTENCILOPERATIONINCR,     
        /**<Increment the stencil-buffer entry, wrapping to zero if 
         *  the new value exceeds the maximum value */
    rwSTENCILOPERATIONDECR,     
        /**<Decrement the stencil-buffer entry, wrapping to the maximum
         *  value if the new value is less than zero */
 
    rwSTENCILOPERATIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilOperation RwStencilOperation;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwStencilFunction represents the comparison functions available for
 * a stencil test. The function may be selected via the 
 * \ref RwRenderState setting \ref rwRENDERSTATESTENCILFUNCTION.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwStencilFunction
{
    rwSTENCILFUNCTIONNASTENCILFUNCTION = 0,

    rwSTENCILFUNCTIONNEVER,         
        /**<Always fail the test */
    rwSTENCILFUNCTIONLESS,          
        /**<Accept the new pixel if its value is less than the value of
         *  the current pixel */
    rwSTENCILFUNCTIONEQUAL,         
        /**<Accept the new pixel if its value equals the value of the
         *  current pixel */
    rwSTENCILFUNCTIONLESSEQUAL,     
        /**<Accept the new pixel if its value is less than or equal to 
         *  the value of the current pixel */
    rwSTENCILFUNCTIONGREATER,       
        /**<Accept the new pixel if its value is greater than the value 
         *  of the current pixel */
    rwSTENCILFUNCTIONNOTEQUAL,      
        /**<Accept the new pixel if its value does not equal the value of 
         *  the current pixel */
    rwSTENCILFUNCTIONGREATEREQUAL,  
        /**<Accept the new pixel if its value is greater than or equal
         *  to the value of the current pixel */
    rwSTENCILFUNCTIONALWAYS,        
        /**<Always pass the test */

    rwSTENCILFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilFunction RwStencilFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwAlphaTestFunction represents the comparison functions available for
 * an alpha test. The function may be selected via the 
 * \ref RwRenderState setting \ref rwRENDERSTATEALPHATESTFUNCTION.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwAlphaTestFunction
{
    rwALPHATESTFUNCTIONNAALPHATESTFUNCTION = 0,

    rwALPHATESTFUNCTIONNEVER,         
        /**<Always fail the test */
    rwALPHATESTFUNCTIONLESS,          
        /**<Accept the new pixel if its alpha value is less than the value of
         *  the reference value */
    rwALPHATESTFUNCTIONEQUAL,         
        /**<Accept the new pixel if its alpha value equals the value of the
         *  reference value */
    rwALPHATESTFUNCTIONLESSEQUAL,     
        /**<Accept the new pixel if its alpha value is less than or equal to 
         *  the value of the reference value */
    rwALPHATESTFUNCTIONGREATER,       
        /**<Accept the new pixel if its alpha value is greater than the value 
         *  of the reference value */
    rwALPHATESTFUNCTIONNOTEQUAL,      
        /**<Accept the new pixel if its alpha value does not equal the value of 
         *  the reference value */
    rwALPHATESTFUNCTIONGREATEREQUAL,  
        /**<Accept the new pixel if its alpha value is greater than or equal
         *  to the value of the reference value */
    rwALPHATESTFUNCTIONALWAYS,        
        /**<Always pass the test */

    rwALPHATESTFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwAlphaTestFunction RwAlphaTestFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwCullMode represents the options available for culling polygons
 * during rendering. The cull mode may be set via the \ref RwRenderState
 * setting \ref rwRENDERSTATECULLMODE.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwCullMode
{
    rwCULLMODENACULLMODE = 0,

    rwCULLMODECULLNONE, 
        /**<Both front and back-facing triangles are drawn. */
    rwCULLMODECULLBACK, 
        /**<Only front-facing triangles are drawn */
    rwCULLMODECULLFRONT,
        /**<Only back-facing triangles are drawn */

    rwCULLMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCullMode RwCullMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup immediatemodedatatypes
 * RwPrimitiveType
 *  This type represents the different types of indexed
 * line and indexed triangle primitives that are available when rendering 2D
 * and 3D immediate mode objects (see API functions \ref RwIm2DRenderIndexedPrimitive,
 * \ref RwIm2DRenderPrimitive, \ref RwIm3DRenderIndexedPrimitive and \ref RwIm3DRenderPrimitive).
 * Indices are into a vertex list and must be defined in a counter-clockwise order
 * (as seen from the camera) to be visible.*/
#endif /* RWADOXYGENEXTERNAL */
enum RwPrimitiveType
{
    rwPRIMTYPENAPRIMTYPE = 0,   /**<Invalid primative type */
    rwPRIMTYPELINELIST = 1,     /**<Unconnected line segments, each line is specified by
                                 * both its start and end index, independently of other lines
                                 * (for example, 3 segments specified as 0-1, 2-3, 4-5) */
    rwPRIMTYPEPOLYLINE = 2,     /**<Connected line segments, each line's start index
                                 * (except the first) is specified by the index of the end of
                                 * the previous segment (for example, 3 segments specified as
                                 * 0-1, 1-2, 2-3) */
    rwPRIMTYPETRILIST = 3,      /**<Unconnected triangles: each triangle is specified by
                                 * three indices, independently of other triangles (for example,
                                 * 3 triangles specified as 0-1-2, 3-4-5, 6-7-8) */
    rwPRIMTYPETRISTRIP = 4,     /**<Connected triangles sharing an edge with, at most, one
                                 * other forming a series (for example, 3 triangles specified
                                 * as 0-2-1, 1-2-3-, 2-4-3) */
    rwPRIMTYPETRIFAN = 5 ,      /**<Connected triangles sharing an edge with, at most,
                                 * two others forming a fan (for example, 3 triangles specified
                                 * as 0-2-1, 0-3-2, 0-4-3) */
    rwPRIMTYPEPOINTLIST = 6,    /**<Points 1, 2, 3, etc. This is not
                                 * supported by the default RenderWare
                                 * immediate or retained-mode pipelines
                                 * (except on PlayStation 2), it is intended
                                 * for use by user-created pipelines */
    rwPRIMITIVETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPrimitiveType RwPrimitiveType;

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/badevice.h ---*/
/***************************************************************************/
/************************* System Requests *********************************/
/***************************************************************************/

/* Device controls:
 *
 * rwDEVICESYSTEMOPEN(NULL, RwEngineOpenParams *openParams, 0)
 * rwDEVICESYSTEMCLOSE(NULL, NULL, 0)
 * rwDEVICESYSTEMSTART(NULL, NULL, 0)
 * rwDEVICESYSTEMSTOP(NULL, NULL, 0)
 * rwDEVICESYSTEMREGISTER(RwDevice *coreDeviceBlock, RwMemoryFunctions *memFuncs, 0)
 * rwDEVICESYSTEMGETNUMMODES(RwInt32 *numModes, NULL, 0)
 * rwDEVICESYSTEMGETMODEINFO(RwVideoMode *modeinfo, NULL, RwInt32 modeNum)
 * rwDEVICESYSTEMUSEMODE(NULL, NULL, RwInt32 modeNum)
 * rwDEVICESYSTEMFOCUS(NULL, NULL, RwBool gainFocus)
 * rwDEVICESYSTEMINITPIPELINE(NULL, NULL, 0)
 * rwDEVICESYSTEMGETMODE(RwInt32 *curMode, NULL, 0)
 * rwDEVICESYSTEMSTANDARDS(RwStandardFunc *fnPtrArray, NULL, RwInt32 arraySize)
 * rwDEVICESYSTEMGETTEXMEMSIZE(RwInt32 *texMemSizeOut, NULL, 0)
 * rwDEVICESYSTEMGETNUMSUBSYSTEMS(RwInt32 *numSubSystemsOut, NULL, 0)
 * rwDEVICESYSTEMGETSUBSYSTEMINFO(RwSubSystemInfo *subSystemInfo, NULL, RwInt32 subSystemNum)
 * rwDEVICESYSTEMGETCURRENTSUBSYSTEM(RwInt32 *curSubSystem, NULL, 0)
 * rwDEVICESYSTEMSETSUBSYSTEM(NULL, NULL, RwInt32 subSystemNum)
 * rwDEVICESYSTEMFINALIZESTART(NULL, NULL, 0)
 * rwDEVICESYSTEMINITIATESTOP(NULL, NULL, 0)
 * rwDEVICESYSTEMRXPIPELINEREQUESTPIPE(RxPipeline **pipelineRef, NULL, RwInt32 pipeType)
 * rwDEVICESYSTEMGETID(RwUInt16 *id, NULL, 0)
 * rwDEVICESYSTEMDD         - start of device specific controls
 */

#define rwPIPETYPEMATERIAL            0
#define rwPIPETYPEWORLDSECTORINSTANCE 1
#define rwPIPETYPEATOMICINSTANCE      2

enum RwCoreDeviceSystemFn
{
    rwDEVICESYSTEMOPEN                  = 0x00,
    rwDEVICESYSTEMCLOSE,
    rwDEVICESYSTEMSTART,
    rwDEVICESYSTEMSTOP,
    rwDEVICESYSTEMREGISTER,
    rwDEVICESYSTEMGETNUMMODES,
    rwDEVICESYSTEMGETMODEINFO,
    rwDEVICESYSTEMUSEMODE,
    rwDEVICESYSTEMFOCUS,
    rwDEVICESYSTEMINITPIPELINE,
    rwDEVICESYSTEMGETMODE,
    rwDEVICESYSTEMSTANDARDS,
    rwDEVICESYSTEMGETTEXMEMSIZE,
    rwDEVICESYSTEMGETNUMSUBSYSTEMS,
    rwDEVICESYSTEMGETSUBSYSTEMINFO,
    rwDEVICESYSTEMGETCURRENTSUBSYSTEM,
    rwDEVICESYSTEMSETSUBSYSTEM,
    rwDEVICESYSTEMFINALIZESTART,
    rwDEVICESYSTEMINITIATESTOP,
    rwDEVICESYSTEMGETMAXTEXTURESIZE,
    rwDEVICESYSTEMRXPIPELINEREQUESTPIPE,
    rwDEVICESYSTEMGETMETRICBLOCK,
    rwDEVICESYSTEMGETID,
    rwDEVICESYSTEMDD                    = 0x1000,
    rwCOREDEVICESYSTEMFNFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCoreDeviceSystemFn RwCoreDeviceSystemFn;

/******************************************************************************/
/********************* Standard functions *************************************/
/******************************************************************************/

#define rwSTANDARDNASTANDARD            0
#define rwSTANDARDCAMERABEGINUPDATE     1   /* Start 3d camera update */
#define rwSTANDARDRGBTOPIXEL            2   /* For an RGB value return a pixel value */
#define rwSTANDARDPIXELTORGB            3   /* For a pixel value returns a RGB value */
#define rwSTANDARDRASTERCREATE          4   /* Create an raster */
#define rwSTANDARDRASTERDESTROY         5   /* Raster destroy */
#define rwSTANDARDIMAGEGETRASTER        6   /* Get image from a raster */
#define rwSTANDARDRASTERSETIMAGE        7   /* Set raster from an image */
#define rwSTANDARDTEXTURESETRASTER      8   /* Set texture's raster */
#define rwSTANDARDIMAGEFINDRASTERFORMAT 9   /* Find a suitable raster format for an image */
#define rwSTANDARDCAMERAENDUPDATE       10  /* End 3d camera update */
#define rwSTANDARDSETRASTERCONTEXT      11  /* Start destination of 2d operations */
#define rwSTANDARDRASTERSUBRASTER       12  /* Make a raster inside another raster */
#define rwSTANDARDRASTERCLEARRECT       13  /* Clear a rectangle of the current dest raster */
#define rwSTANDARDRASTERCLEAR           14  /* Clear the current dest raster */
#define rwSTANDARDRASTERLOCK            15  /* Lock a raster to get it's pixels */
#define rwSTANDARDRASTERUNLOCK          16  /* Unlock a raster to return it's pixels */
#define rwSTANDARDRASTERRENDER          17  /* Render a raster (not scaled, but masked) */
#define rwSTANDARDRASTERRENDERSCALED    18  /* Render a raster (scaled and masked) */
#define rwSTANDARDRASTERRENDERFAST      19  /* Render a raster (not scaled or masked) */
#define rwSTANDARDRASTERSHOWRASTER      20  /* Show a camera raster */
#define rwSTANDARDCAMERACLEAR           21  /* Clear a camera's raster and/or Z raster */
#define rwSTANDARDHINTRENDERF2B         22  /* Set hint for rendering direction in the world */
#define rwSTANDARDRASTERLOCKPALETTE     23  /* Lock a raster to get it's palette */
#define rwSTANDARDRASTERUNLOCKPALETTE   24  /* Unlock a raster to return it's palette */
#define rwSTANDARDNATIVETEXTUREGETSIZE  25  /* Get size of native texture when written to a stream */
#define rwSTANDARDNATIVETEXTUREREAD     26  /* Read native texture from the stream */
#define rwSTANDARDNATIVETEXTUREWRITE    27  /* Write native texture to the stream */
#define rwSTANDARDRASTERGETMIPLEVELS    28  /* Get the number of mip levels in a raster */
#define rwSTANDARDNUMOFSTANDARD         29

/* rwDEVICE_ is a 16 bit device id.
   When adding a new device, add it to the end of the apropriate list.
   "Real" devices should have an 8 bit id. NULLxxx devices should have the
   equivalent "Real" device in the bottom 8 bits, with a non-zero value in
   the top 8 bits */
#define rwDEVICE_UNKNOWN (0)
#define rwDEVICE_D3D8    (1)
#define rwDEVICE_D3D9    (2)
#define rwDEVICE_GCN     (3)
#define rwDEVICE_NULL    (4)
#define rwDEVICE_OPENGL  (5)
#define rwDEVICE_SKY2    (6)
#define rwDEVICE_SOFTRAS (7)
#define rwDEVICE_XBOX    (8)

#define rwDEVICE_NULLxxx  (0x100)
#define rwDEVICE_NULLGCN  (rwDEVICE_NULLxxx | rwDEVICE_GCN)
#define rwDEVICE_NULLSKY  (rwDEVICE_NULLxxx | rwDEVICE_SKY2)
#define rwDEVICE_NULLXBOX (rwDEVICE_NULLxxx | rwDEVICE_XBOX)

/****************************************************************************
 Global Types
 */

/* Standard functions */
typedef RwBool (*RwStandardFunc)(void *out,void *pInOut, RwInt32 nI);

typedef struct RwEngineOpenParams RwEngineOpenParams;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * \struct RwEngineOpenParams
 * This type is used to specify device dependent parameters
 * for use by the API function \ref RwEngineOpen.
 * For a Windows application the displayID field
 * should be set to the window's handle (of type HWND).
 * For NULL and sky libraries displayID=0:
 *
 * \if xbox
 * \see RwXboxDeviceConfig
 * \endif
 *
 * \if gcn
 * \see RwGameCubeDeviceConfig
 * \endif
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwEngineOpenParams
{
    void    *displayID;     /**< Display Identifier */
};

/* nOption is one of a list of possible System defines (see above) */
typedef RwBool
    (*RwSystemFunc)(RwInt32 nOption,
                    void *pOut,
                    void *pInOut,
                    RwInt32 nIn);

/* Device block */
typedef RwBool
    (*RwRenderStateSetFunction)(RwRenderState nState,void *pParam);

typedef RwBool
    (*RwRenderStateGetFunction)(RwRenderState nState,void *pParam);

typedef RwBool
    (*RwIm2DRenderLineFunction)(RwIm2DVertex *vertices,
                                RwInt32 numVertices,
                                RwInt32 vert1,
                                RwInt32 vert2);

typedef RwBool
    (*RwIm2DRenderTriangleFunction)(RwIm2DVertex *vertices,
                                    RwInt32 numVertices,
                                    RwInt32 vert1,
                                    RwInt32 vert2,
                                    RwInt32 vert3);

typedef RwBool
    (*RwIm2DRenderPrimitiveFunction)(RwPrimitiveType primType,
                                     RwIm2DVertex *vertices,
                                     RwInt32 numVertices);

typedef RwBool
    (*RwIm2DRenderIndexedPrimitiveFunction)(RwPrimitiveType primType,
                                            RwIm2DVertex *vertices,
                                            RwInt32 numVertices,
                                            RwImVertexIndex *indices,
                                            RwInt32 numIndices);

typedef RwBool
    (*RwIm3DRenderLineFunction)(RwInt32 vert1,
                                RwInt32 vert2);

typedef RwBool
    (*RwIm3DRenderTriangleFunction)(RwInt32 vert1,
                                    RwInt32 vert2,
                                    RwInt32 vert3);

typedef RwBool
    (*RwIm3DRenderPrimitiveFunction)(RwPrimitiveType primType);

typedef RwBool
    (*RwIm3DRenderIndexedPrimitiveFunction)(RwPrimitiveType primtype,
                                            RwImVertexIndex *indices,
                                            RwInt32 numIndices);


typedef struct RwDevice RwDevice;

#if (!defined(DOXYGEN))
/*
 * struct RwDevice
 * Structure describing a display device
 */
struct RwDevice
{
    RwReal                                  gammaCorrection; /* Gamma correction  */
    RwSystemFunc                            fpSystem;  /* System handler */
    RwReal                                  zBufferNear; /* Near Z buffer value */
    RwReal                                  zBufferFar; /* Far Z buffer value */

    /* Immediate mode functions */
    RwRenderStateSetFunction                fpRenderStateSet; /* Internal Use */
    RwRenderStateGetFunction                fpRenderStateGet; /* Internal Use */

    /* Render functions */
    RwIm2DRenderLineFunction                fpIm2DRenderLine; /* Internal Use */
    RwIm2DRenderTriangleFunction            fpIm2DRenderTriangle; /* Internal Use */
    RwIm2DRenderPrimitiveFunction           fpIm2DRenderPrimitive; /* Internal Use */
    RwIm2DRenderIndexedPrimitiveFunction    fpIm2DRenderIndexedPrimitive; /* Internal Use */

    RwIm3DRenderLineFunction                fpIm3DRenderLine; /* Internal Use */
    RwIm3DRenderTriangleFunction            fpIm3DRenderTriangle; /* Internal Use */
    RwIm3DRenderPrimitiveFunction           fpIm3DRenderPrimitive; /* Internal Use */
    RwIm3DRenderIndexedPrimitiveFunction    fpIm3DRenderIndexedPrimitive; /* Internal Use */
};

#endif /* (!defined(DOXYGEN)) */

typedef struct RwMetrics RwMetrics;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * \struct RwMetrics
 * This structure provides information about the performance
 * of the application.  The metrics are recorded only in the metrics
 * libraries.  To use metrics, you should compile with the RWMETRICS
 * preprocessor symbol defines, and link with the metrics libraries
 * that ship with the SDK.  The metrics are recorded on a per-frame
 * basis.  Each platform may provide additional information that
 * is specific to that platform.  Note that either the
 * \ref numTextureUploads or \ref numResourceAllocs being non-zero can
 * be considered as bad news and will indicate a significantly
 * reduced rendering performance.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwMetrics
{
    RwUInt32    numTriangles;           /**< The number of triangles processed. */
    RwUInt32    numProcTriangles;       /**< The number of mesh triangles processed. */
    RwUInt32    numVertices;            /**< The number of vertices processed. */
    RwUInt32    numTextureUploads;
    /**<
     * \if sky2 The number of textures used. \endif
     * \if d3d8 The number of times RwD3D8SetTexture is called. \endif
     * \if d3d9 The number of times RwD3D9SetTexture is called.  \endif
     * \if opengl The number of times RwOpenGLSetTexture is called. \endif
     * \if xbox The number of textures used. \endif
     * \if gcn The number of textures used. \endif
     */
    RwUInt32    sizeTextureUploads;
    /**<
     * \if sky2 The number of bytes uploaded to the GS. \endif
     * \if d3d8 The size of the textures, in bytes, that have been passed to RwD3D8SetTexture. \endif
     * \if d3d9 The size of the textures, in bytes, that have been passed to RwD3D9SetTexture. \endif
     * \if opengl The size of the textures, in bytes, that have been passed to RwOpenGLSetTexture. \endif
     * \if xbox Size of textures swapped. \endif
     * \if gcn Size of textures swapped. \endif
     */
    RwUInt32    numResourceAllocs;      /**< The number of resource blocks swapped. */
    void        *devSpecificMetrics;    /**< Device specific metrics. */
};

#define SUBSYSTEMNAME_MAXLEN 80

typedef struct RwSubSystemInfo RwSubSystemInfo;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * \struct RwSubSystemInfo
 * This type is used to represent information about a device.
 * The only available field specifies a character string
 * which identifies the subsystem
 * (see API function \ref RwEngineGetSubSystemInfo). */
#endif /* RWADOXYGENEXTERNAL */
struct RwSubSystemInfo
{
    RwChar  name[SUBSYSTEMNAME_MAXLEN]; /**< Sub system string */
};


/* Video modes */
/* These are flag bits which may be ORd */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * RwVideoModeFlag
 * These flags specify the type of display that RenderWare
 * will use.  The flags may be OR'd together to build composite modes.
 * Note that not all modes are supported on all platforms.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwVideoModeFlag
{
    rwVIDEOMODEEXCLUSIVE    = 0x0001, /**<Exclusive (i.e. full-screen) */
    rwVIDEOMODEINTERLACE    = 0x0002, /**<Interlaced                   */
    rwVIDEOMODEFFINTERLACE  = 0x0004, /**<Flicker Free Interlaced      */

    /* Platform specific video mode flags. */

    rwVIDEOMODE_PS2_FSAASHRINKBLIT    = 0x0100,
    /**< \if sky2
     *   Full-screen antialiasing mode 0
     *   \endif
     */
    rwVIDEOMODE_PS2_FSAAREADCIRCUIT   = 0x0200,
    /**< \if sky2
     *   Full-screen antialiasing mode 1
     *   \endif
     */

    rwVIDEOMODE_XBOX_WIDESCREEN       = 0x0100,
    /**< \if xbox
     *   Wide screen.
     *   \endif
     */
    rwVIDEOMODE_XBOX_PROGRESSIVE      = 0x0200,
    /**< \if xbox
     *   Progressive.
     *   \endif
     */
    rwVIDEOMODE_XBOX_FIELD            = 0x0400,
    /**< \if xbox
     *   Field rendering.
     *   \endif
     */
    rwVIDEOMODE_XBOX_10X11PIXELASPECT = 0x0800,
    /**< \if xbox
     *   The frame buffer is centered on the display.
     *   On a TV that is 704 pixels across, this would leave 32 pixels of black
     *   border on the left and 32 pixels of black border on the right.
     *   \endif
     */

    rwVIDEOMODEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwVideoModeFlag RwVideoModeFlag;

#define rwVIDEOMODEFSAA0 rwVIDEOMODE_PS2_FSAASHRINKBLIT
#define rwVIDEOMODEFSAA1 rwVIDEOMODE_PS2_FSAAREADCIRCUIT

typedef struct RwVideoMode RwVideoMode;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * \struct RwVideoMode
 * This type represents a video mode available on a device specified
 * by the frame buffer resolution (width and height) and depth,
 * and a flag indicating  whether the device has exclusive use of
 * the mode (see API function \ref RwEngineGetVideoModeInfo): */
#endif /* RWADOXYGENEXTERNAL */
struct RwVideoMode
{
        RwInt32         width;   /**< Width  */
        RwInt32         height;  /**< Height */
        RwInt32         depth;   /**< Depth  */
        RwVideoModeFlag flags;   /**< Flags  */
        RwInt32         refRate; /**< Approximate refresh rate */
        RwInt32         format;  /**< Raster format
                                  * \see RwRasterFormat 
                                  */
};
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwengine
 * RwEngineInitFlag
 * Engine initialization flags.  An application can use
 * these to control the memory manager that RenderWare uses for dynamic
 * memory management.  By default RenderWare uses FreeLists.  This is the
 * preferred way of using RenderWare.  If the application does not want
 * RenderWare to use the memory manager, then the application can pass
 * rwENGINEINITNOFREELISTS as the argument to \ref RwEngineInit and
 * RenderWare will replace freelist calls to corresponding calls to
 * RwMalloc and RwFree.  This will result in more memory management
 * related calls.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwEngineInitFlag
{
    rwENGINEINITFREELISTS = 0,      /**<Use Freelists */
    rwENGINEINITNOFREELISTS = 0x1,  /**<Don't use Freelists */
    rwENGINEINITFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwEngineInitFlag RwEngineInitFlag;

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bafsys.h ---*/

/****************************************************************************
 Global Types
 */

/*
 * rwFnFexist
 * Returns TRUE if file with given name exists, FALSE if it doesn't.
 */
typedef RwBool  (*rwFnFexist)(const RwChar *name);

/*
 * rwFnFopen
 * Mimics ANSI C Standard Library fopen.
 */
typedef void   *(*rwFnFopen)(const RwChar *name, const RwChar *mode);

/*
 * rwFnFclose
 * Mimics ANSI C Standard Library fclose.
 */
typedef int     (*rwFnFclose)(void *fptr);

/*
 * rwFnFread
 * Mimics ANSI C Standard Library fread.
 */
typedef size_t  (*rwFnFread)(void *addr, size_t size, size_t count, void *fptr);

/*
 * rwFnFwrite
 * Mimics ANSI C Standard Library fwrite.
 */
typedef size_t  (*rwFnFwrite)(const void *addr, size_t size, size_t count, void *fptr);

/*
 * rwFnFgets
 * Mimics ANSI C Standard Library fgets.
 */
typedef RwChar *(*rwFnFgets)(RwChar *buffer, int maxLen, void *fptr);

/*
 * rwFnFputs
 * Mimics ANSI C Standard Library fputs.
 */
typedef int     (*rwFnFputs)(const RwChar *buffer, void *fptr);

/*
 * rwFnFeof
 * Mimics ANSI C Standard Library feof.
 */
typedef int     (*rwFnFeof)(void *fptr);

/*
 * rwFnFseek
 * Mimics ANSI C Standard Library fseek.
 */
typedef int     (*rwFnFseek)(void *fptr, long offset, int origin);

/*
 * rwFnFflush
 * Mimics ANSI C Standard Library fflush.
 */
typedef int     (*rwFnFflush)(void *fptr);

/*
 * rwFnFtell
 * Mimics ANSI C Standard Library ftell.
 */
typedef int     (*rwFnFtell)(void *fptr);


typedef struct RwFileFunctions RwFileFunctions;
/**
 * \ingroup memoryfileinterface
 * \struct RwFileFunctions
 * This type is used to specify the file access
 * functions used by RenderWare. The default file system uses the standard
 * ANSI functions. The application may install an alternative file system
 * providing it complies with the ANSI interface -- see API 
 * function \ref RwOsGetFileInterface.
 *
 * The function types associated with this type are defined as follows 
 *
 * \verbatim
   RwBool (*rwFnFexist)(const RwChar *name)
       void *(*rwFnFopen)(const RwChar *name, const RwChar *access) 
       int (*rwFnFclose)(void *fptr)
       size_t (*rwFnFread)(void *addr, size_t size, size_t count, void *fptr)                        
       size_t (*rwFnFwrite)(const void *addr, size_t size, size_t count,
                 void *fptr)
   RwChar *(*rwFnFgets)(RwChar *buffer, int maxLen, void *fptr)
       int (*rwFnFputs)(const RwChar *buffer, void *fptr)
       int (*rwFnFeof)(void *fptr)
       int (*rwFnFseek)(void *fptr, long offset, int origin)
       int (*rwFnFflush)(void *fptr)
       int (*rwFnFtell)(void *fptr)
 \endverbatim
 * Note the function argument lists are consistent with the ANSI
 * standard file access interface:
 */
struct RwFileFunctions
{
    rwFnFexist  rwfexist; /**< Pointer to fexist function */
    rwFnFopen   rwfopen;  /**< Pointer to fopen function */
    rwFnFclose  rwfclose; /**< Pointer to fclose function */
    rwFnFread   rwfread;  /**< Pointer to fread function */
    rwFnFwrite  rwfwrite; /**< Pointer to fwrite function */
    rwFnFgets   rwfgets;  /**< Pointer to fgets function */
    rwFnFputs   rwfputs;  /**< Pointer to puts function */
    rwFnFeof    rwfeof;   /**< Pointer to feof function */
    rwFnFseek   rwfseek;  /**< Pointer to fseek function */
    rwFnFflush  rwfflush; /**< Pointer to fflush function */
    rwFnFtell   rwftell;  /**< Pointer to ftell function */  
};

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/baerr.h ---*/
/****************************************************************************
 Global Types
 */

typedef struct RwError RwError;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwerror
 * \struct RwError
 * This type represents a RenderWare error specified by the
 * ID of the plugin that the error was issued from (pluginID) and the error
 * code itself (errorCode) (see API function \ref RwErrorGet).
 * \param pluginID The ID of the plugin that issued the error.
 * \param errorCode A value representing the error code.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwError
{
    RwInt32     pluginID;  /**< Internal Use */
    RwInt32     errorCode; /**< Internal Use */
};

#define RWECODE(a,b) a,

/* common errors have the MSB set */

enum RwErrorCodeCommon
{
    E_RW_NOERROR = (int)0x80000000L,
#include "errcom.def"
    E_RW_LASTERROR = RWFORCEENUMSIZEINT
};
typedef enum RwErrorCodeCommon RwErrorCodeCommon;

#undef RWECODE

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/badebug.h ---*/

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwdebug
 * RwDebugType 
 * This type represents the different types of debug and 
 * trace messages that can be sent to the currently installed debug handler 
 * (see API function \ref RwDebugSendMessage)*/
enum RwDebugType
{
    rwNADEBUGTYPE = 0,          /**<Invalid */
    rwDEBUGASSERT,              /**<Send an assert message */
    rwDEBUGERROR,               /**<Send an error message */
    rwDEBUGMESSAGE,             /**<Send an informational message */
    rwDEBUGTRACE,               /**<Send a trace message */
    rwDEBUGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwDebugType RwDebugType;

/**
 * \ingroup rwdebug
 * \ref RwDebugHandler
 * This type represents the
 * function called from \ref RwDebugSendMessage for sending a message to the
 * RenderWare debug stream.
 *
 * \param  type   Type of debug message (assert, error, etc.).
 *
 * \param  string   Pointer to a string containing the error
 * message.
 *
 * \see RwDebugSetHandler
 */
typedef void        (*RwDebugHandler) (RwDebugType type,

                                       const RwChar * string);

#define RwDebugSetHandler(handler)
#define RwDebugSetTraceState(state)
#define RwDebugSendMessage(type, funcName, message)

#if (!defined(RWREGSETDEBUGTRACE))
#define RWREGSETDEBUGTRACE(_name) /* No op */
#endif /* (!defined(RWREGSETDEBUGTRACE)) */

/****************************************************************************
 Defines
 */

/* macros used to access plugin data in objects */
#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))

#define RWPLUGINOFFSETCONST(_type, _base, _offset)              \
   ((const _type *)((const RwUInt8 *)(_base) + (_offset)))

/* macro used to access global data structure (the root type is RwGlobals) */
#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)

#define RWASSERTISTYPE(_f, _t) \
   RWASSERT((((const RwObject *)(_f))->type)==(_t))

/****************************************************************************
 Global Types
 */

enum RwEngineStatus
{
    rwENGINESTATUSIDLE = 0,                     /* This needs to be zero */
    rwENGINESTATUSINITED = 1,
    rwENGINESTATUSOPENED = 2,
    rwENGINESTATUSSTARTED = 3,
    rwENGINESTATUSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwEngineStatus RwEngineStatus;

typedef struct RwGlobals RwGlobals;

#if (!defined(DOXYGEN))
struct RwGlobals
{
#ifdef RWDEBUG
        RwDebugHandler      debugFunction;          /* debug string handler */
        void                *debugFile;             /* debug output file */
        RwInt32             debugStackDepth;        /* current depth of function stack */
        RwBool              debugTrace;             /* is function tracing enabled */
#endif

        /* Current entities */
        void                *curCamera;             /* Current camera */
        void                *curWorld;              /* Current World */

        /* Checking counters */
        RwUInt16            renderFrame;            /* Camera display count */
        RwUInt16            lightFrame;             /* Used to ensure each light is applied only once. */
        RwUInt16            pad[2];                 /* Longword align it again */

        /* For the currently accessed device */
        RwDevice            dOpenDevice;

        /* Standard renderers and functions */
        RwStandardFunc      stdFunc[rwSTANDARDNUMOFSTANDARD];

        /* All of the frames which have been updated */
        RwLinkList          dirtyFrameList;

        /* The file functions */
        RwFileFunctions     fileFuncs;

        /* The string functions */
        RwStringFunctions   stringFuncs;

        /* The memory allocation functions */
        RwMemoryFunctions   memoryFuncs;
#ifdef RWDEBUG
        RwBool              freeListExtraDebug;
#endif /* RWDEBUG */

        /* virtual memory alloc/free functions */
        RwMemoryAllocFn         memoryAlloc;
        RwMemoryFreeFn          memoryFree;

        RwMetrics           *metrics;

        /* Current engine status */
        RwEngineStatus      engineStatus;

        /* Resource arena init size. */
        RwUInt32            resArenaInitSize;
};

typedef struct RwModuleInfo RwModuleInfo;
struct RwModuleInfo
{
        RwInt32     globalsOffset;
        RwInt32     numInstances;
};
#endif /* (!defined(DOXYGEN)) */



/****************************************************************************
 Program wide globals
 */

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwresources
 * \struct RwResEntry
 * RwResEntry object. Instanced data block in resources arena.
 * This should be considered an opaque
 * type. Use the RwResEntry API functions to access.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef struct RwResEntry RwResEntry;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwresources
 * \ref RwResEntryDestroyNotify type represents the function
 * called from \ref RwResourcesFreeResEntry (and indirectly from
 * \ref RwResourcesEmptyArena) immediately before the memory used by the
 * specified resources entry is released.
 *
 * \param  resEntry   Pointer to the instanced data.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void        (*RwResEntryDestroyNotify) (RwResEntry * resEntry);

#if (!defined(DOXYGEN))
struct RwResEntry
{
    RwLLLink            link;   /* Node in the list of resource elements */
    RwInt32             size;   /* Size of this node */
    void               *owner;  /* Owner of this node */
    RwResEntry        **ownerRef; /* Pointer to pointer to this (enables de-alloc) */
    RwResEntryDestroyNotify destroyNotify; /* This is called right before destruction */
};
#endif /* (!defined(DOXYGEN)) */

typedef struct rwResources rwResources;

#if (!defined(DOXYGEN))
struct rwResources
{
    RwInt32             maxSize;
    RwInt32             currentSize;
    RwInt32             reusageSize;

    void               *memHeap;

    RwLinkList          entriesA;
    RwLinkList          entriesB;

    RwLinkList         *freeEntries;
    RwLinkList         *usedEntries;
};


typedef struct rwResourcesGlobals rwResourcesGlobals;
struct rwResourcesGlobals
{
    rwResources         res;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bacolor.h ---*/
/****************************************************************************
 Global Types
 */

#ifndef RWADOXYGENEXTERNAL
typedef struct RwRGBAReal RwRGBAReal;
/**
 * \ingroup rwrgba
 * \struct RwRGBAReal
 * This structure represents an RGBA color which has 
 * components specified as real values. 
 *
 * A color component of an RwRGBA with the value 255 generally corresponds 
 * to the associated component in an RwRGBAReal with the value 1.0f. 
 * However, any values can be substituted to denormalize/normalize 
 * RwRGBAReal and create different effects. For example, while light colors
 * are expressed as normalized RGBA, interesting effects can be gained using 
 * larger values. 
 *
 * It should also be noted that a color component of an RwRGBA with the 
 * value 0 generally corresponds to the associcated component in an 
 * RwRGBAReal with the value 0.0.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};

#if (!defined(RwRGBARealAssign))
#define RwRGBARealAssign(_target, _source)        \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwRGBARealAssign)) */

typedef struct RwRGBA RwRGBA;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrgba
 * \struct RwRGBA
 * This structure represents an RGBA color 
 * which has integer components specified in the range 0 to 255. */
#endif /* RWADOXYGENEXTERNAL */
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};

#if (!defined(RwRGBAAssign))
#define RwRGBAAssign(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwRGBAAssign)) */

#define RwRGBARealAddMacro(o,a,b)                                        \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) + (   (b)->red));                           \
    (o)->green = (((a)->green) + ( (b)->green));                         \
    (o)->blue  = (((a)->blue) + (  (b)->blue));                          \
    (o)->alpha = (((a)->alpha) + ( (b)->alpha));                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealSubMacro(o,a,b)                                        \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) - (   (b)->red));                           \
    (o)->green = (((a)->green) - ( (b)->green));                         \
    (o)->blue  = (((a)->blue) - (  (b)->blue));                          \
    (o)->alpha = (((a)->alpha) - ( (b)->alpha));                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealScaleMacro(o,a,scale)                                  \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) * (   scale));                              \
    (o)->green = (((a)->green) * ( scale));                              \
    (o)->blue  = (((a)->blue) * (  scale));                              \
    (o)->alpha = (((a)->alpha) * ( scale));                              \
}                                                                        \
MACRO_STOP

/* Conversion macros */
#define RwRGBAFromRwRGBARealMacro(o, i)                                  \
MACRO_START                                                              \
{                                                                        \
    RwInt32 quantize;                                                    \
                                                                         \
    quantize = RwInt32FromRealMacro( ((i)->red   * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->red   = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->green * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->green = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->blue  * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->blue  = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->alpha * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->alpha = (RwUInt8) quantize;                                     \
                                                                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealFromRwRGBAMacro(o, i)                                  \
MACRO_START                                                              \
{                                                                        \
    (o)->red   =                                                         \
        (((RwReal)(((i)->red))) * (   (RwReal)((1.0/255.0))));           \
    (o)->green =                                                         \
        (((RwReal)(((i)->green))) * ( (RwReal)((1.0/255.0))));           \
    (o)->blue  =                                                         \
        (((RwReal)(((i)->blue))) * (  (RwReal)((1.0/255.0))));           \
    (o)->alpha =                                                         \
        (((RwReal)(((i)->alpha))) * ( (RwReal)((1.0/255.0))));           \
}                                                                        \
MACRO_STOP

#define RwRGBARealAdd(o,a,b) \
        RwRGBARealAddMacro(o,a,b)

#define RwRGBARealSub(o,a,b) \
        RwRGBARealSubMacro(o,a,b)

#define RwRGBARealScale(o,a,scale) \
        RwRGBARealScaleMacro(o,a,scale)

#define RwRGBAFromRwRGBAReal(o, i) \
        RwRGBAFromRwRGBARealMacro(o, i)

#define RwRGBARealFromRwRGBA(o, i) \
        RwRGBARealFromRwRGBAMacro(o, i)

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/babinmtx.h ---*/

/****************************************************************************
 Global types
 */

/* Matrix stream format */
typedef struct rwStreamMatrix RwMatrixChunkInfo;
typedef struct rwStreamMatrix rwStreamMatrix;
#if (!defined(DOXYGEN))
struct rwStreamMatrix
{
    RwV3d               right;
    RwV3d               up;
    RwV3d               at;
    RwV3d               pos;
    RwInt32             type;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/babinary.h ---*/

/****************************************************************************
 Global Types
 */

typedef struct RwChunkHeaderInfo RwChunkHeaderInfo;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwstream
 * \struct RwChunkHeaderInfo
 * Holds data for a chunk header read from a
 * stream with \ref RwStreamReadChunkHeaderInfo. */
#endif /* RWADOXYGENEXTERNAL */
struct RwChunkHeaderInfo
{
    RwUInt32 type;      /**< chunk ID - see \ref RwStreamFindChunk */
    RwUInt32 length;    /**< length of the chunk data in bytes */
    RwUInt32 version;   /**< version of the chunk data.
                         *   See \ref RwEngineGetVersion. */
    RwUInt32 buildNum;  /**< build number of the RenderWare libraries
                         *   previously used to stream out the data */
    RwBool isComplex;   /**< Internal Use */
};

#define RwRenderStateGetMacro(_state, _value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateGet(_state, _value))

#define RwRenderStateSetMacro(_state, _value) (RWSRCGLOBAL(dOpenDevice).fpRenderStateSet(_state, _value))

#define RwRenderStateGet(_state, _value) RwRenderStateGetMacro(_state, _value)

#define RwRenderStateSet(_state, _value) RwRenderStateSetMacro(_state, _value)

RwMemoryFunctions* RwOsGetMemoryInterface(); // 0x802230
RwFreeList* _rwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint, const RwChar* fileCreate, RwUInt32 lineCreate); // 0x801980
RwFreeList* RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint); // 0x801B70
RwBool RwFreeListDestroy(RwFreeList* freelist); // 0x801B80
void RwFreeListSetFlags(RwFreeList* freeList, RwUInt32 flags); // 0x801C10
RwUInt32 RwFreeListGetFlags(RwFreeList* freeList); // 0x801C20
RwInt32 RwFreeListPurge(RwFreeList* freelist); // 0x801E00
RwFreeList* RwFreeListForAllUsed(RwFreeList* freelist, RwFreeListCallBack fpCallBack, void* data); // 0x801E90
RwInt32 RwFreeListPurgeAllFreeLists(); // 0x801F90
void RwStreamSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7EC760
RwStream* _rwStreamInitialize(RwStream* stream, RwBool rwOwned, RwStreamType type, RwStreamAccessType accessType, const void* data); // 0x7EC810
RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data); // 0x7ECEF0
RwBool RwStreamClose(RwStream* stream, void* data); // 0x7ECE20
RwUInt32 RwStreamRead(RwStream* stream, void* buffer, RwUInt32 length); // 0x7EC9D0
template<typename T>
T RwStreamRead(RwStream* stream, size_t size = sizeof(T)) {
    T data;
    RwStreamRead(stream, &data, size);
    return data;
}
RwStream* RwStreamWrite(RwStream* stream, const void* buffer, RwUInt32 length); // 0x7ECB30
RwStream* RwStreamSkip(RwStream* stream, RwUInt32 offset); // 0x7ECD00
RwBool _rwStringOpen(); // 0x80A240
void _rwStringClose(); // 0x80A440
RwBool _rwStringDestroy(RwChar* string); // 0x80A450
RwUInt32 _rwStringStreamGetSize(const RwChar* string); // 0x80A470
const RwChar* _rwStringStreamWrite(const RwChar* string, RwStream* stream); // 0x80A4A0
RwChar* _rwStringStreamFindAndRead(RwChar* string, RwStream* stream); // 0x80A510
void RwPluginRegistrySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x8087B0
RwBool _rwPluginRegistrySetStaticPluginsSize(RwPluginRegistry* reg, RwInt32 size); // 0x808430
RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x8084A0
RwInt32 _rwPluginRegistryGetPluginOffset(const RwPluginRegistry* reg, RwUInt32 pluginID); // 0x808470
const RwPluginRegistry* _rwPluginRegistryInitObject(const RwPluginRegistry* reg, void* object); // 0x8086E0
const RwPluginRegistry* _rwPluginRegistryDeInitObject(const RwPluginRegistry* reg, void* object); // 0x808740
const RwPluginRegistry* _rwPluginRegistryCopyObject(const RwPluginRegistry* reg, void* dstObject, const void* srcObject); // 0x808770
RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x8088E0
RwInt32 _rwPluginRegistryAddPlgnStrmlwysCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x808920
RwInt32 _rwPluginRegistryAddPlgnStrmRightsCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB); // 0x808950
const RwPluginRegistry* _rwPluginRegistryReadDataChunks(const RwPluginRegistry* reg, RwStream* stream, void* object); // 0x808980
const RwPluginRegistry* _rwPluginRegistryWriteDataChunks(const RwPluginRegistry* reg, RwStream* stream, const void* object); // 0x808B40
const RwPluginRegistry* _rwPluginRegistrySkipDataChunks(const RwPluginRegistry* reg, RwStream* stream); // 0x808C10
RwInt32 _rwPluginRegistryGetSize(const RwPluginRegistry* reg, const void* object); // 0x808B00
const RwPluginRegistry* _rwPluginRegistryInvokeRights(const RwPluginRegistry* reg, RwUInt32 id, void* obj, RwUInt32 extraData); // 0x808AB0
RwBool RwEngineGetMatrixTolerances(RwMatrixTolerance * const tolerance); // 0x7F1780
RwBool RwEngineSetMatrixTolerances(const RwMatrixTolerance * const tolerance); // 0x7F17B0
void RwMatrixSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7F16A0
RwBool RwMatrixDestroy(RwMatrix* mpMat); // 0x7F2A20
RwMatrix* RwMatrixCreate(); // 0x7F2A50
RwMatrix* RwMatrixMultiply(RwMatrix* matrixOut, const RwMatrix* MatrixIn1, const RwMatrix* matrixIn2); // 0x7F18B0
RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp); // 0x7F25A0
RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn); // 0x7F1920
RwMatrix* RwMatrixInvert(RwMatrix* matrixOut, const RwMatrix* matrixIn); // 0x7F2070
RwMatrix* RwMatrixScale(RwMatrix* matrix, const RwV3d* scale, RwOpCombineType combineOp); // 0x7F22C0
RwMatrix* RwMatrixTranslate(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp); // 0x7F2450
RwMatrix* RwMatrixRotate(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp); // 0x7F1FD0
RwMatrix* RwMatrixRotateOneMinusCosineSine(RwMatrix* matrix, const RwV3d* unitAxis, RwReal oneMinusCosine, RwReal sine, RwOpCombineType combineOp); // 0x7F1D00
const RwMatrix* RwMatrixQueryRotate(const RwMatrix* matrix, RwV3d* unitAxis, RwReal* angle, RwV3d* center); // 0x7F2720
RwMatrix* RwMatrixUpdate(RwMatrix* matrix); // 0x7F18A0
RwMatrix* RwMatrixOptimize(RwMatrix* matrix, const RwMatrixTolerance* tolerance); // 0x7F17E0
RwReal _rwMatrixDeterminant(const RwMatrix* matrix); // 0x7F1450
RwReal _rwMatrixNormalError(const RwMatrix* matrix); // 0x7F1500
RwReal _rwMatrixOrthogonalError(const RwMatrix* matrix); // 0x7F14A0
RwReal _rwMatrixIdentityError(const RwMatrix* matrix); // 0x7F1590
RwReal RwV3dNormalize(RwV3d* out, const RwV3d* in); // 0x7ED9B0
RwReal RwV3dLength(const RwV3d* in); // 0x7EDAC0
RwReal RwV2dLength(const RwV2d* in); // 0x7EDBF0
RwReal RwV2dNormalize(RwV2d* out, const RwV2d* in); // 0x7EDC60
RwV3d* RwV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix); // 0x7EDD60
RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix); // 0x7EDD90
RwV3d* RwV3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix); // 0x7EDDC0
RwV3d* RwV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numPoints, const RwMatrix* matrix); // 0x7EDDF0
RwReal _rwSqrt(const RwReal num); // 0x7EDB30
RwReal _rwInvSqrt(const RwReal num); // 0x7EDB90
RwReal _rwV3dNormalize(RwV3d* out, const RwV3d* in); // 0x7ED910
RwSList* _rwSListCreate(RwInt32 size, RwUInt32 hint); // 0x809160
RwBool _rwSListDestroy(RwSList* sList); // 0x809440
void _rwSListDestroyEndEntries(RwSList* sList, RwInt32 amount); // 0x809400
void* _rwSListGetEntry(RwSList* sList, RwInt32 entry); // 0x809510
void* _rwSListGetNewEntry(RwSList* sList, RwUInt32 hint); // 0x809240
RwInt32 _rwSListGetNumEntries(const RwSList* sList); // 0x8094B0
void* _rwSListGetBegin(RwSList* sList); // 0x809530
void* _rwSListGetEnd(RwSList* sList); // 0x809540
RwBool RwIm2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices);
RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices, RwImVertexIndex* indices, RwInt32 numIndices);
RwBool RwIm2DRenderTriangle(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
RwBool RwIm2DRenderLine(RwIm2DVertex* vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwUInt32 RwEngineGetVersion(); // 0x7F2BA0
RwBool RwEngineInit(const RwMemoryFunctions* memFuncs, RwUInt32 initFlags, RwUInt32 resArenaSize); // 0x7F3170
RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor initCB, RwPluginObjectDestructor termCB); // 0x7F2BB0
RwInt32 RwEngineGetPluginOffset(RwUInt32 pluginID); // 0x7F2BE0
RwBool RwEngineOpen(RwEngineOpenParams* initParams); // 0x7F2F70
RwBool RwEngineStart(); // 0x7F2E70
RwBool RwEngineStop(); // 0x7F2E20
RwBool RwEngineClose(); // 0x7F2F00
RwBool RwEngineTerm(); // 0x7F3130
RwInt32 RwEngineGetNumSubSystems(); // 0x7F2C00
RwSubSystemInfo* RwEngineGetSubSystemInfo(RwSubSystemInfo* subSystemInfo, RwInt32 subSystemIndex); // 0x7F2C30
RwInt32 RwEngineGetCurrentSubSystem(); // 0x7F2C60
RwBool RwEngineSetSubSystem(RwInt32 subSystemIndex); // 0x7F2C90
RwInt32 RwEngineGetNumVideoModes(); // 0x7F2CC0
RwVideoMode RwEngineGetVideoModeInfo(RwInt32 modeIndex); // 0x7F2CF0
RwInt32 RwEngineGetCurrentVideoMode(); // 0x7F2D20
RwBool RwEngineSetVideoMode(RwInt32 modeIndex); // 0x7F2D50
RwInt32 RwEngineGetTextureMemorySize(); // 0x7F2D80
RwInt32 RwEngineGetMaxTextureSize(); // 0x7F2DB0
RwBool RwEngineSetFocus(RwBool enable); // 0x7F2DE0
RwMetrics* RwEngineGetMetrics(); // 0x7F2E10
RwFileFunctions* RwOsGetFileInterface(); // 0x804130
RwError* RwErrorGet(RwError* code); // 0x808880
RwError* RwErrorSet(RwError* code); // 0x808820
RwInt32 _rwerror(RwInt32 code, ...); // 0x8088D0
RwBool RwResourcesSetArenaSize(RwUInt32 size); // 0x8080C0
RwInt32 RwResourcesGetArenaSize(); // 0x8081B0
RwInt32 RwResourcesGetArenaUsage(); // 0x8081C0
RwBool RwResourcesEmptyArena(); // 0x8081F0
RwResEntry* RwResourcesAllocateResEntry(void* owner, RwResEntry* ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify); // 0x807ED0
RwBool RwResourcesFreeResEntry(RwResEntry* entry); // 0x807DE0
void _rwResourcesPurge(); // 0x807E50
RwBool RwStreamFindChunk(RwStream* stream, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut); // 0x7ED2D0
RwStream* _rwStreamWriteVersionedChunkHeader(RwStream* stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum); // 0x7ED270
RwStream* RwStreamWriteReal(RwStream* stream, const RwReal* reals, RwUInt32 numBytes); // 0x7ED3D0
RwStream* RwStreamWriteInt32(RwStream* stream, const RwInt32* ints, RwUInt32 numBytes); // 0x7ED460
RwStream* RwStreamWriteInt16(RwStream* stream, const RwInt16* ints, RwUInt32 numBytes); // 0x7ED480
RwStream* RwStreamReadReal(RwStream* stream, RwReal* reals, RwUInt32 numBytes); // 0x7ED4F0
RwStream* RwStreamReadInt32(RwStream* stream, RwInt32* ints, RwUInt32 numBytes); // 0x7ED540
RwStream* RwStreamReadInt16(RwStream* stream, RwInt16* ints, RwUInt32 numBytes); // 0x7ED4A0
RwStream* RwStreamReadChunkHeaderInfo(RwStream* stream, RwChunkHeaderInfo* chunkHeaderInfo); // 0x7ED590
