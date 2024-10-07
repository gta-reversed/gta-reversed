#include "StdInc.h"
#include <unordered_set>

#include "ReversibleHooks.h"
#include "ReversibleHook/Simple.h"
#include "ReversibleHook/Virtual.h"
#include "RootHookCategory.h"
#include <fstream>

namespace ReversibleHooks {

RootHookCategory s_RootCategory{};
HMODULE          s_hThisDLL{}; // Handle to this DLL, only valid after `OnInjectionBegin` is called.

#ifndef NDEBUG
// Not particularly memory efficient, but it should be fine
std::unordered_set<uint32> s_HookedAddresses{};  // Original GTA addresses to which we've installed hooks
#endif

RootHookCategory& GetRootCategory() {
    return s_RootCategory;
}

SetCatOrItemStateResult SetCategoryOrItemStateByPath(std::string_view path, bool enabled) {
    if (path.ends_with("/")) {
        path.remove_suffix(1);
    }

    const auto    seperated = SplitStringView(path, "/") | rng::to<std::vector>();
    HookCategory* cat       = &GetRootCategory();
    for (auto name : std::span(seperated).first(seperated.size() - 1)) {
        cat = cat->FindSubcategory(name);
        if (!cat) {
            return SetCatOrItemStateResult::NotFound;
        }
    }

    if (auto category = cat->FindSubcategory(seperated.back())) {
        category->SetAllItemsEnabled(enabled);
        return SetCatOrItemStateResult::Done;
    } else if (auto item = cat->FindItem(seperated.back())) {
        if (item->Hooked() == enabled) {
            return SetCatOrItemStateResult::Done;
        }
        return item->State(enabled) ? SetCatOrItemStateResult::Done : SetCatOrItemStateResult::Locked;
    }

    return SetCatOrItemStateResult::NotFound;
}

void CheckAll() {
    s_RootCategory.ForEachItem([](auto& item) {
        item->Check();
    });
}

void SwitchHook(std::string_view funcName) {
    s_RootCategory.ForEachItem([=](auto& item) {
        const auto name = item->Name();
        if (name == funcName) {
            item->Switch();
            return;
        }
    });
}

void OnInjectionBegin(HMODULE hThisDLL) {
    s_hThisDLL = hThisDLL;

#ifndef NDEBUG
    s_HookedAddresses.reserve(20000); // Should be enough - We free it after the injection has finished, so it should be fine
#endif
}

void OnInjectionEnd() {
#ifndef NDEBUG
    // Hopefully these deallocates all memory
    s_HookedAddresses.clear();
    s_HookedAddresses = {};
#endif

    s_RootCategory.OnInjectionEnd();

    // WriteHooksToFile("C:/hooks.csv");
}

void InstallVirtual(std::string_view category, std::string fnName, void** vtblGTA, void** vtblOur, void* fnGTAAddr, void* fnOurAddr, size_t nVirtFns, const HookInstallOptions& opt) {
    // Find fn index in vtbl
    const auto spanGTAVTbl = std::span{ vtblGTA, nVirtFns };
    const auto iter = rng::find(spanGTAVTbl, fnGTAAddr);
    if (iter == spanGTAVTbl.end()) {
        /* Tips in case you ever encounter this
        * - Make sure the vtable address and size is correct
        * - Make sure function address you're hooking is correct
        */
        NOTSA_UNREACHABLE("{}: Couldn't find function [{} @ {}] in vtable\n", category, fnName, fnGTAAddr);
    }
    const auto fnVTblIdx = (size_t)rng::distance(spanGTAVTbl.begin(), iter);

    // Make sure vtable entries correspond to GTA's layout
    //assert(vtblOur[fnVTblIdx] == fnOurAddr); // Doesn't work because the compiler generates thunks in debug mode

#ifdef HOOKS_DEBUG
    std::cout << std::format("{}::{} => {}\n", category, fnName, fnVTblIdx);
#endif

    auto item = std::make_shared<ReversibleHook::Virtual>(std::move(fnName), vtblGTA, vtblOur, fnVTblIdx);
    item->State(opt.enabled);
    item->LockState(opt.locked);
    AddItemToCategory(category, std::move(item));
}

void AddItemToCategory(std::string_view category, std::shared_ptr<ReversibleHook::Base> item) {
    s_RootCategory.AddItemToNamedCategory(category, std::move(item));
}

void WriteHooksToFile(const std::filesystem::path& file) {
    std::ofstream of{ file };
    of << "class,fn_name,address,reversed,locked,is_virtual\n";
    s_RootCategory.ForEachCategory([&](const HookCategory& cat) {
        using namespace ReversibleHook;
        for (const auto& item : cat.Items()) {
            const auto isVirtual = item->Type() == Base::HookType::Virtual;
            of
                << cat.Name() << "," // class
                << item->Name() << "," // fn_name
                << "0x" << std::hex << [&] { // address
                switch (item->Type()) {
                case Base::HookType::Virtual:
                    return std::static_pointer_cast<Virtual>(item)->GetHookGTAAddress();
                case Base::HookType::Simple:
                    return std::static_pointer_cast<Simple>(item)->GetHookGTAAddress();
                default:
                    NOTSA_UNREACHABLE();
                }
            }() << std::dec << ","
                << item->Hooked() << "," // reversed // TODO: Improve this (Add `m_isReversed` to `Base`) - For now this will do
                << item->Locked() << "," // locked
                << (item->Type() == Base::HookType::Virtual) << '\n'; // is_virtual
        }
    });
}

namespace detail {
void HookInstall(std::string_view category, std::string fnName, uint32 installAddress, void* addressToJumpTo, HookInstallOptions&& opt) {
#ifndef NDEBUG // Functions with the same name are asserted in `HookCategory::AddItem()`
    auto [iter, inserted] = s_HookedAddresses.insert(installAddress);
    if (!inserted) {
        NOTSA_LOG_ERR("{}/{} is hooked to an address (0x{:X}) that is already hooked! That's bad!", category.data(), fnName.c_str(), LOG_PTR(installAddress));
        assert(false);
        return;
    }
#endif

    auto item = std::make_shared<ReversibleHook::Simple>(std::move(fnName), installAddress, addressToJumpTo, opt.jmpCodeSize, opt.stackArguments);
    item->State(opt.enabled);
    item->LockState(opt.locked);
    AddItemToCategory(category, std::move(item));
}

void VirtualCopy(void* dst, void* src, size_t nbytes) {
    DWORD dwProtect[2] = { 0 };
    VirtualProtect(dst, nbytes, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
    memcpy(dst, src, nbytes);
    VirtualProtect(dst, nbytes, dwProtect[0], &dwProtect[1]);
}

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

// The VTable is exported as a symbol, in the format `??_7<class name>@@6B@` where `<class name>` is the name of the class.
// In order for this to work the class has to be exported (So the `NOTSA_EXPORT_VTABLE` macro has to be used)
void** GetVTableAddress(std::string_view className) {
    CHAR mangledName[1024];
    MangleClassNameMSVC(mangledName, className);
    if (const auto vtbl = reinterpret_cast<void**>(GetProcAddress(s_hThisDLL, mangledName))) {
#ifdef HOOKS_DEBUG
        std::cout << std::format("{}: Our VMT: {} \n", className, (void*)vtbl);
#endif
        return vtbl;
    }

    NOTSA_UNREACHABLE("Couldn't find VTable of `{}`", className);
}

}; // namespace detail
}; // namespace ReversibleHooks
