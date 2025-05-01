/***************************************************************************
 *                                                                         *
 * Module  : rwversion.h                                                   *
 *                                                                         *
 * Purpose : RenderWare Versioning                                         *
 *                                                                         *
 **************************************************************************/

#ifndef RWVERSION_H
#define RWVERSION_H

/*
 * Library version number
 *
 * 0xVJNBB:
 *
 * V  = RenderWare version (3 - 7)
 * J  = Release major revision (0 - 15)
 * N  = Release minor revision (0 - 15)
 * BB = Binary format revision (0 - 63)
 *
 * The limits given above are imposed by the 16-bit packed version
 * format contained in stream chunk headers.
 *
 * rwLIBRARYBASEVERSION     Earliest binary compatible version.
 *
 * rwLIBRARYNOWARNVERSION   'No Warning' version. Binary data older than 
 *                          this will cause warnings to be generated on 
 *                          loading.
 */

#define rwLIBRARYCURRENTRWVERSION       3
#define rwLIBRARYCURRENTMAJORREVISION   7
#define rwLIBRARYCURRENTMINORREVISION   0
#define rwLIBRARYCURRENTBINARYREVISION  2

#define rwLIBRARYBASEVERSION    0x35000
#define rwLIBRARYNOWARNVERSION  0x36000
#define rwLIBRARYCURRENTVERSION ((rwLIBRARYCURRENTRWVERSION << 16) |        \
                                 (rwLIBRARYCURRENTMAJORREVISION << 12) |    \
                                 (rwLIBRARYCURRENTMINORREVISION << 8) |     \
                                 (rwLIBRARYCURRENTBINARYREVISION))


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
 *
 * This 16-bit int will be externally defined in an official build, and
 * is used to construct library ID stamp contained in stream chunk headers. 
 * All unofficial builds will be stamped with the following:-
 */

#if !defined(RWBUILDNUMBER)
#define RWBUILDNUMBER 10
#endif

/*
 * 
 * Library ID stamp
 *
 * Bits allocated in packed format (MSB first):-
 *
 *   2  SDK version (zero is RW3)
 *   4  Major revision number
 *   4  Minor revision number
 *   6  Binary format revision number
 *  16  Library build number
 */

#define RWLIBRARYIDPACK(_version, _buildNum)                \
    (  ((((_version) - 0x30000U) & 0x3ff00U) << 14)         \
     | (((_version) & 0x0003fU) << 16)                      \
     | ((_buildNum) & 0xffffU))

#define RWLIBRARYIDUNPACKVERSION(_libraryID)                \
    (  ((((_libraryID) >> 14) & 0x3ff00U) + 0x30000U)       \
     | (((_libraryID) >> 16) & 0x0003fU))

#define RWLIBRARYIDUNPACKBUILDNUM(_libraryID)               \
    ((_libraryID) & 0xffffU)

#endif /* RWVERSION_H */
