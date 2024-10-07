#pragma once

namespace notsa {
// Implementation of CPointRoute and CNodeRoute [As they basically do the same shit]
// TODO/NOTE: This shit is mostly just a simplified fixed vector implementation :D
template<typename T>
class Route {
public:
    //! Get all active entries
    auto GetAll()       { return m_Entries | rng::views::take(m_NumEntries); }
    auto GetAll() const { return m_Entries | rng::views::take(m_NumEntries); }

    //! Are there are no entries
    bool IsEmpty() const { return m_NumEntries == 0; }

    //! Is there space for more entries
    bool IsFull()  const { return (size_t)m_NumEntries >= std::size(m_Entries); }

    //! Add an entry (Doesn't check whenever there's space for it)
    template<typename... T_Entries>
    void Add(T_Entries&&... entry) { ((m_Entries[m_NumEntries++] = entry), ...); }

    //! Add an entry if we aren't full yet
    void AddUnlessFull(const CVector& entry) {
        if (!IsFull()) {
            Add(entry);
        }
    }

    //! Add entries if there's space for them
    template<typename... T_Points>
    void AddMultipleUnlessFull(T_Points&&... entry) { (AddUnlessFull(entry), ...); }

    //! Reverse the order of entries in-place
    void Reverse() { rng::reverse(GetAll()); }

    //! Clear all entries
    void Clear() { m_NumEntries = 0; }

    //! Get the number of entries
    auto GetSize() const { return m_NumEntries; }

    //! Resize to the given size
    void ResizeTo(uint32 s) { assert(s < m_Entries.size()); m_NumEntries = s; }

    T& operator[](size_t idx)       { return GetAll()[idx]; }
    T  operator[](size_t idx) const { return GetAll()[idx]; }

public: // TODO: Make private
    uint32           m_NumEntries{};
    std::array<T, 8> m_Entries{};
};
};
