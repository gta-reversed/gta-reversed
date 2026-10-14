"""
IDAPython script (runs INSIDE IDA) - read-only export of cross-references and enums, for catalogues.py.

Exports:
  * calls:   for every non-library function in the game region (0x401000-0x748000), the functions it calls directly
  * globals: for every named item in the data segments, how many instructions read/write it and from which functions
  * enums:   every local enum with its members

Run it through the ida-pro-mcp server (no IDA UI needed):
    python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export-xrefs

Output (default): <repo>/.claude/cache/xrefs_export.json  (gitignored)
"""
import json
import os

import ida_funcs
import ida_nalt
import ida_name
import ida_segment
import ida_typeinf
import ida_xref
import idautils

OUT = os.environ.get("GTASA_XREFS_EXPORT") or os.path.join(os.path.dirname(ida_nalt.get_input_file_path() or "."), "xrefs_export.json")
GAME_LO, GAME_HI = 0x401000, 0x748000
DATA_SEGS = (".data", ".bss", "_data", "DATA")
MAX_FUNCS_PER_GLOBAL = 12

calls = {}
for ea in idautils.Functions(GAME_LO, GAME_HI):
    f = ida_funcs.get_func(ea)
    if not f or f.flags & (ida_funcs.FUNC_LIB | ida_funcs.FUNC_THUNK):
        continue
    out = []
    for head in idautils.FuncItems(ea):
        for x in idautils.XrefsFrom(head, ida_xref.XREF_FAR):
            if x.type in (ida_xref.fl_CN, ida_xref.fl_CF):
                g = ida_funcs.get_func(x.to)
                if g and g.start_ea == x.to and x.to not in out:
                    out.append(x.to)
    calls[ea] = out

globals_ = []
for i in range(ida_segment.get_segm_qty()):
    seg = ida_segment.getnseg(i)
    if ida_segment.get_segm_name(seg) not in DATA_SEGS:
        continue
    for ea, name in idautils.Names():
        if not (seg.start_ea <= ea < seg.end_ea):
            continue
        reads = writes = 0
        funcs = []
        for x in idautils.XrefsTo(ea, 0):
            if x.type == ida_xref.dr_W:
                writes += 1
            elif x.type in (ida_xref.dr_R, ida_xref.dr_O):
                reads += 1
            else:
                continue
            g = ida_funcs.get_func(x.frm)
            if g and g.start_ea not in funcs:
                funcs.append(g.start_ea)
        globals_.append({"ea": ea, "name": name, "dname": ida_name.demangle_name(name, ida_name.MNG_SHORT_FORM) or name,
                         "reads": reads, "writes": writes, "nfuncs": len(funcs), "funcs": funcs[:MAX_FUNCS_PER_GLOBAL]})

enums = []
til = ida_typeinf.get_idati()
for ordinal in range(1, ida_typeinf.get_ordinal_limit(til)):
    tif = ida_typeinf.tinfo_t()
    if not tif.get_numbered_type(til, ordinal) or not tif.is_enum():
        continue
    edt = ida_typeinf.enum_type_data_t()
    if not tif.get_enum_details(edt):
        continue
    enums.append({"name": tif.get_type_name() or f"enum_{ordinal}", "size": tif.get_size(),
                  "members": [[m.name, m.value] for m in edt]})

with open(OUT, "w", encoding="utf-8") as fp:
    json.dump({"calls": {f"{k:#x}": [f"{v:#x}" for v in vs] for k, vs in calls.items()},
               "globals": globals_, "enums": enums}, fp)
print(f"wrote {OUT}: {len(calls)} functions, {len(globals_)} globals, {len(enums)} enums")
