#!/usr/bin/env python3
"""
Compare struct layouts in the repo (VALIDATE_SIZE / VALIDATE_OFFSET) with the IDA database types.

  python .claude/skills/find-reversing-targets/scripts/struct_check.py --export      # IDA -> .claude/cache/structs_export.json
  python .claude/skills/find-reversing-targets/scripts/struct_check.py               # report mismatches
  python .claude/skills/find-reversing-targets/scripts/struct_check.py --type CPed   # one type: repo asserts vs IDB members
  python .claude/skills/find-reversing-targets/scripts/struct_check.py --markdown    # for docs

Checks:
  SIZE      VALIDATE_SIZE(T, n) != sizeof(T) in the IDB
  OFFSET    VALIDATE_OFFSET(T, member, off): the IDB has no member starting at `off` (field boundaries disagree)
  NOSIZE    repo type with VALIDATE_OFFSETs but no VALIDATE_SIZE while the IDB knows its size (add one)
The IDB is community-made and can be wrong too - a mismatch means "look at both", not "the repo is wrong".
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
CACHE = REPO / ".claude" / "cache" / "structs_export.json"
SIZE_RE = re.compile(r"VALIDATE_SIZE\(\s*([\w:<>, ]+?)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)")
OFF_RE = re.compile(r"VALIDATE_OFFSET\(\s*([\w:<>]+)\s*,\s*([\w.\[\]]+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)")


def export() -> None:
    sys.path.insert(0, str(HERE))
    from ida_rpc import call
    CACHE.parent.mkdir(parents=True, exist_ok=True)
    code = (f"import os; os.environ['GTASA_STRUCTS_EXPORT'] = {str(CACHE)!r}\n"
            f"exec(open({str(HERE / 'ida_export_structs.py')!r}, encoding='utf-8').read(), {{'__name__': '__main__'}})")
    r = call("py_eval", {"code": code})
    print(r.get("stdout", "").strip(), r.get("stderr", "").strip()[:2000])


def repo_asserts() -> tuple[dict, dict]:
    sizes, offsets = {}, defaultdict(list)
    for p in (REPO / "source").rglob("*.h*"):
        text = p.read_text(encoding="utf-8", errors="replace")
        rel = p.relative_to(REPO).as_posix()
        for m in SIZE_RE.finditer(text):
            line = text.count("\n", 0, m.start()) + 1
            sizes[m.group(1).strip()] = (int(m.group(2), 0), f"{rel}:{line}")
        for m in OFF_RE.finditer(text):
            line = text.count("\n", 0, m.start()) + 1
            offsets[m.group(1).strip()].append((m.group(2), int(m.group(3), 0), f"{rel}:{line}"))
    return sizes, offsets


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--export", action="store_true")
    ap.add_argument("--type")
    ap.add_argument("--markdown", action="store_true")
    args = ap.parse_args()
    if args.export:
        return export()
    if not CACHE.exists():
        sys.exit("error: run with --export first (IDA open with ida-pro-mcp)")
    idb = json.loads(CACHE.read_text(encoding="utf-8"))
    sizes, offsets = repo_asserts()

    if args.type:
        t = idb.get(args.type)
        print(f"{args.type}: repo VALIDATE_SIZE = {hex(sizes[args.type][0]) if args.type in sizes else 'none'}"
              f", IDB size = {hex(t['size']) if t else 'unknown'}")
        repo_off = {off: (m, w) for m, off, w in offsets.get(args.type, [])}
        if t:
            for mem in t["members"]:
                r = repo_off.pop(mem["off"], None)
                print(f"  +0x{mem['off']:04X} {mem['size']:4}  {mem['name']:32} {mem['type'][:40]:40}" + (f"  repo: {r[0]}" if r else ""))
        for off, (m, w) in sorted(repo_off.items()):
            print(f"  +0x{off:04X}  repo member `{m}` ({w}) - no IDB member starts here")
        return

    rows = []
    for name, (size, where) in sorted(sizes.items()):
        t = idb.get(name)
        if t and t["size"] and t["size"] != size:
            rows.append(("SIZE", name, f"repo 0x{size:X} vs IDB 0x{t['size']:X}", where))
    for name, offs in sorted(offsets.items()):
        t = idb.get(name)
        if not t:
            continue
        starts = {m["off"] for m in t["members"]}
        for member, off, where in offs:
            if off not in starts and not t["union"]:
                inside = next((m for m in t["members"] if m["off"] < off < m["off"] + max(m["size"], 1)), None)
                hint = f" (inside IDB member `{inside['name']}` +0x{inside['off']:X} size {inside['size']})" if inside else ""
                rows.append(("OFFSET", name, f"`{member}` @ 0x{off:X}: no IDB member starts there{hint}", where))
        if name not in sizes and t["size"]:
            rows.append(("NOSIZE", name, f"no VALIDATE_SIZE; IDB size is 0x{t['size']:X}", offs[0][2]))

    common = sum(1 for n in sizes if n in idb)
    summary = (f"repo VALIDATE_SIZE: {len(sizes)} types ({common} also in the IDB); IDB structs/unions: {len(idb)}; "
               f"findings: {sum(r[0] == 'SIZE' for r in rows)} SIZE, {sum(r[0] == 'OFFSET' for r in rows)} OFFSET, "
               f"{sum(r[0] == 'NOSIZE' for r in rows)} NOSIZE")
    if args.markdown:
        print(summary + "\n\n| Kind | Type | Detail | Where |\n|---|---|---|---|")
        for k, n, d, w in rows:
            print(f"| {k} | `{n}` | {d} | `{w}` |")
        return
    for k, n, d, w in rows:
        print(f"[{k:6}] {n:36} {d}  ({w})")
    print("\n" + summary)


if __name__ == "__main__":
    main()
