#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

#include <Enums/eScriptCommands.h>
#include "ReversibleHook/Base.h"

//
// Helper macros - For help regarding usage see how they're used (`Find all references` and take a look)
// Generally on top of `InjectHooks` you will need to call `RH_ScopedClass` (or `RH_ScopedNamespace`) and `RH_ScopedCategory`
// `RH_ScopedCategory` generally follows the directory layout - Anything in, lets say, the `Entity` directory should be in the `Entity` category.
// The root folder (source/game_sa) can be referred to by `RH_ScopedCategoryGlobal()` - Thus any source files in it should use this category.
//

// Set scoped namespace name (This only works if you only use `ScopedGlobal` macros)
#define RH_ScopedNamespaceName(ns) \
    ReversibleHooks::ScopeName RHCurrentScopeName {ns};

// Use when `name` is a class
#define RH_ScopedClass(cls) \
    using RHCurrentNS = cls; \
    ReversibleHooks::ScopeName RHCurrentScopeName {#cls};

// Use when `name` is a class
#define RH_ScopedNamedClass(cls, name) \
    using RHCurrentNS = cls; \
    ReversibleHooks::ScopeName RHCurrentScopeName {name};

#define RH_ScopedVirtualClass(cls, addrGTAVtbl, nVirtFns_) \
    using RHCurrentNS = cls; \
    ReversibleHooks::ScopeName RHCurrentScopeName {#cls}; \
    const auto pGTAVTbl = (void**)addrGTAVtbl; \
    const auto pOurVTbl = ReversibleHooks::detail::GetVTableAddress(#cls); \
    const auto nVirtFns = nVirtFns_; \

// Use when `name` is a namespace
#define RH_ScopedNamespace(name) \
    namespace RHCurrentNS = name; \
    ReversibleHooks::ScopeName RHCurrentScopeName {#name};

// Supports nested categories separeted by `/`. Eg.: `Entities/Ped`
#define RH_ScopedCategory(name) \
    ReversibleHooks::ScopeCategory  RHCurrentCat{name};

#define RH_RootCategoryName "Root"
#define RH_GlobalCategoryName "Global"
#define RH_ScopedCategoryGlobal() \
    ReversibleHooks::ScopeCategory  RHCurrentCat{ RH_GlobalCategoryName };

// Install a hook living in the current scoped class/namespace
#define RH_ScopedInstall(fn, fnAddr, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, fnAddr, &RHCurrentNS::fn __VA_OPT__(,) __VA_ARGS__)

// Install a hook on a global function
#define RH_ScopedGlobalInstall(fn, fnAddr, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, fnAddr, &fn __VA_OPT__(,) __VA_ARGS__)

// Tip: If a member function is const just add the `const` keyword after the function arg list;
// Eg.: `void(CRect::*)(float*, float*) const` (Notice the const at the end) (See function `CRect::GetCenter`)
#define RH_ScopedOverloadedInstall(fn, suffix, fnAddr, addrCast, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, fnAddr, static_cast<addrCast>(&RHCurrentNS::fn) __VA_OPT__(,) __VA_ARGS__)

#define RH_ScopedGlobalOverloadedInstall(fn, suffix, fnAddr, addrCast, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, fnAddr, static_cast<addrCast>(&fn) __VA_OPT__(,) __VA_ARGS__)

// Used in CCheat only - Install global `fn` as name `fnName`
#define RH_ScopedNamedGlobalInstall(fn, fnName, fnAddr, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, fnName, fnAddr, &fn __VA_OPT__(,) __VA_ARGS__)

// Similar to RH_ScopedInstall but you can specify the name explicitly.
#define RH_ScopedNamedInstall(fn, fnName, fnAddr, ...) \
    ReversibleHooks::Install(RHCurrentCat.name + "/" + RHCurrentScopeName.name, fnName, fnAddr, &RHCurrentNS::fn __VA_OPT__(,) __VA_ARGS__)

#define RH_ScopedVMTOverloadedInstall(fn, suffix, fnGTAAddr, addrCast, ...) \
    ReversibleHooks::InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, pGTAVTbl, pOurVTbl, (void*)fnGTAAddr, FunctionPointerToVoidP(static_cast<addrCast>(&fn)), nVirtFns __VA_OPT__(,) __VA_ARGS__)

// Install a hook on a virtual function. To use it, `RH_ScopedVirtualClass` must be used instead of `RH_ScopedClass`
#define RH_ScopedVMTInstall(fn, fnGTAAddr, ...) \
    ReversibleHooks::InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, pGTAVTbl, pOurVTbl, (void*)fnGTAAddr, FunctionPointerToVoidP(&RHCurrentNS::fn), nVirtFns __VA_OPT__(,) __VA_ARGS__)

//! Install a script hook
#define RH_ScopedInstallScriptCommand(cmd) \
    ReversibleHooks::InstallScriptCommand(RHCurrentCat.name + "/" + RHCurrentScopeName.name, cmd)

//#define RH_ScopedVMTAddressChange(fn, fnGTAAddr, ...) \
//    ReversibleHooks::InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, pGTAVTbl, pOurVTbl, FunctionPointerToVoidP(fnGTAAddr), nVirtFns __VA_OPT__(,) __VA_ARGS__)

namespace ReversibleHooks {
    class RootHookCategory;

    struct ScopeName {
        std::string name{};
    };

    struct ScopeCategory {
        std::string name{};
    };

    struct HookInstallOptions {
        bool reversed{ true };          // Has this function been reversed?
        bool enabled{ reversed };       // Is this hook enabled (eg.: redirects GTA calls to ours or vice versa if disabled) by default?
        bool locked{ !reversed };       // If this hook shouldn't be switchable from the GUI
        int jmpCodeSize{ 5 };
        int stackArguments{ -1 };
    };

    RootHookCategory& GetRootCategory();

    enum class SetCatOrItemStateResult {
        NotFound,
        Locked,
        Done
    };

    SetCatOrItemStateResult SetCategoryOrItemStateByPath(std::string_view path, bool enabled);

    namespace detail {
        // Change protection of memory pages, and automatically rollback on scope exit
        struct ScopedVirtualProtectModify {
            ScopedVirtualProtectModify(LPVOID address, SIZE_T sz, DWORD newProtect = PAGE_EXECUTE_READWRITE) :
                m_addr{ address },
                m_sz{ sz }
            {
                if (VirtualProtect(address, sz, newProtect, &m_initialProtect) == 0) {
                    assert(0); // Failed
                }
            }

            ~ScopedVirtualProtectModify() {
                DWORD oldProtect{};
                if (VirtualProtect(m_addr, m_sz, m_initialProtect, &oldProtect) == 0) {
                    assert(0); // Failed
                }
            }

        private:
            DWORD  m_initialProtect{};
            LPVOID m_addr{};
            DWORD  m_sz{};
        };


        void HookInstall(std::string_view category, std::string fnName, uint32 installAddress, void* addressToJumpTo, HookInstallOptions&& opt);

        /*void HookSwitch(std::shared_ptr<SReversibleHook> pHook);
        bool IsFunctionHooked(const std::string& category, const std::string& fnName);
        std::shared_ptr<SReversibleHook> GetHook(const std::string& category, const std::string& fnName);*/
        void VirtualCopy(void* dst, void* src, size_t nbytes);

        void** GetVTableAddress(std::string_view name);
    };

    template <typename T>
    static void Install(std::string_view category, std::string fnName, DWORD installAddress, T addressToJumpTo, HookInstallOptions&& opt = {}) {
        auto ptr = FunctionPointerToVoidP(addressToJumpTo);
        detail::HookInstall(category, std::move(fnName), installAddress, ptr, std::move(opt));
    }

    void InstallVirtual(std::string_view category, std::string fnName, void** vtblGTA, void** vtblOur, void* fnGTAAddr, void* fnOurAddr, size_t nVirtFns, const HookInstallOptions& opt = {});

    /*!
    * @param category Category's path, eg.: "Global/"
    * @param item     Item to add
    */
    void AddItemToCategory(std::string_view category, std::shared_ptr<ReversibleHook::Base> item);

    void InstallScriptCommand(std::string_view category, eScriptCommands cmd);

    /*static void Switch(std::shared_ptr<SReversibleHook> pHook) {
        detail::HookSwitch(pHook);
    }*/

    void CheckAll();
    void SwitchHook(std::string_view funcName);

    // Stuff called from InjectHooksMain()

    void OnInjectionBegin(HMODULE hModule);
    void OnInjectionEnd();

    void WriteHooksToFile(const std::filesystem::path&);
};
