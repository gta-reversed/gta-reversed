#pragma once

namespace ReversibleHooks {
namespace Utility {
struct VMTInfo {
    /*!
     * @brief Finds the VMT information for a class by its name.
     * @param name The name of the class.
     * @param size The size of the VMT.
     * @return The VMTInfo structure containing the address and size of the VMT.
     * @throws `std::runtime_error` if the VMT is not found.
     */
    static VMTInfo FindByClassName(const char* name, size_t size);

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
