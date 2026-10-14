#include "StdInc.h"

#include "DiffTest.hpp"

#include <bit>
#include <cctype>
#include <chrono>
#include <cmath>
#include <fstream>

#include <reversiblehooks/RHManager.h>
#include <reversiblehooks/HookCategory.h>

namespace notsa {
namespace difftest {
HookItemPtr FindHook(std::string_view path) {
    const auto slash = path.rfind('/');
    if (slash == std::string_view::npos) {
        return nullptr;
    }
    const auto category = ReversibleHooks::RHManager::GetInstance().GetRootCategory()->FindCategoryByPath(path.substr(0, slash));
    return category
        ? category->FindItem(path.substr(slash + 1))
        : nullptr;
}

uint32 UlpDistance(float a, float b) {
    // Map the float bit patterns onto a monotonic integer line
    const auto toOrdered = [](float f) {
        const auto bits = std::bit_cast<int32>(f);
        return bits < 0 ? static_cast<int64>(INT32_MIN) - bits : static_cast<int64>(bits);
    };
    const auto d = toOrdered(a) - toOrdered(b);
    return static_cast<uint32>(std::min<int64>(d < 0 ? -d : d, UINT32_MAX));
}

bool ApproxEqual(float a, float b, Tolerance tol) {
    if (std::isnan(a) || std::isnan(b)) {
        return std::isnan(a) && std::isnan(b);
    }
    if (a == b) {
        return true;
    }
    if (std::isinf(a) || std::isinf(b)) {
        return false;
    }
    return std::fabs(a - b) <= tol.Abs || UlpDistance(a, b) <= tol.Ulps;
}

void Context::Fail(std::string msg) {
    ++m_NumFailures;
    if (m_Messages.size() < MAX_STORED_MESSAGES) {
        m_Messages.emplace_back(std::move(msg));
    }
}

float Context::InterestingFloat(float randMin, float randMax) {
    static constexpr float s_Interesting[]{
        0.f, -0.f, 1.f, -1.f, 0.5f, -0.5f,
        PI, -PI, TWO_PI, -TWO_PI, HALF_PI, -HALF_PI,
        PI + 0.0001f, -PI - 0.0001f, TWO_PI * 3.f, -TWO_PI * 3.f,
        1e-6f, -1e-6f, 1e6f, -1e6f, 32767.f, -32768.f,
    };
    if (RandomInt(0, 3) == 0) { // 25% interesting, 75% random
        return s_Interesting[RandomInt(0, (int32)std::size(s_Interesting) - 1)];
    }
    return RandomFloat(randMin, randMax);
}

namespace detail {
std::string BytesToHex(const void* data, size_t size) {
    std::string out;
    out.reserve(size * 3);
    for (const auto b : std::span{ static_cast<const uint8*>(data), size }) {
        out += std::format("{:02X} ", b);
    }
    if (!out.empty()) {
        out.pop_back();
    }
    return out;
}
}; // namespace detail

bool CompareBytes(Context& ctx, std::string_view label, const void* expected, const void* actual, size_t size, std::span<const std::pair<size_t, size_t>> ignore) {
    const auto IsIgnored = [&](size_t offset) {
        return rng::any_of(ignore, [offset](const auto& r) { return offset >= r.first && offset < r.first + r.second; });
    };

    const auto* const e = static_cast<const uint8*>(expected);
    const auto* const a = static_cast<const uint8*>(actual);

    std::string diffs; // All differing ranges in one failure message (one check = at most one failure)
    for (size_t i = 0; i < size;) {
        if (e[i] == a[i] || IsIgnored(i)) {
            i++;
            continue;
        }
        size_t end = i;
        while (end < size && e[end] != a[end] && !IsIgnored(end)) {
            end++;
        }
        diffs += std::format("{}+0x{:X}..+0x{:X}: original = [{}], ours = [{}]", diffs.empty() ? "" : "; ", i, end - 1,
            detail::BytesToHex(e + i, end - i), detail::BytesToHex(a + i, end - i));
        i = end;
    }
    ctx.AddChecks(1);
    if (!diffs.empty()) {
        ctx.Fail(std::format("{}: bytes differ at {}", label, diffs));
    }
    return diffs.empty();
}

bool CompareFloats(Context& ctx, std::string_view label, const float* expected, const float* actual, size_t count, Tolerance tol) {
    std::string diffs;
    for (size_t i = 0; i < count; i++) {
        if (!ApproxEqual(expected[i], actual[i], tol)) {
            diffs += std::format("{}[{}] (+0x{:X}): original = {}, ours = {} ({} ULP)", diffs.empty() ? "" : "; ", i, i * 4, expected[i], actual[i], UlpDistance(expected[i], actual[i]));
        }
    }
    ctx.AddChecks(1);
    if (!diffs.empty()) {
        ctx.Fail(std::format("{}: {}", label, diffs));
    }
    return diffs.empty();
}

bool CompareSideEffects(Context& ctx, std::string_view label, std::function<void(bool isOriginal)> run, std::span<const std::pair<void*, size_t>> regions) {
    std::vector<MemorySnapshot> initial, afterOriginal;
    for (const auto& [addr, size] : regions) {
        initial.emplace_back(addr, size);
    }

    run(true);
    for (const auto& [addr, size] : regions) {
        afterOriginal.emplace_back(addr, size);
    }

    for (const auto& s : initial) {
        s.Restore();
    }
    run(false);

    bool equal = true;
    for (auto&& [i, s] : rngv::enumerate(afterOriginal)) {
        equal &= CompareBytes(ctx, std::format("{} [region #{} @ {}]", label, i, s.Address()), s.Bytes().data(), s.Address(), s.Bytes().size());
    }
    return equal;
}

std::vector<TestInfo>& GetRegisteredTests() {
    static std::vector<TestInfo> s_Tests;
    return s_Tests;
}

const char* StatusToString(TestStatus s) {
    switch (s) {
    case TestStatus::NotRun:  return "Not run";
    case TestStatus::Passed:  return "Passed";
    case TestStatus::Failed:  return "Failed";
    case TestStatus::Skipped: return "Skipped";
    default:                  NOTSA_UNREACHABLE_CASE(s);
    }
}

TestResult RunTest(const TestInfo& test, uint32 seed) {
    using HookState = ReversibleHooks::HookCategoryItem::HookState;

    TestResult result{ .Seed = seed };

    // Un-hook the functions under test, so both GTA's original code and ours are unpatched
    struct SavedHook {
        HookItemPtr Item;
        HookState   State;
    };
    std::vector<SavedHook> saved;
    for (const auto path : test.Hooks) {
        auto item = FindHook(path);
        if (!item) {
            // Not registered (eg. its InjectHooks() is commented out): nothing is patched, so GTA's code and ours can be
            // compared as they are. A typo in the path ends up here too - hence the note.
            result.Notes.emplace_back(std::format("Hook `{}` is not registered - comparing the unpatched functions directly", path));
            continue;
        }
        if (item->GetState() == HookState::RedirectToGTA && item->GetIsStateLocked()) {
            result.Status = TestStatus::Skipped;
            result.Messages.emplace_back(std::format("Hook `{}` is not reversed (`.Reversed = false`) - nothing to compare", path));
            continue;
        }
        saved.push_back({ item, item->GetState() });
    }

    if (result.Status != TestStatus::Skipped) {
        for (auto& s : saved) {
            s.Item->GetHook()->State(HookState::Unhooked);
        }

        Context ctx{ seed };
        const auto start = std::chrono::steady_clock::now();
        test.Fn(ctx);
        result.DurationMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();

        result.NumChecks   = ctx.GetNumChecks();
        result.NumFailures = ctx.GetNumFailures();
        result.Messages    = ctx.GetMessages();
        result.Notes.insert(result.Notes.end(), ctx.GetNotes().begin(), ctx.GetNotes().end());
        result.Status      = ctx.GetNumFailures() ? TestStatus::Failed : TestStatus::Passed;
    }

    // Restore hook states
    for (const auto& s : saved) {
        s.Item->GetHook()->State(s.State);
    }

    // Log
    if (result.Status == TestStatus::Passed) {
        NOTSA_LOG_INFO("[DiffTest] PASSED {} ({} checks, {:.2f} ms)", test.Name, result.NumChecks, result.DurationMs);
    } else {
        NOTSA_LOG_WARN("[DiffTest] {} {} ({} failures / {} checks, seed: 0x{:X})", StatusToString(result.Status), test.Name, result.NumFailures, result.NumChecks, seed);
        for (const auto& msg : result.Messages) {
            NOTSA_LOG_WARN("[DiffTest]     {}", msg);
        }
    }

    return result;
}

size_t RunAll(std::string_view filter, bool isGameLoaded, const std::filesystem::path& reportPath, uint32 seed) {
    const auto ContainsCI = [](std::string_view hay, std::string_view needle) {
        return needle.empty() || !rng::search(hay, needle, [](char a, char b) { return std::tolower((uint8)a) == std::tolower((uint8)b); }).empty();
    };

    std::string report = std::format("# gta-reversed differential tests (seed 0x{:X}, filter '{}')\n", seed, filter);
    std::ofstream out;
    if (!reportPath.empty()) {
        out.open(reportPath);
    }
    size_t written = 0;
    const auto FlushReport = [&] { // Write as we go, so a crash/assert in a test still leaves the results so far
        if (out) {
            out << std::string_view{ report }.substr(written);
            out.flush();
            written = report.size();
        }
    };
    size_t numPassed{}, numFailed{}, numSkipped{};
    for (const auto& test : GetRegisteredTests()) {
        if (!ContainsCI(test.Name, filter)) {
            continue;
        }
        report += std::format("RUNNING {}\n", test.Name);
        FlushReport();
        if (test.RequiresLoadedGame && !isGameLoaded) {
            numSkipped++;
            report += std::format("SKIPPED {} (requires a loaded game - run it from the debug menu)\n", test.Name);
            continue;
        }
        const auto r = RunTest(test, seed);
        switch (r.Status) {
        case TestStatus::Passed:  numPassed++;  break;
        case TestStatus::Failed:  numFailed++;  break;
        default:                  numSkipped++; break;
        }
        report += std::format("{} {} ({}/{} checks, {:.2f} ms)\n", StatusToString(r.Status), test.Name, r.NumChecks - r.NumFailures, r.NumChecks, r.DurationMs);
        for (const auto& note : r.Notes) {
            report += std::format("    note: {}\n", note);
        }
        for (const auto& msg : r.Messages) {
            report += std::format("    {}\n", msg);
        }
        if (r.NumFailures > r.Messages.size()) {
            report += std::format("    ... and {} more failures\n", r.NumFailures - r.Messages.size());
        }
    }
    report += std::format("\nTotal: {} passed, {} failed, {} skipped\n", numPassed, numFailed, numSkipped);
    NOTSA_LOG_INFO("[DiffTest] Total: {} passed, {} failed, {} skipped", numPassed, numFailed, numSkipped);

    if (!reportPath.empty() && !out) {
        NOTSA_LOG_ERR("[DiffTest] Couldn't write report to {}", reportPath.string());
    }
    FlushReport();
    return numFailed;
}
}; // namespace difftest
}; // namespace notsa
