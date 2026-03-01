
/*****************************************************************************
 *
 * File :     rpcollis.h
 *
 * Abstract : Renderware Graphics Collision Plugin.
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
 * Copyright (c) 2003 Criterion Software Ltd.
 * All Rights Reserved.
 *
 * RenderWare is a trademark of Canon Inc.
 *
 *****************************************************************************/

#ifndef RPCOLLIS_H
#define RPCOLLIS_H

/******************************************************************************
 *  Include files
 */

#include "rwcore.h"
#include "rpworld.h"

//#include "rpcollis.rpe"    /* automatically generated header file */

/*---- start: ./ctdata.h----*/

/******************************************************************************
 *  Defines
 */

#define rpCOLLTREE_MAXDEPTH                 32

#define rpCOLLSPLIT_ALIGNMENT               16

/* Allow 16 special contents values */
#define rpCOLLSECTOR_CONTENTS_MAXCOUNT      0xef
#define rpCOLLSECTOR_CONTENTS_SPLIT         0xff

#define rpCOLLSECTOR_TYPE_NEG               0x01
#define rpCOLLSECTOR_TYPE_AXISMASK          (0x08 | 0x04)
#define rpCOLLSECTOR_TYPE_X                 0x00
#define rpCOLLSECTOR_TYPE_Y                 0x04
#define rpCOLLSECTOR_TYPE_Z                 0x08

/******************************************************************************
 *  Types
 */

/**
 * \ingroup rpcollis
 * \struct  RpCollSector
 * This opaque type represents a node of an \ref RpCollTree, and corresponds
 * to a region of space within an axis aligned bounding box. A branch sector
 * references a pair of overlapping sub-sectors, and a leaf sector references 
 * any entries that are contained within the region.
 */
typedef struct RpCollSector RpCollSector;


/**
 * \ingroup rpcollis
 * \struct  RpCollTree
 * This opaque type represents a collision tree, which uses overlapping
 * sectors to recursively divide a set of 'entries' into smaller groups.
 * The entries would normally be the triangles of a world 
 * sector or geometry, but in principle could be anything contained by a 
 * bounding box (hence the name 'entries' rather than 'triangles').
 *
 * The external entries may be sorted according to their location in the 
 * collision tree, such that any node of the tree corresponds to a contiguous
 * block of the ordered set. Alternatively, a remapping array is stored with 
 * the tree (see \ref RpCollTreeGetEntryMap). The leaf nodes of the tree 
 * reference the external entries that they contain by specifying a slice of
 * the ordered array.
 *
 * Functions are available for iterating over a collision tree to find all
 * leaf sectors that intersect a given line or bounding box.
 *
 * \see RpCollisionWorldSectorBuildData
 * \see RpCollisionGeometryBuildData
 * \see RpCollisionWorldSectorGetCollTree
 * \see RpCollisionGeometryGetCollTree
 * \see RpCollSectorLineItInit
 * \see RpCollSectorBBoxItInit
 * \see \ref rpcollisoverview
 */
typedef struct RpCollTree RpCollTree;


#if !defined(DOXYGEN)

enum RpCollTreeFlags
{
    rpCOLLTREE_USEMAP        = 0x01,

    rpCOLLTREEFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpCollTreeFlags RpCollTreeFlags;

struct RpCollSector
{
    RwUInt8         type;
    RwUInt8         contents;
    RwUInt16        index;
    RwReal          value;
};

typedef struct RpCollSplit RpCollSplit;
struct RpCollSplit
{
    RpCollSector    leftSector;
    RpCollSector    rightSector;
};

struct RpCollTree
{
    RwUInt32        flags;
    RwBBox          bbox;
    RwUInt16        numEntries;
    RwUInt16        numSplits;
    RpCollSplit    *splits;
    RwUInt16       *map;
};

#endif /* !defined(DOXYGEN) */


/******************************************************************************
 *  Macros
 */

#define RpCollTreeGetEntryMapMacro(_tree)  ((_tree)->map)

#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))
#define RpCollTreeGetEntryMap(_tree)  RpCollTreeGetEntryMapMacro(_tree)
#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/******************************************************************************
 *  Functions
 */

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RpCollTree *
_rpCollTreeCreate(RwUInt16 numEntries,
                  RwUInt16 numSplits, 
                  RwBBox   *bbox,
                  RwUInt32  flags);

extern void
_rpCollTreeDestroy(RpCollTree *tree);

/* 
 *  Streaming
 */
extern RwUInt32
_rpCollTreeStreamGetSize(const RpCollTree *tree);

extern const RpCollTree *
_rpCollTreeStreamWrite(const RpCollTree *tree, RwStream *stream);

extern RpCollTree *
_rpCollTreeStreamRead(RwStream *stream);


#if (defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

extern RwUInt16 *
RpCollTreeGetEntryMap(RpCollTree *tree);

#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./ctdata.h----*/
/*---- start: ./ctbuild.h----*/
/*---- end: ./ctbuild.h----*/
/*---- start: ./ctquery.h----*/

#if !defined(DOXYGEN)

/* RpSector - BBox iterator */

typedef struct RpSectorBBoxItNodeInfo RpSectorBBoxItNodeInfo;
struct RpSectorBBoxItNodeInfo 
{
    RpSector   *sector; 
};

struct RpSectorBBoxIt
{
    RwBBox                  bbox;
    RpSectorBBoxItNodeInfo  stack[rpWORLDMAXBSPDEPTH+1];
    RpSectorBBoxItNodeInfo *curr;
};

/* RpCollSector - BBox iterator */

typedef struct RpCollSectorBBoxItNodeInfo RpCollSectorBBoxItNodeInfo;
struct RpCollSectorBBoxItNodeInfo 
{
    RpCollSector   *sector; 
    RwUInt32        clipFlags; /* RpCollBBoxClipFlags */
};

struct RpCollSectorBBoxIt
{
    RpCollTree                 *tree;
    RwBBox                      bbox; /* Used for rejection tests. Should be
                                           completely outside object. */
    RwBBox                      clipBBox;   /* Used for setting clip flags. Should 
                                           be completely inside object. */
    RpCollSector                dummy;
    RpCollSectorBBoxItNodeInfo  stack[rpCOLLTREE_MAXDEPTH+1];
    RpCollSectorBBoxItNodeInfo *curr;
};

/* RpSector - Line iterator */

typedef struct RpSectorLineItNodeInfo RpSectorLineItNodeInfo;
struct RpSectorLineItNodeInfo 
{
    RpSector   *sector; 
    RwReal      pa; /* Start parameter for segment of line in sector */
    RwReal      pb; /* End parameter for segment of line in sector */
};

struct RpSectorLineIt
{
    RwV3d                   start;
    RwV3d                   delta;
    RwV3d                   recip;
    RwV3d                   padding;
    RpSectorLineItNodeInfo  stack[rpWORLDMAXBSPDEPTH+1];
    RpSectorLineItNodeInfo *curr;
};

/* RpCollSector - Line iterator */

typedef struct RpCollSectorLineItNodeInfo RpCollSectorLineItNodeInfo;
struct RpCollSectorLineItNodeInfo
{
    RpCollSector   *sector;
    RwReal          pa; /* Start parameter for segment of line in sector */
    RwReal          pb; /* End parameter for segment of line in sector */
};

struct RpCollSectorLineIt
{
    RpCollTree                 *tree;
    RwV3d                       start;
    RwV3d                       delta;
    RwV3d                       recip;
    RwV3d                       padding;
    RpCollSector                dummy;
    RpCollSectorLineItNodeInfo  stack[rpCOLLTREE_MAXDEPTH+1];
    RpCollSectorLineItNodeInfo *curr;
};

#endif /* !defined(DOXYGEN) */


/**
 * \ingroup rpcollis
 * RpCollBBoxClipFlags are used to indicate the faces of an \ref RwBBox 
 * that clip a leaf sector of a collision tree (\ref RpCollTree). The shape 
 * of the leaf sector is also an axis aligned bounding box. If the flags are 
 * zero, then the sector is entirely inside the clip box.
 *
 * \see RpCollSectorBBoxIt
 */
enum RpCollBBoxClipFlags
{
    rpCOLLBBOXCLIP_INFX = 0x01, /**< Clipped by infimum  on x-axis */
    rpCOLLBBOXCLIP_SUPX = 0x02, /**< Clipped by supremum on x-axis */
    rpCOLLBBOXCLIP_INFY = 0x04, /**< Clipped by infimum  on y-axis */
    rpCOLLBBOXCLIP_SUPY = 0x08, /**< Clipped by supremum on y-axis */
    rpCOLLBBOXCLIP_INFZ = 0x10, /**< Clipped by infimum  on z-axis */
    rpCOLLBBOXCLIP_SUPZ = 0x20, /**< Clipped by supremum on z-axis */

    rpCOLLBBOXCLIP_MASK = 0x3f,

    rpCOLLBBOXCLIPFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

typedef enum RpCollBBoxClipFlags RpCollBBoxClipFlags;


/**
 * \ingroup rpcollis
 * \struct  RpSectorBBoxIt
 * This opaque type is an iterator object used for traversing the RpSector 
 * tree of an \ref RpWorld to find each \ref RpWorldSector (a leaf 
 * node of the tree) that intersects a given \ref RwBBox.
 *
 * \see RpSectorBBoxItInit
 * \see RpSectorBBoxItFinished
 * \see RpSectorBBoxItFindNextWorldSector
 */
typedef struct RpSectorBBoxIt RpSectorBBoxIt;


/**
 * \ingroup rpcollis
 * \struct  RpCollSectorBBoxIt
 * This opaque type is an iterator object used for traversing the 
 * \ref RpCollSector nodes of an \ref RpCollTree to find the leaf nodes that 
 * intersect a given \ref RwBBox. 
 *
 * \see RpCollSectorBBoxItInit
 * \see RpCollSectorBBoxItFinished
 * \see RpCollSectorBBoxItFindNextEntries
 */
typedef struct RpCollSectorBBoxIt RpCollSectorBBoxIt;


/**
 * \ingroup rpcollis
 * \struct  RpSectorLineIt
 * This opaque type is an iterator object used for traversing the 
 * RpSector tree of an \ref RpWorld to find each \ref RpWorldSector
 * (a leaf node of the tree) that intersects a given line. 
 *
 * \see RpSectorLineItInit
 * \see RpSectorLineItFinished
 * \see RpSectorLineItFindNextWorldSector
 * \see RpSectorLineItSetFarClip
 */
typedef struct RpSectorLineIt RpSectorLineIt;


/**
 * \ingroup rpcollis
 * \struct  RpCollSectorLineIt
 * This opaque type is an iterator object used for traversing the 
 * \ref RpCollSector nodes of an \ref RpCollTree to find the leaf nodes
 * that intersect a given line.
 *
 * \see RpCollSectorLineItInit
 * \see RpCollSectorLineItInitFromSectorIt
 * \see RpCollSectorLineItFinished
 * \see RpCollSectorLineItFindNextEntries
 * \see RpCollSectorLineItSetFarClip
 */
typedef struct RpCollSectorLineIt RpCollSectorLineIt;


/******************************************************************************
 *  Macros
 */

/*
 *  Line and BBox Iterators - test for completion
 */

#define _rpSectorLineItFinishedMacro(_it)     ((_it)->curr <= (_it)->stack)
#define _rpSectorBBoxItFinishedMacro(_it)     ((_it)->curr <= (_it)->stack)
#define _rpCollSectorLineItFinishedMacro(_it) ((_it)->curr <= (_it)->stack)
#define _rpCollSectorBBoxItFinishedMacro(_it) ((_it)->curr <= (_it)->stack)

#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpSectorLineItFinished(_it)     _rpSectorLineItFinishedMacro(_it)
#define RpSectorBBoxItFinished(_it)     _rpSectorBBoxItFinishedMacro(_it)
#define RpCollSectorLineItFinished(_it) _rpCollSectorLineItFinishedMacro(_it)
#define RpCollSectorBBoxItFinished(_it) _rpCollSectorBBoxItFinishedMacro(_it)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */


#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if (defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

extern RwBool
RpSectorBBoxItFinished(RpSectorBBoxIt *it);

extern RwBool
RpSectorLineItFinished(RpSectorLineIt *it);

extern RwBool
RpCollSectorBBoxItFinished(RpCollSectorBBoxIt *it);

extern RwBool
RpCollSectorLineItFinished(RpCollSectorLineIt *it);

#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/*
 *  RpSector iterators 
 */

extern void
RpSectorBBoxItInit(RpSectorBBoxIt *it, 
                   RpWorld        *world,
                   RwBBox         *bbox);

extern RpWorldSector *
RpSectorBBoxItFindNextWorldSector(RpSectorBBoxIt *it);

extern void
RpSectorLineItInit(RpSectorLineIt *it, 
                   RpWorld        *world,
                   RwLine         *line,
                   RwV3d          *padding);

extern RpWorldSector *
RpSectorLineItFindNextWorldSector(RpSectorLineIt *it);

extern void
RpSectorLineItSetFarClip(RpSectorLineIt *it,
                         RwReal          value);

/*
 *  RpCollSector iterators 
 */

extern void
RpCollSectorBBoxItInit(RpCollSectorBBoxIt *it, 
                       RpCollTree         *tree,
                       RwBBox             *outerBBox,
                       RwBBox             *innerBBox);

extern void
RpCollSectorBBoxItFindNextEntries(RpCollSectorBBoxIt  *it, 
                                  RwUInt16            *start, 
                                  RwUInt16            *count,
                                  RwUInt32            *clipFlags);

extern void
RpCollSectorLineItInit(RpCollSectorLineIt  *it, 
                       RpCollTree          *tree,
                       RwLine              *line,
                       RwV3d               *padding);

extern void
RpCollSectorLineItInitFromSectorIt(RpCollSectorLineIt  *it, 
                                   RpCollTree          *tree,
                                   RpSectorLineIt      *sectIt);

extern void
RpCollSectorLineItFindNextEntries(RpCollSectorLineIt *it, 
                                  RwUInt16            *start, 
                                  RwUInt16            *count);

extern void
RpCollSectorLineItSetFarClip(RpCollSectorLineIt *it,
                             RwReal             value);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./ctquery.h----*/

/*---- start: ./colldata.h----*/

/******************************************************************************
 *  API types/globals/macros
 */

#if !defined(DOXYGEN)
typedef struct RpCollisionData RpCollisionData;
struct RpCollisionData
{
    RpCollTree  *tree;
};
#endif /* !defined(DOXYGEN) */

#define _rpCollisionGeometryGetDataMacro(_geometry) \
    RWPLUGINOFFSET(RpCollisionData, _geometry, _rpCollisionGeometryDataOffset)

#define _rpCollisionWorldSectorGetDataMacro(_sector) \
    RWPLUGINOFFSET(RpCollisionData, _sector, _rpCollisionWorldSectorDataOffset)


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpCollisionGeometryGetCollTree(_geometry) \
    (_rpCollisionGeometryGetDataMacro(_geometry)->tree)

#define RpCollisionWorldSectorGetCollTree(_sector) \
    (_rpCollisionWorldSectorGetDataMacro(_sector)->tree)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */


/******************************************************************************
 *  Functions
 */

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwInt32 _rpCollisionGeometryDataOffset;
extern RwInt32 _rpCollisionWorldSectorDataOffset;

/*
 *  Collision data
 */

extern RwBool
RpCollisionPluginAttach(void);

/* 
 *  Worlds
 */
extern RwBool
RpCollisionWorldQueryData(RpWorld *world);

extern RpWorld *
RpCollisionWorldDestroyData(RpWorld *world);

/*
 *  World Sectors  
 */

extern RpWorldSector *
RpCollisionWorldSectorDestroyData(RpWorldSector *worldSector);

extern RwBool
RpCollisionWorldSectorQueryData(RpWorldSector *worldSector);

#if (defined(RWDEBUG) || defined(RWSUPPRESSINLINE))
extern RpCollTree *
RpCollisionWorldSectorGetCollTree(RpWorldSector *worldSector);
#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

extern RpWorldSector *
_rpCollisionWorldSectorSetCollTree(RpWorldSector *worldSector, 
                                   RpCollTree *collTree);
/*
 *  Geometries
 */

extern RpGeometry *
RpCollisionGeometryDestroyData(RpGeometry *geometry);

extern RwBool
RpCollisionGeometryQueryData(RpGeometry *geometry);

#if (defined(RWDEBUG) || defined(RWSUPPRESSINLINE))
extern RpCollTree *
RpCollisionGeometryGetCollTree(RpGeometry *geometry);
#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

extern RpGeometry *
_rpCollisionGeometrySetCollTree(RpGeometry *geometry, 
                                RpCollTree *collTree);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./colldata.h----*/
/*---- start: ./collbuild.h----*/

/******************************************************************************
 *  Types
 */

/**
 * \ingroup rpcollis
 * These flags are used in an \ref RpCollisionBuildParam structure to
 * control the generation of collision data for an \ref RpGeometry or
 * \ref RpWorldSector.
 *
 * \see RpCollisionGeometryBuildData
 * \see RpCollisionWorldSectorBuildData
 * \see RpCollisionWorldBuildData
 */
enum RpCollisionBuildFlags
{
    rpCOLLISIONBUILD_SORTTRIANGLES = 0x01, 
    /**<
     * <b>Use with caution</b>. This specifies that the triangles of an 
     * \ref RpWorldSector or \ref RpGeometry should be sorted according to 
     * their location in the collision tree, thus eliminating the need
     * for a remapping array in the datastructure (see 
     * \ref RpCollTreeGetEntryMap). 
     *
     * If this option is used, it is the responsibility of the application to
     * update any additional data in the world sector or geometry that is 
     * dependent on the triangle order. 
     *
     * If the sort order is required by the application, an 
     * \ref RpCollisionBuildSortCallBack should be specified in the 
     * \ref RpCollisionBuildParam. 
     */

    rpCOLLISIONBUILDFLAGS_FORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpCollisionBuildFlags RpCollisionBuildFlags;


/**
 * \ingroup rpcollis
 * RpCollisionBuildSortCallBack represents a function that may be called 
 * during the generation of collision data for an \ref RpWorldSector or 
 * \ref RpGeometry when the \ref rpCOLLISIONBUILD_SORTTRIANGLES option is
 * used. This option means that the triangles of the world sector
 * or geometry are automatically sorted, and this call back is only
 * required in cases where additional triangle data must be processed
 * such as an \ref RpUserDataArray.
 *
 * The call back may be setup in the \ref RpCollisionBuildParam.
 *
 * \param  object  \ref RpWorldSector or \ref RpGeometry. Use 
 *                 \ref RwObjectGetType to distinguish between the two.
 * \param  tree    The \ref RpCollTree for the sector or geometry.
 * \param  table   An array holding the sort table. This represents the
 *                 new triangle array and each entry contains the 
 *                 original index of the triangle.
 * \param  data    Application specific data.
 *
 * \return Pointer to the world sector or geometry.
 *
 * \see RpCollisionGeometryBuildData
 * \see RpCollisionWorldSectorBuildData
 * \see RpCollisionWorldBuildData
 */
typedef void * (*RpCollisionBuildSortCallBack)(void        *object, 
                                               RpCollTree  *tree, 
                                               RwUInt16    *sortTable,
                                               void        *data);

/**
 * \ingroup rpcollis
 * \struct RpCollisionBuildParam
 * This parameter block may be passed to the functions 
 * \ref RpCollisionWorldSectorBuildData, \ref RpCollisionWorldBuildData 
 * and \ref RpCollisionGeometryBuildData to control the generation of
 * collision data (otherwise a NULL argument indicates that default
 * settings should be used). The function 
 * \ref RpCollisionBuildParamInitDefaults can be used to initialize the
 * parameters to their default settings.
 */
typedef struct RpCollisionBuildParam  RpCollisionBuildParam;
struct RpCollisionBuildParam
{
    RwUInt32    flags;              
        /**< \ref RpCollisionBuildFlags */

    RpCollisionBuildSortCallBack    sortCallBack;
        /**< Function to recieve information about sorting order when the
        \ref rpCOLLISIONBUILD_SORTTRIANGLES option is set. */

    void                           *sortCallBackData;
        /**< Data to be passed to the sort call back */
};


/******************************************************************************
 *  Functions
 */

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* General collision data */
extern RpCollisionBuildParam *
RpCollisionBuildParamInitDefaults(RpCollisionBuildParam *param);

/* WorldSector collision data */
extern RpWorldSector *
RpCollisionWorldSectorBuildData(
    RpWorldSector           *worldSector,
    RpCollisionBuildParam   *param);

/* World collision data (convenience functions which call WorldSector functions) */
extern RpWorld *
RpCollisionWorldBuildData(
    RpWorld                 *world,
    RpCollisionBuildParam   *param);

/* Geometry collision data */
extern RpGeometry *
RpCollisionGeometryBuildData(
    RpGeometry              *geometry,
    RpCollisionBuildParam   *param);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./collbuild.h----*/
/*---- start: ./collquery.h----*/

/******************************************************************************
 *  Types
 */

/**
 * \ingroup rpcollis
 * RpIntersectType, this type represents the different types of
 * primitives that can be used to intersect with an object (for example, see
 * \ref RpCollisionWorldForAllIntersections):
 */
enum RpIntersectType
{
    rpINTERSECTNONE = 0,
    rpINTERSECTLINE,   /**<Line Intersection */
    rpINTERSECTPOINT,  /**<Point Intersection */
    rpINTERSECTSPHERE, /**<Sphere Intersection */
    rpINTERSECTBOX,    /**<Box intersection */
    rpINTERSECTATOMIC, /**<Atomic Intersection based on bounding sphere */
    rpINTERSECTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpIntersectType RpIntersectType;

typedef union RpIntersectData RpIntersectData;
/**
 * \ingroup rpcollis
 * RpIntersectData, this union type is used to specify the parameters
 * for an intersection primitive of the desired type (\ref RpIntersectType)
 */
union RpIntersectData
{
    RwLine              line;       /**<For type rpINTERSECTLINE */
    RwV3d               point;      /**<For type rpINTERSECTPOINT */
    RwSphere            sphere;     /**<For type rpINTERSECTSPHERE */
    RwBBox              box;        /**<For type rpINTERSECTBOX */
    void               *object;     /**<For type rpINTERSECTATOMIC - this
                                     * should hold a pointer to the atomic */
};

/**
 * \ingroup rpcollis
 * \struct  RpIntersection
 * Intersection Object.  This type represents data for an
 * intersection primitive.  It specifies the intersection type
 * (line, sphere, etc.) (type) and parameters describing the given type (t):
 */
struct RpIntersection
{
    RpIntersectData     t; /**< Intersection data. Union type - one
                            * of line, point, sphere or RW object */
    RpIntersectType     type; /**< Intersection type - see
                               * \ref RpIntersectType */
};
typedef struct RpIntersection RpIntersection;

/**
 * \ingroup rpcollis
 * \struct RpCollisionTriangle
 * A structure representing a  collision between
 * an \ref RpIntersection primitive and a triangle.
 * The collision is specified by the triangle's plane normal
 * (normal), the first triangle vertex (point), an index to the triangle in
 * the object geometry's triangle list (index) and the positions of the
 * triangle's vertices (vertices). Note all vector components are in object
 * space.
 *
 * \see RpCollisionWorldForAllIntersections
 * \see RpCollisionGeometryForAllIntersections
 * \see RpAtomicForAllIntersections
 * \see RpIntersectionCallBackWorldTriangle
 * \see RpIntersectionCallBackGeometryTriangle
 */
struct RpCollisionTriangle
{
    RwV3d      normal; /**< Triangle normal */
    RwV3d      point;  /**< First triangle vertex */
    RwInt32    index;  /**< Index of triangle in object (if applicable) */
    RwV3d      *vertices[3]; /**< Pointers to three triangle vertices */
};
typedef struct RpCollisionTriangle RpCollisionTriangle;

/**
 * \ingroup rpcollis
 * \ref RpIntersectionCallBackWorldTriangle represents the function called
 * from \ref RpCollisionWorldForAllIntersections for all intersections between
 * the specified primitive and the static geometry in a given world. This
 * function should return a pointer to the current collision triangle to
 * indicate success. The callback may return NULL to terminate further
 * callbacks on the world.
 *
 * \note The memory pointed to by collTriangle is stored on the stack. 
 * This memory should be considered volatile. To use this data outside
 * of the iterator, copy the contents.
 *
 * \param  intersection   Pointer to the intersection primitive.
 * \param  sector   Pointer to the world sector containing the triangle.
 * \param  collTriangle   Pointer to the \ref RpCollisionTriangle representing
 *              the triangle in the world's static geometry that is intersected.
 * \param  distance   The distance to the intersection point(s).
 * Note that the distance returned depends on the intersection type and is
 * normalized for the given intersection primitive.
 *  \li rpINTERSECTLINE  Distance from start of line to collision
 *              triangle, normalized to length of line.
 *  \li rpINTERSECTSPHERE  Distance of sphere's center from the collision
 *              triangle along the direction of the normal, and normalized
 *              to the sphere's radius (may be negative if the sphere center
 *              is behind the triangle's plane with respect to the direction
 *              of the normal).
 *  \li rpINTERSECTBOX  Distance is undefined.
 *  \li rpINTERSECTATOMIC  Distance of atomic's bounding-sphere center
 *              from the collision triangle along the direction of the normal
 *              and normalized to sphere's radius.
 * \param  data   User defined data pointer
 *
 * \return Pointer to the current collision triangle.
 */
typedef RpCollisionTriangle * (*RpIntersectionCallBackWorldTriangle)
    (RpIntersection * intersection,
     RpWorldSector * sector,
     RpCollisionTriangle * collTriangle, RwReal distance, void *data);

/**
 * \ingroup rpcollis
 * \ref RpIntersectionCallBackAtomic represents the function called from
 * \ref RpWorldForAllAtomicIntersections for all intersections between the
 * specified primitive and collision atomics in a given world. This function
 * should return the current atomic to indicate success. The callback may
 * return NULL to terminate further callbacks on the world.
 *
 * \param  intersection   Pointer to the intersection primitive.
 * \param  sector   Pointer to the world sector containing
 *                   the intersected triangles.
 * \param  atomic   Pointer to the intersected atomic.
 * \param  distance   The collision distance. The distance returned
 * depends on the intersection type which is defined in \ref RpIntersectType.
 *  \li rpINTERSECTPOINT   Distance of point from atomic's bounding
 *      sphere center, normalized to sphere's radius.
 *  \li rpINTERSECTLINE   Distance of atomic's bounding-sphere center from
 *      start of line, projected onto the line, normalized to length of line.
 *      Note that by this definition, if the line starts or ends inside the
 *      sphere, this distance maybe negative or greater than one.
 *  \li rpINTERSECTSPHERE  Distance of atomic's bounding-sphere center
 *      from sphere's center, normalized to sum of spheres' radii.
 *  \li rpINTERSECTBOX  Distance undefined.
 *  \li rpINTERSECTATOMIC  Distance between atomics' bounding-sphere
 *      centers, normalized to sum of spheres' radii.
 * \param  data   User defined data pointer.
 *
 * \return Pointer to the current atomic.
 */
typedef RpAtomic   *(*RpIntersectionCallBackAtomic)
    (RpIntersection * intersection,
     RpWorldSector * sector, RpAtomic * atomic, RwReal distance, void *data);

/**
 * \ingroup rpcollis
 * \ref RpIntersectionCallBackWorldSector represents the function called from
 * \ref RpWorldForAllWorldSectorIntersections for all intersections between the
 * specified primitive and world sectors in a given world. This function should
 * return the current world sector to indicate success. The callback may return
 * NULL to terminate further callbacks on the world.
 *
 * \param  intersection   Pointer to the intersection primitive.
 * \param  sector   Pointer to the world sector containing the intersected
 *                  polygons.
 * \param  data   User defined data pointer
 *
 * \return Pointer to the current world sector.
 */
typedef RpWorldSector *(*RpIntersectionCallBackWorldSector)
    (RpIntersection * intersection, RpWorldSector * worldSector, void *data);

/**
 * \ingroup rpcollis
 * \ref RpIntersectionCallBackGeometryTriangle represents the function called
 * from \ref RpAtomicForAllIntersections and
 * \ref RpCollisionGeometryForAllIntersections
 * for all intersections between the specified primitive and a given atomic.
 * This function should return a pointer to the current collision triangle to
 * indicate success. The callback may return NULL to terminate further
 * callbacks on the atomic.
 *
 * Note that the vertices and normal of the collision triangle are given
 * in the coordinate space of the geometry. If they are required in world
 * coordinates, they must be transformed using \ref RwV3dTransformPoints and
 * \ref RwV3dTransformVectors with the LTM of the atomic's frame. This must
 * be passed via the user-defined data if required.
 *
 * \param  intersection   Pointer to the intersection primitive.
 * \param  collTri   Pointer to the \ref RpCollisionTriangle
 *              representing the triangle in the atomic that is intersected.
 * \param  distance   The distance to the intersection point(s).
 * Note that the distance returned depends on the intersection type and is
 * normalized for the given intersection primitive.
 *  \li rpINTERSECTLINE   Distance from start of line to collision
 *      triangle, normalized to length of line.
 *  \li rpINTERSECTSPHERE  Distance of sphere's center from the collision
 *              triangle along the direction of the normal, and normalized
 *              to the sphere's radius (may be negative if the sphere center
 *              is behind the triangle's plane with respect to the direction
 *              of the normal).
 *  \li rpINTERSECTATOMIC   Distance of atomic's bounding-sphere center
 *      from the collision triangle along the direction of the normal, and
 *      normalized to sphere's radius.
 * \param  data   User defined data pointer
 *
 * \return Pointer to the current collision triangle.
 */
typedef RpCollisionTriangle *(*RpIntersectionCallBackGeometryTriangle)
    (RpIntersection *intersection, RpCollisionTriangle *collTriangle,
    RwReal distance, void *data);


/******************************************************************************
 *  Functions
 */

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RpWorld *
RpWorldForAllWorldSectorIntersections(
    RpWorld                 *world,
    RpIntersection          *intersection,
    RpIntersectionCallBackWorldSector callBack,
    void                    *data);

extern RpWorld *
RpWorldForAllAtomicIntersections(
    RpWorld                 *world,
    RpIntersection          *intersection,
    RpIntersectionCallBackAtomic callBack,
    void                    *data);

extern RpWorld *
RpCollisionWorldForAllIntersections(
    RpWorld                 *world,
    RpIntersection          *intersection,
    RpIntersectionCallBackWorldTriangle callBack,
    void                    *data);

extern RpGeometry *
RpCollisionGeometryForAllIntersections(
    RpGeometry              *geometry,
    RpIntersection          *intersection,
    RpIntersectionCallBackGeometryTriangle callBack,
    void                    *data);

extern RpAtomic *
RpAtomicForAllIntersections(
    RpAtomic                *atomic,
    RpIntersection          *intersection,
    RpIntersectionCallBackGeometryTriangle callBack,
    void                    *data);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

/*---- end: ./collquery.h----*/

#endif /* RPCOLLIS_H */


