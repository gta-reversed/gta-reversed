#ifndef RTDICT_H
#define RTDICT_H

//#include <rtdict.rpe>          /* automatically generated header file */

/* Doxygen plugin groups. */

/**
 * \defgroup rtdict RtDict
 * \ingroup dictionarytools
 *
 * Dictionary Toolkit for RenderWare Graphics.
 */


/****************************************************************************
 Includes
 */

#include <rwplcore.h>

/* RWPUBLIC */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef _RWDLL
#define RTDICTSCHEMADECLARE(schema) extern __declspec(dllimport) RtDictSchema schema;
#else
#define RTDICTSCHEMADECLARE(schema) extern RtDictSchema schema;
#endif

/**
 * \ingroup rtdict
 * Typedef for struct \ref RtDictSchema.
 */
typedef struct RtDictSchema RtDictSchema;

/**
 * \ingroup rtdict
 * Typedef for struct \ref RtDict.
 */
typedef struct RtDict RtDict;

/**
 * \ingroup rtdict
 * Typedef for dictionary entries
 */
typedef void *RtDictEntryType;

/**
 * \ingroup rtdict
 * Typedef for dictionary entries
 */
typedef const void *RtDictEntryConstType;

/**
 * \ingroup rtdict
 *  The following callbacks are needed for each dictionary schema.
 *  See \ref RtDictSchema.
 *
 *
 * \ref RtDictEntryAddRefCallBack
 * defines a callback function for registering that the caller has a
 * reference to a particular entry.
 *
 * \param Entry the entry of which to increment the reference count
 *
 * \return The entry
 */
typedef void *(RtDictEntryAddRefCallBack)(RtDictEntryType entry);

/**
 * \ingroup rtdict
 * \ref RtDictEntryDestroyCallBack
 * defines a callback function for destroying an entry in the dictionary.
 * 
 * \param entry The entry to be destroyed
 *
 * \return Unused, \ref RwBool to match most RenderWare object destruction
 * functions
 */
typedef RwBool (RtDictEntryDestroyCallBack)(RtDictEntryType entry);

/**
 * \ingroup rtdict
 * \ref RtDictEntryGetNameCallBack
 * defines a callback function to get the name of an entry in the
 * dictionary.
 * 
 * \param Entry The entry
 *
 * \return A pointer to the name
 */
typedef const RwChar *(RtDictEntryGetNameCallBack)(RtDictEntryConstType entry);

/**
 * \ingroup rtdict
 * \ref RtDictEntryStreamGetSizeCallBack
 * gets the streamable size of an entry in the dictionary.
 * 
 * \param entry The entry
 *
 * \return The size of the entry when streamed
 */
typedef RwUInt32 (RtDictEntryStreamGetSizeCallBack)(RtDictEntryConstType entry);

/**
 * \ingroup rtdict
 * \ref RtDictEntryStreamReadCallBack
 * reads a new entry from a stream. This function
 * should return a pointer to a freshly allocated and initialized entry,
 * with a nominal reference count of 1 (which will be assigned to the
 * dictionary).
 * 
 * \param stream The stream to read from
 *
 * \return A pointer to the new entry on success; NULL on failure.
 */
typedef RtDictEntryType (RtDictEntryStreamReadCallBack)(RwStream *stream);

/**
 * \ingroup rtdict
 * \ref RtDictStreamReadCompatibilityCallBack
 * enables older dictionaries to be retrofitted to use the new generic 
 * dictionary code. This callback is for internal use.
 * 
 * \param stream The stream to read from
 *
 * \return A pointer to the new dictionary on success; NULL on failure.
 */
typedef RtDict *(RtDictStreamReadCompatibilityCallBack)(RwStream *stream);

/**
 * \ingroup rtdict
 * \ref RtDictEntryStreamWriteCallBack
 * writes an entry of the dictionary to a stream.
 * 
 * \param entry The entry to write
 * \param stream The stream to write to
 *
 * \return The entry on success; NULL on failure.
 */
typedef const RtDictEntryType (RtDictEntryStreamWriteCallBack)(RtDictEntryConstType entry, RwStream *stream);


/**
 *  The following callbacks are used by utility functions that work with dictionaries
 *  See \ref RtDictSchema and \ref RtDict.
 */

/**
 * \ingroup rtdict
 * \ref RtDictCallBack
 * is to be applied to each dictionary in a \ref RtDictSchema.
 * 
 * \param dictionary The dictionary to apply the callback to
 * \param data User supplied data
 *
 * \return The dictionary the callback was applied to.
 * \see RtDictSchemaForAllDictionaries
 */
typedef RtDict *(RtDictCallBack)(RtDict *dictionary, void *data);

/**
 * \ingroup rtdict
 * \ref RtDictEntryCallBack
 * is to be applied to each entry in a dictionary.
 * 
 * \param entry The entry to apply the callback to
 * \param data User supplied data
 *
 * \return The entry the callback was applied to.
 * \see RtDictForAllEntries
 */
typedef RtDictEntryType (RtDictEntryCallBack)(RtDictEntryType entry, void *data);

/**
 * \ingroup rtdict
 * \struct RtDict
 * A container of objects; objects which may be obtained by name. A dictionary
 * is always created and managed via a schema object, \ref RtDictSchema.
 *
 * \see RtDictSchema
 * \see RtDictSchemaCreateDict
 * \see RtDictSchemaStreamReadDict
 */
struct RtDict
{
    RtDictSchema *schema;
            /**< The schema that defines the layout of the dictionary. */
    RwSList *entries;
            /**< The entries held within the dictionary. */
};

/**
 * \ingroup rtdict
 * \struct RtDictSchema
 * Defines the layout and management functions for a dictionary, or set of
 * dictionaries. Maintains a 'current' dictionary for each layout. Provides a 
 * way of creating, or streaming in, dictionaries.
 *
 * \see RtDict
 */
struct RtDictSchema
{
    const RwChar *name;
        /**< Name of the schema.  Handy for debugging. */
    RwUInt32 dictChunkType;
        /**< Chunk type of the dictionary in a stream */
    RwUInt32 entryChunkType;
        /**< Chunk type of the dictionary entries in a stream */
    RwUInt32 compatibilityVersion;
        /**< Used for inner version checking when streaming in dictionaries. If
          *< the dictionary's version is less than this, \ref streamReadCompatibilityCB
          *< will be used for reading in the contents. */
    RwSList *dictionaries;
        /**< List of dictionaries that have been created using this layout */
    RtDict *current;
        /**< Current dictionary of this layout type */
    RtDictEntryAddRefCallBack *addRefCB;
        /**< Callback used to register that the dictionary has a reference to an entry */
    RtDictEntryDestroyCallBack *destroyCB;
        /**< Callback used to register that the dictionary no longer has a reference to an entry */
    RtDictEntryGetNameCallBack *getNameCB;
        /**< Callback used to obtain the name of an entry */
    RtDictEntryStreamGetSizeCallBack *streamGetSizeCB;
        /**< Callback used to obtain the size of an entry */
    RtDictEntryStreamReadCallBack *streamReadCB;
        /**< Callback used to stream in an entry */
    RtDictStreamReadCompatibilityCallBack *streamReadCompatibilityCB;
        /**< Callback used to stream in an old version of the dictionary. For internal use; may be set to NULL */
    RtDictEntryStreamWriteCallBack *streamWriteCB;
        /**< Callback used to stream out an entry */
};

/* Dictionary schema functions */
extern RtDictSchema *
RtDictSchemaInit(RtDictSchema *schema);

extern RwBool
RtDictSchemaDestruct(RtDictSchema *schema);

extern RtDict *
RtDictSchemaStreamReadDict(RtDictSchema *schema, RwStream *stream);

extern RtDict *
RtDictSchemaCreateDict(RtDictSchema *schema);

extern RtDictSchema *
RtDictSchemaAddDict(RtDictSchema *schema, RtDict *dict);

extern RtDictSchema *
RtDictSchemaRemoveDict(RtDictSchema *schema, RtDict *dict);

extern RwBool
RtDictSchemaForAllDictionaries(RtDictSchema *schema, RtDictCallBack *callback, RtDictEntryType data);

extern RtDict *
RtDictSchemaGetCurrentDict(RtDictSchema *schema);

extern RtDictSchema *
RtDictSchemaSetCurrentDict(RtDictSchema *schema, RtDict *dict);

/* Dictionary functions */
extern RwBool
RtDictDestroy(RtDict *dictionary);

extern RtDictEntryType
RtDictAddEntry(RtDict *dictionary, RtDictEntryType entry);

extern RtDictEntryType
RtDictFindNamedEntry(RtDict *dictionary, const RwChar *name);

extern const RtDict *
RtDictForAllEntries(const RtDict *dictionary, RtDictEntryCallBack *callBack, RtDictEntryType data);

extern RtDict *
RtDictRemoveEntry(RtDict *dictionary, RtDictEntryType entry);

extern const RtDict *
RtDictStreamWrite(const RtDict *dictionary, RwStream *stream);

extern RwUInt32
RtDictStreamGetSize(const RtDict *dictionary);

/* RWPUBLICEND */

/* Stealth */
extern RtDict *
_rtDictSchemaInitDict(RtDictSchema *schema, RtDict *dictionary);

extern RwBool
_rtDictDestruct(RtDict *dictionary);


#ifdef    __cplusplus
}
#endif                          /* __cplusplus */


#endif /* RTDICT_H */
