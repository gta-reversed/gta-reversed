#!/usr/bin/env python3
"""
Script command (SCM opcode) locator: name <-> opcode, repo status and the ORIGINAL implementation (the `case` inside
`CRunningScript::ProcessCommands<N>To<N+99>`).

  python .claude/skills/reverse-script-command/scripts/script_cmd.py export                  # IDA -> .claude/cache/script_cases.json (once)
  python .claude/skills/reverse-script-command/scripts/script_cmd.py locate SET_CHAR_PROOFS  # or COMMAND_X, 0x02AB, 683
  python .claude/skills/reverse-script-command/scripts/script_cmd.py locate 0x02AB --decompile   # Hex-Rays lines of that case (needs IDA)
  python .claude/skills/reverse-script-command/scripts/script_cmd.py locate 0x02AB --dump         # annotated disassembly from the exe (no IDA)
  python .claude/skills/reverse-script-command/scripts/script_cmd.py list --status unimplemented --sort size [--filter CHAR] [--top 50]
  python .claude/skills/reverse-script-command/scripts/script_cmd.py stats

Repo status per command: HANDLER (REGISTER_COMMAND_HANDLER), NOP, UNIMPLEMENTED (REGISTER_COMMAND_UNIMPLEMENTED),
UNREGISTERED (no registration at all -> GTA's original dispatcher runs it).
Case size = bytes between the case label and the next case target - a good proxy for difficulty.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
CACHE = REPO / ".claude" / "cache" / "script_cases.json"
ENUM = REPO / "source" / "game_sa" / "Enums" / "eScriptCommands.h"
CMDS = REPO / "source" / "game_sa" / "Scripts"


def load_enum() -> tuple[dict[str, int], dict[int, str]]:
    by_name, by_op = {}, {}
    text = ENUM.read_text(encoding="utf-8", errors="replace")
    text = re.sub(r"#if BUILD_XBOX.*?#else", "", text, flags=re.S)  # PC build
    for m in re.finditer(r"^\s*(COMMAND_\w+)\s*=\s*(0x[0-9A-Fa-f]+)", text, re.M):
        op = int(m.group(2), 16)
        by_name[m.group(1)] = op
        by_op.setdefault(op, m.group(1))
    return by_name, by_op


def load_repo_status() -> dict[str, tuple[str, str, str | None]]:
    """COMMAND_X -> (status, file:line, handler fn)"""
    st: dict[str, tuple[str, str, str | None]] = {}
    for p in CMDS.rglob("*.cpp"):
        for n, line in enumerate(p.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
            if line.lstrip().startswith("//"):
                continue
            where = f"{p.relative_to(REPO).as_posix()}:{n}"
            if m := re.search(r"REGISTER_COMMAND_HANDLER\(\s*(COMMAND_\w+)\s*,\s*([^)]+)\)", line):
                st[m.group(1)] = ("HANDLER", where, m.group(2).strip())
            elif m := re.search(r"REGISTER_COMMAND_NOP\(\s*(COMMAND_\w+)", line):
                st[m.group(1)] = ("NOP", where, None)
            elif m := re.search(r"REGISTER_COMMAND_UNIMPLEMENTED\(\s*(COMMAND_\w+)", line):
                st.setdefault(m.group(1), ("UNIMPLEMENTED", where, None))
    return st


def load_cases() -> dict[int, dict]:
    if not CACHE.exists():
        return {}
    return {int(k): v for k, v in json.loads(CACHE.read_text(encoding="utf-8"))["cases"].items()}


def resolve(arg: str, by_name: dict[str, int]) -> int:
    a = arg.strip()
    if re.fullmatch(r"0x[0-9A-Fa-f]+", a):
        return int(a, 16)
    if a.isdigit():
        return int(a)
    name = a if a.startswith("COMMAND_") else "COMMAND_" + a
    if name.upper() in by_name:
        return by_name[name.upper()]
    sys.exit(f"error: unknown command `{arg}`")


def handler_body(fn: str | None) -> str | None:
    if not fn:
        return None
    fn = re.sub(r"<.*", "", fn).split("::")[-1]
    rx = re.compile(rf"^[\w:<>,\s\*&]*\b{re.escape(fn)}\s*\(", re.M)
    for p in CMDS.rglob("*.cpp"):
        t = p.read_text(encoding="utf-8", errors="replace")
        m = rx.search(t)
        if m:
            return f"{p.relative_to(REPO).as_posix()}:{t.count(chr(10), 0, m.start()) + 1}"
    return None


def shared_case_result(addr: int) -> str | None:
    """Decode what a shared (no-op) case returns: `xor al, al` = OR_CONTINUE, `or al, 0FFh` = OR_INTERRUPT."""
    kind = classify_case(addr)
    if kind == "continue":
        return ("returns OR_CONTINUE (0) without reading its parameters - the command does nothing in the original. "
                "Note: REGISTER_COMMAND_UNIMPLEMENTED returns OR_INTERRUPT instead (and debug-breaks)")
    if kind == "interrupt":
        return "returns OR_INTERRUPT (-1) - the dispatcher's default path: the original doesn't implement this command"
    return None


_PE = None


def classify_case(addr: int) -> str:
    """'continue' / 'interrupt' if the case is a trivial return path (no calls, sets al to 0 / -1), else 'code'."""
    global _PE
    try:
        sys.path.insert(0, str(REPO / ".claude" / "skills" / "validate-against-binary" / "scripts"))
        import asm_validate as av
        import capstone
        _PE = _PE or av.PEImage(av.resolve_exe(REPO, None))
    except SystemExit:
        return "code"
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for _ in range(2):  # follow one jmp
        code = _PE.read(addr, 48) or b""
        nxt = None
        for insn in md.disasm(code, addr):
            text = f"{insn.mnemonic} {insn.op_str}"
            if insn.mnemonic == "call":
                return "code"
            if text == "xor al, al":
                return "continue"
            if text in ("or al, 0xff", "mov al, 0xff"):
                return "interrupt"
            if insn.mnemonic == "jmp":
                try:
                    nxt = int(insn.op_str, 16)
                except ValueError:
                    return "code"
                break
            if insn.mnemonic.startswith("ret") or insn.mnemonic.startswith("j"):
                return "code"
        if nxt is None:
            return "code"
        addr = nxt
    return "code"


def cmd_export() -> None:
    sys.path.insert(0, str(REPO / ".claude" / "skills" / "find-reversing-targets" / "scripts"))
    from ida_rpc import call
    CACHE.parent.mkdir(parents=True, exist_ok=True)
    code = (f"import os; os.environ['GTASA_SCRIPT_CASES_EXPORT'] = {str(CACHE)!r}\n"
            f"exec(open({str(HERE / 'ida_export_script_cases.py')!r}, encoding='utf-8').read(), {{'__name__': '__main__'}})")
    r = call("py_eval", {"code": code})
    print(r.get("stdout", "").strip(), r.get("stderr", "").strip())


def cmd_locate(args, by_name, by_op, status, cases) -> None:
    op = resolve(args.command, by_name)
    name = by_op.get(op, "?")
    st, where, fn = status.get(name, ("UNREGISTERED", None, None))
    case = cases.get(op)
    print(f"{name}  opcode 0x{op:04X} ({op})")
    print(f"  repo:     {st}" + (f" at {where}" if where else "") + (f" -> {fn} ({handler_body(fn)})" if fn else ""))
    if not case:
        print("  original: no case found in the dispatcher map" + ("" if cases else " (run `script_cmd.py export` with IDA open)"))
        return
    size = (case["end"] - case["case"]) if case["end"] else None
    shared = [by_op.get(o, hex(o)) for o, c in cases.items() if c["case"] == case["case"] and o != op]
    if shared:
        print(f"  shared:   same case target as {len(shared)} other opcode(s) (eg. {', '.join(shared[:4])}) -"
              f" a no-op / 'unsupported' path; `size` is meaningless then")
        verdict = shared_case_result(case["case"])
        if verdict:
            print(f"  vanilla:  {verdict}")
    dname = re.sub(r"_ZN14CRunningScript\d+(ProcessCommands\d+To\d+)Ei", r"CRunningScript::\1", case["func_name"])
    print(f"  original: case at 0x{case['case']:X} in {dname} (0x{case['func']:X})" + (f", ~{size} bytes until 0x{case['end']:X}" if size else ""))
    print("  note:     the case body starts after CollectParameters(N); outputs go through ScriptParams + StoreParameters(K);"
          " UpdateCompareFlag(x) means the handler returns bool")
    if args.decompile:
        sys.path.insert(0, str(REPO / ".claude" / "skills" / "find-reversing-targets" / "scripts"))
        from ida_rpc import call
        lo, hi = case["case"], case["end"] or case["case"] + 0x400
        tmp = REPO / ".claude" / "cache" / "_case_pseudocode.py"
        tmp.write_text(f"import os\nos.environ.update({{'CASE_FUNC': '{case['func']:#x}', 'CASE_LO': '{lo:#x}', 'CASE_HI': '{hi:#x}'}})\n"
                       + (HERE / "ida_case_pseudocode.py").read_text(encoding="utf-8"), encoding="utf-8")
        r = call("py_exec_file", {"file_path": str(tmp)})
        print(f"\n--- Hex-Rays pseudocode of the case [0x{lo:X}, 0x{hi:X}) ---")
        print(r.get("stdout", "").rstrip() or r.get("stderr", ""))
    if args.dump:
        sys.path.insert(0, str(REPO / ".claude" / "skills" / "validate-against-binary" / "scripts"))
        import asm_validate as av
        import types
        pe = av.PEImage(av.resolve_exe(REPO, None))
        rep = av.Report(False)
        opts = types.SimpleNamespace(size=(case["end"] - case["case"]) if case["end"] else 0x200, dump=True, no_check=True,
                                     cpp_file=None, cpp_func=None, helper_depth=2, strict=False)
        av.validate(av.Repo(REPO), pe, case["case"], opts, rep)
        print("\n".join(rep.lines[2:] if len(rep.lines) > 2 else rep.lines))


def cmd_list(args, by_name, by_op, status, cases) -> None:
    from collections import Counter
    share = Counter(c["case"] for c in cases.values())
    rows = []
    for name, op in by_name.items():
        if op > 0x0A4E:
            continue
        st = status.get(name, ("UNREGISTERED", None, None))[0]
        if args.status and st.lower() != args.status.lower():
            continue
        if args.filter and args.filter.upper() not in name:
            continue
        if args.original and not original_kind(op, cases).startswith(args.original.replace("-", " ")):
            continue
        c = cases.get(op)
        shared = c is not None and share[c["case"]] > 1
        size = (c["end"] - c["case"]) if c and c["end"] and not shared else None
        if args.hide_shared and shared:
            continue
        rows.append((name, op, st, size, shared))
    key = (lambda r: (r[3] is None, r[3] or 0)) if args.sort == "size" else (lambda r: r[1])
    for name, op, st, size, shared in sorted(rows, key=key)[: args.top or None]:
        sz = f"~{size} B" if size else ("shared" if shared else "?")
        print(f"  0x{op:04X}  {st:13} {sz:>8}  {name}")
    print(f"\n{len(rows)} command(s)")


def original_kind(op: int, cases: dict[int, dict]) -> str:
    c = cases.get(op)
    if not c:
        return "no case"
    k = classify_case(c["case"])
    if k != "code":
        return {"continue": "no-op (OR_CONTINUE)", "interrupt": "not implemented (OR_INTERRUPT)"}[k]
    shared = sum(1 for x in cases.values() if x["case"] == c["case"])
    # big shared groups are almost always a dispatcher's default/cleanup path the classifier couldn't prove
    return "real code" if shared == 1 else ("real code shared" if shared < 8 else "default path?")


def cmd_stats(by_name, status, cases) -> None:
    from collections import Counter
    table = Counter()
    for n, op in by_name.items():
        if op <= 0x0A4E:
            table[(status.get(n, ("UNREGISTERED",))[0], original_kind(op, cases))] += 1
    print(f"vanilla commands (<= 0x0A4E): {sum(table.values())}\n")
    kinds = ["real code", "real code shared", "default path?", "no-op (OR_CONTINUE)", "not implemented (OR_INTERRUPT)", "no case"]
    short = ["real", "real/shared", "default?", "no-op", "not-impl", "no case"]
    print(f"  {'repo status':14}" + "".join(f"{k:>13}" for k in short))
    for st in ("HANDLER", "NOP", "UNIMPLEMENTED", "UNREGISTERED"):
        print(f"  {st:14}" + "".join(f"{table[(st, k)]:>13}" for k in kinds))
    print("\n  real = unique case code; real/shared = case shared by 2-7 opcodes (eg. VAR/LVAR variants);"
          " default? = shared by 8+ (likely the default path); no-op = returns OR_CONTINUE doing nothing;"
          " not-impl = returns OR_INTERRUPT (default path)")
    print("\nWork left = UNREGISTERED x real code (the original dispatcher still runs them);"
          " UNIMPLEMENTED x real code / NOP x real code deserve a look (may be mis-registered).")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("export")
    lp = sub.add_parser("locate")
    lp.add_argument("command")
    lp.add_argument("--decompile", action="store_true")
    lp.add_argument("--dump", action="store_true")
    li = sub.add_parser("list")
    li.add_argument("--status", choices=["handler", "nop", "unimplemented", "unregistered"])
    li.add_argument("--filter", default="")
    li.add_argument("--sort", choices=["opcode", "size"], default="opcode")
    li.add_argument("--top", type=int, default=0)
    li.add_argument("--hide-shared", action="store_true", help="hide opcodes whose case is shared with others (no-op paths)")
    li.add_argument("--original", choices=["real", "real-code-shared", "default", "no-op", "not-implemented", "no-case"],
                    help="filter by what the ORIGINAL does (real code / no-op / OR_INTERRUPT default / no case)")
    sub.add_parser("stats")
    args = ap.parse_args()

    if args.cmd == "export":
        return cmd_export()
    by_name, by_op = load_enum()
    status = load_repo_status()
    cases = load_cases()
    if args.cmd == "locate":
        cmd_locate(args, by_name, by_op, status, cases)
    elif args.cmd == "list":
        cmd_list(args, by_name, by_op, status, cases)
    else:
        cmd_stats(by_name, status, cases)


if __name__ == "__main__":
    main()
