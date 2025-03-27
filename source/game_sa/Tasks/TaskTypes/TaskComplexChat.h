#pragma once

#include "TaskComplex.h"
#include <Audio/Enums/PedSpeechContexts.h>

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexChat : public CTaskComplex {
public:
    bool                 m_IsChatter{};
    CPed*                m_ChatPartner{};
    int32                m_Stage{};
    eGlobalSpeechContext m_GlobalSpeechContext{};
    bool                 m_TaskFinished{};
    int32                m_NoChatTimeout{};

public:
    static constexpr auto Type = TASK_COMPLEX_CHAT;

    static void InjectHooks();

    CTaskComplexChat(bool isChatter, CPed* partner, int32 stage, int16 globalSpeechContext);
    CTaskComplexChat(const CTaskComplexChat&);
    ~CTaskComplexChat() override;

    eTaskType GetTaskType() const override { return Type; } // 0x682D30
    CTask*    Clone() const       override { return new CTaskComplexChat{*this}; }
    CTask*    ControlSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    CreateNextSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x682CB0
    CTaskComplexChat* Constructor(uint8 a, CPed* b, int32 c, int16 d) {
        this->CTaskComplexChat::CTaskComplexChat(a, b, c, d);
        return this;
    }

    // 0x682D40
    CTaskComplexChat* Destructor() {
        this->CTaskComplexChat::~CTaskComplexChat();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexChat, 0x20);
