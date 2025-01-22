/***************************************************************************
 * Module  : rpcriter.h                                                    *
 *                                                                         *
 * Purpose : Store pluginIDs for RenderWare Graphics plugins and toolkits  *
 *                                                                         *
 **************************************************************************/

/* These are plugins */

#define rwID_METRICSPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x01)
/**
 * \ingroup rwengine
 * \def rwID_SPLINEPLUGIN defines the pluginID for the \ref rpspline plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SPLINEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x02)
#define rwID_STEREOPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x03)
#define rwID_VRMLPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x04)
/**
 * \ingroup rwengine
 * \def rwID_MORPHPLUGIN defines the pluginID for the \ref rpmorph plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_MORPHPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x05)
#define rwID_PVSPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x06)
#define rwID_MEMLEAKPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x07)
#define rwID_ANIMPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x08)
#define rwID_GLOSSPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x09)
/**
 * \ingroup rwengine
 * \def rwID_LOGOPLUGIN defines the pluginID for the \ref rplogo plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_LOGOPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0a)
#define rwID_MEMINFOPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0b)
/**
 * \ingroup rwengine
 * \def rwID_RANDOMPLUGIN defines the pluginID for the \ref rprandom plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_RANDOMPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0c)
/**
 * \ingroup rwengine
 * \def rwID_PNGIMAGEPLUGIN defines the pluginID for the \ref rtpng plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PNGIMAGEPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0d)
#define rwID_BONEPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0e)
#define rwID_VRMLANIMPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x0f)
/**
 * \ingroup rwengine
 * \def rwID_SKYMIPMAPVAL defines the pluginID for the \ref rtmipk plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SKYMIPMAPVAL    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x10)
#define rwID_MRMPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x11)
/**
 * \ingroup rwengine
 * \def rwID_LODATMPLUGIN defines the pluginID for the \ref rplodatm plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_LODATMPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x12)
#define rwID_MEPLUGIN        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x13)
/**
 * \ingroup rwengine
 * \def rwID_LTMAPPLUGIN defines the pluginID for the \ref rpltmap plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_LTMAPPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x14)
#define rwID_REFINEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x15)
/**
 * \ingroup rwengine
 * \def rwID_SKINPLUGIN defines the pluginID for the \ref rpskin plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SKINPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x16)
#define rwID_LABELPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x17)
#define rwID_PARTICLESPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x18)
#define rwID_GEOMTXPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x19)
#define rwID_SYNTHCOREPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1a)
#define rwID_STQPPPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1b)
#define rwID_PARTPPPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1c)
/**
 * \ingroup rwengine
 * \def rwID_COLLISPLUGIN defines the pluginID for the \ref rpcollis plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_COLLISPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1d)
/**
 * \ingroup rwengine
 * \def rwID_HANIMPLUGIN defines the pluginID for the \ref rphanim plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_HANIMPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1e)
/**
 * \ingroup rwengine
 * \def rwID_USERDATAPLUGIN defines the pluginID for the \ref rpuserdata plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_USERDATAPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1f)
/**
 * \ingroup rwengine
 * \def rwID_MATERIALEFFECTSPLUGIN defines the pluginID for the \ref rpmatfx plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_MATERIALEFFECTSPLUGIN \
                             MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x20)
#define rwID_PARTICLESYSTEMPLUGIN \
                             MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x21)
/**
 * \ingroup rwengine
 * \def rwID_DMORPHPLUGIN defines the pluginID for the \ref rpdmorph plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_DMORPHPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x22)
/**
 * \ingroup rwengine
 * \def rwID_PATCHPLUGIN defines the pluginID for the \ref rppatch plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PATCHPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x23)
/**
 * \ingroup rwengine
 * \def rwID_TEAMPLUGIN defines the pluginID for the \ref rpteam plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TEAMPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x24)
/**
 * \ingroup rwengine
 * \def rwID_CROWDPPPLUGIN defines the pluginID for the \ref rpcrowd plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_CROWDPPPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x25)
#define rwID_MIPSPLITPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x26)
/**
 * \ingroup rwengine
 * \def rwID_ANISOTPLUGIN defines the pluginID for the \ref rpanisot plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_ANISOTPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x27)
/* #define THIS SPACE FREE!     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x28) */
/**
 * \if gcn \ingroup rwengine
 * \def rwID_GCNMATPLUGIN defines the GameCube specific Multi-Texturing
 *      extension to the RpMaterial.See \ref RwEngineRegisterPlugin \endif
 */
#define rwID_GCNMATPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x29)
/**
 * \ingroup rwengine
 * \def rwID_GPVSPLUGIN defines the pluginID for the \ref rppvs plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_GPVSPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2a)
/**
 * \if xbox \ingroup rwengine
 *     \def rwID_XBOXMATPLUGIN defines the Xbox specific Multi-Texturing
 *          extension to the RpMaterial. See \ref RwEngineRegisterPlugin
 * \endif
 */
#define rwID_XBOXMATPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2b)
/**
 * \if gcn \ingroup rwengine
 * \def rwID_MULTITEXPLUGIN defines the pluginID for the \ref rpmatfx
 *      Multi-Texturing Extension. See \ref RpMTEffect and
 *      \ref RwEngineRegisterPlugin \endif
 * \if xbox \ingroup rwengine
 * \def rwID_MULTITEXPLUGIN defines the pluginID for the \ref rpmatfx
 *      Multi-Texturing Extension. See \ref RpMTEffect and
 *      \ref RwEngineRegisterPlugin\endif
 */
#define rwID_MULTITEXPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2c)
#define rwID_CHAINPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2d)
/**
 * \ingroup rwengine
 * \def rwID_TOONPLUGIN defines the pluginID for the \ref rptoon plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TOONPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2e)
/**
 * \ingroup rwengine
 * \def rwID_PTANKPLUGIN defines the pluginID for the \ref rpptank plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PTANKPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2f)
/**
 * \ingroup rwengine
 * \def rwID_PRTSTDPLUGIN defines the pluginID for the \ref rpprtstd plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PRTSTDPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x30)
/**
 * \if sky2 \ingroup rwengine
 * \def rwID_PDSPLUGIN defines the pluginID for the \ref rppds plugin.
 * See \ref RwEngineRegisterPlugin \endif
 */
#define rwID_PDSPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x31)
/**
 * \ingroup rwengine
 * \def rwID_PRTADVPLUGIN defines the pluginID for the \ref rpprtadv plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PRTADVPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x32)
/**
 * \if xbox \ingroup rwengine
 * \def rwID_NORMMAPPLUGIN defines the pluginID for the \ref rpnormmap plugin.
 * See \ref RwEngineRegisterPlugin \endif
 */
#define rwID_NORMMAPPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x33)
/**
 * \ingroup rwengine
 * \def rwID_ADCPLUGIN defines the pluginID for the \ref rpadc plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_ADCPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x34)
/**
 * \ingroup rwengine
 * \def rwID_UVANIMPLUGIN defines the pluginID for the \ref rpuvanim plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_UVANIMPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x35)

/**
 * \ingroup rwengine
 * \def rwID_ENVIRONMENTPLUGIN defines the pluginID for background environment
 * settings for a scene. For internal use.
 */
#define rwID_ENVIRONMENTPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x36)

/* These are toolkits */

/**
 * \ingroup rwengine
 * \def rwID_CHARSEPLUGIN defines the pluginID for the \ref rtcharset plugin.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_CHARSEPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x80)
/**
 * \ingroup rwengine
 * \def rwID_NOHSWORLDPLUGIN defines the pluginID for the \ref rtworldimport toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_NOHSWORLDPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x81)
#define rwID_IMPUTILPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x82)
/**
 * \ingroup rwengine
 * \def rwID_SLERPPLUGIN defines the pluginID for the \ref rtslerp toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SLERPPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x83)
#define rwID_OPTIMPLUGIN       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x84)
/**
 * \ingroup rwengine
 * \def rwID_TLWORLDPLUGIN defines the pluginID for the \ref rtworld toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TLWORLDPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x85)
#define rwID_DATABASEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x86)
#define rwID_RAYTRACEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x87)
/**
 * \ingroup rwengine
 * \def rwID_RAYPLUGIN defines the pluginID for the \ref rtray toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_RAYPLUGIN         MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x88)
#define rwID_LIBRARYPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x89)
/**
 * \ingroup rwengine
 * \def rwID_2DPLUGIN defines the pluginID for the \ref rt2d toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_2DPLUGIN          MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x90)
/**
 * \ingroup rwengine
 * \def rwID_TILERENDPLUGIN defines the pluginID for the \ref RtTileRender toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TILERENDPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x91)
/**
 * \ingroup rwengine
 * \def rwID_JPEGIMAGEPLUGIN defines the pluginID for the RtJPEG toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_JPEGIMAGEPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x92)
#define rwID_TGAIMAGEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x93)
#define rwID_GIFIMAGEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x94)
/**
 * \ingroup rwengine
 * \def rwID_QUATPLUGIN defines the pluginID for the \ref rtquat toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_QUATPLUGIN        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x95)
/**
 * \ingroup rwengine
 * \def rwID_SPLINEPVSPLUGIN defines the pluginID for the \ref rtsplinepvs toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SPLINEPVSPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x96)
#define rwID_MIPMAPPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x97)
/**
 * \ingroup rwengine
 * \def rwID_MIPMAPKPLUGIN defines the pluginID for the \ref rpmipkl toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_MIPMAPKPLUGIN     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x98)
#define rwID_2DFONT            MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x99)
/**
 * \ingroup rwengine
 * \def rwID_INTSECPLUGIN defines the pluginID for the \ref rtintersection toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_INTSECPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9a)
/**
 * \ingroup rwengine
 * \def rwID_TIFFIMAGEPLUGIN defines the pluginID for the \ref rttiff toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TIFFIMAGEPLUGIN   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9b)
/**
 * \ingroup rwengine
 * \def rwID_PICKPLUGIN defines the pluginID for the \ref rtpick toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PICKPLUGIN        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9c)
/**
 * \ingroup rwengine
 * \def rwID_BMPIMAGEPLUGIN defines the pluginID for the \ref rtbmp toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_BMPIMAGEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9d)
/**
 * \ingroup rwengine
 * \def rwID_RASIMAGEPLUGIN defines the pluginID for the \ref rtras toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_RASIMAGEPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9e)
#define rwID_SKINFXPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x9f)
/**
 * \ingroup rwengine
 * \def rwID_VCATPLUGIN defines the pluginID for the \ref rtvcat toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_VCATPLUGIN        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa0)
#define rwID_2DPATH            MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa1)
#define rwID_2DBRUSH           MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa2)
#define rwID_2DOBJECT          MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa3)
#define rwID_2DSHAPE           MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa4)
#define rwID_2DSCENE           MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa5)
#define rwID_2DPICKREGION      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa6)
#define rwID_2DOBJECTSTRING    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa7)
#define rwID_2DANIMPLUGIN      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa8)
/**
 * \ingroup rwengine
 * \def rwID_2DANIM defines the pluginID for the \ref rt2danim toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_2DANIM            MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xa9)
#define rwID_2DKEYFRAME        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb0)
#define rwID_2DMAESTRO         MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb1)
/**
 * \ingroup rwengine
 * \def rwID_BARYCENTRIC defines the pluginID for the \ref rtbary toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_BARYCENTRIC       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb2)
/**
 * \ingroup rwengine
 * \def rwID_PITEXDICTIONARYTK defines the pluginID for the \ref rtpitexd toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_PITEXDICTIONARYTK MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb3)
/**
 * \ingroup rwengine
 * \def rwID_TOCTOOLKIT defines the pluginID for the \ref rttoc toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_TOCTOOLKIT        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb4)
#define rwID_TPLTOOLKIT        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb5)
/**
 * \if sky2 \ingroup rwengine
 * \def rwID_ALTPIPETOOLKIT defines the pluginID for the \ref rtaltpipesky2 toolkit.
 * See \ref RwEngineRegisterPlugin \endif
 */
#define rwID_ALTPIPETOOLKIT    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb6)
/**
 * \ingroup rwengine
 * \def rwID_ANIMTOOLKIT defines the pluginID for the \ref rtanim toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_ANIMTOOLKIT       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb7)
/**
 * \ingroup rwengine
 * \def rwID_SKINSPLITTOOKIT defines the pluginID for the \ref rtskinsplit toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_SKINSPLITTOOKIT   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb8)
/**
 * \ingroup rwengine
 * \def rwID_CMPKEYTOOLKIT defines the pluginID for the \ref rtcmpkey toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_CMPKEYTOOLKIT     MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xb9)
/**
 * \ingroup rwengine
 * \def rwID_GEOMCONDPLUGIN defines the pluginID for the \ref rtgcond toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_GEOMCONDPLUGIN    MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xba)
/**
 * \ingroup rwengine
 * \def rwID_WINGPLUGIN defines the pluginID for the \ref rtwing toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_WINGPLUGIN        MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xbb)
/**
 * \ingroup rwengine
 * \def rwID_GENCPIPETOOLKIT defines the pluginID for the \ref rtgencpipe toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_GENCPIPETOOLKIT   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xbc)
/**
 * \ingroup rwengine
 * \def rwID_LTMAPCNVTOOLKIT defines the pluginID for the \ref rtltmapcnv toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_LTMAPCNVTOOLKIT   MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xbd)
/**
 * \ingroup rwengine
 * \def rwID_FILESYSTEMPLUGIN defines the pluginID for the \ref rtfsyst toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_FILESYSTEMPLUGIN  MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xbe)
/**
 * \ingroup rwengine
 * \def rwID_DICTTOOLKIT defines the pluginID for the \ref rtdict toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_DICTTOOLKIT       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xbf)
/**
 * \ingroup rwengine
 * \def rwID_DICTTOOLKIT defines the pluginID for the \ref rtdict toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_UVANIMLINEAR      MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xc0)
/**
 * \ingroup rwengine
 * \def rwID_DICTTOOLKIT defines the pluginID for the \ref rtdict toolkit.
 * See \ref RwEngineRegisterPlugin
 */
#define rwID_UVANIMPARAM       MAKECHUNKID(rwVENDORID_CRITERIONTK, 0xc1)



