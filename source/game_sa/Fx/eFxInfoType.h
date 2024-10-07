#pragma once

/* Legend:
 *   MH - Manhunt (Version 104)
 *   SA - San Andreas (Version 109)
 */

enum eFxInfoType : uint16 {
    FX_INFO_EMRATE_DATA           = 0x1001, // MH SA
    //                            = 0x1002  //
    FX_INFO_EMSIZE_DATA           = 0x1004, // MH SA
    FX_INFO_EMSPEED_DATA          = 0x1008, // MH SA
    FX_INFO_EMDIR_DATA            = 0x1010, // MH SA
    FX_INFO_EMANGLE_DATA          = 0x1020, // MH SA
    FX_INFO_EMLIFE_DATA           = 0x1040, // MH SA
    FX_INFO_EMPOS_DATA            = 0x1080, // MH SA
    FX_INFO_EMWEATHER_DATA        = 0x1100, //    SA
    FX_INFO_EMROTATION_DATA       = 0x1200, //    SA
    //                            = 0x1400, //
    //                            = 0x1800, //

    FX_INFO_NOISE_DATA            = 0x2001, // MH SA
    FX_INFO_FORCE_DATA            = 0x2002, // MH SA
    FX_INFO_FRICTION_DATA         = 0x2004, // MH SA
    FX_INFO_ATTRACTPT_DATA        = 0x2008, // MH SA
    FX_INFO_ATTRACTLINE_DATA      = 0x2010, // MH SA
    FX_INFO_GROUNDCOLLIDE_DATA    = 0x2020, // MH SA
    FX_INFO_WIND_DATA             = 0x2040, // MH SA
    FX_INFO_JITTER_DATA           = 0x2080, // MH SA
    FX_INFO_ROTSPEED_DATA         = 0x2100, //    SA
    FX_INFO_FLOAT_DATA            = 0x2200, //    SA
    FX_INFO_UNDERWATER_DATA       = 0x2400, //
    //                            = 0x2800, //

    FX_INFO_COLOUR_DATA           = 0x4001, // MH SA
    FX_INFO_SIZE_DATA             = 0x4002, // MH SA
    FX_INFO_SPRITERECT_DATA       = 0x4004, // MH SA
    FX_INFO_HEATHAZE_DATA         = 0x4008, //    SA
    FX_INFO_TRAIL_DATA            = 0x4010, //    SA
    FX_INFO_FLAT_DATA             = 0x4020, // MH SA
    FX_INFO_DIR_DATA              = 0x4040, // MH SA
    FX_INFO_ANIMTEX_DATA          = 0x4080, // MH SA
    FX_INFO_COLOURRANGE_DATA      = 0x4100, // MH SA
    FX_INFO_SELFLIT_DATA          = 0x4200, //    SA
    FX_INFO_COLOURBRIGHT_DATA     = 0x4400, //    SA

    FX_INFO_ROTATE_DATA           = 0x4008, // MH
    FX_INFO_TEXCOORDS_DATA        = 0x4010, // MH
    FX_INFO_ROTATEOFFSET_DATA     = 0x4200, // MH
    //                            = 0x4800, //

    FX_INFO_SMOKE_DATA            = 0x8001, //    SA
};
