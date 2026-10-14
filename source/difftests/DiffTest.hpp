/*!
 * NOTSA - Differential tests
 *
 * Validates reversed functions against the ORIGINAL machine code of the game, at runtime, inside the game process.
 *
 * How it works:
 *  - Every reversed function is installed as a two-way reversible hook. While a hook is `Unhooked`,
 *    neither GTA's code nor ours is patched, so the original can be called through its GTA address
 *    and ours can be called directly.
 *  - A test temporarily un-hooks the listed hooks, calls both implementations with the same inputs
 *    (hand-picked edge cases and/or random fuzzing with a fixed seed) and compares the results
 *    (return values, out-params, or the bytes of objects they mutated).
 *  - Callees of the tested function keep their current hook state, so both implementations call
 *    exactly the same callees - only the function under test differs.
 *
 * Tests are registered with `NOTSA_DIFFTEST` and run from the debug menu (F7 -> Tools -> Diff tests).
 * See `docs/ReversingGuide.md` for the full workflow.
 */
#pragma once

#include <cstring>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <random>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include <reversiblehooks/HookCategoryItem.h>

namespace notsa {
namespace difftest {
using HookItemPtr = std::shared_ptr<ReversibleHooks::HookCategoryItem>;

/*!
 * @brief Find a hook by its full path, as shown in the hooks GUI: `<Category>/<Scope>/<Function>`.
 * @example `"Global/CGeneral/LimitRadianAngle"`, `"Entity/Ped/CPed/SetMoveAnim"`
 * @return The hook, or `nullptr` if it doesn't exist
 */
HookItemPtr FindHook(std::string_view path);

/*!
 * @brief Get a typed pointer to the ORIGINAL GTA function at `address`.
 * @note Only call it while the hook of that function is unhooked (the test runner does this for the hooks the test lists),
 *       otherwise you'd be calling our code through GTA's address.
 * @example `GTAFn<float(__cdecl*)(float)>(0x53CB50)`, `GTAFn<bool(__thiscall*)(CPed*, int32)>(0x5E4A20)`
 */
template<typename FnPtr>
    requires std::is_pointer_v<FnPtr> && std::is_function_v<std::remove_pointer_t<FnPtr>>
FnPtr GTAFn(uintptr address) {
    return reinterpret_cast<FnPtr>(address);
}

//! Tolerance used when comparing floats. Default is bit-exact (NaN == NaN, but +0 != -0 is NOT checked - they compare equal).
struct Tolerance {
    float  Abs{ 0.f }; //!< Absolute tolerance (|a - b| <= Abs)
    uint32 Ulps{ 0 };  //!< Max distance in units-in-the-last-place (useful for x87 (original) vs SSE (ours) precision differences)
};

//! Returns the distance between 2 floats in ULPs
uint32 UlpDistance(float a, float b);

bool ApproxEqual(float a, float b, Tolerance tol);

/*!
 * @brief State of a running test: assertion bookkeeping and a deterministic RNG for fuzzing.
 */
class Context {
public:
    static constexpr size_t MAX_STORED_MESSAGES = 32;

    explicit Context(uint32 seed) :
        m_Rng{ seed },
        m_Seed{ seed }
    {
    }

    //! Record a failure (only the first `MAX_STORED_MESSAGES` are stored, all are counted)
    void Fail(std::string msg);

    //! Check a condition, record a failure if it's false
    bool Check(bool cond, std::string_view what) {
        ++m_NumChecks;
        if (!cond) {
            Fail(std::string{ what });
        }
        return cond;
    }

    //! Add a note that's shown even if the test passes (eg.: number of iterations)
    void Note(std::string msg) { m_Notes.emplace_back(std::move(msg)); }

    // Deterministic random input generation (seeded per test, so failures are reproducible)
    float   RandomFloat(float min, float max) { return std::uniform_real_distribution<float>{ min, max }(m_Rng); }
    int32   RandomInt(int32 min, int32 max) { return std::uniform_int_distribution<int32>{ min, max }(m_Rng); }
    bool    RandomBool() { return RandomInt(0, 1) != 0; }
    CVector RandomVector(float min, float max) { return { RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) }; }

    //! A float from a set of "interesting" values (0, -0, ±1, ±PI, ±2PI, huge, tiny, ...) or a random one
    float InterestingFloat(float randMin = -1000.f, float randMax = 1000.f);

    auto        GetSeed() const { return m_Seed; }
    auto        GetNumChecks() const { return m_NumChecks; }
    auto        GetNumFailures() const { return m_NumFailures; }
    const auto& GetMessages() const { return m_Messages; }
    const auto& GetNotes() const { return m_Notes; }
    void        AddChecks(size_t n) { m_NumChecks += n; }

private:
    std::mt19937             m_Rng;
    uint32                   m_Seed{};
    size_t                   m_NumChecks{};
    size_t                   m_NumFailures{};
    std::vector<std::string> m_Messages{};
    std::vector<std::string> m_Notes{};
};

//
// Value formatting/comparison helpers
//
namespace detail {
std::string BytesToHex(const void* data, size_t size);

template<typename T>
std::string ToString(const T& v) {
    if constexpr (std::is_same_v<T, CVector>) {
        return std::format("{{{}, {}, {}}}", v.x, v.y, v.z);
    } else if constexpr (std::is_same_v<T, CVector2D>) {
        return std::format("{{{}, {}}}", v.x, v.y);
    } else if constexpr (std::is_enum_v<T>) {
        return std::format("{}", static_cast<std::underlying_type_t<T>>(v));
    } else if constexpr (std::is_pointer_v<T>) {
        return std::format("{}", static_cast<const void*>(v));
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::format("{}", v);
    } else {
        return BytesToHex(&v, sizeof(T));
    }
}

template<typename... Ts>
std::string TupleToString(const std::tuple<Ts...>& t) {
    std::string out{ "(" };
    std::apply([&](const auto&... e) {
        size_t i{};
        ((out += (i++ ? ", " : "") + ToString(e)), ...);
    }, t);
    return out + ")";
}

template<typename T>
bool Equal(const T& a, const T& b, Tolerance tol) {
    if constexpr (std::is_floating_point_v<T>) {
        return ApproxEqual(static_cast<float>(a), static_cast<float>(b), tol);
    } else if constexpr (std::is_same_v<T, CVector>) {
        return ApproxEqual(a.x, b.x, tol) && ApproxEqual(a.y, b.y, tol) && ApproxEqual(a.z, b.z, tol);
    } else if constexpr (std::is_same_v<T, CVector2D>) {
        return ApproxEqual(a.x, b.x, tol) && ApproxEqual(a.y, b.y, tol);
    } else if constexpr (std::equality_comparable<T>) {
        return a == b;
    } else {
        static_assert(std::is_trivially_copyable_v<T>, "Type is neither equality comparable nor trivially copyable - compare it manually");
        return std::memcmp(&a, &b, sizeof(T)) == 0;
    }
}
}; // namespace detail

/*!
 * @brief Call the original and our implementation with the same arguments and compare the return values.
 * @param ctx      Test context
 * @param label    Shown in failure messages (eg.: function name)
 * @param original Callable invoking the original (usually `GTAFn<...>(0xADDR)`)
 * @param ours     Callable invoking our implementation (eg.: `&CGeneral::LimitRadianAngle` or a lambda)
 * @param args     Arguments, as a tuple (they're passed by value/copy to both)
 * @param tol      Float tolerance
 * @return Whether the results were equal (always true for `void` functions)
 */
template<typename Orig, typename Ours, typename... Args>
bool CompareCall(Context& ctx, std::string_view label, Orig&& original, Ours&& ours, const std::tuple<Args...>& args, Tolerance tol = {}) {
    using Ret = std::invoke_result_t<Orig, Args...>;
    if constexpr (std::is_void_v<Ret>) {
        std::apply(original, args);
        std::apply(ours, args);
        return true;
    } else {
        const Ret expected = std::apply(original, args);
        const Ret actual   = static_cast<Ret>(std::apply(ours, args));
        if (detail::Equal(expected, actual, tol)) {
            ctx.AddChecks(1);
            return true;
        }
        ctx.AddChecks(1);
        ctx.Fail(std::format("{}{}: original = {}, ours = {}", label, detail::TupleToString(args), detail::ToString(expected), detail::ToString(actual)));
        return false;
    }
}

/*!
 * @brief Random (fuzz) testing: `iterations` times generate arguments with `gen` and `CompareCall` them.
 * @param gen `(Context&) -> std::tuple<Args...>`
 * @return Number of mismatches
 */
template<typename Gen, typename Orig, typename Ours>
size_t Fuzz(Context& ctx, std::string_view label, size_t iterations, Gen&& gen, Orig&& original, Ours&& ours, Tolerance tol = {}) {
    size_t mismatches{};
    for (size_t i = 0; i < iterations; i++) {
        if (!CompareCall(ctx, label, original, ours, std::invoke(gen, ctx), tol)) {
            mismatches++;
        }
    }
    ctx.Note(std::format("{}: {} fuzz iterations, {} mismatches", label, iterations, mismatches));
    return mismatches;
}

/*!
 * @brief Compare 2 objects byte-by-byte and report the differing byte ranges (offsets help finding the member, see `VALIDATE_OFFSET`s).
 * @note Use it for functions that mutate state: copy the initial object twice, run the original on one copy and ours on the other.
 *       Pointers to the copies themselves (self references, linked lists) obviously differ - exclude them via `ignore`.
 * @param ignore Byte ranges `{offset, size}` to ignore
 */
bool CompareBytes(Context& ctx, std::string_view label, const void* expected, const void* actual, size_t size, std::span<const std::pair<size_t, size_t>> ignore = {});

/*!
 * @brief Compare `count` floats with a tolerance - for float-heavy objects (matrices, vectors) where the original's x87
 *        extended precision makes `sin`/`cos`/`sqrt` chains differ in the last bit. Reports index, offset and ULP distance.
 */
bool CompareFloats(Context& ctx, std::string_view label, const float* expected, const float* actual, size_t count, Tolerance tol);

template<typename T>
bool CompareObjects(Context& ctx, std::string_view label, const T& expected, const T& actual, std::span<const std::pair<size_t, size_t>> ignore = {}) {
    return CompareBytes(ctx, label, &expected, &actual, sizeof(T), ignore);
}

/*!
 * @brief Snapshot of a memory region (eg.: a global array) - take one before calling the original, restore it, then call ours and compare.
 */
class MemorySnapshot {
public:
    MemorySnapshot(const void* address, size_t size) :
        m_Address{ address },
        m_Bytes{ static_cast<const uint8*>(address), static_cast<const uint8*>(address) + size }
    {
    }

    template<typename T>
    explicit MemorySnapshot(const T& obj) : MemorySnapshot{ &obj, sizeof(T) } {}

    //! Write the snapshotted bytes back
    void Restore() const { std::memcpy(const_cast<void*>(m_Address), m_Bytes.data(), m_Bytes.size()); }

    //! Compare current memory with the snapshot
    bool CompareWithCurrent(Context& ctx, std::string_view label) const { return CompareBytes(ctx, label, m_Bytes.data(), m_Address, m_Bytes.size()); }

    const auto& Bytes() const { return m_Bytes; }
    auto        Address() const { return m_Address; }

private:
    const void*        m_Address;
    std::vector<uint8> m_Bytes;
};

/*!
 * @brief Run `fn` on the original and on ours, restoring the given memory regions in between, then compare the regions.
 * @param run `(bool isOriginal) -> void` - call the original when `isOriginal` is true, ours otherwise
 * @param regions Memory regions (globals/objects) the function touches
 */
bool CompareSideEffects(Context& ctx, std::string_view label, std::function<void(bool isOriginal)> run, std::span<const std::pair<void*, size_t>> regions);

//
// Registration & running
//
using TestFn = void (*)(Context&);

struct TestInfo {
    std::string_view              Name;     //!< Display name, usually `Class::Function`
    std::vector<std::string_view> Hooks;    //!< Hook paths that are un-hooked while the test runs (the functions under test)
    TestFn                        Fn;
    std::source_location          Location;
    bool                          RequiresLoadedGame{}; //!< Needs the world/pools/streaming (only run from the debug menu, in-game)
};

enum class TestStatus {
    NotRun,
    Passed,
    Failed,
    Skipped, //!< Eg.: hook not found / not reversed
};

struct TestResult {
    TestStatus               Status{ TestStatus::NotRun };
    size_t                   NumChecks{};
    size_t                   NumFailures{};
    uint32                   Seed{};
    double                   DurationMs{};
    std::vector<std::string> Messages{};
    std::vector<std::string> Notes{};
};

std::vector<TestInfo>& GetRegisteredTests();

struct Registrar {
    Registrar(std::string_view name, std::initializer_list<std::string_view> hooks, TestFn fn, bool requiresLoadedGame = false, std::source_location loc = std::source_location::current()) {
        GetRegisteredTests().emplace_back(TestInfo{ name, std::vector<std::string_view>{ hooks }, fn, loc, requiresLoadedGame });
    }
};

constexpr uint32 DEFAULT_SEED = 0x5A5A1234;

//! Run a single test (un-hooks its hooks, runs it, restores the hooks). Results are also logged.
TestResult RunTest(const TestInfo& test, uint32 seed = DEFAULT_SEED);

/*!
 * @brief Run every registered test whose name contains `filter` (case-insensitive, empty = all) and write a report.
 * @param isGameLoaded If false, tests that need a loaded game are skipped
 * @param reportPath   Where to write the plain-text report (empty = don't write)
 * @return Number of failed tests
 */
size_t RunAll(std::string_view filter, bool isGameLoaded, const std::filesystem::path& reportPath = {}, uint32 seed = DEFAULT_SEED);

const char* StatusToString(TestStatus s);
}; // namespace difftest
}; // namespace notsa

#define NOTSA_DIFFTEST_CONCAT_IMPL(a, b) a##b
#define NOTSA_DIFFTEST_CONCAT(a, b)      NOTSA_DIFFTEST_CONCAT_IMPL(a, b)

#define NOTSA_DIFFTEST_IMPL(_name, _id, _requiresGame, ...)                                                                                       \
    static void NOTSA_DIFFTEST_CONCAT(NotsaDiffTestFn_, _id)(::notsa::difftest::Context & ctx);                                                \
    static const ::notsa::difftest::Registrar NOTSA_DIFFTEST_CONCAT(s_NotsaDiffTestReg_, _id){ _name, { __VA_ARGS__ }, &NOTSA_DIFFTEST_CONCAT(NotsaDiffTestFn_, _id), _requiresGame }; \
    static void NOTSA_DIFFTEST_CONCAT(NotsaDiffTestFn_, _id)([[maybe_unused]] ::notsa::difftest::Context & ctx)

/*!
 * @brief Define a differential test
 * @param _name Display name (string literal), usually `"Class::Function"`
 * @param ...   Full hook paths of the functions under test (string literals), eg.: `"Global/CGeneral/LimitRadianAngle"`
 *
 * @code
 * NOTSA_DIFFTEST("CGeneral::LimitRadianAngle", "Global/CGeneral/LimitRadianAngle") {
 *     using namespace notsa::difftest;
 *     Fuzz(ctx, "LimitRadianAngle", 10'000,
 *         [](Context& c) { return std::tuple{ c.InterestingFloat(-50.f, 50.f) }; },
 *         GTAFn<float(__cdecl*)(float)>(0x53CB50),
 *         &CGeneral::LimitRadianAngle
 *     );
 * }
 * @endcode
 */
#define NOTSA_DIFFTEST(_name, ...) NOTSA_DIFFTEST_IMPL(_name, __COUNTER__, false, __VA_ARGS__)

//! Same as `NOTSA_DIFFTEST`, but the test needs a loaded game (world, pools, player...) - it's skipped by `--run-difftests` at startup
#define NOTSA_DIFFTEST_INGAME(_name, ...) NOTSA_DIFFTEST_IMPL(_name, __COUNTER__, true, __VA_ARGS__)
