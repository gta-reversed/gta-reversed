/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   rtnormmap.h                                                -*
 *-                                                                         -*
 *-  Purpose :   Normal Maps toolkit public API                             -*
 *-                                                                         -*
 *===========================================================================*/

#if (!defined(RTNORMMAP_H))
#define RTNORMMAP_H

/**
 * \defgroup rtnormmap RtNormMap
 * \ingroup lighting
 *
 * Normal Maps Toolkit for RenderWare.
 */

/****************************************************************************
 Includes
 */

/*--- Include files ---*/
#include "rwcore.h"

//#include "rtnormmap.rpe"   /* automatically generated header file */

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwImage *
RtNormMapCreateFromImage(RwImage *image,
                         RwBool clamp,
                         RwReal bumpiness);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */

#endif                          /* (!defined(RTNORMMAP_H)) */
