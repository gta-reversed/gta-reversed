#pragma once

/*!
 * @brief Generic voices (For `PED_TYPE_GEN`)
 */
enum eGenSpeechVoices : int16 {
    VOICE_GEN_BBDYG1  = 0,   // 0x0
    VOICE_GEN_BBDYG2  = 1,   // 0x1
    VOICE_GEN_BFORI   = 2,   // 0x2
    VOICE_GEN_BFOST   = 3,   // 0x3
    VOICE_GEN_BFYBE   = 4,   // 0x4
    VOICE_GEN_BFYBU   = 5,   // 0x5
    VOICE_GEN_BFYCRP  = 6,   // 0x6
    VOICE_GEN_BFYPRO  = 7,   // 0x7
    VOICE_GEN_BFYRI   = 8,   // 0x8
    VOICE_GEN_BFYST   = 9,   // 0x9
    VOICE_GEN_BIKDRUG = 10,  // 0xA
    VOICE_GEN_BIKERA  = 11,  // 0xB
    VOICE_GEN_BIKERB  = 12,  // 0xC
    VOICE_GEN_BMOCD   = 13,  // 0xD
    VOICE_GEN_BMORI   = 14,  // 0xE
    VOICE_GEN_BMOSEC  = 15,  // 0xF
    VOICE_GEN_BMOST   = 16,  // 0x10
    VOICE_GEN_BMOTR1  = 17,  // 0x11
    VOICE_GEN_BMYAP   = 18,  // 0x12
    VOICE_GEN_BMYBE   = 19,  // 0x13
    VOICE_GEN_BMYBOUN = 20,  // 0x14
    VOICE_GEN_BMYBOX  = 21,  // 0x15
    VOICE_GEN_BMYBU   = 22,  // 0x16
    VOICE_GEN_BMYCG   = 23,  // 0x17
    VOICE_GEN_BMYCON  = 24,  // 0x18
    VOICE_GEN_BMYCR   = 25,  // 0x19
    VOICE_GEN_BMYDJ   = 26,  // 0x1A
    VOICE_GEN_BMYDRUG = 27,  // 0x1B
    VOICE_GEN_BMYMOUN = 28,  // 0x1C
    VOICE_GEN_BMYPOL1 = 29,  // 0x1D
    VOICE_GEN_BMYPOL2 = 30,  // 0x1E
    VOICE_GEN_BMYRI   = 31,  // 0x1F
    VOICE_GEN_BMYST   = 32,  // 0x20
    VOICE_GEN_BYMPI   = 33,  // 0x21
    VOICE_GEN_CWFOFR  = 34,  // 0x22
    VOICE_GEN_CWFOHB  = 35,  // 0x23
    VOICE_GEN_CWFYFR1 = 36,  // 0x24
    VOICE_GEN_CWFYFR2 = 37,  // 0x25
    VOICE_GEN_CWFYHB1 = 38,  // 0x26
    VOICE_GEN_CWMOFR1 = 39,  // 0x27
    VOICE_GEN_CWMOHB1 = 40,  // 0x28
    VOICE_GEN_CWMOHB2 = 41,  // 0x29
    VOICE_GEN_CWMYFR  = 42,  // 0x2A
    VOICE_GEN_CWMYHB1 = 43,  // 0x2B
    VOICE_GEN_CWMYHB2 = 44,  // 0x2C
    VOICE_GEN_DNFOLC1 = 45,  // 0x2D
    VOICE_GEN_DNFOLC2 = 46,  // 0x2E
    VOICE_GEN_DNFYLC  = 47,  // 0x2F
    VOICE_GEN_DNMOLC1 = 48,  // 0x30
    VOICE_GEN_DNMOLC2 = 49,  // 0x31
    VOICE_GEN_DNMYLC  = 50,  // 0x32
    VOICE_GEN_DWFOLC  = 51,  // 0x33
    VOICE_GEN_DWFYLC1 = 52,  // 0x34
    VOICE_GEN_DWFYLC2 = 53,  // 0x35
    VOICE_GEN_DWMOLC1 = 54,  // 0x36
    VOICE_GEN_DWMOLC2 = 55,  // 0x37
    VOICE_GEN_DWMYLC1 = 56,  // 0x38
    VOICE_GEN_DWMYLC2 = 57,  // 0x39
    VOICE_GEN_HFORI   = 58,  // 0x3A
    VOICE_GEN_HFOST   = 59,  // 0x3B
    VOICE_GEN_HFYBE   = 60,  // 0x3C
    VOICE_GEN_HFYPRO  = 61,  // 0x3D
    VOICE_GEN_HFYRI   = 62,  // 0x3E
    VOICE_GEN_HFYST   = 63,  // 0x3F
    VOICE_GEN_HMORI   = 64,  // 0x40
    VOICE_GEN_HMOST   = 65,  // 0x41
    VOICE_GEN_HMYBE   = 66,  // 0x42
    VOICE_GEN_HMYCM   = 67,  // 0x43
    VOICE_GEN_HMYCR   = 68,  // 0x44
    VOICE_GEN_HMYDRUG = 69,  // 0x45
    VOICE_GEN_HMYRI   = 70,  // 0x46
    VOICE_GEN_HMYST   = 71,  // 0x47
    VOICE_GEN_IMYST   = 72,  // 0x48
    VOICE_GEN_IRFYST  = 73,  // 0x49
    VOICE_GEN_IRMYST  = 74,  // 0x4A
    VOICE_GEN_MAFFA   = 75,  // 0x4B
    VOICE_GEN_MAFFB   = 76,  // 0x4C
    VOICE_GEN_MALE01  = 77,  // 0x4D
    VOICE_GEN_NOVOICE = 78,  // 0x4E
    VOICE_GEN_OFORI   = 79,  // 0x4F
    VOICE_GEN_OFOST   = 80,  // 0x50
    VOICE_GEN_OFYRI   = 81,  // 0x51
    VOICE_GEN_OFYST   = 82,  // 0x52
    VOICE_GEN_OMOBOAT = 83,  // 0x53
    VOICE_GEN_OMOKUNG = 84,  // 0x54
    VOICE_GEN_OMORI   = 85,  // 0x55
    VOICE_GEN_OMOST   = 86,  // 0x56
    VOICE_GEN_OMYRI   = 87,  // 0x57
    VOICE_GEN_OMYST   = 88,  // 0x58
    VOICE_GEN_SBFORI  = 89,  // 0x59
    VOICE_GEN_SBFOST  = 90,  // 0x5A
    VOICE_GEN_SBFYPRO = 91,  // 0x5B
    VOICE_GEN_SBFYRI  = 92,  // 0x5C
    VOICE_GEN_SBFYST  = 93,  // 0x5D
    VOICE_GEN_SBFYSTR = 94,  // 0x5E
    VOICE_GEN_SBMOCD  = 95,  // 0x5F
    VOICE_GEN_SBMORI  = 96,  // 0x60
    VOICE_GEN_SBMOST  = 97,  // 0x61
    VOICE_GEN_SBMOTR1 = 98,  // 0x62
    VOICE_GEN_SBMOTR2 = 99,  // 0x63
    VOICE_GEN_SBMYCR  = 100, // 0x64
    VOICE_GEN_SBMYRI  = 101, // 0x65
    VOICE_GEN_SBMYST  = 102, // 0x66
    VOICE_GEN_SBMYTR3 = 103, // 0x67
    VOICE_GEN_SFYPRO  = 104, // 0x68
    VOICE_GEN_SHFYPRO = 105, // 0x69
    VOICE_GEN_SHMYCR  = 106, // 0x6A
    VOICE_GEN_SMYST   = 107, // 0x6B
    VOICE_GEN_SMYST2  = 108, // 0x6C
    VOICE_GEN_SOFORI  = 109, // 0x6D
    VOICE_GEN_SOFOST  = 110, // 0x6E
    VOICE_GEN_SOFYBU  = 111, // 0x6F
    VOICE_GEN_SOFYRI  = 112, // 0x70
    VOICE_GEN_SOFYST  = 113, // 0x71
    VOICE_GEN_SOMOBU  = 114, // 0x72
    VOICE_GEN_SOMORI  = 115, // 0x73
    VOICE_GEN_SOMOST  = 116, // 0x74
    VOICE_GEN_SOMYAP  = 117, // 0x75
    VOICE_GEN_SOMYBU  = 118, // 0x76
    VOICE_GEN_SOMYRI  = 119, // 0x77
    VOICE_GEN_SOMYST  = 120, // 0x78
    VOICE_GEN_SWFOPRO = 121, // 0x79
    VOICE_GEN_SWFORI  = 122, // 0x7A
    VOICE_GEN_SWFOST  = 123, // 0x7B
    VOICE_GEN_SWFYRI  = 124, // 0x7C
    VOICE_GEN_SWFYST  = 125, // 0x7D
    VOICE_GEN_SWFYSTR = 126, // 0x7E
    VOICE_GEN_SWMOCD  = 127, // 0x7F
    VOICE_GEN_SWMORI  = 128, // 0x80
    VOICE_GEN_SWMOST  = 129, // 0x81
    VOICE_GEN_SWMOTR1 = 130, // 0x82
    VOICE_GEN_SWMOTR2 = 131, // 0x83
    VOICE_GEN_SWMOTR3 = 132, // 0x84
    VOICE_GEN_SWMOTR4 = 133, // 0x85
    VOICE_GEN_SWMOTR5 = 134, // 0x86
    VOICE_GEN_SWMYCR  = 135, // 0x87
    VOICE_GEN_SWMYHP1 = 136, // 0x88
    VOICE_GEN_SWMYHP2 = 137, // 0x89
    VOICE_GEN_SWMYRI  = 138, // 0x8A
    VOICE_GEN_SWMYST  = 139, // 0x8B
    VOICE_GEN_VBFYPRO = 140, // 0x8C
    VOICE_GEN_VBFYST2 = 141, // 0x8D
    VOICE_GEN_VBMOCD  = 142, // 0x8E
    VOICE_GEN_VBMYCR  = 143, // 0x8F
    VOICE_GEN_VBMYELV = 144, // 0x90
    VOICE_GEN_VHFYPRO = 145, // 0x91
    VOICE_GEN_VHFYST3 = 146, // 0x92
    VOICE_GEN_VHMYCR  = 147, // 0x93
    VOICE_GEN_VHMYELV = 148, // 0x94
    VOICE_GEN_VIMYELV = 149, // 0x95
    VOICE_GEN_VWFYPRO = 150, // 0x96
    VOICE_GEN_VWFYST1 = 151, // 0x97
    VOICE_GEN_VWFYWAI = 152, // 0x98
    VOICE_GEN_VWMOTR1 = 153, // 0x99
    VOICE_GEN_VWMOTR2 = 154, // 0x9A
    VOICE_GEN_VWMYAP  = 155, // 0x9B
    VOICE_GEN_VWMYBJD = 156, // 0x9C
    VOICE_GEN_VWMYCD  = 157, // 0x9D
    VOICE_GEN_VWMYCR  = 158, // 0x9E
    VOICE_GEN_WFOPJ   = 159, // 0x9F
    VOICE_GEN_WFORI   = 160, // 0xA0
    VOICE_GEN_WFOST   = 161, // 0xA1
    VOICE_GEN_WFYBE   = 162, // 0xA2
    VOICE_GEN_WFYBU   = 163, // 0xA3
    VOICE_GEN_WFYCRK  = 164, // 0xA4
    VOICE_GEN_WFYCRP  = 165, // 0xA5
    VOICE_GEN_WFYJG   = 166, // 0xA6
    VOICE_GEN_WFYLG   = 167, // 0xA7
    VOICE_GEN_WFYPRO  = 168, // 0xA8
    VOICE_GEN_WFYRI   = 169, // 0xA9
    VOICE_GEN_WFYRO   = 170, // 0xAA
    VOICE_GEN_WFYST   = 171, // 0xAB
    VOICE_GEN_WFYSTEW = 172, // 0xAC
    VOICE_GEN_WMOMIB  = 173, // 0xAD
    VOICE_GEN_WMOPJ   = 174, // 0xAE
    VOICE_GEN_WMOPREA = 175, // 0xAF
    VOICE_GEN_WMORI   = 176, // 0xB0
    VOICE_GEN_WMOSCI  = 177, // 0xB1
    VOICE_GEN_WMOST   = 178, // 0xB2
    VOICE_GEN_WMOTR1  = 179, // 0xB3
    VOICE_GEN_WMYBE   = 180, // 0xB4
    VOICE_GEN_WMYBMX  = 181, // 0xB5
    VOICE_GEN_WMYBOUN = 182, // 0xB6
    VOICE_GEN_WMYBOX  = 183, // 0xB7
    VOICE_GEN_WMYBP   = 184, // 0xB8
    VOICE_GEN_WMYBU   = 185, // 0xB9
    VOICE_GEN_WMYCD1  = 186, // 0xBA
    VOICE_GEN_WMYCD2  = 187, // 0xBB
    VOICE_GEN_WMYCH   = 188, // 0xBC
    VOICE_GEN_WMYCON  = 189, // 0xBD
    VOICE_GEN_WMYCONB = 190, // 0xBE
    VOICE_GEN_WMYCR   = 191, // 0xBF
    VOICE_GEN_WMYDRUG = 192, // 0xC0
    VOICE_GEN_WMYGAR  = 193, // 0xC1
    VOICE_GEN_WMYGOL1 = 194, // 0xC2
    VOICE_GEN_WMYGOL2 = 195, // 0xC3
    VOICE_GEN_WMYJG   = 196, // 0xC4
    VOICE_GEN_WMYLG   = 197, // 0xC5
    VOICE_GEN_WMYMECH = 198, // 0xC6
    VOICE_GEN_WMYMOUN = 199, // 0xC7
    VOICE_GEN_WMYPLT  = 200, // 0xC8
    VOICE_GEN_WMYRI   = 201, // 0xC9
    VOICE_GEN_WMYRO   = 202, // 0xCA
    VOICE_GEN_WMYSGRD = 203, // 0xCB
    VOICE_GEN_WMYSKAT = 204, // 0xCC
    VOICE_GEN_WMYST   = 205, // 0xCD
    VOICE_GEN_WMYTX1  = 206, // 0xCE
    VOICE_GEN_WMYTX2  = 207, // 0xCF
    VOICE_GEN_WMYVA   = 208, // 0xD0
    VOICE_GEN_END     = 209, // 0xD1
};

/*!
 * @brief Emergency voices (For `PED_TYPE_EMG`)
 */
enum eEmgSpeechVoices : int16 {
    VOICE_EMG_ARMY1   = 0,  // 0x0
    VOICE_EMG_ARMY2   = 1,  // 0x1
    VOICE_EMG_ARMY3   = 2,  // 0x2
    VOICE_EMG_EMT1    = 3,  // 0x3
    VOICE_EMG_EMT2    = 4,  // 0x4
    VOICE_EMG_EMT3    = 5,  // 0x5
    VOICE_EMG_EMT4    = 6,  // 0x6
    VOICE_EMG_EMT5    = 7,  // 0x7
    VOICE_EMG_FBI2    = 8,  // 0x8
    VOICE_EMG_FBI3    = 9,  // 0x9
    VOICE_EMG_FBI4    = 10, // 0xA
    VOICE_EMG_FBI5    = 11, // 0xB
    VOICE_EMG_FBI6    = 12, // 0xC
    VOICE_EMG_LAPD1   = 13, // 0xD
    VOICE_EMG_LAPD2   = 14, // 0xE
    VOICE_EMG_LAPD3   = 15, // 0xF
    VOICE_EMG_LAPD4   = 16, // 0x10
    VOICE_EMG_LAPD5   = 17, // 0x11
    VOICE_EMG_LAPD6   = 18, // 0x12
    VOICE_EMG_LAPD7   = 19, // 0x13
    VOICE_EMG_LAPD8   = 20, // 0x14
    VOICE_EMG_LVPD1   = 21, // 0x15
    VOICE_EMG_LVPD2   = 22, // 0x16
    VOICE_EMG_LVPD3   = 23, // 0x17
    VOICE_EMG_LVPD4   = 24, // 0x18
    VOICE_EMG_LVPD5   = 25, // 0x19
    VOICE_EMG_MCOP1   = 26, // 0x1A
    VOICE_EMG_MCOP2   = 27, // 0x1B
    VOICE_EMG_MCOP3   = 28, // 0x1C
    VOICE_EMG_MCOP4   = 29, // 0x1D
    VOICE_EMG_MCOP5   = 30, // 0x1E
    VOICE_EMG_MCOP6   = 31, // 0x1F
    VOICE_EMG_PULASKI = 32, // 0x20
    VOICE_EMG_RCOP1   = 33, // 0x21
    VOICE_EMG_RCOP2   = 34, // 0x22
    VOICE_EMG_RCOP3   = 35, // 0x23
    VOICE_EMG_RCOP4   = 36, // 0x24
    VOICE_EMG_SFPD1   = 37, // 0x25
    VOICE_EMG_SFPD2   = 38, // 0x26
    VOICE_EMG_SFPD3   = 39, // 0x27
    VOICE_EMG_SFPD4   = 40, // 0x28
    VOICE_EMG_SFPD5   = 41, // 0x29
    VOICE_EMG_SWAT1   = 42, // 0x2A
    VOICE_EMG_SWAT2   = 43, // 0x2B
    VOICE_EMG_SWAT4   = 44, // 0x2C
    VOICE_EMG_SWAT6   = 45, // 0x2D
    VOICE_EMG_END     = 46, // 0x2E
};

/*!
 * @brief Player voices (For `PED_TYPE_PLAYER`)
 */
enum ePlySpeechVoices : int16 {
    VOICE_PLY_AG  = 0,  // 0x0
    VOICE_PLY_AG2 = 1,  // 0x1

    VOICE_PLY_AR  = 2,  // 0x2
    VOICE_PLY_AR2 = 3,  // 0x3

    VOICE_PLY_CD  = 4,  // 0x4
    VOICE_PLY_CD2 = 5,  // 0x5

    VOICE_PLY_CF  = 6,  // 0x6
    VOICE_PLY_CF2 = 7,  // 0x7

    VOICE_PLY_CG  = 8,  // 0x8
    VOICE_PLY_CG2 = 9,  // 0x9

    VOICE_PLY_CR  = 10, // 0xA
    VOICE_PLY_CR2 = 11, // 0xB

    VOICE_PLY_PG  = 12, // 0xC
    VOICE_PLY_PG2 = 13, // 0xD

    VOICE_PLY_PR  = 14, // 0xE
    VOICE_PLY_PR2 = 15, // 0xF

    VOICE_PLY_WG  = 16, // 0x10
    VOICE_PLY_WG2 = 17, // 0x11

    VOICE_PLY_WR  = 18, // 0x12
    VOICE_PLY_WR2 = 19, // 0x13

    VOICE_PLY_END = 20, // 0x14
};

/*!
 * @brief Gang voices (For `PED_TYPE_GANG`)
 */
enum eGngSpeechVoices : int16 {
    VOICE_GNG_BALLAS1  = 0,  // 0x0
    VOICE_GNG_BALLAS2  = 1,  // 0x1
    VOICE_GNG_BALLAS3  = 2,  // 0x2
    VOICE_GNG_BALLAS4  = 3,  // 0x3
    VOICE_GNG_BALLAS5  = 4,  // 0x4
    VOICE_GNG_BIG_BEAR = 5,  // 0x5
    VOICE_GNG_CESAR    = 6,  // 0x6
    VOICE_GNG_DNB1     = 7,  // 0x7
    VOICE_GNG_DNB2     = 8,  // 0x8
    VOICE_GNG_DNB3     = 9,  // 0x9
    VOICE_GNG_DNB5     = 10, // 0xA
    VOICE_GNG_DWAINE   = 11, // 0xB
    VOICE_GNG_FAM1     = 12, // 0xC
    VOICE_GNG_FAM2     = 13, // 0xD
    VOICE_GNG_FAM3     = 14, // 0xE
    VOICE_GNG_FAM4     = 15, // 0xF
    VOICE_GNG_FAM5     = 16, // 0x10
    VOICE_GNG_JIZZY    = 17, // 0x11
    VOICE_GNG_LSV1     = 18, // 0x12
    VOICE_GNG_LSV2     = 19, // 0x13
    VOICE_GNG_LSV3     = 20, // 0x14
    VOICE_GNG_LSV4     = 21, // 0x15
    VOICE_GNG_LSV5     = 22, // 0x16
    VOICE_GNG_MACCER   = 23, // 0x17
    VOICE_GNG_MAFBOSS  = 24, // 0x18
    VOICE_GNG_OGLOC    = 25, // 0x19
    VOICE_GNG_RYDER    = 26, // 0x1A
    VOICE_GNG_SFR1     = 27, // 0x1B
    VOICE_GNG_SFR2     = 28, // 0x1C
    VOICE_GNG_SFR3     = 29, // 0x1D
    VOICE_GNG_SFR4     = 30, // 0x1E
    VOICE_GNG_SFR5     = 31, // 0x1F
    VOICE_GNG_SMOKE    = 32, // 0x20
    VOICE_GNG_STRI1    = 33, // 0x21
    VOICE_GNG_STRI2    = 34, // 0x22
    VOICE_GNG_STRI4    = 35, // 0x23
    VOICE_GNG_STRI5    = 36, // 0x24
    VOICE_GNG_SWEET    = 37, // 0x25
    VOICE_GNG_TBONE    = 38, // 0x26
    VOICE_GNG_TORENO   = 39, // 0x27
    VOICE_GNG_TRUTH    = 40, // 0x28
    VOICE_GNG_VLA1     = 41, // 0x29
    VOICE_GNG_VLA2     = 42, // 0x2A
    VOICE_GNG_VLA3     = 43, // 0x2B
    VOICE_GNG_VLA4     = 44, // 0x2C
    VOICE_GNG_VLA5     = 45, // 0x2D
    VOICE_GNG_VMAFF1   = 46, // 0x2E
    VOICE_GNG_VMAFF2   = 47, // 0x2F
    VOICE_GNG_VMAFF3   = 48, // 0x30
    VOICE_GNG_VMAFF4   = 49, // 0x31
    VOICE_GNG_VMAFF5   = 50, // 0x32
    VOICE_GNG_WOOZIE   = 51, // 0x33
    VOICE_GNG_END      = 52, // 0x34
};

/*!
 * @brief Girlfriend voices (For `PED_TYPE_GFD`)
 */
enum eGfdSpeechVoices : int16 {
    VOICE_GFD_BARBARA  = 0,  // 0x0
    VOICE_GFD_BMOBAR   = 1,  // 0x1
    VOICE_GFD_BMYBARB  = 2,  // 0x2
    VOICE_GFD_BMYTATT  = 3,  // 0x3
    VOICE_GFD_CATALINA = 4,  // 0x4
    VOICE_GFD_DENISE   = 5,  // 0x5
    VOICE_GFD_HELENA   = 6,  // 0x6
    VOICE_GFD_KATIE    = 7,  // 0x7
    VOICE_GFD_MICHELLE = 8,  // 0x8
    VOICE_GFD_MILLIE   = 9,  // 0x9
    VOICE_GFD_POL_ANN  = 10, // 0xA
    VOICE_GFD_WFYBURG  = 11, // 0xB
    VOICE_GFD_WFYCLOT  = 12, // 0xC
    VOICE_GFD_WMYAMMO  = 13, // 0xD
    VOICE_GFD_WMYBARB  = 14, // 0xE
    VOICE_GFD_WMYBELL  = 15, // 0xF
    VOICE_GFD_WMYCLOT  = 16, // 0x10
    VOICE_GFD_WMYPIZZ  = 17, // 0x11
    VOICE_GFD_END      = 18, // 0x12
};

/*!
 * @brief Pain voices
 */
enum ePainSpeechVoices : int16 {
    VOICE_PAIN_NON_BINARY = -1, // notsa
    VOICE_PAIN_CARL       = 0,  // 0x0
    VOICE_PAIN_FEMALE     = 1,  // 0x1
    VOICE_PAIN_MALE       = 2,  // 0x2
    VOICE_PAIN_END        = 3,  // 0x3
};

enum {
    VOICE_UNK = -1
};

/*! 
 * @brief Use any of the enums below for this type:
 * @brief `eGenSpeechVoices`, `eEmgSpeechVoices`, `ePlySpeechVoices`,
 * @brief `eGngSpeechVoices`, `eGfdSpeechVoices`, `ePainSpeechVoices` 
 */
using ePedSpeechVoiceS16 = int16;
using ePedSpeechVoiceS32 = int32;
