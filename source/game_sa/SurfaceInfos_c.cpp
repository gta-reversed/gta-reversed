#include "StdInc.h"

#include "SurfaceInfos_c.h"
#include "Skidmark.h"
#include <string_view>

// ============================================================================
// HELPER METHODS
// ============================================================================
namespace {
// Zero-cost bounds checking.
// Falls back to index 0 (SURFACE_DEFAULT) if a corrupted SurfaceID is passed.
template<typename IndexT, typename ContainerT>
constexpr IndexT SafeIndex(IndexT index, const ContainerT& container) noexcept {
    return static_cast<size_t>(index) < std::size(container) ? index : static_cast<IndexT>(0);
}
}

// ============================================================================
// HOOKS
// ============================================================================
void SurfaceInfos_c::InjectHooks() {
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
SurfaceId SurfaceInfos_c::GetSurfaceIdFromName(const char* cName) {
    static constexpr struct {
        std::string_view   name;
        const eSurfaceType type;
    } mapping[] = {
        { cDefaultName,          SURFACE_DEFAULT             },
        { "TARMAC",              SURFACE_TARMAC              },
        { "TARMAC_FUCKED",       SURFACE_TARMAC_FUCKED       },
        { "TARMAC_REALLYFUCKED", SURFACE_TARMAC_REALLYFUCKED },
        { "PAVEMENT",            SURFACE_PAVEMENT            },
        { "PAVEMENT_FUCKED",     SURFACE_PAVEMENT_FUCKED     },
        { "GRAVEL",              SURFACE_GRAVEL              },
        { "FUCKED_CONCRETE",     SURFACE_FUCKED_CONCRETE     },
        { "PAINTED_GROUND",      SURFACE_PAINTED_GROUND      },
        { "GRASS_SHORT_LUSH",    SURFACE_GRASS_SHORT_LUSH    },
        { "GRASS_MEDIUM_LUSH",   SURFACE_GRASS_MEDIUM_LUSH   },
        { "GRASS_LONG_LUSH",     SURFACE_GRASS_LONG_LUSH     },
        { "GRASS_SHORT_DRY",     SURFACE_GRASS_SHORT_DRY     },
        { "GRASS_MEDIUM_DRY",    SURFACE_GRASS_MEDIUM_DRY    },
        { "GRASS_LONG_DRY",      SURFACE_GRASS_LONG_DRY      },
        { "GOLFGRASS_ROUGH",     SURFACE_GOLFGRASS_ROUGH     },
        { "GOLFGRASS_SMOOTH",    SURFACE_GOLFGRASS_SMOOTH    },
        { "STEEP_SLIDYGRASS",    SURFACE_STEEP_SLIDYGRASS    },
        { "STEEP_CLIFF",         SURFACE_STEEP_CLIFF         },
        { "FLOWERBED",           SURFACE_FLOWERBED           },
        { "MEADOW",              SURFACE_MEADOW              },
        { "WASTEGROUND",         SURFACE_WASTEGROUND         },
        { "WOODLANDGROUND",      SURFACE_WOODLANDGROUND      },
        { "VEGETATION",          SURFACE_VEGETATION          },
        { "MUD_WET",             SURFACE_MUD_WET             },
        { "MUD_DRY",             SURFACE_MUD_DRY             },
        { "DIRT",                SURFACE_DIRT                },
        { "DIRTTRACK",           SURFACE_DIRTTRACK           },
        { "SAND_DEEP",           SURFACE_SAND_DEEP           },
        { "SAND_MEDIUM",         SURFACE_SAND_MEDIUM         },
        { "SAND_COMPACT",        SURFACE_SAND_COMPACT        },
        { "SAND_ARID",           SURFACE_SAND_ARID           },
        { "SAND_MORE",           SURFACE_SAND_MORE           },
        { "SAND_BEACH",          SURFACE_SAND_BEACH          },
        { "CONCRETE_BEACH",      SURFACE_CONCRETE_BEACH      },
        { "ROCK_DRY",            SURFACE_ROCK_DRY            },
        { "ROCK_WET",            SURFACE_ROCK_WET            },
        { "ROCK_CLIFF",          SURFACE_ROCK_CLIFF          },
        { "WATER_RIVERBED",      SURFACE_WATER_RIVERBED      },
        { "WATER_SHALLOW",       SURFACE_WATER_SHALLOW       },
        { "CORNFIELD",           SURFACE_CORNFIELD           },
        { "HEDGE",               SURFACE_HEDGE               },
        { "WOOD_CRATES",         SURFACE_WOOD_CRATES         },
        { "WOOD_SOLID",          SURFACE_WOOD_SOLID          },
        { "WOOD_THIN",           SURFACE_WOOD_THIN           },
        { "GLASS",               SURFACE_GLASS               },
        { "GLASS_WINDOWS_LARGE", SURFACE_GLASS_WINDOWS_LARGE },
        { "GLASS_WINDOWS_SMALL", SURFACE_GLASS_WINDOWS_SMALL },
        { "EMPTY1",              SURFACE_EMPTY1              },
        { "EMPTY2",              SURFACE_EMPTY2              },
        { "GARAGE_DOOR",         SURFACE_GARAGE_DOOR         },
        { "THICK_METAL_PLATE",   SURFACE_THICK_METAL_PLATE   },
        { "SCAFFOLD_POLE",       SURFACE_SCAFFOLD_POLE       },
        { "LAMP_POST",           SURFACE_LAMP_POST           },
        { "METAL_GATE",          SURFACE_METAL_GATE          },
        { "METAL_CHAIN_FENCE",   SURFACE_METAL_CHAIN_FENCE   },
        { "GIRDER",              SURFACE_GIRDER              },
        { "FIRE_HYDRANT",        SURFACE_FIRE_HYDRANT        },
        { "CONTAINER",           SURFACE_CONTAINER           },
        { "NEWS_VENDOR",         SURFACE_NEWS_VENDOR         },
        { "WHEELBASE",           SURFACE_WHEELBASE           },
        { "CARDBOARDBOX",        SURFACE_CARDBOARDBOX        },
        { "PED",                 SURFACE_PED                 },
        { "CAR",                 SURFACE_CAR                 },
        { "CAR_PANEL",           SURFACE_CAR_PANEL           },
        { "CAR_MOVINGCOMPONENT", SURFACE_CAR_MOVINGCOMPONENT },
        { "TRANSPARENT_CLOTH",   SURFACE_TRANSPARENT_CLOTH   },
        { "RUBBER",              SURFACE_RUBBER              },
        { "PLASTIC",             SURFACE_PLASTIC             },
        { "TRANSPARENT_STONE",   SURFACE_TRANSPARENT_STONE   },
        { "WOOD_BENCH",          SURFACE_WOOD_BENCH          },
        { "CARPET",              SURFACE_CARPET              },
        { "FLOORBOARD",          SURFACE_FLOORBOARD          },
        { "STAIRSWOOD",          SURFACE_STAIRSWOOD          },
        { "P_SAND",              SURFACE_P_SAND              },
        { "P_SAND_DENSE",        SURFACE_P_SAND_DENSE        },
        { "P_SAND_ARID",         SURFACE_P_SAND_ARID         },
        { "P_SAND_COMPACT",      SURFACE_P_SAND_COMPACT      },
        { "P_SAND_ROCKY",        SURFACE_P_SAND_ROCKY        },
        { "P_SANDBEACH",         SURFACE_P_SANDBEACH         },
        { "P_GRASS_SHORT",       SURFACE_P_GRASS_SHORT       },
        { "P_GRASS_MEADOW",      SURFACE_P_GRASS_MEADOW      },
        { "P_GRASS_DRY",         SURFACE_P_GRASS_DRY         },
        { "P_WOODLAND",          SURFACE_P_WOODLAND          },
        { "P_WOODDENSE",         SURFACE_P_WOODDENSE         },
        { "P_ROADSIDE",          SURFACE_P_ROADSIDE          },
        { "P_ROADSIDEDES",       SURFACE_P_ROADSIDEDES       },
        { "P_FLOWERBED",         SURFACE_P_FLOWERBED         },
        { "P_WASTEGROUND",       SURFACE_P_WASTEGROUND       },
        { "P_CONCRETE",          SURFACE_P_CONCRETE          },
        { "P_OFFICEDESK",        SURFACE_P_OFFICEDESK        },
        { "P_711SHELF1",         SURFACE_P_711SHELF1         },
        { "P_711SHELF2",         SURFACE_P_711SHELF2         },
        { "P_711SHELF3",         SURFACE_P_711SHELF3         },
        { "P_RESTUARANTTABLE",   SURFACE_P_RESTUARANTTABLE   },
        { "P_BARTABLE",          SURFACE_P_BARTABLE          },
        { "P_UNDERWATERLUSH",    SURFACE_P_UNDERWATERLUSH    },
        { "P_UNDERWATERBARREN",  SURFACE_P_UNDERWATERBARREN  },
        { "P_UNDERWATERCORAL",   SURFACE_P_UNDERWATERCORAL   },
        { "P_UNDERWATERDEEP",    SURFACE_P_UNDERWATERDEEP    },
        { "P_RIVERBED",          SURFACE_P_RIVERBED          },
        { "P_RUBBLE",            SURFACE_P_RUBBLE            },
        { "P_BEDROOMFLOOR",      SURFACE_P_BEDROOMFLOOR      },
        { "P_KIRCHENFLOOR",      SURFACE_P_KIRCHENFLOOR      },
        { "P_LIVINGRMFLOOR",     SURFACE_P_LIVINGRMFLOOR     },
        { "P_CORRIDORFLOOR",     SURFACE_P_CORRIDORFLOOR     },
        { "P_711FLOOR",          SURFACE_P_711FLOOR          },
        { "P_FASTFOODFLOOR",     SURFACE_P_FASTFOODFLOOR     },
        { "P_SKANKYFLOOR",       SURFACE_P_SKANKYFLOOR       },
        { "P_MOUNTAIN",          SURFACE_P_MOUNTAIN          },
        { "P_MARSH",             SURFACE_P_MARSH             },
        { "P_BUSHY",             SURFACE_P_BUSHY             },
        { "P_BUSHYMIX",          SURFACE_P_BUSHYMIX          },
        { "P_BUSHYDRY",          SURFACE_P_BUSHYDRY          },
        { "P_BUSHYMID",          SURFACE_P_BUSHYMID          },
        { "P_GRASSWEEFLOWERS",   SURFACE_P_GRASSWEEFLOWERS   },
        { "P_GRASSDRYTALL",      SURFACE_P_GRASSDRYTALL      },
        { "P_GRASSLUSHTALL",     SURFACE_P_GRASSLUSHTALL     },
        { "P_GRASSGRNMIX",       SURFACE_P_GRASSGRNMIX       },
        { "P_GRASSBRNMIX",       SURFACE_P_GRASSBRNMIX       },
        { "P_GRASSLOW",          SURFACE_P_GRASSLOW          },
        { "P_GRASSROCKY",        SURFACE_P_GRASSROCKY        },
        { "P_GRASSSMALLTREES",   SURFACE_P_GRASSSMALLTREES   },
        { "P_DIRTROCKY",         SURFACE_P_DIRTROCKY         },
        { "P_DIRTWEEDS",         SURFACE_P_DIRTWEEDS         },
        { "P_GRASSWEEDS",        SURFACE_P_GRASSWEEDS        },
        { "P_RIVEREDGE",         SURFACE_P_RIVEREDGE         },
        { "P_POOLSIDE",          SURFACE_P_POOLSIDE          },
        { "P_FORESTSTUMPS",      SURFACE_P_FORESTSTUMPS      },
        { "P_FORESTSTICKS",      SURFACE_P_FORESTSTICKS      },
        { "P_FORRESTLEAVES",     SURFACE_P_FORRESTLEAVES     },
        { "P_DESERTROCKS",       SURFACE_P_DESERTROCKS       },
        { "P_FORRESTDRY",        SURFACE_P_FORRESTDRY        },
        { "P_SPARSEFLOWERS",     SURFACE_P_SPARSEFLOWERS     },
        { "P_BUILDINGSITE",      SURFACE_P_BUILDINGSITE      },
        { "P_DOCKLANDS",         SURFACE_P_DOCKLANDS         },
        { "P_INDUSTRIAL",        SURFACE_P_INDUSTRIAL        },
        { "P_INDUSTJETTY",       SURFACE_P_INDUSTJETTY       },
        { "P_CONCRETELITTER",    SURFACE_P_CONCRETELITTER    },
        { "P_ALLEYRUBISH",       SURFACE_P_ALLEYRUBISH       },
        { "P_JUNKYARDPILES",     SURFACE_P_JUNKYARDPILES     },
        { "P_JUNKYARDGRND",      SURFACE_P_JUNKYARDGRND      },
        { "P_DUMP",              SURFACE_P_DUMP              },
        { "P_CACTUSDENSE",       SURFACE_P_CACTUSDENSE       },
        { "P_AIRPORTGRND",       SURFACE_P_AIRPORTGRND       },
        { "P_CORNFIELD",         SURFACE_P_CORNFIELD         },
        { "P_GRASSLIGHT",        SURFACE_P_GRASSLIGHT        },
        { "P_GRASSLIGHTER",      SURFACE_P_GRASSLIGHTER      },
        { "P_GRASSLIGHTER2",     SURFACE_P_GRASSLIGHTER2     },
        { "P_GRASSMID1",         SURFACE_P_GRASSMID1         },
        { "P_GRASSMID2",         SURFACE_P_GRASSMID2         },
        { "P_GRASSDARK",         SURFACE_P_GRASSDARK         },
        { "P_GRASSDARK2",        SURFACE_P_GRASSDARK2        },
        { "P_GRASSDIRTMIX",      SURFACE_P_GRASSDIRTMIX      },
        { "P_RIVERBEDSTONE",     SURFACE_P_RIVERBEDSTONE     },
        { "P_RIVERBEDSHALLOW",   SURFACE_P_RIVERBEDSHALLOW   },
        { "P_RIVERBEDWEEDS",     SURFACE_P_RIVERBEDWEEDS     },
        { "P_SEAWEED",           SURFACE_P_SEAWEED           },
        { "DOOR",                SURFACE_DOOR                },
        { "PLASTICBARRIER",      SURFACE_PLASTICBARRIER      },
        { "PARKGRASS",           SURFACE_PARKGRASS           },
        { "STAIRSSTONE",         SURFACE_STAIRSSTONE         },
        { "STAIRSMETAL",         SURFACE_STAIRSMETAL         },
        { "STAIRSCARPET",        SURFACE_STAIRSCARPET        },
        { "FLOORMETAL",          SURFACE_FLOORMETAL          },
        { "FLOORCONCRETE",       SURFACE_FLOORCONCRETE       },
        { "BIN_BAG",             SURFACE_BIN_BAG             },
        { "THIN_METAL_SHEET",    SURFACE_THIN_METAL_SHEET    },
        { "METAL_BARREL",        SURFACE_METAL_BARREL        },
        { "PLASTIC_CONE",        SURFACE_PLASTIC_CONE        },
        { "PLASTIC_DUMPSTER",    SURFACE_PLASTIC_DUMPSTER    },
        { "METAL_DUMPSTER",      SURFACE_METAL_DUMPSTER      },
        { "WOOD_PICKET_FENCE",   SURFACE_WOOD_PICKET_FENCE   },
        { "WOOD_SLATTED_FENCE",  SURFACE_WOOD_SLATTED_FENCE  },
        { "WOOD_RANCH_FENCE",    SURFACE_WOOD_RANCH_FENCE    },
        { "UNBREAKABLE_GLASS",   SURFACE_UNBREAKABLE_GLASS   },
        { "HAY_BALE",            SURFACE_HAY_BALE            },
        { "GORE",                SURFACE_GORE                },
        { "RAILTRACK",           SURFACE_RAILTRACK           }
    };

    std::string_view nameToFind{ cName };
    for (const auto& [name, type] : mapping) {
        if (nameToFind == name) {
            return type;
        }
    }
    return SURFACE_DEFAULT;
}

// 0x55D0E0
void SurfaceInfos_c::LoadAdhesiveLimits() {
    return plugin::CallMethod<0x55D0E0, SurfaceInfos_c*>(this);

    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile("data\\surface.dat", "rb");
#if FIX_BUGS
    if (!file) {
        NOTSA_LOG_DEBUG("[SurfaceInfos_c] Failed to open surface.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == ';' || !*line) {
            continue;
        }

        char value[4]{}; // Zero initialized for safety
        VERIFY(sscanf_s(line, "%s", SCANF_S_STR(value)) == 1);
        for (auto i = *line; i != ' '; i = *++line) {
            if (i == '\t') {
                break;
            }
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x55EB90
void SurfaceInfos_c::LoadSurfaceInfos() {
    auto* file = CFileMgr::OpenFile("data\\surfinfo.dat", "r");
#if FIX_BUGS
    if (!file) {
        NOTSA_LOG_DEBUG("[SurfaceInfos_c] Failed to open surfinfo.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line) {
            continue;
        }

        SurfaceInfo si{};
        si.Read(line);

        auto  id      = SurfaceInfos_c::GetSurfaceIdFromName(si.SurfaceName);
        auto  safeId  = SafeIndex(id, m_surfaces);
        auto& surface = m_surfaces[safeId];

        std::string_view adhesionGroup{ si.AdhesionGroup };
        if (adhesionGroup == "RUBBER") {
            surface.ucAdhesionGroup = ADHESION_GROUP_RUBBER;
        } else if (adhesionGroup == "HARD") {
            surface.ucAdhesionGroup = ADHESION_GROUP_HARD;
        } else if (adhesionGroup == "ROAD") {
            surface.ucAdhesionGroup = ADHESION_GROUP_ROAD;
        } else if (adhesionGroup == "LOOSE") {
            surface.ucAdhesionGroup = ADHESION_GROUP_LOOSE;
        } else if (adhesionGroup == "SAND") {
            surface.ucAdhesionGroup = ADHESION_GROUP_SAND;
        } else if (adhesionGroup == "WET") {
            surface.ucAdhesionGroup = ADHESION_GROUP_WET;
        }

        surface.tyreGrip = static_cast<uint32>(si.TyreGrip * 10.0f);
        surface.wetGrip  = static_cast<uint32>(si.WetGrip * 100.0f); // BUG:? Data has negative values, with *original* conversion we loose sign

        std::string_view skidMark{ si.SkidMark };
        if (skidMark == "DEFAULT") {
            surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::DEFAULT);
        } else if (skidMark == "SANDY") {
            surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::SANDY);
        } else if (skidMark == "MUDDY") {
            surface.ucSkidmarkType = static_cast<uint32>(eSkidmarkType::MUDDY);
        }

        std::string_view frictionEffect{ si.FrictionEffect };
        if (frictionEffect == "NONE") {
            surface.ucFrictionEffect = FRICTION_EFFECT_NONE;
        } else if (frictionEffect == "SPARKS") {
            surface.ucFrictionEffect = FRICTION_EFFECT_SPARKS;
        }

        std::string_view bulletFx{ si.BulletFx };
        if (bulletFx == "NONE") {
            surface.ucBulletFX = BULLET_FX_NONE;
        } else if (bulletFx == "SPARKS") {
            surface.ucBulletFX = BULLET_FX_SPARKS;
        } else if (bulletFx == "SAND") {
            surface.ucBulletFX = BULLET_FX_SAND;
        } else if (bulletFx == "WOOD") {
            surface.ucBulletFX = BULLET_FX_WOOD;
        } else if (bulletFx == "DUST") {
            surface.ucBulletFX = BULLET_FX_DUST;
        }

        // Modern and safe boolean casting
        surface.bIsSoftLand         = si.SoftLand != 0;
        surface.bIsSeeThrough       = si.SeeThrough != 0;
        surface.bIsShootThrough     = si.ShootThrough != 0;
        surface.bIsSand             = si.Sand != 0;
        surface.bIsWater            = si.Water != 0;
        surface.bIsShallowWater     = si.ShallowWater != 0;
        surface.bIsBeach            = si.Beach != 0;
        surface.bIsSteepSlope       = si.SteepSlope != 0;
        surface.bIsGlass            = si.Glass != 0;
        surface.bIsStairs           = si.Stairs != 0;
        surface.bIsSkateable        = si.Skateable != 0;
        surface.bIsPavement         = si.Pavement != 0;
        surface.ucRoughness         = static_cast<uint32>(si.Roughness);
        surface.ucFlammability      = static_cast<uint32>(si.Flame);
        surface.bCreatesSparks      = si.Sparks != 0;
        surface.bCantSprintOn       = si.Sprint != 0;
        surface.bLeavesFootsteps    = si.Footsteps != 0;
        surface.bProducesFootDust   = si.FootDust != 0;
        surface.bMakesCarDirty      = si.CarDirt != 0;
        surface.bMakesCarClean      = si.CarClean != 0;
        surface.bCreatesWheelGrass  = si.WheelGrass != 0;
        surface.bCreatesWheelGravel = si.WheelGravel != 0;
        surface.bCreatesWheelMud    = si.WheelMud != 0;
        surface.bCreatesWheelDust   = si.WheelDust != 0;
        surface.bCreatesWheelSand   = si.WheelSand != 0;
        surface.bCreatesWheelSpray  = si.WheelSpray != 0;
        surface.bCreatesPlants      = si.ProcPlant != 0;
        surface.bCreatesObjects     = si.ProcObj != 0;
        surface.bCanClimb           = si.Climbable != 0;
    }
    CFileMgr::CloseFile(file);
}

// 0x55F2B0
void SurfaceInfos_c::LoadSurfaceAudioInfos() {
    auto* file = CFileMgr::OpenFile("data\\surfaud.dat", "r");
#if FIX_BUGS
    if (!file) {
        NOTSA_LOG_DEBUG("[SurfaceInfos_c] Failed to open surfaud.dat");
        CFileMgr::CloseFile(file);
        return;
    }
#endif
    for (const char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line) {
            continue;
        }

        char name[64]{};
        // Explicitly zero-initialize to prevent undefined behavior if parsing fails
        int32 concrete{}, grass{}, sand{}, gravel{}, wood{}, water{}, metal{}, longGrass{}, tile{};

        VERIFY(sscanf_s(line, "%s %d %d %d %d %d %d %d %d %d", SCANF_S_STR(name), &concrete, &grass, &sand, &gravel, &wood, &water, &metal, &longGrass, &tile) == 10);

        auto  id                  = GetSurfaceIdFromName(name);
        auto  safeId              = SafeIndex(id, m_surfaces);
        auto& surface             = m_surfaces[safeId];

        surface.bIsAudioConcrete  = concrete != 0;
        surface.bIsAudioGrass     = grass != 0;
        surface.bIsAudioSand      = sand != 0;
        surface.bIsAudioGravel    = gravel != 0;
        surface.bIsAudioWood      = wood != 0;
        surface.bIsAudioWater     = water != 0;
        surface.bIsAudioMetal     = metal != 0;
        surface.bIsAudioLongGrass = longGrass != 0;
        surface.bIsAudioTile      = tile != 0;
    }
    CFileMgr::CloseFile(file);
}

void SurfaceInfos_c::Init() {
    // They call CFileMgr::SetDir("") only once in LoadAdhesiveLimits
    LoadAdhesiveLimits();
    LoadSurfaceInfos();
    LoadSurfaceAudioInfos();
}

// ============================================================================
// ACCESSOR METHODS (Safeguarded against OOB crashes)
// ============================================================================

// 0x55E5C0
eAdhesionGroup SurfaceInfos_c::GetAdhesionGroup(SurfaceId id) {
    return static_cast<eAdhesionGroup>(m_surfaces[SafeIndex(id, m_surfaces)].ucAdhesionGroup);
}

// 0x55E5E0
float SurfaceInfos_c::GetTyreGrip(SurfaceId id) {
    return static_cast<float>(m_surfaces[SafeIndex(id, m_surfaces)].tyreGrip) * 0.1F;
}

// 0x55E600
float SurfaceInfos_c::GetWetMultiplier(SurfaceId id) {
    return static_cast<float>(m_surfaces[SafeIndex(id, m_surfaces)].wetGrip) * 0.01F * CWeather::WetRoads + 1.0F;
}

// 0x55E630
uint32 SurfaceInfos_c::GetSkidmarkType(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].ucSkidmarkType;
}

// 0x55E650
eFrictionEffect SurfaceInfos_c::GetFrictionEffect(SurfaceId id) {
    return static_cast<eFrictionEffect>(m_surfaces[SafeIndex(id, m_surfaces)].ucFrictionEffect);
}

// 0x55E670
uint32 SurfaceInfos_c::GetBulletFx(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].ucBulletFX;
}

// 0x55E690
bool SurfaceInfos_c::IsSoftLanding(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsSoftLand;
}

// 0x55E6B0
bool SurfaceInfos_c::IsSeeThrough(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsSeeThrough;
}

// 0x55E6D0
bool SurfaceInfos_c::IsShootThrough(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsShootThrough;
}

// 0x55E6F0
bool SurfaceInfos_c::IsSand(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsSand;
}

// 0x55E710
bool SurfaceInfos_c::IsWater(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsWater;
}

// 0x55E730
bool SurfaceInfos_c::IsShallowWater(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsShallowWater;
}

// 0x55E750
bool SurfaceInfos_c::IsBeach(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsBeach;
}

// 0x55E770
bool SurfaceInfos_c::IsSteepSlope(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsSteepSlope;
}

// 0x55E790
bool SurfaceInfos_c::IsGlass(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsGlass;
}

// 0x55E7B0
bool SurfaceInfos_c::IsStairs(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsStairs;
}

// 0x55E7D0
bool SurfaceInfos_c::IsSkateable(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsSkateable;
}

// 0x55E7F0
bool SurfaceInfos_c::IsPavement(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsPavement;
}

// 0x55E810
uint32 SurfaceInfos_c::GetRoughness(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].ucRoughness;
}

// 0x55E830
uint32 SurfaceInfos_c::GetFlammability(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].ucFlammability;
}

// 0x55E850
bool SurfaceInfos_c::CreatesSparks(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesSparks;
}

// 0x55E870
bool SurfaceInfos_c::CantSprintOn(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCantSprintOn;
}

// 0x55E890
bool SurfaceInfos_c::LeavesFootsteps(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bLeavesFootsteps;
}

// 0x55E8B0
bool SurfaceInfos_c::ProducesFootDust(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bProducesFootDust;
}

// 0x55E8D0
bool SurfaceInfos_c::MakesCarDirty(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bMakesCarDirty;
}

// 0x55E8F0
bool SurfaceInfos_c::MakesCarClean(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bMakesCarClean;
}

// 0x55E910
bool SurfaceInfos_c::CreatesWheelGrass(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelGrass;
}

// 0x55E930
bool SurfaceInfos_c::CreatesWheelGravel(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelGravel;
}

// 0x55E950
bool SurfaceInfos_c::CreatesWheelMud(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelMud;
}

// 0x55E970
bool SurfaceInfos_c::CreatesWheelDust(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelDust;
}

// 0x55E990
bool SurfaceInfos_c::CreatesWheelSand(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelSand;
}

// 0x55E9B0
bool SurfaceInfos_c::CreatesWheelSpray(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesWheelSpray;
}

// 0x55E9D0
bool SurfaceInfos_c::CreatesPlants(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesPlants;
}

// 0x55E9F0
bool SurfaceInfos_c::CreatesObjects(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCreatesObjects;
}

// 0x55EA10
bool SurfaceInfos_c::CanClimb(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bCanClimb;
}

// 0x55EA30
bool SurfaceInfos_c::IsAudioConcrete(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioConcrete;
}

// 0x55EA50
bool SurfaceInfos_c::IsAudioGrass(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioGrass;
}

// 0x55EA70
bool SurfaceInfos_c::IsAudioSand(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioSand;
}

// 0x55EA90
bool SurfaceInfos_c::IsAudioGravel(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioGravel;
}

// 0x55EAB0
bool SurfaceInfos_c::IsAudioWood(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioWood;
}

// 0x55EAD0
bool SurfaceInfos_c::IsAudioWater(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioWater;
}

// 0x55EAF0
bool SurfaceInfos_c::IsAudioMetal(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioMetal;
}

// 0x55EB10
bool SurfaceInfos_c::IsAudioLongGrass(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioLongGrass;
}

// 0x55EB30
bool SurfaceInfos_c::IsAudioTile(SurfaceId id) {
    return m_surfaces[SafeIndex(id, m_surfaces)].bIsAudioTile;
}

// 0x55EB50
float SurfaceInfos_c::GetAdhesiveLimit(CColPoint* colPoint) {
    const auto idA       = SafeIndex(colPoint->m_nSurfaceTypeA, m_surfaces);
    const auto idB       = SafeIndex(colPoint->m_nSurfaceTypeB, m_surfaces);

    const auto& surfaceA = m_surfaces[idA];
    const auto& surfaceB = m_surfaces[idB];

    // Bounds checking for the 2D array could also be applied here if needed,
    // assuming ucAdhesionGroup always stays within safe bounds (0 to 5)
    return m_adhesiveLimits[surfaceB.ucAdhesionGroup][surfaceA.ucAdhesionGroup];
}
