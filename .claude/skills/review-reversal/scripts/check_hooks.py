#!/usr/bin/env python3
"""
Heuristic consistency checker for reversed code in gta-reversed.

It enforces the rules from `docs/HookRegistration.md` and catches the most common
reversing mistakes. It is regex based, so treat WARN as "look at this", not as proof.

Checks per .cpp file:
  [E] class defines `X::InjectHooks()` but `source/InjectHooksMain.cpp` never calls `X::InjectHooks();`
  [E] the same GTA address is installed twice in one file
  [E] a function body still calls `plugin::Call*<0xADDR...>` of ITS OWN address, but its
      RH install does not have `.Reversed = false`  (=> infinite recursion / wrong progress)
  [W] RH install has `.Reversed = false` but the function body has no `plugin::Call*`
      (maybe it was reversed and the flag was forgotten)
  [W] a `// 0xADDR` comment directly above a function definition has no matching RH install
  [W] `RH_ScopedVMTInstall` used but the scope is `RH_ScopedClass` (needs `RH_ScopedVirtualClass`)
  [W] header has `static void InjectHooks();` but no `friend void InjectHooksMain();` while it is private
      (or `friend` present while InjectHooks is public - harmless, just noise)

Usage (repo root):
  python .claude/skills/review-reversal/scripts/check_hooks.py                 # files changed vs master (+ uncommitted)
  python .claude/skills/review-reversal/scripts/check_hooks.py --base origin/master
  python .claude/skills/review-reversal/scripts/check_hooks.py source/game_sa/Fire.cpp source/game_sa/Fire.h
  python .claude/skills/review-reversal/scripts/check_hooks.py --all           # whole code base (noisy)

Exit code: 1 if any [E] was found, else 0.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ADDR = r"0x[0-9A-Fa-f]{5,8}"
INSTALL_RE = re.compile(r"^\s*(?P<macro>RH_Scoped\w*Install)\(\s*(?P<fn>[\w:~]+)\s*,(?P<rest>.*)$")
SCOPE_RE = re.compile(r"RH_Scoped(?P<kind>VirtualClass|Class|NamedClass|Namespace|NamespaceName)\(\s*(?P<name>[\w:]+)")
INJECT_DEF_RE = re.compile(r"^\s*void\s+(?P<cls>[\w:]+)::InjectHooks\s*\(\s*\)")
FUNC_DEF_RE = re.compile(
    r"^(?!\s*(?:if|for|while|switch|return|else)\b)[\w:<>,\*&\s~]*?\b(?P<cls>[A-Za-z_]\w*)::(?P<fn>~?[A-Za-z_]\w*)\s*\([^;]*$"
)
PLUGIN_CALL_RE = re.compile(r"plugin::Call\w*<[^>]*?(?P<addr>" + ADDR + r")")
ADDR_COMMENT_RE = re.compile(r"^\s*//\s*(?P<addr>" + ADDR + r")\s*$")


class Report:
    def __init__(self) -> None:
        self.errors = 0
        self.warnings = 0

    def err(self, where: str, msg: str) -> None:
        self.errors += 1
        print(f"[E] {where}: {msg}")

    def warn(self, where: str, msg: str) -> None:
        self.warnings += 1
        print(f"[W] {where}: {msg}")


def repo_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if (p / "source" / "InjectHooksMain.cpp").exists():
            return p
    sys.exit("error: run from inside the gta-reversed repository")


def changed_files(root: Path, base: str) -> list[Path]:
    def git(*a: str) -> list[str]:
        r = subprocess.run(["git", *a], cwd=root, capture_output=True, text=True)
        return [l for l in r.stdout.splitlines() if l.strip()]

    names = set(git("diff", "--name-only", f"{base}...HEAD"))
    names |= set(git("diff", "--name-only"))
    names |= set(git("diff", "--name-only", "--cached"))
    names |= set(git("ls-files", "--others", "--exclude-standard", "source"))
    return [root / n for n in sorted(names) if n.startswith("source/") and n.endswith((".cpp", ".h", ".hpp"))]


def function_bodies(lines: list[str]) -> dict[str, list[tuple[int, str, str]]]:
    """Very rough: map `fn` name -> list of (start_line, cls, body_text). Brace counting from the definition."""
    out: dict[str, list[tuple[int, str, str]]] = {}
    i = 0
    while i < len(lines):
        m = FUNC_DEF_RE.match(lines[i])
        if m and "InjectHooks" not in lines[i]:
            # find opening brace within the next few lines
            j = i
            while j < len(lines) and "{" not in lines[j] and ";" not in lines[j] and j - i < 6:
                j += 1
            if j < len(lines) and "{" in lines[j]:
                depth, k, body = 0, j, []
                while k < len(lines):
                    depth += lines[k].count("{") - lines[k].count("}")
                    body.append(lines[k])
                    if depth <= 0 and k > j - 1 and "}" in lines[k]:
                        break
                    k += 1
                code = "\n".join(re.sub(r"//.*$", "", b) for b in body)  # ignore commented-out plugin::Call
                out.setdefault(m.group("fn"), []).append((i + 1, m.group("cls"), code))
                i = k + 1
                continue
        i += 1
    return out


INJECT_MAIN_RAW = ""
_PE: object = False


def get_pe(root: Path):
    """The gta_sa.exe image (via validate-against-binary/asm_validate.py) if configured, else None."""
    global _PE
    if _PE is False:
        _PE = None
        try:
            sys.path.insert(0, str(root / ".claude" / "skills" / "validate-against-binary" / "scripts"))
            import os
            import asm_validate as av
            cand = os.environ.get("GTASA_EXE")
            cfg = root / ".claude" / "gtasa_exe_path.txt"
            if not cand and cfg.exists():
                cand = cfg.read_text(encoding="utf-8").strip()
            if cand and Path(cand).exists():
                _PE = av.PEImage(Path(cand))
        except Exception:  # capstone missing etc. - the check is optional
            _PE = None
    return _PE


def check_cpp(path: Path, root: Path, inject_main: str, rep: Report) -> None:
    rel = path.relative_to(root).as_posix()
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()

    # 1. InjectHooks registered in InjectHooksMain
    for n, line in enumerate(lines, 1):
        m = INJECT_DEF_RE.match(line)
        if m:
            cls = m.group("cls")
            short = cls.split("::")[-1]
            if not re.search(rf"\b{re.escape(cls)}::InjectHooks\s*\(\s*\)\s*;", inject_main) and \
               not re.search(rf"\b{re.escape(short)}::InjectHooks\s*\(\s*\)\s*;", inject_main):
                commented = re.search(rf"^\s*//\s*{re.escape(short)}::InjectHooks\s*\(", INJECT_MAIN_RAW, re.M)
                if commented:
                    rep.warn(f"{rel}:{n}", f"`{cls}::InjectHooks()` is commented out in InjectHooksMain - hooks disabled on purpose? (its reversed code never runs)")
                else:
                    rep.err(f"{rel}:{n}", f"`{cls}::InjectHooks()` is never called (not in InjectHooksMain nor another InjectHooks) - its reversed code never runs")

    # 2. collect installs
    installs: list[dict] = []
    scope_stack: list[tuple[int, str]] = []  # (brace depth, scope kind) - handles nested `{ RH_ScopedClass(...) }` blocks
    depth = 0
    for n, line in enumerate(lines, 1):
        if line.lstrip().startswith("//"):
            continue
        s = SCOPE_RE.search(line)
        if s:
            scope_stack.append((depth, s.group("kind")))
        depth += line.count("{") - line.count("}")
        while scope_stack and scope_stack[-1][0] > depth:
            scope_stack.pop()
        scope_kind = scope_stack[-1][1] if scope_stack else None
        m = INSTALL_RE.match(line)
        if not m:
            continue
        addr = re.search(ADDR, line)
        installs.append({
            "line": n,
            "fn": m.group("fn"),
            "macro": m.group("macro"),
            "addr": addr.group(0).lower() if addr else None,
            "not_reversed": bool(re.search(r"\.Reversed\s*=\s*false|\.State\s*=\s*HS::RedirectToGTA", line)),
        })
        if "VMT" in m.group("macro").upper() and scope_kind in ("Class", "NamedClass"):
            rep.warn(f"{rel}:{n}", f"`{m.group('macro')}` requires `RH_ScopedVirtualClass(cls, vtblAddr, nVirtFns)` scope")

    # 2b. VMT installs must point into the GTA vtable declared by RH_ScopedVirtualClass (needs the exe)
    #     A wrong vtable / address makes VMTInfo throw inside DllMain -> the game hangs at startup.
    pe = get_pe(root)
    if pe is not None:
        vt = None
        for n, line in enumerate(lines, 1):
            if line.lstrip().startswith("//"):
                continue
            v = re.search(r"RH_ScopedVirtualClass\(\s*([\w:]+)\s*,\s*(0x[0-9A-Fa-f]+)\s*,\s*(\d+)\s*\)", line)
            if v:
                base, count = int(v.group(2), 16), int(v.group(3))
                vt = (v.group(1), base, {pe.read_u32(base + i * 4) for i in range(count)} - {None})
                continue
            m = INSTALL_RE.match(line)
            if m and vt and re.match(r"RH_Scoped(VMT|NamedVMT|VMTOverloaded)Install", m.group("macro")):
                a = re.search(ADDR, line)
                if a and int(a.group(0), 16) not in vt[2]:
                    rep.err(f"{rel}:{n}", f"{a.group(0)} is not in {vt[0]}'s GTA vtable 0x{vt[1]:X} (wrong vtable/count/address) - VMTInfo will throw at startup")

    # 3. duplicate addresses
    seen: dict[str, int] = {}
    for ins in installs:
        a = ins["addr"]
        if a and a in seen:
            rep.err(f"{rel}:{ins['line']}", f"address {a} is already installed at line {seen[a]}")
        elif a:
            seen[a] = ins["line"]

    installed_addrs = {i["addr"] for i in installs if i["addr"]}
    bodies = function_bodies(lines)

    # 4. plugin::Call of own address without Reversed=false / Reversed=false without plugin::Call
    for ins in installs:
        if not ins["addr"]:
            continue
        fn = ins["fn"].split("::")[-1]
        candidates = bodies.get(fn, [])
        if not candidates:
            continue
        calls_self = any(ins["addr"] in [c.lower() for c in PLUGIN_CALL_RE.findall(b)] for _, _, b in candidates)
        calls_any = any(PLUGIN_CALL_RE.search(b) for _, _, b in candidates)
        if calls_self and not ins["not_reversed"]:
            rep.err(f"{rel}:{ins['line']}",
                    f"`{ins['fn']}` ({ins['addr']}) still forwards to GTA via plugin::Call but is installed as reversed "
                    f"-> add `{{ .Reversed = false }}`")
        is_stub = any("NOTSA_UNREACHABLE" in b for _, _, b in candidates)
        if ins["not_reversed"] and not calls_any and not is_stub and len(candidates) == 1:
            rep.warn(f"{rel}:{ins['line']}",
                     f"`{ins['fn']}` is marked `.Reversed = false` but its body has no plugin::Call "
                     f"- if it is reversed now, drop the flag")

    # 5. address comments above definitions without install
    for n, line in enumerate(lines[:-1], 1):
        m = ADDR_COMMENT_RE.match(line)
        if not m:
            continue
        nxt = lines[n] if n < len(lines) else ""
        if FUNC_DEF_RE.match(nxt) and m.group("addr").lower() not in installed_addrs and "InjectHooks" not in nxt:
            fn = FUNC_DEF_RE.match(nxt).group("fn")
            rep.warn(f"{rel}:{n}", f"`{fn}` is annotated with {m.group('addr')} but no RH_*Install uses that address "
                                   f"(fine only if it is inlined/unused/NOTSA in the original)")


def check_header(path: Path, root: Path, rep: Report) -> None:
    rel = path.relative_to(root).as_posix()
    text = path.read_text(encoding="utf-8", errors="replace")
    if "static void InjectHooks();" not in text:
        return
    lines = text.splitlines()
    idx = next(i for i, l in enumerate(lines) if "static void InjectHooks();" in l)
    access = None
    for l in reversed(lines[:idx]):
        s = l.strip()
        if s in ("public:", "private:", "protected:"):
            access = s[:-1]
            break
        if re.match(r"^(class|struct)\b", s):
            access = "private" if s.startswith("class") else "public"
            break
    has_friend = "friend void InjectHooksMain();" in text
    if access in ("private", "protected") and not has_friend:
        if re.search(r"\bfriend\b", text):
            rep.warn(f"{rel}:{idx + 1}", "`InjectHooks` is private and only another `friend` can call it - make sure that friend does")
        else:
            rep.err(f"{rel}:{idx + 1}", "`InjectHooks` is private but there is no `friend void InjectHooksMain();`")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", help="files to check (default: changed vs --base)")
    ap.add_argument("--base", default="master", help="git base for the changed-files mode (default: master)")
    ap.add_argument("--all", action="store_true", help="check every file under source/")
    args = ap.parse_args()

    root = repo_root()
    if args.all:
        files = [p for p in (root / "source").rglob("*") if p.suffix in (".cpp", ".h", ".hpp")]
    elif args.files:
        files = [(root / f).resolve() if not Path(f).is_absolute() else Path(f) for f in args.files]
    else:
        files = changed_files(root, args.base)

    if not files:
        print("no source files to check")
        return

    global INJECT_MAIN_RAW
    INJECT_MAIN_RAW = (root / "source" / "InjectHooksMain.cpp").read_text(encoding="utf-8", errors="replace")
    # Every `X::InjectHooks();` call in the code base (usually InjectHooksMain, sometimes another class' InjectHooks)
    inject_main = "\n".join(
        line
        for p in (root / "source").rglob("*.cpp")
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines()
        if "::InjectHooks" in line and not line.lstrip().startswith("//") and not re.search(r"\bvoid\s+[\w:]+::InjectHooks", line)
    )
    rep = Report()
    for f in files:
        if not f.exists():
            continue
        if f.suffix == ".cpp":
            check_cpp(f, root, inject_main, rep)
        else:
            check_header(f, root, rep)

    print(f"\nchecked {len(files)} file(s): {rep.errors} error(s), {rep.warnings} warning(s)")
    sys.exit(1 if rep.errors else 0)


if __name__ == "__main__":
    main()
