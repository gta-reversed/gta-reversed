#include "StdInc.h"

#include "TempColModels.h"

void CTempColModels::InjectHooks() {
    RH_ScopedClass(CTempColModels);
    RH_ScopedCategory("Collision");

    RH_ScopedGlobalInstall(Shutdown, 0x41B360);
    RH_ScopedGlobalInstall(Initialise, 0x5BA8C0);
}

void CTempColModels::Initialise() {
    const auto InitializeColModel = [](CColModel& cm, CVector bbMin, CVector bbMax, CVector spCenter, float spRadius, uint8 colSlot = 0) {
        cm.GetBoundingSphere().Set(spRadius, spCenter);
        cm.GetBoundingBox().Set(bbMin, bbMax);
        cm.m_nColSlot = colSlot;
    };

    // 0x5BA8EC
    {
        InitializeColModel(
            ms_colModelBBox,
            {-2.00f, -2.00f, -2.00f}, { 2.00f,  2.00f,  2.00f},
            { 0.00f,  0.00f,  0.00f}, 2.00f
        );
    }

    // 0x5BA94C - 0x5BAAF4
    for (auto i = 0; i < 5; i++) {
        InitializeColModel(
            ms_colModelCutObj[i],
            {-2.00f, -2.00f, -2.00f}, { 2.00f,  2.00f,  2.00f},
            { 0.00f,  0.00f,  0.00f}, 2.00f
        );
    }

    // 0x5BAB5C
    {
        InitializeColModel(
            ms_colModelPed1,
            {-0.35f, -0.35f, -1.00f}, { 0.35f,  0.35f,  0.95f},
            { 0.00f,  0.00f,  0.00f}, 1.00f
        );

        ms_colModelPed1.AllocateData(3, 0, 2, 0, 0, false);
        auto* const cd = ms_colModelPed1.m_pColData;

        cd->GetSpheres()[0].Set(0.35f, { 0.00f,  0.00f, -0.20f}, SURFACE_PED, PED_COL_SPHERE_LEG);
        cd->GetSpheres()[1].Set(0.35f, { 0.00f,  0.00f,  0.20f}, SURFACE_PED, PED_COL_SPHERE_MID);
        cd->GetSpheres()[2].Set(0.35f, { 0.00f,  0.00f,  0.60f}, SURFACE_PED, PED_COL_SPHERE_HEAD);

        cd->GetLines()[0].Set({0.f, 0.f, 0.f}, {0.f, 0.f, -1.f}); // Leg L (?)
        cd->GetLines()[1].Set({0.f, 0.f, 0.f}, {0.f, 0.f,  1.f}); // Leg R (?)
        cd->m_nNumLines = 0; // ????
    }

    // 0x5BACC6
    {
        InitializeColModel(
            ms_colModelPed2,
            {-0.35f, -0.35f, -1.00f}, { 0.35f,  0.90f,  0.95f},
            { 0.00f,  0.00f,  0.00f}, 1.00f
        );

        ms_colModelPed2.AllocateData(9, 0, 2, 0, 0, false);
        auto* const cd = ms_colModelPed2.m_pColData;

        cd->GetSpheres()[0].Set(0.35f, { 0.00f,  0.00f, -0.20f}, SURFACE_PED, PED_COL_SPHERE_LEG);
        cd->GetSpheres()[1].Set(0.35f, { 0.00f,  0.00f,  0.20f}, SURFACE_PED, PED_COL_SPHERE_MID);
        cd->GetSpheres()[2].Set(0.35f, { 0.00f,  0.00f,  0.60f}, SURFACE_PED, PED_COL_SPHERE_HEAD);
        cd->GetSpheres()[3].Set(0.13f, { 0.18f,  0.35f,  0.75f}, SURFACE_PED, PED_PIECE_RIGHT_ARM);
        cd->GetSpheres()[4].Set(0.13f, { 0.18f,  0.50f,  0.75f}, SURFACE_PED, PED_PIECE_RIGHT_ARM);
        cd->GetSpheres()[5].Set(0.13f, {-0.18f,  0.35f,  0.75f}, SURFACE_PED, PED_PIECE_LEFT_ARM);
        cd->GetSpheres()[6].Set(0.13f, {-0.18f,  0.50f,  0.75f}, SURFACE_PED, PED_PIECE_LEFT_ARM);
        cd->GetSpheres()[7].Set(0.20f, { 0.00f,  0.50f,  0.45f}, SURFACE_PED, PED_PIECE_RIGHT_LEG);
        cd->GetSpheres()[8].Set(0.35f, { 0.00f,  0.55f,  0.20f}, SURFACE_PED, PED_PIECE_ASS);

        cd->GetLines()[0].Set({0.f, 0.f, 0.f}, {0.f, 0.f, -1.f}); // Leg L (?)
        cd->GetLines()[1].Set({0.f, 0.f, 0.f}, {0.f, 0.f,  1.f}); // Leg R (?)
        cd->m_nNumLines = 0; // ????
    }

    // 0x5BAF71
    {
        InitializeColModel(
            ms_colModelDoor1,
            {-0.30f,  0.00f, -0.60f}, { 0.30f, -1.20f,  0.60f},
            { 0.00f, -0.60f,  0.00f}, 1.50f
        );

        ms_colModelDoor1.AllocateData(3, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelDoor1.m_pColData;

        cd->GetSpheres()[0].Set(0.15f, { 0.00f, -0.25f, -0.35f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[1].Set(0.15f, { 0.00f, -0.95f, -0.35f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[2].Set(0.25f, { 0.00f, -0.60f,  0.25f}, SURFACE_CAR_PANEL);
    }

    // 0x5BB082
    {
        InitializeColModel(
            ms_colModelBumper1,
            {-1.20f, -0.30f, -0.20f}, { 1.20f,  0.30f,  0.20f},
            { 0.00f,  0.00f,  0.00f}, 2.20f
        );

        ms_colModelBumper1.AllocateData(4, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelBumper1.m_pColData;

        cd->GetSpheres()[0].Set(0.15f, { 0.85f, -0.05f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[1].Set(0.15f, { 0.40f,  0.05f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[2].Set(0.15f, {-0.40f,  0.05f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[3].Set(0.15f, {-0.85f, -0.05f,  0.00f}, SURFACE_CAR_PANEL);
    }

    // 0x5BB1C4
    {
        InitializeColModel(
            ms_colModelPanel1,
            {-0.30f, -0.60f, -0.15f}, { 0.30f,  0.60f,  0.15f},
            { 0.00f,  0.00f,  0.00f}, 1.40f
        );

        ms_colModelPanel1.AllocateData(4, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelPanel1.m_pColData;

        cd->GetSpheres()[0].Set(0.15f, { 0.15f,  0.45f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[1].Set(0.15f, { 0.15f, -0.45f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[2].Set(0.15f, {-0.15f, -0.45f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[3].Set(0.15f, {-0.15f,  0.45f,  0.00f}, SURFACE_CAR_PANEL);
    }

    // 0x5BB306
    {
        InitializeColModel(
            ms_colModelBonnet1,
            {-0.70f, -0.20f, -0.30f}, { 0.70f,  1.20f,  0.30f},
            { 0.00f,  0.50f,  0.00f}, 1.70f
        );

        ms_colModelBonnet1.AllocateData(4, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelBonnet1.m_pColData;

        cd->GetSpheres()[0].Set(0.20f, {-0.40f,  0.10f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[1].Set(0.20f, {-0.40f,  0.90f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[2].Set(0.20f, { 0.40f,  0.10f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[3].Set(0.20f, { 0.40f,  0.90f,  0.00f}, SURFACE_CAR_PANEL);
    }

    // 0x5BB44C
    {
        InitializeColModel(
            ms_colModelBoot1,
            {-0.70f, -0.90f, -0.30f}, { 0.70f,  0.20f,  0.30f},
            { 0.00f, -0.40f,  0.00f}, 1.40f
        );

        ms_colModelBoot1.AllocateData(4, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelBoot1.m_pColData;

        cd->GetSpheres()[0].Set(0.20f, {-0.40f, -0.10f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[1].Set(0.20f, {-0.40f, -0.60f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[2].Set(0.20f, { 0.40f, -0.10f,  0.00f}, SURFACE_CAR_PANEL);
        cd->GetSpheres()[3].Set(0.20f, { 0.40f, -0.60f,  0.00f}, SURFACE_CAR_PANEL);
    }

    // 0x5BB592
    {
        InitializeColModel(
            ms_colModelWheel1,
            {-0.70f, -0.40f, -0.40f}, { 0.70f,  0.40f,  0.40f},
            { 0.00f,  0.00f,  0.00f}, 1.40f
        );

        ms_colModelWheel1.AllocateData(2, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelWheel1.m_pColData;

        cd->GetSpheres()[0].Set(0.35f, {-0.30f,  0.00f,  0.00f}, SURFACE_WHEELBASE);
        cd->GetSpheres()[1].Set(0.35f, { 0.30f,  0.00f,  0.00f}, SURFACE_WHEELBASE);
    }

    // 0x5BB66A
    {
        InitializeColModel(
            ms_colModelBodyPart1,
            {-0.30f, -0.30f, -0.30f}, { 1.10f,  0.30f,  0.30f},
            { 0.40f,  0.00f,  0.00f}, 0.70f
        );

        ms_colModelBodyPart1.AllocateData(2, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelBodyPart1.m_pColData;

        cd->GetSpheres()[0].Set(0.20f, { 0.00f,  0.00f,  0.00f}, SURFACE_PED);
        cd->GetSpheres()[1].Set(0.20f, { 0.80f,  0.00f,  0.00f}, SURFACE_PED);
    }

    // 0x5BB742
    {
        InitializeColModel(
            ms_colModelBodyPart2,
            {-0.20f, -0.20f, -0.20f}, { 0.70f,  0.20f,  0.20f},
            { 0.25f,  0.00f,  0.00f}, 0.50f
        );

        ms_colModelBodyPart2.AllocateData(2, 0, 0, 0, 0, false);
        auto* const cd = ms_colModelBodyPart2.m_pColData;

        cd->GetSpheres()[0].Set(0.15f, { 0.00f,  0.00f,  0.00f}, SURFACE_PED);
        cd->GetSpheres()[1].Set(0.15f, { 0.50f,  0.00f,  0.00f}, SURFACE_PED);
    }

    // 0x5BB82C
    {
        InitializeColModel(
            ms_colModelWeapon,
            {-0.25f, -0.25f, -0.25f}, { 0.25f,  0.25f,  0.25f},
            { 0.00f,  0.00f,  0.00f}, 0.25f
        );
    }
}

void CTempColModels::Shutdown() {
    ms_colModelBBox.RemoveCollisionVolumes();
    ms_colModelPed1.RemoveCollisionVolumes();
    ms_colModelPed2.RemoveCollisionVolumes();
    ms_colModelDoor1.RemoveCollisionVolumes();
    ms_colModelBumper1.RemoveCollisionVolumes();
    ms_colModelPanel1.RemoveCollisionVolumes();
    ms_colModelBonnet1.RemoveCollisionVolumes();
    ms_colModelBoot1.RemoveCollisionVolumes();
    ms_colModelWheel1.RemoveCollisionVolumes();
    ms_colModelBodyPart1.RemoveCollisionVolumes();
    ms_colModelBodyPart2.RemoveCollisionVolumes();
    ms_colModelWeapon.RemoveCollisionVolumes();
    rng::for_each(ms_colModelCutObj, &CColModel::RemoveCollisionVolumes);
}
