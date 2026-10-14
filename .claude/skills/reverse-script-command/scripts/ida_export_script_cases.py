"""
IDAPython (runs INSIDE IDA, read-only): map every SCM opcode to the address of its `case` in the original
`CRunningScript::ProcessCommands<N>To<N+99>` dispatchers.

Run through the MCP server:
  python .claude/skills/reverse-script-command/scripts/script_cmd.py export

Output: <repo>/.claude/cache/script_cases.json  ->  {"opcode": {"func": ea, "func_name": str, "case": ea, "end": ea|None}}
`end` is the next case/target address inside the same dispatcher (approximate end of the case body).
"""
import json
import os

import ida_funcs
import ida_nalt
import ida_xref
import idaapi
import idautils

OUT = os.environ.get("GTASA_SCRIPT_CASES_EXPORT", "script_cases.json")

result = {}
dispatchers = []
for ea in idautils.Functions():
    name = ida_funcs.get_func_name(ea) or ""
    if "ProcessCommands" not in name:
        continue
    dispatchers.append((ea, name))
    func = ida_funcs.get_func(ea)
    import re
    m = re.search(r"ProcessCommands(\d+)To(\d+)", name)
    base = int(m.group(1)) if m else 0
    # Collect every switch; the dispatcher's main switch is the one with the most cases (others are nested switches
    # inside command bodies, e.g. in 1500To1599)
    switches = []
    targets_all = set()
    for head in idautils.FuncItems(ea):
        si = ida_nalt.get_switch_info(head)
        if not si:
            continue
        res = idaapi.calc_switch_cases(head, si)
        if not res:
            continue
        cases = {}
        for idx in range(len(res.cases)):
            target = res.targets[idx]
            targets_all.add(target)
            for value in res.cases[idx]:
                cases[int(value)] = target
        switches.append(cases)
    if not switches:
        continue
    main = max(switches, key=len)
    ordered = sorted(targets_all | {func.end_ea})
    for value, target in main.items():
        opcode = value if base <= value <= base + 99 else base + value  # values may be relative to the base
        if not (base <= opcode <= base + 99):
            continue
        nxt = next((t for t in ordered if t > target), None)
        result[str(opcode)] = {"func": ea, "func_name": name, "case": target, "end": nxt}

os.makedirs(os.path.dirname(OUT) or ".", exist_ok=True)
with open(OUT, "w", encoding="utf-8") as fp:
    json.dump({"dispatchers": dispatchers, "cases": result}, fp, indent=0)
print(f"wrote {OUT}: {len(dispatchers)} dispatchers, {len(result)} opcode cases")
