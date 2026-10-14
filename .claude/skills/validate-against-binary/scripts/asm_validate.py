#!/usr/bin/env python3
"""
Static validation of a reversed function against the ORIGINAL machine code in gta_sa.exe (1.0 US compact).

It disassembles the original function straight from the exe (no IDA needed), extracts its "fingerprint"
and checks it against our C++ implementation in the repo:

  * callees       - every `call <addr>` / tail-`jmp <addr>` is mapped to a repo symbol (RH installs,
                    `// 0xADDR` comments) and must be referenced by our body - directly or through a helper
                    our body calls (helpers are followed 2 levels deep)
  * globals       - every absolute data reference is mapped to a `StaticRef` (or the nearest one + offset)
                    and must be referenced (directly, through an accessor like `GetTimeStep()`, or a helper)
  * float consts  - every float/double loaded from .rdata (and float immediates pushed as args) must
                    appear in our code BIT-EXACT as float32: literals, named constants (PI...), `a * b`, `x / c`
                    (compiled as `x * (1/c)`). Near-misses are reported as PRECISION MISMATCH.
  * strings       - every string literal referenced should appear in our code
  * virtual calls - listed with their vtable slot (informational)
  * extra calls   - hooked functions our code calls but the original doesn't (inlined? wrong?) - informational

`--dump` prints an ANNOTATED DISASSEMBLY (callee names, globals, float values, strings resolved from the repo) -
handy for reversing without IDA.

Usage (repo root):
  python .claude/skills/validate-against-binary/scripts/asm_validate.py CGeneral::LimitAngle
  python .claude/skills/validate-against-binary/scripts/asm_validate.py 0x53CB00 --dump
  python .claude/skills/validate-against-binary/scripts/asm_validate.py CFoo::Bar --cpp-file source/game_sa/Foo.cpp --cpp-func Bar
  python .claude/skills/validate-against-binary/scripts/asm_validate.py --file source/game_sa/Foo.cpp   # every reversed hook in a file

The exe path comes from --exe, else env var GTASA_EXE, else `.claude/gtasa_exe_path.txt` (one line).
Requires: capstone (`pip install capstone`). Read-only: never modifies the exe.

Severity: [MISSING] = likely wrong (exit 1) | [CHECK] = verify manually (exit 1 only with --strict) | [INFO] = context.
Exit code: 0 = no findings, 1 = findings, 2 = usage/environment error.
A clean result is necessary, not sufficient: still compare the control flow and run a NOTSA_DIFFTEST.
"""
from __future__ import annotations

import argparse
import os
import re
import struct
import sys
from bisect import bisect_right
from collections import defaultdict
from dataclasses import dataclass, field
from functools import lru_cache
from pathlib import Path

try:
    import capstone
    from capstone import x86
except ImportError:  # pragma: no cover - only disassembling needs it; the repo index/PE reader are used without it
    capstone = x86 = None


def require_capstone() -> None:
    if capstone is None:
        sys.stderr.write("error: capstone is required: pip install capstone\n")
        sys.exit(2)

EXPECTED_COMPACT_SIZE = 5_189_632
CRT_START = 0x820000  # statically linked MSVC CRT/library code lives above this in gta_sa.exe (rand, _ftol, memcpy, ...)

# Constants the compiler materialises on its own (unsigned -> float conversion, rounding helpers, ...)
COMPILER_FLOAT_CONSTS = {4294967296.0, 2147483648.0, 18446744073709551616.0}

# Globals usually accessed through helpers instead of by name
GLOBAL_ACCESSOR_HINTS = {
    "ms_modelInfoPtrs": ["GetModelInfo", "GetColModel", "GetModelInfoUInt16", "ModelInfo", "GetVehicleModelInfo", "GetPedModelInfo"],
    "ms_vehiclePool": ["GetVehiclePool"], "ms_pPedPool": ["GetPedPool"], "ms_pObjectPool": ["GetObjectPool"],
    "ms_pBuildingPool": ["GetBuildingPool"], "ms_pDummyPool": ["GetDummyPool"], "ms_pTaskPool": ["GetTaskPool"],
    "Players": ["FindPlayerPed", "FindPlayerInfo", "FindPlayerVehicle", "FindPlayerWanted", "FindPlayerCoors", "FindPlayerEntity", "FindPlayerSpeed"],
    "PlayerInFocus": ["FindPlayerPed", "FindPlayerInfo", "FindPlayerVehicle", "FindPlayerWanted", "FindPlayerCoors", "FindPlayerEntity", "FindPlayerSpeed"],
    "currArea": ["CanSeeOutSideFromCurrArea", "GetCurrArea"],
    # CTimer variables are private, accessed through getters
    "m_snTimeInMilliseconds": ["GetTimeInMS", "GetTimeInMSPauseMode"], "m_snPreviousTimeInMilliseconds": ["GetPreviousTimeInMS"],
    "m_snTimeInMillisecondsNonClipped": ["GetTimeInMSNonClipped"], "m_snTimeInMillisecondsPauseMode": ["GetTimeInMSPauseMode"],
    "ms_fTimeStep": ["GetTimeStep", "GetTimeStepInSeconds", "GetTimeStepInMS"], "ms_fTimeStepNonClipped": ["GetTimeStepNonClipped"],
    "m_UserPause": ["GetIsUserPaused"], "m_CodePause": ["GetIsCodePaused"], "ms_fTimeScale": ["GetTimeScale"],
}

# --------------------------------------------------------------------------------------------------
# PE
# --------------------------------------------------------------------------------------------------


@dataclass
class Section:
    name: str
    va: int
    vsize: int
    raw_ptr: int
    raw_size: int
    characteristics: int

    @property
    def executable(self) -> bool:
        return bool(self.characteristics & 0x20000000)

    def contains(self, addr: int) -> bool:
        return self.va <= addr < self.va + max(self.vsize, self.raw_size)


class PEImage:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()
        d = self.data
        if d[:2] != b"MZ":
            raise ValueError("not a PE file")
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        if d[pe:pe + 4] != b"PE\0\0":
            raise ValueError("bad PE signature")
        coff = pe + 4
        nsec, = struct.unpack_from("<H", d, coff + 2)
        opt_size, = struct.unpack_from("<H", d, coff + 16)
        opt = coff + 20
        self.image_base, = struct.unpack_from("<I", d, opt + 28)
        imp_rva, _ = struct.unpack_from("<II", d, opt + 96 + 8)  # DataDirectory[1] = imports
        self.sections: list[Section] = []
        for i in range(nsec):
            o = opt + opt_size + i * 40
            name = d[o:o + 8].rstrip(b"\0").decode("latin-1")
            vsize, va, rsize, rptr = struct.unpack_from("<IIII", d, o + 8)
            ch, = struct.unpack_from("<I", d, o + 36)
            self.sections.append(Section(name, self.image_base + va, vsize, rptr, rsize, ch))
        self.imports: dict[int, str] = {}
        try:
            self._parse_imports(imp_rva)
        except Exception:
            pass

    def section_of(self, addr: int) -> Section | None:
        return next((s for s in self.sections if s.contains(addr)), None)

    def offset_of(self, addr: int) -> int | None:
        s = self.section_of(addr)
        if not s:
            return None
        off = addr - s.va
        return s.raw_ptr + off if off < s.raw_size else None

    def read(self, addr: int, size: int) -> bytes | None:
        o = self.offset_of(addr)
        return None if o is None else self.data[o:o + size]

    def read_u32(self, addr: int) -> int | None:
        b = self.read(addr, 4)
        return struct.unpack("<I", b)[0] if b and len(b) == 4 else None

    def is_code(self, addr: int) -> bool:
        s = self.section_of(addr)
        return bool(s and s.executable)

    def read_cstring(self, addr: int, maxlen: int = 256) -> str | None:
        b = self.read(addr, maxlen)
        if not b or b"\0" not in b:
            return None
        s = b.split(b"\0", 1)[0]
        if len(s) < 2 or any(c < 0x09 or (0x0D < c < 0x20) or c > 0x7E for c in s):
            return None
        return s.decode("latin-1")

    def looks_like_vtable(self, addr: int) -> bool:
        s = self.section_of(addr)
        if not s or s.name != ".rdata":
            return False
        return all((v := self.read_u32(addr + i * 4)) is not None and self.is_code(v) for i in range(2))

    def _rva_str(self, rva: int) -> str:
        o = self.offset_of(self.image_base + rva)
        return self.data[o:self.data.index(b"\0", o)].decode("latin-1") if o is not None else "?"

    def _parse_imports(self, imp_rva: int) -> None:
        if not imp_rva:
            return
        o = self.offset_of(self.image_base + imp_rva)
        while o is not None:
            oft, _, _, name_rva, ft = struct.unpack_from("<IIIII", self.data, o)
            if not name_rva:
                break
            dll = self._rva_str(name_rva)
            thunk_rva, i = oft or ft, 0
            while True:
                v = self.read_u32(self.image_base + thunk_rva + i * 4)
                if not v:
                    break
                fn = f"#{v & 0xFFFF}" if v & 0x80000000 else self._rva_str(v + 2)
                self.imports[self.image_base + ft + i * 4] = f"{dll}!{fn}"
                i += 1
            o += 20


# --------------------------------------------------------------------------------------------------
# Repo index & source cache
# --------------------------------------------------------------------------------------------------

ADDR_RE = r"0x[0-9A-Fa-f]{5,8}"
INSTALL_RE = re.compile(r"^\s*RH_Scoped(?P<kind>\w*?)Install\(\s*(?P<fn>[\w:~<>]+)\s*,(?P<rest>.*)$")
SCOPE_RE = re.compile(r"RH_Scoped(?:Virtual|Named)?(?:Class|Namespace)\(\s*(?P<name>[\w:]+)")
VTBL_SCOPE_RE = re.compile(r"RH_ScopedVirtualClass\(\s*(?P<name>[\w:]+)\s*,\s*(?P<addr>" + r"0x[0-9A-Fa-f]{5,8}" + r")")
STATICREF_RES = [
    re.compile(r"&\s*(?P<name>\w+)\s*=\s*(?:Scoped)?StaticRef\s*<[^;]*?(?P<addr>" + ADDR_RE + r")"),
    re.compile(r"(?P<name>\w+)\s*=\s*(?:Scoped)?StaticRef\s*<[^>]*>\s*\(\s*(?P<addr>" + ADDR_RE + r")"),
]
TOPLEVEL_FN_RE = re.compile(r"^(?![\s#/]|return\b|if\b|else\b)[\w\s\*&:<>,]*?\b(?:\w+::)?(?P<fn>[A-Za-z_]\w*)\s*\([^;]*$")
FORWARD_RE = re.compile(r"(?:\)\s*(?P<addr>" + ADDR_RE + r")\s*\)\s*\(|plugin::Call\w*<[^>]*?\b(?P<addr2>" + ADDR_RE + r"))")
DECL_ADDR_RE = re.compile(r"^[\w\s\*&:<>,]*?\b(?P<fn>[A-Za-z_]\w*)\s*\([^;{}]*\)\s*(?:const\s*)?;\s*//\s*(?P<addr>" + ADDR_RE + r")\b")
ADDR_COMMENT_RE = re.compile(r"^\s*//\s*(?P<addr>" + ADDR_RE + r")\b")
FUNC_NAME_RE = re.compile(r"(?:(?P<cls>[A-Za-z_]\w*)::)?(?P<fn>~?[A-Za-z_]\w*)\s*\(")
CONSTEXPR_FLOAT_RE = re.compile(r"constexpr\s+(?:static\s+)?(?:inline\s+)?(?:float|double|auto)\s+(?P<name>[A-Z_][A-Z0-9_]*)\s*=\s*(?P<val>[^;]+);")
SAFE_EXPR_RE = re.compile(r"^[\d\s.eE+\-*/()]+$")
CPP_KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "static_cast", "reinterpret_cast", "const_cast",
                "dynamic_cast", "assert", "decltype", "alignof", "catch", "new", "delete", "case", "else", "do"}


def f32(v: float) -> float:
    """Round to float32 (what the game actually stores)."""
    try:
        return struct.unpack("<f", struct.pack("<f", v))[0]
    except (OverflowError, struct.error):
        return v


def eval_const_expr(expr: str, named: dict[str, float]) -> float | None:
    """Evaluate a simple constant expression like `PI / 2.0f` or `0.5f * PI` (float32 rounding)."""
    e = re.sub(r"(?<=[\d.])[fF]\b", "", expr.strip())
    e = re.sub(r"\b[A-Za-z_]\w*\b", lambda m: repr(named[m.group(0)]) if m.group(0) in named else "@", e)
    if "@" in e or not SAFE_EXPR_RE.match(e):
        return None
    try:
        return f32(float(eval(e, {"__builtins__": {}}, {})))  # only digits and operators reach this point
    except Exception:
        return None


def strip_comments(code: str) -> str:
    code = re.sub(r"/\*.*?\*/", lambda m: "\n" * m.group(0).count("\n"), code, flags=re.S)
    return re.sub(r"//[^\n]*", "", code)


@dataclass
class Symbol:
    name: str   # qualified when known (Class::Fn)
    short: str  # last component, searched for in C++ bodies
    where: str  # file:line
    kind: str   # function | global
    is_global_fn: bool = False


@dataclass
class Definition:
    path: str
    line: int
    header: str  # text from the line start up to the opening brace
    body: str    # comment-stripped, from the line start to the closing brace


def load_idb_names(root: Path) -> tuple[dict[int, str], dict[int, str]]:
    """Optional: function/data names exported from the IDB (`ida_rpc.py export`). Used only to label things the repo lacks."""
    p = root / ".claude" / "cache" / "idb_export.json"
    if not p.exists():
        return {}, {}
    try:
        import json
        exp = json.loads(p.read_text(encoding="utf-8"))
        funcs = {f["ea"]: (f["dname"] or f["name"]).split("(")[0] for f in exp.get("funcs", [])}
        names = {n["ea"]: n["dname"] or n["name"] for n in exp.get("names", [])}
        return funcs, names
    except (OSError, ValueError, KeyError):
        return {}, {}


class Repo:
    def __init__(self, root: Path) -> None:
        self.root = root
        self.funcs: dict[int, list[Symbol]] = defaultdict(list)
        self.globals_: dict[int, Symbol] = {}
        self.named_consts: dict[str, float] = {}
        self.name_to_addrs: dict[str, set[int]] = defaultdict(set)
        self.vtables: dict[int, str] = {}          # GTA vtable address -> class
        self.non_overriding: set[int] = set()      # `.Overrides = false` virtual hooks (implemented by a base class)
        self.files: dict[str, str] = {}        # rel path -> raw text
        self.stripped: dict[str, str] = {}     # rel path -> comment-stripped text
        self.paths_by_stem: dict[str, list[str]] = defaultdict(list)
        self._sorted_globals: list[int] = []
        self._def_cache: dict[tuple[str | None, str], list[Definition]] = {}
        self._build()
        self.idb_funcs, self.idb_names = load_idb_names(root)

    def idb_name(self, addr: int) -> str | None:
        """Name from the IDA database export (.claude/cache/idb_export.json), if available."""
        n = self.idb_funcs.get(addr) or self.idb_names.get(addr)
        return n if n and not re.match(r"^_?(sub|unk|byte|word|dword|flt|dbl|off|qword|loc)_[0-9A-F]+$", n) else None

    # ---- indexing ----
    def _build(self) -> None:
        pending: list[tuple[str, str]] = []
        for path in sorted((self.root / "source").rglob("*")):
            if path.suffix not in (".cpp", ".h", ".hpp"):
                continue
            try:
                text = path.read_text(encoding="utf-8", errors="replace")
            except OSError:
                continue
            rel = path.relative_to(self.root).as_posix()
            self.files[rel] = text
            self.stripped[rel] = strip_comments(text)
            self.paths_by_stem[path.stem].append(rel)
            lines = text.splitlines()
            scope = None
            scope_stack: list[tuple[int, str]] = []  # (brace depth, scope) - nested `{ RH_ScopedClass(X); ... }` blocks
            depth = 0
            current_fn = None
            for n, line in enumerate(lines, 1):
                if line.lstrip().startswith("//") and "RH_Scoped" in line:
                    continue
                s = SCOPE_RE.search(line)
                if s:
                    scope_stack.append((depth, s.group("name")))
                code_line = re.sub(r"//.*|\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])*'", "", line)
                depth += code_line.count("{") - code_line.count("}")
                while scope_stack and scope_stack[-1][0] > depth:
                    scope_stack.pop()
                scope = scope_stack[-1][1] if scope_stack else None
                v = VTBL_SCOPE_RE.search(line)
                if v:
                    self.vtables[int(v.group("addr"), 16)] = v.group("name")
                m = INSTALL_RE.match(line)
                if m:
                    a = re.search(ADDR_RE, m.group("rest"))
                    if a:
                        fn = m.group("fn")
                        is_global = "Global" in m.group("kind")
                        short = fn.split("::")[-1].split("<")[0]
                        if fn in ("Constructor", "Destructor") and scope:
                            short = scope.split("::")[-1] if fn == "Constructor" else "~" + scope.split("::")[-1]
                        q = fn if ("::" in fn or is_global or not scope) else f"{scope}::{fn}"
                        addr = int(a.group(0), 16)
                        self.funcs[addr].append(Symbol(q, short, f"{rel}:{n}", "function", is_global))
                        if re.search(r"\.Overrides\s*=\s*false", m.group("rest")):
                            self.non_overriding.add(addr)
                        self.name_to_addrs[short].add(addr)
                for rx in STATICREF_RES:
                    for g in rx.finditer(line):
                        addr = int(g.group("addr"), 16)
                        self.globals_.setdefault(addr, Symbol(g.group("name"), g.group("name"), f"{rel}:{n}", "global"))
                c = ADDR_COMMENT_RE.match(line)
                if c and n < len(lines):
                    nxt = lines[n]
                    fm = FUNC_NAME_RE.search(nxt)
                    if fm and not nxt.strip().startswith(("//", "RH_", "return", "if", "for", "while", "#")):
                        addr = int(c.group("addr"), 16)
                        if not any(sym.short == fm.group("fn") for sym in self.funcs.get(addr, [])):
                            q = f"{fm.group('cls')}::{fm.group('fn')}" if fm.group("cls") else fm.group("fn")
                            self.funcs[addr].append(Symbol(q, fm.group("fn"), f"{rel}:{n + 1}", "function"))
                # Un-hooked wrappers that forward to GTA: `return ((T(__cdecl*)(...))0x7EF450)(...)` / `plugin::Call<0x...>`
                fd = TOPLEVEL_FN_RE.match(line)
                if fd:
                    current_fn = fd.group("fn")
                w = FORWARD_RE.search(line)
                if w and current_fn:
                    addr = int(w.group("addr") or w.group("addr2"), 16)
                    if addr not in self.funcs:
                        self.funcs[addr].append(Symbol(current_fn, current_fn, f"{rel}:{n}", "function", True))
                # `RpClump* RpClumpRender(RpClump* clump); // 0x749B20` (RenderWare & other un-hooked wrappers)
                d = DECL_ADDR_RE.match(line)
                if d:
                    addr = int(d.group("addr"), 16)
                    if not any(sym.short == d.group("fn") for sym in self.funcs.get(addr, [])):
                        self.funcs[addr].append(Symbol(d.group("fn"), d.group("fn"), f"{rel}:{n}", "function", True))
                for k in CONSTEXPR_FLOAT_RE.finditer(line):
                    pending.append((k.group("name"), k.group("val")))
        for _ in range(4):  # named constants may reference each other (HALF_PI = PI / 2.0f)
            for name, expr in pending:
                if name not in self.named_consts:
                    v = eval_const_expr(expr, self.named_consts)
                    if v is not None:
                        self.named_consts[name] = v

    def func_names(self, addr: int) -> list[Symbol]:
        return self.funcs.get(addr, [])

    def nearest_global(self, addr: int, max_dist: int = 0x4000) -> tuple[Symbol, int] | None:
        if not self._sorted_globals:
            self._sorted_globals = sorted(self.globals_)
        i = bisect_right(self._sorted_globals, addr) - 1
        if i < 0:
            return None
        base = self._sorted_globals[i]
        return (self.globals_[base], addr - base) if addr - base <= max_dist else None

    # ---- definitions ----
    def find_definitions(self, cls: str | None, fn: str, file_hint: str | None = None) -> list[Definition]:
        key = (cls, fn) if not file_hint else (f"{cls}@{file_hint}", fn)
        if key in self._def_cache:
            return self._def_cache[key]
        if cls:
            rx = re.compile(rf"\b{re.escape(cls)}::{re.escape(fn)}\s*\(")
        else:
            rx = re.compile(rf"(?<![\w.>])(?:\w+::)?{re.escape(fn)}\s*\(")
        files = [file_hint] if file_hint and file_hint in self.stripped else list(self.stripped)
        out: list[Definition] = []
        for rel in files:
            text = self.stripped[rel]
            if fn.lstrip("~") not in text:
                continue
            for m in rx.finditer(text):
                d = self._definition_at(rel, text, m)
                if d:
                    out.append(d)
        self._def_cache[key] = out
        return out

    @staticmethod
    def _definition_at(rel: str, text: str, m: re.Match) -> Definition | None:
        line_start = text.rfind("\n", 0, m.start()) + 1
        prefix = text[line_start:m.start()]
        # `prefix` is the return type etc. (`CTask* `, `const CVector& `, `std::vector<int> `) - reject call sites
        if re.search(r"\b(return|RH_\w+|if|while|for|new|case|else)\b|[=,(!|?+\-/]\s*$|&&\s*$", prefix) or "." in prefix or "->" in prefix:
            return None
        j, depth = m.end() - 1, 0
        while j < len(text):
            if text[j] == "(":
                depth += 1
            elif text[j] == ")":
                depth -= 1
                if depth == 0:
                    break
            j += 1
        k = j + 1
        while k < len(text) and text[k] not in "{;":
            if text[k] == "=" and text[k + 1:k + 2] != "=":  # `= default;`, `= 0;`, default args already consumed
                return None
            k += 1
        if k >= len(text) or text[k] == ";":
            return None
        between = text[j + 1:k]
        if re.search(r"[^\s\w:,(){}<>\[\]&*.\-]", between.replace("->", "")) and ":" not in between:
            return None
        depth, e = 0, k
        while e < len(text):
            if text[e] == "{":
                depth += 1
            elif text[e] == "}":
                depth -= 1
                if depth == 0:
                    break
            e += 1
        return Definition(rel, text.count("\n", 0, m.start()) + 1, text[line_start:k], text[line_start:e + 1])

    def preceding_lines(self, d: Definition, n: int = 3) -> str:
        lines = self.files[d.path].splitlines()
        return "\n".join(lines[max(0, d.line - 1 - n):d.line - 1])

    def pick_definition(self, defs: list[Definition], addr: int) -> Definition | None:
        """Prefer the definition annotated with `// 0xADDR` right above it (overloads, ctor vs wrapper)."""
        tag = re.compile(rf"0x{addr:X}\b", re.I)
        tagged = [d for d in defs if tag.search(self.preceding_lines(d))]
        real = [d for d in tagged if not re.search(r"\bplugin::Call", d.body)] or tagged
        return (real or defs or [None])[0]

    # ---- helper closure ----
    @lru_cache(maxsize=None)
    def included_stems(self, rel: str) -> frozenset[str]:
        """Stems of the headers #included by `rel` (and by its own header)."""
        stems: set[str] = set()
        p = Path(rel)
        for f in (rel, p.with_suffix(".h").as_posix(), p.with_suffix(".hpp").as_posix()):
            for m in re.finditer(r'#include\s*[<"]([^>"]+)[>"]', self.files.get(f, "")):
                stems.add(Path(m.group(1)).stem)
        return frozenset(stems)

    def called_names(self, body: str) -> set[str]:
        return {name for _, name in self.called_refs(body)}

    def called_refs(self, body: str) -> set[tuple[str | None, str]]:
        """Functions referenced by `body`: calls (`Fn(`, `Cls::Fn(`, `obj.Fn(`, `p->Fn(`) and member pointers (`&Cls::Fn`)."""
        refs: set[tuple[str | None, str]] = set()
        for m in re.finditer(r"(?<![\w.>])(?:(\w+)::)?([A-Za-z_]\w*)\s*(?:<[^<>;(){}]*>)?\s*\(", body):
            if m.group(2) not in CPP_KEYWORDS:
                refs.add((m.group(1), m.group(2)))
        for m in re.finditer(r"(?:\.|->)\s*([A-Za-z_]\w*)\s*(?:<[^<>;(){}]*>)?\s*\(", body):
            refs.add((None, m.group(1)))
        for m in re.finditer(r"&\s*(\w+)::([A-Za-z_]\w*)\b(?!\s*\()", body):  # member function pointers (rng::for_each(r, &Cls::Fn))
            refs.add((m.group(1), m.group(2)))
        return refs

    def helper_text(self, body: str, ours: Definition | None = None, depth: int = 2, max_defs_per_name: int = 6) -> dict[str, str]:
        """Bodies of the functions `body` references, recursively (helpers the original may have inlined). -> {name: text}"""
        out: dict[str, str] = {}
        frontier: list[tuple[str, set[str]]] = [(body, {ours.path} if ours else set())]  # (text, files it came from)
        seen: set[tuple[str | None, str]] = set()
        for _ in range(depth):
            nxt = []
            for b, origin in frontier:
                for ref in self.called_refs(b) - seen:
                    seen.add(ref)
                    cls, name = ref
                    defs = self.find_definitions(cls, name) if cls else []
                    if not defs:  # namespace member / unqualified / method call: any definition with that name,
                        # preferring the files the referencing code lives in (namespaces: `RenderBuffer::Render`)
                        # then files whose header is #included by them (`g_furnitureMan.Init()` -> FurnitureManager_c.cpp)
                        related = {stem for o in origin for stem in self.included_stems(o)}
                        defs = sorted(self.find_definitions(None, name),
                                      key=lambda d: (d.path not in origin, Path(d.path).stem not in related))[:max_defs_per_name]
                    defs = [d for d in defs if not (ours and d.path == ours.path and d.line == ours.line)]
                    txt = "\n".join(d.body for d in defs if len(d.body) < 20000)
                    if txt:
                        key = f"{cls}::{name}" if cls else name
                        out[key] = txt
                        nxt.append((txt, {d.path for d in defs}))
            frontier = nxt
        return out


# --------------------------------------------------------------------------------------------------
# Disassembly
# --------------------------------------------------------------------------------------------------

# After `fcom X; fnstsw ax`: C0 (0x01) = st0 < X, C2 (0x04) = unordered, C3 (0x40) = st0 == X
FPU_JCC = {
    (0x41, "jne"): "<=", (0x41, "je"): ">",
    (0x01, "jne"): "<",  (0x01, "je"): ">=",
    (0x40, "jne"): "==", (0x40, "je"): "!=",
    (0x05, "jp"): ">=",  (0x05, "jnp"): "<",
    (0x44, "jp"): "!=",  (0x44, "jnp"): "==",
    (0x45, "jp"): "> or unordered", (0x45, "jnp"): "<= (ordered)",
}
FLOAT_MNEMONICS = re.compile(r"^f(ld|add|sub|subr|mul|div|divr|com|comp|ucom|ucomp)$")


@dataclass
class Fingerprint:
    start: int
    end: int
    insns: list = field(default_factory=list)
    calls: dict[int, int] = field(default_factory=lambda: defaultdict(int))
    tail_jumps: set[int] = field(default_factory=set)
    import_calls: dict[str, int] = field(default_factory=lambda: defaultdict(int))
    virtual_calls: list[tuple[int, str]] = field(default_factory=list)
    indirect_calls: list[int] = field(default_factory=list)
    globals_: dict[int, set[str]] = field(default_factory=lambda: defaultdict(set))
    vtables: set[int] = field(default_factory=set)
    floats: dict[float, set[int]] = field(default_factory=lambda: defaultdict(set))
    strings: dict[int, str] = field(default_factory=dict)
    immediates: set[int] = field(default_factory=set)
    annotations: dict[int, list[str]] = field(default_factory=lambda: defaultdict(list))


def looks_like_float_imm(v: int) -> float | None:
    """Heuristic: a 32-bit immediate that is a 'nice' float (eg. 0x3F800000 = 1.0)."""
    if v in (0, 0xFFFFFFFF) or (v & 0x7FFFFFFF) < 0x30000000:
        return None
    f = struct.unpack("<f", struct.pack("<I", v & 0xFFFFFFFF))[0]
    if f != f or abs(f) > 1e7 or abs(f) < 1e-5:
        return None
    for digits in range(1, 7):
        if f32(float(f"{f:.{digits}g}")) == f:
            return f
    return None


MATH_CLASSES = {"CVector", "CVector2D", "CMatrix", "CMatrixLink", "CQuaternion", "CRect", "CRGBA", "CSimpleTransform",
                "CCompressedVector", "CMaths", "RwV3d", "RwMatrix", "CBox", "CSphere", "CColSphere", "CColBox"}
RAII_HELPERS = {"RegisterReference", "CleanUpOldReference", "SafeRegisterRef", "SafeCleanUpRef", "ChangeEntityReference",
                "RemoveRef", "AddRef", "operator_delete", "operator_new"}


def leaf_size(pe: PEImage, addr: int, max_insns: int = 40) -> int | None:
    """Number of instructions if `addr` is a small function without calls (inline helper), else None."""
    try:
        fp = disassemble(pe, addr, max_insns=max_insns + 1)
    except ValueError:
        return None
    if len(fp.insns) > max_insns or fp.calls or fp.import_calls or fp.virtual_calls:
        return None
    return len(fp.insns)


def classify_crt(pe: PEImage, addr: int) -> str:
    code = pe.read(addr, 32) or b""
    if b"\xdf\x7c\x24" in code[:24] or b"\xdf\x7d" in code[:24]:  # fistp qword ptr [esp+x] -> _ftol
        return "_ftol (float -> integer truncation, ie. a cast)"
    return "CRT/library function"


def ctor_dtor_vtable(pe: PEImage, addr: int) -> int | None:
    """If the function at `addr` stores a vtable into `[reg]` near its start, it's a ctor/dtor -> that vtable."""
    require_capstone()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    code = pe.read(addr, 0x60) or b""
    for insn in md.disasm(code, addr):
        if insn.mnemonic == "mov" and len(insn.operands) == 2:
            dst, src = insn.operands
            if dst.type == x86.X86_OP_MEM and dst.mem.disp == 0 and dst.mem.base != 0 and src.type == x86.X86_OP_IMM                and pe.looks_like_vtable(src.imm & 0xFFFFFFFF):
                return src.imm & 0xFFFFFFFF
        if insn.group(capstone.CS_GRP_RET):
            break
    return None


def disassemble(pe: PEImage, start: int, max_insns: int = 20000, forced_size: int | None = None) -> Fingerprint:
    require_capstone()
    sec = pe.section_of(start)
    if not sec or not sec.executable:
        raise ValueError(f"0x{start:X} is not in an executable section")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    off = pe.offset_of(start)
    code = pe.data[off:sec.raw_ptr + sec.raw_size]

    fp = Fingerprint(start, start)
    max_target = start
    fcmp: tuple[str, str] | None = None  # (mnemonic, operand) of the last x87 compare
    test_mask: int | None = None         # mask of the last `test ah, X` after `fnstsw ax`
    for insn in md.disasm(code, start):
        if forced_size is not None and insn.address >= start + forced_size:
            break
        if len(fp.insns) >= max_insns:
            break
        if insn.mnemonic == "int3" and insn.address > start and forced_size is None:
            break
        fp.insns.append(insn)
        ops = insn.operands
        is_call = insn.group(capstone.CS_GRP_CALL)
        is_jump = insn.group(capstone.CS_GRP_JUMP)
        is_ret = insn.group(capstone.CS_GRP_RET)

        # x87 compare decoding: fcom(p) X / fnstsw ax / test ah, MASK / jcc  ->  "st(0) OP X"
        if re.match(r"^fu?comp?p?$", insn.mnemonic):
            label = insn.op_str or "st(1)"
            mem = next((o for o in ops if o.type == x86.X86_OP_MEM and o.mem.base == 0), None)
            if mem is not None and (s := pe.section_of(mem.mem.disp & 0xFFFFFFFF)) and s.name == ".rdata" and mem.size in (4, 8):
                raw = pe.read(mem.mem.disp & 0xFFFFFFFF, mem.size)
                label = repr(struct.unpack("<f" if mem.size == 4 else "<d", raw)[0]) + ("f" if mem.size == 4 else "")
            fcmp, test_mask = (insn.mnemonic, label), None
        elif insn.mnemonic == "test" and insn.op_str.startswith("ah,") and fcmp:
            test_mask = int(insn.op_str.split(",")[1], 0)
        elif is_jump and fcmp and test_mask is not None and insn.mnemonic != "jmp":
            cond = FPU_JCC.get((test_mask, insn.mnemonic))
            if cond:
                fp.annotations[insn.address].append(f"jump if st(0) {cond} {fcmp[1]}" + (" (popped)" if "p" in fcmp[0][3:] else ""))
            fcmp = test_mask = None

        # Function extent: forward branches / jump tables
        if is_jump and ops:
            op = ops[0]
            if op.type == x86.X86_OP_IMM and op.imm > insn.address and (insn.mnemonic != "jmp" or op.imm <= insn.address + 0x2000):
                max_target = max(max_target, op.imm)
            elif op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index != 0 and op.mem.scale == 4:
                tbl = op.mem.disp & 0xFFFFFFFF
                for i in range(512):  # switch jump table
                    t = pe.read_u32(tbl + i * 4)
                    if t is None or not (start <= t < start + 0x10000):
                        break
                    max_target = max(max_target, t)
                fp.annotations[insn.address].append(f"switch jump table @ 0x{tbl:X}")

        # Calls
        if is_call and ops:
            op = ops[0]
            if op.type == x86.X86_OP_IMM:
                fp.calls[op.imm] += 1
            elif op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and op.mem.disp in pe.imports:
                fp.import_calls[pe.imports[op.mem.disp]] += 1
                fp.annotations[insn.address].append(pe.imports[op.mem.disp])
            elif op.type == x86.X86_OP_MEM and op.mem.base != 0 and op.mem.index == 0:
                slot = op.mem.disp
                fp.virtual_calls.append((insn.address, f"virtual call via [{insn.reg_name(op.mem.base)}+0x{slot:X}] (vtable slot {slot // 4})"))
                fp.annotations[insn.address].append(f"vtbl slot {slot // 4}")
            else:
                fp.indirect_calls.append(insn.address)

        # Data references
        for op in ops:
            if op.type == x86.X86_OP_MEM and op.mem.base == 0 and (op.mem.disp & 0xFFFFFFFF) >= pe.image_base:
                addr = op.mem.disp & 0xFFFFFFFF
                s = pe.section_of(addr)
                if not s or s.executable or addr in pe.imports or (is_jump and op.mem.index != 0):
                    continue
                if FLOAT_MNEMONICS.match(insn.mnemonic) and s.name == ".rdata" and op.size in (4, 8):
                    raw = pe.read(addr, op.size)
                    if raw:
                        val = struct.unpack("<f" if op.size == 4 else "<d", raw)[0]
                        fp.floats[val].add(insn.address)
                        fp.annotations[insn.address].append(f"= {val!r}" + ("f" if op.size == 4 else " (double)"))
                        continue
                fp.globals_[addr].add(f"{op.size}b" + ("[]" if op.mem.index else ""))
            elif op.type == x86.X86_OP_IMM and not is_call and not is_jump:
                v = op.imm & 0xFFFFFFFF
                s = pe.section_of(v)
                if s and not s.executable and v >= pe.image_base and insn.mnemonic in ("push", "mov", "lea", "cmp"):
                    st = pe.read_cstring(v)
                    if pe.looks_like_vtable(v):
                        fp.vtables.add(v)
                        fp.annotations[insn.address].append(f"vtable 0x{v:X}")
                    elif st is not None:
                        fp.strings[v] = st
                        fp.annotations[insn.address].append(f'"{st}"')
                    else:
                        fp.globals_[v].add("&")
                else:
                    fv = looks_like_float_imm(v) if insn.mnemonic in ("mov", "push") else None
                    if fv is not None:
                        fp.floats[fv].add(insn.address)
                        fp.annotations[insn.address].append(f"= {fv!r}f ?")
                    elif op.imm < 0x10000 or v > 0xFFFF0000:
                        fp.immediates.add(op.imm)

        fp.end = insn.address + insn.size
        if forced_size is None:
            if is_ret and insn.address >= max_target:
                break
            if insn.mnemonic == "jmp" and insn.address >= max_target:
                if ops and ops[0].type == x86.X86_OP_IMM and not (start <= ops[0].imm < insn.address):
                    fp.tail_jumps.add(ops[0].imm)
                    fp.calls[ops[0].imm] += 1
                break
    return fp


# --------------------------------------------------------------------------------------------------
# C++ side helpers
# --------------------------------------------------------------------------------------------------

CPP_FLOAT_LIT_RE = re.compile(r"(?<![\w.])(?P<num>(?:\d+\.\d*|\.\d+|\d+)(?:[eE][-+]?\d+)?)(?P<suf>[fF]?)(?![\w.])")
OPERAND = r"(?:\d+\.?\d*(?:[eE][-+]?\d+)?[fF]?|[A-Z_][A-Z0-9_]*)"


def cpp_float_values(code: str, named: dict[str, float]) -> set[float]:
    vals: list[float] = []
    for m in CPP_FLOAT_LIT_RE.finditer(code):
        try:
            vals.append(float(m.group("num")))
        except ValueError:
            pass
    for name, v in named.items():
        if name in code and re.search(rf"\b{name}\b", code):
            vals.append(v)
    for m in re.finditer(rf"(?=({OPERAND})\s*([/*])\s*({OPERAND}))", code):
        v = eval_const_expr(f"{m.group(1)} {m.group(2)} {m.group(3)}", named)
        if v is not None:
            vals.append(v)
    for m in re.finditer(rf"/\s*({OPERAND})", code):  # `x / c` is compiled as `x * (1/c)`
        v = eval_const_expr(m.group(1), named)
        if v:
            vals.append(1.0 / v)
    if re.search(r"\bDegreesToRadians\b|\bDEG_TO_RAD\b", code):
        vals.append(0.017453292519943295)
    if re.search(r"\bRadiansToDegrees\b", code):
        vals.append(57.29577951308232)
    vals += [-v for v in vals]
    return {f32(v) for v in vals}


def ulp_distance(a: float, b: float) -> int:
    def ordered(f: float) -> int:
        i = struct.unpack("<i", struct.pack("<f", f))[0]
        return -(2**31) - i if i < 0 else i
    return abs(ordered(a) - ordered(b))


def float_match(value: float, cpp_vals: set[float]) -> tuple[str, float | None]:
    """-> ("exact", v) | ("close", v) | ("none", None). Exact = same float32 bits."""
    target = f32(value)
    if target in cpp_vals:
        return "exact", target
    close = [v for v in cpp_vals if abs(v - target) <= max(abs(target), 1e-12) * 1e-4]
    if close:
        return "close", min(close, key=lambda v: abs(v - target))
    return "none", None


def accessor_names(var: str) -> list[str]:
    """`ms_fTimeStep` -> [TimeStep, GetTimeStep, ...] - globals are often accessed through getters."""
    core = re.sub(r"^(?:ms_|m_|s_|g_)", "", var)
    core = re.sub(r"^(?:f|n|b|p|sz|a|i|u|v|w|dw|uc|us)(?=[A-Z])", "", core)
    core = core[:1].upper() + core[1:]
    return [core, f"Get{core}", f"Set{core}", f"Is{core}"] + GLOBAL_ACCESSOR_HINTS.get(var, [])


# --------------------------------------------------------------------------------------------------
# Validation
# --------------------------------------------------------------------------------------------------


class Report:
    def __init__(self, strict: bool) -> None:
        self.strict = strict
        self.findings = 0
        self.checks = 0
        self.lines: list[str] = []

    def section(self, title: str) -> None:
        self.lines.append(f"\n### {title}")

    def ok(self, msg: str) -> None:
        self.lines.append(f"  [OK]      {msg}")

    def info(self, msg: str) -> None:
        self.lines.append(f"  [INFO]    {msg}")

    def missing(self, msg: str) -> None:
        self.findings += 1
        self.lines.append(f"  [MISSING] {msg}")

    def check(self, msg: str) -> None:
        self.checks += 1
        if self.strict:
            self.findings += 1
        self.lines.append(f"  [CHECK]   {msg}")


def locate_ours(repo: Repo, addr: int, syms: list[Symbol], cpp_file: str | None, cpp_func: str | None) -> Definition | None:
    if cpp_file or cpp_func:
        fn = cpp_func or (syms[0].short if syms else None)
        if not fn:
            return None
        cls = fn.split("::")[-2] if "::" in fn else None
        return repo.pick_definition(repo.find_definitions(cls, fn.split("::")[-1], cpp_file), addr)
    for s in syms:
        parts = s.name.split("::")
        fn, cls = s.short, (parts[-2] if len(parts) > 1 else None)
        if parts[-1] in ("Constructor", "Destructor") and cls:
            fn = cls if parts[-1] == "Constructor" else "~" + cls
        hint = s.where.rsplit(":", 1)[0]
        candidates = []
        if cls and not s.is_global_fn:
            candidates += repo.find_definitions(cls, fn)
            stem = Path(hint).with_suffix("").as_posix()
            for h in (stem + ".h", stem + ".hpp"):  # inline definitions inside the class body
                if h in repo.stripped:
                    candidates += repo.find_definitions(None, fn, h)
        else:
            candidates += repo.find_definitions(None, fn, hint) or repo.find_definitions(None, fn)
        if not candidates and cls:  # functions defined inside `namespace Cls { ... }` are unqualified
            candidates += repo.find_definitions(None, fn, hint)
        if not candidates:  # templates / inline definitions in an #included header (eg. CTaskComplexSeekEntity<T>)
            for stem in sorted(repo.included_stems(hint)):
                for h in repo.paths_by_stem.get(stem, []):
                    if h.endswith((".h", ".hpp")):
                        candidates += repo.find_definitions(None, fn, h)
        d = repo.pick_definition(candidates, addr)
        if d:
            return d
    return None


def is_defaulted(repo: Repo, syms: list[Symbol]) -> bool:
    """`~BoneNode_c() = default; // 0x616B80` - compiler generated, nothing to compare."""
    for s in syms:
        parts = s.name.split("::")
        cls = parts[-2] if len(parts) > 1 else None
        fn = s.short
        if parts[-1] in ("Constructor", "Destructor") and cls:
            fn = cls if parts[-1] == "Constructor" else "~" + cls
        stem = Path(s.where.rsplit(":", 1)[0]).with_suffix("").as_posix()
        for h in (stem + ".h", stem + ".hpp"):
            if re.search(rf"{re.escape(fn)}\s*\([^;{{]*\)\s*(?:noexcept\s*)?=\s*default\s*;", repo.stripped.get(h, "")):
                return True
    return False


def validate(repo: Repo, pe: PEImage, addr: int, args, rep: Report) -> None:
    syms = repo.func_names(addr)
    fp = disassemble(pe, addr, forced_size=args.size)
    title = syms[0].name if syms else f"sub_{addr:X}"
    rep.lines.append(f"## {title} @ 0x{addr:X}  ({len(fp.insns)} instructions, 0x{fp.end - fp.start:X} bytes, ends at 0x{fp.end:X})")
    if syms:
        rep.lines.append(f"   hooked at {syms[0].where}")

    def callee_label(a: int) -> str:
        ss = repo.func_names(a)
        if ss:
            return " | ".join(s.name for s in ss)
        idb = repo.idb_name(a)
        if a >= CRT_START:
            return f"{idb or classify_crt(pe, a)} sub_{a:X}"
        return f"{idb} [IDB, not in repo]" if idb else f"sub_{a:X} (not in repo)"

    def global_label(a: int) -> str:
        if a in repo.globals_:
            return repo.globals_[a].name
        if idb := repo.idb_name(a):
            return f"{idb} [IDB]"
        near = repo.nearest_global(a)
        return f"{near[0].name}+0x{near[1]:X}" if near else f"unk_{a:X}"

    if args.dump:
        rep.lines.append("")
        for insn in fp.insns:
            notes = list(fp.annotations.get(insn.address, []))
            for op in insn.operands:
                if (insn.group(capstone.CS_GRP_CALL) or insn.address == fp.end - insn.size) and op.type == x86.X86_OP_IMM and op.imm in fp.calls:
                    notes.append(("tail call -> " if op.imm in fp.tail_jumps else "") + callee_label(op.imm))
                elif op.type == x86.X86_OP_MEM and op.mem.base == 0 and (op.mem.disp & 0xFFFFFFFF) in fp.globals_:
                    notes.append(global_label(op.mem.disp & 0xFFFFFFFF))
                elif op.type == x86.X86_OP_IMM and (op.imm & 0xFFFFFFFF) in fp.globals_:
                    notes.append("&" + global_label(op.imm & 0xFFFFFFFF))
            note = ("  ; " + ", ".join(notes)) if notes else ""
            rep.lines.append(f"  {insn.address:08X}  {insn.mnemonic:<7} {insn.op_str}{note}")
    if args.no_check:
        return

    ours = locate_ours(repo, addr, syms, args.cpp_file, args.cpp_func)
    if not ours:
        if addr in repo.non_overriding:
            rep.info("`.Overrides = false` hook: our implementation is the base class' function - validate with --cpp-func Base::Fn")
        elif is_defaulted(repo, syms):
            rep.info("`= default` (compiler generated) - compare member destruction/initialisation order manually if needed")
        else:
            rep.missing("could not find our C++ implementation - pass --cpp-file/--cpp-func")
        return
    rep.lines.append(f"   ours at {ours.path}:{ours.line}")
    if re.search(rf"plugin::Call\w*<[^>]*0x{addr:X}\b", ours.body, re.I):
        rep.info("our implementation is still a plugin::Call stub to the original - nothing to validate")
        return

    body = ours.body
    own_names = {s.short for s in syms} | {s.short.lstrip("~") for s in syms}
    helpers = repo.helper_text(body, ours=ours, depth=args.helper_depth)

    def found(name: str) -> tuple[bool, str | None]:
        rx = re.compile(rf"\b{re.escape(name)}\b")
        if rx.search(body):
            return True, None
        for h, txt in helpers.items():
            if rx.search(txt):
                return True, h
        return False, None

    # Calls
    rep.section("Calls (original -> ours)")
    if not fp.calls and not fp.import_calls:
        rep.info("no direct calls")
    for target, count in sorted(fp.calls.items()):
        ss = repo.func_names(target)
        times = f" x{count}" if count > 1 else ""
        tail = " (tail call)" if target in fp.tail_jumps else ""
        if not ss:
            vt = ctor_dtor_vtable(pe, target) if target < CRT_START else None
            idb = repo.idb_name(target) or ""
            if target >= CRT_START:
                rep.check(f"0x{target:X}{times} {idb or classify_crt(pe, target)} - ours must do the same (std/C call, cast, ...)")
            elif "::~" in idb or (idb and idb.split("::")[-1] == idb.split("::")[0] and "::" in idb):
                rep.check(f"0x{target:X}{times} {idb} [IDB] - implicit constructor/destructor in C++ (local/temporary/member) - verify")
            elif vt:
                cls = repo.vtables.get(vt, f"class with vtable 0x{vt:X}")
                rep.check(f"0x{target:X}{times} constructor/destructor of {cls} - usually implicit in C++ (local/temporary object, base class); verify")
            else:
                small = leaf_size(pe, target)
                if small is not None:
                    rep.check(f"0x{target:X}{times}{tail} - small leaf helper ({small} insns) not in the repo: probably an inline "
                              f"operator/getter in ours (CVector ops, ctor of a POD...) - verify with --dump of 0x{target:X}")
                else:
                    idb = repo.idb_name(target)
                    rep.missing(f"0x{target:X}{times}{tail}" + (f" {idb} [IDB]" if idb else "") +
                                " - not in the repo; declare it (plugin::Call stub + hook) and call it, or reverse it")
            continue
        hits = [(s, *found(s.short)) for s in ss]
        hits += [(s, *found(s.short.rsplit("_", 1)[0])) for s in ss if "_" in s.short.strip("_")]  # `Fn_int32`-style hook wrappers
        hit = next(((s, via) for s, ok_, via in hits if ok_), None)
        if hit:
            s, via = hit
            rep.ok(f"0x{target:X} {s.name}{times}{tail}" + (f" (via {via})" if via else ""))
        elif ss[0].name.split("::")[0] in MATH_CLASSES:
            rep.check(f"0x{target:X} {ss[0].name}{times}{tail} - math helper not referenced by name: ours probably uses an operator/other helper - verify")
        elif ss[0].short in RAII_HELPERS:
            rep.check(f"0x{target:X} {ss[0].name}{times}{tail} - may be done implicitly (RAII member like notsa::EntityRef, destructor) - verify")
        else:
            rep.missing(f"0x{target:X} {ss[0].name}{times}{tail} - not called by our code (nor by its helpers): missing call or different name?")
    for imp, count in sorted(fp.import_calls.items()):
        fn = imp.split("!")[-1].lstrip("_")
        ok_, via = found(fn)
        (rep.ok if ok_ else rep.check)(f"import {imp} x{count}" + (f" (via {via})" if via else ""))
    for a, desc in fp.virtual_calls:
        rep.info(f"0x{a:X}: {desc}")
    for a in fp.indirect_calls:
        rep.info(f"0x{a:X}: indirect call (function pointer)")

    called_targets = set(fp.calls)
    extra = []
    for name in sorted(repo.called_names(body) - own_names):
        addrs = repo.name_to_addrs.get(name)
        if addrs and len(addrs) == 1 and not (addrs & called_targets):
            extra.append(name)
    if extra:
        rep.info("ours calls hooked function(s) the original doesn't call directly: " + ", ".join(extra) + " (fine if inlined in the original - verify)")

    # Globals
    rep.section("Globals")
    if not fp.globals_ and not fp.vtables:
        rep.info("no absolute data references")
    for a in sorted(fp.vtables):
        rep.info(f"0x{a:X} vtable (constructor/destructor or RTTI-less type check)")
    for a, widths in sorted(fp.globals_.items()):
        w = ", ".join(sorted(widths))
        sym = repo.globals_.get(a)
        # a global with its own name in the IDB is a separate variable, not a field of the nearest StaticRef
        near = None if sym or repo.idb_name(a) else repo.nearest_global(a)
        sym = sym or (near[0] if near else None)
        label = global_label(a)
        if not sym:
            sec = pe.section_of(a)
            if sec and sec.name == ".rdata":
                raw = pe.read(a, 16) or b""
                rep.check(f"0x{a:X} read-only data ({w}) [{raw.hex(' ')}] - constant table/GUID/struct: "
                          f"make sure ours uses the same values (copy as constexpr or StaticRef)")
            else:
                rep.missing(f"0x{a:X} {label} ({w}) - global without StaticRef in the repo: add one (name/type from IDA)")
            continue
        names = [sym.short] + accessor_names(sym.short)
        hit = next(((n, via) for n in names for ok_, via in [found(n)] if ok_), None)
        if hit:
            n, via = hit
            how = "" if n == sym.short else f" as {n}()"
            rep.ok(f"0x{a:X} {label} ({w}){how}" + (f" (via {via})" if via else ""))
        elif near:
            rep.check(f"0x{a:X} {label} ({w}) - member/element of `{sym.name}`; make sure ours accesses the same field")
        else:
            rep.check(f"0x{a:X} {label} ({w}) - not referenced by name or accessor (another helper?)")

    # Floats
    rep.section("Float constants")
    cpp_vals = cpp_float_values(body, repo.named_consts)
    helper_vals = cpp_float_values("\n".join(helpers.values()), repo.named_consts) if helpers else set()
    if not fp.floats:
        rep.info("none")
    for val, where in sorted(fp.floats.items()):
        at = ", ".join(f"0x{w:X}" for w in sorted(where)[:4])
        if val in COMPILER_FLOAT_CONSTS:
            rep.info(f"{val!r} (at {at}) - compiler-generated (unsigned -> float conversion)")
            continue
        kind, near = float_match(val, cpp_vals)
        if kind != "exact":
            hkind, hnear = float_match(val, helper_vals)
            if hkind == "exact":
                rep.ok(f"{val!r} (at {at}) (in a helper)")
                continue
        if kind == "exact":
            rep.ok(f"{val!r} (at {at})")
        elif kind == "close":
            ulps = ulp_distance(near, f32(val))
            msg = f"{val!r} (at {at}) - ours only has {near:.9g} ({ulps} ULP off)"
            if ulps <= 2:
                rep.check(msg + ": computed differently (eg. `x / c` vs `x * k`, double vs float literal) - results may differ in the last bit")
            else:
                rep.missing(msg + ": PRECISION MISMATCH (imprecise named constant like PI = 3.14159f? rounded literal?) - use the exact value from the binary")
        else:
            rep.check(f"{val!r} (at {at}) - not found as a literal/named constant (computed differently? wrong value?)")

    # Strings
    if fp.strings:
        rep.section("Strings")
        raw_all = repo.files[ours.path]
        for a, s in sorted(fp.strings.items()):
            if s in body or s in raw_all or any(s in t for t in helpers.values()):
                rep.ok(f'0x{a:X} "{s}"')
            else:
                rep.check(f'0x{a:X} "{s}" - not found (built with a format string? table elsewhere?)')

    imms = sorted(i for i in fp.immediates if abs(i) > 1 and abs(i) not in (4, 8, 0xC, 0x10))
    if imms:
        rep.section("Integer immediates (informational: enum values, sizes, flags, offsets)")
        rep.lines.append("  " + ", ".join(f"{i:#x}" if abs(i) > 9 else str(i) for i in imms[:60]))


def repo_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if (p / "source" / "InjectHooksMain.cpp").exists():
            return p
    sys.stderr.write("error: run from inside the gta-reversed repository\n")
    sys.exit(2)


def resolve_exe(root: Path, arg: str | None) -> Path:
    cand = arg or os.environ.get("GTASA_EXE")
    if not cand:
        cfg = root / ".claude" / "gtasa_exe_path.txt"
        if cfg.exists():
            cand = cfg.read_text(encoding="utf-8").strip()
    if not cand:
        sys.stderr.write("error: pass --exe, set GTASA_EXE or write the exe path into .claude/gtasa_exe_path.txt\n")
        sys.exit(2)
    p = Path(cand)
    if not p.exists():
        sys.stderr.write(f"error: exe not found: {p}\n")
        sys.exit(2)
    return p


def resolve_target(repo: Repo, target: str) -> list[int]:
    if re.fullmatch(r"(0x)?[0-9A-Fa-f]{5,8}", target):
        return [int(target, 16)]
    short = target.split("::")[-1]
    cls = target.split("::")[-2] if "::" in target else None
    return sorted({a for a, ss in repo.funcs.items() for s in ss
                   if (s.short == short or s.name.split("::")[-1] == short) and (cls is None or s.name.startswith(cls + "::") or f"::{cls}::" in s.name)})


def addresses_in_file(repo: Repo, rel: str) -> list[int]:
    out = []
    for line in repo.files.get(rel, "").splitlines():
        m = INSTALL_RE.match(line)
        if m and not re.search(r"\.Reversed\s*=\s*false", line):
            a = re.search(ADDR_RE, m.group("rest"))
            if a:
                out.append(int(a.group(0), 16))
    return out


def main() -> None:
    require_capstone()
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("target", nargs="?", help="GTA address (0x53CB00) or C++ name (CGeneral::LimitAngle)")
    ap.add_argument("--file", help="validate every reversed hook installed in this .cpp (summary mode)")
    ap.add_argument("--exe", help="path to gta_sa.exe (1.0 US compact, 5,189,632 bytes)")
    ap.add_argument("--cpp-file", help="file containing our implementation (default: auto-detect)")
    ap.add_argument("--cpp-func", help="function name (Class::Fn or Fn) of our implementation (default: from the hook)")
    ap.add_argument("--size", type=lambda s: int(s, 0), help="force the function size in bytes (if the extent heuristic fails)")
    ap.add_argument("--helper-depth", type=int, default=2, help="how deep to follow helper functions (default 2)")
    ap.add_argument("--dump", action="store_true", help="print annotated disassembly")
    ap.add_argument("--no-check", action="store_true", help="only dump, don't compare with C++")
    ap.add_argument("--strict", action="store_true", help="treat [CHECK] items as findings (exit code 1)")
    ap.add_argument("--quiet", action="store_true", help="only print MISSING/CHECK lines")
    args = ap.parse_args()
    if not args.target and not args.file:
        ap.error("give a target or --file")

    root = repo_root()
    exe = resolve_exe(root, args.exe)
    pe = PEImage(exe)
    if exe.stat().st_size != EXPECTED_COMPACT_SIZE:
        print(f"WARNING: {exe.name} is {exe.stat().st_size:,} bytes, expected {EXPECTED_COMPACT_SIZE:,} (compact exe) - addresses may not match\n")
    repo = Repo(root)

    if args.file:
        rel = Path(args.file).as_posix()
        addrs = addresses_in_file(repo, rel)
        if not addrs:
            sys.stderr.write(f"error: no reversed hooks found in {rel}\n")
            sys.exit(2)
    else:
        addrs = resolve_target(repo, args.target)
        if not addrs:
            sys.stderr.write(f"error: no address known for `{args.target}` (not installed / no `// 0xADDR` comment) - pass the address\n")
            sys.exit(2)
        if len(addrs) > 1:
            print(f"note: `{args.target}` matches several addresses: {', '.join(f'0x{a:X}' for a in addrs)} - validating all\n")

    total_findings = total_checks = 0
    for addr in addrs:
        rep = Report(args.strict)
        try:
            validate(repo, pe, addr, args, rep)
        except ValueError as e:
            rep.missing(str(e))
        total_findings += rep.findings
        total_checks += rep.checks
        if args.quiet or args.file:
            flagged = [l for l in rep.lines if "[MISSING]" in l or "[CHECK]" in l]
            status = "OK " if not rep.findings else "BAD"
            print(f"[{status}] {rep.lines[0][3:]}  -> {rep.findings} finding(s), {rep.checks} check(s)")
            for l in flagged:
                print("      " + l.strip())
        else:
            print("\n".join(rep.lines))
            print(f"\nResult: {rep.findings} finding(s), {rep.checks} item(s) to check manually.")
    if len(addrs) > 1:
        print(f"\nTotal: {len(addrs)} function(s), {total_findings} finding(s), {total_checks} check(s).")
    if not args.quiet:
        print("Note: a clean fingerprint is necessary, not sufficient - also compare the control flow and run a NOTSA_DIFFTEST.")
    sys.exit(1 if total_findings else 0)


if __name__ == "__main__":
    main()
