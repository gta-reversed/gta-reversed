#!/usr/bin/env python3
"""
Push names the REPO knows into the IDA database, so Hex-Rays output gets closer to the final C++.

Only AUTO/placeholder names are replaced (sub_XXXX, _sub_XXXX, nullsub_, dword_/unk_/byte_/flt_/off_...): names someone typed in the IDB are
never overwritten. Nothing is saved - review in IDA and save (or close without saving) yourself.

  python .claude/skills/find-reversing-targets/scripts/repo_to_idb.py              # dry run: list what would change
  python .claude/skills/find-reversing-targets/scripts/repo_to_idb.py --apply      # rename in the running IDA

Sources: RH_*Install / `// 0xADDR` names (functions) and StaticRef variables (globals; class statics get `Class::`).
Needs IDA open with ida-pro-mcp and a fresh `ida_rpc.py export`. Back up your .i64 first.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / ".claude" / "skills" / "validate-against-binary" / "scripts"))
import asm_validate as av  # noqa: E402

DUMMY = re.compile(r"^_?(sub|nullsub|unk|byte|word|dword|qword|flt|dbl|off|asc|stru|loc|j_sub)_[0-9A-Fa-f]+$")
OWNER_RE = re.compile(r"^\s*(?:class|struct|namespace)\s+(?:NOTSA_EXPORT_VTABLE\s+)?([A-Za-z_]\w*)\b(?![^{;]*;)", re.M)

APPLY_SCRIPT = r'''
import json, os, re, ida_name, ida_bytes
PLACEHOLDER = re.compile(r"^_?(sub|nullsub|unk|byte|word|dword|qword|flt|dbl|off|asc|stru|loc|j_sub)_[0-9A-Fa-f]+$")
renames = json.load(open(os.environ["RENAMES_JSON"], encoding="utf-8"))
done = skipped = 0
for ea, name in renames:
    cur = ida_name.get_name(ea)
    if cur and not ida_bytes.has_dummy_name(ida_bytes.get_flags(ea)) and not PLACEHOLDER.match(cur):
        skipped += 1      # a real name exists - never overwrite
        continue
    if ida_name.set_name(ea, name, ida_name.SN_NOCHECK | ida_name.SN_NOWARN | ida_name.SN_FORCE):
        done += 1
print(f"renamed {done}, skipped {skipped} (already named)")
'''


def owner_of(repo: av.Repo, where: str) -> str | None:
    rel, line = where.rsplit(":", 1)
    text = "\n".join(repo.files[rel].splitlines()[: int(line)])
    owners = OWNER_RE.findall(text)
    return owners[-1] if owners else None


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    exp = json.loads((REPO / ".claude" / "cache" / "idb_export.json").read_text(encoding="utf-8"))
    repo = av.Repo(REPO)
    fn = {f["ea"]: f["name"] for f in exp["funcs"]}
    nm = {n["ea"]: n["name"] for n in exp["names"]}

    renames: list[tuple[int, str]] = []
    for a, syms in repo.funcs.items():
        if a in fn and DUMMY.match(fn[a]) and syms:
            name = syms[0].name
            if name.endswith(("::Constructor", "::Destructor")):
                cls = name.split("::")[-2]
                name = f"{cls}::{cls}" if name.endswith("Constructor") else f"{cls}::~{cls}"
            renames.append((a, name))
    for a, sym in repo.globals_.items():
        cur = nm.get(a)
        if cur is not None and not DUMMY.match(cur):
            continue
        name = sym.name
        if re.match(r"^(ms_|m_)", name):
            owner = owner_of(repo, sym.where)
            if owner:
                name = f"{owner}::{name}"
        renames.append((a, name))

    print(f"{len(renames)} renames ({sum(1 for a, _ in renames if a in fn)} functions, {sum(1 for a, _ in renames if a not in fn)} globals)")
    for a, n in sorted(renames)[:40]:
        print(f"  0x{a:06X}  {fn.get(a) or nm.get(a) or '(unnamed)'} -> {n}")
    if len(renames) > 40:
        print(f"  ... {len(renames) - 40} more")
    if not args.apply:
        print("\ndry run - pass --apply to rename in the running IDA (nothing is saved)")
        return

    from ida_rpc import call
    tmp = REPO / ".claude" / "cache" / "_renames.json"
    tmp.write_text(json.dumps(renames), encoding="utf-8")
    script = REPO / ".claude" / "cache" / "_apply_renames.py"
    script.write_text(f"import os\nos.environ['RENAMES_JSON'] = {str(tmp)!r}\n" + APPLY_SCRIPT, encoding="utf-8")
    r = call("py_exec_file", {"file_path": str(script)})
    print(r.get("stdout", "").strip(), r.get("stderr", "").strip())
    print("Done. Review in IDA; save the database only if you want to keep the names.")


if __name__ == "__main__":
    main()
