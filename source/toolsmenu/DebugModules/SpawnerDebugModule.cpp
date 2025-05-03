#include <StdInc.h>

#include <imgui.h>

#include "SpawnerDebugModule.hpp"
#include "PedSpawner.h"
#include "VehicleSpawner.h"

SpawnerDebugModule::SpawnerDebugModule()  {
    VehicleDebugModule::Initialise();
    PedSpawnerModule::Initialise();
}

void SpawnerDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Spawner", { 500.f, 400.f }, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    if (ImGui::BeginTabBar("##spawnerTabs")) {
        const auto RenderSpawnerInTab = [](const char* name, auto& spawner) {
            if (ImGui::BeginTabItem(name)) {
                spawner.Render();
                ImGui::EndTabItem();
            }
        };
        RenderSpawnerInTab("Peds", m_PedSpawner);
        RenderSpawnerInTab("Vehicles", m_VehicleSpawner);

        ImGui::EndTabBar();
    }
}

void SpawnerDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Tools" }, [&] {
        ImGui::MenuItem("Spawner", nullptr, &m_IsOpen);
    });
}

/*!
* Entity spawner implementation
*/

void SpawnerDebugModule::EntitySpawner::Render() {
    m_Filter.Draw();

    if (!ImGui::BeginTable(
        "ItemsTable",
        2,
        0
        //|  ImGuiTableFlags_Sortable
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_Reorderable
        | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_BordersInnerV
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_SizingFixedFit
        //| ImGuiTableFlags_SortMulti
    )) {
        return;
    }

    ImGui::TableSetupColumn("Model ID", ImGuiTableColumnFlags_WidthStretch, 1.f);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 5.f);
    ImGui::TableHeadersRow();

    for (const auto& i : m_Items) {
        // Check passes the filter
        if (!m_Filter.PassFilter(i.Name)) {
            continue;
        }

        ImGui::PushID(i.Model);
        ImGui::BeginGroup();

        // Model
        ImGui::TableNextColumn();
        ImGui::Text("%u", (int32)(i.Model));
        ImGui::SameLine();
        if (ImGui::Selectable("##s", m_Selected == i.Model, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
            m_Selected = i.Model;
            if (ImGui::IsMouseDoubleClicked(0)) {
                Spawn(i);
            }
        }

        // Name
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(i.Name);

        ImGui::EndGroup();
        ImGui::PopID();
    }

    ImGui::EndTable();
}

/*!
* Vehicle spawner implementation
*/

SpawnerDebugModule::VehicleSpawner::VehicleSpawner() :
    EntitySpawner{{
        { MODEL_LANDSTAL, "Landstalker" },
        { MODEL_BRAVURA, "Bravura" },
        { MODEL_BUFFALO, "Buffalo" },
        { MODEL_LINERUN, "Linerunner" },
        { MODEL_PEREN, "Perennial" },
        { MODEL_SENTINEL, "Sentinel" },
        { MODEL_DUMPER, "Dumper" },
        { MODEL_FIRETRUK, "Fire Truck" },
        { MODEL_TRASH, "Trashmaster" },
        { MODEL_STRETCH, "Stretch" },
        { MODEL_MANANA, "Manana" },
        { MODEL_INFERNUS, "Infernus" },
        { MODEL_VOODOO, "Voodoo" },
        { MODEL_PONY, "Pony" },
        { MODEL_MULE, "Mule" },
        { MODEL_CHEETAH, "Cheetah" },
        { MODEL_AMBULAN, "Ambulance" },
        { MODEL_LEVIATHN, "Leviathan" },
        { MODEL_MOONBEAM, "Moonbeam" },
        { MODEL_ESPERANT, "Esperanto" },
        { MODEL_TAXI, "Taxi" },
        { MODEL_WASHING, "Washington" },
        { MODEL_BOBCAT, "Bobcat" },
        { MODEL_MRWHOOP, "Mr. Whoopee" },
        { MODEL_BFINJECT, "BF Injection" },
        { MODEL_HUNTER, "Hunter" },
        { MODEL_PREMIER, "Premier" },
        { MODEL_ENFORCER, "Enforcer" },
        { MODEL_SECURICA, "Securicar" },
        { MODEL_BANSHEE, "Banshee" },
        { MODEL_PREDATOR, "Predator" },
        { MODEL_BUS, "Bus" },
        { MODEL_RHINO, "Rhino" },
        { MODEL_BARRACKS, "Barracks" },
        { MODEL_HOTKNIFE, "Hotknife" },
        { MODEL_ARTICT1, "Trailer 1" },
        { MODEL_PREVION, "Previon" },
        { MODEL_COACH, "Coach" },
        { MODEL_CABBIE, "Cabbie" },
        { MODEL_STALLION, "Stallion" },
        { MODEL_RUMPO, "Rumpo" },
        { MODEL_RCBANDIT, "RC Bandit" },
        { MODEL_ROMERO, "Romero" },
        { MODEL_PACKER, "Packer" },
        { MODEL_MONSTER, "Monster" },
        { MODEL_ADMIRAL, "Admiral" },
        { MODEL_SQUALO, "Squalo" },
        { MODEL_SEASPAR, "Seasparrow" },
        { MODEL_PIZZABOY, "Pizzaboy" },
        { MODEL_TRAM, "Tram" },
        { MODEL_ARTICT2, "Trailer 2" },
        { MODEL_TURISMO, "Turismo" },
        { MODEL_SPEEDER, "Speeder" },
        { MODEL_REEFER, "Reefer" },
        { MODEL_TROPIC, "Tropic" },
        { MODEL_FLATBED, "Flatbed" },
        { MODEL_YANKEE, "Yankee" },
        { MODEL_CADDY, "Caddy" },
        { MODEL_SOLAIR, "Solair" },
        { MODEL_TOPFUN, "Berkley's RC Van" },
        { MODEL_SKIMMER, "Skimmer" },
        { MODEL_PCJ600, "PCJ-600" },
        { MODEL_FAGGIO, "Faggio" },
        { MODEL_FREEWAY, "Freeway" },
        { MODEL_RCBARON, "RC Baron" },
        { MODEL_RCRAIDER, "RC Raider" },
        { MODEL_GLENDALE, "Glendale" },
        { MODEL_OCEANIC, "Oceanic" },
        { MODEL_SANCHEZ, "Sanchez" },
        { MODEL_SPARROW, "Sparrow" },
        { MODEL_PATRIOT, "Patriot" },
        { MODEL_QUAD, "Quadbike" },
        { MODEL_COASTG, "Coastguard" },
        { MODEL_DINGHY, "Dinghy" },
        { MODEL_HERMES, "Hermes" },
        { MODEL_SABRE, "Sabre" },
        { MODEL_RUSTLER, "Rustler" },
        { MODEL_ZR350, "ZR-350" },
        { MODEL_WALTON, "Walton" },
        { MODEL_REGINA, "Regina" },
        { MODEL_COMET, "Comet" },
        { MODEL_BMX, "BMX" },
        { MODEL_BURRITO, "Burrito" },
        { MODEL_CAMPER, "Camper" },
        { MODEL_MARQUIS, "Marquis" },
        { MODEL_BAGGAGE, "Baggage" },
        { MODEL_DOZER, "Dozer" },
        { MODEL_MAVERICK, "Maverick" },
        { MODEL_VCNMAV, "News Chopper" },
        { MODEL_RANCHER, "Rancher" },
        { MODEL_FBIRANCH, "FBI Rancher" },
        { MODEL_VIRGO, "Virgo" },
        { MODEL_GREENWOO, "Greenwood" },
        { MODEL_JETMAX, "Jetmax" },
        { MODEL_HOTRING, "Hotring Racer" },
        { MODEL_SANDKING, "Sandking" },
        { MODEL_BLISTAC, "Blista Compact" },
        { MODEL_POLMAV, "Police Maverick" },
        { MODEL_BOXVILLE, "Boxville" },
        { MODEL_BENSON, "Benson" },
        { MODEL_MESA, "Mesa" },
        { MODEL_RCGOBLIN, "RC Goblin" },
        { MODEL_HOTRINA, "Hotring Racer 3" },
        { MODEL_HOTRINB, "Hotring Racer 2" },
        { MODEL_BLOODRA, "Bloodring Banger" },
        { MODEL_RNCHLURE, "Rancher Lure" },
        { MODEL_SUPERGT, "Super GT" },
        { MODEL_ELEGANT, "Elegant" },
        { MODEL_JOURNEY, "Journey" },
        { MODEL_BIKE, "Bike" },
        { MODEL_MTBIKE, "Mountain Bike" },
        { MODEL_BEAGLE, "Beagle" },
        { MODEL_CROPDUST, "Cropduster" },
        { MODEL_STUNT, "Stuntplane" },
        { MODEL_PETRO, "Tanker" },
        { MODEL_RDTRAIN, "Roadtrain" },
        { MODEL_NEBULA, "Nebula" },
        { MODEL_MAJESTIC, "Majestic" },
        { MODEL_BUCCANEE, "Buccaneer" },
        { MODEL_SHAMAL, "Shamal" },
        { MODEL_HYDRA, "Hydra" },
        { MODEL_FCR900, "FCR-900" },
        { MODEL_NRG500, "NRG-500" },
        { MODEL_COPBIKE, "HPV1000" },
        { MODEL_CEMENT, "Cement Truck" },
        { MODEL_TOWTRUCK, "Towtruck" },
        { MODEL_FORTUNE, "Fortune" },
        { MODEL_CADRONA, "Cadrona" },
        { MODEL_FBITRUCK, "FBI Truck" },
        { MODEL_WILLARD, "Willard" },
        { MODEL_FORKLIFT, "Forklift" },
        { MODEL_TRACTOR, "Tractor" },
        { MODEL_COMBINE, "Combine Harvester" },
        { MODEL_FELTZER, "Feltzer" },
        { MODEL_REMINGTN, "Remington" },
        { MODEL_SLAMVAN, "Slamvan" },
        { MODEL_BLADE, "Blade" },
        { MODEL_FREIGHT, "Freight" },
        { MODEL_STREAK, "Streak" },
        { MODEL_VORTEX, "Vortex" },
        { MODEL_VINCENT, "Vincent" },
        { MODEL_BULLET, "Bullet" },
        { MODEL_CLOVER, "Clover" },
        { MODEL_SADLER, "Sadler" },
        { MODEL_FIRELA, "Fire Truck Ladder" },
        { MODEL_HUSTLER, "Hustler" },
        { MODEL_INTRUDER, "Intruder" },
        { MODEL_PRIMO, "Primo" },
        { MODEL_CARGOBOB, "Cargobob" },
        { MODEL_TAMPA, "Tampa" },
        { MODEL_SUNRISE, "Sunrise" },
        { MODEL_MERIT, "Merit" },
        { MODEL_UTILITY, "Utility Van" },
        { MODEL_NEVADA, "Nevada" },
        { MODEL_YOSEMITE, "Yosemite" },
        { MODEL_WINDSOR, "Windsor" },
        { MODEL_MONSTERA, "Monster 2" },
        { MODEL_MONSTERB, "Monster 3" },
        { MODEL_URANUS, "Uranus" },
        { MODEL_JESTER, "Jester" },
        { MODEL_SULTAN, "Sultan" },
        { MODEL_STRATUM, "Stratum" },
        { MODEL_ELEGY, "Elegy" },
        { MODEL_RAINDANC, "Raindance" },
        { MODEL_RCTIGER, "RC Tiger" },
        { MODEL_FLASH, "Flash" },
        { MODEL_TAHOMA, "Tahoma" },
        { MODEL_SAVANNA, "Savanna" },
        { MODEL_BANDITO, "Bandito" },
        { MODEL_FREIFLAT, "Freight Train Flatbed" },
        { MODEL_STREAKC, "Streak Train Trailer" },
        { MODEL_KART, "Kart" },
        { MODEL_MOWER, "Mower" },
        { MODEL_DUNERIDE, "Dune" },
        { MODEL_SWEEPER, "Sweeper" },
        { MODEL_BROADWAY, "Broadway" },
        { MODEL_TORNADO, "Tornado" },
        { MODEL_AT400, "AT-400" },
        { MODEL_DFT30, "DFT-30" },
        { MODEL_HUNTLEY, "Huntley" },
        { MODEL_STAFFORD, "Stafford" },
        { MODEL_BF400, "BF-400" },
        { MODEL_NEWSVAN, "Newsvan" },
        { MODEL_TUG, "Tug" },
        { MODEL_PETROTR, "Trailer (Tanker Commando)" },
        { MODEL_EMPEROR, "Emperor" },
        { MODEL_WAYFARER, "Wayfarer" },
        { MODEL_EUROS, "Euros" },
        { MODEL_HOTDOG, "Hotdog" },
        { MODEL_CLUB, "Club" },
        { MODEL_FREIBOX, "Box Freight" },
        { MODEL_ARTICT3, "Trailer 3" },
        { MODEL_ANDROM, "Andromada" },
        { MODEL_DODO, "Dodo" },
        { MODEL_RCCAM, "RC Cam" },
        { MODEL_LAUNCH, "Launch" },
        { MODEL_COPCARLA, "Police LS" },
        { MODEL_COPCARSF, "Police SF" },
        { MODEL_COPCARVG, "Police LV" },
        { MODEL_COPCARRU, "Police Ranger" },
        { MODEL_PICADOR, "Picador" },
        { MODEL_SWATVAN, "S.W.A.T." },
        { MODEL_ALPHA, "Alpha" },
        { MODEL_PHOENIX, "Phoenix" },
        { MODEL_GLENSHIT, "Glendale Damaged" },
        { MODEL_SADLSHIT, "Sadler Damaged" },
        { MODEL_BAGBOXA, "Baggage Trailer (covered)" },
        { MODEL_BAGBOXB, "Baggage Trailer (Uncovered)" },
        { MODEL_TUGSTAIR, "Trailer (Stairs)" },
        { MODEL_BOXBURG, "Boxville Mission" },
        { MODEL_FARMTR1, "Farm Trailer" },
        { MODEL_UTILTR1, "Street Clean Trailer" },
    }}
{

}

CPhysical* SpawnerDebugModule::VehicleSpawner::Spawn(const Item& item) {
    return CCheat::VehicleCheat(item.Model);
}

/*!
* Ped spawner implementation
*/

SpawnerDebugModule::PedSpawner::PedSpawner() :
    EntitySpawner{{
        { MODEL_PLAYER, "CJ"},
        { MODEL_MALE01, "Male 01 (Special)" },
        { MODEL_BFORI, "Black Female Old Rich" },
        { MODEL_BFOST, "Black Female Old Street" },
        { MODEL_VBFYCRP, "(Las Vegas) Black Female Young Croupier" },
        { MODEL_BFYRI, "Black Female Young Rich" },
        { MODEL_BFYST, "Black Female Young Street" },
        { MODEL_BMORI, "Black Male Old Rich" },
        { MODEL_BMOST, "Black Male Old Street" },
        { MODEL_BMYAP, "Black Male Young Airport" },
        { MODEL_BMYBU, "Black Male Young Business" },
        { MODEL_BMYBE, "Black Male Young Beach" },
        { MODEL_BMYDJ, "Black Male Young Disk Jockey" },
        { MODEL_BMYRI, "Black Male Young Rich" },
        { MODEL_BMYCR, "Black Male Young Criminal" },
        { MODEL_BMYST, "Black Male Young Street" },
        { MODEL_WMYBMX, "White Male Young BMX Rider" },
        { MODEL_WBDYG1, "Madd Doggs Bodyguard 1" },
        { MODEL_WBDYG2, "Madd Doggs Bodyguard 2" },
        { MODEL_WMYBP, "White Male Young Backpacker" },
        { MODEL_WMYCON, "White Male Young Construction Worker" },
        { MODEL_BMYDRUG, "Black Male Young Drug Dealer" },
        { MODEL_WMYDRUG, "White Male Young Drug Dealer" },
        { MODEL_HMYDRUG, "Hispanic Male Young Drug Dealer" },
        { MODEL_DWFOLC, "Desert White Female Old Low Class?" },
        { MODEL_DWMOLC1, "Desert Male Female Old Low Class 1?" },
        { MODEL_DWMOLC2, "Desert Male Female Old Low Class 2?" },
        { MODEL_DWMYLC1, "Desert Male Female Young Low Class 1?" },
        { MODEL_HMOGAR, "Hispanic Male Old Gardener" },
        { MODEL_WMYGOL1, "White Male Young Golfer 1" },
        { MODEL_WMYGOL2, "White Male Young Golfer 2" },
        { MODEL_HFORI, "Hispanic Female Old Rich" },
        { MODEL_HFOST, "Hispanic Female Old Street" },
        { MODEL_HFYRI, "Hispanic Female Young Rich" },
        { MODEL_HFYST, "Hispanic Female Young Street" },
        { MODEL_HMORI, "Hispanic Male Old Rich" },
        { MODEL_HMOST, "Hispanic Male Old Street" },
        { MODEL_HMYBE, "Hispanic Male Young Beach" },
        { MODEL_HMYRI, "Hispanic Male Young Rich" },
        { MODEL_HMYCR, "Hispanic Male Young Criminal" },
        { MODEL_HMYST, "Hispanic Male Young Street" },
        { MODEL_OMOKUNG, "Oriental Male Old Kung Fu" },
        { MODEL_WMYMECH, "White Male Young Mechanic" },
        { MODEL_BMYMOUN, "Black Male Young Mountain Biker" },
        { MODEL_WMYMOUN, "White Male Young Mountain Biker" },
        { MODEL_OFORI, "Oriental Female Old Rich" },
        { MODEL_OFOST, "Oriental Female Old Street" },
        { MODEL_OFYRI, "Oriental Female Young Rich" },
        { MODEL_OFYST, "Oriental Female Young Street" },
        { MODEL_OMORI, "Oriental Male Old Rich" },
        { MODEL_OMOST, "Oriental Male Old Street" },
        { MODEL_OMYRI, "Oriental Male Young Rich" },
        { MODEL_OMYST, "Oriental Male Young Street" },
        { MODEL_WMYPLT, "White Male Young Pilot" },
        { MODEL_WMOPJ, "White Male Old Pyjamas" },
        { MODEL_BFYPRO, "Black Female Young Prostitute" },
        { MODEL_HFYPRO, "Hispanic Female Young Prostitute" },
        { MODEL_BMYPOL1, "Black Male Young Pool 1" },
        { MODEL_BMYPOL2, "Black Male Young Pool 2" },
        { MODEL_WMOPREA, "White Male Old Preacher" },
        { MODEL_SBFYST, "(San Fransisco) Black Female Young Street" },
        { MODEL_WMOSCI, "White Male Old Scientist" },
        { MODEL_WMYSGRD, "White Male Young Security Guard" },
        { MODEL_SWMYHP1, "(San Fransisco) White Male Young Hippie 1" },
        { MODEL_SWMYHP2, "(San Fransisco) White Male Young Hippie 2" },
        { MODEL_SWFOPRO, "(San Fransisco) White Female Old Prostitute" },
        { MODEL_WFYSTEW, "White Female Young Stewardess" },
        { MODEL_SWMOTR1, "(San Fransisco) White Male Old Tramp 1" },
        { MODEL_WMOTR1, "White Male Old Tramp 1" },
        { MODEL_BMOTR1, "Black Male Old Tramp 1" },
        { MODEL_VBMYBOX, "(Las Vegas) Black Male Young Boxer" },
        { MODEL_VWMYBOX, "(Las Vegas) White Male Young Boxer" },
        { MODEL_VHMYELV, "(Las Vegas) Hispanic Male Young Elvis" },
        { MODEL_VBMYELV, "(Las Vegas) Black Male Young Elvis" },
        { MODEL_VIMYELV, "(Las Vegas) Indian Male Young Elvis" },
        { MODEL_VWFYPRO, "(Las Vegas) White Female Young Prostitute" },
        { MODEL_VWFYST1, "(Las Vegas) White, Female, Young, Stripper 1" },
        { MODEL_WFORI, "White Female Old Rich" },
        { MODEL_WFOST, "White Female Old Street" },
        { MODEL_WFYJG, "White Female Young Jogger" },
        { MODEL_WFYRI, "White Female Young Rich" },
        { MODEL_WFYRO, "White Female Young Rollerblader" },
        { MODEL_WFYST, "White Female Young Street" },
        { MODEL_WMORI, "White Male Old Rich" },
        { MODEL_WMOST, "White Male Old Street" },
        { MODEL_WMYJG, "White Male Young Jogger" },
        { MODEL_WMYLG, "White Male Young Lifeguard" },
        { MODEL_WMYRI, "White Male Young Rich" },
        { MODEL_WMYRO, "White Male Young Rollerblader" },
        { MODEL_WMYCR, "White Male Young Criminal" },
        { MODEL_WMYST, "White Male Young Street" },
        { MODEL_BALLAS1, "Ballas Gang Member 1" },
        { MODEL_BALLAS2, "Ballas Gang Member 2" },
        { MODEL_BALLAS3, "Ballas Gang Member 3" },
        { MODEL_FAM1, "Grove Street Famalies Gang Member 1" },
        { MODEL_FAM2, "Grove Street Famalies Gang Member 2" },
        { MODEL_FAM3, "Grove Street Famalies Gang Member 3" },
        { MODEL_LSV1, "Los Santos Vagos Gang Member 1" },
        { MODEL_LSV2, "Los Santos Vagos Gang Member 2" },
        { MODEL_LSV3, "Los Santos Vagos Gang Member 3" },
        { MODEL_MAFFA, "Mafia Gang Member A" },
        { MODEL_MAFFB, "Mafia Gang Member B" },
        { MODEL_MAFBOSS, "Mafia Boss" },
        { MODEL_VLA1, "Vagos Los Aztecas Gang Member 1" },
        { MODEL_VLA2, "Vagos Los Aztecas Gang Member 2" },
        { MODEL_VLA3, "Vagos Los Aztecas Gang Member 3" },
        { MODEL_TRIADA, "Triad Gang Member A" },
        { MODEL_TRIADB, "Triad Gang Member B" },
        { MODEL_TRIBOSS, "Triad Boss" },
        { MODEL_DNB1, "Da Nang Boys Gang Member 1" },
        { MODEL_DNB2, "Da Nang Boys Gang Member 2" },
        { MODEL_DNB3, "Da Nang Boys Gang Member 3" },
        { MODEL_VMAFF1, "(Las Vegas) Mafia Gang Member 1" },
        { MODEL_VMAFF2, "(Las Vegas) Mafia Gang Member 2" },
        { MODEL_VMAFF3, "(Las Vegas) Mafia Gang Member 3" },
        { MODEL_VMAFF4, "(Las Vegas) Mafia Gang Member 4" },
        { MODEL_DNMYLC, "(Desert) Native Male, Young, Low Class?" },
        { MODEL_DNFOLC1, "(Desert) Native Female, Old Low Class 1?" },
        { MODEL_DNFOLC2, "(Desert) Native Female, Old Low Class 2?" },
        { MODEL_DNFYLC, "(Desert) Native Female Young, Low Class?" },
        { MODEL_DNMOLC1, "(Desert) Native Male Old Low Class 1?" },
        { MODEL_DNMOLC2, "(Desert) Native Male Old Low Class 2?" },
        { MODEL_SBMOTR2SBMOTR2, "(San Fransisco) Black Male Old Tramp 2" },
        { MODEL_SWMOTR2, "(San Fransisco) White Male Old Tramp 2" },
        { MODEL_SBMYTR3, "(San Fransisco) White Male Young Tramp 3" },
        { MODEL_SWMOTR3, "(San Fransisco) White Male Old Tramp 3" },
        { MODEL_WFYBE, "(San Fransisco) White Female Young Beach" },
        { MODEL_BFYBE, "(San Fransisco) Black Female Young Beach" },
        { MODEL_HFYBE, "(San Fransisco) Hispanic Female Young Beach" },
        { MODEL_SOFYBU, "(San Fransisco) Oriental Female Young Business" },
        { MODEL_SBMYST, "(San Fransisco) Black Male Young Street" },
        { MODEL_SBMYCR, "(San Fransisco) Black Male Young Criminal" },
        { MODEL_BMYCG, "Black Male Young Crack Guard?" },
        { MODEL_WFYCRK, "White Female Young Crack" },
        { MODEL_HMYCM, "Hispanic Male Young Crack Meth?" },
        { MODEL_WMYBU, "White Male Young Business" },
        { MODEL_BFYBU, "Black Female Young Business" },
        { MODEL_WFYBU, "White Female Young Business" },
        { MODEL_DWFYLC1, "(Desert) White Female Young Low Class 1?" },
        { MODEL_WFYPRO, "White Female Young Prostitute" },
        { MODEL_WMYCONB, "White Male Young Construction Worker B" },
        { MODEL_WMYBE, "White Male Young Beach" },
        { MODEL_WMYPIZZ, "White Male Young Pizza Stack Employee" },
        { MODEL_BMOBAR, "Black Male Old Barber (Old Reece)" },
        { MODEL_CWFYHB, "(Country) White Female Young Hillbilly" },
        { MODEL_CWMOFR, "(Country) White Male Old FR?" },
        { MODEL_CWMOHB1, "(Country) White Male Old Hilbilly 1" },
        { MODEL_CWMOHB2, "(Country) White Male Old Hilbilly 2" },
        { MODEL_CWMYFR, "(Country) White Male Young FR" },
        { MODEL_CWMYHB1, "(Country) White Male Young, Hillbilly 1" },
        { MODEL_BMYBOUN, "Black Male Young Bouncer" },
        { MODEL_WMYBOUN, "White Male Young Bouncer" },
        { MODEL_WMOMIB, "White Male Old Men In Black" },
        { MODEL_BMYMIB, "Black Male Young Men In Black" },
        { MODEL_WMYBELL, "White Male Young Clucking Bell Employee" },
        { MODEL_BMOCHIL, "Black Male Old Chilli Stall Owner" },
        { MODEL_SOFYRI, "(San Fransisco) Oriental Female Young Rich" },
        { MODEL_SOMYST, "(San Fransisco) Oriental Male Young Street" },
        { MODEL_VWMYBJD, "(Las Vegas) White Male Young Blackjack Dealer" },
        { MODEL_VWFYCRP, "(Las Vegas) White Female Young Croupier" },
        { MODEL_SFR1, "San Fransisco Rifa Gang Member 1" },
        { MODEL_SFR2, "San Fransisco Rifa Gang Member 2" },
        { MODEL_SFR3, "San Fransisco Rifa Gang Member 3" },
        { MODEL_BMYBAR, "Black Male Young Barber" },
        { MODEL_WMYBAR, "White Male Young Barber" },
        { MODEL_WFYSEX, "White Female Young Sexy" },
        { MODEL_WMYAMMO, "White Male Young Ammunation Guy" },
        { MODEL_BMYTATT, "Black Male Young Tattoo" },
        { MODEL_VWMYCR, "(Vegas) White Male Young Criminal" },
        { MODEL_VBMOCD, "(Vegas) Black Male Old Cab Driver" },
        { MODEL_VBMYCR, "(Vegas) Black Male Young Criminal" },
        { MODEL_VHMYCR, "(Vegas) Hispanic Male Young Criminal" },
        { MODEL_SBMYRI, "(San Fransisco) Black Male Young Rich" },
        { MODEL_SOMYRI, "(San Fransisco) Oriental Male Young Rich" },
        { MODEL_SOMYBU, "(San Fransisco) Oriental Male Young Business" },
        { MODEL_SWMYST, "(San Fransisco) White Male Young Street" },
        { MODEL_WMYVA, "White Male Young Valet" },
        { MODEL_COPGRL3, "Barbara Schternvart" },
        { MODEL_GUNGRL3, "Helena Wankstein" },
        { MODEL_MECGRL3, "Michelle Cannes" },
        { MODEL_NURGRL3, "Katie Zhan" },
        { MODEL_CROGRL3, "Millie Perkins" },
        { MODEL_GANGRL3, "Denise Robinson" },
        { MODEL_CWFOFR, "(Country) White Femal, Old, FR?" },
        { MODEL_CWFOHB, "(Country) White Female Old Hillbilly" },
        { MODEL_CWFYFR1, "(Country) White Female Young FR1?" },
        { MODEL_CWFYFR2, "(Country) White Female Young FR2?" },
        { MODEL_CWMYHB2, "(Country) White Male Young Hillbilly 2" },
        { MODEL_DWFYLC2, "(Desert) White Female Young LC2?" },
        { MODEL_DWMYLC2, "(Desert) White Male Young LC2?" },
        { MODEL_OMYKARA, "Oriental Male Young Karate Trainer" },
        { MODEL_WMYKARA, "White Male Young Karate Trainer" },
        { MODEL_WFYBURG, "White Female Young Burger Shot Employee" },
        { MODEL_VWMYCD, "(Las Vegas) White Male Young Cab Driver" },
        { MODEL_VHFYPRO, "(Las Vegas) Hispanic Female Young Prostitute" },
        { MODEL_OMONOOD, "Oriental Male Old Noodle Stall Owner" },
        { MODEL_OMOBOAT, "Oriental Male Old Boat School Instructor" },
        { MODEL_WFYCLOT, "White Female Old CLOT? (Bar Steward)" },
        { MODEL_VWMOTR1, "(Las Vegas) White Male Old Tramp 1" },
        { MODEL_VWMOTR2, "(Las Vegas) White Male Old Tramp 2" },
        { MODEL_VWFYWAI, "(Las Vegas) White Female Young Waiter (Maria)" },
        { MODEL_SBFORI, "(San Fransisco) Black Female Old Rich" },
        { MODEL_SWFYRI, "(San Fransisco) White Female Young Rich" },
        { MODEL_WMYCLOT, "White Male Young CLOT? (Bar Stuard)" },
        { MODEL_SBFOST, "(San Fransisco) Black Female Old Street" },
        { MODEL_SBFYRI, "(San Fransisco) Black Female Young Rich" },
        { MODEL_SBMOCD, "(San Fransisco) Black Male Old Cab Driver" },
        { MODEL_SBMORI, "(San Fransisco) Black Male Old Rich" },
        { MODEL_SBMOST, "(San Fransisco) Black Male Old Street" },
        { MODEL_SHMYCR, "(San Fransisco) Hispanic Male Young Criminal" },
        { MODEL_SOFORI, "(San Fransisco) Oriental Female Old Rich" },
        { MODEL_SOFOST, "(San Fransisco) Oriental Female Old Street" },
        { MODEL_SOFYST, "(San Fransisco) Oriental Female Young Street" },
        { MODEL_SOMOBU, "(San Fransisco) Oriental Male Old Business" },
        { MODEL_SOMORI, "(San Fransisco) Oriental Male Old Rich" },
        { MODEL_SOMOST, "(San Fransisco) Oriental Male Old Street" },
        { MODEL_SWMOTR5, "(San Fransisco) White Male Old Tramp 5" },
        { MODEL_SWFORI, "(San Fransisco) White Female Old Rich" },
        { MODEL_SWFOST, "(San Fransisco) White Female Old Street" },
        { MODEL_SWFYST, "(San Fransisco) White Female Young Street" },
        { MODEL_SWMOCD, "(San Fransisco) White Male Old Cab Driver" },
        { MODEL_SWMORI, "(San Fransisco) White Male Old Rich" },
        { MODEL_SWMOST, "(San Fransisco) White Male Old Street" },
        { MODEL_SHFYPRO, "(San Fransisco) Hispanic Female Young Prostitute" },
        { MODEL_SBFYPRO, "(San Fransisco) Black Female Young Prostitute" },
        { MODEL_SWMOTR4, "(San Fransisco) White Male Old Tramp 4" },
        { MODEL_SWMYRI, "(San Fransisco) White Male Young Rich" },
        { MODEL_SMYST, "(San Fransisco) Male Young Street" },
        { MODEL_SMYST2, "(San Fransisco) Male Young Street 2" },
        { MODEL_SFYPRO, "(San Fransisco) Female Young Prostitute" },
        { MODEL_VBFYST2, "(Las Vegas) Black Female Young Street 2" },
        { MODEL_VBFYPRO, "(Las Vegas) Black Female Young Prostitute" },
        { MODEL_VHFYST3, "(Las Vegas) Hispanic Female Young Street 3" },
        { MODEL_BIKERA, "Biker A" },
        { MODEL_BIKERB, "Biker B" },
        { MODEL_BMYPIMP, "Black, Male, Young, Pimp" },
        { MODEL_SWMYCR, "(San Fransisco) White Male Young Criminal" },
        { MODEL_WFYLG, "White Female Young Lifeguard" },
        { MODEL_WMYVA2, "White Male Young Valet 2" },
        { MODEL_BMOSEC, "Black Male Old Security Guard" },
        { MODEL_BIKDRUG, "Biker / Drug Dealer" },
        { MODEL_WMYCH, "White Male Young Chauffeur" },
        { MODEL_SBFYSTR, "(San Fransisco) Black Female Young Stripper" },
        { MODEL_SWFYSTR, "(San Fransisco) White Female Young Stripper" },
        { MODEL_HECK1, "Heckler 1" },
        { MODEL_HECK2, "Heckler 2" },
        { MODEL_BMYCON, "Black Male Young Construction Worker" },
        { MODEL_WMYCD1, "White Male Young Cab Driver 1" },
        { MODEL_BMOCD, "Black Male Old Cab Driver" },
        { MODEL_VWFYWA2, "(Vegas) White Female Young Waiter 2" },
        { MODEL_WMOICE, "White Male Old Ice Cream Man" },
        { MODEL_LAEMT1, "Los Angeles Emergency Medical Technician 1" },
        { MODEL_LVEMT1, "Las Vegas Emergency Medical Technician 1" },
        { MODEL_SFEMT1, "San Fransisco Emergency Medical Technician 1" },
        { MODEL_LAFD1, "Los Angeles Fire Department Firefighter 1" },
        { MODEL_LVFD1, "Las Vegas Fire Department Firefighter 1" },
        { MODEL_SFFD1, "San Fransisco Fire Department Firefighter 1" },
        { MODEL_LAPD1, "Los Angeles Police Department Cop 1" },
        { MODEL_SFPD1, "San Fransisco Police Department Cop 1" },
        { MODEL_LVPD1, "Las Vegas Police Department Cop 1" },
        { MODEL_CSHER, "Country Sheriff" },
        { MODEL_LAPDM1, "Los Angeles Police Department Motorbike Cop 1" },
        { MODEL_SWAT, "S.W.A.T Officer" },
        { MODEL_FBI, "FBI Agent" },
        { MODEL_ARMY, "Army Officer" },
        { MODEL_DSHER, "Desert Sheriff" },
    }}
{
}

CPhysical* SpawnerDebugModule::PedSpawner::Spawn(const Item& item) {
    CStreaming::RequestModel(item.Model, STREAMING_MISSION_REQUIRED | STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    CPed* ped = new CCivilianPed(CPopulation::IsFemale(item.Model) ? PED_TYPE_CIVFEMALE : PED_TYPE_CIVMALE, item.Model);
    ped->SetOrientation(0.0f, 0.0f, 0.0f);
    ped->SetPosn(FindPlayerPed()->TransformFromObjectSpace(CVector(2.0f, 2.0f, 0.0f)));
    CWorld::Add(ped);
    ped->PositionAnyPedOutOfCollision();
    return ped;
}
