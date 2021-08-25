#include "StdInc.h"

static std::map<std::string, std::vector<std::shared_ptr<SReversibleHook>>> m_HooksMap;

namespace ReversibleHooks{

std::map<std::string, std::vector<std::shared_ptr<SReversibleHook>>>& GetAllHooks() {
    return m_HooksMap;
}

void UnHook(const std::string& className, const char* functionName)
{
    if (className.empty())
        return;
    const auto& allHooks = GetAllHooks();
    if (functionName) {
        for (auto& classHooks : allHooks) {
            if (classHooks.first == className) {
                for (auto& hook : classHooks.second) {
                    if (hook->m_bIsHooked && strcmp(hook->m_sFunctionName.c_str(), functionName) == 0) {
                        hook->Switch();
                        printf("UnHooked %s::%s\n", className.c_str(), functionName);
                        return;
                    }
                }
            }
        }
    } else {
        for (auto& classHooks : allHooks) {
            if (classHooks.first == className) {
                for (auto& hook : classHooks.second) {
                    if (hook->m_bIsHooked)
                        hook->Switch();
                }
                printf("UnHooked class %s\n", className.c_str());
                return;
            }
        }
    }
}

namespace detail {
void HookInstall(const std::string& sIdentifier, const std::string& sFuncName, unsigned int installAddress, void* addressToJumpTo, int iJmpCodeSize, bool bDisableByDefault)
{
    assert(!GetHook(sIdentifier, sFuncName));

    auto& usedVector = m_HooksMap[sIdentifier];
    usedVector.emplace_back(std::make_shared<SSimpleReversibleHook>(sIdentifier, sFuncName, installAddress, addressToJumpTo, iJmpCodeSize));
    if (bDisableByDefault)
        usedVector.back()->Switch();
}

void HookInstallVirtual(const std::string& sIdentifier, const std::string& sFuncName, void* libVTableAddress, const std::vector<uint32_t>& vecAddressesToHook)
{
    assert(!GetHook(sIdentifier, sFuncName));
    m_HooksMap[sIdentifier].emplace_back(std::make_shared<SVirtualReversibleHook>(sIdentifier, sFuncName, libVTableAddress, vecAddressesToHook));
}

void HookSwitch(std::shared_ptr<SReversibleHook> pHook)
{
    pHook->Switch();
}

bool IsFunctionHooked(const std::string& sIdentifier, const std::string& sFuncName)
{
    auto hook = GetHook(sIdentifier, sFuncName);
    if (!hook)
        return false;

    return hook->m_bIsHooked;
}

std::shared_ptr<SReversibleHook> GetHook(const std::string& sIdentifier, const std::string& sFuncName)
{
    if (m_HooksMap.find(sIdentifier) == m_HooksMap.end())
        return nullptr;

    auto& vecHooks = m_HooksMap[sIdentifier];
    auto findResult = std::find_if(vecHooks.begin(), vecHooks.end(), [&](const std::shared_ptr<SReversibleHook> hook) {return hook->m_sFunctionName == sFuncName; });
    if (findResult == vecHooks.end())
        return nullptr;

    return *findResult;
}
}; // namespace detail

unsigned int GetJMPLocation(unsigned int dwFrom, unsigned int dwTo)
{
    return dwTo - dwFrom - x86JMPSize;
}

unsigned int GetFunctionLocationFromJMP(unsigned int dwJmpLoc, unsigned int dwJmpOffset)
{
    return dwJmpOffset + dwJmpLoc + x86JMPSize;
}
}; // namespace ReversibleHooks

SReversibleHook::SReversibleHook(std::string id, std::string name, eReversibleHookType type) :
    m_sIdentifier(std::move(id)),
    m_sFunctionName(std::move(name)),
    m_eHookType(type)
{
}
