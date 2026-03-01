#pragma once

class CEvent;
class CEntity;
class CPed;
class CPedGroup;

class CInformGroupEvent {
public:
    CPed*      m_Ped{};
    CPedGroup* m_PedGroup{};
    CEvent*    m_Event{};
    int32      m_Time{-1};

public:
    static void InjectHooks();

    // 0x4AC350
    CInformGroupEvent() = default;

    ~CInformGroupEvent();
    void Process();

private: // Wrappers for hooks
    CInformGroupEvent* Constructor() {
        this->CInformGroupEvent::CInformGroupEvent();
        return this;
    }

    CInformGroupEvent* Destructor() {
        this->CInformGroupEvent::~CInformGroupEvent();
        return this;
    }
};

VALIDATE_SIZE(CInformGroupEvent, 0x10);
