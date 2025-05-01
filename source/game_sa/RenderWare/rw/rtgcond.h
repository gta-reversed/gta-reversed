/*
 * File :     rtgcond.h
 *
 * Abstract : Allows geometric (vertex and polygon) and attribute
 * (uvs, prelights, etc.) operations in a simple function independent
 * manner.
 *
 * Author: Adam James
 *
 * Copyright (c) 2002 Criterion Software Ltd.
 */

#ifndef RTGCOND_H
#define RTGCOND_H

/****************************************************************************
 Includes
 */
#include "rwcore.h"

#define GCONDR1TOL 0.0001f      /* Geometry within this tolerance is touching */
#define GCONDZERO 0.005f        /* Virtual zero for results of cross products, for example */

#if (!defined(DOXYGEN))

typedef struct GCondLimits GCondLimits;
struct GCondLimits
{
    RwReal  min;
    RwReal  max;
};

typedef struct GCondUVLimits GCondUVLimits;
struct GCondUVLimits
{
    GCondLimits   u;
    GCondLimits   v;
};

typedef struct GCondMaterialData GCondMaterialData;
struct GCondMaterialData
{
    GCondUVLimits   uvLimits[rwMAXTEXTURECOORDS];
};

#define GCONDGETMATERIALDATA(material)          \
    ((GCondMaterialData *)                      \
     (((RwUInt8 *)material)+                    \
      GCondMaterialDataOffset))

#define GCONDGETCONSTMATERIALDATA(material)     \
    ((const GCondMaterialData *)                \
     (((const RwUInt8 *)material)+              \
      GCondMaterialDataOffset))

#endif /* (!defined(DOXYGEN)) */

/* RWPUBLIC */

/**
 * \defgroup rtgcond RtGCond
 * \ingroup basicgeometry
 *
 * Geometry Conditioning toolkit for RenderWare Graphics.
 */

/****************************************************************************
 Global types
 */

/**
 * \ingroup rtgcond
 * \ref RtGCondFlag
 *
 * An enumeration of geometry attribute flags.
 */
typedef enum
{
    rtGCONDNORMALS   = 0x01, /**<has normals */
    rtGCONDTEXTURES  = 0x02, /**<has texture coordinates */
    rtGCONDPRELIT    = 0x04, /**<has luminance values */
    rtGCONDFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
}RtGCondFlag;

 
 
/*
 * typedef for struct \ref RtGCondParameters
 */
typedef struct RtGCondParameters RtGCondParameters;

/**
 * \ingroup rtgcond
 * \struct RtGCondParameters
 * Parameters used with provided geometry conditioning pipeline(s).
 * They are initialized to default values using \ref RtGCondParametersInit.
 * 
 */
struct RtGCondParameters
{
    RwInt32             flags;            /**< geometry attribute flags */

    RwReal              weldThreshold;    /**< positional threshold for vertex welding, -1.0f disables the test */
    RwReal              angularThreshold; /**< angular threshold for vertex welding, in degrees, -1.0f disables the test */
    RwReal              uvThreshold;      /**< uv threshold for vertex welding, -1.0f disables the test */
    RwReal              preLitThreshold;  /**< prelight threshold for vertex welding, -1.0f disables the test */
    RwReal              areaThreshold;    /**< area threshold for removal of zero area polygons, -1.0f disables the test */

    RwReal              uvLimitMin;       /**< minimum limit for uv coords */
    RwReal              uvLimitMax;       /**< maximum limit for uv coords */
    RwBool              texelLimits;       /**< Specifies if texel based UV limits should be used */

    RwTextureAddressMode textureMode[rwMAXTEXTURECOORDS];
    /**< For each element set to rwTEXTUREADDRESSWRAP, weld vertices according to
     *   mod(1) congruent texture coordinates
     * |0-1|0-1|0-1| -> |0-1|1-2|2-3| -> |0      3|
     */

    RwReal              polyNormalsThreshold;  /**< fractional range which polygons normal area have to fall within to be welded, -1.0f disables the test */
    RwReal              polyUVsThreshold;      /**< fractional range which polygons uv area have to fall within to be welded, -1.0f disables the test */
    RwReal              polyPreLitsThreshold;  /**< fractional range which polygons prelit area have to fall within to be welded, -1.0f disables the test */

    RwInt32 decimationMode; /**< Controls the way edge decimation proceeds, 1 is default, 2 is alternative */

    RwInt32 decimationPasses;
    /**< Determines the number of passes through the geometry looking for
     *   opportunities to weld polygon pairs
     */

    RwInt32 convexPartitioningMode;
    /**< Controls the way polygons are subdivided into triangles. Choose one
     *   of \ref RtWingEdgePartitioning
     */

};

/**
 * \ingroup rtgcond
 * \ref RtGCondDestroyVertexUserdataCallBack is the type for the callback function
 * for vertex destruction, for example, those welded
 *
 * \param pUserdata A handle to the userdata
 *
 * \return TRUE on success, FALSE otherwise
 *
 * \see RtGCondSetUserdataCallBacks
 */
typedef RwBool (*RtGCondDestroyVertexUserdataCallBack)(void **pUserdata);

/**
 * \ingroup rtgcond
 * \ref RtGCondCloneVertexUserdataCallBack Callback for vertex
 * cloning, for example, on unsharing
 *
 * \param pUserdataDst A handle to the userdata for the destination
 * \param pUserdataSrc A handle to the userdata for the source
 *
 * \return TRUE on success, FALSE otherwise
 *
 * \see RtGCondSetUserdataCallBacks
 */
typedef RwBool (*RtGCondCloneVertexUserdataCallBack)(void **pUserdataDst, void **pUserdataSrc);

/**
 * \ingroup rtgcond
 * \ref RtGCondInterpVertexUserdataCallBack Callback for vertex
 * interpolation, for example, sliver removal
 *
 * \param pUserdataDst A handle to the userdata for the destination
 * \param pUserdata1 A handle to the first source
 * \param pUserdata2 A handle to the second source
 * \param delta The delta between the two sources
 *
 * \return TRUE on success, FALSE otherwise
 *
 * \see RtGCondSetUserdataCallBacks
 */
typedef RwBool (*RtGCondInterpVertexUserdataCallBack)(void **pUserdataDst,
                      void **pUserdata1, void **pUserdata2, RwReal delta);

/**
 * \ingroup rtgcond
 * \ref RtGCondDestroyPolygonUserdataCallBack Callback for polygon
 * destruction, for example, zero areas
 *
 * \param pUserdata A handle to the userdata
 *
 * \return TRUE on success, FALSE otherwise
 *
 * \see RtGCondSetUserdataCallBacks
 */
typedef RwBool (*RtGCondDestroyPolygonUserdataCallBack)(void **pUserdata);

/**
 * \ingroup rtgcond
 * \ref RtGCondSplitPolygonUserdataCallBack Callback for polygon
 * division, for example, convex partitioning
 *
 * \param pUserdataDst A handle to the userdata for the destination
 * \param pUserdataSrc A handle to the userdata for the source
 *
 * \return TRUE on success, FALSE otherwise
 *
 * \see RtGCondSetUserdataCallBacks
 */
typedef RwBool (*RtGCondSplitPolygonUserdataCallBack)(void **pUserdataDst,
                                                      void **pUserdataSrc);

/*
 * typedef for struct \ref RtGCondUserdataCallBacks
 */
typedef struct RtGCondUserdataCallBacks RtGCondUserdataCallBacks;

/**
 * \ingroup rtgcond
 * \struct RtGCondUserdataCallBacks
 * Collection of callbacks
 */
struct RtGCondUserdataCallBacks
{
    RtGCondCloneVertexUserdataCallBack cloneVertexUserdata;         /**< Callback on vertex cloning */
    RtGCondInterpVertexUserdataCallBack interpVertexUserdata;       /**< Callback on vertex interpolation */
    RtGCondSplitPolygonUserdataCallBack splitPolygonUserdata;       /**< Callback on polygon division */
    RtGCondDestroyVertexUserdataCallBack destroyVertexUserdata;     /**< Callback on vertex destruction */
    RtGCondDestroyPolygonUserdataCallBack destroyPolygonUserdata;   /**< Callback on polygon destruction */
};

typedef struct RtGCondVertex RtGCondVertex;
/**
 * \ingroup rtgcond
 * \struct RtGCondVertex
 * Holds data for each vertex.
 *
 */
struct RtGCondVertex
{
    RwV3d           position;           /**< World space vertex position */
    RwInt32         index;              /**< unique id */
    RwV3d           normal;             /**< World space vertex normal */
    RwRGBA          preLitCol;          /**< Prelight color */
    RwInt32         matIndex;           /**< Vertex material index */
    RwInt32         *brauxIndices;      /**< Internal use only. */
    RwInt32         numBrauxIndices;    /**< Internal use only. */
    void            *pUserData;         /**< Pointer to unspecified per vertex user data */
    RwTexCoords     texCoords[rwMAXTEXTURECOORDS];  /**< Vertex texture coordinates */
};

typedef struct RtGCondPolygon RtGCondPolygon;
/**
 * \ingroup rtgcond
 * \struct RtGCondPolygon
 * Holds data for each polygon.
 *
 */
struct RtGCondPolygon
{
    RwInt32     id;                     /**< unique id */
    RwInt32     matIndex;               /**< The material index */

    RwInt32     *indices;               /**< contiguous list of vertex indices */
    RwInt32     numIndices;             /**< number of vertex indices */

    void        *pUserData;             /**< Pointer to unspecified per polygon user data */
    void        *pInternalData;         /**< Internal use only */
};

typedef struct RtGCondGeometryList RtGCondGeometryList;
/**
 * \ingroup rtgcond
 * \struct RtGCondGeometryList
 * Container for vertices and polygons
 *
 */
struct RtGCondGeometryList
{
    RpMaterialList  matList;            /**< Material list */
    RwInt32         numUVs;             /**< Number of UV sets in the geometry */

    RtGCondVertex   *vertices;          /**< Vertex list */
    RwInt32         numVertices;        /**< Number of vertices in list */

    RtGCondPolygon  *polygons;          /**< Polygon list */
    RwInt32         numPolygons;        /**< Number of polygons in list */
};

typedef void (*RtGCondGeometryConditioningPipeline) (RtGCondGeometryList *geometryList);
extern RtGCondUserdataCallBacks GCUserDataCallBacks;


/****************************************************************************
 Function prototypes
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
extern void
RtGCondSetUserdataCallBacks(RtGCondCloneVertexUserdataCallBack
                                cloneVertexUserdataCB,
                            RtGCondInterpVertexUserdataCallBack
                                interpVertexUserdataCB,
                            RtGCondSplitPolygonUserdataCallBack
                                splitPolygonUserdataCB,
                            RtGCondDestroyVertexUserdataCallBack
                                destroyVertexUserdataCB,
                            RtGCondDestroyPolygonUserdataCallBack
                                destroyPolygonUserdataCB);

extern void     RtGCondSetGeometryConditioningPipeline(
                    RtGCondGeometryConditioningPipeline pipeline);
extern RtGCondGeometryConditioningPipeline
                RtGCondGetGeometryConditioningPipeline( void );
extern void     RtGCondApplyGeometryConditioningPipeline(RtGCondGeometryList *geometryList);

/* Pipeline node functions... */
extern void     RtGCondSnapPipelineNode(RtGCondGeometryList *geometryList, RwReal weldThreshold);
extern void     RtGCondSnapUVsPipelineNode(RtGCondGeometryList *geometryList,
                                           RwReal weldThreshold);
extern void     RtGCondCullZeroAreaPolygonsPipelineNode(RtGCondGeometryList *geometryList,
                                                        RwReal areaThreshold);
extern void     RtGCondLimitUVsPipelineNode(RtGCondGeometryList * geometryList,
                                            RwReal limitUVMin, RwReal limitUVMax,
                                            RwBool texelLimits);
extern void     RtGCondRemapVerticesPipelineNode(RtGCondGeometryList * geometryList);
extern void     RtGCondUnshareVerticesOnMaterialBoundariesPipelineNode(
                                      RtGCondGeometryList* geometryList);
extern RwBool   RtGCondUnshareVerticesPipelineNode(RtGCondGeometryList *geometryList);
extern void     RtGCondRemoveSliversPipelineNode(RtGCondGeometryList* geometryList);
extern void     RtGCondRemoveIdenticalPolygonsPipelineNode(RtGCondGeometryList* geometryList);
extern void     RtGCondSortVerticesOnMaterialPipelineNode(RtGCondGeometryList * geometryList);
extern RwInt32  *RtGCondWeldVerticesPipelineNode(RtGCondGeometryList *geometryList,
                                                 RwReal weldThreshold,
                                                 RwReal normalWeldThreshold, RwReal uvThreshold,
                                                 RwReal preLitThreshold, RwBool ignoreTextures,
                                                 RwBool remap, RwBool implicit,
                                                 RwBool averageAttributes);
extern void     RtGCondBuildNormalsPipelineNode(RtGCondGeometryList * geometryList);

/* Supplied pipelines... */
extern RwBool   RtGCondFixAndFilterGeometryPipeline(RtGCondGeometryList *geometryList);
extern RwBool   RtGCondDecimateAndWeldGeometryPipeline(RtGCondGeometryList *geometryList);

/* Allocation functions. */
extern RwBool   RtGCondAllocateVertices(RtGCondGeometryList* geometryList, RwInt32 num);
extern RwBool   RtGCondReallocateVertices(RtGCondGeometryList* geometryList, RwInt32 num);
extern RwBool   RtGCondAllocatePolygons(RtGCondGeometryList* geometryList, RwInt32 num);
extern RwBool   RtGCondReallocatePolygons(RtGCondGeometryList* geometryList, RwInt32 num);
extern RwBool   RtGCondAllocateIndices(RtGCondPolygon* polygon, RwInt32 num);
extern RwBool   RtGCondReallocateIndices(RtGCondPolygon* polygon, RwInt32 num);

/* Parameter functions. */
extern void     RtGCondParametersInit(RtGCondParameters* gcParams);
extern void     RtGCondParametersSet(RtGCondParameters* gcParams);
extern RtGCondParameters
                *RtGCondParametersGet(void);

/* Utility functions. */
extern RwBool   RtGCondNormalize(RwV3d* vec);
extern RwReal   RtGCondLength(RwV3d* vec);
extern RwReal   RtGCondAreaOfPolygon(RtGCondGeometryList* geometryList,
                                   RtGCondPolygon* polygon);
extern void     _rtGCondCopyVertexInfo(RtGCondVertex* from, RtGCondVertex* to, RwInt32 index);
extern void     RtGCondFreeIndices(RtGCondPolygon* polygon);
extern void     RtGCondFreeVertices(RtGCondGeometryList* geometryList);
extern void     RtGCondFreePolygons(RtGCondGeometryList* geometryList);
extern void     _rtGCondDestroyVertexPolygonReference(RtGCondGeometryList* geometryList);
extern void     _rtGCondCreateVertexPolygonReference(RtGCondGeometryList* geometryList,
                                                RwInt32* remapper);
extern RwBool   RtGCondColinearVertices(RwV3d* v1, RwV3d* v2, RwV3d* v3,
                                        RwBool strictOrder, RwReal tol);
extern RwBool   RtGCondVectorsEqual(RwV3d* v1, RwV3d* v2, RwReal tol);
extern void     RtGCondGroundUVs(RtGCondGeometryList * geometryList);

extern RwBool   RtGCondInitialize(void);
extern void     RtGCondMaterialGetUVLimits(RpMaterial *mat, RwInt32 uvSet,
                                           RwReal *uMin, RwReal *uMax,
                                           RwReal *vMin, RwReal *vMax);
extern void     RtGCondMaterialSetUVLimits(RpMaterial *mat, RwInt32 uvSet,
                                           RwReal uMin, RwReal uMax,
                                           RwReal vMin, RwReal vMax);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define _rtImportBuildSectorGetWeldThresholdPositional() \
        _rtGCondGetWeldThresholdPositional()

#define _rtImportBuildSectorGetWeldThresholdAngular() \
        _rtGCondGetWeldThresholdAngular()

#define _rtImportBuildSectorSetWeldThresholdPositional(_componentThreshold) \
        _rtGCondSetWeldThresholdPositional(_componentThreshold)

#define _rtImportBuildSectorSetWeldThresholdAngular(_cosineThreshold) \
        _rtGCondSetWeldThresholdAngular(_cosineThreshold)

#endif /* RTGCOND_H */

/* RWPUBLICEND */
