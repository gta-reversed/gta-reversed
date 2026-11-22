#include "StdInc.h"
#include "VMTInfo.h"
#include "dllmain.h"
#include "HooksUtility.hpp"

namespace {
// Really fucking simple name mangling for msvc
// Check this out: https://en.m.wikiversity.org/wiki/Visual_C%2B%2B_name_mangling
void MangleClassNameMSVC(CHAR* out, std::string_view name) {
    if (const auto openerPos = name.find('<'); openerPos != std::string_view::npos) { // Templated class, this only works for single templated classes (for now)
        // ??_7?$CTaskComplexSeekEntity@VCEntitySeekPosCalculatorStandard@@@@6B@
        const auto closerPos = name.rfind('>');
        *std::format_to(out, "??_7?${}@V{}@@@@6B@", name.substr(0, openerPos), name.substr(openerPos + 1, closerPos - openerPos - 1)) = 0;
    } else {
        // ??_7CTaskSimple@@6B@
        *std::format_to(out, "??_7{}@@6B@", name) = 0;
    }
}

void** FindVMTAddressByClassName(std::string_view className) {
    // The VTable is exported as a symbol, in the format `??_7<class name>@@6B@` where `<class name>` is the name of the class.
    // In order for this to work the class has to be exported (So the `NOTSA_EXPORT_VTABLE` macro has to be used)
    CHAR mangledName[1024];
    MangleClassNameMSVC(mangledName, className);
    if (const auto address = reinterpret_cast<void**>(GetProcAddress(notsa::GetDLLHandle(), mangledName))) {
        return address;
    }
    throw std::runtime_error{ std::format("Failed to find VMT for class '{}'", className) };
}
};

auto ReversibleHooks::Utility::VMTInfo::FindByClassName(const char* name, size_t size) -> VMTInfo {
    return VMTInfo{ FindVMTAddressByClassName(name), size };
}

size_t ReversibleHooks::Utility::VMTInfo::FindIndexOf(void* fn) {
    ReversibleHooks::Utility::ScopedVirtualProtectModify g{ m_Table, m_Size * sizeof(*m_Table) };
    for (size_t i = 0; i < m_Size; i++) {    
        if (m_Table[i] == fn) {
            return i;
        }
    }
    throw std::runtime_error{ std::format("Failed to find function `{}` in VMT", fn) };
}
