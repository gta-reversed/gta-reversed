"""
IDAPython script (runs INSIDE IDA) - read-only export of the GTA:SA IDB to JSON.

Exports every function (address, size, name, demangled name, library/thunk flags, prototype), every named data item
outside code (address, name, segment, size, type), the segment layout and the local type catalog.

Run it:
  * from IDA: File -> Script file... (writes next to the IDB unless GTASA_IDB_EXPORT is set), or
  * through the ida-pro-mcp server (no IDA UI needed):
      python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export

Output (default): <repo>/.claude/cache/idb_export.json  (gitignored - consumed by idb_coverage.py)
"""
import json
import os

import ida_bytes
import ida_funcs
import ida_nalt
import ida_name
import ida_segment
import ida_typeinf
import idautils

OUT = os.environ.get("GTASA_IDB_EXPORT") or os.path.join(os.path.dirname(ida_nalt.get_input_file_path() or "."), "idb_export.json")


def demangle(n):
    d = ida_name.demangle_name(n, ida_name.MNG_SHORT_FORM) if n else None
    return d or n


def type_of(ea, name=""):
    tif = ida_typeinf.tinfo_t()
    return tif._print(name, ida_typeinf.PRTYPE_1LINE) if ida_nalt.get_tinfo(tif, ea) else None


funcs = []
for ea in idautils.Functions():
    f = ida_funcs.get_func(ea)
    n = ida_funcs.get_func_name(ea)
    funcs.append({
        "ea": ea, "size": f.end_ea - f.start_ea, "name": n, "dname": demangle(n),
        "lib": bool(f.flags & ida_funcs.FUNC_LIB), "thunk": bool(f.flags & ida_funcs.FUNC_THUNK),
        "proto": type_of(ea, n),
    })

names = []
for ea, n in idautils.Names():
    seg = ida_segment.getseg(ea)
    sname = ida_segment.get_segm_name(seg) if seg else None
    if sname in (".text", "_rwcseg"):
        continue
    names.append({"ea": ea, "name": n, "dname": demangle(n), "seg": sname, "size": ida_bytes.get_item_size(ea), "type": type_of(ea)})

segments = []
for s in idautils.Segments():
    seg = ida_segment.getseg(s)
    segments.append({"name": ida_segment.get_segm_name(seg), "start": seg.start_ea, "end": seg.end_ea, "perm": seg.perm})

til = ida_typeinf.get_idati()
types = []
for ordinal in range(1, ida_typeinf.get_ordinal_limit(til)):
    tif = ida_typeinf.tinfo_t()
    if not tif.get_numbered_type(til, ordinal) or not tif.get_type_name():
        continue
    kind = ("struct" if tif.is_struct() else "union" if tif.is_union() else "enum" if tif.is_enum()
            else "typedef" if tif.is_typedef() else "func" if tif.is_func() else "other")
    types.append({"name": tif.get_type_name(), "kind": kind, "size": tif.get_size() if kind in ("struct", "union", "enum") else None})

os.makedirs(os.path.dirname(OUT) or ".", exist_ok=True)
with open(OUT, "w", encoding="utf-8") as fp:
    json.dump({"input": ida_nalt.get_input_file_path(), "funcs": funcs, "names": names, "segments": segments, "types": types}, fp)
print(f"wrote {OUT}: {len(funcs)} functions, {len(names)} data names, {len(types)} types")
