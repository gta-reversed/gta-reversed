"""
IDAPython (runs INSIDE IDA, read-only): export every struct/union with its size and members (offset, size, name, type).
Run through: python .claude/skills/find-reversing-targets/scripts/struct_check.py --export
Output: env GTASA_STRUCTS_EXPORT (default structs_export.json)
"""
import json
import os

import ida_typeinf

OUT = os.environ.get("GTASA_STRUCTS_EXPORT", "structs_export.json")
til = ida_typeinf.get_idati()
result = {}
for ordinal in range(1, ida_typeinf.get_ordinal_limit(til)):
    tif = ida_typeinf.tinfo_t()
    if not tif.get_numbered_type(til, ordinal) or not (tif.is_struct() or tif.is_union()):
        continue
    name = tif.get_type_name()
    if not name:
        continue
    udt = ida_typeinf.udt_type_data_t()
    if not tif.get_udt_details(udt):
        continue
    members = []
    for m in udt:
        members.append({"off": m.offset // 8, "size": m.size // 8, "name": m.name, "type": m.type._print()})
    result[name] = {"size": tif.get_size(), "union": tif.is_union(), "members": members}

with open(OUT, "w", encoding="utf-8") as fp:
    json.dump(result, fp)
print(f"wrote {OUT}: {len(result)} structs/unions")
