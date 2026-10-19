#pragma once

template<typename T, typename Deleter = std::default_delete<T>, typename Alloc = std::allocator<T>>
using HeapPtrArray = std::vector<std::unique_ptr<T, Deleter>, Alloc>;

// This class has to be heap allocated! (with `new`)
class OALBase {
public:
    // Number of living `OALBase` object instances.
    static inline uint32 livingCount{};

    // To be garbage collected OpenAL objects.
    static inline HeapPtrArray<OALBase> trashCan{};

    uint32 m_refCount;

public:
    // NOTSA: possibly inlined in SA.
    OALBase();

    virtual ~OALBase();
    virtual void Release();

    // NOTSA
    virtual void AddRef();
};

bool OALCheckErrors(std::string_view file, int32 line);
#define OAL_CHECKED(c) (c, OALCheckErrors(__FILE__, __LINE__))
