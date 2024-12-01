/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <execution>

#define INVALID_POOL_SLOT (-1)

/*
    R* terminology      Our terminology
    JustIndex           Index
    Index               Id
    Ref                 Handle/Ref
*/

union tPoolObjectFlags {
    struct {
        uint8 nId : 7;    // Mask: 0x7F
        bool  bEmpty : 1; // Mask: 0x80
    };

private:
    uint8 nValue;

public:
    void Init() {
        bEmpty = true;
        nId = 0;
    }

    uint8 IntValue() {
        return nValue;
    }
};

VALIDATE_SIZE(tPoolObjectFlags, 1);

// `DontDebugCheckAlloc` is NOTSA, used to skip allocation fail checking, as some places actually handle it correctly.
template <class A, class B = A, bool DontDebugCheckAlloc = false> class CPool {
public:
    // NOTSA typenames
    using base_type = A;   // Common base of all these objects
    using widest_type = B; // Type using the most memory (So each object takes this much memory basically)

    B*                m_pObjects;
    tPoolObjectFlags* m_byteMap;
    int32             m_nSize;
    int32             m_nFirstFree;
    bool              m_bOwnsAllocations;
    bool              m_bIsLocked; // Seemingly not used anywhere, only toggled on/off

    // Default constructor for statically allocated pools
    CPool() {
        // Remember to call CPool::Init to fill in the fields!
        m_pObjects = nullptr;
        m_byteMap = nullptr;
        m_nSize = 0;
        m_bOwnsAllocations = false;
    }

    // Initializes pool
    CPool(int32 size, const char* name) {
        m_pObjects = static_cast<B*>(operator new(sizeof(B) * size));
        assert(m_pObjects);

        m_byteMap  = new tPoolObjectFlags[size]();
        assert(m_byteMap);

        m_nSize = size;
        m_nFirstFree = -1;
        m_bOwnsAllocations = true;
        for (auto i = 0; i < size; ++i) {
            m_byteMap[i].Init();
        }
    }

    ~CPool() {
        Flush();
    }

    // Initialises a pool with pre-allocated
    // To be called one-time-only for statically allocated pools.
    void Init(int32 size, void* objects, void* infos) {
        assert(m_pObjects == nullptr); // Since we statically allocated the pools we do not deallocate.

        m_pObjects = static_cast<B*>(objects);
        m_byteMap  = static_cast<tPoolObjectFlags*>(infos);
        m_nSize = size;
        m_nFirstFree = -1;
        m_bOwnsAllocations = false;
        for (auto i = 0; i < size; ++i) {
            m_byteMap[i].Init();
        }
    }

    // Shutdown pool
    void Flush() {
        if (m_bOwnsAllocations) {
            delete[] m_pObjects;
            delete[] m_byteMap;
        }
        m_pObjects = nullptr;
        m_byteMap = nullptr;
        m_nSize = 0;
        m_nFirstFree = 0;
    }

    // Clears pool
    void Clear() {
        for (auto i = 0; i < m_nSize; i++)
            m_byteMap[i].bEmpty = true;
    }

    auto GetSize() {
        return m_nSize;
    }

    // Returns if specified slot is free
    // 0x404940
    bool IsFreeSlotAtIndex(int32 idx) const {
        assert(IsIndexInBounds(idx));
        return m_byteMap[idx].bEmpty;
    }

    // Returns slot index for this object
    int32 GetIndex(const A* obj) const {
        assert(IsFromObjectArray(obj));
        return reinterpret_cast<const B*>(obj) - m_pObjects;
    }

    // Returns pointer to object by slot index
    A* GetAt(int32 idx) {
        assert(IsIndexInBounds(idx));
        return !IsFreeSlotAtIndex(idx) ? (A*)&m_pObjects[idx] : nullptr;
    }

    A* GetAtNoChecks(int32 idx) {
        assert(IsIndexInBounds(idx));
        return (A*)&m_pObjects[idx];
    }

    // Marks slot as free / used (0x404970)
    void SetFreeAt(int32 idx, bool bFree) {
        assert(IsIndexInBounds(idx));
        m_byteMap[idx].bEmpty = bFree;
    }

    // Set new id for slot (0x54F9F0)
    void SetIdAt(int32 idx, uint8 id) {
        assert(IsIndexInBounds(idx));
        m_byteMap[idx].nId = id;
    }

    // Get id for slot (0x552200)
    uint8 GetIdAt(int32 idx) {
        assert(IsIndexInBounds(idx));
        return m_byteMap[idx].nId;
    }

    // Allocates object
    A* New() {
        bool bReachedTop = false;
        do {
            if (++m_nFirstFree >= m_nSize) {
                if (bReachedTop) {
                    m_nFirstFree = -1;
                    if constexpr (DontDebugCheckAlloc) {
                        DEV_LOG("Allocataion failed!"); // Code can handle alloc failures
                    } else {
                        NOTSA_DEBUG_BREAK(); // Code can't handle alloc failures, so break
                    }
                    return nullptr;
                }
                bReachedTop = true;
                m_nFirstFree = 0;
            }
        } while (!m_byteMap[m_nFirstFree].bEmpty);

        m_byteMap[m_nFirstFree].bEmpty = false;
        ++m_byteMap[m_nFirstFree].nId;

        return &m_pObjects[m_nFirstFree];
    }

    // Allocates object at a specific index from a SCM handle (ref) (0x59F610)
    void CreateAtRef(int32 ref) {
        const auto idx = GetIndexFromRef(ref); // GetIndexFromRef asserts if idx out of range
        m_byteMap[idx].bEmpty = false;
        m_byteMap[idx].nId = ref & 0x7F;
        m_nFirstFree = 0;
        while (!m_byteMap[m_nFirstFree].bEmpty) // Find next free
            ++m_nFirstFree;
    }

    // 0x5A1C00
    /*!
    * @brief Allocate object at ref
    * @returns A ptr to the object at ref
    */
    A* NewAt(int32 ref) {
        // TODO/NOTE: Maybe check if where we're allocating at is free?
        A* result = &m_pObjects[GetIndexFromRef(ref)]; // GetIndexFromRef asserts if idx out of range
        CreateAtRef(ref);
        return result;
    }

    // Deallocates object
    void Delete(A* obj) {
#ifdef FIX_BUGS // C++ says that `delete nullptr` is well defined, and should do nothing.
        if (!obj) {
            return;
        }
#endif
        int32 index = GetIndex(obj);
        m_byteMap[index].bEmpty = true;
        if (index < m_nFirstFree)
            m_nFirstFree = index;
    }

    // Returns SCM handle (ref) for object (0x424160)
    int32 GetRef(const A* obj) {
        const auto idx = GetIndex(obj);
        return (idx << 8) + m_byteMap[idx].IntValue();
    }

    // Returns pointer to object by SCM handle (ref)
    A* GetAtRef(int32 ref) {
        int32 idx = ref >> 8; // It is possible the ref is invalid here, thats why we check for the idx is valid below (And also why GetIndexFromRef isn't used, it would assert)
        return IsIndexInBounds(idx) && m_byteMap[idx].IntValue() == (ref & 0xFF)
            ? reinterpret_cast<A*>(&m_pObjects[idx])
            : nullptr;
    }

    A* GetAtRefNoChecks(int32 ref) {
        return GetAt(GetIndexFromRef(ref));
    }

    /*!
    * @addr 0x54F6B0
    * @brief Calculate the number of used slots. CAUTION: Slow, especially for large pools.
    */
    size_t GetNoOfUsedSpaces() {
        return (size_t)std::count_if(m_byteMap, m_byteMap + m_nSize, [](auto&& v) { return !v.bEmpty; });
    }

    auto GetNoOfFreeSpaces() {
        return m_nSize - GetNoOfUsedSpaces();
    }

    // 0x54F690
    auto GetObjectSize() {
        return sizeof(B);
    }

    // 0x5A1CD0
    bool IsObjectValid(const A *obj) const {
        return IsFromObjectArray(obj) && !IsFreeSlotAtIndex(GetIndex(obj));
    }

    // Helper so we don't write memcpy manually
    void CopyItem(A* dest, A* src) {
        *reinterpret_cast<B*>(dest) = *reinterpret_cast<B*>(src);
    }

    //
    // NOTSA section
    //

    // Check if index is in array bounds
    [[nodiscard]] bool IsIndexInBounds(int32 idx) const {
        return idx >= 0 && idx < m_nSize;
    }

    // Check if object pointer is inside object array (e.g.: It's index is in the bounds of the array)
    bool IsFromObjectArray(const A* obj) const {
        return obj >= m_pObjects && obj < m_pObjects + m_nSize;
    }

    // Get slot index from ref
    int32 GetIndexFromRef(int32 ref) {
        const auto idx = ref >> 8;
        assert(IsIndexInBounds(idx));
        return idx;
    }

    // NOTSA - Get all valid objects - Useful for iteration
    template<typename T = A&>
    auto GetAllValid() {
        using namespace std;
        return span{ m_pObjects, (size_t)m_nSize }
            | rngv::filter([this](auto&& obj) { return !IsFreeSlotAtIndex(GetIndex(&obj)); }) // Filter only slots in use
            | rngv::transform([](auto&& obj) -> T {
                if constexpr (std::is_pointer_v<T>) { // For pointers we also do an address-of
                    return static_cast<T>(&obj);
                } else {
                    return static_cast<T>(obj);
                }
            });
    }

    // Similar to above, but gives back a pair [index, object]
    template<typename T = A>
    auto GetAllValidWithIndex() {
        return GetAllValid<T&>()
             | rng::views::transform([this](auto&& obj) { return std::make_pair(GetIndex(&obj), std::ref(obj)); });
    }
};
VALIDATE_SIZE(CPool<int32>, 0x14);
