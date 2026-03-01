
/* *INDENT-OFF* */

/* RWPUBLIC */

/****************************************************************************
 *
 * File: rwg/rwsdk/plugin/adc/rpadc.h
 *
 * Copyright (C) 2002 Criterion Technologies.
 *
 * Purpose: RenderWare ADC flag generation plugin
 *
 ****************************************************************************/

#if (!defined(_RP_ADC_H))
#define _RP_ADC_H

/**
 * \defgroup rpadc RpADC
 * \ingroup basicgeometry
 *
 * Address Control flag generation plugin for RenderWare Graphics.
 */


/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/
#include <rwcore.h>

/*===========================================================================*
 *--- Private Types ---------------------------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rpadc
 * \struct RpADCDataTag
 * A structure that handles an object ADC support information.
 */
typedef struct RpADCDataTag
{
    RwBool  adcFormatted; /**< ADC support flag */
    RwInt8 *adcBits;      /**< Pointer to the object ADC flag array */
    RwInt32 numBits;      /**< Number of Bits used for streaming */
} RpADCData;

/**
 * \ingroup rpadc
 * \ref RpADCMeshCallback
 * Optional per-mesh callback, allowing the application to opt out of
 * conversion of specific meshes by returning FALSE.
 *
 * \param mesh Pointer to the mesh.
 *
 * \return TRUE to generate ADC flags.
 */
typedef RwBool (* RpADCMeshCallback)(RpMesh *mesh);

/* RWPUBLICEND */

/*===========================================================================*
 *--- Private Variables -----------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private Defines -------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private Functions -----------------------------------------------------*
 *===========================================================================*/

/* RWPUBLIC */

#if (defined(__cplusplus))
extern "C"
{
#endif /* (defined(__cplusplus)) */

extern RwBool
RpADCPluginAttach(void);

extern RwBool
RpADCGeometryConvertIgnoreWinding(RpGeometry *geometry);

extern RwBool
RpADCWorldSectorConvertIgnoreWinding(RpWorldSector *sector);

extern RwBool
RpADCGeometryConvertPreserveWinding(RpGeometry *geometry);

extern RwBool
RpADCWorldSectorConvertPreserveWinding(RpWorldSector *sector);

extern RwInt8 *
RpADCGeometryGetADCSupport(RpGeometry *geometry);

extern RwInt8 *
RpADCWorldSectorGetADCSupport(RpWorldSector *sector);

extern RpADCMeshCallback
RpADCGetMeshCallback(void);

extern void
RpADCSetMeshCallback(RpADCMeshCallback callback);

#if (defined(__cplusplus))
}
#endif /* (defined(__cplusplus)) */

#endif /* (!defined(_RP_ADC_H)) */

/* RWPUBLICEND */

/* *INDENT-ON* */
