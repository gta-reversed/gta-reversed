#pragma once

#include "TaskComplex.h"

class CTask;

class NOTSA_EXPORT_VTABLE CTaskComplexSequence : public CTaskComplex {
    enum class eRepeatMode : uint32 {
        DONT_REPEAT,
        FOREVER,
    };
public:
    int32       m_CurrTaskIdx{};  //!< Current running task [Indexing `m_Tasks`]
    CTask*      m_Tasks[8]{};     //!< The sequenced tasks
    eRepeatMode m_RepeatMode{eRepeatMode::DONT_REPEAT};
    int32       m_RepeatedCnt{};  //!< This simply tells us how many times the sequence has been repeated. If `m_RepeatMode` is `FOREVER`, it can be greater than 1.
    bool        m_bFlushTasks{};  
    uint32      m_RefCnt{};       //!< count of how many CTaskComplexUseSequence instances are using this sequence

public:
    static constexpr auto Type = TASK_COMPLEX_SEQUENCE;

    //! Constructor
    CTaskComplexSequence() = default;

    //! Construct using multiple tasks, same as constructing and then calling `AddTask` for every task passed in.
    template<Task... Ts>
    CTaskComplexSequence(Ts*... ts) :
        CTaskComplexSequence{}
    {
        AddTasks(ts...);
    }

    CTaskComplexSequence(const CTaskComplexSequence&);
    ~CTaskComplexSequence() override;

    eTaskType GetTaskType() const override { return Type; } // 0x632C60
    CTask* Clone() const override { return new CTaskComplexSequence{*this}; } // 0x5F6710
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void AddTask(CTask* task);

    //! @brief Add multiple tasks at once
    template<Task... Ts>
    void AddTasks(Ts*... ts) { (AddTask(ts), ...); }

    void AddTask(int32 id, CTask* task);
    CTask* CreateNextSubTask(CPed* ped, int32& taskIndex, int32& repeatCount) const;
    void Flush();
    bool Contains(eTaskType taskType) const;
    void SetCanBeEmptied(bool flush);
    void DeRegister();
    void SetRepeatMode(bool repeat);
    bool IsActive() const { return m_Tasks[0] != nullptr; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexSequence* Constructor();
};
VALIDATE_SIZE(CTaskComplexSequence, 0x40);
