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
 * Filename: <C:/daily/rwsdk/include/d3d9/rwcore.h>
 * Automatically Generated on: Fri Oct 03 09:52:43 2003
 *
 ************************************************************************/

/*--- System Header Files ---*/
#include "rwplcore.h"


/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2resort.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2macros.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2heap.h ---*/

#if (defined(RWDEBUG) && (defined(RWMEMDEBUG)))

#if (!defined(DISABLERWHEAP))
#define DISABLERWHEAP
#endif /* (!defined(DISABLERWHEAP)) */

#endif /* (defined(RWDEBUG) && (defined(RWMEMDEBUG))) */

typedef struct rxHeapFreeBlock rxHeapFreeBlock;
typedef struct rxHeapSuperBlockDescriptor rxHeapSuperBlockDescriptor;
typedef struct RxHeap RxHeap;
typedef struct rxHeapBlockHeader rxHeapBlockHeader;

#if (!defined(DOXYGEN))
struct rxHeapFreeBlock
{
    RwUInt32            size;
    rxHeapBlockHeader  *ptr;
};

struct rxHeapSuperBlockDescriptor
{
    void *start;
    RwUInt32 size;
    rxHeapSuperBlockDescriptor *next;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rwcoregeneric
 * \struct RxHeap 
 * structure describing a pipeline execution heap 
 */
struct RxHeap
{
    RwUInt32            superBlockSize; /**< Granularity of heap growth */
    rxHeapSuperBlockDescriptor *head;   /**< Internally used superblock pointer */
    rxHeapBlockHeader  *headBlock;      /**< Internally used block pointer */
    rxHeapFreeBlock    *freeBlocks;     /**< Internally used free blocks pointer */
    RwUInt32            entriesAlloced; /**< Number of entries allocated */
    RwUInt32            entriesUsed;    /**< Number of entries used */
    RwBool              dirty;          /**< Internally used boolean, flags whether
                                         *   the heap needs resetting or not. */
};

#if (!defined(DOXYGEN))
struct rxHeapBlockHeader
{
    /* present in all blocks (used & unused) */
    rxHeapBlockHeader  *prev, *next;
    RwUInt32            size;
    rxHeapFreeBlock    *freeEntry; /* (or null) */
    RwUInt32            pad[4]; /* alignment padding to 32 bytes */
};
#endif /* (!defined(DOXYGEN)) */

#if (defined(DISABLERWHEAP))

typedef struct rxHeapMallocTrace rxHeapMallocTrace;
struct rxHeapMallocTrace
{
    rxHeapMallocTrace *next;
    rxHeapBlockHeader *block;
};

#endif                          /* (defined(DISABLERWHEAP)) */


/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2dep.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2core.h ---*/

/* Beneficial padding of PowerPipe types is still being worked out... */
#define PADCLUSTERSx


/*************************************************************
 * Global Defines
 */

#define RWBOOLTOGGLE(bVar) ((bVar == FALSE)?(bVar = TRUE):(bVar = FALSE))

/* Both these currently limited due to the use of RwUInt32 bit-fields */
#define RXNODEMAXCLUSTERSOFINTEREST 32
#define RXNODEMAXOUTPUTS            32

/* Useful (for memory alloc) to limit this during pipeline construction */
#define RXPIPELINEDEFAULTMAXNODES   64

/*
 * Cluster flags
 */

#define rxCLFLAGS_NULL          ((RwUInt16) 0x0000U)
#define rxCLFLAGS_CLUSTERVALID  ((RwUInt16) 0x0001U)
#define rxCLFLAGS_EXTERNAL ((RwUInt16) 0x0002U)
#define rxCLFLAGS_EXTERNALMODIFIABLE ((RwUInt16) 0x0004U | 0x0002U)
#define rxCLFLAGS_MODIFIED ((RwUInt16) 0x0008U)

/*
 * Packet flags
 */

#define rxPKFLAGS_NULL        ((RwUInt16) 0x0000U)

/*
 * used in input specification
 */

/**
 * \ingroup rwcoregeneric
 * \ref RxClusterValidityReq
 * Flags specifying the state requirements for
 * a \ref RxCluster on entry to a node */
enum RxClusterValidityReq
{
    rxCLREQ_DONTWANT = 0, /**<The cluster is required but any data within it is
                           * not wanted and will be overwritten */
    rxCLREQ_REQUIRED = 1, /**<The cluster is required and it must contain
                           * valid data */
    rxCLREQ_OPTIONAL = 2, /**<The cluster will be used if it is present and
                           * contains valid data, otherwise the node will
                           * make do without it. */
    rxCLUSTERVALIDITYREQFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};


/**
 * \ingroup rwcoregeneric
 * \ref RxClusterValid
 * Flags specifying the state requirements for
 * a \ref RxCluster on exit from a node */
enum RxClusterValid
{
    rxCLVALID_NOCHANGE = 0, /**<The cluster and its data will not change in
                             * validity on passing through this node */
    rxCLVALID_VALID = 1,    /**<The cluster and its data will be valid on
                             * exit from this node */
    rxCLVALID_INVALID = 2,  /**<The cluster's data will be invalid on
                             * exit from this node */
    rxCLUSTERVALIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/****************************************************************************
 Global Types
 */

typedef struct RxClusterDefinition RxClusterDefinition;

typedef struct rxReq rxReq;

typedef enum RxClusterValidityReq RxClusterValidityReq;
typedef enum RxClusterValid RxClusterValid;
typedef struct RxOutputSpec RxOutputSpec;
typedef struct RxClusterRef RxClusterRef;
typedef struct RxIoSpec RxIoSpec;

typedef struct RxNodeMethods RxNodeMethods;
typedef struct RxNodeDefinition RxNodeDefinition;

typedef struct RxCluster RxCluster;
typedef struct RxPipelineCluster RxPipelineCluster;
typedef struct RxPacket RxPacket;
typedef struct RxPipelineNode RxPipelineNode;
typedef struct RxPipelineNodeTopSortData RxPipelineNodeTopSortData;
typedef struct RxPipelineNode RxPipelineNodeInstance;
typedef struct RxPipelineNodeParam RxPipelineNodeParam;
typedef struct RxExecutionContext RxExecutionContext;
typedef struct RxPipelineRequiresCluster RxPipelineRequiresCluster;
typedef struct RxPipeline RxPipeline;

/***************************************************************************
 *
 * C L U S T E R   D E F I N I T I O N
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxClusterDefinition
 * Structure describing a cluster */
struct RxClusterDefinition
{
    RwChar             *name;                  /**< Name */
    RwUInt32            defaultStride;         /**< Default stride */
    RwUInt32            defaultAttributes;     /**< Default attributes */
    const RwChar         *attributeSet;           /**< Attribute set */
};


/***************************************************************************
 *
 * N O D E   D E F I N I T I O N   S T R U C T S
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxOutputSpec
 * Structure describing an output specification of a node */
struct RxOutputSpec
{
     RwChar             *name;                  /**< Name */
     RxClusterValid     *outputClusters;        /**< States of clusters of interest on output */
     RxClusterValid      allOtherClusters;      /**< States of clusters not of interest on output */
};



/**
 * \ingroup rwcoregeneric
 * \ref RxClusterForcePresent
 *  Flags specifying whether an \ref RxCluster
 * should be forced to be present in an \ref RxPipelineNode */
enum RxClusterForcePresent
{
    rxCLALLOWABSENT = FALSE, /**<The cluster is allowed to be absent if no prior
                              *  nodes create it and no subsequent nodes require it */
    rxCLFORCEPRESENT = TRUE, /**<The cluster must be present even if no prior
                              *  nodes create it and no subsequent nodes require it */

    rxCLUSTERFORCEPRESENTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxClusterForcePresent RxClusterForcePresent;

/**
 * \ingroup rwcoregeneric
 * \struct RxClusterRef
 * Structure describing a cluster reference */
struct RxClusterRef
{
    RxClusterDefinition  *clusterDef;           /**< Cluster definition */
    RxClusterForcePresent forcePresent;         /**< Specifies whether the cluster should be forced present */
    RwUInt32              reserved;             /**< Omit or initialize to zero */
};

#define rxCLRESERVED       ((RwUInt32)0)

/**
 * \ingroup rwcoregeneric
 * \struct RxIoSpec
 * Structure describing an input/output specification of a node */
struct RxIoSpec
{
    RwUInt32              numClustersOfInterest;/**< Number of Clusters of interest */
    RxClusterRef         *clustersOfInterest;   /**< Clusters of interest array */
    RxClusterValidityReq *inputRequirements;    /**< Requirements of the clusters of interest on input to the node */
    RwUInt32              numOutputs;           /**< Number of outputs from the node */
    RxOutputSpec         *outputs;              /**< Output specification array */
};

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeBodyFn is the callback to be
 * called during pipeline execution -- and, typically, process
 * \ref RxPacket's -- for the owning pipeline node.
 *
 * \param  self   A pointer to the pipeline node being executed
 * \param  params   A pointer to a parameter structure
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxNodeBodyFn) (RxPipelineNode * self,
                                const RxPipelineNodeParam *params);

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeInitFn
 * is the callback to be called,
 * for the owning node definition, the first time an \ref RxPipeline
 * referencing that node definition is unlocked.
 *
 * \param  self   A pointer to the node definition
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxNodeInitFn) (RxNodeDefinition * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeTermFn
 * is the callback to be called,
 * for the owning node definition, the last time an \ref RxPipeline
 * referencing that node definition is destroyed or locked.
 *
 * \param  self   A pointer to the node definition
 *
 * \return None
 *
 * \see RxNodeMethods
 */
typedef void   (*RxNodeTermFn) (RxNodeDefinition * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeInitFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is unlocked.
 *
 * \param  self   A pointer to the pipeline node
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxPipelineNodeInitFn) (RxPipelineNode * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeTermFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is locked or
 * destroyed.
 *
 * \param  self   A pointer to the pipeline node
 *
 * \return None
 *
 * \see RxNodeMethods
 */
typedef void   (*RxPipelineNodeTermFn) (RxPipelineNode * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeConfigFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is unlocked,
 * *after* all \ref RxPipelineNodeInitFn's have been called for the
 * pipeline in question. This func is to be used as described in
 * RxPipelineNodeSendConfigMsg.
 *
 * \param  self       A pointer to the pipeline node
 * \param  pipeline   A pointer to the containing pipeline
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxPipelineNodeConfigFn) (RxPipelineNode * self,
                                          RxPipeline * pipeline);

/**
 * \ingroup rwcoregeneric
 * \ref RxConfigMsgHandlerFn
 * is the callback to be called, for the owning pipeline node, whenever
 * a message is sent to it by the \ref RxPipelineNodeConfigFn of another
 * pipeline node in the same pipeline. See \ref RxPipelineNodeSendConfigMsg.
 * 
 * \param  self   A pointer to the pipeline node
 * \param  msg   Message ID
 * \param  intparam   Meaning is message-specific
 * \param  ptrparam   Meaning is message-specific
 *
 * \return A RwInt32 value, 0: unserviced; -ve: error; +ve: informative success
 * 
 * \see RxNodeMethods
 */
typedef RwUInt32 (*RxConfigMsgHandlerFn) (RxPipelineNode * self,
                                          RwUInt32 msg,
                                          RwUInt32 intparam,
                                          void *ptrparam);

/**
 * \ingroup rwcoregeneric
 * \struct RxNodeMethods
 * A structure describing a set
 * of node methods
 *
 * \see RxNodeBodyFn
 * \see RxNodeInitFn
 * \see RxNodeTermFn
 * \see RxPipelineNodeInitFn
 * \see RxPipelineNodeTermFn
 * \see RxPipelineNodeConfigFn
 * \see RxConfigMsgHandlerFn
 */
struct RxNodeMethods
{
    RxNodeBodyFn         nodeBody;              /**< Node body function */
    RxNodeInitFn         nodeInit;              /**< Node initialization function */
    RxNodeTermFn         nodeTerm;              /**< Node termination function */
    RxPipelineNodeInitFn pipelineNodeInit;      /**< Pipeline node initialization function */
    RxPipelineNodeTermFn pipelineNodeTerm;      /**< Pipeline node termination function */
    RxPipelineNodeConfigFn pipelineNodeConfig;  /**< Pipleline node configuation function */
    RxConfigMsgHandlerFn configMsgHandler;      /**< Configuaraton message handler function */
};

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeDefEditable
 * Flags specifying whether an \ref RxNodeDefinition
 * is editable or not (\ref RxPipelineNodeCloneDefinition,
 * \ref RxPipelineNodeReplaceCluster and \ref RxPipelineNodeRequestCluster
 * create editable copies of node definitions as the originals may be
 * static definitions). */
enum RxNodeDefEditable
{
    /**<This node definition should not be freed */
    rxNODEDEFCONST = FALSE,
    /**< This node definition is a temporary,
     * modified copy of another and can be freed */
    rxNODEDEFEDITABLE = TRUE,
    rxNODEDEFEDITABLEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxNodeDefEditable RxNodeDefEditable;

/**
 * \ingroup rwcoregeneric
 * \struct RxNodeDefinition
 * A structure describing a node definition */
struct RxNodeDefinition
{
    RwChar             *name;                   /**< Name */
    RxNodeMethods       nodeMethods;            /**< Node methods */
    RxIoSpec            io;                     /**< Input/output specification */
    RwUInt32            pipelineNodePrivateDataSize; /**< Size in bytes of node's private data  */
    RxNodeDefEditable   editable;               /**< Flags whether a node is editable */
    RwInt32             InputPipesCnt;          /**< Count of the unlocked pipelines containing this node */
};

/***************************************************************************
 *
 * P I P E L I N E - E X E C U T I O N - T I M E   S T R U C T S
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineCluster
 * A structure describing a pipeline cluster;
 * that is, an \ref RxClusterDefinition in the context of a specific \ref RxPipeline
 * (in the same way as an \ref RxPipelineNode is an \ref RxNodeDefinition in the
 * context of a specific \ref RxPipeline). The \ref RxCluster is the structure
 * representing this and the \ref RxClusterDefinition within \ref RxPacket's at
 * pipeline-execution-time */
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;            /**< Cluster refererence */
    RwUInt32             creationAttributes;    /**< Creation Attributes */
};

/**
 * \ingroup rwcoregeneric
 * \struct RxCluster
 * A structure describing a cluster; this is
 * the representative of an \ref RxClusterDefinition and \ref RxPipelineCluster
 * within \ref RxPacket's at pipeline-execution-time. */
struct RxCluster
{
    RwUInt16            flags;                  /**< Bitfield of flags e.g. modification permissions */
    RwUInt16            stride;                 /**< Stride in bytes of the cluster's data */
    void               *data;                   /**< The Cluster's data */
    void               *currentData;            /**< A 'cursor', referencing the current element in the data */
    RwUInt32            numAlloced;             /**< Allocated count */
    RwUInt32            numUsed;                /**< Used count */
    RxPipelineCluster  *clusterRef;             /**< Valid after a cluster has been locked for writing,
                                                 *   otherwise NULL. Analog of \ref RxPipelineNode and
                                                 *   its 'nodeDef' member. */
    RwUInt32            attributes;             /**< Attributes */
#ifdef PADCLUSTERS
    RwUInt32 pad[1];                            /**< Alignment padding */
#endif
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPacket
 * A structure describing a packet header */
struct RxPacket
{
    RwUInt16            flags;                  /**< Flags to guide pipeline execution */
    RwUInt16            numClusters;            /**< Maximum number of clusters simultanesouly present in the current pipeline */
    RxPipeline         *pipeline;               /**< The pipeline in which this packet is embedded (the current pipeline) */
    RwUInt32           *inputToClusterSlot;     /**< LUT to locate clusters of interest in the packet */
    RwUInt32           *slotsContinue;          /**< Bitfields specifying clusters persisting to output node */
    RxPipelineCluster **slotClusterRefs;        /**< Internal Use */
#ifdef PADCLUSTERS
    RwUInt32 pad[3];                            /**< Alignment padding */
#endif
    RxCluster           clusters[1];            /**< Cluster array large enough for widest part of the pipeline */
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNode
 * Structure describing a pipeline Node;
 * that is an \ref RxNodeDefinition in the context 
 * of a specific \ref RxPipeline. 
 */
struct RxPipelineNode
{
    RxNodeDefinition   *nodeDef;            /**< Node definition reference */
    RwUInt32            numOutputs;         /**< Output count */
    RwUInt32           *outputs;            /**< Output array, indexing the pipeline's array of pipeline nodes */
    RxPipelineCluster **slotClusterRefs;    /**< For packets passing through this node, SlotClusterRefs[n]
                                             * identifies the cluster to be found at index n in the packet's
                                             * Clusters[] array. These assignments are determined at pipeline
                                             * Unlock() time and are good for the life of the pipeline. */
    RwUInt32           *slotsContinue;      /**< Cluster persistence table for the node's outputs */
    void               *privateData;        /**< Pointer to the pipeline Node's private data */
    RwUInt32           *inputToClusterSlot; /**< LUT to locate clusters of interest in packets passing through this pipeline Node */
    RxPipelineNodeTopSortData *topSortData; /**< Internal Use */
    void               *initializationData; /**< Used by the pipeline node initialisation
                                             * function in setting up the Private data. */
    RwUInt32        initializationDataSize; /**< Present so that if the node is cloned
                                             * we can copy the initialisation data. */
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNodeTopSortData
 * Structure describing data used during topological sorting 
 * during \ref RxLockedPipeUnlock ; 
 * RenderWare users do not need to understand this */
struct RxPipelineNodeTopSortData
{
    RwUInt32            numIns;                 /**< Input count */
    RwUInt32            numInsVisited;          /**< Count of inputs visited during dependencies
                                                 * propagation/cluster slot allocation */
    rxReq              *req;                    /**< Req used in dependencies propagation/cluster
                                                 * slot allocation */
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNodeParam
 * Structure holding parameters
 * to pass to node body functions */
struct RxPipelineNodeParam
{
    void   *dataParam;   /**< The data pointer passed in to \ref RxPipelineExecute */
    RxHeap *heap;        /**< The heap associated with the current pipeline exeuction */
};

enum rxEmbeddedPacketState
{
    rxPKST_PACKETLESS = 0, /* Packet not created */
    rxPKST_UNUSED     = 1, /* Packet created and then destroyed */
    rxPKST_INUSE      = 2, /* Packet created but not yet destroyed and fetched but not yet dispatched */
    rxPKST_PENDING    = 3, /* Packet created but not destroyed and dispatched but not yet fetched */
    rxEMBEDDEDPACKETSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rxEmbeddedPacketState rxEmbeddedPacketState;

/**
 * \ingroup rwcoregeneric
 * \struct RxExecutionContext
 * Structure describing an execution context */
struct RxExecutionContext
{
    RxPipeline           *pipeline;     /**< Currently executing pipeline */
    RxPipelineNode       *currentNode;  /**< Currently executing node */
    RwInt32               exitCode;     /**< Used internally for nodes returning error codes on exit */
    RwUInt32              pad;          /**< Alignment padding */
    RxPipelineNodeParam   params;       /**< The parameters passed to node bodies */
    /*RwUInt32              pad[2];*/
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineRequiresCluster
 * Structure describing a pipeline requirement of a cluster */
struct RxPipelineRequiresCluster
{
    RxClusterDefinition *clusterDef;    /**< Reference to a Cluster definition */
    RxClusterValidityReq rqdOrOpt;      /**< Cluster validity requirement (rxCLREQ_DONTWANT, rxCLREQ_REQUIRED or rxCLREQ_OPTIONAL) */
    RwUInt32             slotIndex;     /**< Index into the packet's cluster array within this pipeline */
};

/**
 * \ingroup rwcoregeneric
 * \struct RxPipeline
 * Structure describing a pipeline */
struct RxPipeline
{
    RwBool                     locked;                /**< Flags whether the pipeline is locked for editing */
    RwUInt32                   numNodes;              /**< Node count */
    RxPipelineNode            *nodes;                 /**< Topologically sorted array of pipeline nodes */
    RwUInt32                   packetNumClusterSlots; /**< Number of slots allocated for clusters */
    rxEmbeddedPacketState      embeddedPacketState;   /**< The state of this pipeline's embedded packet */
    RxPacket                  *embeddedPacket;        /**< This pipeline's embedded packet */

    RwUInt32                   numInputRequirements;  /**< Input requirements count */
    RxPipelineRequiresCluster *inputRequirements;     /**< Input requirements array */

    void                      *superBlock;            /**< Internally used block of memory */
    RwUInt32                   superBlockSize;        /**< Internally used block of memory */

    RwUInt32                   entryPoint;            /**< The index of the node which is the entry point of this pipeline */
    RwUInt32                   pluginId;              /**< If required, the Id of the plugin owning this pipeline, or 0 */
    RwUInt32                   pluginData;            /**< Rights callback extra data */
};

/****************************************************************************
 * Global Prototypes
 */

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeOutputCallBack 
 * is the callback function supplied 
 * to \ref RxPipelineNodeForAllConnectedOutputs.
 *
 * The callback will be passed a pointer to the \ref RxPipelineNode whose
 * outputs are being traversed and a pointer to the current output
 * \ref RxPipelineNode, as well as a pointer to an optional user-defined
 * data structure (callbackdata). If no such structure was specified, this
 * will be NULL.
 *
 * \param  node         A pointer to the pipeline node whose outputs
 *                      are being traversed
 * \param  outputnode   A pointer to the current output
 *                      pipeline node
 * \param  callbackdata A pointer to optional user-supplied data
 *
 * \return Returns a pointer to the \ref RxPipelineNode whose outputs are being
 * traversed, or NULL to terminate traversal
 */
typedef RxPipelineNode * (*RxPipelineNodeOutputCallBack) (RxPipelineNode * node,
                                                  RxPipelineNode * outputnode,
                                                  void *callbackdata);

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/d3d9/nodeD3D9SubmitNoLight.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2define.h ---*/

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeOutput 
 * typedef for a reference to an output of a pipeline node */
typedef RwUInt32       *RxNodeOutput;

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeInput 
 *  typedef for a reference to the input of a pipeline node */
typedef RxPipelineNode *RxNodeInput;

/**
 * \ingroup rwcoregeneric
 * \ref RxLockedPipe
 * typedef for a reference to a locked pipeline 
 */
typedef RxPipeline      RxLockedPipe;

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d9rendst.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d9raster.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d9convrt.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d92drend.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d9device.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/common/ssematml.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/common/cpuext.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/baraster.h ---*/

/****************************************************************************
 Defines
 */

/**
 * \ingroup rwraster
 * \ref RwRasterLockMode represents the options available for locking 
 * a raster so that it may be modified (see API function \ref RwRasterLock). An 
 * application may wish to write to the raster, read from the raster or
 * simultaneously write and read a raster (rwRASTERLOCKWRITE | rwRASTERLOCKREAD).
 */
enum RwRasterLockMode
{
    rwRASTERLOCKWRITE = 0x01,   /**<Lock for writing */
    rwRASTERLOCKREAD = 0x02,    /**<Lock for reading */
    rwRASTERLOCKNOFETCH = 0x04, /**<When used in combination with
                                 *  rwRASTERLOCKWRITE, asks the driver not to
                                 *  fetch the pixel data. This is only useful
                                 *  if it is known that ALL the raster data is
                                 *  going to be overwritten before the raster
                                 *  is unlocked, i.e. from an 
                                 *  \ref RwRasterSetFromImage call. This flag
                                 *  is not supported by all drivers. */
    rwRASTERLOCKRAW = 0x08,    /**<When used in combination with
                                   rwRASTERLOCKWRITE or rwRASTERLOCKREAD
                                   allows access to the raw platform specific
                                   pixel format */
    rwRASTERLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

#define rwRASTERLOCKREADWRITE   (rwRASTERLOCKREAD|rwRASTERLOCKWRITE)

typedef enum RwRasterLockMode RwRasterLockMode;

/**
 * \ingroup rwraster
 *  \ref RwRasterFlipMode represents
 *  raster flip modes */
enum RwRasterFlipMode
{
    rwRASTERFLIPDONTWAIT = 0,   /**<Don't wait for VSync */
    rwRASTERFLIPWAITVSYNC = 1,  /**<Flip on VSync */
    rwRASTERFLIPMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFlipMode RwRasterFlipMode;

/**
 * \ingroup rwraster
 *  RwRasterType 
 *  This type represents the options available for creating a new 
 * raster (se API function \ref RwRasterCreate)*/
enum RwRasterType
{
    rwRASTERTYPENORMAL = 0x00,          /**<Normal */
    rwRASTERTYPEZBUFFER = 0x01,         /**<Z Buffer */
    rwRASTERTYPECAMERA = 0x02,          /**<Camera */
    rwRASTERTYPETEXTURE = 0x04,         /**<Texture */
    rwRASTERTYPECAMERATEXTURE = 0x05,   /**<Camera texture */
    rwRASTERTYPEMASK = 0x07,            /**<Mask for finding type */

    rwRASTERPALETTEVOLATILE = 0x40,        /**<If set, hints that the palette will change often */
    rwRASTERDONTALLOCATE = 0x80,        /**<If set the raster is not allocated */
    rwRASTERTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterType RwRasterType;

/**
 * \ingroup rwraster
 * \ref RwRasterFormat is a set of values and flags which may be combined to
 * specify a raster format. The format chosen for a particular raster depends
 * on the hardware device and the raster type specified at creation time
 * (see API function \ref RwRasterCreate). The format may be retrieved using
 * API function \ref RwRasterGetFormat.
 *
 * The raster format is a packed set of bits which contains the following
 * four pieces of information (these may be combined with bitwise OR):
 *
 * <ol>
 * <li> The pixel color format corresponding to one of the following values:
 *      <ul>
 *      <li> rwRASTERFORMAT1555
 *      <li> rwRASTERFORMAT565
 *      <li> rwRASTERFORMAT4444
 *      <li> rwRASTERFORMATLUM8
 *      <li> rwRASTERFORMAT8888
 *      <li> rwRASTERFORMAT888
 *      <li> rwRASTERFORMAT16
 *      <li> rwRASTERFORMAT24
 *      <li> rwRASTERFORMAT32
 *      <li> rwRASTERFORMAT555
 *      </ul>
 *      This value may be masked out of the raster format using
 *      rwRASTERFORMATPIXELFORMATMASK.
 * <li> The palette depth if the raster is palettized:
 *      <ul> 
 *      <li> rwRASTERFORMATPAL4
 *      <li> rwRASTERFORMATPAL8
 *      </ul>
 *      In these cases, the color format refers to that of the palette.
 * <li> Flag rwRASTERFORMATMIPMAP. Set if the raster contains mipmap levels.
 * <li> Flag rwRASTERFORMATAUTOMIPMAP. Set if the mipmap levels were generated
 *      automatically by RenderWare.
 * </ol>
 */
enum RwRasterFormat
{
    rwRASTERFORMATDEFAULT = 0x0000, /* Whatever the hardware likes best */

    rwRASTERFORMAT1555 = 0x0100,    /**<16 bits - 1 bit alpha, 5 bits red, green and blue */
    rwRASTERFORMAT565 = 0x0200,     /**<16 bits - 5 bits red and blue, 6 bits green */
    rwRASTERFORMAT4444 = 0x0300,    /**<16 bits - 4 bits per component */
    rwRASTERFORMATLUM8 = 0x0400,    /**<Gray scale */
    rwRASTERFORMAT8888 = 0x0500,    /**<32 bits - 8 bits per component */
    rwRASTERFORMAT888 = 0x0600,     /**<24 bits - 8 bits per component */
    rwRASTERFORMAT16 = 0x0700,      /**<16 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT24 = 0x0800,      /**<24 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT32 = 0x0900,      /**<32 bits - undefined: useful for things like Z buffers */
    rwRASTERFORMAT555 = 0x0a00,     /**<16 bits - 5 bits red, green and blue */

    rwRASTERFORMATAUTOMIPMAP = 0x1000, /**<RenderWare generated the mip levels */

    rwRASTERFORMATPAL8 = 0x2000,    /**<8 bit palettised */
    rwRASTERFORMATPAL4 = 0x4000,    /**<4 bit palettised */

    rwRASTERFORMATMIPMAP = 0x8000,  /**<Mip mapping on */

    rwRASTERFORMATPIXELFORMATMASK = 0x0f00, /**<The pixel color format 
                                             *  (excluding palettised bits) */
    rwRASTERFORMATMASK = 0xff00     /**<The whole format */ ,
    rwRASTERFORMATFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterFormat RwRasterFormat;

enum RwRasterPrivateFlag
{
    rwRASTERGAMMACORRECTED = 0x01,
    rwRASTERPIXELLOCKEDREAD = 0x02, /* pixels are locked for reading */
    rwRASTERPIXELLOCKEDWRITE = 0x04, /* pixels are locked for writing */
    rwRASTERPALETTELOCKEDREAD = 0x08, /* palette is locked for reading */
    rwRASTERPALETTELOCKEDWRITE = 0x10, /* palette is locked for writing */
    rwRASTERPIXELLOCKEDRAW = 0x20, /* the pixels are in platform specific
                                      format, used in combination with
                                      rwRASTERPIXELLOCKEDREAD &
                                      rwRASTERPIXELLOCKEDWRITE */
    rwRASTERPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRasterPrivateFlag RwRasterPrivateFlag;

#define rwRASTERPIXELLOCKED     (rwRASTERPIXELLOCKEDREAD | rwRASTERPIXELLOCKEDWRITE)
#define rwRASTERPALETTELOCKED   (rwRASTERPALETTELOCKEDREAD | rwRASTERPALETTELOCKEDWRITE)
#define rwRASTERLOCKED          (rwRASTERPIXELLOCKED|rwRASTERPALETTELOCKED)

/* How big is my stack!!! */
#define rwRASTERCONTEXTSTACKSIZE 10

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwraster
 * \struct RwRaster 
 * Raster containing device-dependent pixels. 
 * This should be considered an opaque type.
 * Use the RwRaster API functions to access.
 */
typedef struct RwRaster RwRaster;

#if (!defined(DOXYGEN))
struct RwRaster
{
    RwRaster           *parent; /* Top level raster if a sub raster */
    RwUInt8            *cpPixels; /* Pixel pointer when locked */
    RwUInt8            *palette; /* Raster palette */
    RwInt32             width, height, depth; /* Dimensions of raster */
    RwInt32             stride; /* Lines bytes of raster */
    RwInt16             nOffsetX, nOffsetY; /* Sub raster offset */
    RwUInt8             cType;  /* Type of raster */
    RwUInt8             cFlags; /* Raster flags */
    RwUInt8             privateFlags; /* Raster private flags */
    RwUInt8             cFormat; /* Raster format */

    RwUInt8            *originalPixels;
    RwInt32             originalWidth;
    RwInt32             originalHeight;
    RwInt32             originalStride;
};
#endif /* (!defined(DOXYGEN)) */

/****************************************************************************
 <macro/inline functionality
 */

#define RwRasterGetWidthMacro(_raster) \
    ((_raster)->width)

#define RwRasterGetHeightMacro(_raster) \
    ((_raster)->height)

#define RwRasterGetStrideMacro(_raster) \
    ((_raster)->stride)

#define RwRasterGetDepthMacro(_raster) \
    ((_raster)->depth)

#define RwRasterGetFormatMacro(_raster) \
    ((((_raster)->cFormat) & (rwRASTERFORMATMASK >> 8)) << 8)

#define RwRasterGetTypeMacro(_raster) \
    (((_raster)->cType) & rwRASTERTYPEMASK)

#define RwRasterGetParentMacro(_raster) \
    ((_raster)->parent)


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RwRasterGetWidth(_raster)                   \
    RwRasterGetWidthMacro(_raster)

#define RwRasterGetHeight(_raster)                  \
    RwRasterGetHeightMacro(_raster)

#define RwRasterGetStride(_raster)                  \
    RwRasterGetStrideMacro(_raster)

#define RwRasterGetDepth(_raster)                   \
    RwRasterGetDepthMacro(_raster)

#define RwRasterGetFormat(_raster)                  \
    RwRasterGetFormatMacro(_raster)

#define RwRasterGetType(_raster)                  \
    RwRasterGetTypeMacro(_raster)

#define RwRasterGetParent(_raster)                  \
    RwRasterGetParentMacro(_raster)

#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/****************************************************************************
 Global Defines
 */

#define RXHEAPPLATFORMDEFAULTSIZE  (1 << 12) /* 4k */

/****************************************************************************
 Global Types
 */

/* We use D3D9 formats for the instanced versions, to allow hardware T&L */

/*
 * Typedef for an RxObjSpace3DVertex.
 */
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;

/**
 * \ingroup cored3d9
 * \struct RxObjSpace3DVertex
 * Structure representing object space vertex.
 */
struct RxObjSpace3DVertex
{
    RwV3d       objVertex;        /**< position */
    RwV3d       objNormal;        /**< normal */
    RwUInt32    color;            /**< emissive color*/
    RwReal      u;                /**< u */
    RwReal      v;                /**< v */
};

/* This vertex is non truncatable */
#define RxObjSpace3DVertexNoUVsNoNormalsSize (sizeof(RxObjSpace3DVertex))
#define RxObjSpace3DVertexNoUVsSize          (sizeof(RxObjSpace3DVertex))
#define RxObjSpace3DVertexFullSize           (sizeof(RxObjSpace3DVertex))

/**
 * \ingroup cored3d9
 * \ref RxObjSpace3DLitVertex
 * Typedef for an RxObjSpace3DLitVertex.
 */
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;

/** 
 * \ingroup cored3d9
 * \ref RwIm3DVertex
 * Typedef for an RwIm3DVertex.
 */
typedef RxObjSpace3DLitVertex RwIm3DVertex;

/* LEGACY-SUPPORT macro */
/**
 * \ingroup cored3d9
 * \ref RxScrSpace2DVertex
 * Typedef for an RxScrSpace2DVertex structure
 */
typedef RwIm2DVertex RxScrSpace2DVertex;

/****************************************************************************
 Object-space 3D unlit vertex macros
 */

/* Vertex positions */
#define RxObjSpace3DVertexGetPos(_vert, _pos) \
    (*(_pos) = (_vert)->objVertex)
#define RxObjSpace3DVertexSetPos(_vert, _pos) \
    ((_vert)->objVertex = *(_pos))

/* Pre-lighting colours */
#define RxObjSpace3DVertexGetPreLitColor(_vert, _col) \
MACRO_START \
{ \
    (_col)->alpha = (RwUInt8)((_vert)->color >> 24) & 0xFF; \
    (_col)->red   = (RwUInt8)((_vert)->color >> 16) & 0xFF; \
    (_col)->green = (RwUInt8)((_vert)->color >>  8) & 0xFF; \
    (_col)->blue  = (RwUInt8)((_vert)->color      ) & 0xFF; \
} \
MACRO_STOP

#define RxObjSpace3DVertexSetPreLitColor(_vert, _col) \
    ((_vert)->color = (((RwUInt32)(_col)->alpha) << 24) | \
                              (((RwUInt32)(_col)->red)   << 16) | \
                              (((RwUInt32)(_col)->green) <<  8) | \
                              (((RwUInt32)(_col)->blue)       ))

/* This uses the same slot as color (they are mutually exclusive) */
#define RxObjSpace3DVertexGetColor RxObjSpace3DVertexGetPreLitColor

/* Normals */
#define RxObjSpace3DVertexGetNormal(_vert, _normal)             \
    (*(_normal) = (_vert)->objNormal)
#define RxObjSpace3DVertexSetNormal(_vert, _normal)             \
    ((_vert)->objNormal = *(_normal))

/* Us and Vs */
#define RxObjSpace3DVertexGetU(_vert)                          \
    ((_vert)->u)
#define RxObjSpace3DVertexGetV(_vert)                          \
    ((_vert)->v)
#define RxObjSpace3DVertexSetU(_vert, _imu)                     \
    ((_vert)->u = (_imu))
#define RxObjSpace3DVertexSetV(_vert, _imv)                     \
    ((_vert)->v = (_imv))

/****************************************************************************
 Object-space 3D lit vertex macros
 */

/* Vertex positions */
#define RxObjSpace3DLitVertexGetPos(_vert, _pos) \
    (*(_pos) = (_vert)->objVertex)
#define RxObjSpace3DLitVertexSetPos(_vert, _pos) \
    ((_vert)->objVertex = *(_pos))

/* Vertex colours */
#define RxObjSpace3DLitVertexGetColor(_vert, _col) \
MACRO_START \
{ \
    (_col)->red   = ((_vert)->color >> 16) & 0xFF; \
    (_col)->green = ((_vert)->color >>  8) & 0xFF; \
    (_col)->blue  = ((_vert)->color      ) & 0xFF; \
    (_col)->alpha = ((_vert)->color >> 24) & 0xFF; \
} \
MACRO_STOP
#define RxObjSpace3DLitVertexSetColor(_vert, _col) \
    ((_vert)->color = (((RwUInt32)(_col)->alpha) << 24) | \
                              (((RwUInt32)(_col)->red)   << 16) | \
                              (((RwUInt32)(_col)->green) <<  8) | \
                              (((RwUInt32)(_col)->blue)       ))

/* Us and Vs */
#define RxObjSpace3DLitVertexGetU(_vert) \
    ((_vert)->u)
#define RxObjSpace3DLitVertexGetV(_vert) \
    ((_vert)->v)
#define RxObjSpace3DLitVertexSetU(_vert, _imu) \
    ((_vert)->u = (_imu))
#define RxObjSpace3DLitVertexSetV(_vert, _imv) \
    ((_vert)->v = (_imv))

/* LEGACY-SUPPORT for old objvert names - NB does NOT guarantee the
 * app will work, because the old IM3DVERTEX macros are NOT correctly
 * abstracted - 'Get' will return pointers to RwV3ds inside the
 * ObjVert, but you can't assume there are any RwV3ds inside an
 * opaque vertex type */

#define RwIm3DVertexSetU   RxObjSpace3DLitVertexSetU
#define RwIm3DVertexSetV   RxObjSpace3DLitVertexSetV
#define RwIm3DVertexGetNext(_vert)      ((_vert) + 1)

#define RwIm2DCameraVertexSetU(_devvert, _camvert, _u, _recipz) \
MACRO_START                                                     \
{                                                               \
    RwReal _uTmp = _u;                                          \
    _camvert->u = _uTmp;                                        \
    RwIm2DVertexSetU(_devvert, _uTmp, _recipz);                 \
}                                                               \
MACRO_STOP
#define RwIm2DCameraVertexSetV(_devvert, _camvert, _v, _recipz) \
MACRO_START                                                     \
{                                                               \
    RwReal _vTmp = _v;                                          \
    _camvert->v = _vTmp;                                        \
    RwIm2DVertexSetV(_devvert, _vTmp, _recipz);                 \
}                                                               \
MACRO_STOP

#define RwIm3DVertexSetPos(_vert, _imx, _imy, _imz) \
MACRO_START \
{ \
    (_vert)->objVertex.x = _imx; \
    (_vert)->objVertex.y = _imy; \
    (_vert)->objVertex.z = _imz; \
} \
MACRO_STOP

#define RwIm3DVertexSetNormal(vert, imx, imy, imz)         \
MACRO_START                                                \
{                                                          \
    RwV3d packed;                                          \
    packed.x = imx;                                        \
    packed.y = imy;                                        \
    packed.z = imz;                                        \
    RxObjSpace3DVertexSetNormal(vert, &packed);            \
}                                                          \
MACRO_STOP

#define RwIm3DVertexSetRGBA(_vert, _r, _g, _b, _a) \
MACRO_START \
{ \
    ((_vert)->color = ((_a) << 24) | \
                              ((_r) << 16) | \
                              ((_g) << 8) | \
                              ((_b))); \
} \
MACRO_STOP

#define RwIm3DVertexGetPos(_vert) (&((_vert)->objVertex))
#define RwIm3DVertexGetNormal(vert) (&((vert)->objNormal))

#define RwIm3DVertexCopyRGBA(_dst, _src) (((_dst)->color) = ((_src)->color))


/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2renderstate.h ---*/

/**
 * \ingroup rwcoregeneric
 *  RxRenderStateFlag 
 *  Flags used in the \ref RxRenderStateVector structure */
enum RxRenderStateFlag
{
    rxRENDERSTATEFLAG_TEXTUREPERSPECTIVE   = 0x00000001, /**<Perspective texturing is to be enabled */
    rxRENDERSTATEFLAG_ZTESTENABLE          = 0x00000002, /**<Z-Buffer testing is to be performed */
    rxRENDERSTATEFLAG_ZWRITEENABLE         = 0x00000004, /**<Z-Buffer writing is to be enabled */
    rxRENDERSTATEFLAG_VERTEXALPHAENABLE    = 0x00000008, /**<Vertex alpha is to be enabled */
    rxRENDERSTATEFLAG_FOGENABLE            = 0x00000010, /**<Fog is to be enabled */
    rxRENDERSTATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxRenderStateFlag RxRenderStateFlag;

typedef struct RxRenderStateVector RxRenderStateVector;
/**
 * \ingroup rwcoregeneric
 * \struct RxRenderStateVector
 * Structure describing a render-state vector,
 * used by the RxClRenderState cluster */
struct RxRenderStateVector
{
    RwUInt32             Flags;         /**< A load of the boolean renderstate options */
    RwShadeMode          ShadeMode;     /**< Flat or Gouraud currently */
    RwBlendFunction      SrcBlend;      /**< Src  alpha, 1-src  alpha, etc */
    RwBlendFunction      DestBlend;     /**< Dest alpha, 1-dest alpha, etc */
    RwRaster            *TextureRaster; /**< texture raster */
    RwTextureAddressMode AddressModeU;  /**< U addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureAddressMode AddressModeV;  /**< V addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureFilterMode  FilterMode;    /**< filtering mode - combos of NEAREST LINEAR MIP */
    RwRGBA               BorderColor;   /**< Border color for texturing address mode border */
    RwFogType            FogType;       /**< Select the type of fogging to use */
    RwRGBA               FogColor;      /**< Color used for fogging */
};

#if (!defined(RxRenderStateVectorAssign))
#define RxRenderStateVectorAssign(_target, _source)            \
    ( *(_target) = *(_source) )
#endif /* (!defined(RxRenderStateVectorAssign)) */


/*--- Automatically derived from: C:/daily/rwsdk/src/baimage.h ---*/

/****************************************************************************
 Defines
 */

/* If this bit is set then the image has been allocated by the user */

enum RwImageFlag
{
    rwNAIMAGEFLAG = 0x00,
    rwIMAGEALLOCATED = 0x1,
    rwIMAGEGAMMACORRECTED = 0x2,
    rwIMAGEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwImageFlag RwImageFlag;

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwimage
 * \struct RwImage 
 * Image containing device-independent pixels. 
 * This should be considered an opaque type.
 * Use the RwImage API functions to access.
 */
typedef struct RwImage RwImage;

#if (!defined(DOXYGEN))
struct RwImage
{
        RwInt32             flags;

        RwInt32             width;  /* Device may have different ideas */
        RwInt32             height; /* internally !! */

        RwInt32             depth;  /* Of referenced image */
        RwInt32             stride;

        RwUInt8            *cpPixels;
        RwRGBA             *palette;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rwimage
 * \ref RwImageCallBackRead 
 * is the function registered with \ref RwImageRegisterImageFormat that is used,
 * for example by \ref RwImageRead and \ref RwImageReadMaskedImage,
 * to read images of a specific format from a disk file.
 * 
 * \param  imageName   Pointer to a string containing the file name of the image.
 *
 * \return Returns a pointer to the image read.
 *
 * \see RwImageRegisterImageFormat
 *
 */
typedef RwImage *(*RwImageCallBackRead)(const RwChar * imageName);

/**
 * \ingroup rwimage
 * \ref RwImageCallBackWrite 
 * is the function registered with \ref RwImageRegisterImageFormat that is used,
 * for example by \ref RwImageWrite, 
 * to write images of a specific format to a disk file.
 * 
 * \param  image   Pointer to the image.
 *
 * \param  imageName   Pointer to a string containing the file name
 * of the image.
 *
 *
 * \return Pointer to the written image.
 *
 * \see RwImageRegisterImageFormat
 *
 */
typedef RwImage *(*RwImageCallBackWrite)(RwImage *image, const RwChar *imageName);


/****************************************************************************
 <macro/inline functionality
 */

#define RwImageSetStrideMacro(_image, _stride)      \
    (((_image)->stride = (_stride)), (_image))

#define RwImageSetPixelsMacro(_image, _pixels)      \
    (((_image)->cpPixels = (_pixels)), (_image))

#define RwImageSetPaletteMacro(_image, _palette)    \
    (((_image)->palette = (_palette)), (_image))

#define RwImageGetWidthMacro(_image)                \
    ((_image)->width)

#define RwImageGetHeightMacro(_image)               \
    ((_image)->height)

#define RwImageGetDepthMacro(_image)                \
    ((_image)->depth)

#define RwImageGetStrideMacro(_image)               \
    ((_image)->stride)

#define RwImageGetPixelsMacro(_image)               \
    ((_image)->cpPixels)

#define RwImageGetPaletteMacro(_image)              \
    ((_image)->palette)


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RwImageSetStride(_image, _stride)           \
    RwImageSetStrideMacro(_image, _stride)

#define RwImageSetPixels(_image, _pixels)           \
    RwImageSetPixelsMacro(_image, _pixels)

#define RwImageSetPalette(_image, _palette)         \
    RwImageSetPaletteMacro(_image, _palette)

#define RwImageGetWidth(_image)                     \
    RwImageGetWidthMacro(_image)

#define RwImageGetHeight(_image)                    \
    RwImageGetHeightMacro(_image)

#define RwImageGetDepth(_image)                     \
    RwImageGetDepthMacro(_image)

#define RwImageGetStride(_image)                    \
    RwImageGetStrideMacro(_image)

#define RwImageGetPixels(_image)                    \
    RwImageGetPixelsMacro(_image)

#define RwImageGetPalette(_image)                   \
    RwImageGetPaletteMacro(_image)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/****************************************************************************
 Function prototypes
 */

/*--- Automatically derived from: C:/daily/rwsdk/driver/common/palquant.h ---*/

/****************************************************************************
 Defines
 */

#define RWPALQUANT_MAXDEPTH 8
#define RWPALQUANT_MAXCOLOR (1<<RWPALQUANT_MAXDEPTH)

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwpalquant
 * \struct RwPalQuant
 * This opaque type is a structure that stores working data for the palette
 * quantization functions. This contains the color quantization tree, which
 * has a maximum depth set globally by \ref RwPalQuantSetMaxDepth.
 *
 * \see RwPalQuantInit
 * \see RwPalQuantAddImage
 * \see RwPalQuantResolvePalette
 * \see RwPalQuantMatchImage
 * \see RwPalQuantTerm
 */
typedef struct RwPalQuant RwPalQuant;

#if (!defined(DOXYGEN))

typedef struct _rwPalQuantRGBABox _rwPalQuantRGBABox;
typedef struct _rwPalQuantOctNode _rwPalQuantOctNode;
typedef struct _rwPalQuantLeafNode _rwPalQuantLeafNode;
typedef struct _rwPalQuantBranchNode _rwPalQuantBranchNode;

struct _rwPalQuantRGBABox
{
    RwInt32 col0[4];    /* min value, inclusive */
    RwInt32 col1[4];    /* max value, exclusive */
};

struct _rwPalQuantLeafNode
{
    /* Represents stats for both true leaf nodes and also branch nodes */
    RwReal      weight;     /* Accumulated weight */
    RwRGBAReal  ac;         /* Accumulated (weight * color) */
    RwReal      var;        /* Weight * variance */
    RwUInt8     palIndex;
};

struct _rwPalQuantBranchNode
{
    _rwPalQuantOctNode *dir[16];
};

struct _rwPalQuantOctNode
{
    _rwPalQuantLeafNode   Leaf;
    _rwPalQuantBranchNode Branch;
};

struct RwPalQuant
{
    _rwPalQuantRGBABox  Mcube[RWPALQUANT_MAXCOLOR];
    RwReal              Mvv[RWPALQUANT_MAXCOLOR];
    _rwPalQuantLeafNode Mvol[RWPALQUANT_MAXCOLOR];
    _rwPalQuantOctNode  *root;                    
    RwFreeList          *cubefreelist;            
};

#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/batextur.h ---*/

/****************************************************************************
 Defines
 */

/* Type ID */
#define rwTEXDICTIONARY 6

/* Mipmap Name generation - maximum number of RwChar characters which can
 * be appended to the root name.
 */
#define rwTEXTUREMIPMAPNAMECHARS    16

/* We define texture names to be a maximum of 16 ISO chars */
#define rwTEXTUREBASENAMELENGTH     32

#define rwTEXTUREFILTERMODEMASK     0x000000FF
#define rwTEXTUREADDRESSINGUMASK    0x00000F00
#define rwTEXTUREADDRESSINGVMASK    0x0000F000
#define rwTEXTUREADDRESSINGMASK     (rwTEXTUREADDRESSINGUMASK |  \
                                     rwTEXTUREADDRESSINGVMASK)

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwtexdict
 * \struct RwTexDictionary 
 * is a texture dictionary containing textures. 
 * This should be considered an opaque type.
 * Use the RwTexDictionary API functions to access.
 */
typedef struct RwTexDictionary RwTexDictionary;

#if (!defined(DOXYGEN))
struct RwTexDictionary
{
    RwObject            object; /* Homogeneous type */
    RwLinkList          texturesInDict; /* List of textures in dictionary */
    RwLLLink            lInInstance; /* Link list of all dicts in system */
};
/* Information is entirely device dependent */
#endif /* (!defined(DOXYGEN)) */


/* Parent is the dictionary */

/**
 * \ingroup rwtexture
 * \struct  RwTexture 
 * is a texture object. 
 * This should be considered an opaque type.
 * Use the RwTexture API functions to access.
 */
typedef struct RwTexture RwTexture;

#if (!defined(DOXYGEN))
struct RwTexture
{
    RwRaster           *raster; /** pointer to RwRaster with data */
    RwTexDictionary    *dict;   /* Dictionary this texture is in */
    RwLLLink            lInDictionary; /* List of textures in this dictionary */

    RwChar              name[rwTEXTUREBASENAMELENGTH];  /* Name of the texture */
    RwChar              mask[rwTEXTUREBASENAMELENGTH];  /* Name of the textures mask */

    /* 31 [xxxxxxxx xxxxxxxx vvvvuuuu ffffffff] 0 */
    RwUInt32            filterAddressing; /* Filtering & addressing mode flags */

    RwInt32             refCount; /* Reference count, surprisingly enough */
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackRead 
 * represents the function used by \ref RwTextureRead to read the specified
 * texture from a disk file. This function should return a pointer to the
 * texture to indicate success.
 * 
 * \param  name   Pointer to a string containing the name of
 * the texture to read.
 *
 * \param  maskName   Pointer to a string containing the name
 * of the mask to read and apply to the texture.
 *
 * \return Pointer to the texture
 *
 * \see RwTextureSetReadCallBack
 * \see RwTextureGetReadCallBack
 */
typedef RwTexture *(*RwTextureCallBackRead)(const RwChar *name,
                                            const RwChar *maskName);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackFind
 * represents the function used by \ref RwTextureRead to search for a 
 * texture in memory before attempting to read one from disk. This
 * may involve searching previously loaded texture dictionaries.
 *
 * \param  name   Pointer to a string containing the name of
 * the texture to find.
 *
 * \return Pointer to the texture, or NULL if not found.
 *
 * \see RwTextureSetFindCallBack
 * \see RwTextureGetFindCallBack
 */
typedef RwTexture *(*RwTextureCallBackFind)(const RwChar *name);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBack
 * represents the function called from \ref RwTexDictionaryForAllTextures
 * for all textures in a given texture dictionary. This function should
 * return the current texture to indicate success. The callback may return
 * NULL to terminate further callbacks on the texture dictionary.
 * 
 * \param  texture   Pointer to the current texture.
 *
 * \param  data   User-defined data pointer.
 *
 * \return Pointer to the current texture
 *
 * \see RwTexDictionaryForAllTextures
 */
typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *data);


/**
 * \ingroup rwtexdict
 * \ref RwTexDictionaryCallBack
 * represents the function called from \ref RwTexDictionaryForAllTexDictionaries
 * for all texture dictionaries that currently exist. This function should
 * return the current texture dictionary to indicate success. The callback may
 * return NULL to terminate further callbacks on the texture dictionary. It may
 * safely destroy the current texture dictionary without adversely affecting
 * the iteration process.
 * 
 * \param  dict   Pointer to the current texture dictionary.
 *
 * \param  data   User-defined data pointer.
 *
 * \return Pointer to the current texture dictionary
 *
 * \see RwTexDictionaryForAllTexDictionaries
 */
typedef RwTexDictionary *(*RwTexDictionaryCallBack)(RwTexDictionary *dict, void *data);


/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackMipmapGeneration 
 * is the callback function supplied to \ref RwTextureSetMipmapGenerationCallBack 
 * and returned from \ref RwTextureGetMipmapGenerationCallBack.
 *
 * The supplied function will be passed a pointer to a raster and an image.
 * The raster is the target for the generated mipmap levels and the image 
 * provides the base for their generation.
 * 
 * \param  raster   Pointer to raster, the target for generated mipmap levels
 * \param  image    Pointer to image, used to generate mipmap levels.
 * 
 * \return
 * Returns a pointer to the raster if successful or NULL if an error occurred.
 * 
 * \see RwTextureSetMipmapGenerationCallBack
 * \see RwTextureGetMipmapGenerationCallBack
 * \see RwTextureSetAutoMipmapping
 * \see RwTextureGetAutoMipmapping
 */
typedef RwRaster *(*RwTextureCallBackMipmapGeneration)(RwRaster * raster,
                                                       RwImage * image);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackMipmapName
 * is the callback function supplied to \ref RwTextureSetMipmapNameCallBack and
 * returned from \ref RwTextureGetMipmapNameCallBack.
 *
 * The supplied function will be passed a pointer to a root name, a maskName, a mipmap
 * level and a format. The function returns TRUE if successful and the root name will have been 
 * modified to equal the mipmap name.
 * 
 * \param  name       Pointer to a string containing the root name of the texture. The 
 * mipmap level name is put here.
 * \param  maskName   Pointer to a string containing the root mask name of the texture or
 * NULL if no mask name is required.
 * \param  mipLevel   A value equal to the mipmap level for which the name is required.
 * \param  format     A value describing the mipmapping mode. A combination of the bit
 * flags rwRASTERFORMATMIPMAP and rwRASTERFORMATAUTOMIPMAP.
 * 
 * \return
 * Returns TRUE if the name is generated successfully or FALSE if an error occurred.
 * 
 * \see RwTextureGenerateMipmapName
 * \see RwTextureSetMipmapNameCallBack
 * \see RwTextureGetMipmapNameCallBack
 * \see RwTextureSetAutoMipmapping
 * \see RwTextureGetAutoMipmapping
 */
typedef RwBool (*RwTextureCallBackMipmapName)(RwChar *name,
                                              RwChar *maskName,
                                              RwUInt8 mipLevel,
                                              RwInt32 format);

/****************************************************************************
 <macro/inline functionality
 */

#define RwTextureGetRasterMacro(_tex)                       \
    ((_tex)->raster)

#define RwTextureAddRefMacro(_tex)                          \
    (((_tex)->refCount++), (_tex))

#define RwTextureAddRefVoidMacro(_tex)                      \
MACRO_START                                                 \
{                                                           \
    (_tex)->refCount++;                                     \
}                                                           \
MACRO_STOP

#define RwTextureGetNameMacro(_tex)                         \
    ((_tex)->name)

#define RwTextureGetMaskNameMacro(_tex)                     \
    ((_tex)->mask)

#define RwTextureGetDictionaryMacro(_tex)                   \
    ((_tex)->dict)

#define RwTextureSetFilterModeMacro(_tex, _filtering)                       \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREFILTERMODEMASK) |               \
      (((RwUInt32)(_filtering)) &  rwTEXTUREFILTERMODEMASK)),               \
     (_tex))

#define RwTextureGetFilterModeMacro(_tex)                                   \
    ((RwTextureFilterMode)((_tex)->filterAddressing &                       \
                           rwTEXTUREFILTERMODEMASK))

#define RwTextureSetAddressingMacro(_tex, _addressing)                      \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGMASK) |               \
      (((((RwUInt32)(_addressing)) <<  8) & rwTEXTUREADDRESSINGUMASK) |     \
       ((((RwUInt32)(_addressing)) << 12) & rwTEXTUREADDRESSINGVMASK))),    \
     (_tex))

#define RwTextureSetAddressingUMacro(_tex, _addressing)                     \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGUMASK) |              \
      (((RwUInt32)(_addressing) << 8) & rwTEXTUREADDRESSINGUMASK)),         \
     (_tex))

#define RwTextureSetAddressingVMacro(_tex, _addressing)                     \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGVMASK) |              \
      (((RwUInt32)(_addressing) << 12) & rwTEXTUREADDRESSINGVMASK)),        \
     (_tex))

#define RwTextureGetAddressingMacro(_tex)                                   \
    (((((_tex)->filterAddressing & rwTEXTUREADDRESSINGUMASK) >>  8) ==      \
      (((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) ?      \
     ((RwTextureAddressMode)(((_tex)->filterAddressing &                    \
                              rwTEXTUREADDRESSINGVMASK) >> 12)) :           \
     rwTEXTUREADDRESSNATEXTUREADDRESS)

#define RwTextureGetAddressingUMacro(_tex)                                  \
    ((RwTextureAddressMode)(((_tex)->filterAddressing &                     \
                             rwTEXTUREADDRESSINGUMASK) >> 8))

#define RwTextureGetAddressingVMacro(_tex)                                  \
    ((RwTextureAddressMode)(((_tex)->filterAddressing &                     \
                             rwTEXTUREADDRESSINGVMASK) >> 12))


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RwTextureGetRaster(_tex)                            \
    RwTextureGetRasterMacro(_tex)

#define RwTextureAddRef(_tex)                               \
    RwTextureAddRefMacro(_tex)

#define RwTextureGetName(_tex)                              \
    RwTextureGetNameMacro(_tex)

#define RwTextureGetMaskName(_tex)                          \
    RwTextureGetMaskNameMacro(_tex)

#define RwTextureGetDictionary(_tex)                        \
    RwTextureGetDictionaryMacro(_tex)

#define RwTextureSetFilterMode(_tex, _filtering)            \
    RwTextureSetFilterModeMacro(_tex, _filtering)

#define RwTextureGetFilterMode(_tex)                        \
    RwTextureGetFilterModeMacro(_tex)

#define RwTextureSetAddressing(_tex, _addressing)           \
    RwTextureSetAddressingMacro(_tex, _addressing)

#define RwTextureSetAddressingU(_tex, _addressing)          \
    RwTextureSetAddressingUMacro(_tex, _addressing)

#define RwTextureSetAddressingV(_tex, _addressing)          \
    RwTextureSetAddressingVMacro(_tex, _addressing)

#define RwTextureGetAddressing(_tex)                        \
    RwTextureGetAddressingMacro(_tex)

#define RwTextureGetAddressingU(_tex)                       \
    RwTextureGetAddressingUMacro(_tex)

#define RwTextureGetAddressingV(_tex)                       \
    RwTextureGetAddressingVMacro(_tex)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/p2stdcls.h ---*/

/*
 * Current:
 *
 * wait on Simon for instructions to do cluster renaming thing,
 * or go thru and  change all cluster type names and cluster
 * names (add CSL?)
 *
 */

/* CamVerts.csl */

/* clip flags */

/**
 * \ingroup rwcoregeneric
 * \ref RwClipFlag
 * Flags specifying the clipping status of a vertex
 */
enum RwClipFlag
{
    rwXLOCLIP   = 0x01, /**<The vertex is outside the low X clip-plane */
    rwXHICLIP   = 0x02, /**<The vertex is outside the high X clip-plane */
    rwXCLIP     = 0x03, /**<The vertex is outside an X clip-plane */

    rwYLOCLIP   = 0x04, /**<The vertex is outside the low Y clip-plane */
    rwYHICLIP   = 0x08, /**<The vertex is outside the high Z clip-plane */
    rwYCLIP     = 0x0C, /**<The vertex is outside a Y clip-plane */

    rwZLOCLIP   = 0x10, /**<The vertex is outside the low Z clip-plane */
    rwZHICLIP   = 0x20, /**<The vertex is outside the high Z clip-plane */
    rwZCLIP     = 0x30, /**<The vertex is outside a Z clip-plane */

    rwCLIPMASK  = 0x3F, /**<Mask covering all used bits in the clip flags
                         * in case a plugin  wants free ones to use (e.g RpGloss) */

    rwCLIPFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
/*
 * Typedef for RwClipFlag enumeration specifying the clipping status of a vertex
 */
typedef enum RwClipFlag RwClipFlag;



typedef struct RxCamSpace3DVertex RxCamSpace3DVertex;

/**
 * \ingroup rwcoregeneric
 * \struct RxCamSpace3DVertex
 * Structure describing a camera-space 3D vertex.
 */
struct RxCamSpace3DVertex
{
    /* Position in sheared camera-space (pre-projection) */
    RwV3d           cameraVertex; /**< \ref RwV3d camera-space position of the vertex */
    /* Clip flags on this vertex */
    RwUInt8         clipFlags;    /**< Clip flags for the vertex generated during transformation into camera-space, see \ref RwClipFlag */
    RwUInt8         pad[3];       /**< Alignment padding */
    /* Lit color */
    RwRGBAReal      col;          /**< Accumulated \ref RwReal light values (initialized to zero or prelight colors) */
    /* Only used by the clipper */
    RwReal          u;            /**< Texture U coordinate */
    RwReal          v;            /**< Texture V coordinate */
};

/* Supports pipeline1 apps: */
/**
 * \ingroup rwcoregeneric
 * \ref RwCameraVertex
 * typedef for a structure describing a camera-space 3D vertex.
 */
typedef RxCamSpace3DVertex RwCameraVertex;

/* (used during lighting) Doesn't use alpha yet, but it will */
#define RxCamSpace3DVertexSetRGBA(camvert, r, g, b, a)  \
MACRO_START                                             \
{                                                       \
    ((camvert)->col.red = (r));                         \
    ((camvert)->col.green = (g));                       \
    ((camvert)->col.blue = (b));                        \
    ((camvert)->col.alpha = (a));                       \
}                                                       \
MACRO_STOP

#define RxCamSpace3DVertexAddRGBA(camvert, r, g, b, a)  \
MACRO_START                                             \
{                                                       \
    ((camvert)->col.red += (r));                        \
    ((camvert)->col.green += (g));                      \
    ((camvert)->col.blue += (b));                       \
    ((camvert)->col.alpha += (a));                      \
}                                                       \
MACRO_STOP

/* LEGACY-SUPPORT macros */
#define RXCAMSPACE3DVERTEXSetRGBA(camvert, r, g, b, a)  \
    RxCamSpace3DVertexSetRGBA(camvert, r, g, b, a)
#define RXCAMSPACE3DVERTEXAddRGBA(camvert, r, g, b, a)  \
    RxCamSpace3DVertexAddRGBA(camvert, r, g, b, a)

/* MeshState.csl */


/**
 * \ingroup rwcoregeneric
 * \ref RxGeometryFlag
 * Flags describing geometry properties
 */
enum RxGeometryFlag
{
    rxGEOMETRY_TRISTRIP      = 0x01,  /**<This geometry's meshes can be rendered as tri-strips */
    rxGEOMETRY_POSITIONS     = 0x02,  /**<This geometry has positions */
    rxGEOMETRY_TEXTURED      = 0x04,  /**<This geometry has textures applied */
    rxGEOMETRY_PRELIT        = 0x08,  /**<This geometry has luminance values */
    rxGEOMETRY_NORMALS       = 0x10,  /**<This geometry has normals */
    rxGEOMETRY_LIGHT         = 0x20,  /**<This geometry will be lit */
    rxGEOMETRY_MODULATE      = 0x40,  /**<This geometry will modulate the material color with the vertex colors (prelit + lit) */
    rxGEOMETRY_TEXTURED2     = 0x80,  /**<This geometry has 2 set of texture coordinates */

    rxGEOMETRY_COLORED       = 0x100, /**<This mesh specifies per-vertex colors. NB: ONLY USED IN IM3D */

    rxGEOMETRYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
/*
 * Typedef for RxGeometryFlag enumeration describing geometry properties
 */
typedef enum RxGeometryFlag RxGeometryFlag;

typedef struct RxMeshStateVector RxMeshStateVector;

/**
 * \ingroup rwcoregeneric
 * \struct RxMeshStateVector
 * Structure describing a mesh-state vector,
 * used by the RxClMeshState cluster
 */
struct RxMeshStateVector
{
    RwInt32             Flags;              /**< \ref RxGeometryFlag Flags from the source geometry */
    void               *SourceObject;       /**< A void pointer. In immediate mode it points to an
                                             * internal structure and in atomic/world-sector object
                                             * or material pipelines it points to an \ref RpMaterial. */
    RwMatrix            Obj2World;          /**< \ref RwMatrix to transform from object-space to world-space */
    RwMatrix            Obj2Cam;            /**< \ref RwMatrix to transform from object-space to camera-space */
    RwSurfaceProperties SurfaceProperties;  /**< \ref RwSurfaceProperties */
    /* We can't necessarily reference an RpMaterial in here (i.e with Im3D),
     * because RpMaterials are defined in RpWorld not RwCore */
    RwTexture          *Texture;            /**< A pointer to a \ref RwTexture */
    RwRGBA              MatCol;             /**< \ref RwRGBA material color */
    RxPipeline         *Pipeline;           /**< A pointer to the material pipeline where appropriate */
    /* rwPRIMTYPETRILIST/TRIFAN/TRISTRIP/LINELIST/POLYLINE */
    RwPrimitiveType     PrimType;           /**< \ref RwPrimitiveType primitive type */
    /* Interpretation based on PrimType */
    RwUInt32            NumElements;        /**< \ref RwUInt32 number of elements (triangles, lines...) */
    RwUInt32            NumVertices;        /**< \ref RwUInt32 number of vertices */
    RwInt32             ClipFlagsOr;        /**< Boolean OR of the \ref RwClipFlag clip flags of all vertices in the mesh */
    RwInt32             ClipFlagsAnd;       /**< Boolean AND of the \ref RwClipFlag clip flags of all vertices in the mesh */
    void               *SourceMesh;         /**< A void pointer to the source \ref RpMesh */
    void               *DataObject;         /**< Mirrors the void data pointer of \ref RxPipelineExecute */
};

/* RxScatter.csl */
typedef struct RxScatter RxScatter;
/**
 * \ingroup rwcoregeneric
 * \struct RxScatter
 * Structure used by the RxClScatter cluster
 */
struct RxScatter
{
    RxPipeline    *pipeline; /**< \ref RxPipeline pointer, causes Scatter.csl
                              * to spit the packet out to the specified pipeline */
    RxPipelineNode *node;    /**< \ref RxPipelineNode pointer, causes Scatter.csl
                              * to send the packet to the specified PipelineNode
                              * (as long as it is actually one of its outputs!) */
};

/* RxInterpolants.csl */
typedef struct RxInterp RxInterp;
/**
 * \ingroup rwcoregeneric
 * \struct RxInterp
 * Structure used by the RxClInterp cluster
 */
struct RxInterp
{
    RxVertexIndex originalVert; /**< \ref RxVertexIndex index to the new vertex generated on the clipped edge */
    RxVertexIndex parentVert1;  /**< \ref RxVertexIndex index to the first vertex of the clipped edge */
    RxVertexIndex parentVert2;  /**< \ref RxVertexIndex index to the second vertex of the clipped edge */
    RwReal        interp;       /**< \ref RwReal interpolant along the clipped edge */
};

/* RxUVs.csl */
typedef struct RxUV RxUV;
/**
 * \ingroup rwcoregeneric
 * \struct RxUV
 * Structure used by the RxClUV cluster
 */
struct RxUV
{
    RwReal u; /**< \ref RwReal U texture coordinate */
    RwReal v; /**< \ref RwReal V texture coordinate */
};

/* RxTriPlanes.csl */
typedef struct RxTriPlane RxTriPlane;

/**
 * \ingroup rwcoregeneric
 * \struct RxTriPlane
 * for the packed plane structure used in RpWorlds and by the RxClTriPlane cluster
 * NOTE: this is currently not used in any nodes that ship with the SDK
 */
struct RxTriPlane
{
    /* Packed plane format used in RpWorlds */
    RwUInt32 N; /**< \ref RwUInt32 Packed normal */
    RwReal   w; /**< \ref RwReal distance from origin */
};

/* RxVSteps.csl */

#if (defined(_MSC_VER))
#  if (_MSC_VER>=1000)
#    pragma pack(push, 1)
#  endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

typedef struct RxVStep RxVStep;
/**
 * \ingroup rwcoregeneric
 * \struct RxVStep
 * Structure used by the RxClVStep cluster.
 * To use the step values in the RxClVStep cluster, start at the beginning
 * of the RxVStep and vertex arrays and proceed as follows: (a) Process one
 * vertex, (b) Skip 'step' vertices, (c) Increment the cursor of the RxClVStep
 * cluster. Repeat (a)-(c) until the entire vertex array has been processed.
 * If the RxVStep array contains valid data, you should not have to bounds-check
 * its cursor.
 */
struct RxVStep
{
    RwUInt8 step; /**< \ref RwUInt8 The number of vertices after the current one
                   * which can be skipped in lighting and other calculations because,
                   * for example, they belong only to back-facing triangles */
};

#if (defined(_MSC_VER))
#  if (_MSC_VER>=1000)
#    pragma pack(pop)
#  endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

/* CamNorms.csl */
/**
 * \ingroup rwcoregeneric
 * \ref RxCamNorm
 * typedef for \ref RwV3d used by the RxClVStep cluster */
typedef RwV3d RxCamNorm;

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/baim3d.h ---*/

/**
 * \ingroup rwim3d
 * RwIm3DTransformFlags
 *  The bit-field type  RwIm3DTransformFlags
 * specifies options available for controlling execution of the 3D immediate
 * mode pipeline (see API function \ref RwIm3DTransform):*/
enum RwIm3DTransformFlags
{
    rwIM3D_VERTEXUV      = 1,   /**<Texture coordinates in source vertices should be used */
    rwIM3D_ALLOPAQUE     = 2,   /**<All source vertices are opaque (alpha is 255) */
    rwIM3D_NOCLIP        = 4,   /**<No clipping should be performed on the geometry (the
                                 * app may know it is all onscreen or within the guard band clipping
                                 * region for the current hardware, so clipping can be skipped) */
    rwIM3D_VERTEXXYZ     = 8,   /**<Vertex positions */
    rwIM3D_VERTEXRGBA    = 16,  /**<Vertex color */

    rwIM3DTRANSFORMFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwIm3DTransformFlags RwIm3DTransformFlags;



typedef struct rwIm3DPool            rwIm3DPool;
typedef struct rwImmediGlobals       rwImmediGlobals;
typedef struct rwIm3DRenderPipelines rwIm3DRenderPipelines;
typedef struct _rwIm3DPoolStash       _rwIm3DPoolStash;

#if (0 && defined(SKY2))
typedef struct rwIm3DVertexCache     RwIm3DVertexCache;
#endif /* (0&& defined(SKY2)) */

/* complete information to reconstruct post-transform Im3D "mesh" packet */

#if (!defined(DOXYGEN))
struct _rwIm3DPoolStash
{
    RwUInt32                 flags;    /* rwIM3D_VERTEXUV, rwIM3D_ALLOPAQUE, rwIM3D_NOCLIP etc */
    RwMatrix                *ltm;      /* world-space frame of the vertices, if they have one */
    RwUInt32                 numVerts;
    RxObjSpace3DVertex      *objVerts;
    RxCamSpace3DVertex      *camVerts;
    RxScrSpace2DVertex      *devVerts;
    RxMeshStateVector       *meshState;
    RxRenderStateVector     *renderState;
    RxPipeline              *pipeline;
    RwPrimitiveType          primType;
    RxVertexIndex           *indices;
    RwUInt32                 numIndices;
};

/* Used as a cache of transformed vertices */
struct rwIm3DPool
{
    RwUInt16         numElements; /* could become RwUInt32 nowadays */
    RwUInt16         pad;         /* alignment padding */
    void            *elements;    /* the original array of verts (or whatever...) - referenced not copied */
    RwInt32          stride;      /* the stride of the element */
    _rwIm3DPoolStash  stash;
};

struct rwIm3DRenderPipelines
{
    /* One Im3D render pipeline pointer per rwPRIMTYPE
     * (several pointers may point to the same pipeline though) */
    RxPipeline *triList;
    RxPipeline *triFan;
    RxPipeline *triStrip;
    RxPipeline *lineList;
    RxPipeline *polyLine;
    RxPipeline *pointList;
};

struct rwImmediGlobals
{
    RxPipeline           *im3DTransformPipeline;
    rwIm3DRenderPipelines im3DRenderPipelines;

    /* Platforms that have their own non-generic pipelines
     * (OPENGL, D3D8, SKY2, KAMUI2, DOLPHIN) put them here: */
    RxPipeline           *platformIm3DTransformPipeline;
    rwIm3DRenderPipelines platformIm3DRenderPipelines;

#if (defined(SKY2_DRVMODEL_H))
    /* The triangle PS2All render pipe automatically
     * calls this PS2AllMat pipeline: */
    RxPipeline           *ps2AllMatIm3DTrianglePipeline;
    /* The line PS2All render pipe automatically
     * calls this PS2AllMat pipeline: */
    RxPipeline           *ps2AllMatIm3DLinePipeline;
#endif /* (defined(SKY2_DRVMODEL_H)) */

    rwIm3DPool   curPool;              /* The current cache of transformed vertices */
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/d3d9/im3dpipe.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/d3d9dxttex.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/driver/d3d9/drvfns.h ---*/

#define RWD3D9_MAX_TEXTURE_STAGES  8

#define RWD3D9_MAX_VERTEX_STREAMS  2

typedef struct RxD3D9VertexStream RxD3D9VertexStream;
/**
 * \ingroup worldextensionsd3d9
 * \struct RxD3D9VertexStream
 * This structure contains D3D9 resource specific components.
 */
struct RxD3D9VertexStream
{
    void *vertexBuffer;     /**< Vertex buffer */
    RwUInt32 offset;        /**< Offset in bytes since the beginning
                                 of the Vertex buffer */
    RwUInt32 stride;        /**< Size of the components in bytes */
    RwUInt16 geometryFlags; /**< Geometry locked flags */
    RwUInt8 managed;        /**< Created by the Vertex Buffer Manager */
    RwUInt8 dynamicLock;    /**< Using RwD3D9DynamicVertexBufferLock */
};

typedef void(*rwD3D9DeviceRestoreCallBack)(void);

/*--- Automatically derived from: C:/daily/rwsdk/src/baframe.h ---*/

/****************************************************************************
 Defines
 */

#if (!defined(RWFRAMESTATICPLUGINSSIZE))
#define RWFRAMESTATICPLUGINSSIZE 0
#endif

#define RWFRAMEALIGNMENT(_frame) \
   (! (((rwFRAMEALIGNMENT)-1) & ((RwUInt32)(_frame))))

/* Type ID */
#define rwFRAME 0

/* dirty flag bits */
#define rwFRAMEPRIVATEHIERARCHYSYNCLTM  0x01
#define rwFRAMEPRIVATEHIERARCHYSYNCOBJ  0x02
#define rwFRAMEPRIVATESUBTREESYNCLTM    0x04
#define rwFRAMEPRIVATESUBTREESYNCOBJ    0x08
#define rwFRAMEPRIVATESTATIC            0x10

/****************************************************************************
 Global Types
 */

#if (!defined(DOXYGEN))
struct RwFrame
{
    RwObject            object;

    RwLLLink            inDirtyListLink;

    /* Put embedded matrices here to ensure they remain 16-byte aligned */
    RwMatrix            modelling;
    RwMatrix            ltm;

    RwLinkList          objectList; /* List of objects connected to a frame */

    struct RwFrame      *child;
    struct RwFrame      *next;
    struct RwFrame      *root;   /* Root of the tree */

#if (RWFRAMESTATICPLUGINSSIZE)
    RWALIGN(RwUInt8             pluginData[RWFRAMESTATICPLUGINSSIZE], rwFRAMEALIGNMENT);
#endif /* defined(RWFRAMESTATICPLUGINSIZE)) */
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rwframe
 * \struct RwFrame 
 * Frame for defining object position and orientation. 
 * This should be considered an opaque type.
 * Use the RwFrame API functions to access.
 */
typedef struct RwFrame RWALIGN(RwFrame, rwFRAMEALIGNMENT);

/**
 * \ingroup rwframe
 * \ref RwFrameCallBack type represents the function 
 * called from \ref RwFrameForAllChildren for all child frames linked to a given frame.  
 * This function should return a pointer to the current frame to indicate success.  
 * The callback may return NULL to terminate further callbacks on the child frames.
 * 
 * \param  frame   Pointer to the current frame, supplied by
 * iterator.
 * \param  data  Pointer to developer-defined data structure.
 *
 * \return Pointer to the current frame, or NULL If not found.
 *
 * \see RwFrameForAllChildren
 *
 */
typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);


#define RwFrameGetParentMacro(_f)   ((RwFrame *)rwObjectGetParent(_f))
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetParent(_f)    RwFrameGetParentMacro(_f)
#endif

#define RwFrameGetMatrixMacro(_f)   (&(_f)->modelling)
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetMatrix(_f)    RwFrameGetMatrixMacro(_f)
#endif

/*--- Automatically derived from: C:/daily/rwsdk/src/batypehf.h ---*/

typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);

#if (!defined(DOXYGEN))
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
#endif /* (!defined(DOXYGEN)) */

/* Frames */
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
extern void _rwObjectHasFrameReleaseFrame(void *object);

/* ObjectHASFRAME METHODS */
#define rwObjectHasFrameInitialize(o, type, subtype, syncFunc)  \
MACRO_START                                                     \
{                                                               \
    rwObjectInitialize(o, type, subtype);                       \
    ((RwObjectHasFrame *)o)->sync = syncFunc;                   \
}                                                               \
MACRO_STOP

#define rwObjectHasFrameSync(o) \
    ((RwObjectHasFrame *)(o))->sync(o)

/* Compatibility macros */

#define rwObjectHasFrameSetFrame(object, frame) \
        _rwObjectHasFrameSetFrame(object, frame)
#define rwObjectHasFrameReleaseFrame(object) \
        _rwObjectHasFrameReleaseFrame(object)

/*--- Automatically derived from: C:/daily/rwsdk/src/basync.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/babintex.h ---*/
/****************************************************************************
 Global types
 */
typedef struct rpTextureChunkInfo RwTextureChunkInfo;

#if (!defined(DOXYGEN))
struct rpTextureChunkInfo
{
    RwTextureFilterMode filtering;
    RwTextureAddressMode addressingU;
    RwTextureAddressMode addressingV;
};
#endif /* (!defined(DOXYGEN)) */

/* Bit flags defining properties of textures when stream */
enum RwTextureStreamFlags
{
    rwNATEXTURESTREAMFLAG = 0x00,
    rwTEXTURESTREAMFLAGSUSERMIPMAPS = 0x01,
    rwTEXTURESTREAMFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureStreamFlags RwTextureStreamFlags;

/*--- Automatically derived from: C:/daily/rwsdk/src/babinfrm.h ---*/
/****************************************************************************
 Global types
 */

typedef struct rwFrameList rwFrameList;

#if (!defined(DOXYGEN))
struct rwFrameList
{
    RwFrame **frames;
    RwInt32 numFrames;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/babbox.h ---*/
/****************************************************************************
 Global types
 */

typedef struct RwBBox RwBBox;
/**
 * \ingroup rwbbox
 * \struct RwBBox
 * This type represents a 3D axis-aligned bounding-box
 * specified by the positions of two corners which lie on a diagonal.
 * Typically used to specify a world bounding-box when the world is created
 * 
 * \param sup Supremum vertex (contains largest values)
 * \param inf Infimum vertex (contains smallest values)
 * 
 * \see RpWorldCreate
 */
struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};

#if (!defined(RwBBoxAssign))
#define RwBBoxAssign(_target, _source)            \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwBBoxAssign)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/bacamera.h ---*/

/****************************************************************************
 Defines
 */

/* Type ID */
#define rwCAMERA 4

/* RWPUBLICEND */

/* Used by some drivers for overdraw frustums */

#if defined(__GNUC__) \
    && ((__GNUC__ > 2 ) || (__GNUC__ == 2) && (__GNUC_MINOR__ == 96))

#define RWPLANESETCLOSEST(p)                            \
    (p).closestX = (((p).plane.normal.x)>=((RwReal)0)); \
    (p).closestY = (((p).plane.normal.y)>=((RwReal)0)); \
    (p).closestZ = (((p).plane.normal.z)>=((RwReal)0))

#else
/* This version is not -fstrict-alias safe */
/* 1 Bll 0 Fur */

#define RWPLANESETCLOSEST(p)                                                 \
    (p).closestX = (RwUInt8)(((*(RwInt32 *)(&(p).plane.normal.x))>>31) +1);  \
    (p).closestY = (RwUInt8)(((*(RwInt32 *)(&(p).plane.normal.y))>>31) +1);  \
    (p).closestZ = (RwUInt8)(((*(RwInt32 *)(&(p).plane.normal.z))>>31) +1)

#endif

/* RWPUBLIC */


/****************************************************************************
 <macro/inline functionality
 */

#define RwCameraGetViewOffsetMacro(_camera)                     \
    (&((_camera)->viewOffset))

#define RwCameraSetRasterMacro(_camera, _raster)                \
    (((_camera)->frameBuffer = (_raster)), (_camera))

#define RwCameraSetRasterVoidMacro(_camera, _raster)            \
MACRO_START                                                     \
{                                                               \
    (_camera)->frameBuffer = (_raster);                         \
}                                                               \
MACRO_STOP

#define RwCameraGetRasterMacro(_camera)                         \
    ((_camera)->frameBuffer)

#define RwCameraSetZRasterMacro(_camera, _raster)               \
    (((_camera)->zBuffer = (_raster)), (_camera))

#define RwCameraSetZRasterVoidMacro(_camera, _raster)           \
MACRO_START                                                     \
{                                                               \
    (_camera)->zBuffer = (_raster);                             \
}                                                               \
MACRO_STOP

#define RwCameraGetZRasterMacro(_camera)                        \
    ((_camera)->zBuffer)

#define RwCameraGetNearClipPlaneMacro(_camera)                  \
    ((_camera)->nearPlane)

#define RwCameraGetFarClipPlaneMacro(_camera)                   \
    ((_camera)->farPlane)

#define RwCameraSetFogDistanceMacro(_camera, _distance)         \
    (((_camera)->fogPlane = (_distance)), (_camera))

#define RwCameraGetFogDistanceMacro(_camera)                    \
    ((_camera)->fogPlane)

#define RwCameraGetCurrentCameraMacro()                         \
    ((RwCamera *)RWSRCGLOBAL(curCamera))

#define RwCameraGetProjectionMacro(_camera)                     \
    ((_camera)->projectionType)

#define RwCameraGetViewWindowMacro(_camera)                     \
    (&((_camera)->viewWindow))

#define RwCameraGetViewMatrixMacro(_camera)                     \
    (&((_camera)->viewMatrix))

#define RwCameraSetFrameMacro(_camera, _frame)                  \
    (_rwObjectHasFrameSetFrame((_camera), (_frame)), (_camera))

#define RwCameraSetFrameVoidMacro(_camera, _frame)      \
MACRO_START                                             \
{                                                       \
    _rwObjectHasFrameSetFrame((_camera), (_frame));     \
}                                                       \
MACRO_STOP


#define RwCameraGetFrameMacro(_camera)                          \
    ((RwFrame *)rwObjectGetParent((_camera)))

#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RwCameraGetViewOffset(_camera)                          \
    RwCameraGetViewOffsetMacro(_camera)

#define RwCameraSetRaster(_camera, _raster)                     \
    RwCameraSetRasterMacro(_camera, _raster)

#define RwCameraGetRaster(_camera)                              \
    RwCameraGetRasterMacro(_camera)

#define RwCameraSetZRaster(_camera, _raster)                    \
    RwCameraSetZRasterMacro(_camera, _raster)

#define RwCameraGetZRaster(_camera)                             \
    RwCameraGetZRasterMacro(_camera)

#define RwCameraGetNearClipPlane(_camera)                       \
    RwCameraGetNearClipPlaneMacro(_camera)

#define RwCameraGetFarClipPlane(_camera)                        \
    RwCameraGetFarClipPlaneMacro(_camera)

#define RwCameraSetFogDistance(_camera, _distance)              \
    RwCameraSetFogDistanceMacro(_camera, _distance)

#define RwCameraGetFogDistance(_camera)                         \
    RwCameraGetFogDistanceMacro(_camera)

#define RwCameraGetCurrentCamera()                              \
    RwCameraGetCurrentCameraMacro()

#define RwCameraGetProjection(_camera)                          \
    RwCameraGetProjectionMacro(_camera)

#define RwCameraGetViewWindow(_camera)                          \
    RwCameraGetViewWindowMacro(_camera)

#define RwCameraGetViewMatrix(_camera)                          \
    RwCameraGetViewMatrixMacro(_camera)

#define RwCameraSetFrame(_camera, _frame)                       \
    RwCameraSetFrameMacro(_camera, _frame)

#define RwCameraGetFrame(_camera)                               \
    RwCameraGetFrameMacro(_camera)

#define RwRasterGetDepthMacro(_raster) ((_raster)->depth)
#define RwRasterGetDepth(_raster) RwRasterGetDepthMacro(_raster)

#define RwIm2DGetNearScreenZMacro() (RWSRCGLOBAL(dOpenDevice).zBufferNear)
#define RwIm2DGetNearScreenZ() RwIm2DGetNearScreenZMacro()

#define RwIm2DGetFarScreenZMacro() (RWSRCGLOBAL(dOpenDevice).zBufferFar)
#define RwIm2DGetFarScreenZ() RwIm2DGetFarScreenZMacro()

#define RwCameraGetNearClipPlaneMacro(_camera) ((_camera)->nearPlane)
#define RwCameraGetNearClipPlane(_camera) RwCameraGetNearClipPlaneMacro(_camera)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwcamera
 * RwCameraClearMode 
 * Camera clear flags */
enum RwCameraClearMode
{
    rwCAMERACLEARIMAGE = 0x1,   /**<Clear the frame buffer */
    rwCAMERACLEARZ = 0x2,       /**<Clear the Z buffer */
    rwCAMERACLEARSTENCIL = 0x4, /**<\if xbox   Clear the stencil buffer \endif
                                  * \if d3d8   Clear the stencil buffer \endif
                                  * \if d3d9   Clear the stencil buffer \endif
                                  * \if opengl Clear the stencil buffer \endif
                                  */
    rwCAMERACLEARMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraClearMode RwCameraClearMode;

/**
 * \ingroup rwcamera
 * RwCameraProjection 
 * This type represents the options available for 
 * setting the camera projection model, either perspective projection or 
* parallel projection (see API function \ref RwCameraSetProjection)*/
enum RwCameraProjection
{
    rwNACAMERAPROJECTION = 0,   /**<Invalid projection */
    rwPERSPECTIVE = 1,          /**<Perspective projection */
    rwPARALLEL = 2,             /**<Parallel projection */
    rwCAMERAPROJECTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraProjection RwCameraProjection;

/**
 * \ingroup rwcamera
 * RwFrustumTestResult 
 * This type represents the results from a 
 * camera frustum test on a given sphere (see API function 
 * \ref RwCameraFrustumTestSphere)*/
enum RwFrustumTestResult
{
    rwSPHEREOUTSIDE = 0,    /**<Outside the frustum */
    rwSPHEREBOUNDARY = 1,   /**<On the boundary of the frustum */
    rwSPHEREINSIDE = 2,     /**<Inside the frustum */
    rwFRUSTUMTESTRESULTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFrustumTestResult RwFrustumTestResult;


/**
 * \ingroup rwcamera
 * \struct RwCamera
 * Camera object for rendering a view. 
 * This should be considered an opaque type.
 * Use the RwCamera API functions to access.
 */
typedef struct RwCamera RWALIGN(RwCamera, rwMATRIXALIGNMENT);

/* This allows a world to overload functionality */
typedef RwCamera   *(*RwCameraBeginUpdateFunc) (RwCamera * camera);
typedef RwCamera   *(*RwCameraEndUpdateFunc) (RwCamera * camera);

typedef struct RwFrustumPlane RwFrustumPlane;

#if (!defined(DOXYGEN))
/*
 * Structure describing a frustrum plane.
 */
struct RwFrustumPlane
{
        RwPlane             plane;
        RwUInt8             closestX;
        RwUInt8             closestY;
        RwUInt8             closestZ;
        RwUInt8             pad;
};


struct RwCamera
{
        RwObjectHasFrame    object;

        /* Parallel or perspective projection */
        RwCameraProjection  projectionType;

        /* Start/end update functions */
        RwCameraBeginUpdateFunc beginUpdate;
        RwCameraEndUpdateFunc endUpdate;

        /* The view matrix */
        RwMatrix            viewMatrix;

        /* The cameras image buffer */
        RwRaster           *frameBuffer;

        /* The Z buffer */
        RwRaster           *zBuffer;

        /* Cameras mathmatical characteristics */
        RwV2d               viewWindow;
        RwV2d               recipViewWindow;
        RwV2d               viewOffset;
        RwReal              nearPlane;
        RwReal              farPlane;
        RwReal              fogPlane;

        /* Transformation to turn camera z or 1/z into a Z buffer z */
        RwReal              zScale, zShift;

        /* The clip-planes making up the viewing frustum */
        RwFrustumPlane      frustumPlanes[6];
        RwBBox              frustumBoundBox;

        /* Points on the tips of the view frustum */
        RwV3d               frustumCorners[8];
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rwcamera
 * \ref RwCameraCallBack type represents a function called from any camera
 * iterator that may be implemented in plugins. This function should return a
 * pointer to the current camera to indicate success. The callback may return
 * NULL to terminate further callbacks on other cameras.
 * 
 * \param  camera   Pointer to the current camera, supplied by iterator.
 * \param  data  Pointer to developer-defined data structure.
 *
 * \return Pointer to the current camera, or NULL If not found.
 */
typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);

/*--- Automatically derived from: C:/daily/rwsdk/src/bacamval.h ---*/

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/bapipe.h ---*/

#if (!defined(DOXYGEN))
struct rwPipeGlobals
{
    RwFreeList          *pipesFreeList;   /* Save mallocs, use a freelist */
    RxRenderStateVector  defaultRenderState;
    RwLinkList           allPipelines;    /* Unused as of yet, meant to be used to keep track of all
                                           * created pipelines (for CBs and maybe cleanup) */
    RwUInt32             maxNodesPerPipe; /* Initialised from _rxPipelineMaxNodes at startup. Used to
                                           * allow conservative, single allocations during pipelock */

    /* NOTE: Rw and RpWorld PowerPipe globals kept together for simplicity */

    /* The current default pipelines (used if pipe == NULL for an object) */
    RxPipeline          *currentAtomicPipeline;
    RxPipeline          *currentWorldSectorPipeline;
    RxPipeline          *currentMaterialPipeline;
    /* Generic C-based pipes that run on all platforms
     * - these are set as the current pipes at startup unless
     *   platform-specific pipes (below) are created */
    RxPipeline          *genericAtomicPipeline;
    RxPipeline          *genericWorldSectorPipeline;
    RxPipeline          *genericMaterialPipeline;
    /* Platforms that have their own non-generic pipelines
     * (OPENGL, D3D8, SKY2, KAMUI2, DOLPHIN) put them here: */
    RxPipeline          *platformAtomicPipeline;
    RxPipeline          *platformWorldSectorPipeline;
    RxPipeline          *platformMaterialPipeline;
};
#endif /* (!defined(DOXYGEN)) */

typedef struct rwPipeGlobals rwPipeGlobals;

#define RXPIPELINEGLOBAL(var) (RWPLUGINOFFSET(rwPipeGlobals, RwEngineInstance, _rxPipelineGlobalsOffset)->var)

/*--- Automatically derived from: C:/daily/rwsdk/src/babincam.h ---*/
/****************************************************************************
 Global types
 */

/* Camera stream format */

/**
 * \ingroup rwcamera
 * \ref RwCameraChunkInfo is typedef'd to a structure that holds camera
 * data. This should be considered an opaque type. Use the RwCamera
 * API functions to access it.
 */

typedef struct rwStreamCamera RwCameraChunkInfo;
typedef struct rwStreamCamera rwStreamCamera;

#if (!defined(DOXYGEN))
struct rwStreamCamera
{
    RwV2d viewWindow;
    RwV2d viewOffset;
    RwReal nearPlane, farPlane;
    RwReal fogPlane;
    RwUInt32 projection;
};
#endif /* (!defined(DOXYGEN)) */

#define rwObjectHasFrameSetFrame(object, frame) _rwObjectHasFrameSetFrame(object, frame)
#define rwObjectHasFrameReleaseFrame(object) _rwObjectHasFrameReleaseFrame(object)

RxHeap* RxHeapCreate(RwUInt32 size); // 0x809F90
void RxHeapDestroy(RxHeap* heap); // 0x809F30
RwBool _rxHeapReset(RxHeap* heap); // 0x809EC0
void* RxHeapAlloc(RxHeap* heap, RwUInt32 size); // 0x809AA0
void RxHeapFree(RxHeap* heap, void* block); // 0x8098B0
void* RxHeapRealloc(RxHeap* heap, void* block, RwUInt32 newSize, RwBool allowCopy); // 0x809D10
void RxPipelineSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x804FC0
RwBool _rxPipelineOpen(); // 0x804FE0
RwBool _rxPipelineClose(); // 0x804F60
RxPipeline* RxPipelineCreate(); // 0x8057B0
void _rxPipelineDestroy(RxPipeline* Pipeline); // 0x805820
RxHeap* RxHeapGetGlobalHeap(); // 0x8052F0
RxPipeline* RxPipelineExecute(RxPipeline* pipeline, void* data, RwBool heapReset); // 0x805710
RxPacket* RxPacketCreate(RxPipelineNode* node); // 0x805300
RxCluster* RxClusterSetStride(RxCluster* cluster, RwInt32 stride); // 0x805330
RxCluster* RxClusterSetExternalData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements); // 0x805340
RxCluster* RxClusterSetData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements); // 0x8053A0
void _rxPacketDestroy(RxPacket* Packet); // 0x80F070
RxCluster* RxClusterInitializeData(RxCluster* cluster, RwUInt32 numElements, RwUInt16 stride); // 0x805400
RxCluster* RxClusterResizeData(RxCluster* CurrentCluster, RwUInt32 NumElements); // 0x805470
RxCluster* RxClusterDestroyData(RxCluster* CurrentCluster); // 0x8054C0
RxCluster* RxClusterLockWrite(RxPacket* packet, RwUInt32 clusterIndex, RxPipelineNode* node); // 0x8054F0
void RxClusterUnlock(RxCluster* cluster); // 0x8055C0
RwUInt32 RxPipelineNodeSendConfigMsg(RxPipelineNode* dest, RwUInt32 msg, RwUInt32 intparam, void* ptrparam); // 0x8055D0
RxPipelineNode* RxPipelineNodeForAllConnectedOutputs(RxPipelineNode* node, RxPipeline* pipeline, RxPipelineNodeOutputCallBack callbackfn, void* callbackdata); // 0x8055F0
RxPipelineCluster* RxPipelineNodeGetPipelineCluster(RxPipelineNode* node, RwUInt32 clustersOfInterestIndex); // 0x805680
RwUInt32 RxPipelineClusterGetCreationAttributes(RxPipelineCluster* cluster); // 0x8056B0
RxPipelineCluster* RxPipelineClusterSetCreationAttributes(RxPipelineCluster* cluster, RwUInt32 creationAttributes); // 0x8056C0
RwUInt32 RxClusterGetAttributes(RxCluster* cluster); // 0x8056E0
RxCluster* RxClusterSetAttributes(RxCluster* cluster, RwUInt32 attributes); // 0x8056F0
RxNodeOutput RxPipelineNodeFindOutputByName(RxPipelineNode* node, const RwChar* outputname); // 0x805D10
RxNodeOutput RxPipelineNodeFindOutputByIndex(RxPipelineNode* node, RwUInt32 outputindex); // 0x805D70
RxNodeInput RxPipelineNodeFindInput(RxPipelineNode* node); // 0x805DA0
RxNodeDefinition* RxPipelineNodeCloneDefinition(RxPipelineNode* node, RxClusterDefinition* cluster2add); // 0x8059C0
RxPipeline* RxPipelineNodeRequestCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* clusterDef); // 0x805DB0
RxPipeline* RxPipelineNodeReplaceCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* oldClusterDef, RxClusterDefinition* newClusterDef); // 0x805E20
void* RxPipelineNodeGetInitData(RxPipelineNode* node); // 0x805F30
void* RxPipelineNodeCreateInitData(RxPipelineNode* node, RwUInt32 size); // 0x805EA0
RxPipeline* RxPipelineClone(RxPipeline* pipeline); // 0x806AC0
RxPipelineNode* RxPipelineFindNodeByName(RxPipeline* pipeline, const RwChar* name, RxPipelineNode* start, RwInt32* nodeIndex); // 0x806B30
RxPipelineNode* RxPipelineFindNodeByIndex(RxPipeline* pipeline, RwUInt32 nodeindex); // 0x806BC0
RxLockedPipe* RxPipelineLock(RxPipeline* pipeline); // 0x806990
RxPipeline* RxLockedPipeUnlock(RxLockedPipe* pipeline); // 0x805F40
RxPipeline* RxLockedPipeReplaceNode(RxLockedPipe* pipeline, RxPipelineNode* node, RxNodeDefinition* nodeDef); // 0x806F20
RxPipeline* RxLockedPipeDeleteNode(RxLockedPipe* pipeline, RxPipelineNode* node); // 0x807040
RxPipeline* RxLockedPipeSetEntryPoint(RxLockedPipe* pipeline, RxNodeInput in); // 0x807070
RxPipelineNode* RxLockedPipeGetEntryPoint(RxLockedPipe* pipeline); // 0x8070D0
RxPipeline* RxLockedPipeAddFragment(RxLockedPipe* pipeline, RwUInt32 firstIndex, RxNodeDefinition* nodeDef0, RwUInt32 nodeUnk = 0); // 0x806BE0
RxPipeline* RxLockedPipeAddPath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in); // 0x807100
RxPipeline* RxLockedPipeDeletePath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in); // 0x807170
RxPipeline* RxPipelineInsertDebugNode(RxPipeline* pipeline, RxPipelineNode* before, RxPipelineNode* after, RxNodeDefinition* debugNode); // 0x8071B0
void RwD3D9VertexBufferManagerChangeDefaultSize(RwUInt32 defaultSize); // 0x7F57F0
RwBool RwD3D9CreateVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32* offset); // 0x7F5500
void RwD3D9DestroyVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32 offset); // 0x7F56A0
RwBool RwD3D9DynamicVertexBufferCreate(RwUInt32 size, void* vertexBuffer); // 0x7F5A00
void RwD3D9DynamicVertexBufferDestroy(); // 0x7F5AE0
RwBool RwD3D9DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void* vertexBufferOut, void* vertexDataOut, RwUInt32* baseIndexOut); // 0x7F5B10
RwBool RwD3D9DynamicVertexBufferUnlock(); // 0x7F5C90
void RwRasterSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7FB350
// flags RwRasterFormat, RwRasterPrivateFlag
RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags); // 0x7FB230
RwBool RwRasterDestroy(RwRaster* raster); // 0x7FB020
RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset); // 0x7FAEA0
RwInt32 RwRasterGetNumLevels(RwRaster* raster); // 0x7FB160
RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect); // 0x7FB1D0
RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y); // 0x7FAF50
RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y); // 0x7FAFB0
RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect); // 0x7FAE80
RwRaster* RwRasterPushContext(RwRaster* raster); // 0x7FB060
RwRaster* RwRasterPopContext(); // 0x7FB110
RwRaster* RwRasterGetCurrentContext(); // 0x7FAE60
RwBool RwRasterClear(RwInt32 pixelValue); // 0x7FAEE0
RwBool RwRasterClearRect(RwRect* rpRect, RwInt32 pixelValue); // 0x7FAF90
RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags); // 0x7FB1A0
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode); // 0x7FB2D0
RwRaster* RwRasterUnlock(RwRaster* raster); // 0x7FAEC0
RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode); // 0x7FB0E0
RwRaster* RwRasterUnlockPalette(RwRaster* raster); // 0x7FAFF0
RwInt32 RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x7FB0B0
RwInt32 RwRasterGetPluginOffset(RwUInt32 pluginID); // 0x7FAE40
RxRenderStateVector* RxRenderStateVectorSetDefaultRenderStateVector(RxRenderStateVector* rsvp); // 0x80EA40
RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth); // 0x8026E0
RwBool RwImageDestroy(RwImage* image); // 0x802740
RwImage* RwImageAllocatePixels(RwImage* image); // 0x8027A0
RwImage* RwImageFreePixels(RwImage* image); // 0x802860
RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask); // 0x802AF0
RwImage* RwImageMakeMask(RwImage* image); // 0x802A20
RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname); // 0x8035C0
RwImage* RwImageRead(const RwChar* imageName); // 0x802FD0
const RwChar* RwImageSetPath(const RwChar* path); // 0x802EA0
RwBool RwImageSetGamma(RwReal gammaValue); // 0x803FE0
RwImage* RwImageGammaCorrect(RwImage* image); // 0x803E30
RwBool RwPalQuantInit(RwPalQuant* pq); // 0x80C470
void RwPalQuantTerm(RwPalQuant* pq); // 0x80C520
void RwPalQuantAddImage(RwPalQuant* pq, RwImage* img, RwReal weight); // 0x80AA80
RwInt32 RwPalQuantResolvePalette(RwRGBA* palette, RwInt32 maxcols, RwPalQuant* pq); // 0x80AF60
void RwPalQuantMatchImage(RwUInt8* dstpixels, RwInt32 dststride, RwInt32 dstdepth, RwBool dstPacked, RwPalQuant* pq, RwImage* img); // 0x80BF20
RwBool RwTextureSetMipmapping(RwBool enable); // 0x7F3530
RwBool RwTextureGetMipmapping(); // 0x7F3550
RwBool RwTextureSetAutoMipmapping(RwBool enable); // 0x7F3560
RwBool RwTextureGetAutoMipmapping(); // 0x7F3580
RwBool RwTextureSetMipmapGenerationCallBack(RwTextureCallBackMipmapGeneration callback); // 0x7F3C70
RwTextureCallBackMipmapGeneration RwTextureGetMipmapGenerationCallBack(); // 0x7F3C90
RwBool RwTextureSetMipmapNameCallBack(RwTextureCallBackMipmapName callback); // 0x7F3CA0
RwTextureCallBackMipmapName RwTextureGetMipmapNameCallBack(); // 0x7F3CC0
RwBool RwTextureGenerateMipmapName(RwChar* name, RwChar* maskName, RwUInt8 mipLevel, RwInt32 format); // 0x7F3AA0
RwBool RwTextureRasterGenerateMipmaps(RwRaster* raster, RwImage* image); // 0x7F3CD0
RwBool _rwTextureSetAutoMipMapState(RwBool enable); // 0x7F3590
RwBool _rwTextureGetAutoMipMapState(); // 0x7F35C0
RwTextureCallBackRead RwTextureGetReadCallBack(); // 0x7F3520
RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack); // 0x7F3500
RwTextureCallBackFind RwTextureGetFindCallBack(); // 0x7F34F0
RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack); // 0x7F34D0
RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name); // 0x7F38A0
RwTexture* RwTextureSetMaskName(RwTexture* texture, const RwChar* maskName); // 0x7F3910
void RwTexDictionarySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7F3E80
RwTexDictionary* RwTexDictionaryCreate(); // 0x7F3600
RwBool RwTexDictionaryDestroy(RwTexDictionary* dict); // 0x7F36A0
RwTexture* RwTextureCreate(RwRaster* raster); // 0x7F37C0
RwTexture* RwTextureRead(const char* name, const char* maskName); // 0x7F3AC0
RwBool RwTextureDestroy(RwTexture* texture); // 0x7F3820
RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster); // 0x7F35D0
RwTexture* RwTexDictionaryAddTexture(RwTexDictionary* dict, RwTexture* texture); // 0x7F3980
RwTexture* RwTexDictionaryRemoveTexture(RwTexture* texture); // 0x7F39C0
RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name); // 0x7F39F0
RwTexDictionary* RwTexDictionaryGetCurrent(); // 0x7F3A90
RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict); // 0x7F3A70
const RwTexDictionary* RwTexDictionaryForAllTextures(const RwTexDictionary* dict, RwTextureCallBack fpCallBack, void* data); // 0x7F3730
RwBool RwTexDictionaryForAllTexDictionaries(RwTexDictionaryCallBack fpCallBack, void* data); // 0x7F3770
RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x7F3BB0
RwInt32 RwTexDictionaryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x7F3C10
RwInt32 RwTextureGetPluginOffset(RwUInt32 pluginID); // 0x7F3BE0
RwInt32 RwTexDictionaryGetPluginOffset(RwUInt32 pluginID); // 0x7F3C40
RwBool RwTextureValidatePlugins(const RwTexture* texture); // 0x7F3C00
RwBool RwTexDictionaryValidatePlugins(const RwTexDictionary* dict); // 0x7F3C60
rwIm3DPool* _rwIm3DGetPool(); // 0x7EFDD0
void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags); // 0x7EF450
RwBool RwIm3DEnd(); // 0x7EF520
RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2); // 0x7EF900
RwBool RwIm3DRenderTriangle(RwInt32 vert1, RwInt32 vert2, RwInt32 vert3); // 0x7EF810
RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices); // 0x7EF550
RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType); // 0x7EF6B0
RxPipeline* RwIm3DGetTransformPipeline(); // 0x7EF9D0
RxPipeline* RwIm3DGetRenderPipeline(RwPrimitiveType primType); // 0x7EF9E0
RxPipeline* RwIm3DSetTransformPipeline(RxPipeline* pipeline); // 0x7EFAC0
RxPipeline* RwIm3DSetRenderPipeline(RxPipeline* pipeline, RwPrimitiveType primType); // 0x7EFB10
RwRaster* RwD3D9RasterStreamReadDDS(RwStream* stream); // 0x81F360
RwTexture* RwD3D9DDSTextureRead(const RwChar* name, const RwChar* maskname); // 0x820A10
RwBool RwD3D9RasterIsCompressed(const RwRaster* raster); // 0x820C90
RwBool RwD3D9DeviceSupportsDXTTexture(); // 0x7F9C30
IDirect3DDevice9* RwD3D9GetCurrentD3DDevice(); // 0x7F9D50
RwUInt32 RwD3D9EngineGetMaxMultiSamplingLevels(); // 0x7F84E0
void RwD3D9EngineSetMultiSamplingLevels(RwUInt32 numLevels); // 0x7F84F0
void RwD3D9EngineSetRefreshRate(RwUInt32 refreshRate); // 0x7F8580
void RwD3D9EngineSetMultiThreadSafe(RwBool enable); // 0x7F8620
void RwD3D9EngineSetSoftwareVertexProcessing(RwBool enable); // 0x7F8630
void* RwD3D9GetCurrentD3DRenderTarget(RwUInt32 index); // 0x7F9E80
RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster); // 0x7F9E90
RwBool RwD3D9ChangeVideoMode(RwInt32 modeIndex); // 0x7F8640
RwBool RwD3D9ChangeMultiSamplingLevels(RwUInt32 numLevels); // 0x7F8A90
RwBool RwD3D9CameraAttachWindow(void* camera, void* hwnd); // 0x7F8D70
void RwD3D9SetStreamSource(RwUInt32 streamNumber, void* streamData, RwUInt32 offset, RwUInt32 stride); // 0x7FA030
void _rwD3D9RenderStateFlushCache(); // 0x7FC200
void _rwD3D9DrawIndexedPrimitiveUP(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 primitiveCount, const void* indexData, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride); // 0x7FA1F0
void _rwD3D9DrawPrimitiveUP(RwUInt32 primitiveType, RwUInt32 primitiveCount, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride); // 0x7FA290
void _rwD3D9DrawIndexedPrimitive(RwUInt32 primitiveType, RwInt32 baseVertexIndex, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 primitiveCount); // 0x7FA320
void _rwD3D9SetVertexShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount); // 0x7FACA0
void _rwD3D9SetPixelShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount); // 0x7FAD00
void _rwD3D9SetFVF(RwUInt32 fvf); // 0x7F9F30
void _rwD3D9SetVertexShader(void *shader); // 0x7F9FB0
void _rwD3D9SetPixelShader(void *shader); // 0x7F9FF0
void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value); // 0x7FC2D0
void RwD3D9GetRenderState(RwUInt32 state, void* value); // 0x7FC320
void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value); // 0x7FC340
void RwD3D9GetTextureStageState(RwUInt32 stage, RwUInt32 type, void* value); // 0x7FC3A0
void RwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value); // 0x7FC3C0
void RwD3D9GetSamplerState(RwUInt32 stage, RwUInt32 type, void* value); // 0x7FC400
void RwD3D9SetStencilClear(RwUInt32 stencilClear); // 0x7F9D30
RwUInt32 RwD3D9GetStencilClear(); // 0x7F9D40
RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage); // 0x7FDE70
RwBool RwD3D9SetTransform(RwUInt32 state, const void* matrix); // 0x7FA390
void RwD3D9GetTransform(RwUInt32 state, void* matrix); // 0x7FA4F0
RwBool RwD3D9SetMaterial(const void* material); // 0x7FC430
RwBool RwD3D9SetClipPlane(RwUInt32 index, const RwV4d* plane); // 0x7FC4A0
RwBool RwD3D9SetTransformWorld(const RwMatrix* matrix); // 0x7FA520
RwBool RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surfaceProps, const RwRGBA* color, RwUInt32 flags); // 0x7FC4D0
RwBool RwD3D9SetLight(RwInt32 index, const void* light); // 0x7FA660
void RwD3D9GetLight(RwInt32 index, void* light); // 0x7FA820
RwBool RwD3D9EnableLight(RwInt32 index, RwBool enable); // 0x7FA860
RwBool RwD3D9IndexBufferCreate(RwUInt32 numIndices, void* indexBuffer); // 0x4C9970
RwBool RwD3D9CreateVertexDeclaration(const void* elements, void* vertexdeclaration); // 0x7FAA30
void RwD3D9DeleteVertexDeclaration(); // 0x7FAC10
void RwD3D9DeleteVertexShader(); // 0x7FAC90
RwBool RwD3D9CreatePixelShader(const RwUInt32* function, void* shader); // 0x7FACC0
void RwD3D9DeletePixelShader(); // 0x7FACF0
D3DCAPS9* RwD3D9GetCaps(); // 0x7FAD20
RwBool RwD3D9CameraIsSphereFullyInsideFrustum(const void* camera, const void* sphere); // 0x7FAD30
RwBool RwD3D9CameraIsBBoxFullyInsideFrustum(const void* camera, const void* boundingBox); // 0x7FAD90
void _rwD3D9RasterConvertToNonPalettized(RwRaster* raster); // 0x4CD250
RwBool _rwDeviceRegisterPlugin(void); // 0x7F5F60
void _rwD3D9DeviceSetRestoreCallback(rwD3D9DeviceRestoreCallBack callback); // 0x7FAE20
rwD3D9DeviceRestoreCallBack _rwD3D9DeviceGetRestoreCallback(void); // 0x7FAE30
RwImage* RwImageResample(RwImage* dstImage, const RwImage* srcImage); // 0x80C600
RwImage* RwImageCreateResample(const RwImage* srcImage, RwInt32 width, RwInt32 height); // 0x80CD10
RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster); // 0x804250
RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image); // 0x804290
RwRaster* RwRasterRead(const RwChar* filename); // 0x8043F0
RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname); // 0x8044E0
RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat); // 0x8042C0
RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data); // 0x7F1200
RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine); // 0x7F0E30
RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine); // 0x7F1010
RwFrame* RwFrameScale(RwFrame* frame, const RwV3d* v, RwOpCombineType combine); // 0x7F0ED0
RwFrame* RwFrameTransform(RwFrame* frame, const RwMatrix* m, RwOpCombineType combine); // 0x7F0F70
RwFrame* RwFrameOrthoNormalize(RwFrame* frame); // 0x7F1170
RwFrame* RwFrameSetIdentity(RwFrame* frame); // 0x7F10B0
RwFrame* RwFrameCloneHierarchy(RwFrame* root); // 0x7F0250
RwBool RwFrameDestroyHierarchy(RwFrame* frame); // 0x7F08A0
RwFrame* RwFrameForAllChildren(RwFrame* frame, RwFrameCallBack callBack, void* data); // 0x7F0DC0
RwFrame* RwFrameRemoveChild(RwFrame* child); // 0x7F0CD0
RwFrame* RwFrameAddChild(RwFrame* parent, RwFrame* child); // 0x7F0B00
RwFrame* RwFrameAddChildNoUpdate(RwFrame* parent, RwFrame* child); // 0x7F09C0
RwFrame* RwFrameGetRoot(const RwFrame* frame); // 0x7F09B0
RwMatrix* RwFrameGetLTM(RwFrame* frame); // 0x7F0990
RwFrame* RwFrameUpdateObjects(RwFrame* frame); // 0x7F0910
void RwFrameSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7EFED0
RwFrame* RwFrameCreate(); // 0x7F0410
RwBool RwFrameDestroy(RwFrame* frame); // 0x7F05A0
void _rwFrameInit(RwFrame* frame); // 0x7F0450
void _rwFrameDeInit(RwFrame* frame); // 0x7F06F0
RwBool RwFrameDirty(const RwFrame* frame); // 0x7F0340
RwInt32 RwFrameCount(RwFrame* frame); // 0x7F0E00
RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x7F1260
RwInt32 RwFrameGetPluginOffset(RwUInt32 pluginID); // 0x7F1290
RwBool RwFrameValidatePlugins(const RwFrame* frame); // 0x7F12B0
RwFrame* _rwFrameCloneAndLinkClones(RwFrame* root); // 0x7EFFB0
RwFrame* _rwFramePurgeClone(RwFrame* root); // 0x7F01A0
void _rwObjectHasFrameReleaseFrame(); // 0x804F40
RwBool _rwFrameSyncDirty(void); // 0x809550
void _rwFrameSyncHierarchyLTM(RwFrame* frame); // 0x8097D0
RwInt32 RwTextureRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x804550
RwInt32 RwTextureSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x804580
RwUInt32 RwTextureStreamGetSize(const RwTexture* texture); // 0x8045A0
RwTexture* RwTextureStreamRead(RwStream* stream); // 0x8046E0
const RwTexture* RwTextureStreamWrite(const RwTexture* texture, RwStream* stream); // 0x8045E0
RwInt32 RwTexDictionaryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x8048E0
RwInt32 RwTexDictionarySetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x804910
RwUInt32 RwTexDictionaryStreamGetSize(const RwTexDictionary* texDict); // 0x804930
RwTexDictionary* RwTexDictionaryStreamRead(RwStream* stream); // 0x804C30
const RwTexDictionary* RwTexDictionaryStreamWrite(const RwTexDictionary* texDict, RwStream* stream); // 0x8049F0
RwTextureChunkInfo* _rwTextureChunkInfoRead(RwStream* stream, RwTextureChunkInfo* textureChunkInfo, RwInt32* bytesRead); // 0x804E60
void RwFrameListSetAutoUpdate(RwBool flag); // 0x807570
RwInt32 RwFrameRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x807580
RwInt32 RwFrameSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x8075B0
rwFrameList* _rwFrameListInitialize(rwFrameList* frameList, RwFrame* frame); // 0x8075D0
RwBool _rwFrameListFindFrame(const rwFrameList* frameList, const RwFrame* frame, RwInt32* npIndex); // 0x8076E0
rwFrameList* _rwFrameListDeinitialize(rwFrameList* frameList); // 0x807720
RwUInt32 _rwFrameListStreamGetSize(const rwFrameList* frameList); // 0x807750
rwFrameList* _rwFrameListStreamRead(RwStream* stream, rwFrameList* fl); // 0x807970
const rwFrameList* _rwFrameListStreamWrite(const rwFrameList* frameList, RwStream* stream); // 0x8077A0
RwBBox* RwBBoxCalculate(RwBBox* boundBox, const RwV3d* verts, RwInt32 numVerts); // 0x808F60
RwBBox* RwBBoxInitialize(RwBBox* boundBox, const RwV3d* vertex); // 0x809020
RwBBox* RwBBoxAddPoint(RwBBox* boundBox, const RwV3d* vertex); // 0x809060
RwBool RwBBoxContainsPoint(const RwBBox* boundBox, const RwV3d* vertex); // 0x8090E0
RwCamera* RwCameraBeginUpdate(RwCamera* camera); // 0x7EE190
RwCamera* RwCameraEndUpdate(RwCamera* camera); // 0x7EE180
RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode); // 0x7EE340
RwCamera* RwCameraShowRaster(RwCamera* camera, void* pDev, RwUInt32 flags); // 0x7EE370
void RwCameraSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc); // 0x7EE0F0
RwBool RwCameraDestroy(RwCamera* camera); // 0x7EE4B0
RwCamera* RwCameraCreate(); // 0x7EE4F0
RwCamera* RwCameraClone(RwCamera* camera); // 0x7EF3B0
RwCamera* RwCameraSetViewOffset(RwCamera* camera, const RwV2d* offset); // 0x7EE1A0
RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow); // 0x7EE410
RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection); // 0x7EE3A0
RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip); // 0x7EE1D0
RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip); // 0x7EE2A0
RwInt32 RwCameraRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB); // 0x7EE450
RwInt32 RwCameraGetPluginOffset(RwUInt32 pluginID); // 0x7EE480
RwBool RwCameraValidatePlugins(const RwCamera* camera); // 0x7EE4A0
RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera* camera, const RwSphere* sphere); // 0x7EE2D0
RwInt32 RwCameraRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB); // 0x808C90
RwInt32 RwCameraSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB); // 0x808CC0
RwUInt32 RwCameraStreamGetSize(const RwCamera* camera); // 0x808CE0
RwCamera* RwCameraStreamRead(RwStream* stream); // 0x808DE0
const RwCamera* RwCameraStreamWrite(const RwCamera* camera, RwStream* stream); // 0x808D00
RwCameraChunkInfo* RwCameraChunkInfoRead(RwStream* stream, RwCameraChunkInfo* cameraChunkInfo, RwInt32* bytesRead); // 0x808EF0
