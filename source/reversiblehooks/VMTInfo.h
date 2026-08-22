#pragma once

namespace ReversibleHooks {
namespace Utility {
class VMTInfo {
public:
    static constexpr auto DESTRUCTOR_VMT_INDEX = 0;

public:
    VMTInfo(void** table, size_t size) :
        m_Table(table),
        m_Size(size)
    {
        assert(size > 0);
    }

    /*!
     * @brief Finds the VMT information for a class by its name.
     * @param name The name of the class.
     * @param size The size of the VMT.
     * @return The VMTInfo structure containing the address and size of the VMT.
     * @throws `std::runtime_error` if the VMT is not found.
     */
    static auto FindByClassName(const char* name, size_t size) -> VMTInfo;

    /*!
     * @brief Finds the index of a function in the VMT.
     * @param fn The function pointer to search for.
     * @return The index of the function in the VMT
     * @throws `std::runtime_error` if the function is not found in the VMT.
     */
    size_t FindIndexOf(void* fn);

    /*!
     * @brief Finds the address of a function in the VMT.
     */
    void** FindEntryAddressOf(void* fn) {
        return GetEntryAddressAt(FindIndexOf(fn));
    }

    /*! 
     * @brief Get entry address from index
     */
    void** GetEntryAddressAt(size_t idx) {
        assert(idx < m_Size);
        return &m_Table[idx];
    }

    /*!
     * @return Get function pointer at index
     */
    void* GetFunctionAt(size_t idx) {
        assert(idx < m_Size);
        return m_Table[idx];
    }

    /*!
     * @return The address of the VMT.
     */
    auto GetAddress() const noexcept { return m_Table; }

    /*!
     * @return The size of the VMT.
     */
    auto GetSize() const noexcept { return m_Size; }

private:
    void** m_Table;
    size_t m_Size;
};
};
};
