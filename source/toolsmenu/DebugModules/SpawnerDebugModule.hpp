#pragma once

#include "DebugModule.h"

#include "Enums/eModelID.h"

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

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(EntitySpawner, m_Filter, m_Selected);

    protected:
        std::vector<Item> m_Items{};
        ImGuiTextFilter   m_Filter{};
        eModelID          m_Selected{ MODEL_INVALID };
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
    SpawnerDebugModule() = default;

    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(SpawnerDebugModule, m_IsOpen, m_VehicleSpawner, m_PedSpawner);

private:
    bool m_IsOpen{};

    VehicleSpawner m_VehicleSpawner{};
    PedSpawner     m_PedSpawner{};
};
