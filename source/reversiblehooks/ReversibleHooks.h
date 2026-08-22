#pragma once

#include <string>

#include "VMTInfo.h"
#include "HooksUtility.hpp"
#include "RHManager.h"

//
// Helper macros - For help regarding usage see how they're used (`Find all references` and take a look)
// Generally on top of `InjectHooks` you will need to call `RH_ScopedClass` (or `RH_ScopedNamespace`) and `RH_ScopedCategory`
// `RH_ScopedCategory` generally follows the directory layout - Anything in, lets say, the `Entity` directory should be in the `Entity` category.
// The root folder (source/game_sa) can be referred to by `RH_ScopedCategoryGlobal()` - Thus any source files in it should use this category.
//

// Private, dont use this
#define RH_InstallProlouge(_scopeName) \
    ReversibleHooks::ScopeName RHCurrentScopeName {_scopeName}; \
    using HS  = ReversibleHooks::RHManager::HookInstallOptions::HS; \
    auto* const rh = &ReversibleHooks::RHManager::GetInstance();

// Set scoped namespace name (This only works if you only use `ScopedGlobal` macros)
#define RH_ScopedNamespaceName(ns) \
    RH_InstallProlouge(ns) \

// Use when `name` is a class
#define RH_ScopedClass(cls) \
    RH_InstallProlouge(#cls) \
    using RHCurrentNS = cls;

// Use when `name` is a class
#define RH_ScopedNamedClass(cls, name) \
    RH_InstallProlouge(name) \
    using RHCurrentNS = cls;

#define RH_ScopedVirtualClass(cls, addrGTAVtbl, nVirtFns_) \
    RH_InstallProlouge(#cls) \
    const auto pGTAVTbl = ReversibleHooks::Utility::VMTInfo{ (void**)addrGTAVtbl, nVirtFns_ }; \
    const auto pOurVTbl = ReversibleHooks::Utility::VMTInfo::FindByClassName(#cls, nVirtFns_); \
    using RHCurrentNS = cls;

// Use when `name` is a namespace
#define RH_ScopedNamespace(name) \
    RH_InstallProlouge(#name) \
    namespace RHCurrentNS = name;

// Supports nested categories separeted by `/`. Eg.: `Entities/Ped`
#define RH_ScopedCategory(name) \
    ReversibleHooks::ScopeCategory  RHCurrentCat{name};

#define RH_GlobalCategoryName "Global"
#define RH_ScopedCategoryGlobal() \
    ReversibleHooks::ScopeCategory  RHCurrentCat{ RH_GlobalCategoryName };

// Install a hook living in the current scoped class/namespace
#define RH_ScopedInstall(fn, _addressGTA, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, _addressGTA, &RHCurrentNS::fn __VA_OPT__(,) __VA_ARGS__)

// Install a hook on a global function
#define RH_ScopedGlobalInstall(fn, _addressGTA, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, _addressGTA, &fn __VA_OPT__(,) __VA_ARGS__)

// Tip: If a member function is const just add the `const` keyword after the function arg list;
// Eg.: `void(CRect::*)(float*, float*) const` (Notice the const at the end) (See function `CRect::GetCenter`)
#define RH_ScopedOverloadedInstall(fn, suffix, _addressGTA, addrCast, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, _addressGTA, static_cast<addrCast>(&RHCurrentNS::fn) __VA_OPT__(,) __VA_ARGS__)

#define RH_ScopedGlobalOverloadedInstall(fn, suffix, _addressGTA, addrCast, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, _addressGTA, static_cast<addrCast>(&fn) __VA_OPT__(,) __VA_ARGS__)

// Install global `fn` as name `fnName`
#define RH_ScopedNamedGlobalInstall(fn, fnName, _addressGTA, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, fnName, _addressGTA, &fn __VA_OPT__(,) __VA_ARGS__)

// Similar to RH_ScopedInstall but you can specify the name explicitly.
#define RH_ScopedNamedInstall(fn, fnName, _addressGTA, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, fnName, _addressGTA, &RHCurrentNS::fn __VA_OPT__(,) __VA_ARGS__)

#define RH_ScopedVMTOverloadedInstall(fn, suffix, fnGTAAddr, addrCast, ...) \
    rh->InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn "-" suffix, pOurVTbl, ReversibleHooks::Utility::FunctionToVoidPtr(static_cast<addrCast>(&RHCurrentNS::fn)), pGTAVTbl, (void*)fnGTAAddr __VA_OPT__(,) __VA_ARGS__)

// Install a hook on a virtual function. To use it, `RH_ScopedVirtualClass` must be used instead of `RH_ScopedClass`
#define RH_ScopedNamedVMTInstall(fn, fnName, fnGTAAddr, ...) \
    rh->InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, fnName, pOurVTbl, ReversibleHooks::Utility::FunctionToVoidPtr(&RHCurrentNS::fn), pGTAVTbl, (void*)fnGTAAddr __VA_OPT__(,) __VA_ARGS__)

// Install a hook on a virtual function. To use it, `RH_ScopedVirtualClass` must be used instead of `RH_ScopedClass`
#define RH_ScopedVMTInstall(fn, fnGTAAddr, ...) \
    RH_ScopedNamedVMTInstall(fn, #fn, fnGTAAddr __VA_OPT__(,) __VA_ARGS__)

//! Install a script hook
#define RH_ScopedInstallScriptCommand(cmd) \
    rh->InstallScriptCommand(RHCurrentCat.name + "/" + RHCurrentScopeName.name, cmd)

// Install constructor (possibly overloaded)
#define RH_ScopedConstructorInstall(_addressGTA, suffix, opts, ...) \
    rh->InstallConstructor<RHCurrentNS __VA_OPT__(,) __VA_ARGS__>(RHCurrentCat.name + "/" + RHCurrentScopeName.name, suffix, _addressGTA, opts)

// Install a virtual destructor hook
#define RH_ScopedVMTDestructorInstall(fnGTAAddr, ...) \
    rh->InstallVirtualDestructor<RHCurrentNS>(RHCurrentCat.name + "/" + RHCurrentScopeName.name, pOurVTbl, pGTAVTbl, fnGTAAddr __VA_OPT__(, ) __VA_ARGS__)

// Install classic destructor hook (For virtual ones use RH_ScopedDestructorInstall)
#define RH_ScopedDestructorInstall(fnGTAAddr, ...) \
    rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, "Desructor", fnGTAAddr, ReversibleHooks::Utility::GetScalarDestructorAddress<RHCurrentNS>() __VA_OPT__(,) __VA_ARGS__)

//#define RH_ScopedVMTAddressChange(fn, fnGTAAddr, ...) \
//    rh->InstallVirtual(RHCurrentCat.name + "/" + RHCurrentScopeName.name, #fn, pGTAVTbl, pOurVTbl, FunctionPointerToVoidP(fnGTAAddr), nVirtFns __VA_OPT__(,) __VA_ARGS__)

namespace ReversibleHooks {
struct ScopeName {
    std::string name{};
};

struct ScopeCategory {
    std::string name{};
};
};
