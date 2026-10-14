#!/usr/bin/env python3
"""
REAL reversing coverage of the GAME's own code: every function of gta_sa.exe (from the IDA database export) vs the repo.

`docs/hooks.json` / `ReversedClasses.md` only count functions that already have a hook. This script starts from the
binary instead, so it also shows the functions/classes nobody has registered yet.

Scope (default `methods`): only code written for the game -
  * inside the game code region of the exe (not RenderWare, D3DX, libpng, MSVC CRT, static initialisers/EH funclets),
  * not a library function (IDA FLIRT `FUNC_LIB`), not a thunk, not a RenderWare/D3DX-named helper,
  * a member function of a GAME class (`CPed::Update`, `FxSystem_c::Play`, ...). Free functions, unnamed `sub_`s and
    classes that are FLIRT noise from other libraries (`std::`, `ATL::`, `CryptoPP`, Qt...) are excluded.
`--scope game` widens it to every function in the game region (adds free functions + unnamed/tiny helpers).

Repo status of each function:
  REVERSED  - hooked and reversed
  DORMANT   - reversed, but the class' InjectHooks() is never called, so the hook is never installed
  STUB      - hooked with `.Reversed = false` (forwards to GTA)
  NOHOOK    - `plugin::Call` stub in the repo WITHOUT an RH install (violates docs/HookRegistration.md)
  KNOWN     - address appears in the repo (comment, declaration) but isn't hooked
  MISSING   - not in the repo at all

Usage (repo root; needs .claude/cache/idb_export.json -> `python .../ida_rpc.py export` with IDA open):
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py                  # summary
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --classes 40     # classes with most work left
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class CCam     # one class, function by function
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --missing 50 --filter Ped   # biggest missing functions
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --untouched 30   # classes with ZERO repo presence
  python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --markdown --classes 60   # tables for docs
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
sys.path.insert(0, str(REPO / ".claude" / "skills" / "validate-against-binary" / "scripts"))
from asm_validate import ADDR_RE, INSTALL_RE, Repo  # noqa: E402  (shared repo index)

# Address map of gta_sa.exe 1.0 US (compact) - derived from the IDB (see docs/BinaryMap.md)
REGIONS = [
    (0x401000, 0x5C0000, "game"),
    (0x5C0000, 0x5D0000, "game"),        # contains the embedded D3DX JPEG library (FUNC_LIB, filtered out)
    (0x5D0000, 0x748000, "game"),        # 0x72xxxx-0x748000 also holds RW plugins + the platform skeleton (filtered by name/FLIRT)
    (0x748000, 0x760000, "renderware"),  # RpWorld, D3D9 pipelines
    (0x760000, 0x7C0000, "d3dx"),        # D3DX9 math + shader compiler
    (0x7C0000, 0x7E0000, "rw-toolkits+libpng"),
    (0x7E0000, 0x820000, "renderware"),  # RW core, D3D9 driver, plugin registry, MatFX
    (0x820000, 0x840000, "crt"),         # statically linked MSVC CRT
    (0x840000, 0x857000, "init+eh"),     # static initialisers (GlobalInit_*) and EH unwind funclets
    (0x857000, 0x858000, "renderware"),  # _rwcseg
]
LIB_NAME_RE = re.compile(r"^_?_?(Rw|Rp|Rt|rw|rp|rt|Rx|rx|D3DX|c_D3DX|x3d_|png_|PNG|_D3D|D3D9|Im2D|Im3D|HAnim|UVAnim)")
# "Classes" that are FLIRT signature noise from unrelated libraries/programs (not GTA code)
FOREIGN_CLASS_RE = re.compile(r"^(std::|ATL::|CryptoPP|Q[A-Z]|K[A-Z][a-z]|lisp|TSmartPointer|MESH|REPLACED_BY|Multiply3x3|Render(::|$)|"
                              r"Securom|C$|D3DX|D3DXShader|D3DXCore|GXHalfEdge)")
PSEUDO_CLASSES = ("(unnamed)", "(trivial)", "(global)")
STATUSES = ("REVERSED", "DORMANT", "STUB", "NOHOOK", "KNOWN", "MISSING")


def region_of(ea: int) -> str:
    for lo, hi, name in REGIONS:
        if lo <= ea < hi:
            return name
    return "other"


def class_of(dname: str) -> str:
    base = re.sub(r"<[^<>]*>", "", (dname or "").split("(")[0])
    if re.match(r"^_?sub_[0-9A-Fa-f]+$", base):
        return "(unnamed)"
    if re.match(r"^_?(nullsub|dummy)_\w*$", base):
        return "(trivial)"
    return base.rsplit("::", 1)[0] if "::" in base else "(global)"


def is_game_region_function(f: dict) -> bool:
    """In the game region, not FLIRT library, not thunk, not RW/D3DX named."""
    if f["lib"] or f["thunk"] or region_of(f["ea"]) != "game":
        return False
    return not LIB_NAME_RE.match(f["dname"] or "")


def is_game_method(f: dict) -> bool:
    cls = class_of(f["dname"])
    return is_game_region_function(f) and cls not in PSEUDO_CLASSES and not FOREIGN_CLASS_RE.match(cls)


def repo_status(repo: Repo) -> dict[int, str]:
    status: dict[int, str] = {addr: "KNOWN" for addr in repo.funcs}
    stub_re = re.compile(r"plugin::Call\w*<[^>]*?\b(" + ADDR_RE + r")")
    for text in repo.stripped.values():
        for m in stub_re.finditer(text):
            status[int(m.group(1), 16)] = "NOHOOK"
    # Classes whose InjectHooks() is never called: their hooks are never installed -> DORMANT
    calls = "\n".join(l for t in repo.files.values() for l in t.splitlines()
                      if "::InjectHooks" in l and not l.lstrip().startswith("//") and not re.search(r"\bvoid\s+[\w:]+::InjectHooks", l))
    called = set(re.findall(r"(\w+)::InjectHooks\s*\(\s*\)\s*;", calls))
    for rel, text in repo.files.items():
        defined = re.search(r"void\s+(?:\w+::)*(\w+)::InjectHooks\s*\(\s*\)", text)
        dormant = bool(defined) and defined.group(1) not in called
        for line in text.splitlines():
            m = INSTALL_RE.match(line)
            if not m or line.lstrip().startswith("//"):
                continue
            a = re.search(ADDR_RE, m.group("rest"))
            if a:
                addr = int(a.group(0), 16)
                if status.get(addr) != "REVERSED":
                    reversed_ = not re.search(r"\.Reversed\s*=\s*false", line)
                    status[addr] = ("DORMANT" if dormant and reversed_ else "STUB" if not reversed_ else "REVERSED")
    return status


def pct(a: float, b: float) -> str:
    return f"{100 * a / b:.1f}%" if b else "-"


def stats(fs: list[dict]) -> Counter:
    c = Counter()
    for f in fs:
        c[f["status"]] += 1
        c["total"] += 1
        c["bytes"] += f["size"]
        c[f"bytes_{f['status']}"] += f["size"]
    return c


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--export", default=str(REPO / ".claude" / "cache" / "idb_export.json"))
    ap.add_argument("--scope", choices=("methods", "game"), default="methods", help="methods = game class methods only (default)")
    ap.add_argument("--classes", type=int, metavar="N", help="top N classes by bytes left to reverse")
    ap.add_argument("--class", dest="cls", help="list every function of one class (IDA name, eg. CCam)")
    ap.add_argument("--missing", type=int, metavar="N", help="N biggest functions not in the repo")
    ap.add_argument("--untouched", type=int, metavar="N", help="N biggest classes with no function in the repo")
    ap.add_argument("--filter", default="", help="substring filter on function/class name")
    ap.add_argument("--markdown", action="store_true", help="print markdown (summary + class table)")
    args = ap.parse_args()

    exp_path = Path(args.export)
    if not exp_path.exists():
        sys.exit(f"error: {exp_path} not found - open the IDB in IDA (with ida-pro-mcp) and run:\n"
                 f"  python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export")
    exp = json.loads(exp_path.read_text(encoding="utf-8"))
    status = repo_status(Repo(REPO))

    funcs = exp["funcs"]
    for f in funcs:
        f["status"] = status.get(f["ea"], "MISSING")
        f["cls"] = class_of(f["dname"])
    methods = [f for f in funcs if is_game_method(f)]
    game_all = [f for f in funcs if is_game_region_function(f)]
    scope = methods if args.scope == "methods" else game_all
    flt = args.filter.lower()

    classes: dict[str, Counter] = defaultdict(Counter)
    for f in scope:
        c = classes[f["cls"]]
        c[f["status"]] += 1
        c["total"] += 1
        c["bytes"] += f["size"]
        if f["status"] != "REVERSED":
            c["bytes_left"] += f["size"]

    def summary_lines() -> list[str]:
        m = stats(methods)
        n_classes = len({f["cls"] for f in methods})
        free = [f for f in game_all if f["cls"] == "(global)"]
        unnamed = [f for f in game_all if f["cls"] in ("(unnamed)", "(trivial)")]
        foreign = [f for f in game_all if f["cls"] not in PSEUDO_CLASSES and FOREIGN_CLASS_RE.match(f["cls"])]
        out = [
            f"IDB: {exp.get('input', exp_path)} - {len(funcs)} functions in total",
            "",
            f"GAME CLASS METHODS: {m['total']} functions in {n_classes} classes ({m['bytes'] / 1024:.0f} KiB of code)",
        ]
        for s, desc in (("REVERSED", "hooked + reversed"), ("DORMANT", "reversed, but its class' InjectHooks() is never called"),
                        ("STUB", "hooked, `.Reversed = false`"),
                        ("NOHOOK", "plugin::Call stub WITHOUT hook"), ("KNOWN", "address in repo, not hooked"),
                        ("MISSING", "not in the repo")):
            out.append(f"  {s.lower():9} {m[s]:6}  {pct(m[s], m['total']):>6} of functions  {pct(m['bytes_' + s], m['bytes']):>6} of bytes   ({desc})")
        out += [
            "",
            "Excluded from the count above:",
            f"  free (non-member) functions in game code   {len(free):6}  ({stats(free)['REVERSED']} reversed)",
            f"  unnamed/trivial sub_/nullsub_ in game code {len(unnamed):6}  (mostly tiny compiler-generated/inlined helpers)",
            f"  FLIRT noise classes (std::, ATL::, ...)    {len(foreign):6}",
        ]
        other = Counter(region_of(f["ea"]) for f in funcs if not is_game_region_function(f))
        other["library/RW helpers inside game region"] = other.pop("game", 0)
        for k, v in other.most_common():
            out.append(f"  {k:43}{v:6}")
        hooks_path = REPO / "docs" / "hooks.json"
        if hooks_path.exists():
            hooks = json.loads(hooks_path.read_text(encoding="utf-8"))
            done = sum(1 for h in hooks if h.get("IsReversed"))
            out += ["", f"(For comparison, docs/hooks.json only counts registered hooks: {done}/{len(hooks)} = {pct(done, len(hooks))})"]
        return out

    if args.markdown:
        print("```\n" + "\n".join(summary_lines()) + "\n```\n")
        print("| Class | Methods | Reversed | Stub | Unhooked stub | Known | Missing | Bytes left |\n|---|---:|---:|---:|---:|---:|---:|---:|")
        for name, c in sorted(classes.items(), key=lambda kv: -kv[1]["bytes_left"])[: args.classes or 40]:
            print(f"| `{name}` | {c['total']} | {c['REVERSED']} | {c['STUB']} | {c['NOHOOK']} | {c['KNOWN']} | {c['MISSING']} | {c['bytes_left']:,} |")
        return

    if args.cls:
        rows = [f for f in scope if f["cls"] == args.cls or f["cls"].endswith("::" + args.cls)]
        for f in sorted(rows, key=lambda f: f["ea"]):
            print(f"  0x{f['ea']:06X} {f['size']:6}  {f['status']:8}  {f['dname']}")
        c = stats(rows)
        print(f"\n{args.cls}: {c['total']} functions - " + ", ".join(f"{s.lower()} {c[s]}" for s in STATUSES))
        return

    if args.missing:
        rows = [f for f in scope if f["status"] == "MISSING" and (not flt or flt in f["dname"].lower())]
        for f in sorted(rows, key=lambda f: -f["size"])[: args.missing]:
            print(f"  0x{f['ea']:06X} {f['size']:6} bytes  {f['dname']}")
        print(f"\n{len(rows)} missing functions ({args.scope})" + (f" matching '{args.filter}'" if flt else ""))
        return

    if args.untouched:
        rows = [(n, c) for n, c in classes.items() if c["total"] == c["MISSING"] and n not in PSEUDO_CLASSES and (not flt or flt in n.lower())]
        for n, c in sorted(rows, key=lambda kv: -kv[1]["bytes"])[: args.untouched]:
            print(f"  {n:40} {c['total']:4} functions {c['bytes']:7,} bytes")
        print(f"\n{len(rows)} classes with no presence in the repo")
        return

    print("\n".join(summary_lines()))
    if args.classes:
        print(f"\nTop {args.classes} classes by bytes left to reverse ({args.scope}):")
        print(f"  {'class':40} {'total':>5} {'rev':>5} {'stub':>5} {'nohk':>5} {'known':>5} {'miss':>5} {'bytes left':>10}")
        rows = [(n, c) for n, c in classes.items() if not flt or flt in n.lower()]
        for n, c in sorted(rows, key=lambda kv: -kv[1]["bytes_left"])[: args.classes]:
            print(f"  {n[:40]:40} {c['total']:5} {c['REVERSED']:5} {c['STUB']:5} {c['NOHOOK']:5} {c['KNOWN']:5} {c['MISSING']:5} {c['bytes_left']:10,}")


if __name__ == "__main__":
    main()
