
/***************************************************************************
 *                                                                         *
 * Module  : rtwing.h                                                      *
 *                                                                         *
 * Purpose : Winged-edge (half-edge) algorithms                            *
 *                                                                         *
 **************************************************************************/

#ifndef RTWING_H
#define RTWING_H

/****************************************************************************
 Includes
 */

#include "rwcore.h"
#include "rtimport.h"
#include "rtgcond.h"

/* RWPUBLIC */


/****************************************************************************
 Defines
 */

/**
 * \defgroup rtwing RtWing
 * \ingroup basicgeometry
 * 
 * Winged edge/half-edge toolkit for RenderWare Graphics.
 */


/****************************************************************************
 Typedefs
 */
typedef struct RtWings RtWings; /* container for the faces, vertices and edges */
typedef struct RtWingFace RtWingFace;
typedef struct RtWingVertex RtWingVertex;
typedef struct RtWingEdge RtWingEdge;

/**
 * \ingroup rtwing
 * \struct RtWingFace
 * Holds data for each face in the winged edge data structure.
 */
struct RtWingFace
{
    RtWingEdge*     edge;           /**< pointer to one of the edges */
    RtGCondPolygon* polygonInfo;    /**< pointer to RtGCond polygon rep. */
    RwV3d           normal;         /**< INTERNAL USE ONLY */
    RwReal          value;          /**< INTERNAL USE ONLY */
};

/**
 * \ingroup rtwing
 * \struct RtWingVertex
 * Holds data for each vertex in the winged edge data structure.
 */
struct RtWingVertex
{
    RtWingEdge*     edge;           /**< pointer to one of the edges */
    RtGCondVertex*  vertexInfo;     /**< pointer to RtGCond vertex rep. */
    RwReal          value;          /**< INTERNAL USE ONLY */
};

/**
 * \ingroup rtwing
 * \ref RtWingEdgeNeighborRelation
 *
 * An enumeration that tags an edge with neighbor information to help
 * queries and filter operations.
 */
typedef enum 
{
    rtWINGEDGENULL = 0,             /**< The edge has no neighbor */
    rtWINGEDGECREASE,               /**< The edge has a neighbor with mismatching attributes, e.g. face normal or material */
    rtWINGEDGECONTINUOUS,           /**< The edge has a neighbor with matching attributes */
    rtWINGEDGEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
}
RtWingEdgeNeighborRelation;

/**
 * \ingroup rtwing
 * \ref RtWingEdgePartitioning
 *
 * An enumeration that provides a value for the convex partitioner.
 */
typedef enum 
{
    rtWINGEDGEPARTITIONFAN = 0,     /**< Polygon is partitioned to help tri-fanning */
    rtWINGEDGEPARTITIONTACK,        /**< Polygon is partitioned to help tri-stripping */
    rtWINGEDGEPARTITIONEAR,         /**< Polygon is partitioned successfully if concave */
    rtWINGEDGEPARTITIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
}
RtWingEdgePartitioning;

/**
 * \ingroup rtwing
 * \ref RtWingDecimationApproach
 *
 * An enumeration that provides a value for edge decimation. Note, this is only
 * a tie-breaker and should not be considered a high control priority.
 */
typedef enum 
{
    rtWINGEDGEDECIMATIONFEW = 0,     /**< Polygon is partitioned to reduce triangles */
    rtWINGEDGEDECIMATIONSMALL,       /**< Polygon is partitioned to reduce size */
    rtWINGEDGEDECIMATIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
}
RtWingDecimationApproach;

/**
 * \ingroup rtwing
 * \struct RtWingEdge
 * Holds data for each edge in the winged edge data structure.
 */
struct RtWingEdge
{
    RtWingEdge*     neighbor;                       /**< pointer to neighbor, if any */
    RtWingEdgeNeighborRelation neighborRelation;    /**< relationship info */

    RtWingVertex*   vertex;                         /**< pointer to vertex */
    RtWingFace*     face;                           /**< pointer to face */

    RtWingEdge*     next;                           /**< pointer to next edge */
    RtWingEdge*     prev;                           /**< pointer to previous edge (non-manifold requirement) */

    RwReal          value; /**< INTERNAL USE ONLY */
};

/**
 * \ingroup rtwing
 * \struct RtWings
 * Wrapper for wing each faces, edges and vertices.
 */
struct RtWings
{
    RtWingFace*     wingFaces;      /**< contiguous list of faces */
    RwInt32 wingFacesSize;          /**< number of (space allocated for) faces */
    RtWingVertex*   wingVertices;   /**< contiguous list of vertices */
    RwInt32 wingVerticesSize;       /**< number of (space allocated for) vertices */
    RtWingEdge*     wingEdges;      /**< contiguous list of edges */
    RwInt32 wingEdgesSize;          /**< number of (space allocated for) edges */
};

typedef RtWingEdge*
(*RtWingEdgeCallBack) (RtWingEdge* wingEdge, void* userData);
typedef RtWingFace*
(*RtWingFaceCallBack) (RtWingFace* wingFace, void* userData);
typedef RtWingVertex*
(*RtWingVertexCallBack) (RtWingVertex* wingVertex, void* userData);

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern RtWingFace   *RtWingFaceForAllEdges(RtWingFace* wingFace,
                        RtWingEdgeCallBack callBack, void* userData);
extern RtWingFace   *RtWingFaceForAllFaces(RtWingFace* wingFace,
                        RtWingFaceCallBack callBack, void* userData);
extern RtWingVertex *RtWingVertexForAllEdges(RtWingVertex* wingVertex,
                        RtWingEdgeCallBack callBack, void* userData);
extern RtWingVertex *RtWingVertexForAllFaces(RtWingVertex* wingVertex,
                        RtWingFaceCallBack callBack, void* userData);

void                RtWingMatchUVsAndPromoteContinuity(RtWings* wings,
                                    RtGCondGeometryList* geometryList);
void                RtWingUpdateInternalRepresentation(RtWingFace* face, RtWingEdge* edge,
                                                       RtWingVertex* vertex);

extern RwBool       RtWingCreate(RtWings* wings, RtGCondGeometryList* geometryList);
extern void         RtWingEdgeDecimation(RtWings* wings, RtGCondGeometryList *geometryList);
extern void         RtWingDestroy(RtWings* wings);
extern void         RtWingPartitionPolygon(RtWings* wings, RtWingFace* face,
                       RtGCondGeometryList* geometryList,
                       RtWingEdgePartitioning how, RwInt32 ctr);
extern void         RtWingConvexPartitioning(RtWings* wings,
                        RtGCondGeometryList *geometryList, RtWingEdgePartitioning how);
extern RtWingEdge   *RtWingReallocateWingEdges(RtWings* wings, RwInt32 extraEdges);
extern RtWingVertex *RtWingReallocateWingVertices(RtWings* wings,
                        RtGCondGeometryList *geometryList, RwInt32 extraVertices);
extern RtWingFace   *RtWingReallocateWingFaces(RtWings* wings,
                        RtGCondGeometryList *geometryList, RwInt32 extraFaces);


#ifdef    __cplusplus
}
#endif                          /* __cplusplus */


#endif                          /* RTWING_H */

/* RWPUBLICEND */
