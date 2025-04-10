#include "StdInc.h"

#include "SurfaceInfos_c.h"
#include "Skidmark.h"

void SurfaceInfos_c::InjectHooks()
{
    RH_ScopedClass(SurfaceInfos_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetSurfaceIdFromName, 0x55D220);
    RH_ScopedInstall(Init, 0x55F420);

    RH_ScopedInstall(GetAdhesionGroup, 0x55E5C0);
    RH_ScopedInstall(GetTyreGrip, 0x55E5E0);
    RH_ScopedInstall(GetWetMultiplier, 0x55E600);
    RH_ScopedInstall(GetSkidmarkType, 0x55E630);
    RH_ScopedInstall(GetFrictionEffect, 0x55E650);
    RH_ScopedInstall(GetBulletFx, 0x55E670);
    RH_ScopedInstall(IsSoftLanding, 0x55E690);
    RH_ScopedInstall(IsSeeThrough, 0x55E6B0);
    RH_ScopedInstall(IsShootThrough, 0x55E6D0);
    RH_ScopedInstall(IsSand, 0x55E6F0);
    RH_ScopedInstall(IsWater, 0x55E710);
    RH_ScopedInstall(IsShallowWater, 0x55E730);
    RH_ScopedInstall(IsBeach, 0x55E750);
    RH_ScopedInstall(IsSteepSlope, 0x55E770);
    RH_ScopedInstall(IsGlass, 0x55E790);
    RH_ScopedInstall(IsStairs, 0x55E7B0);
    RH_ScopedInstall(IsSkateable, 0x55E7D0);
    RH_ScopedInstall(IsPavement, 0x55E7F0);
    RH_ScopedInstall(GetRoughness, 0x55E810);
    RH_ScopedInstall(GetFlammability, 0x55E830);
    RH_ScopedInstall(CreatesSparks, 0x55E850);
    RH_ScopedInstall(CantSprintOn, 0x55E870);
    RH_ScopedInstall(LeavesFootsteps, 0x55E890);
    RH_ScopedInstall(ProducesFootDust, 0x55E8B0);
    RH_ScopedInstall(MakesCarDirty, 0x55E8D0);
    RH_ScopedInstall(MakesCarClean, 0x55E8F0);
    RH_ScopedInstall(CreatesWheelGrass, 0x55E910);
    RH_ScopedInstall(CreatesWheelGravel, 0x55E930);
    RH_ScopedInstall(CreatesWheelMud, 0x55E950);
    RH_ScopedInstall(CreatesWheelDust, 0x55E970);
    RH_ScopedInstall(CreatesWheelSand, 0x55E990);
    RH_ScopedInstall(CreatesWheelSpray, 0x55E9B0);
    RH_ScopedInstall(CreatesPlants, 0x55E9D0);
    RH_ScopedInstall(CreatesObjects, 0x55E9F0);
    RH_ScopedInstall(CanClimb, 0x55EA10);
    RH_ScopedInstall(IsAudioConcrete, 0x55EA30);
    RH_ScopedInstall(IsAudioGrass, 0x55EA50);
    RH_ScopedInstall(IsAudioSand, 0x55EA70);
    RH_ScopedInstall(IsAudioGravel, 0x55EA90);
    RH_ScopedInstall(IsAudioWood, 0x55EAB0);
    RH_ScopedInstall(IsAudioWater, 0x55EAD0);
    RH_ScopedInstall(IsAudioMetal, 0x55EAF0);
    RH_ScopedInstall(IsAudioLongGrass, 0x55EB10);
    RH_ScopedInstall(IsAudioTile, 0x55EB30);
    RH_ScopedInstall(GetAdhesiveLimit, 0x55EB50);
}

// 0x55D220
SurfaceId SurfaceInfos_c::GetSurfaceIdFromName(Const char* cName)
{
    static constexpr struct { const char* name; const eSurfaceType type; } mapping[] = {
        { cDefaultName,             SURFACE_DEFAULT                },
        { "TARMAC",                 SURFACE_TARMAC                 },
        { "TARMAC_FUCKED",          SURFACE_TARMAC_FUCKED          },
        { "TARMAC_REALLYFUCKED",    SURFACE_TARMAC_REALLYFUCKED    },
        { "PAVEMENT",               SURFACE_PAVEMENT               },
        { "PAVEMENT_FUCKED",        SURFACE_PAVEMENT_FUCKED        },
        { "GRAVEL",                 SURFACE_GRAVEL                 },
        { "FUCKED_CONCRETE",        SURFACE_FUCKED_CONCRETE        },
        { "PAINTED_GROUND",         SURFACE_PAINTED_GROUND         },
        { "GRASS_SHORT_LUSH",       SURFACE_GRASS_SHORT_LUSH       },
        { "GRASS_MEDIUM_LUSH",      SURFACE_GRASS_MEDIUM_LUSH      },
        { "GRASS_LONG_LUSH",        SURFACE_GRASS_LONG_LUSH        },
        { "GRASS_SHORT_DRY",        SURFACE_GRASS_SHORT_DRY        },
        { "GRASS_MEDIUM_DRY",       SURFACE_GRASS_MEDIUM_DRY       },
        { "GRASS_LONG_DRY",         SURFACE_GRASS_LONG_DRY         },
        { "GOLFGRASS_ROUGH",        SURFACE_GOLFGRASS_ROUGH        },
        { "GOLFGRASS_SMOOTH",       SURFACE_GOLFGRASS_SMOOTH       },
        { "STEEP_SLIDYGRASS",       SURFACE_STEEP_SLIDYGRASS       },
        { "STEEP_CLIFF",            SURFACE_STEEP_CLIFF            },
        { "FLOWERBED",              SURFACE_FLOWERBED              },
        { "MEADOW",                 SURFACE_MEADOW                 },
        { "WASTEGROUND",            SURFACE_WASTEGROUND            },
        { "WOODLANDGROUND",         SURFACE_WOODLANDGROUND         },
        { "VEGETATION",             SURFACE_VEGETATION             },
        { "MUD_WET",                SURFACE_MUD_WET                },
        { "MUD_DRY",                SURFACE_MUD_DRY                },
        { "DIRT",                   SURFACE_DIRT                   },
        { "DIRTTRACK",              SURFACE_DIRTTRACK              },
        { "SAND_DEEP",              SURFACE_SAND_DEEP              },
        { "SAND_MEDIUM",            SURFACE_SAND_MEDIUM            },
        { "SAND_COMPACT",           SURFACE_SAND_COMPACT           },
        { "SAND_ARID",              SURFACE_SAND_ARID              },
        { "SAND_MORE",              SURFACE_SAND_MORE              },
        { "SAND_BEACH",             SURFACE_SAND_BEACH             },
        { "CONCRETE_BEACH",         SURFACE_CONCRETE_BEACH         },
        { "ROCK_DRY",               SURFACE_ROCK_DRY               },
        { "ROCK_WET",               SURFACE_ROCK_WET               },
        { "ROCK_CLIFF",             SURFACE_ROCK_CLIFF             },
        { "WATER_RIVERBED",         SURFACE_WATER_RIVERBED         },
        { "WATER_SHALLOW",          SURFACE_WATER_SHALLOW          },
        { "CORNFIELD",              SURFACE_CORNFIELD              },
        { "HEDGE",                  SURFACE_HEDGE                  },
        { "WOOD_CRATES",            SURFACE_WOOD_CRATES            },
        { "WOOD_SOLID",             SURFACE_WOOD_SOLID             },
        { "WOOD_THIN",              SURFACE_WOOD_THIN              },
        { "GLASS",                  SURFACE_GLASS                  },
        { "GLASS_WINDOWS_LARGE",    SURFACE_GLASS_WINDOWS_LARGE    },
        { "GLASS_WINDOWS_SMALL",    SURFACE_GLASS_WINDOWS_SMALL    },
        { "EMPTY1",                 SURFACE_EMPTY1                 },
        { "EMPTY2",                 SURFACE_EMPTY2                 },
        { "GARAGE_DOOR",            SURFACE_GARAGE_DOOR            },
        { "THICK_METAL_PLATE",      SURFACE_THICK_METAL_PLATE      },
        { "SCAFFOLD_POLE",          SURFACE_SCAFFOLD_POLE          },
        { "LAMP_POST",              SURFACE_LAMP_POST              },
        { "METAL_GATE",             SURFACE_METAL_GATE             },
        { "METAL_CHAIN_FENCE",      SURFACE_METAL_CHAIN_FENCE      },
        { "GIRDER",                 SURFACE_GIRDER                 },
        { "FIRE_HYDRANT",           SURFACE_FIRE_HYDRANT           },
        { "CONTAINER",              SURFACE_CONTAINER              },
        { "NEWS_VENDOR",            SURFACE_NEWS_VENDOR            },
        { "WHEELBASE",              SURFACE_WHEELBASE              },
        { "CARDBOARDBOX",           SURFACE_CARDBOARDBOX           },
        { "PED",                    SURFACE_PED                    },
        { "CAR",                    SURFACE_CAR                    },
        { "CAR_PANEL",              SURFACE_CAR_PANEL              },
        { "CAR_MOVINGCOMPONENT",    SURFACE_CAR_MOVINGCOMPONENT    },
        { "TRANSPARENT_CLOTH",      SURFACE_TRANSPARENT_CLOTH      },
        { "RUBBER",                 SURFACE_RUBBER                 },
        { "PLASTIC",                SURFACE_PLASTIC                },
        { "TRANSPARENT_STONE",      SURFACE_TRANSPARENT_STONE      },
        { "WOOD_BENCH",             SURFACE_WOOD_BENCH             },
        { "CARPET",                 SURFACE_CARPET                 },
        { "FLOORBOARD",             SURFACE_FLOORBOARD             },
        { "STAIRSWOOD",             SURFACE_STAIRSWOOD             },
        { "P_SAND",                 SURFACE_P_SAND                 },
        { "P_SAND_DENSE",           SURFACE_P_SAND_DENSE           },
        { "P_SAND_ARID",            SURFACE_P_SAND_ARID            },
        { "P_SAND_COMPACT",         SURFACE_P_SAND_COMPACT         },
        { "P_SAND_ROCKY",           SURFACE_P_SAND_ROCKY           },
        { "P_SANDBEACH",            SURFACE_P_SANDBEACH            },
        { "P_GRASS_SHORT",          SURFACE_P_GRASS_SHORT          },
        { "P_GRASS_MEADOW",         SURFACE_P_GRASS_MEADOW         },
        { "P_GRASS_DRY",            SURFACE_P_GRASS_DRY            },
        { "P_WOODLAND",             SURFACE_P_WOODLAND             },
        { "P_WOODDENSE",            SURFACE_P_WOODDENSE            },
        { "P_ROADSIDE",             SURFACE_P_ROADSIDE             },
        { "P_ROADSIDEDES",          SURFACE_P_ROADSIDEDES          },
        { "P_FLOWERBED",            SURFACE_P_FLOWERBED            },
        { "P_WASTEGROUND",          SURFACE_P_WASTEGROUND          },
        { "P_CONCRETE",             SURFACE_P_CONCRETE             },
        { "P_OFFICEDESK",           SURFACE_P_OFFICEDESK           },
        { "P_711SHELF1",            SURFACE_P_711SHELF1            },
        { "P_711SHELF2",            SURFACE_P_711SHELF2            },
        { "P_711SHELF3",            SURFACE_P_711SHELF3            },
        { "P_RESTUARANTTABLE",      SURFACE_P_RESTUARANTTABLE      },
        { "P_BARTABLE",             SURFACE_P_BARTABLE             },
        { "P_UNDERWATERLUSH",       SURFACE_P_UNDERWATERLUSH       },
        { "P_UNDERWATERBARREN",     SURFACE_P_UNDERWATERBARREN     },
        { "P_UNDERWATERCORAL",      SURFACE_P_UNDERWATERCORAL      },
        { "P_UNDERWATERDEEP",       SURFACE_P_UNDERWATERDEEP       },
        { "P_RIVERBED",             SURFACE_P_RIVERBED             },
        { "P_RUBBLE",               SURFACE_P_RUBBLE               },
        { "P_BEDROOMFLOOR",         SURFACE_P_BEDROOMFLOOR         },
        { "P_KIRCHENFLOOR",         SURFACE_P_KIRCHENFLOOR         },
        { "P_LIVINGRMFLOOR",        SURFACE_P_LIVINGRMFLOOR        },
        { "P_CORRIDORFLOOR",        SURFACE_P_CORRIDORFLOOR        },
        { "P_711FLOOR",             SURFACE_P_711FLOOR             },
        { "P_FASTFOODFLOOR",        SURFACE_P_FASTFOODFLOOR        },
        { "P_SKANKYFLOOR",          SURFACE_P_SKANKYFLOOR          },
        { "P_MOUNTAIN",             SURFACE_P_MOUNTAIN             },
        { "P_MARSH",                SURFACE_P_MARSH                },
        { "P_BUSHY",                SURFACE_P_BUSHY                },
        { "P_BUSHYMIX",             SURFACE_P_BUSHYMIX             },
        { "P_BUSHYDRY",             SURFACE_P_BUSHYDRY             },
        { "P_BUSHYMID",             SURFACE_P_BUSHYMID             },
        { "P_GRASSWEEFLOWERS",      SURFACE_P_GRASSWEEFLOWERS      },
        { "P_GRASSDRYTALL",         SURFACE_P_GRASSDRYTALL         },
        { "P_GRASSLUSHTALL",        SURFACE_P_GRASSLUSHTALL        },
        { "P_GRASSGRNMIX",          SURFACE_P_GRASSGRNMIX          },
        { "P_GRASSBRNMIX",          SURFACE_P_GRASSBRNMIX          },
        { "P_GRASSLOW",             SURFACE_P_GRASSLOW             },
        { "P_GRASSROCKY",           SURFACE_P_GRASSROCKY           },
        { "P_GRASSSMALLTREES",      SURFACE_P_GRASSSMALLTREES      },
        { "P_DIRTROCKY",            SURFACE_P_DIRTROCKY            },
        { "P_DIRTWEEDS",            SURFACE_P_DIRTWEEDS            },
        { "P_GRASSWEEDS",           SURFACE_P_GRASSWEEDS           },
        { "P_RIVEREDGE",            SURFACE_P_RIVEREDGE            },
        { "P_POOLSIDE",             SURFACE_P_POOLSIDE             },
        { "P_FORESTSTUMPS",         SURFACE_P_FORESTSTUMPS         },
        { "P_FORESTSTICKS",         SURFACE_P_FORESTSTICKS         },
        { "P_FORRESTLEAVES",        SURFACE_P_FORRESTLEAVES        },
        { "P_DESERTROCKS",          SURFACE_P_DESERTROCKS          },
        { "P_FORRESTDRY",           SURFACE_P_FORRESTDRY           },
        { "P_SPARSEFLOWERS",        SURFACE_P_SPARSEFLOWERS        },
        { "P_BUILDINGSITE",         SURFACE_P_BUILDINGSITE         },
        { "P_DOCKLANDS",            SURFACE_P_DOCKLANDS            },
        { "P_INDUSTRIAL",           SURFACE_P_INDUSTRIAL           },
        { "P_INDUSTJETTY",          SURFACE_P_INDUSTJETTY          },
        { "P_CONCRETELITTER",       SURFACE_P_CONCRETELITTER       },
        { "P_ALLEYRUBISH",          SURFACE_P_ALLEYRUBISH          },
        { "P_JUNKYARDPILES",        SURFACE_P_JUNKYARDPILES        },
        { "P_JUNKYARDGRND",         SURFACE_P_JUNKYARDGRND         },
        { "P_DUMP",                 SURFACE_P_DUMP                 },
        { "P_CACTUSDENSE",          SURFACE_P_CACTUSDENSE          },
        { "P_AIRPORTGRND",          SURFACE_P_AIRPORTGRND          },
        { "P_CORNFIELD",            SURFACE_P_CORNFIELD            },
        { "P_GRASSLIGHT",           SURFACE_P_GRASSLIGHT           },
        { "P_GRASSLIGHTER",         SURFACE_P_GRASSLIGHTER         },
        { "P_GRASSLIGHTER2",        SURFACE_P_GRASSLIGHTER2        },
        { "P_GRASSMID1",            SURFACE_P_GRASSMID1            },
        { "P_GRASSMID2",            SURFACE_P_GRASSMID2            },
        { "P_GRASSDARK",            SURFACE_P_GRASSDARK            },
        { "P_GRASSDARK2",           SURFACE_P_GRASSDARK2           },
        { "P_GRASSDIRTMIX",         SURFACE_P_GRASSDIRTMIX         },
        { "P_RIVERBEDSTONE",        SURFACE_P_RIVERBEDSTONE        },
        { "P_RIVERBEDSHALLOW",      SURFACE_P_RIVERBEDSHALLOW      },
        { "P_RIVERBEDWEEDS",        SURFACE_P_RIVERBEDWEEDS        },
        { "P_SEAWEED",              SURFACE_P_SEAWEED              },
        { "DOOR",                   SURFACE_DOOR                   },
        { "PLASTICBARRIER",         SURFACE_PLASTICBARRIER         },
        { "PARKGRASS",              SURFACE_PARKGRASS              },
        { "STAIRSSTONE",            SURFACE_STAIRSSTONE            },
        { "STAIRSMETAL",            SURFACE_STAIRSMETAL            },
        { "STAIRSCARPET",           SURFACE_STAIRSCARPET           },
        { "FLOORMETAL",             SURFACE_FLOORMETAL             },
        { "FLOORCONCRETE",          SURFACE_FLOORCONCRETE          },
        { "BIN_BAG",                SURFACE_BIN_BAG                },
        { "THIN_METAL_SHEET",       SURFACE_THIN_METAL_SHEET       },
        { "METAL_BARREL",           SURFACE_METAL_BARREL           },
        { "PLASTIC_CONE",           SURFACE_PLASTIC_CONE           },
        { "PLASTIC_DUMPSTER",       SURFACE_PLASTIC_DUMPSTER       },
        { "METAL_DUMPSTER",         SURFACE_METAL_DUMPSTER         },
        { "WOOD_PICKET_FENCE",      SURFACE_WOOD_PICKET_FENCE      },
        { "WOOD_SLATTED_FENCE",     SURFACE_WOOD_SLATTED_FENCE     },
        { "WOOD_RANCH_FENCE",       SURFACE_WOOD_RANCH_FENCE       },
        { "UNBREAKABLE_GLASS",      SURFACE_UNBREAKABLE_GLASS      },
        { "HAY_BALE",               SURFACE_HAY_BALE               },
        { "GORE",                   SURFACE_GORE                   },
        { "RAILTRACK",              SURFACE_RAILTRACK              }
    };
    for (const auto& [name, type] : mapping) {
        if (strcmp(cName, name) == 0)
            return type;
    }
    return SURFACE_DEFAULT;
}

// 0x55D0E0
void SurfaceInfos_c::LoadAdhesiveLimits()
{
    return plugin::CallMethod<0x55D0E0, SurfaceInfos_c*>(this);

    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("data\\surface.dat", "rb");
#if FIX_BUGS
    if (!file) {
        DEV_LOG("[SurfaceInfos_c] Failed to open surface.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == ';' || !*line)
            continue;

        char value[4];
        VERIFY(sscanf_s(line, "%s", SCANF_S_STR(value)) == 1);
        for (auto i = *line; i != ' '; i = *++line) {
            if (i == '\t')
                break;
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x55EB90
void SurfaceInfos_c::LoadSurfaceInfos()
{
    auto* file = CFileMgr::OpenFile("data\\surfinfo.dat", "r");
#if FIX_BUGS
    if (!file) {
        DEV_LOG("[SurfaceInfos_c] Failed to open surfinfo.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        SurfaceInfo si{};
        si.Read(line);

        auto id = SurfaceInfos_c::GetSurfaceIdFromName(si.SurfaceName);
        auto& surface = m_surfaces[id];

        if      (!strcmp(si.AdhesionGroup, "RUBBER")) surface.ucAdhesionGroup = ADHESION_GROUP_RUBBER;
        else if (!strcmp(si.AdhesionGroup, "HARD"))   surface.ucAdhesionGroup = ADHESION_GROUP_HARD;
        else if (!strcmp(si.AdhesionGroup, "ROAD"))   surface.ucAdhesionGroup = ADHESION_GROUP_ROAD;
        else if (!strcmp(si.AdhesionGroup, "LOOSE"))  surface.ucAdhesionGroup = ADHESION_GROUP_LOOSE;
        else if (!strcmp(si.AdhesionGroup, "SAND"))   surface.ucAdhesionGroup = ADHESION_GROUP_SAND;
        else if (!strcmp(si.AdhesionGroup, "WET"))    surface.ucAdhesionGroup = ADHESION_GROUP_WET;

        surface.tyreGrip = uint32(si.TyreGrip * 10.0f);
        surface.wetGrip  = uint32(si.WetGrip  * 100.0f); // BUG:? Data has negative values, with *original* conversion we loose sign

        if (!strcmp(si.SkidMark, "DEFAULT")) surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::DEFAULT);
        if (!strcmp(si.SkidMark, "SANDY"))   surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::SANDY);
        if (!strcmp(si.SkidMark, "MUDDY"))   surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::MUDDY);

        if      (!strcmp(si.FrictionEffect, "NONE"))   surface.ucFrictionEffect = FRICTION_EFFECT_NONE;
        else if (!strcmp(si.FrictionEffect, "SPARKS")) surface.ucFrictionEffect = FRICTION_EFFECT_SPARKS;

        if      (!strcmp(si.BulletFx, "NONE"))   surface.ucBulletFX = BULLET_FX_NONE;
        else if (!strcmp(si.BulletFx, "SPARKS")) surface.ucBulletFX = BULLET_FX_SPARKS;
        else if (!strcmp(si.BulletFx, "SAND"))   surface.ucBulletFX = BULLET_FX_SAND;
        else if (!strcmp(si.BulletFx, "WOOD"))   surface.ucBulletFX = BULLET_FX_WOOD;
        else if (!strcmp(si.BulletFx, "DUST"))   surface.ucBulletFX = BULLET_FX_DUST;

        surface.bIsSoftLand         = (bool)si.SoftLand;
        surface.bIsSeeThrough       = (bool)si.SeeThrough;
        surface.bIsShootThrough     = (bool)si.ShootThrough;
        surface.bIsSand             = (bool)si.Sand;
        surface.bIsWater            = (bool)si.Water;
        surface.bIsShallowWater     = (bool)si.ShallowWater;
        surface.bIsBeach            = (bool)si.Beach;
        surface.bIsSteepSlope       = (bool)si.SteepSlope;
        surface.bIsGlass            = (bool)si.Glass;
        surface.bIsStairs           = (bool)si.Stairs;
        surface.bIsSkateable        = (bool)si.Skateable;
        surface.bIsPavement         = (bool)si.Pavement;
        surface.ucRoughness         = (uint32)si.Roughness;
        surface.ucFlammability      = (uint32)si.Flame;
        surface.bCreatesSparks      = (bool)si.Sparks;
        surface.bCantSprintOn       = (bool)si.Sprint;
        surface.bLeavesFootsteps    = (bool)si.Footsteps;
        surface.bProducesFootDust   = (bool)si.FootDust;
        surface.bMakesCarDirty      = (bool)si.CarDirt;
        surface.bMakesCarClean      = (bool)si.CarClean;
        surface.bCreatesWheelGrass  = (bool)si.WheelGrass;
        surface.bCreatesWheelGravel = (bool)si.WheelGravel;
        surface.bCreatesWheelMud    = (bool)si.WheelMud;
        surface.bCreatesWheelDust   = (bool)si.WheelDust;
        surface.bCreatesWheelSand   = (bool)si.WheelSand;
        surface.bCreatesWheelSpray  = (bool)si.WheelSpray;
        surface.bCreatesPlants      = (bool)si.ProcPlant;
        surface.bCreatesObjects     = (bool)si.ProcObj;
        surface.bCanClimb           = (bool)si.Climbable;
    }
    CFileMgr::CloseFile(file);
}

// 0x55F2B0
void SurfaceInfos_c::LoadSurfaceAudioInfos()
{
    auto* file = CFileMgr::OpenFile("data\\surfaud.dat", "r");
#if FIX_BUGS
    if (!file) {
        DEV_LOG("[SurfaceInfos_c] Failed to open surfaud.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        char  name[64];
        int32 concrete, grass, sand, gravel, wood, water, metal, longGrass, tile;
        VERIFY(sscanf_s(line, "%s %d %d %d %d %d %d %d %d %d", SCANF_S_STR(name), &concrete, &grass, &sand, &gravel, &wood, &water, &metal, &longGrass, &tile) == 10);

        auto id = GetSurfaceIdFromName(name);
        auto& surface = m_surfaces[id];

        surface.bIsAudioConcrete  = (bool)concrete;
        surface.bIsAudioGrass     = (bool)grass;
        surface.bIsAudioSand      = (bool)sand;
        surface.bIsAudioGravel    = (bool)gravel;
        surface.bIsAudioWood      = (bool)wood;
        surface.bIsAudioWater     = (bool)water;
        surface.bIsAudioMetal     = (bool)metal;
        surface.bIsAudioLongGrass = (bool)longGrass;
        surface.bIsAudioTile      = (bool)tile;
    }
    CFileMgr::CloseFile(file);
}

void SurfaceInfos_c::Init()
{
    // They call CFileMgr::SetDir("") only once in LoadAdhesiveLimits
    LoadAdhesiveLimits();
    LoadSurfaceInfos();
    LoadSurfaceAudioInfos();
}

// 0x55E5C0
eAdhesionGroup SurfaceInfos_c::GetAdhesionGroup(SurfaceId id)
{
    return static_cast<eAdhesionGroup>(m_surfaces[id].ucAdhesionGroup);
}

// 0x55E5E0
float SurfaceInfos_c::GetTyreGrip(SurfaceId id)
{
    return static_cast<float>(m_surfaces[id].tyreGrip) * 0.1F;
}

// 0x55E600
float SurfaceInfos_c::GetWetMultiplier(SurfaceId id)
{
    return static_cast<float>(m_surfaces[id].wetGrip) * 0.01F * CWeather::WetRoads + 1.0F;
}

// 0x55E630
uint32 SurfaceInfos_c::GetSkidmarkType(SurfaceId id)
{
    return m_surfaces[id].ucSkidmarkType;
}

// 0x55E650
eFrictionEffect SurfaceInfos_c::GetFrictionEffect(SurfaceId id)
{
    return static_cast<eFrictionEffect>(m_surfaces[id].ucFrictionEffect);
}

// 0x55E670
uint32 SurfaceInfos_c::GetBulletFx(SurfaceId id)
{
    return m_surfaces[id].ucBulletFX;
}

// 0x55E690
bool SurfaceInfos_c::IsSoftLanding(SurfaceId id)
{
    return m_surfaces[id].bIsSoftLand;
}

// 0x55E6B0
bool SurfaceInfos_c::IsSeeThrough(SurfaceId id)
{
    return m_surfaces[id].bIsSeeThrough;
}

// 0x55E6D0
bool SurfaceInfos_c::IsShootThrough(SurfaceId id)
{
    return m_surfaces[id].bIsShootThrough;
}

// 0x55E6F0
bool SurfaceInfos_c::IsSand(SurfaceId id)
{
    return m_surfaces[id].bIsSand;
}

// 0x55E710
bool SurfaceInfos_c::IsWater(SurfaceId id)
{
    return m_surfaces[id].bIsWater;
}

// 0x55E730
bool SurfaceInfos_c::IsShallowWater(SurfaceId id)
{
    return m_surfaces[id].bIsShallowWater;
}

// 0x55E750
bool SurfaceInfos_c::IsBeach(SurfaceId id)
{
    return m_surfaces[id].bIsBeach;
}

// 0x55E770
bool SurfaceInfos_c::IsSteepSlope(SurfaceId id)
{
    return m_surfaces[id].bIsSteepSlope;
}

// 0x55E790
bool SurfaceInfos_c::IsGlass(SurfaceId id)
{
    return m_surfaces[id].bIsGlass;
}

// 0x55E7B0
bool SurfaceInfos_c::IsStairs(SurfaceId id)
{
    return m_surfaces[id].bIsStairs;
}

// 0x55E7D0
bool SurfaceInfos_c::IsSkateable(SurfaceId id)
{
    return m_surfaces[id].bIsSkateable;
}

// 0x55E7F0
bool SurfaceInfos_c::IsPavement(SurfaceId id)
{
    return m_surfaces[id].bIsPavement;
}

// 0x55E810
uint32 SurfaceInfos_c::GetRoughness(SurfaceId id)
{
    return m_surfaces[id].ucRoughness;
}

// 0x55E830
uint32 SurfaceInfos_c::GetFlammability(SurfaceId id)
{
    return m_surfaces[id].ucFlammability;
}

// 0x55E850
bool SurfaceInfos_c::CreatesSparks(SurfaceId id)
{
    return m_surfaces[id].bCreatesSparks;
}

// 0x55E870
bool SurfaceInfos_c::CantSprintOn(SurfaceId id)
{
    return m_surfaces[id].bCantSprintOn;
}

// 0x55E890
bool SurfaceInfos_c::LeavesFootsteps(SurfaceId id)
{
    return m_surfaces[id].bLeavesFootsteps;
}

// 0x55E8B0
bool SurfaceInfos_c::ProducesFootDust(SurfaceId id)
{
    return m_surfaces[id].bProducesFootDust;
}

// 0x55E8D0
bool SurfaceInfos_c::MakesCarDirty(SurfaceId id)
{
    return m_surfaces[id].bMakesCarDirty;
}

// 0x55E8F0
bool SurfaceInfos_c::MakesCarClean(SurfaceId id)
{
    return m_surfaces[id].bMakesCarClean;
}

// 0x55E910
bool SurfaceInfos_c::CreatesWheelGrass(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelGrass;
}

// 0x55E930
bool SurfaceInfos_c::CreatesWheelGravel(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelGravel;
}

// 0x55E950
bool SurfaceInfos_c::CreatesWheelMud(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelMud;
}

// 0x55E970
bool SurfaceInfos_c::CreatesWheelDust(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelDust;
}

// 0x55E990
bool SurfaceInfos_c::CreatesWheelSand(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelSand;
}

// 0x55E9B0
bool SurfaceInfos_c::CreatesWheelSpray(SurfaceId id)
{
    return m_surfaces[id].bCreatesWheelSpray;
}

// 0x55E9D0
bool SurfaceInfos_c::CreatesPlants(SurfaceId id)
{
    return m_surfaces[id].bCreatesPlants;
}

// 0x55E9F0
bool SurfaceInfos_c::CreatesObjects(SurfaceId id)
{
    return m_surfaces[id].bCreatesObjects;
}

// 0x55EA10
bool SurfaceInfos_c::CanClimb(SurfaceId id)
{
    return m_surfaces[id].bCanClimb;
}

// 0x55EA30
bool SurfaceInfos_c::IsAudioConcrete(SurfaceId id)
{
    return m_surfaces[id].bIsAudioConcrete;
}

// 0x55EA50
bool SurfaceInfos_c::IsAudioGrass(SurfaceId id)
{
    return m_surfaces[id].bIsAudioGrass;
}

// 0x55EA70
bool SurfaceInfos_c::IsAudioSand(SurfaceId id)
{
    return m_surfaces[id].bIsAudioSand;
}

// 0x55EA90
bool SurfaceInfos_c::IsAudioGravel(SurfaceId id)
{
    return m_surfaces[id].bIsAudioGravel;
}

// 0x55EAB0
bool SurfaceInfos_c::IsAudioWood(SurfaceId id)
{
    return m_surfaces[id].bIsAudioWood;
}

// 0x55EAD0
bool SurfaceInfos_c::IsAudioWater(SurfaceId id)
{
    return m_surfaces[id].bIsAudioWater;
}

// 0x55EAF0
bool SurfaceInfos_c::IsAudioMetal(SurfaceId id)
{
    return m_surfaces[id].bIsAudioMetal;
}

// 0x55EB10
bool SurfaceInfos_c::IsAudioLongGrass(SurfaceId id)
{
    return m_surfaces[id].bIsAudioLongGrass;
}

// 0x55EB30
bool SurfaceInfos_c::IsAudioTile(SurfaceId id)
{
    return m_surfaces[id].bIsAudioTile;
}

// 0x55EB50
float SurfaceInfos_c::GetAdhesiveLimit(CColPoint* colPoint)
{
    auto& surfaceA = m_surfaces[colPoint->m_nSurfaceTypeA];
    auto& surfaceB = m_surfaces[colPoint->m_nSurfaceTypeB];
    return m_adhesiveLimits[surfaceB.ucAdhesionGroup][surfaceA.ucAdhesionGroup];
}
