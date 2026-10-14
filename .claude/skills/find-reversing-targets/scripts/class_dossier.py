#!/usr/bin/env python3
"""
Class dossier: everything needed before reversing a class, in ONE markdown report (instead of 10-20 tool calls).

  python .claude/skills/find-reversing-targets/scripts/class_dossier.py CGarage
  python .claude/skills/find-reversing-targets/scripts/class_dossier.py CGarage --ida        # + live IDA data (struct members, callers)
  python .claude/skills/find-reversing-targets/scripts/class_dossier.py CGarage --out .claude/cache/dossiers/CGarage.md

Sections:
  1. Where it lives in the repo (header/source, InjectHooks registration, VALIDATE_SIZE, StaticRefs)
  2. Every method in the binary (IDB) with repo status, size, address
  3. Vtable (from RH_ScopedVirtualClass / IDB `vtable for'X`): slot -> function -> status
  4. Struct layout: repo VALIDATE_SIZE/OFFSET vs IDB type size (+ IDB members with --ida)
  5. Dependencies of each NOT-reversed method: callees (reversed or not), globals, float constants - and a suggested
     order (leaf-first, smallest-first)
  6. (--ida) Callers of the class' unreversed methods - how arguments are used

Needs .claude/cache/idb_export.json (ida_rpc.py export) and the exe path (.claude/gtasa_exe_path.txt).
"""
from __future__ import annotations

import argparse
import json
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / ".claude" / "skills" / "validate-against-binary" / "scripts"))
import asm_validate as av  # noqa: E402
import idb_coverage as cov  # noqa: E402


def find_repo_files(repo: av.Repo, cls: str) -> list[str]:
    rx = re.compile(rf"^\s*(?:class|struct)\s+(?:NOTSA_EXPORT_VTABLE\s+)?{re.escape(cls)}\b(?!\s*;)", re.M)
    hdrs = [rel for rel, t in repo.stripped.items() if rel.endswith((".h", ".hpp")) and rx.search(t)]
    out = []
    for h in hdrs:
        out.append(h)
        cpp = str(Path(h).with_suffix(".cpp").as_posix())
        if cpp in repo.files:
            out.append(cpp)
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cls")
    ap.add_argument("--ida", action="store_true", help="query the running IDA (ida-pro-mcp) for struct members and callers")
    ap.add_argument("--out", help="write to this file instead of stdout")
    args = ap.parse_args()
    cls = args.cls

    exp_path = REPO / ".claude" / "cache" / "idb_export.json"
    if not exp_path.exists():
        sys.exit("error: run `python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export` first (IDA open)")
    exp = json.loads(exp_path.read_text(encoding="utf-8"))
    repo = av.Repo(REPO)
    status = cov.repo_status(repo)
    pe = av.PEImage(av.resolve_exe(REPO, None))

    out: list[str] = []
    w = out.append

    methods = [f for f in exp["funcs"] if cov.class_of(f["dname"]) == cls and not f["lib"]]
    for f in methods:
        f["status"] = status.get(f["ea"], "MISSING")
    methods.sort(key=lambda f: f["ea"])
    c = cov.Counter(f["status"] for f in methods)
    w(f"# Dossier: `{cls}`\n")
    w(f"{len(methods)} methods in the binary - " + ", ".join(f"{s.lower()} {c[s]}" for s in cov.STATUSES) + "\n")

    # 1. repo
    w("## 1. Repo\n")
    files = find_repo_files(repo, cls)
    if not files:
        w(f"- **Not in the repo.** Create it with the `reverse-class` skill (directory = subsystem = hook category).\n")
    for rel in files:
        text = repo.files[rel]
        w(f"- `{rel}`")
        for m in re.finditer(rf"VALIDATE_SIZE\(\s*{re.escape(cls)}\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)", text):
            w(f"  - `VALIDATE_SIZE({cls}, {m.group(1)})`")
        n_off = len(re.findall(rf"VALIDATE_OFFSET\(\s*{re.escape(cls)}\s*,", text))
        if n_off:
            w(f"  - {n_off} `VALIDATE_OFFSET`s")
        for m in re.finditer(r"RH_Scoped(VirtualClass|Class|NamedClass|Namespace)\(([^)]*)\)", text):
            w(f"  - `RH_Scoped{m.group(1)}({m.group(2).strip()})`")
        refs = re.findall(r"auto&\s+(\w+)\s*=\s*StaticRef<([^;]*?)>\s*\(\s*(0x[0-9A-Fa-f]+)", text)
        for name, typ, addr in refs[:20]:
            w(f"  - StaticRef `{name}` @ {addr} : `{typ.strip()}`")
    if files:
        inj = re.search(rf"\b{re.escape(cls)}::InjectHooks\s*\(\s*\)\s*;", repo.files.get("source/InjectHooksMain.cpp", ""))
        w(f"- `{cls}::InjectHooks()` called from InjectHooksMain: **{'yes' if inj else 'NO - hooks are never installed!'}**")
    w("")

    # 2. methods
    w("## 2. Methods (binary order)\n")
    w("| Address | Size | Status | Method | Repo |\n|---|---:|---|---|---|")
    for f in methods:
        syms = repo.func_names(f["ea"])
        where = syms[0].where if syms else ""
        w(f"| `0x{f['ea']:06X}` | {f['size']} | {f['status']} | `{f['dname']}` | {where} |")
    w("")

    # 3. vtable
    vt_addr, vt_count = None, None
    for rel in files:
        m = re.search(rf"RH_ScopedVirtualClass\(\s*{re.escape(cls)}\s*,\s*(0x[0-9A-Fa-f]+)\s*,\s*(\d+)\s*\)", repo.files[rel])
        if m:
            vt_addr, vt_count = int(m.group(1), 16), int(m.group(2))
    if vt_addr is None:
        n = next((n for n in exp["names"] if n["dname"] in (f"`vtable for'{cls}", f"{cls}::`vftable'")), None)
        if n:
            vt_addr = n["ea"] + (8 if "vtable for" in n["dname"] else 0)  # Itanium-style names point at offset-to-top/RTTI
    if vt_addr:
        w(f"## 3. Vtable @ 0x{vt_addr:X}" + (f" ({vt_count} entries per RH_ScopedVirtualClass)" if vt_count else "") + "\n")
        w("| Slot | Offset | Function | Status |\n|---:|---:|---|---|")
        fn_by_ea = {f["ea"]: f for f in exp["funcs"]}
        for i in range(vt_count or 64):
            v = pe.read_u32(vt_addr + i * 4)
            if v is None or not pe.is_code(v):
                break
            f = fn_by_ea.get(v)
            w(f"| {i} | 0x{i * 4:X} | `{f['dname'] if f else hex(v)}` (0x{v:X}) | {status.get(v, 'MISSING')} |")
        w("")

    # 4. struct
    w("## 4. Struct layout\n")
    t = next((t for t in exp["types"] if t["name"] == cls), None)
    repo_size = None
    for rel in files:
        m = re.search(rf"VALIDATE_SIZE\(\s*{re.escape(cls)}\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)", repo.files[rel])
        if m:
            repo_size = int(m.group(1), 0)
    w(f"- IDB type size: {hex(t['size']) if t and t['size'] else 'unknown'}; repo VALIDATE_SIZE: {hex(repo_size) if repo_size else 'none'}"
      + ("  **MISMATCH**" if t and t["size"] and repo_size and t["size"] != repo_size else ""))
    if args.ida:
        from ida_rpc import call
        r = call("type_inspect", {"name": cls}) if True else {}
        w("\n```\n" + json.dumps(r, indent=1)[:6000] + "\n```")
    w("")

    # 5. dependencies of not-reversed methods
    todo = [f for f in methods if f["status"] != "REVERSED"]
    w(f"## 5. Dependencies of the {len(todo)} not-reversed methods\n")
    plan = []
    for f in todo:
        try:
            fp = av.disassemble(pe, f["ea"])
        except ValueError:
            continue
        callees = []
        blocked = 0
        for tgt, n in sorted(fp.calls.items()):
            if tgt >= av.CRT_START:
                continue
            st = status.get(tgt, "MISSING")
            name = (repo.func_names(tgt)[0].name if repo.func_names(tgt) else None) or repo.idb_name(tgt) or f"sub_{tgt:X}"
            if st != "REVERSED" and av.leaf_size(pe, tgt) is None and tgt != f["ea"]:
                blocked += 1
            callees.append(f"{name} [{st}]" + (f" x{n}" if n > 1 else ""))
        globals_ = sorted({(repo.globals_[a].name if a in repo.globals_ else (repo.idb_name(a) or hex(a))) for a in fp.globals_})
        plan.append((blocked, f["size"], f))
        w(f"### `{f['dname']}` - 0x{f['ea']:X}, {f['size']} bytes, {f['status']}")
        w(f"- calls: {', '.join(callees) if callees else 'nothing (leaf)'}")
        if fp.virtual_calls:
            w(f"- virtual calls: {', '.join(d.split('(')[-1].rstrip(')') for _, d in fp.virtual_calls[:8])}")
        if globals_:
            w(f"- globals: {', '.join(globals_[:15])}")
        if fp.floats:
            w(f"- float constants: {', '.join(repr(v) for v in sorted(fp.floats)[:12])}")
        w("")
    if plan:
        w("### Suggested order (callees ready first, then smallest)\n")
        for i, (blocked, size, f) in enumerate(sorted(plan, key=lambda p: (p[0] > 0, p[0], p[1])), 1):
            w(f"{i}. `{f['dname']}` (0x{f['ea']:X}, {size} B)" + (f" - waits on {blocked} non-reversed callee(s)" if blocked else " - all callees available"))
        w("")

    # 6. callers (IDA)
    if args.ida and todo:
        from ida_rpc import call
        w("## 6. Callers of not-reversed methods (IDA)\n")
        for f in todo[:25]:
            r = call("xrefs_to", {"addrs": hex(f["ea"]), "limit": 10})
            items = r.get("result", [{}])[0].get("xrefs", []) if isinstance(r.get("result"), list) else []
            callers = sorted({x.get("fn", {}).get("name", x.get("addr")) if isinstance(x.get("fn"), dict) else str(x.get("addr")) for x in items})
            w(f"- `{f['dname']}` <- {', '.join(callers) if callers else '(no xrefs / virtual only)'}")
        w("")

    text = "\n".join(out)
    if args.out:
        Path(args.out).parent.mkdir(parents=True, exist_ok=True)
        Path(args.out).write_text(text, encoding="utf-8")
        print(f"wrote {args.out}")
    else:
        print(text)


if __name__ == "__main__":
    main()
