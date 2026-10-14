#pragma once

#include "DebugModule.h"
#include <difftests/DiffTest.hpp>

namespace notsa {
namespace debugmodules {
/*!
 * @brief Runs the differential tests (`NOTSA_DIFFTEST`) that compare reversed functions with the original game code.
 */
class DiffTestsDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(DiffTestsDebugModule, m_IsOpen, m_Seed);

private:
    void Run(size_t idx);

private:
    bool                                      m_IsOpen{};
    int32                                     m_Seed{ 0x5A5A1234 };
    ImGuiTextFilter                           m_NameFilter{};
    std::vector<notsa::difftest::TestResult>  m_Results{};
};
}; // namespace debugmodules
}; // namespace notsa
