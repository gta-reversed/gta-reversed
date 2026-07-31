#pragma once

template<typename T, typename Deleter = std::default_delete<T>, typename Alloc = std::allocator<T>>
using HeapPtrArray = std::vector<std::unique_ptr<T, Deleter>, Alloc>;

// This class has to be heap allocated! (with `new`)
class OALBase {
public:
    OALBase();
    virtual ~OALBase();

    virtual void AddRef() { ++m_RefCount; }
    virtual void Release();

protected:
    uint32 m_RefCount;

private:
    // Number of living `OALBase` object instances.
    static inline uint32 s_LivingCount{};

    // To be garbage-collected OpenAL objects.
    static inline HeapPtrArray<OALBase> s_TrashCan{};
    static inline void*                 s_TrashMutex{};
};

bool OALCheckErrors(std::string_view file, int32 line);
#define OAL_CHECKED(c) (c, OALCheckErrors(__FILE__, __LINE__))
