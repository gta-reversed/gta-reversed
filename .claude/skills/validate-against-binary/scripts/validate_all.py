#!/usr/bin/env python3
"""
Run asm_validate on EVERY reversed hook and save structured results (JSON lines) for reporting / mining.

  python .claude/skills/validate-against-binary/scripts/validate_all.py                     # -> .claude/cache/validate_all.jsonl
  python .claude/skills/validate-against-binary/scripts/validate_all.py --filter Ped --out x.jsonl

Each record: {addr, name, where, findings, checks, lines: [...], extra: [...names ours calls but original doesn't...]}.
Consumed by inline_catalogue.py. Takes ~0.3 s per function.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
import types
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import asm_validate as av  # noqa: E402


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=None)
    ap.add_argument("--filter", default="")
    args = ap.parse_args()

    root = av.repo_root()
    out = Path(args.out) if args.out else root / ".claude" / "cache" / "validate_all.jsonl"
    out.parent.mkdir(parents=True, exist_ok=True)
    pe = av.PEImage(av.resolve_exe(root, None))
    repo = av.Repo(root)
    addrs = sorted({a for rel in repo.files if rel.endswith(".cpp") and args.filter.lower() in rel.lower()
                    for a in av.addresses_in_file(repo, rel)})
    opts = types.SimpleNamespace(size=None, dump=False, no_check=False, cpp_file=None, cpp_func=None, helper_depth=2, strict=False)
    with out.open("w", encoding="utf-8") as fp:
        for i, addr in enumerate(addrs):
            rep = av.Report(False)
            try:
                av.validate(repo, pe, addr, opts, rep)
            except Exception as e:  # keep going, record the error
                rep.lines.append(f"  [ERROR]   {type(e).__name__}: {e}")
            syms = repo.func_names(addr)
            extra = []
            for l in rep.lines:
                m = re.search(r"doesn't call directly: (.*?) \(fine", l)
                if m:
                    extra = [x.strip() for x in m.group(1).split(",")]
            fp.write(json.dumps({
                "addr": addr, "name": syms[0].name if syms else None, "where": syms[0].where if syms else None,
                "findings": rep.findings, "checks": rep.checks,
                "lines": [l.strip() for l in rep.lines if re.search(r"\[(MISSING|CHECK|ERROR)\]", l)], "extra": extra,
            }) + "\n")
            if i % 250 == 0:
                print(f"{i}/{len(addrs)}", flush=True)
    print(f"done: {len(addrs)} functions -> {out}")


if __name__ == "__main__":
    main()
