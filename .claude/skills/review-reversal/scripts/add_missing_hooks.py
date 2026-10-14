#!/usr/bin/env python3
"""
Register `RH_*Install(..., { .Reversed = false })` for `plugin::Call` stubs that have NO hook (docs/HookRegistration.md:
"All functions present in the class must have a matching RH_Install"). Behaviour doesn't change - the hook redirects
to GTA, exactly like the stub - but the function becomes visible in the hooks GUI, hooks.json and progress tools.

  python .claude/skills/review-reversal/scripts/add_missing_hooks.py                 # dry run (what would be added / skipped and why)
  python .claude/skills/review-reversal/scripts/add_missing_hooks.py --apply         # edit the .cpp files
  python .claude/skills/review-reversal/scripts/add_missing_hooks.py --file source/game_sa/Garage.cpp --apply

Conservative: only non-virtual member/static functions of the class whose `InjectHooks()` lives in the same .cpp,
no constructors/destructors, unique hook names (overloads get `RH_ScopedOverloadedInstall` with a cast + suffix).
Commented-out placeholders (`// RH_ScopedInstall(Fn, 0xADDR);`) are revived in place. Everything else is reported for
manual handling. Always build afterwards.
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
STUB_RE = re.compile(r"plugin::Call(Method)?(AndReturn)?<[^>]*?\b(0x[0-9A-Fa-f]{5,8})")
DEF_RE = re.compile(r"^(?P<ret>[\w:<>,\t \*&]+?)[\t ]*\b(?P<cls>[A-Za-z_]\w*)::(?P<fn>~?[A-Za-z_]\w*)\s*\((?P<params>[^)]*)\)\s*(?P<const>const)?\s*\{", re.M)
INSTALL_ADDR_RE = re.compile(r"RH_Scoped\w*Install\([^;\n]*?(0x[0-9A-Fa-f]{5,8})")
SKIP_DIRS = ("source/game_sa/RenderWare/", "source/app/", "source/oswrapper/")


def split_params(params: str) -> list[str]:
    params = re.sub(r"/\*.*?\*/|//[^\n]*", "", params, flags=re.S)  # comments inside the parameter list
    out, depth, cur = [], 0, ""
    for ch in params:
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return [p.strip() for p in out if p.strip() and p.strip() != "void"]


def param_type(p: str) -> str:
    p = p.split("=")[0].strip()
    m = re.match(r"^(.*?[\s\*&])([A-Za-z_]\w*)(\s*\[[^\]]*\])?$", p)
    if m and m.group(1).strip():
        return m.group(1).strip() + ("*" if m.group(3) else "")
    return p


def inject_hooks_span(text: str) -> tuple[str | None, int | None]:
    """(class of the first `X::InjectHooks()` in the file, index of its closing brace)."""
    m = re.search(r"void\s+([\w:]+)::InjectHooks\s*\(\s*\)\s*\{", text)
    if not m:
        return None, None
    depth, i = 0, m.end() - 1
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return m.group(1).split("::")[-1], i
        i += 1
    return m.group(1).split("::")[-1], None


def body_of(text: str, open_idx: int) -> str:
    depth, j = 0, open_idx
    while j < len(text):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                break
        j += 1
    return text[open_idx:j]


def process(path: Path, apply: bool) -> tuple[int, int]:
    rel = path.relative_to(REPO).as_posix()
    text = path.read_text(encoding="utf-8", errors="replace")
    if "plugin::Call" not in text:
        return 0, 0
    cls, _ = inject_hooks_span(text)
    live = "\n".join(l for l in text.splitlines() if not l.lstrip().startswith("//"))
    installed_addrs = {int(a, 16) for a in INSTALL_ADDR_RE.findall(live)}
    installed_names = set(re.findall(r"RH_Scoped(?:VMT)?Install\(\s*(\w+)\s*,", live))
    installed_names |= {f"{a}-{b}" for a, b in re.findall(r"RH_Scoped\w*OverloadedInstall\(\s*(\w+)\s*,\s*\"([^\"]*)\"", live)}
    header = path.with_suffix(".h")
    header_text = header.read_text(encoding="utf-8", errors="replace") if header.exists() else ""

    appended: list[str] = []
    revived: dict[int, str] = {}
    reports: list[str] = []
    for d in DEF_RE.finditer(text):
        body = re.sub(r"//[^\n]*", "", body_of(text, d.end() - 1))
        sm = STUB_RE.search(body)
        if not sm:
            continue
        addr = int(sm.group(3), 16)
        fn, dcls = d.group("fn"), d.group("cls")
        if addr in installed_addrs or fn == "InjectHooks":
            continue
        decls = [l for l in header_text.splitlines() if re.search(rf"\b{re.escape(fn)}\s*\(", l) and not l.lstrip().startswith("//")]
        why = None
        if not cls:
            why = "no InjectHooks() in this file"
        elif dcls != cls:
            why = f"belongs to `{dcls}`, InjectHooks scope is `{cls}`"
        elif fn in (cls, "~" + cls, "Constructor", "Destructor"):
            why = "constructor/destructor - register manually"
        elif any(re.search(r"\b(virtual|override)\b", l) for l in decls):
            why = "virtual - needs RH_ScopedVMTInstall (+ RH_ScopedVirtualClass scope) - register manually"
        elif not decls:
            why = "declaration not found in the header"
        elif re.search(r"template\s*<[^>]*>\s*$", text[max(0, d.start() - 200):d.start()]):
            why = "template - register an instantiation manually, eg. RH_ScopedInstall(Fn<CPtrListDoubleLink<CPhysical*>>, 0x...)"
        if why:
            reports.append(f"  SKIP 0x{addr:X} {dcls}::{fn}: {why}")
            continue

        is_static = any(re.search(r"\bstatic\b", l) for l in decls)
        if len(decls) > 1 or fn in installed_names:
            ptypes = [param_type(p) for p in split_params(d.group("params"))]
            ret = re.sub(r"\b(static|inline|virtual|NOTSA_FORCEINLINE)\b", "", d.group("ret")).strip()
            args_ = ", ".join(ptypes)
            cast = f"{ret}(*)({args_})" if is_static else f"{ret}({cls}::*)({args_}){' const' if d.group('const') else ''}"
            suffix = "-".join(re.sub(r"[^\w]", "", t.replace("*", "Ptr").replace("&", "Ref")) for t in ptypes) or "void"
            if f"{fn}-{suffix}" in installed_names:
                suffix += f"-{addr:X}"
            line = f'    RH_ScopedOverloadedInstall({fn}, "{suffix}", 0x{addr:X}, {cast}, {{ .Reversed = false }});'
            installed_names.add(f"{fn}-{suffix}")
        else:
            line = f"    RH_ScopedInstall({fn}, 0x{addr:X}, {{ .Reversed = false }});"
            installed_names.add(fn)
        installed_addrs.add(addr)
        if re.search(rf"^[ \t]*//\s*RH_Scoped\w*Install\([^\n]*0x{addr:X}\b", text, re.M | re.I):
            revived[addr] = line
        else:
            appended.append(line)

    if appended or revived or reports:
        print(f"{rel}:")
        for l in appended:
            print("  ADD       " + l.strip())
        for l in revived.values():
            print("  UNCOMMENT " + l.strip())
        for r in reports:
            print(r)

    if apply and (appended or revived):
        for addr, line in revived.items():
            text = re.sub(rf"^[ \t]*//\s*RH_Scoped\w*Install\([^\n]*0x{addr:X}\b[^\n]*$", lambda _m, l=line: l, text, count=1, flags=re.M | re.I)
        if appended:
            _, close_idx = inject_hooks_span(text)
            line_start = text.rfind("\n", 0, close_idx) + 1
            block = "\n    // Stubs forwarding to GTA, registered by add_missing_hooks.py\n" + "\n".join(appended) + "\n"
            text = text[:line_start] + block.lstrip("\n") + text[line_start:]
        path.write_text(text, encoding="utf-8", newline="")
    return len(appended) + len(revived), len(reports)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--file")
    args = ap.parse_args()
    files = [REPO / args.file] if args.file else sorted((REPO / "source").rglob("*.cpp"))
    added = skipped = 0
    for path in files:
        if path.relative_to(REPO).as_posix().startswith(SKIP_DIRS):
            continue
        a, s = process(path, args.apply)
        added += a
        skipped += s
    print(f"\n{added} hook(s) {'registered' if args.apply else 'to register'}, {skipped} skipped (register manually)")


if __name__ == "__main__":
    main()
