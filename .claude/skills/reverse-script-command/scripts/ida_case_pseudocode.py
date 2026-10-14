"""
IDAPython (runs INSIDE IDA, read-only): print the Hex-Rays pseudocode lines of `func` whose ctree items lie in
[lo, hi) - ie. one `case` of a huge switch. Parameters come from env vars CASE_FUNC, CASE_LO, CASE_HI (hex).
Used by script_cmd.py `locate --decompile` (the MCP `decompile` tool truncates very large functions).
"""
import os

import ida_hexrays
import ida_lines

FUNC = int(os.environ["CASE_FUNC"], 16)
LO = int(os.environ["CASE_LO"], 16)
HI = int(os.environ["CASE_HI"], 16)

cfunc = ida_hexrays.decompile(FUNC)
pseudo = cfunc.get_pseudocode()
rows = set()


class Collector(ida_hexrays.ctree_visitor_t):
    def __init__(self):
        ida_hexrays.ctree_visitor_t.__init__(self, ida_hexrays.CV_FAST)

    def _add(self, item):
        if LO <= item.ea < HI:
            coords = cfunc.find_item_coords(item)
            if coords:
                rows.add(coords[1] if isinstance(coords, tuple) else coords)

    def visit_insn(self, i):
        self._add(i)
        return 0

    def visit_expr(self, e):
        self._add(e)
        return 0


Collector().apply_to(cfunc.body, None)
if rows:
    first, last = min(rows), max(rows)
    for y in range(first, last + 1):  # contiguous block, so braces/labels in between are kept
        print(ida_lines.tag_remove(pseudo[y].line))
else:
    print(f"(no pseudocode items in [0x{LO:X}, 0x{HI:X}))")
