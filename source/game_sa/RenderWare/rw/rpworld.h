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
 * Filename: <C:/daily/rwsdk/include/d3d9/rpworld.h>
 * Automatically Generated on: Fri Oct 03 09:52:43 2003
 *
 ************************************************************************/

/*--- System Header Files ---*/
#include <stdarg.h>
#include <stdlib.h>

/*****************************************************************************
 Global Defines & Enums
 */

#define RWD3D9VSCONST_TRANSFORM_OFFSET  0
#define RWD3D9VSCONST_AMBIENT_OFFSET    4
#define RWD3D9VSCONST_LIGHTS_OFFSET     5

typedef enum rwD3D9VertexShaderEffectType
{
    rwD3D9VERTEXSHADEREFFECT_NONE = 0,
    rwD3D9VERTEXSHADEREFFECT_BUMPMAP = 1,
    rwD3D9VERTEXSHADEREFFECT_ENVMAP,
    rwD3D9VERTEXSHADEREFFECT_BUMPENVMAP,
    rwD3D9VERTEXSHADEREFFECT_DUAL,
    rwD3D9VERTEXSHADEREFFECT_DUALREPLICATE,
    rwD3D9VERTEXSHADEREFFECT_UVANIM,
    rwD3D9VERTEXSHADEREFFECT_DUALUVANIM,
    rwD3D9VERTEXSHADEREFFECT_NORMALMAP,
    rwD3D9VERTEXSHADEREFFECT_NORMALENVMAP,
    rwD3D9VERTEXSHADEREFFECT_CUBEMAP,
    rwD3D9VERTEXSHADEREFFECT_BUMPCUBEMAP,

    rwD3D9VERTEXSHADEREFFECT_MAX = (1 << 6),

} rwD3D9VertexShaderEffectType;


typedef enum rwD3D9VertexShaderFogMode
{
    rwD3D9VERTEXSHADERFOG_NONE = 0,
    rwD3D9VERTEXSHADERFOG_LINEAR = 1,
    rwD3D9VERTEXSHADERFOG_EXP,
    rwD3D9VERTEXSHADERFOG_EXP2,

    rwD3D9VERTEXSHADERFOG_MAX
} rwD3D9VertexShaderFogMode;

/******************************************************************************
 Global Types
 */

#if !defined(DOXYGEN)

/* This struct is used to specify the required functionality of the vertex shader */
typedef struct _rpD3D9VertexShaderDescriptor
{
    RwUInt8 numDirectionalLights : 4;
    RwUInt8 numPointLights : 4;
    RwUInt8 numSpotLights : 4;
    RwUInt8 numTexCoords : 4;

    RwUInt8 morphing : 1;
    RwUInt8 numWeights : 3;
    RwUInt8 prelit : 1;
    RwUInt8 normals : 1;
    RwUInt8 normalizeNormals : 1;
    RwUInt8 modulateMaterial : 1;
    RwUInt8 fogMode : 2;
    RwUInt8 effect : 6;

} _rpD3D9VertexShaderDescriptor;

/*
 * Offsets for the required vertex shader constants.
 * The combined transposed matrix world-view-projection is always at offset 0.
 * The ambient is always at offset 4.
 * The first light is always at offset 5.
 * 0xFF indicates that the specified constant is not used.
 */
typedef struct _rpD3D9VertexShaderDispatchDescriptor
{
    RwUInt8 offsetMaterialColor;
    RwUInt8 offsetFogRange;
    RwUInt8 offsetEffect;
    RwUInt8 offsetMorphingCoef;
    RwUInt8 offsetBoneMatrices;
    RwUInt8 pad[3];

} _rpD3D9VertexShaderDispatchDescriptor;

#endif /* !defined(DOXYGEN) */

/*
 * Vertex shader pipeline callbacks
 */

typedef struct RxD3D9ResEntryHeader RxD3D9ResEntryHeader;
typedef struct RxD3D9InstanceData RxD3D9InstanceData;
typedef struct RpMaterial RpMaterial;

/* Initialize shared components of "desc" and to prepare internal render data */

typedef RwBool (*_rxD3D9VertexShaderInstanceCallBack)(void *object,
                                                      RxD3D9ResEntryHeader *resEntryHeader,
                                                      RwBool reinstance);

typedef void (*_rxD3D9VertexShaderBeginCallBack)(void *object,
                                                 RwUInt32 type,
                                                 _rpD3D9VertexShaderDescriptor  *desc);

/* _rwD3D9VSSetActiveWorldMatrix MUST be called before using this two callbacks */

typedef RwV4d * (*_rxD3D9VertexShaderLightingCallBack)(void *object,
                                                       RwUInt32 type,
                                                       RwV4d    *shaderConstantPtr,
                                                       _rpD3D9VertexShaderDescriptor  *desc);

typedef void *(*_rxD3D9VertexShaderGetMaterialShaderCallBack)(const RpMaterial *material,
                                                              _rpD3D9VertexShaderDescriptor *desc,
                                                              _rpD3D9VertexShaderDispatchDescriptor *dispatch);


typedef void (*_rxD3D9VertexShaderMeshRenderCallBack)(RxD3D9ResEntryHeader *resEntryHeader,
                                                      RxD3D9InstanceData *instancedMesh,
                                                      const _rpD3D9VertexShaderDescriptor  *desc,
                                                      const _rpD3D9VertexShaderDispatchDescriptor *dispatch);

typedef void (*_rxD3D9VertexShaderEndCallBack)(void *object,
                                               RwUInt32 type,
                                               _rpD3D9VertexShaderDescriptor  *desc);

/****************************************************************************
 Global Types
 */


typedef struct RpMaterialChunkInfo RpMaterialChunkInfo;
typedef struct RpMaterialChunkInfo _rpMaterial;

#if (!defined(DOXYGEN))
struct RpMaterialChunkInfo
{
    RwInt32             flags;       /* Material flags - unused currently - 
                                      * for future expansion */
    RwRGBA              color;       /* Color of material. */
    RwInt32             unused;      /* Not used */
    RwBool              textured;    /* Are we textured? */
    RwSurfaceProperties surfaceProps;/* Surface properties */
};
#endif /* (!defined(DOXYGEN)) */

#if (!defined(RwMaterialAssign))
#define RwMaterialAssign(_target, _source)             \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwMaterialAssign)) */

/**
 * \ingroup rpmaterial
 * \struct RpMaterial 
 * Material object. This should be 
 * considered an opaque type. Use the RpMaterial API functions to access.
 */
typedef struct RpMaterial RpMaterial;

#if (!defined(DOXYGEN))
struct RpMaterial
{
        RwTexture           *texture; /**< texture */
        RwRGBA              color; /**< color */              
        RxPipeline          *pipeline; /**< pipeline */     
        RwSurfaceProperties surfaceProps; /**< surfaceProps */
        RwInt16             refCount;          /* C.f. rwsdk/world/bageomet.h:RpGeometry */
        RwInt16             pad;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpmaterial
 * \ref RpMaterialCallBack 
 * represents the function called from \ref RpGeometryForAllMaterials and 
 * \ref RpWorldForAllMaterials for all materials referenced by polygons in a 
 * given geometry. This function should return a pointer to the current 
 * material to indicate success. The callback may return NULL to terminate 
 * further callbacks on the materials.
 * 
 * \param  material   Pointer to the current material
 * \param  data       Pointer to developer-defined data structure.
 * 
 * \return Pointer to the current material.
 */
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

/****************************************************************************
 <macro/inline functionality
 */

#define RpMaterialAddRefMacro(_material)                            \
    (((_material)->refCount++), (_material))

#define RpMaterialAddRefVoidMacro(_material)                        \
MACRO_START                                                         \
{                                                                   \
    (_material)->refCount++;                                        \
}                                                                   \
MACRO_STOP


#define RpMaterialSetColorMacro(_material, _color)                  \
    (RwRGBAAssign(&((_material)->color), (_color)), (_material))

#define RpMaterialGetColorMacro(_material)                          \
    (&((_material)->color))

#define RpMaterialSetSurfacePropertiesMacro(_material, _surfProps)  \
    (RwSurfacePropertiesAssign(&((_material)->surfaceProps),        \
                               (_surfProps)), (_material))

#define RpMaterialGetSurfacePropertiesMacro(_material)              \
    (&((_material)->surfaceProps))

#define RpMaterialGetTextureMacro(_material)                        \
    ((_material)->texture)


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpMaterialAddRef(_material)                                 \
    RpMaterialAddRefMacro(_material)

#define RpMaterialSetColor(_material, _color)                       \
    RpMaterialSetColorMacro(_material, _color)

#define RpMaterialGetColor(_material)                               \
    RpMaterialGetColorMacro(_material)

#define RpMaterialSetSurfaceProperties(_material, _surfProps)       \
    RpMaterialSetSurfacePropertiesMacro(_material, _surfProps)

#define RpMaterialGetSurfaceProperties(_material)                   \
    RpMaterialGetSurfacePropertiesMacro(_material)

#define RpMaterialGetTexture(_material)                             \
    RpMaterialGetTextureMacro(_material)

#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/bamatlst.h ---*/
/****************************************************************************
 Global Types
 */

typedef struct RpMaterialList RpMaterialList;

#if (!defined(DOXYGEN))
struct RpMaterialList
{
    RpMaterial     **materials;
    RwInt32        numMaterials;
    RwInt32        space;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/bamesh.h ---*/

/*
 *
 * Purpose: Provide construction and enumeration facilities for meshes.
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 */

#define RPMESHGLOBAL(var)                                   \
    (RWPLUGINOFFSET(rpMeshGlobals,                          \
                    RwEngineInstance,                       \
                    meshModule.globalsOffset)->var)

#define rwPRIMTYPEOR                            \
    (rwPRIMTYPELINELIST |                       \
      rwPRIMTYPEPOLYLINE |                      \
      rwPRIMTYPETRILIST |                       \
      rwPRIMTYPETRISTRIP |                      \
      rwPRIMTYPETRIFAN   |                      \
      rwPRIMTYPEPOINTLIST)

#define rpMESHHEADERPRIMTYPEOR                  \
    (0 /* rpMESHHEADERTRILIST*/ |               \
      rpMESHHEADERTRISTRIP |                    \
      rpMESHHEADERTRIFAN  |                     \
      rpMESHHEADERLINELIST |                    \
      rpMESHHEADERPOLYLINE |                    \
      rpMESHHEADERPOINTLIST)

/****************************************************************************
 Global types
 */

/*
 * typedef for header structure listing all meshes
 * constituting a single RpGeometry or RpWorldSector
 */
typedef struct RpMeshHeader RpMeshHeader;

/**
 * \ingroup rpmesh
 * \ref RpMeshHeaderFlags
 * represents the different types of mesh.
 * \see RpMeshHeader
 */
enum RpMeshHeaderFlags
{
    /* NOTE: trilists are denoted by absence of any other
     *       primtype flags, so be careful that you test:
     *        (triListFlag == (flags&triListFlag))
     *       or:
     *        (0 == (flags&rpMESHHEADERPRIMMASK))
     *       and not:
     *        (flags&triListFlag)
     */
    rpMESHHEADERTRISTRIP  = 0x0001, /**< Render as tristrips */
    rpMESHHEADERTRIFAN    = 0x0002, /**< On PlayStation 2 these will be converted to trilists */
    rpMESHHEADERLINELIST  = 0x0004, /**< Render as linelists */
    rpMESHHEADERPOLYLINE  = 0x0008, /**< On PlayStation 2 these will be converted to linelists */
    rpMESHHEADERPOINTLIST = 0x0010, /**< Pointlists are supported only if rendered by
                                     *   custom pipelines; there is no default RenderWare
                                     *   way to render pointlists. */

    rpMESHHEADERPRIMMASK  = 0x00FF, /**< All bits reserved for specifying primitive type */
    rpMESHHEADERUNINDEXED = 0x0100, /**< Topology is defined implicitly by vertex
                                     *   order, ergo the mesh contains no indices */
    rpMESHHEADERFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/*
 * Typedef for RpMeshHeaderFlags enumeration
 * representing the different types of mesh
 */
typedef enum RpMeshHeaderFlags RpMeshHeaderFlags;

typedef struct rpMeshGlobals rpMeshGlobals;

#if (!defined(DOXYGEN))
struct rpMeshGlobals
{
    RwInt16             nextSerialNum;
    RwFreeList         *triStripListEntryFreeList;
    RwUInt8             meshFlagsToPrimType[rpMESHHEADERPRIMTYPEOR];
    RwUInt8             primTypeToMeshFlags[rwPRIMTYPEOR];
};
#endif /* (!defined(DOXYGEN)) */

typedef struct RpBuildMeshTriangle RpBuildMeshTriangle;

/**
 * \ingroup rpmesh
 * \struct RpBuildMeshTriangle
 * This type represents an array of indices into
 * the object vertex array. Used during the construction
 * of tristrips.
 *
 * See API functions
 * \see RpBuildMeshGeneratePreprocessTriStrip
 * \see RpBuildMeshGenerateExhaustiveTriStrip
 * \see RpBuildMeshGenerateTrivialTriStrip
 * \see RpBuildMeshGenerateDefaultTriStrip
 * and
 * \see RpMeshSetTriStripMethod
 * \see RpMeshGetTriStripMethod
 */
struct RpBuildMeshTriangle
{
    RwUInt16            vertIndex[3]; /**< indices into object vertex
                                       *   array. */
    RpMaterial         *material;     /**< pointer to material used to
                                       *   render the mesh. */
    RwUInt16            matIndex;     /**< unique index for material used */
    RwUInt16            textureIndex; /**< unique index for texture used */
    RwUInt16            rasterIndex;  /**< unique index for raster used */
    RwUInt16            pipelineIndex;/**< unique index for pipeline used */
};

typedef struct RpBuildMesh RpBuildMesh;

/**
 * \ingroup rpmesh
 * \struct RpBuildMesh
 * This type represents a mesh ready for tri stripping.
 *
 * See API functions
 * \see RpBuildMeshGeneratePreprocessTriStrip
 * \see RpBuildMeshGenerateExhaustiveTriStrip
 * \see RpBuildMeshGenerateTrivialTriStrip
 * \see RpBuildMeshGenerateDefaultTriStrip
 * and
 * \see RpMeshSetTriStripMethod
 * \see RpMeshGetTriStripMethod
 */
struct RpBuildMesh
{
    RwUInt32            triangleBufferSize; /**< number of triangles
                                             *   space has been allocated
                                             *   for. */
    RwUInt32            numTriangles;       /**< number of triangles to be
                                             *   tristripped. */
    RpBuildMeshTriangle *meshTriangles;     /**< pointer to build mesh
                                             *   triangles. */
};

typedef struct RpMesh RpMesh;

/**
 * \ingroup rpmesh
 * \struct RpMesh
 * This type represents a single polygon mesh.
 * A mesh is defined as a collection of triangles derived from an RpGeometry
 * or RpWorldSector which have a common material.
 *
 * See API functions \see RpGeometryForAllMeshes and
 * \see RpWorldSectorForAllMeshes and
 * the corresponding function callback types:
 */
struct RpMesh
{
    RxVertexIndex      *indices;    /**< vertex indices defining the mesh */
    RwUInt32            numIndices; /**< number of vertices in mesh */
    RpMaterial         *material;   /**< pointer to material used to
                                     *   render the mesh. */
};

/**
 * \ingroup rpmesh
 * \struct RpMeshHeader
 * Header for all meshes that constitute a single RpGeometry or RpWorldSector
 */
struct RpMeshHeader
{
    RwUInt32            flags;    /**< \see RpMeshHeaderFlags */
    RwUInt16            numMeshes; /**< Number of meshes in object */
    RwUInt16            serialNum; /**< Determine if mesh has changed
                                    * since last instance */
    RwUInt32            totalIndicesInMesh; /**< Total triangle index
                                             * count in all meshes
                                             */
    RwUInt32            firstMeshOffset; /**< Offset in bytes from end this
                                          * structure RpMeshHeader
                                          * to the first mesh
                                          */
};

/**
 * \ingroup rpmesh
 * \ref RpMeshCallBack is the callback
 * function supplied to \ref RpGeometryForAllMeshes and
 * \ref RpWorldSectorForAllMeshes for all meshes in a given geometry.
 *
 * This function should return a pointer to the current mesh to indicate
 * success. The callback may return NULL to terminate further callbacks
 * on the meshes.
 *
 * \param  mesh        Pointer to the current mesh, supplied by
 *                     iterator.
 * \param  meshHeader  Pointer to the meshes header
 * \param  data        Pointer to developer-defined data structure.
 *
 * \return Returns a pointer to the current mesh if successful or NULL 
 * if an error occurred.
 */
typedef RpMesh     *(*RpMeshCallBack) (RpMesh * mesh,
                                       RpMeshHeader * meshHeader,
                                       void *data);

/*--- Automatically derived from: C:/daily/rwsdk/world/bameshop.h ---*/

/****************************************************************************
 Defines
 */

#define RPTRISTRIPPOLYGONMAXEDGES 3

/**
 * \ingroup rpmesh
 * \ref RpTriStripMeshCallBack is the callback to generate triangle strips
 * when the triangle stripped geometries or world sectors are unlocked.
 *
 * \param buildMesh pointer to the mesh which the triangle strip will be
 *                  generated from.
 * \param data      pointer to user-supplied data to pass to the callback
 * function.
 *
 * \return a pointer to the constructed mesh header.
 *
 */
typedef RpMeshHeader *
(*RpTriStripMeshCallBack) (RpBuildMesh *buildMesh, void *data);



/****************************************************************************
 Global types
 */

typedef struct RpTriStripEdge RpTriStripEdge;

typedef struct RpTriStripPolygon RpTriStripPolygon;
/**
 * \ingroup rpmesh
 * \struct RpTriStripPolygon
 * This type represents a polygon.
 *
 * \see RpTriStripPolygonFollowStrip
 * \see RpBuildMeshGenerateTriStrip
 * \see RpTriStripMeshTunnel
 */
struct RpTriStripPolygon
{
    RwLLLink inEndLink;     /**< link for list of polygons joined by fewer than
                                 2 strip edges */
    RwLLLink inUsedLink;    /**< link for list of used polygons */
    RwLLLink inFreeLink;    /**< link for list of free polygons */
    RwUInt32 numEdges;      /**< number of edges */
    RpTriStripEdge *edges[RPTRISTRIPPOLYGONMAXEDGES];   /**< polygon edges */
    RxVertexIndex vertIndex[RPTRISTRIPPOLYGONMAXEDGES]; /**< polygon vertex 
                                                             indices */
    RwUInt32 testFrame;     /**< used to prevent a tri-strip being counted by
                                 the cost function more than once */
};

/**
 * \ingroup rpmesh
 * \struct RpTriStripEdge
 * This type represents a polygon edge which may be part of a tri-strip.
 *
 * \see RpTriStripPolygonFollowStrip
 * \see RpBuildMeshGenerateTriStrip
 * \see RpTriStripMeshTunnel
 */
struct RpTriStripEdge
{
    RwLLLink inUsedLink;        /**< link for list of used edges */      
    RwLLLink inFreeLink;        /**< link for list of free edges */
    RpTriStripPolygon *poly1;   /**< first polygon including this edge */
    RpTriStripPolygon *poly2;   /**< second polygon including this edge or NULL
                                     if this edge is only included in one */
    RxVertexIndex vert1;        /**< index of first vertex of edge */
    RxVertexIndex vert2;        /**< index of second vertex of edge */
    RwBool strip;               /**< TRUE if the edge is part of a tri-strip */
};

typedef struct RpTriStripMesh RpTriStripMesh;
/**
 * \ingroup rpmesh
 * \struct RpTriStripMesh
 * This type represents a mesh and the tri-strips it contains.
 *
 * \see RpBuildMeshGenerateTriStrip
 * \see RpTriStripMeshTunnel
 */
struct RpTriStripMesh
{
    RwUInt32 numPolygons;       /**< the number of polygons in the mesh */
    RwLinkList polygonEndList;  /**< linked list of polygons joined by fewer 
                                     than 2 strip edges. */
    RwLinkList polygonUsedList; /**< linked list of used polygons */
    RwLinkList polygonFreeList; /**< linked list of free polygons */
    RwLinkList edgeUsedList;    /**< linked list of used edges */
    RwLinkList edgeFreeList;    /**< linked list of free edges */
};

/**
 * \ingroup rpmesh
 * \ref RpTriStripMeshStripCallBack represents the callback function to
 * tri-strip a mesh.
 * 
 * \param  mesh Pointer to the mesh to tri-strip.
 * \param  data Pointer to user-supplied data.
 *
 * \return Returns a pointer to the current mesh if successful or NULL if an
 *         error occurred. 
 */
typedef RpTriStripMesh *
(*RpTriStripMeshStripCallBack) (RpTriStripMesh *mesh, void *data);

typedef struct RpTriStripData RpTriStripData;
/**
 * \ingroup rpmesh
 * \struct RpTriStripData
 * This type represents the data required by the 
 * \ref RpBuildMeshGenerateTriStrip function to tri-strip a mesh. This data must
 * be the user-supplied data passed into RpMeshSetTriStripMethod when using
 * \ref RpBuildMeshGenerateTriStrip.
 *
 * \see RpMeshGetTriStripMethod
 * \see RpMeshSetTriStripMethod
 * \see RpBuildMeshGenerateTriStrip
 * \see RpTriStripMeshTunnel
 */
struct RpTriStripData
{
    RwBool ignoreWinding;                /**< TRUE to ignore winding order */
    RpTriStripMeshStripCallBack stripCB; /**< callback to tri-strip mesh */
    void *data;                          /**< data to supply to callback */
};

/**
 * \ingroup rpmesh
 * \ref RpTriStripCostCallBack represents the callback function used by the
 * tunnelling tri-stripper to determine the cost of a tri-strip.
 * 
 * \param  startPolygon Pointer to polygon at the start of the tri-strip.
 * \param  testFrame    Value that must be written to the testFrame of all 
 *                      polygons in the tri-strip
 * \param  data         Pointer to user-supplied data.
 *
 * \return Returns the cost of the tri-strip.
 */
typedef RwUInt32
(*RpTriStripCostCallBack) (RpTriStripPolygon *startPolygon, RwUInt32 testFrame, void *data);

typedef struct RpTriStripTunnelData RpTriStripTunnelData;
/**
 * \ingroup rpmesh
 * \struct RpTriStripTunnelData
 * This type represents the data required by the 
 * \ref RpTriStripMeshTunnel function to tri-strip a mesh. This data must
 * be the user-supplied data in the RpTriStripData structure passed into
 * \ref RpMeshSetTriStripMethod as the user-supplied data when using
 * \ref RpTriStripMeshTunnel.
 *
 * \see RpMeshGetTriStripMethod
 * \see RpMeshSetTriStripMethod
 * \see RpBuildMeshGenerateTriStrip
 * \see RpTriStripMeshTunnel
 * \see RpTriStripDefaultCost
 */
struct RpTriStripTunnelData
{
    RwReal quality; /**< Values between 0.1 and 0.5 tend to be fairly quick but
        don't always give the best results. Values greater than 0.5 tend to 
        give good results useful for final artwork but can take much longer.
        Large values e.g. 5 allow the length limit to be the limiting factor
        instead. */
    RwUInt32 lengthLimit; /**< The maximum tunnel length to use. Values between
        10 and 50 tend to be fairly quick but don't always give the best
        results. Values greater than 50 tend to give good results for final
        artwork but can take much longer. Large values e.g. 10000 allow the
        quality to be the limiting factor. */
    RpTriStripCostCallBack costCB; /**< callback to determine tri-strip cost */
    void *data;                    /**< data to supply to callbacks */
};



/****************************************************************************
 Global Variables 
 */


/*--- Automatically derived from: C:/daily/rwsdk/world/bageomet.h ---*/

/*
 * Handling atomic's geometry
 * Geometry describe objects, and are the building blocks for atomics
 *
 * Copyright (c) 1998 Criterion Software Ltd.
*/


/****************************************************************************
 Defines
 */

/* Type ID */
#define rpGEOMETRY 8

/**
 * \ingroup rpgeometry
 * RpGeometryFlag
 * Geometry type flags 
 *
 * When creating a geometry, these flags can be OR'ed together to
 * specify the format along with the rpGEOMETRYTEXCOORDSETS(n) macro if more
 * than two sets of texture coordinates are required. See \ref RpGeometryCreate
 * for more details.
 *
 * \see RpGeometryCreate().
 */
enum RpGeometryFlag
{
    rpGEOMETRYTRISTRIP  = 0x00000001, /**<This geometry's meshes can be
                                          rendered as strips.
                                          \ref RpMeshSetTriStripMethod is
                                          used to change this method.*/
    rpGEOMETRYPOSITIONS = 0x00000002, /**<This geometry has positions */  
    rpGEOMETRYTEXTURED  = 0x00000004, /**<This geometry has only one set of
                                          texture coordinates. Texture
                                          coordinates are specified on a per
                                          vertex basis */
    rpGEOMETRYPRELIT    = 0x00000008, /**<This geometry has pre-light colors */
    rpGEOMETRYNORMALS   = 0x00000010, /**<This geometry has vertex normals */
    rpGEOMETRYLIGHT     = 0x00000020, /**<This geometry will be lit */
    rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040, /**<Modulate material color 
                                                      with vertex colors
                                                      (pre-lit + lit) */

    rpGEOMETRYTEXTURED2 = 0x00000080, /**<This geometry has at least 2 sets of
                                          texture coordinates. */

    /*
     * These above flags were stored in the flags field in an RwObject, they
     * are now stored in the flags file of the RpGeometry.
     */

    rpGEOMETRYNATIVE            = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE    = 0x02000000,

    rpGEOMETRYFLAGSMASK         = 0x000000FF,
    rpGEOMETRYNATIVEFLAGSMASK   = 0x0F000000,

    rpGEOMETRYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryFlag RpGeometryFlag;

/* Multi texture coordinate format specifier for RpGeometryCreate() */
#define rpGEOMETRYTEXCOORDSETS(_num) ((_num & 0xff) << 16)

/**
 * \ingroup rpgeometry
 * RpGeometryLockMode
 * Geometry lock flags 
 */
enum RpGeometryLockMode
{
    rpGEOMETRYLOCKPOLYGONS   =   0x01, /**<Lock the polygons (triangle list) */
    rpGEOMETRYLOCKVERTICES   =   0x02, /**<Lock the vertex positional data */
    rpGEOMETRYLOCKNORMALS    =   0x04, /**<Lock the vertex normal data */
    rpGEOMETRYLOCKPRELIGHT   =   0x08, /**<Lock the pre-light values */
    rpGEOMETRYLOCKTEXCOORDS  =   0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS1 =   0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS2 =   0x20, /**<Lock the texture coordinates set 2*/
    rpGEOMETRYLOCKTEXCOORDS3 =   0x40, /**<Lock the texture coordinates set 3*/
    rpGEOMETRYLOCKTEXCOORDS4 =   0x80, /**<Lock the texture coordinates set 4*/
    rpGEOMETRYLOCKTEXCOORDS5 = 0x0100, /**<Lock the texture coordinates set 5*/
    rpGEOMETRYLOCKTEXCOORDS6 = 0x0200, /**<Lock the texture coordinates set 6*/
    rpGEOMETRYLOCKTEXCOORDS7 = 0x0400, /**<Lock the texture coordinates set 7*/
    rpGEOMETRYLOCKTEXCOORDS8 = 0x0800, /**<Lock the texture coordinates set 8*/
    rpGEOMETRYLOCKTEXCOORDSALL = 0x0ff0, /**<Lock all texture coordinate sets*/
    rpGEOMETRYLOCKALL        = 0x0fff, /**<Combination of all the above */

    rpGEOMETRYLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryLockMode RpGeometryLockMode;

/* Convenience macro for generating a tex coord lock flag */
#define rpGEOMETRYLOCKTEXCOORDSIDX(_idx) (rpGEOMETRYLOCKTEXCOORDS1 << (_idx))


/****************************************************************************
 Global Types
 */

typedef struct rpGeometryGlobals rpGeometryGlobals;

#if (!defined(DOXYGEN))
struct rpGeometryGlobals
{
        RwFreeList         *geomFreeList;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpgeometry
 * \struct RpGeometry 
 * Geometry object. This should be considered an opaque type. 
 * Use the RpGeometry API functions to access.
 */
typedef struct RpGeometry RpGeometry;

/**
 * \ingroup rpmorphtarget
 * \struct RpMorphTarget 
 * Morph target -- vertex positions and normals. 
 * This should be considered an opaque type. 
 * Use RpMorphTarget API functions to access.
 */
typedef struct RpMorphTarget RpMorphTarget;

#if (!defined(DOXYGEN))
struct RpMorphTarget
{
        RpGeometry *parentGeom; 
        RwSphere   boundingSphere;
        RwV3d      *verts;
        RwV3d      *normals;
};
#endif /* (!defined(DOXYGEN)) */

typedef struct RpTriangle RpTriangle;

/**
 * \ingroup rpgeomtry
 * \struct RpTriangle 
 * This type represents a triangle in a geometry specified 
 * by three indices into the geometry's vertex list (vertIndex)
 * together with an  index in to the geometry's 
 * material list (matIndex) (see API function \ref RpGeometryGetTriangles)
 */
struct RpTriangle
{
    RwUInt16            vertIndex[3];   /**< vertex indices */
    RwUInt16            matIndex;       /**< Index into material list */
};

#if (!defined(DOXYGEN))
struct RpGeometry
{
    RwObject            object;     /* Generic type */

    RwUInt32            flags;      /* Geometry flags */

    RwUInt16            lockedSinceLastInst; /* What has been locked since we last instanced - for re-instancing */
    RwInt16             refCount;   /* Reference count (for keeping track of atomics referencing geometry) */

    RwInt32             numTriangles; /* Quantity of various things (polys, verts and morph targets) */
    RwInt32             numVertices;
    RwInt32             numMorphTargets;
    RwInt32             numTexCoordSets;

    RpMaterialList      matList;

    RpTriangle         *triangles;  /* The triangles */

    RwRGBA             *preLitLum;  /* The pre-lighting values */

    RwTexCoords        *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RpMeshHeader       *mesh;   /* The mesh - groups polys of the same material */

    RwResEntry         *repEntry;       /* Information for an instance */

    RpMorphTarget      *morphTarget;    /* The Morph Target */
};

typedef struct RpGeometryChunkInfo RpGeometryChunkInfo;
typedef struct RpGeometryChunkInfo _rpGeometry;

struct RpGeometryChunkInfo
{
    RwInt32             format;  /* Compression flags and number of texture coord sets */

    RwInt32             numTriangles;
    RwInt32             numVertices;

    RwInt32             numMorphTargets;
};
#endif /* (!defined(DOXYGEN)) */

/* Callbacks */

/**
 * \ingroup rpgeometry
 * \ref RpGeometryCallBack represents the simple callback function for the \ref RpGeometry object.
 * The callback may return NULL to terminate further callbacks on 
 * the geometry.
 * 
 * \param  geometry   Pointer to the current geometry, supplied by iterator.
 * \param  data       Pointer to developer-defined data structure.
 *
 * \return Pointer to the current geometry, or NULL If not found.
 */
typedef RpGeometry *(*RpGeometryCallBack)(RpGeometry *geometry, void *data);

/**
 * \ingroup rpgeometry
 * \ref RpGeometrySortByMaterialCallBack 
 * \ref RpGeometrySortByMaterialCallBack is used by
 * \ref RpGeometrySortByMaterial in order to preserve the validity of plugin
 * data when the vertices in an \ref RpGeometry are sorted (and some possibly
 * duplicated).
 *
 * A vertex map table is provided to the callback - this contains, for each
 * vertex in the new geometry, the index of the corresponding vertex in the
 * original geometry.
 *
 * \param  oldGeom           Pointer to the source geometry.
 * \param  newGeom           Pointer to the new, sorted geometry.
 * \param  remapTable        Pointer to the vertex map table
 * \param  numberOfEntries   Number of vertices in the new 
 *                           geometry (size of the map table)
 */
typedef void (*RpGeometrySortByMaterialCallBack)(const RpGeometry *oldGeom,
                                                 RpGeometry *newGeom,
                                                 RwUInt16 *remapTable,
                                                 RwUInt16 numberOfEntries);

/****************************************************************************
 <macro/inline functionality
 */

#define RpMorphTargetSetBoundingSphereMacro(_mt, _sphere)       \
    (RwSphereAssign(&((_mt)->boundingSphere), (_sphere)), (_mt))

#define RpMorphTargetGetBoundingSphereMacro(_mt)                \
    (&((_mt)->boundingSphere))

#define RpGeometryGetNumMorphTargetsMacro(_geometry)            \
    ((_geometry)->numMorphTargets)

#define RpGeometryGetMorphTargetMacro(_geometry, _index)        \
    (&((_geometry)->morphTarget[(_index)]))

#define RpGeometryGetPreLightColorsMacro(_geometry)             \
    ((_geometry)->preLitLum)

#define RpGeometryGetVertexTexCoordsMacro(_geometry, _uvIndex)    \
    ((_geometry)->texCoords[(_uvIndex) - 1])

#define RpGeometryGetNumTexCoordSetsMacro(_geometry)            \
    ((_geometry)->numTexCoordSets)

#define RpGeometryGetNumVerticesMacro(_geometry)                \
    ((_geometry)->numVertices)

#define RpMorphTargetGetVerticesMacro(_mt)                      \
    ((_mt)->verts)

#define RpMorphTargetGetVertexNormalsMacro(_mt)                 \
    ((_mt)->normals)

#define RpGeometryGetTrianglesMacro(_geometry)                  \
    ((_geometry)->triangles)

#define RpGeometryGetNumTrianglesMacro(_geometry)               \
    ((_geometry)->numTriangles)

#define RpGeometryGetMaterialMacro(_geometry, _num)             \
    (((_geometry)->matList.materials)[(_num)])

#define RpGeometryGetNumMaterialsMacro(_geometry)               \
    ((_geometry)->matList.numMaterials)

#define RpGeometryGetFlagsMacro(_geometry)                      \
    ((_geometry)->flags)

#define RpGeometrySetFlagsMacro(_geometry, _flags)              \
    (((_geometry)->flags = (_flags)), (_geometry))


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpMorphTargetSetBoundingSphere(_geometry, _sphere)      \
    RpMorphTargetSetBoundingSphereMacro(_geometry, _sphere)

#define RpMorphTargetGetBoundingSphere(_geometry)               \
    RpMorphTargetGetBoundingSphereMacro(_geometry)

#define RpGeometryGetNumMorphTargets(_geometry)                 \
    RpGeometryGetNumMorphTargetsMacro(_geometry)

#define RpGeometryGetMorphTarget(_geometry, _index)             \
    RpGeometryGetMorphTargetMacro(_geometry, _index)

#define RpGeometryGetPreLightColors(_geometry)                  \
    RpGeometryGetPreLightColorsMacro(_geometry)

#define RpGeometryGetVertexTexCoords(_geometry, _uvIndex)       \
    RpGeometryGetVertexTexCoordsMacro(_geometry, _uvIndex)

#define RpGeometryGetNumTexCoordSets(_geometry)                 \
    RpGeometryGetNumTexCoordSetsMacro(_geometry)

#define RpGeometryGetNumVertices(_geometry)                     \
    RpGeometryGetNumVerticesMacro(_geometry)

#define RpMorphTargetGetVertices(_mt)                           \
    RpMorphTargetGetVerticesMacro(_mt)

#define RpMorphTargetGetVertexNormals(_mt)                      \
    RpMorphTargetGetVertexNormalsMacro(_mt)

#define RpGeometryGetTriangles(_geometry)                       \
    RpGeometryGetTrianglesMacro(_geometry)

#define RpGeometryGetNumTriangles(_geometry)                    \
    RpGeometryGetNumTrianglesMacro(_geometry)

#define RpGeometryGetMaterial(_geometry, _num)                  \
    RpGeometryGetMaterialMacro(_geometry, _num)

#define RpGeometryGetNumMaterials(_geometry)                    \
    RpGeometryGetNumMaterialsMacro(_geometry)

#define RpGeometryGetFlags(_geometry)                           \
    RpGeometryGetFlagsMacro(_geometry)

#define RpGeometrySetFlags(_geometry, _flags)                   \
    RpGeometrySetFlagsMacro(_geometry, _flags)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/****************************************************************************
 Defines
 */

/* Type ID */
#define rpWorldSector 0xff     /* Not a true 'type'! */

#define rpMINDISTANCEBETWEENVERTICES (RwReal)(0.0001)

#define RPV3DFROMVERTEXNORMAL(v, n) \
    (v).x = (((RwReal)((n).x)) * ( (RwReal)(1.0/128))); \
    (v).y = (((RwReal)((n).y)) * ( (RwReal)(1.0/128))); \
    (v).z = (((RwReal)((n).z)) * ( (RwReal)(1.0/128)))

/****************************************************************************
 Global types
 */

typedef struct RpVertexNormal RpVertexNormal;

#if (!defined(DOXYGEN))
struct RpVertexNormal
{
    RwInt8          x;
    RwInt8          y;
    RwInt8          z;
    RwUInt8         pad; /* pad character to force alignment */
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpworldsector
 * \struct RpWorldSector
 * World Sector object. This should be
 * considered an opaque type. Use the RpWorldSector API functions to access.
 */
typedef struct RpWorldSector RpWorldSector;

#if (!defined(DOXYGEN))
struct RpWorldSector
{
    RwInt32                 type;

    RpTriangle              *triangles;             /* Triangles */

    RwV3d                   *vertices;              /* Vertex positions */
    RpVertexNormal          *normals;               /* Vertex normals */

    RwTexCoords             *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RwRGBA                  *preLitLum;             /* Pre lit luminances */

    /* Pointer to memory allocated for vertex and poly pointers */
    RwResEntry              *repEntry;

    /* Atomics in this sectors */
    /* The pointers are frigged so they look like they are pointing to
       Atomics when they are pointing to here */
    RwLinkList              collAtomicsInWorldSector;       /* Coll priority */

    /* Lights in an atomic sector */
    RwLinkList              lightsInWorldSector;

    /* Outer bounding box of sector based on BSP planes */
    RwBBox                  boundingBox;

    /* Bounding box tightly enclosing geometry */
    RwBBox                  tightBoundingBox;

    /* The mesh which groups same material triangles together */
    RpMeshHeader            *mesh;

    /* The WorldSector object pipeline for this WorldSector */
    RxPipeline    *pipeline;

    /* Material list window base
     * (triangles in a given sector can "see"
     * the 256 materials from
     * MatList[matListWindowBase] -> MatList[matListWindowBase + 255])
     */
    RwUInt16                matListWindowBase;

    RwUInt16                numVertices;            /* Num vertices */
    RwUInt16                numTriangles;           /* Num triangles */
    RwUInt16                pad;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpworldsector
 * \ref RpWorldSectorCallBack
 * represents the function called from \ref RpWorldForAllWorldSectors,
 * \ref RpAtomicForAllWorldSectors and \ref RpLightForAllWorldSectors for all
 * world sectors in a given world or world sectors a given atomic or light lies
 * in. This function should return a pointer to the current world sector to
 * indicate success. The callback may return NULL to terminate further
 * callbacks on the atomic or light.
 *
 * \param  sector   Pointer to the current world sector
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Pointer to the current world sector.
 *
 */
typedef RpWorldSector *(*RpWorldSectorCallBack)(RpWorldSector *worldSector, void *data);

typedef struct RpSector RpSector;

#if (!defined(DOXYGEN))
struct RpSector
{
    RwInt32                 type;
};

/* NOTE: The left and right pointers can point to an RpPlaneSector or
 *       an RpWorldSector
 * This is determined what the type is via the type value
 */

typedef struct RpPlaneSector RpPlaneSector;

struct RpPlaneSector
{
    RwInt32                 type;

    RwReal                  value;
    RpSector                *leftSubTree;   /* Sector 'left' (less) of the plane */
    RpSector                *rightSubTree;  /* Sector 'right' (more) of the plane */
    RwReal                  leftValue;
    RwReal                  rightValue;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/balight.h ---*/

/*
 * Lighting 3D objects.
 * Lights are used to illuminate atomics and worlds
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 */


/****************************************************************************
 Defines
 */

/* Binary Light */
typedef struct RpLightChunkInfo RpLightChunkInfo;
typedef struct RpLightChunkInfo _rpLight;

#if (!defined(DOXYGEN))
struct RpLightChunkInfo
{
        RwReal              radius; /**< radius */
        RwReal              red; /**< red */
        RwReal              green; /**< green */
        RwReal              blue; /**< blue */
        RwReal              minusCosAngle; /**< minusCosAngle */
        RwUInt32            typeAndFlags; /**< typeAndFlags */
};
#endif /* (!defined(DOXYGEN)) */

/* Type ID */
#define rpLIGHT 3

/* Beyond this the lights must be positioned */
#define rpLIGHTPOSITIONINGSTART 0x80

/**
 * \ingroup rplight
 * \ref RpLightType are
 * light sub types.  This type represents the different 
 * types of light source that can be created using the API function \ref RpLightCreate.  
 * Note that lights of types rpLIGHTPOINT, rpLIGHTSPOT and rpLIGHTSPOTSOFT have linear 
 * intensity fall-off with distance from the source, reducing to zero at the light's radius:*/
enum RpLightType
{
    rpNALIGHTTYPE = 0,

    /* These don't take part in the tie mechanism (no position) */
    rpLIGHTDIRECTIONAL, /**<Directional Light */
    rpLIGHTAMBIENT, /**<Ambient Light */

    /* These do take part in the tie mechanism (do have position) */
    rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART, /**<Point Light */
    rpLIGHTSPOT, /**<Spot Light */
    rpLIGHTSPOTSOFT, /**<Soft Spot Light */
    rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightType RpLightType;

#define rpLIGHTMINCONEANGLE ((RwReal)0.0f)

#if (defined(XBOX_DRVMODEL_H))
#define rpLIGHTMAXCONEANGLE (rwPIOVER2)
#elif (defined(OPENGL_DRVMODEL_H))
#define rpLIGHTMAXCONEANGLE (rwPIOVER2)
#elif (defined(GCN_DRVMODEL_H))
#define rpLIGHTMAXCONEANGLE (rwPIOVER2)
#elif (defined(D3D8_DRVMODEL_H))
#define rpLIGHTMAXCONEANGLE (rwPIOVER2)
#elif (defined(D3D9_DRVMODEL_H))
#define rpLIGHTMAXCONEANGLE (rwPIOVER2)
#else
#define rpLIGHTMAXCONEANGLE (rwPI)
#endif

/*************/

/*** FLAGS ***/

/*************/

/**
 * \ingroup rplight
 * \ref RpLightFlag defines what geometry is influenced by the light. 
 * The bit-field RpLightFlag specifies the options available for controlling the scope 
 * of a light source (see API function \ref RpLightSetFlags):*/
enum RpLightFlag
{
    rpLIGHTLIGHTATOMICS = 0x01, /**<The light source illuminates all atomics in a world */
    rpLIGHTLIGHTWORLD = 0x02, /**<The light source illuminates all static geometry in a world */
    rpLIGHTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightFlag RpLightFlag;

/* rpLIGHTPRIVATENOCHROMA - turns on optimisations to do with
 *                          lights which are a shade of grey
 */
enum rpLightPrivateFlag
{
    rpLIGHTPRIVATENOCHROMA = 0x01,
    rpLIGHTPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rpLightPrivateFlag rpLightPrivateFlag;

/*********************/

/**
 * \ingroup rplight
 * \struct RpLight 
 * Light object. This should be 
 * considered an opaque type. Use the RpLight API functions to access.
 */
typedef struct RpLight RpLight;

#if (!defined(DOXYGEN))
struct RpLight
{
        RwObjectHasFrame    object; /**< object */
        RwReal              radius; /**< radius */
        RwRGBAReal          color; /**< color */  /* Light color */
        RwReal              minusCosAngle; /**< minusCosAngle */  
        RwLinkList          WorldSectorsInLight; /**< WorldSectorsInLight */
        RwLLLink            inWorld; /**< inWorld */
        RwUInt16            lightFrame; /**< lightFrame */
        RwUInt16            pad;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rplight
 * \ref RpLightCallBack 
 * represents the function called from \ref RpWorldForAllLights and 
 * \ref RpWorld SectorForAllLights for all lights in a given world or world 
 * sector. This function should return a pointer to the current light to 
 * indicate success. The callback may return NULL to terminate further 
 * callbacks on the world sector.
 *
 * \param  light  Pointer to the current light in the world 
 *                sector.
 * \param  data   Pointer to developer-defined data structure.
 *
 * \return Pointer to the current light.
 */
typedef RpLight    *(*RpLightCallBack) (RpLight * light, void *data);

typedef struct RpLightTie RpLightTie;

/**
 * \ingroup rplight
 * \struct RpLightTie
 *
 * RpLightTie is a linked list of lights inside world sectors. These are
 * created when frame hierarchies are updated as part of an \ref RwCameraBeginUpdate.
 * This is used for determining which lights influence a world sector and the atomics inside it.
 * Creation and destruction of RpLightTies is internal to the world plugin.
 */
struct RpLightTie
{
    /* Information for an atomic sector */
    RwLLLink            lightInWorldSector; /**< Lights IN this ATOMIC SECTOR */
    RpLight            *light;              /**< A pointer to a light */

    /* Information for a atomic */
    RwLLLink            WorldSectorInLight; /**< Atomic sectors HOLDING this Light */
    RpWorldSector      *sect;               /**< A pointer to a world sector */
};

/****************************************************************************
 <macro/inline functionality
 */

#define RpLightGetRadiusMacro(_light)                       \
    ((_light)->radius)

#define RpLightGetColorMacro(_light)                        \
    (&((_light)->color))

#define RpLightSetFrameMacro(_light, _frame)                \
    (rwObjectHasFrameSetFrame((_light), (_frame)), (_light))

#define RpLightGetFrameMacro(_light)                        \
    ((RwFrame *)rwObjectGetParent((_light)))

#define RpLightGetTypeMacro(_light)                         \
    ((RpLightType)rwObjectGetSubType((_light)))

#define RpLightSetFlagsMacro(_light, _flags)                \
    ((rwObjectSetFlags((_light), (_flags))), (_light))

#define RpLightGetFlagsMacro(_light)                        \
    (rwObjectGetFlags((_light)))


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpLightGetRadius(_light)                            \
    RpLightGetRadiusMacro(_light)

#define RpLightGetColor(_light)                             \
    RpLightGetColorMacro(_light)

#define RpLightSetFrame(_light, _frame)                     \
    RpLightSetFrameMacro(_light, _frame)

#define RpLightGetFrame(_light)                             \
    RpLightGetFrameMacro(_light)

#define RpLightGetType(_light)                              \
    RpLightGetTypeMacro(_light)

#define RpLightSetFlags(_light, _flags)                     \
    RpLightSetFlagsMacro(_light, _flags)

#define RpLightGetFlags(_light)                             \
    RpLightGetFlagsMacro(_light)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/pipe/p2/d3d9/D3D9lights.h ---*/
/*
 * typedef for struct RpD3D9AttenuationParams
 */
typedef struct RpD3D9AttenuationParams RpD3D9AttenuationParams;
/**
 * \ingroup rplightd3d9
 * \struct RpD3D9AttenuationParams
 *  This type represents the attenuation model of a spot or point light.
 */
struct RpD3D9AttenuationParams
{
    RwReal constant;    /**< Constant attenuation coefficient */
    RwReal linear;      /**< Linear attenuation coefficient */
    RwReal quadratic;   /**< Quadratic attenuation coefficient */
};

/*--- Automatically derived from: C:/daily/rwsdk/world/baclump.h ---*/

/*
 * Clump and atomic handling.
 * Clumps and atomics are the movable rendered objects in the world
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 */


/****************************************************************************
 Defines
 */

/****************************** Object type ID ******************************/

/* Type IDs */

#define rpATOMIC 1
#define rpCLUMP 2

/* Interpolator flags */
enum RpInterpolatorFlag
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
    rpINTERPOLATORFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpInterpolatorFlag rpInterpolatorFlag;

/**
 * \ingroup rpatomic
 * The bit-field type RpAtomicFlag specifies the options available for
 * controlling the behavior of atomics. See API function \ref RpAtomicSetFlags.
 *
 * \see RpAtomicSetFlags
 * \see RpAtomicGetFlags
 * \see RpWorldSectorForAllCollisionAtomics
 */

enum RpAtomicFlag
{
    rpATOMICCOLLISIONTEST = 0x01, /**<A generic collision flag to indicate
                                   * that the atomic should be considered
                                   * in collision tests.
                                   */
    rpATOMICRENDER = 0x04,      /**<The atomic is rendered if it is
                                 * in the view frustum.
                                 */

    rpATOMICFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicFlag RpAtomicFlag;

/**
 * \ingroup rpatomic
 * \ref RpAtomicSetGeomFlag defines how an atomic references a new geometry
 *
 * \see RpAtomicSetGeometry
 */
enum RpAtomicSetGeomFlag
{
    rpATOMICSAMEBOUNDINGSPHERE = 0x01, /**<The bounding-sphere for the
                                        * new geometry is assumed to be the
                                        * same as the old one, if any, and
                                        * should not be recalculated.
                                        */

    rpATOMICSETGEOMFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicSetGeomFlag RpAtomicSetGeomFlag;

enum RpAtomicPrivateFlag
{
    rpATOMICPRIVATEWORLDBOUNDDIRTY = 0x01,
    rpATOMICPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicPrivateFlag rpAtomicPrivateFlag;


/**
 * \ingroup rpatomic
 * \struct RpAtomic
 * Atomic Geometry object. This should be
 * considered an opaque type. Use the RpAtomic API functions to access.
 */
typedef struct RpAtomic RpAtomic;

/**
 * \ingroup rpinterpolator
 * \struct RpInterpolator
 * Morph Target Interpolator.
 * This should be considered an opaque type.
 * Use the RpInterpolator API functions to access.
 */
typedef struct RpInterpolator RpInterpolator;

#if (!defined(DOXYGEN))
struct RpInterpolator
{
        RwInt32             flags; /**< flags */
        RwInt16             startMorphTarget; /**< startMorphTarget */
        RwInt16             endMorphTarget; /**< endMorphTarget */
        RwReal              time; /**< time */
        RwReal              recipTime; /**< recipTime */
        RwReal              position; /**< position */
};
#endif /* (!defined(DOXYGEN)) */

/* More callbacks */

/**
 * \ingroup rpclump
 * \struct RpClump
 * Clump Geometry object. This should be
 * considered an opaque type. Use the RpClump API functions to access.
 */
typedef struct RpClump RpClump;

/**
 * \ingroup rpclump
 * RpClumpCallBack represents the function called from 
 * \ref RwCameraForAllClumpsInFrustum when a clump lies inside the current 
 * camera's view frustum. It is also the callback type used with
 * \ref RpWorldForAllClumps. 
 *
 * This function should return a pointer to the current clump to indicate 
 * success. The callback may return NULL to terminate further callbacks on 
 * the clumps.
 *
 * \param  clump   Pointer to the current clump, supplied by
 *                 iterator.
 * \param  data    Pointer to developer-defined data structure.
 *
 * \return Pointer to the current clump.
 *
 * \see RpWorldForAllClumps
 * \see RpClumpGetCallBack
 * \see RpClumpSetCallBack
 * \see RwCameraForAllClumpsInFrustum
 */
typedef RpClump    *(*RpClumpCallBack) (RpClump * clump, void *data);

#if (!defined(DOXYGEN))
struct RpClump
{
        RwObject            object;

        /* Information about all the Atomics */
        RwLinkList          atomicList;

        /* Lists of lights and cameras */
        RwLinkList          lightList;
        RwLinkList          cameraList;

        /* The clump in a world */
        RwLLLink            inWorldLink;

        /* Clump frustum callback */
        RpClumpCallBack     callback;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpatomic
 * \ref RpAtomicCallBackRender represents the function called from
 * \ref RpAtomicRender when the specified atomic
 * lies inside the current camera's view frustum. The default callback
 * initiates execution of the atomic rendering pipeline. This function should
 * return a pointer to the atomic to indicate success.
 *
 * \param  atomic   Pointer to the current atomic, supplied by
 *                  iterator.
 *
 * \return Returns a pointer to the atomic to indicate success.
 *
 * \see RpAtomicRender
 */

typedef RpAtomic   *(*RpAtomicCallBackRender) (RpAtomic * atomic);

#if (!defined(DOXYGEN))
struct RpAtomic
{
    RwObjectHasFrame    object;

    /* Information for an instance */
    RwResEntry         *repEntry;

    /* Triangles making the object */
    RpGeometry         *geometry;

    /* Interpolated bounding sphere (in object space and world space) */
    RwSphere            boundingSphere;
    RwSphere            worldBoundingSphere;

    /* Connections to other atomics */
    RpClump            *clump;
    RwLLLink            inClumpLink;

    /* callbacks */
    RpAtomicCallBackRender renderCallBack;

    /* Interpolation animation pointer */
    RpInterpolator      interpolator;

    /* Counter for checks of "render has occurred already" */
    RwUInt16            renderFrame;
    RwUInt16            pad;

    /* Connections to sectors */
    RwLinkList          llWorldSectorsInAtomic;

    /* The Atomic object pipeline for this Atomic */
    RxPipeline         *pipeline;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpatomic
 * \ref RpAtomicCallBack represents the function called from \ref RpWorldSectorForAllAtomics and
 * \ref RpClumpForAllAtomics for all atomics in a given world sector or clump.
 * This function should return a pointer to the current atomic to indicate
 * success. The callback may return NULL to terminate further callbacks on
 * the world sector.
 *
 * \param  atomic   Pointer to the current atomic, supplied by
 *                  iterator.
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Returns a pointer to the current atomic
 */
typedef RpAtomic   *(*RpAtomicCallBack) (RpAtomic * atomic, void *data);

typedef struct RpTie RpTie;

/**
 * \ingroup rpatomic
 * \struct RpTie
 *
 * RpTie is a linked list of atomics inside world sectors. These are
 * created when frame hierarchies are updated as part of an \ref RwCameraBeginUpdate.
 * This is used for frustum culling atomics by world sector.
 * Creation and destruction of RpTies is internal to the world plugin.
 */
struct RpTie
{
    /* Information for an atomic sector */
    RwLLLink            lAtomicInWorldSector; /**< Atomics IN this ATOMIC SECTOR */
    RpAtomic           *apAtom;               /**< An atomic */

    /* Information for a atomic */
    RwLLLink            lWorldSectorInAtomic; /**< Atomic sectors HOLDING this atomic */
    RpWorldSector      *worldSector;          /**< A world sector */
};

typedef struct RpClumpChunkInfo RpClumpChunkInfo;
typedef struct RpClumpChunkInfo _rpClump;

#if (!defined(DOXYGEN))
struct RpClumpChunkInfo
{
    RwInt32             numAtomics;
    RwInt32             numLights;
    RwInt32             numCameras;
};

#endif /* (!defined(DOXYGEN)) */

/****************************************************************************
 <macro/inline functionality

 */

 /* NB "RpAtomicRender(atom++) will break it */
#define RpAtomicRenderMacro(_atomic)                                    \
    ((_atomic)->renderCallBack(_atomic))

#define RpAtomicGetGeometryMacro(_atomic)                               \
    ((_atomic)->geometry)

#if (!defined(RpAtomicSetRenderCallBackMacro))

/* NB "RpAtomicSetRenderCallBack(atom++, callback)" will break it */
#define RpAtomicSetRenderCallBackMacro(_atomic, _callback)              \
MACRO_START                                                             \
{                                                                       \
    (_atomic)->renderCallBack = (_callback);                            \
    if (!(_atomic)->renderCallBack)                                     \
    {                                                                   \
        (_atomic)->renderCallBack = AtomicDefaultRenderCallBack;        \
    }                                                                   \
}                                                                       \
MACRO_STOP

#endif /* (!defined(RpAtomicSetRenderCallBackMacro)) */

#define RpAtomicGetRenderCallBackMacro(_atomic)                         \
    ((_atomic)->renderCallBack)

#define RpAtomicGetInterpolatorMacro(_atomic)                         \
    (&((_atomic)->interpolator))

#define RpInterpolatorGetStartMorphTargetMacro(_intrp)                  \
    ((_intrp)->startMorphTarget)

#define RpInterpolatorGetEndMorphTargetMacro(_intrp)                    \
    ((_intrp)->endMorphTarget)

#define RpInterpolatorGetValueMacro(_intrp)                             \
    ((_intrp)->position)

#define RpInterpolatorGetScaleMacro(_intrp)                             \
    ((_intrp)->time)

/* NB "RpInterpolatorSetStartMorphTarget(interp++, target)" will break it */
#define RpInterpolatorSetStartMorphTargetMacro(_intrp, _target, _atomic)\
    ((_intrp)->startMorphTarget = (RwInt16) (_target),                  \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetEndMorphTarget(interp++, target)" will break it */
#define RpInterpolatorSetEndMorphTargetMacro(_intrp, _target, _atomic)  \
    ((_intrp)->endMorphTarget = (RwInt16) (_target),                    \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetValue(interp++, value)" will break it */
#define RpInterpolatorSetValueMacro(_intrp, _value, _atomic)            \
    ((_intrp)->position = (_value),                                     \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetScale(interp++, *(scale++))" will break it */
#define RpInterpolatorSetScaleMacro(_intrp, _scale, _atomic)            \
    ((_intrp)->time = (_scale),                                         \
     (_intrp)->recipTime = (RwReal) (1.0) / (_scale),                   \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

#define RpAtomicGetClumpMacro(_atomic)                                  \
    ((_atomic)->clump)

/* NB "RpAtomicGetBoundingSphere(atomic++)" will break it */
#define RpAtomicGetBoundingSphereMacro(_atomic)                         \
    ((((_atomic)->interpolator.flags & rpINTERPOLATORDIRTYSPHERE)?      \
      _rpAtomicResyncInterpolatedSphere(_atomic), 0: 0),                \
      &((_atomic)->boundingSphere))
#define RpAtomicGetFrameMacro(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))

/* NB "RpClumpSetFrame(clump++, frame)" will break it */
#if (!defined(RpClumpSetFrameMacro))
#define RpClumpSetFrameMacro(_clump, _frame)                            \
    (rwObjectSetParent(_clump, _frame),                                 \
     (_clump))
#endif /* (!defined(RpClumpSetFrameMacro)) */

#if (!defined(RpClumpSetFrameVoidMacro))
#define RpClumpSetFrameVoidMacro(_clump, _frame)                        \
MACRO_START                                                             \
{                                                                       \
    rwObjectSetParent(_clump, _frame);                                  \
}                                                                       \
MACRO_STOP
#endif /* (!defined(RpClumpSetFrameVoidMacro)) */

#define RpClumpGetFrameMacro(_clump)                                    \
    ((RwFrame *) rwObjectGetParent(_clump))

/* NB "RpAtomicSetFlags(atomic++, flags)" will break it */
#if (!defined(RpAtomicSetFlagsMacro))
#define RpAtomicSetFlagsMacro(_atomic, _flags)                          \
    (rwObjectSetFlags(_atomic, _flags),                                 \
     (_atomic))
#endif /* (!defined(RpAtomicSetFlagsMacro)) */

#define RpAtomicGetFlagsMacro(_atomic)                                  \
    (rwObjectGetFlags(_atomic))

#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))

#define RpAtomicRender(_atomic) \
    RpAtomicRenderMacro(_atomic)

#define RpAtomicGetGeometry(_atomic) \
    RpAtomicGetGeometryMacro(_atomic)

#define RpAtomicSetRenderCallBack(_atomic, _callback) \
    RpAtomicSetRenderCallBackMacro(_atomic, _callback)

#define RpAtomicGetRenderCallBack(_atomic) \
    RpAtomicGetRenderCallBackMacro(_atomic)

#define RpAtomicGetInterpolator(_atomic) \
    RpAtomicGetInterpolatorMacro(_atomic)

#define RpInterpolatorGetStartMorphTarget(_intrp) \
    RpInterpolatorGetStartMorphTargetMacro(_intrp)

#define RpInterpolatorGetEndMorphTarget(_intrp) \
    RpInterpolatorGetEndMorphTargetMacro(_intrp)

#define RpInterpolatorGetValue(_intrp) \
    RpInterpolatorGetValueMacro(_intrp)

#define RpInterpolatorGetScale(_intrp) \
    RpInterpolatorGetScaleMacro(_intrp)

#define RpInterpolatorSetStartMorphTarget(_intrp, _target, _atomic) \
    RpInterpolatorSetStartMorphTargetMacro(_intrp, _target, _atomic)

#define RpInterpolatorSetEndMorphTarget(_intrp, _target, _atomic) \
    RpInterpolatorSetEndMorphTargetMacro(_intrp, _target, _atomic)

#define RpInterpolatorSetValue(_intrp, _value, _atomic) \
    RpInterpolatorSetValueMacro(_intrp, _value, _atomic)

#define RpInterpolatorSetScale(_intrp, _scale, _atomic) \
    RpInterpolatorSetScaleMacro(_intrp, _scale, _atomic)

#define RpAtomicGetClump(_atomic) \
    RpAtomicGetClumpMacro(_atomic)

#define RpAtomicGetBoundingSphere(_atomic) \
    RpAtomicGetBoundingSphereMacro(_atomic)

#define RpAtomicGetFrame(_atomic) \
    RpAtomicGetFrameMacro(_atomic)

#define RpClumpSetFrame(_clump, _frame) \
    RpClumpSetFrameMacro(_clump, _frame)

#define RpClumpGetFrame(_clump) \
    RpClumpGetFrameMacro(_clump)

#define RpAtomicSetFlags(_atomic, _flags) \
    RpAtomicSetFlagsMacro(_atomic, _flags)

#define RpAtomicGetFlags(_atomic) \
    RpAtomicGetFlagsMacro(_atomic)

#endif /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

/*
 * World handling.
 * World give objects scope, and provide a mechanism for
 * efficiency static object rendering.
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 *
 */

/****************************************************************************
 Defines
 */

/* Type ID */
#define rpWORLD 7

/* RpWorld private flags (in RwObject) */
enum RpWorldPrivateFlag
{
    rpWORLDSINGLEMALLOC = 0x01,
    rpWORLDPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldPrivateFlag RpWorldPrivateFlag;

/**
 * \ingroup rpworldsub
 * The bit-field type \ref RpWorldFlag specifies the options available
 * for creating the static geometry component of a world (see API function \ref RpWorldSetFlags):
 */
enum RpWorldFlag
{
    rpWORLDTRISTRIP  = 0x01, /**<This world's meshes can be rendered
                                 as tri strips */
    rpWORLDPOSITIONS = 0x02, /**<This world has positions */
    rpWORLDTEXTURED  = 0x04, /**<This world has only one set of texture coordinates */
    rpWORLDPRELIT    = 0x08, /**<This world has luminance values */
    rpWORLDNORMALS   = 0x10, /**<This world has normals */
    rpWORLDLIGHT     = 0x20, /**<This world will be lit */
    rpWORLDMODULATEMATERIALCOLOR = 0x40,
                             /**<Modulate material color with vertex colors
                                 (pre-lit + lit) */
    rpWORLDTEXTURED2 = 0x80, /**<This world has 2 or more sets of texture coordinates */

    /*
     * These above flags were stored in the flags field in an RwObject, they
     * are now stored in the flags file of the RpWorld.
     */

    rpWORLDNATIVE               = 0x01000000,
    rpWORLDNATIVEINSTANCE       = 0x02000000,

    rpWORLDFLAGSMASK            = 0x000000FF,
    rpWORLDNATIVEFLAGSMASK      = 0x0F000000,

    rpWORLDSECTORSOVERLAP       = 0x40000000,

    rpWORLDFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldFlag RpWorldFlag;

/* Multi texture coordinate format specifier */
#define rpWORLDTEXCOORDSETS(_num)  ((_num & 0xff) << 16)

/* Maximum depth of BSP tree */
#define rpWORLDMAXBSPDEPTH 64

/****************************************************************************
 Global types
 */

/**
 * \ingroup rpworldsub
 *
 * A pluginID is a unique identifiers for a plugin. This is a list of the IDs for
 * RpWorld plugin, which is not just one plugin, but a collection of plugins
 * that extend RenderWare Graphics Core. See \ref RwEngineRegisterPlugin
 *
 */
enum RwCriterionWorldID
{
    /* Guard value that should not be used. */
    rwID_NAWORLDID = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x00),

    /* The valid plugin IDs */
    /**< RpMaterial pluginID */
    rwID_MATERIALMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x01),
    /**< RpMesh pluginID */
    rwID_MESHMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x02),
    /**< RpGeometry pluginID */
    rwID_GEOMETRYMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x03),
    /**< RpClump pluginID */
    rwID_CLUMPMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x04),
    /**< RpLight pluginID */
    rwID_LIGHTMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x05),
    /* Not used */
    rwID_COLLISIONMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x06),
    /**< RpWorld pluginID */
    rwID_WORLDMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x07),
    /* Not used */
    rwID_RANDOMMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x08),
    /**< PluginID for RpWorld's objects */
    rwID_WORLDOBJMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x09),
    /**< RpWorldSector pluginID */
    rwID_SECTORMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0A),
    /**< Binary RpWorld pluginID */
    rwID_BINWORLDMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0B),
    /**< RpWorld pipeline pluginID */
    rwID_WORLDPIPEMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0D),
    /**< Binary RpMesh pluginID */
    rwID_BINMESHPLUGIN       = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0E),
    /**< RpWorld device pluginID */
    rwID_RXWORLDDEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0F),
    /**< PluginID for platform specific serialization data */
    rwID_NATIVEDATAPLUGIN    = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10),
    /**< \if xbox Vertex format pluginID \endif */
    /**< \if gcn  Vertex format pluginID \endif */
    rwID_VERTEXFMTPLUGIN     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x11),
    rwCRITERIONWORLDIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionWorldID RwCriterionWorldID;

/**
 * \ingroup rpworldsub
 * \ref RpWorldRenderOrder
 * represents the options available for
 * the rendering order of world sectors in the camera's view frustum (see
 * API function \ref RpWorldSetRenderOrder).
 */
enum RpWorldRenderOrder
{
    rpWORLDRENDERNARENDERORDER = 0,
    rpWORLDRENDERFRONT2BACK,  /**<Renders nearest sectors first */
    rpWORLDRENDERBACK2FRONT, /**<Renders furthest sectors first */
    rpWORLDRENDERORDERFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldRenderOrder RpWorldRenderOrder;

/**
 * \ingroup rpworldsector
 * \ref RpWorldSectorCallBackRender represents the function called from
 * \ref RpWorldSectorRender when the specified world sector lies inside the
 * current camera's view frustum. The default callback initiates execution of
 * the world sector rendering pipeline. This function should return a pointer
 * to the world sector to indicate success
 *
 * \param  worldSector   Pointer to the world sector to be
 *                       rendered
 *
 * \return Pointer to the world sector.
 *
 */
typedef RpWorldSector *(*RpWorldSectorCallBackRender) (RpWorldSector *
                                                       worldSector);

/**
 * \ingroup rpworldsub
 * \struct RpWorld
 * World object. This should be considered an opaque type.
 * Use the RpWorld API functions to access.
 */
typedef struct RpWorld RpWorld;

#if (!defined(DOXYGEN))
struct RpWorld
{
    RwObject            object;

    RwUInt32            flags;

    RpWorldRenderOrder  renderOrder;

    /* Materials */
    RpMaterialList      matList;

    /* The world stored as a BSP tree */
    RpSector           *rootSector;

    /* The number of texture coordinate sets in each sector */
    RwInt32             numTexCoordSets;

    /* Render frame used when last rendered */
    RwInt32             numClumpsInWorld;
    RwLLLink           *currentClumpLink;

    /* All the clumps in the world */
    RwLinkList          clumpList;

    /* All of the lights in the world */
    RwLinkList          lightList;

    /* Directional lights in the world */
    RwLinkList          directionalLightList;

    /* The worlds origin offset */
    RwV3d               worldOrigin;

    /* Bounding box around the whole world */
    RwBBox              boundingBox;

    /* The callbacks functions */
    RpWorldSectorCallBackRender renderCallBack;

    RxPipeline         *pipeline;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/pipe/p2/d3d9/d3d9usage.h ---*/
/****************************************************************************
 Global Types
 */

/**
 * \ingroup worldextensionsd3d9
 * RpD3D9GeometryUsageFlag represents the options available to inform
 * RWD3D9 about the future usage of a specific geometry.
 * The usage flags may be set via \ref RpD3D9GeometrySetUsageFlags.
 */
enum RpD3D9GeometryUsageFlag
{
    rpD3D9GEOMETRYUSAGE_DYNAMICPOSITIONS  =     0x02,   /**<Positions will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICNORMALS    =     0x04,   /**<Normals will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICPRELIT     =     0x08,   /**<Prelight information will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS  =     0x10,   /**<The 1st set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS1 =     0x10,   /**<The 1st set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS2 =     0x20,   /**<The 2nd set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS3 =     0x40,   /**<The 3rd set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS4 =     0x80,   /**<The 4th set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS5 =   0x0100,   /**<The 5th set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS6 =   0x0200,   /**<The 6th set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS7 =   0x0400,   /**<The 7th set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDS8 =   0x0800,   /**<The 8th set of texture coordinates will be locked and changed every frame. */
    rpD3D9GEOMETRYUSAGE_DYNAMICTEXCOORDSALL = 0x0ff0,   /**<All sets of texture coordinates will be locked and changed every frame. */

    rpD3D9GEOMETRYUSAGE_DYNAMICMASK       =   0x0fff,   /**<Mask for finding dynamic information. */

    rpD3D9GEOMETRYUSAGE_CREATETANGENTS    =  0x10000,   /**<Per-vertex tangents are going to be used for this geometry. */

    rpD3D9GEOMETRYUSAGEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT

};
typedef enum RpD3D9GeometryUsageFlag RpD3D9GeometryUsageFlag;

/**
 * \ingroup worldextensionsd3d9
 * RpD3D9WorldSectorUsageFlag represents the options available to inform
 * RWD3D9 about the future usage of a specific world sector.
 * The usage flags may be set via \ref RpD3D9WorldSectorSetUsageFlags.
 */
enum RpD3D9WorldSectorUsageFlag
{
    rpD3D9WORLDSECTORUSAGE_CREATETANGENTS    =  0x10000,   /**<Per-vertex tangents are going to be used for this world sector. */

    rpD3D9WORLDSECTORUSAGEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT

};
typedef enum RpD3D9WorldSectorUsageFlag RpD3D9WorldSectorUsageFlag;

/*--- Automatically derived from: C:/daily/rwsdk/world/pipe/p2/d3d9/D3D9pipe.h ---*/
/****************************************************************************
 Global Defines
 */
#define RWD9DOBJECTGETTYPE(_object) (*((RwUInt8 *)(_object)))

#define RWD9DOBJECTISGEOMETRY(_object) (RWD9DOBJECTGETTYPE(_object) == (RwUInt8)rpGEOMETRY)
#define RWD9DOBJECTISATOMIC(_object) (RWD9DOBJECTGETTYPE(_object) == (RwUInt8)rpATOMIC)
#define RWD9DOBJECTISWORLDSECTOR(_object) (RWD9DOBJECTGETTYPE(_object) == (RwUInt8)-1)

/****************************************************************************
 Global Types
 */

typedef struct RxD3D9ResEntryHeader RxD3D9ResEntryHeader;
/**
 * \ingroup worldextensionsd3d9
 * \struct RxD3D9ResEntryHeader
 * This structure contains D3D9 resource specific components.
 */
struct RxD3D9ResEntryHeader
{
    RwUInt32    serialNumber;   /**< The mesh headers serial number */

    RwUInt32    numMeshes;      /**< The number of meshes */

    IDirect3DIndexBuffer9        *indexBuffer;   /**< Index buffer */ // NOTSA: Changed type from void*

    D3DPRIMITIVETYPE    primType;       /**< Primitive type */

    RxD3D9VertexStream vertexStream[RWD3D9_MAX_VERTEX_STREAMS];   /**< Vertex streams */

    RwBool      useOffsets;      /**< Use vertex buffer offsets when setting the streams */

    IDirect3DVertexDeclaration9        *vertexDeclaration;   /**< Vertex declaration */ // NOTSA: Changed type from `void*`

    RwUInt32    totalNumIndex;  /**< Total number of indices. Needed for
                                     reinstancing, not for rendering */

    RwUInt32    totalNumVertex; /**< Total number of vertices. Needed for
                                     reinstancing, not for rendering */
};

/* This is what I keep in memory as part of the instance data setup */
typedef struct RxD3D9InstanceData RxD3D9InstanceData;
/**
 * \ingroup worldextensionsd3d9
 * \struct RxD3D9InstanceData
 * This structure contains D3D9 resource specific components.
 */
struct RxD3D9InstanceData
{
    RwUInt32        numIndex;       /**< Number of indices. Needed for
                                         reinstancing, not for rendering */

    RwUInt32        minVert;        /**< The vertex index where the
                                         meshes vertices begin. Needed for
                                         reinstancing, not for rendering */

    RpMaterial      *material;      /**< The material for this instanced data */

    RwBool          vertexAlpha;    /**< The prelight data contains alpha */

    IDirect3DVertexShader9            *vertexShader;   /**< Vertex shader */

    RwUInt32        baseIndex;      /**< Needed if we share the vertex buffer */

    RwUInt32        numVertices;    /**< Number of vertices in the vertex buffer */

    RwUInt32        startIndex;     /**< Location in the index array to start
                                         reading vertices. */

    RwUInt32        numPrimitives;  /**< Number of primitives to render */
};

/**
 * \ingroup worldextensionsd3d9
 * \ref RxD3D9AllInOneInstanceCallBack callback function.
 * 
 * \param object Pointer to the object.
 * \param resEntryHeader Pointer to \ref RxD3D9ResEntryHeader.
 * \param reinstance Boolean whether to reinstance.
 *
 * \return TRUE if successful, FALSE otherwise.
 *
 * \see RxD3D9AllInOneSetInstanceCallBack
 * \see RxD3D9AllInOneGetInstanceCallBack
 * \see RxNodeDefinitionGetD3D9AtomicAllInOne
 * \see RxNodeDefinitionGetD3D9WorldSectorAllInOne
 */
typedef RwBool (*RxD3D9AllInOneInstanceCallBack)(void *object,
                                                 RxD3D9ResEntryHeader *resEntryHeader,
                                                 RwBool reinstance);

/**
 * \ingroup worldextensionsd3d9
 * \ref RxD3D9AllInOneReinstanceCallBack callback function.
 * 
 * \param object Pointer to the object.
 * \param resEntry Pointer to \ref RxD3D9ResEntryHeader.
 * \param instanceCallback Pointer to \ref RxD3D9AllInOneInstanceCallBack.
 *
 * \return TRUE if successful, FALSE otherwise.
 *
 * \see RxD3D9AllInOneSetReinstanceCallBack
 * \see RxD3D9AllInOneGetReinstanceCallBack
 * \see RxNodeDefinitionGetD3D9AtomicAllInOne
 * \see RxNodeDefinitionGetD3D9WorldSectorAllInOne
 */
typedef RwBool (*RxD3D9AllInOneReinstanceCallBack)(void *object,
                                                   RwResEntry *resEntry,
                                                   RxD3D9AllInOneInstanceCallBack instanceCallback);

/**
 * \ingroup worldextensionsd3d9
 * \ref RxD3D9AllInOneLightingCallBack callback function.
 * 
 * \param object Void pointer to the object.
 *
 * \see RxD3D9AllInOneSetLightingCallBack
 * \see RxD3D9AllInOneGetLightingCallBack
 * \see RxNodeDefinitionGetD3D9AtomicAllInOne
 * \see RxNodeDefinitionGetD3D9WorldSectorAllInOne
 */
typedef void (*RxD3D9AllInOneLightingCallBack)(void *object);

/**
 * \ingroup worldextensionsd3d9
 * \ref RxD3D9AllInOneRenderCallBack callback function.
 * 
 * \param repEntry Pointer to a resource entry.
 * \param object Pointer to a object that the resource entry belongs to.
 * \param type rpATOMIC or rwSECTORATOMIC.
 * \param flags \ref RpGeometryFlag's or \ref RpWorldFlag's.
 *
 * \see RxD3D9AllInOneSetRenderCallBack
 * \see RxD3D9AllInOneGetRenderCallBack
 * \see RxNodeDefinitionGetD3D9AtomicAllInOne
 * \see RxNodeDefinitionGetD3D9WorldSectorAllInOne
 */
typedef void (*RxD3D9AllInOneRenderCallBack)(RwResEntry *repEntry,
                                             void *object,
                                             RwUInt8 type,
                                             RwUInt32 flags);


/*--- Automatically derived from: C:/daily/rwsdk/world/babinwor.h ---*/
/****************************************************************************
 Global types
 */

/* Binary Representation
 *
 */
typedef struct RpWorldChunkInfoSector RpWorldSectorChunkInfo;
typedef struct RpWorldChunkInfoSector _rpWorldSector;

#if (!defined(DOXYGEN))
struct RpWorldChunkInfoSector
{
    RwInt32 matListWindowBase;
    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwV3d inf;
    RwV3d sup;
    RwBool collSectorPresent;   /* unused but retains same struct size */
    RwBool unused;
};


typedef struct RpPlaneSectorChunkInfo RpPlaneSectorChunkInfo;
typedef struct RpPlaneSectorChunkInfo _rpPlaneSector;

struct RpPlaneSectorChunkInfo
{
    RwInt32 type;
    RwReal value;    
    RwBool leftIsWorldSector;
    RwBool rightIsWorldSector;
    RwReal leftValue;
    RwReal rightValue;
};

typedef struct RpWorldChunkInfo RpWorldChunkInfo;
typedef struct RpWorldChunkInfo _rpWorld;

struct RpWorldChunkInfo
{
    RwBool rootIsWorldSector;

    RwV3d invWorldOrigin;

    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numPlaneSectors;
    RwInt32 numWorldSectors;
    RwInt32 colSectorSize;    

    RwInt32 format;  /* Flags about the world */

    /* Added in 34003 */
    RwBBox  boundingBox;
};

typedef struct rpWorldChunkInfo34000 rpWorldChunkInfo34000;

struct rpWorldChunkInfo34000
{
    RwBool rootIsWorldSector;

    RwV3d invWorldOrigin;

    RwSurfaceProperties surfaceProps;

    RwInt32 numTriangles;
    RwInt32 numVertices;
    RwInt32 numPlaneSectors;
    RwInt32 numWorldSectors;
    RwInt32 colSectorSize;    

    RwInt32 format;  /* Flags about the world */
};
#endif /* (!defined(DOXYGEN)) */

void _rwD3D9VSSetActiveWorldMatrix(const RwMatrix* worldMatrix); // 0x764650
void _rwD3D9VSGetComposedTransformMatrix(); // 0x7646E0
void _rwD3D9VSGetWorldViewTransposedMatrix(); // 0x764730
void _rwD3D9VSGetWorldViewMatrix(); // 0x764760
void _rwD3D9VSGetInverseWorldMatrix(); // 0x7647B0
void _rwD3D9VSGetWorldMultiplyMatrix(RwMatrix* worldMultiplyMatrix, const RwMatrix* matrix); // 0x764920
void _rwD3D9VSGetWorldMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix); // 0x764960
void _rwD3D9VSGetWorldViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix); // 0x7649D0
void _rwD3D9VSGetWorldNormalizedMultiplyTransposeMatrix(void* worldMultiplyMatrix, const RwMatrix* matrix); // 0x764A70
void _rwD3D9VSGetWorldNormalizedViewMultiplyTransposeMatrix(void* worldViewMultiplyMatrix, const RwMatrix* matrix); // 0x764B50
void _rwD3D9VSGetWorldNormalizedTransposeMatrix(); // 0x764C60
void _rwD3D9VSGetProjectionTransposedMatrix(); // 0x764D20
void _rwD3D9VSGetNormalInLocalSpace(const RwV3d* normalWorldSpace, RwV3d* normalLocalSpace); // 0x764D30
void _rwD3D9VSGetPointInLocalSpace(const RwV3d* pointWorldSpace, RwV3d* pointLocalSpace); // 0x764E70
void _rwD3D9VSGetRadiusInLocalSpace(RwReal radiusWorldSpace, RwReal* radiusLocalSpace); // 0x764F60
void _rpD3D9VertexShaderCachePurge(); // 0x75EE60
void* _rpD3D9GetVertexShader(const _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x75EED0
RwUInt32 _rpD3D9GetNumConstantsUsed(const _rpD3D9VertexShaderDescriptor* desc); // 0x75EDD0
void _rxD3D9VertexShaderDefaultBeginCallBack(void* object, RwUInt32 type, _rpD3D9VertexShaderDescriptor* desc); // 0x760DF0
RwV4d* _rxD3D9VertexShaderDefaultLightingCallBack(void* object, RwUInt32 type, RwV4d* shaderConstantPtr, _rpD3D9VertexShaderDescriptor* desc); // 0x761170
void* _rxD3D9VertexShaderDefaultGetMaterialShaderCallBack(const RpMaterial* material, _rpD3D9VertexShaderDescriptor* desc, _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761010
void _rxD3D9VertexShaderDefaultMeshRenderCallBack(RxD3D9ResEntryHeader* resEntryHeader, RxD3D9InstanceData* instancedMesh, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761030
void _rxD3D9VertexShaderDefaultEndCallBack(void* object, RwUInt32 type, _rpD3D9VertexShaderDescriptor* desc); // 0x761000
void _rpD3D9VertexShaderUpdateLightsColors(RwV4d* shaderConstantPtr, const _rpD3D9VertexShaderDescriptor* desc, RwReal ambientCoef, RwReal diffuseCoef); // 0x761720
void _rpD3D9VertexShaderUpdateMaterialColor(const RwRGBA* color, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761820
void _rpD3D9VertexShaderUpdateFogData(const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x7618B0
void _rpD3D9VertexShaderUpdateMorphingCoef(RwReal morphingCoef, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761950
void _rpD3DVertexShaderSetUVAnimMatrix(RwMatrix* matrix, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x7619A0
void _rpD3D9VertexShaderSetEnvMatrix(RwFrame* frame, const _rpD3D9VertexShaderDescriptor* desc, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761A10
void _rpD3D9VertexShaderSetBumpMatrix(RwFrame* frame, RwReal factor, const _rpD3D9VertexShaderDispatchDescriptor* dispatch); // 0x761B70
RxNodeDefinition* RxNodeDefinitionGetD3D9WorldSectorAllInOne(); // 0x75E9F0
RxNodeDefinition* RxNodeDefinitionGetD3D9AtomicAllInOne(); // 0x7582E0
void RpMaterialSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x74D8C0
RpMaterial* RpMaterialCreate(); // 0x74D990
RwBool RpMaterialDestroy(RpMaterial* material); // 0x74DA20
RpMaterial* RpMaterialClone(RpMaterial* material); // 0x74DA80
RpMaterial* RpMaterialSetTexture(RpMaterial* material, RwTexture* texture); // 0x74DBC0
RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x74DBF0
RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x74DC20
RwInt32 RpMaterialSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x74DC50
RwInt32 RpMaterialSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB); // 0x74DC70
RwInt32 RpMaterialGetPluginOffset(RwUInt32 pluginID); // 0x74DC90
RwBool RpMaterialValidatePlugins(const RpMaterial* material); // 0x74DCB0
RwUInt32 RpMaterialStreamGetSize(const RpMaterial* material); // 0x74E010
RpMaterial* RpMaterialStreamRead(RwStream* stream); // 0x74DD30
const RpMaterial* RpMaterialStreamWrite(const RpMaterial* material, RwStream* stream); // 0x74E050
RpMaterialChunkInfo* _rpMaterialChunkInfoRead(RwStream* stream, RpMaterialChunkInfo* materialChunkInfo, RwInt32* bytesRead); // 0x74DCC0
RpMaterialList* _rpMaterialListInitialize(RpMaterialList* matList); // 0x74E1B0
RpMaterialList* _rpMaterialListDeinitialize(RpMaterialList* matList); // 0x74E150
RpMaterial** _rpMaterialListAlloc(RwUInt32 count); // 0x74E1C0
RpMaterial* _rpMaterialListGetMaterial(const RpMaterialList* matList, RwInt32 matIndex); // 0x74E2B0
RpMaterialList* _rpMaterialListSetSize(RpMaterialList* matList, RwInt32 size); // 0x74E2C0
RpMaterialList* _rpMaterialListCopy(RpMaterialList* matListOut, const RpMaterialList* matListIn); // 0x74E1F0
RwInt32 _rpMaterialListAppendMaterial(RpMaterialList* matList, RpMaterial* material); // 0x74E350
RwInt32 _rpMaterialListFindMaterialIndex(const RpMaterialList* matList, const RpMaterial* material); // 0x74E420
RwUInt32 _rpMaterialListStreamGetSize(const RpMaterialList* matList); // 0x74E450
RpMaterialList* _rpMaterialListStreamRead(RwStream* stream, RpMaterialList* matList); // 0x74E600
const RpMaterialList* _rpMaterialListStreamWrite(const RpMaterialList* matList, RwStream* stream); // 0x74E4B0
void* _rpMeshOpen(void* instance, RwInt32 offset, RwInt32 size); // 0x758970
void* _rpMeshClose(void* instance, RwInt32 offset, RwInt32 size); // 0x758940
RwInt16 _rpMeshGetNextSerialNumber(void); // 0x7590E0
RpBuildMesh* _rpBuildMeshCreate(RwUInt32 bufferSize); // 0x758A90
RwBool _rpBuildMeshDestroy(RpBuildMesh* mesh); // 0x758B80
RwBool _rpMeshDestroy(RpMeshHeader* mesh); // 0x758BC0
RpBuildMesh* _rpBuildMeshAddTriangle(RpBuildMesh* mesh, RpMaterial* material, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3, RwUInt16 matIndex, RwUInt16 textureIndex, RwUInt16 rasterIndex, RwUInt16 pipelineIndex); // 0x758C00
RpMeshHeader* _rpMeshHeaderForAllMeshes(RpMeshHeader* meshHeader, RpMeshCallBack fpCallBack, void* data); // 0x758D30
RwStream* _rpMeshWrite(const RpMeshHeader* meshHeader, const void* object, RwStream* stream, const RpMaterialList* matList); // 0x758D70
RpMeshHeader* _rpMeshRead(RwStream* stream, const void* object, const RpMaterialList* matList); // 0x758EC0
RwInt32 _rpMeshSize(const RpMeshHeader* meshHeader, const void* object); // 0x759090
void _rpMeshHeaderDestroy(RpMeshHeader* meshHeader); // 0x758910
RpMeshHeader* _rpMeshHeaderCreate(RwUInt32 size); // 0x758920
RpTriStripPolygon* RpTriStripPolygonFollowStrip(RpTriStripPolygon* curr, RpTriStripPolygon* prev); // 0x75C330
RwUInt32 RpTriStripDefaultCost(RpTriStripPolygon* startPolygon, RwUInt32 testFrame, void* data); // 0x75B500
RpTriStripMesh* RpTriStripMeshTunnel(RpTriStripMesh* mesh, void* data); // 0x75B780
RpTriStripMesh* RpTriStripMeshQuick(RpTriStripMesh* mesh, void* data); // 0x75BD80
RpMeshHeader* RpBuildMeshGenerateTriStrip(RpBuildMesh* buildMesh, void* data); // 0x75C380
RpMeshHeader* RpBuildMeshGenerateTrivialTriStrip(RpBuildMesh* buildMesh, void* data); // 0x759100
RpMeshHeader* RpBuildMeshGenerateDefaultTriStrip(RpBuildMesh* buildmesh, void* data); // 0x7591B0
RpMeshHeader* RpBuildMeshGeneratePreprocessTriStrip(RpBuildMesh* buildmesh, void* data); // 0x75A900
RpMeshHeader* RpBuildMeshGenerateExhaustiveTriStrip(RpBuildMesh* buildmesh, void* data); // 0x75A940
RpMeshHeader* RpBuildMeshGenerateDefaultIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data); // 0x75A8E0
RpMeshHeader* RpBuildMeshGeneratePreprocessIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data); // 0x75A920
RpMeshHeader* RpBuildMeshGenerateExhaustiveIgnoreWindingTriStrip(RpBuildMesh* buildmesh, void* data); // 0x75B4E0
RwBool RpMeshSetTriStripMethod(RpTriStripMeshCallBack callback, void* data); // 0x75D4C0
RwBool RpMeshGetTriStripMethod(RpTriStripMeshCallBack* callback, void* data); // 0x75D500
RpMeshHeader* _rpTriListMeshGenerate(RpBuildMesh* buildMesh, void* data); // 0x75D530
RpMeshHeader* _rpMeshOptimise(RpBuildMesh* buildmesh, RwUInt32 flags); // 0x75D970
RpGeometry* RpGeometryTransform(RpGeometry* geometry, const RwMatrix* matrix); // 0x74BFE0
RpGeometry* RpGeometryCreateSpace(RwReal radius); // 0x74C130
const RpMorphTarget* RpMorphTargetCalcBoundingSphere(const RpMorphTarget* morphTarget, RwSphere* boundingSphere); // 0x74C200
RwInt32 RpGeometryAddMorphTargets(RpGeometry* geometry, RwInt32 mtcount); // 0x74C310
RwInt32 RpGeometryAddMorphTarget(RpGeometry* geometry); // 0x74C4D0
RpGeometry* RpGeometryRemoveMorphTarget(RpGeometry* geometry, RwInt32 morphTarget); // 0x74C4E0
const RpGeometry* RpGeometryTriangleSetVertexIndices(const RpGeometry* geometry, RpTriangle* triangle, RwUInt16 vert1, RwUInt16 vert2, RwUInt16 vert3); // 0x74C690
RpGeometry* RpGeometryTriangleSetMaterial(RpGeometry* geometry, RpTriangle* triangle, RpMaterial* material); // 0x74C6C0
const RpGeometry* RpGeometryTriangleGetVertexIndices(const RpGeometry* geometry, const RpTriangle* triangle, RwUInt16* vert1, RwUInt16* vert2, RwUInt16* vert3); // 0x74C720
RpMaterial* RpGeometryTriangleGetMaterial(const RpGeometry* geometry, const RpTriangle* triangle); // 0x74C760
RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data); // 0x74C790
RpGeometry* RpGeometryLock(RpGeometry* geometry, RwInt32 lockMode); // 0x74C7D0
RpGeometry* RpGeometryUnlock(RpGeometry* geometry); // 0x74C800
const RpGeometry* RpGeometryForAllMeshes(const RpGeometry* geometry, RpMeshCallBack fpCallBack, void* data); // 0x74CA60
RpGeometry* RpGeometryCreate(RwInt32 numVert, RwInt32 numTriangles, RwUInt32 format); // 0x74CA90
RwBool RpGeometryDestroy(RpGeometry* geometry); // 0x74CCC0
RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x74CD70
RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x74CDA0
RwInt32 RpGeometrySetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x74CDD0
RwInt32 RpGeometryGetPluginOffset(RwUInt32 pluginID); // 0x74CDF0
RwBool RpGeometryValidatePlugins(const RpGeometry* geometry); // 0x74CE10
RwUInt32 RpGeometryStreamGetSize(const RpGeometry* geometry); // 0x74CE20
const RpGeometry* RpGeometryStreamWrite(const RpGeometry* geometry, RwStream* stream); // 0x74CED0
RpGeometry* RpGeometryStreamRead(RwStream* stream); // 0x74D190
RpGeometryChunkInfo* _rpGeometryChunkInfoRead(RwStream* stream, RpGeometryChunkInfo* geometryChunkInfo, RwInt32* bytesRead); // 0x74D750
RpWorldSector* RpWorldSectorRender(RpWorldSector* worldSector); // 0x761C50
const RpWorldSector* RpWorldSectorForAllMeshes(const RpWorldSector* sector, RpMeshCallBack fpCallBack, void* data); // 0x761C60
RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x761C90
RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x761CC0
RwInt32 RpWorldSectorSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x761CF0
RwInt32 RpWorldSectorSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB); // 0x761D10
RwInt32 RpWorldSectorGetPluginOffset(RwUInt32 pluginID); // 0x761D30
RwBool RpWorldSectorValidatePlugins(const RpWorldSector* sector); // 0x761D50
void RpLightSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x752250
RpLight* RpLightCreate(RwInt32 type); // 0x752110
RwBool RpLightDestroy(RpLight* light); // 0x7520D0
RpLight* RpLightSetRadius(RpLight* light, RwReal radius); // 0x751A70
RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color); // 0x751A90
RwReal RpLightGetConeAngle(const RpLight* light); // 0x751AE0
RpLight* RpLightSetConeAngle(RpLight* ight, RwReal angle); // 0x751D20
RwUInt32 RpLightStreamGetSize(const RpLight* light); // 0x751E10
RpLight* RpLightStreamRead(RwStream* stream); // 0x751F00
RpLightChunkInfo* _rpLightChunkInfoRead(RwStream* stream, RpLightChunkInfo* lightChunkInfo, RwInt32* bytesRead); // 0x752060
RwInt32 RpLightRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x751D60
RwInt32 RpLightRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x751D90
RwInt32 RpLightSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x751DC0
RwInt32 RpLightGetPluginOffset(RwUInt32 pluginID); // 0x751DE0
RwBool RpLightValidatePlugins(const RpLight* light); // 0x751E00
void RpD3D9LightSetAttenuationParams(RpLight* light, const RpD3D9AttenuationParams* params); // 0x755D20
void RpD3D9LightGetAttenuationParams(const RpLight* light, RpD3D9AttenuationParams* params); // 0x755D50
RwBool _rwD3D9LightsOpen(void); // 0x755D80
RwBool _rwD3D9LightsGlobalEnable(RpLightFlag flags); // 0x756070
RwBool _rwD3D9LightDirectionalEnable(RpLight* light); // 0x756260
RwBool _rwD3D9LightLocalEnable(RpLight* light); // 0x756390
void _rwD3D9LightsEnable(RwBool enable, RwUInt32 type); // 0x756600
void _rwD3D9LightsClose(void); // 0x755FE0
void RpAtomicSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x749720
void RpClumpSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x749740
RpAtomic* AtomicDefaultRenderCallBack(RpAtomic* atomic); // 0x7491C0
void _rpAtomicResyncInterpolatedSphere(RpAtomic* atomic); // 0x7491F0
const RwSphere* RpAtomicGetWorldBoundingSphere(RpAtomic* atomic); // 0x749330
void RpClumpGtaCancelStream();
RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data); // 0x749B70
RpClump* RpClumpForAllLights(RpClump* clump, RpLightCallBack callback, void* data); // 0x749C00
RpClump* RpClumpForAllCameras(RpClump* clump, RwCameraCallBack callback, void* data); // 0x749BB0
RpAtomic* RpAtomicSetFrame(RpAtomic* atomic, RwFrame* frame); // 0x74BF20
RpClump* RpClumpCreateSpace(const RwV3d* position, RwReal radius); // 0x749970
RpClump* RpClumpRender(RpClump* clump); // 0x749B20
RpClump* RpClumpRemoveAtomic(RpClump* clump, RpAtomic* atomic); // 0x74A4C0
RpClump* RpClumpAddAtomic(RpClump* clump, RpAtomic* atomic); // 0x74A490
RpClump* RpClumpRemoveLight(RpClump* clump, RpLight* light); // 0x74A520
RpClump* RpClumpAddLight(RpClump* clump, RpLight* light); // 0x74A4F0
RpClump* RpClumpRemoveCamera(RpClump* clump, RwCamera* camera); // 0x74A580
RpClump* RpClumpAddCamera(RpClump* clump, RwCamera* camera); // 0x74A550
RwBool RpClumpDestroy(RpClump* clump); // 0x74A310
RpClump* RpClumpCreate(void); // 0x74A290
RpClump* RpClumpClone(RpClump* clump); // 0x749F70
RwBool RpAtomicDestroy(RpAtomic* atomic); // 0x749DC0
RpAtomic* RpAtomicClone(RpAtomic* atomic); // 0x749E60
RpAtomic* RpAtomicCreate(void); // 0x749C50
RpAtomic* RpAtomicSetGeometry(RpAtomic* atomic, RpGeometry* geometry, RwUInt32 flags); // 0x749D40
RpClump* RpClumpSetCallBack(RpClump* clump, RpClumpCallBack callback); // 0x74A240
RpClumpCallBack RpClumpGetCallBack(const RpClump* clump); // 0x74A270
RwInt32 RpClumpGetNumAtomics(RpClump* clump); // 0x7498E0
RwInt32 RpClumpGetNumLights(RpClump* clump); // 0x749910
RwInt32 RpClumpGetNumCameras(RpClump* clump); // 0x749940
RpClump* RpLightGetClump(const RpLight* light); // 0x749E40
RpClump* RwCameraGetClump(const RwCamera* camera); // 0x749E50
RwUInt32 RpAtomicStreamGetSize(RpAtomic* atomic); // 0x74A5B0
RpAtomic* RpAtomicStreamRead(RwStream* stream); // 0x74B030
RpAtomic* RpAtomicStreamWrite(RpAtomic* atomic, RwStream* stream); // 0x74A850
RwUInt32 RpClumpStreamGetSize(RpClump* clump); // 0x74A5E0
RpClump* RpClumpStreamRead(RwStream* stream); // 0x74B420
RpClump* RpClumpStreamWrite(RpClump* clump, RwStream* stream); // 0x74AA10
RpClumpChunkInfo* _rpClumpChunkInfoRead(RwStream* stream, RpClumpChunkInfo* clumpChunkInfo, RwInt32* bytesRead); // 0x74BD40
RwInt32 RpAtomicRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x74BDA0
RwInt32 RpClumpRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x74BDD0
RwInt32 RpAtomicRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x74BE00
RwInt32 RpAtomicSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x74BE30
RwInt32 RpAtomicSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB); // 0x74BE50
RwInt32 RpClumpRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x74BE70
RwInt32 RpClumpSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x74BEA0
RwInt32 RpAtomicGetPluginOffset(RwUInt32 pluginID); // 0x74BEC0
RwInt32 RpClumpGetPluginOffset(RwUInt32 pluginID); // 0x74BEE0
RwBool RpAtomicValidatePlugins(const RpAtomic* atomic); // 0x74BF00
RwBool RpClumpValidatePlugins(const RpClump* clump); // 0x74BF10
RwBool RpAtomicInstance(RpAtomic* atomic); // 0x74BF40
RwBool _rpWorldFindBBox(RpWorld* world, RwBBox* boundingBox); // 0x74EFA0
RpWorld* _rpWorldSetupSectorBoundingBoxes(RpWorld* world); // 0x74F020
void _rpWorldSectorDeinstanceAll(RpSector* sector); // 0x74ECA0
void _rpWorldSectorDestroyRecurse(RpSector* sector); // 0x74ED50
RwBool _rpWorldForAllGlobalLights(RpLightCallBack callBack, void* data); // 0x74EF10
RpWorldSector* _rpWorldSectorForAllLocalLights(RpWorldSector* sector, RpLightCallBack callBack, void* data); // 0x74EF60
RpWorldSector* _rpSectorDefaultRenderCallBack(RpWorldSector* sector); // 0x74EEC0
RpWorld* RpWorldLock(RpWorld* world); // 0x74F1A0
RpWorld* RpWorldUnlock(RpWorld* world); // 0x74F210
void _rpWorldRegisterWorld(RpWorld* world, RwUInt32 memorySize); // 0x74F0C0
void _rpWorldUnregisterWorld(RpWorld* world); // 0x74F140
RwBool RpWorldPluginAttach(); // 0x74FDA0
RpWorld* RpWorldForAllClumps(RpWorld* world, RpClumpCallBack fpCallBack, void* data); // 0x74FB80
RpWorld* RpWorldForAllMaterials(RpWorld* world, RpMaterialCallBack fpCallBack, void* data); // 0x74FBC0
RpWorld* RpWorldForAllLights(RpWorld* world, RpLightCallBack fpCallBack, void* data); // 0x74FC00
RpWorld* RpWorldForAllWorldSectors(RpWorld* world, RpWorldSectorCallBack fpCallBack, void* data); // 0x74FC70
RpWorld* RpWorldRender(RpWorld* world); // 0x74F570
RwBool RpWorldDestroy(RpWorld* world); // 0x74F610
RpWorld* RpWorldCreate(RwBBox* boundingBox); // 0x74F760
RpWorld* RpWorldSetSectorRenderCallBack(RpWorld* world, RpWorldSectorCallBackRender fpCallBack); // 0x74F730
RpWorldSectorCallBackRender RpWorldGetSectorRenderCallBack(const RpWorld* world); // 0x74F750
RpWorld* RpWorldSectorGetWorld(const RpWorldSector* sector); // 0x74F4E0
RwInt32 RpWorldRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x74FCD0
RwInt32 RpWorldRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x74FD00
RwInt32 RpWorldSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x74FD30
RwInt32 RpWorldSetStreamRightsCallBack(RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB); // 0x74FD50
RwInt32 RpWorldGetPluginOffset(RwUInt32 pluginID); // 0x74FD70
RwBool RpWorldValidatePlugins(RpWorld* world); // 0x74FD90
void RpD3D9GeometrySetUsageFlags(RpGeometry* geometry, RpD3D9GeometryUsageFlag flags); // 0x7588B0
RpD3D9GeometryUsageFlag RpD3D9GeometryGetUsageFlags(const RpGeometry* geometry); // 0x7588D0
void RpD3D9WorldSectorSetUsageFlags(RpWorldSector* sector, RpD3D9WorldSectorUsageFlag flags); // 0x7588E0
RpD3D9WorldSectorUsageFlag RpD3D9WorldSectorGetUsageFlags(const RpWorldSector* sector); // 0x758900
void RxD3D9AllInOneSetInstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneInstanceCallBack callback); // 0x757380
RxD3D9AllInOneInstanceCallBack RxD3D9AllInOneGetInstanceCallBack(RxPipelineNode* node); // 0x757390
void RxD3D9AllInOneSetReinstanceCallBack(RxPipelineNode* node, RxD3D9AllInOneReinstanceCallBack callback); // 0x7573A0
RxD3D9AllInOneReinstanceCallBack RxD3D9AllInOneGetReinstanceCallBack(RxPipelineNode* node); // 0x7573B0
void RxD3D9AllInOneSetLightingCallBack(RxPipelineNode* node, RxD3D9AllInOneLightingCallBack callback); // 0x7573C0
RxD3D9AllInOneLightingCallBack RxD3D9AllInOneGetLightingCallBack(RxPipelineNode* node); // 0x7573D0
void RxD3D9AllInOneSetRenderCallBack(RxPipelineNode* node, RxD3D9AllInOneRenderCallBack callback); // 0x7573E0
RxD3D9AllInOneRenderCallBack RxD3D9AllInOneGetRenderCallBack(RxPipelineNode* node); // 0x7573F0
void _rpD3D9AddDynamicGeometry(RpGeometry* geometry); // 0x74E970
void _rpD3D9RemoveDynamicGeometry(RpGeometry* geometry); // 0x74E9E0
RwUInt32 _rpD3D9VertexDeclarationGetSize(RwUInt32 type); // 0x7522E0
RwUInt32 _rpD3D9VertexDeclarationGetStride(const void* vertexDeclaration); // 0x7522F0
RwUInt32 _rpD3D9VertexDeclarationInstV3d(RwUInt32 type, RwUInt8* mem, const RwV3d* src, RwInt32 numVerts, RwUInt32 stride); // 0x752AD0
RwUInt32 _rpD3D9VertexDeclarationInstV3dComp(RwUInt32 type, RwUInt8* mem, const RpVertexNormal* src, RwInt32 numVerts, RwUInt32 stride); // 0x7531B0
RwUInt32 _rpD3D9VertexDeclarationInstV3dMorph(RwUInt32 type, RwUInt8* mem, const RwV3d* src1, const RwV3d* src2, RwReal scale, RwInt32 numVerts, RwUInt32 stride); // 0x753B60
RwUInt32 _rpD3D9VertexDeclarationInstV2d(RwUInt32 type, RwUInt8* mem, const RwV2d* src, RwInt32 numVerts, RwUInt32 stride); // 0x7544E0
RwBool _rpD3D9VertexDeclarationInstColor(RwUInt8* mem, const RwRGBA* color, RwInt32 numVerts, RwUInt32 stride); // 0x754AE0
RwUInt32 _rpD3D9VertexDeclarationInstIndices(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, RwInt32 numVerts, RwUInt32 stride); // 0x754B40
RwUInt32 _rpD3D9VertexDeclarationInstIndicesRemap(RwUInt32 type, RwUInt8* mem, const RwUInt32* indices, const RwUInt8* remap, RwInt32 numVerts, RwUInt32 stride); // 0x754C80
RwUInt32 _rpD3D9VertexDeclarationInstWeights(RwUInt32 type, RwUInt8* mem, const RwV4d* src, RwInt32 numVerts, RwUInt32 stride); // 0x752320
RwUInt32 _rpD3D9VertexDeclarationInstTangent(RwUInt32 type, RwUInt8* mem, const RwV3d* pos, const RwTexCoords* texCoord, RxD3D9ResEntryHeader* meshHeader, RwUInt32 stride); // 0x754E20
void _rpD3D9VertexDeclarationUnInstV3d(RwUInt32 type, RwV3d* dst, const RwUInt8* src); // 0x7551F0
void _rpD3D9VertexDeclarationUnInstV2d(RwUInt32 type, RwV2d* dst, const RwUInt8* src); // 0x7555E0
void _rpD3D9GetMinMaxValuesV3d(const RwV3d* src, RwInt32 numElements, RwV3d* min, RwV3d* max); // 0x755830
void _rpD3D9GetMinMaxValuesV2d(const RwV2d* src, RwInt32 numElements, RwV2d* min, RwV2d* max); // 0x7558F0
RwUInt32 _rpD3D9FindFormatV3d(const RwV3d* src, RwInt32 numElements); // 0x755980
RwUInt32 _rpD3D9FindFormatV2d(const RwV2d* src, RwInt32 numElements); // 0x755AA0
RxPipeline* RpWorldSetDefaultSectorPipeline(RxPipeline* pipeline); // 0x75E270
RxPipeline* RpAtomicSetDefaultPipeline(RxPipeline* pipeline); // 0x75E2A0
RxPipeline* RpMaterialSetDefaultPipeline(RxPipeline* pipeline); // 0x75E2D0
const RpGeometry* RpGeometryIsCorrectlySorted(const RpGeometry* geometry, RwBool* result); // 0x75D9D0
RpGeometry* RpGeometrySortByMaterial(const RpGeometry* geometry, RpGeometrySortByMaterialCallBack callback); // 0x75DAE0
void RpTieSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x74FF60
void RpLightTieSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x74FF80
RpWorld* RpWorldRemoveCamera(RpWorld* world, RwCamera* camera); // 0x750F50
RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera); // 0x750F20
RpWorld* RwCameraGetWorld(const RwCamera* camera); // 0x750F80
RpWorld* RpWorldRemoveAtomic(RpWorld* world, RpAtomic* atomic); // 0x750FC0
RpWorld* RpWorldAddAtomic(RpWorld* world, RpAtomic* atomic); // 0x750F90
RpWorld* RpAtomicGetWorld(const RpAtomic* atomic); // 0x751050
RpWorld* RpWorldAddClump(RpWorld* world, RpClump* clump); // 0x751300
RpWorld* RpWorldRemoveClump(RpWorld* world, RpClump* clump); // 0x751460
RpWorld* RpClumpGetWorld(const RpClump* clump); // 0x7516B0
RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light); // 0x751910
RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light); // 0x751960
RpWorld* RpLightGetWorld(const RpLight* light); // 0x7519E0
RwCamera* RwCameraForAllClumpsInFrustum(RwCamera* camera, void* data); // 0x7516C0
RwCamera* RwCameraForAllAtomicsInFrustum(RwCamera* camera, RpAtomicCallBack callback, void* data); // 0x7517F0
RwCamera* RwCameraForAllSectorsInFrustum(RwCamera* camera, RpWorldSectorCallBack callBack, void* data); // 0x751660
RpLight* RpLightForAllWorldSectors(RpLight* light, RpWorldSectorCallBack callback, void* data); // 0x7519F0
RpAtomic* RpAtomicForAllWorldSectors(RpAtomic* atomic, RpWorldSectorCallBack callback, void* data); // 0x751060
RpWorldSector* RpWorldSectorForAllAtomics(RpWorldSector* sector, RpAtomicCallBack callback, void* data); // 0x7510A0
RpWorldSector* RpWorldSectorForAllCollisionAtomics(RpWorldSector* sector, RpAtomicCallBack callback, void* data); // 0x751140
RpWorldSector* RpWorldSectorForAllLights(RpWorldSector* sector, RpLightCallBack callback, void* data); // 0x7511E0
RwUInt32 RpWorldStreamGetSize(const RpWorld* world); // 0x761EA0
RpWorld* RpWorldStreamRead(RwStream* stream); // 0x762960
const RpWorld* RpWorldStreamWrite(const RpWorld* world, RwStream* stream); // 0x762150
RpWorldSectorChunkInfo* _rpWorldSectorChunkInfoRead(RwStream* stream, RpWorldSectorChunkInfo* worldSectorChunkInfo, RwInt32* bytesRead); // 0x7635B0
RpPlaneSectorChunkInfo* _rpPlaneSectorChunkInfoRead(RwStream* stream, RpPlaneSectorChunkInfo* planeSectorChunkInfo, RwInt32* bytesRead); // 0x763620
RpWorldChunkInfo* _rpWorldChunkInfoRead(RwStream* stream, RpWorldChunkInfo* worldChunkInfo, RwInt32* bytesRead); // 0x763690

/* Usefull functions */
void
_rwD3D9EnableClippingIfNeeded(void *object,
    RwUInt32 type);


/****************************************************************************
macro/inline functionality
*/

#define RpWorldSectorGetBBoxMacro(_sector)              \
    (&((_sector)->boundingBox))

#define RpWorldSectorGetTightBBoxMacro(_sector)         \
    (&((_sector)->tightBoundingBox))

#define RpWorldSectorGetNumVerticesMacro(_sector)       \
    ((_sector)->numVertices)

#define RpWorldSectorGetNumTrianglesMacro(_sector)      \
    ((_sector)->numTriangles)

#define RpWorldSectorGetTrianglesMacro(_sector)         \
    ((_sector)->triangles)

#define RpWorldSectorGetVerticesMacro(_sector)          \
    ((_sector)->vertices)

#if ((!defined(RWDEBUG)) && (!defined(RWSUPPRESSINLINE)))

#define RpWorldSectorGetBBox(_sector)                   \
    RpWorldSectorGetBBoxMacro(_sector)

#define RpWorldSectorGetTightBBox(_sector)              \
    RpWorldSectorGetTightBBoxMacro(_sector)

#define RpWorldSectorGetNumTriangles(_sector)           \
    RpWorldSectorGetNumTrianglesMacro(_sector)

#define RpWorldSectorGetNumVertices(_sector)            \
    RpWorldSectorGetNumVerticesMacro(_sector)

#define RpWorldSectorGetTriangles(_sector)              \
    RpWorldSectorGetTrianglesMacro(_sector)

#define RpWorldSectorGetVertices(_sector)               \
    RpWorldSectorGetVerticesMacro(_sector)

#endif /* ((!defined(RWDEBUG)) && (!defined(RWSUPPRESSINLINE))) */
