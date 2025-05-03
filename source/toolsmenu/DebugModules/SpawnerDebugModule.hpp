#pragma once

#include "DebugModule.h"

class SpawnerDebugModule final : public DebugModule {
    class EntitySpawner {
    protected:
        struct Item {
            eModelID    Model;
            const char* Name;
        };

    public:
        EntitySpawner(std::vector<Item> items) :
            m_Items{ std::move(items) }
        {
        }
        virtual ~EntitySpawner() = default;

        void Render();
        virtual CPhysical* Spawn(const Item& item) = 0;
    protected:
        std::vector<Item> m_Items{};
        ImGuiTextFilter   m_Filter{};
        eModelID          m_Selected{};
    };

    class VehicleSpawner final : public EntitySpawner {
    public:
        VehicleSpawner();

        CPhysical* Spawn(const Item& item) override;
    };

    class PedSpawner final : public EntitySpawner {
    public:
        PedSpawner();

        CPhysical* Spawn(const Item& item) override;
    };

public:
    SpawnerDebugModule();

    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(SpawnerDebugModule, m_IsOpen);

private:
    bool m_IsOpen{};

    VehicleSpawner m_VehicleSpawner{};
    PedSpawner     m_PedSpawner{};
};
