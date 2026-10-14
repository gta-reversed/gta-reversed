#!/usr/bin/env python3
"""
PostToolUse hook (Edit|Write): after a C++ file under source/ changes, run the hook-registration checker on it and
feed any [E] findings back to Claude. Never blocks; silent when the file is clean or not a source file.
"""
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

try:
    data = json.load(sys.stdin)
except ValueError:
    sys.exit(0)
path = (data.get("tool_input") or {}).get("file_path") or (data.get("tool_response") or {}).get("filePath") or ""
p = Path(path)
try:
    rel = p.resolve().relative_to(REPO).as_posix()
except (ValueError, OSError):
    sys.exit(0)
if not rel.startswith("source/") or p.suffix not in (".cpp", ".h", ".hpp"):
    sys.exit(0)

files = [rel]
pair = p.with_suffix(".h" if p.suffix == ".cpp" else ".cpp")
if pair.exists():
    files.append(pair.resolve().relative_to(REPO).as_posix())
r = subprocess.run([sys.executable, str(REPO / ".claude/skills/review-reversal/scripts/check_hooks.py"), *files],
                   cwd=REPO, capture_output=True, text=True, timeout=120)
errors = [l for l in r.stdout.splitlines() if l.startswith("[E]")]
if errors:
    print(json.dumps({"hookSpecificOutput": {
        "hookEventName": "PostToolUse",
        "additionalContext": "check_hooks.py found hook-registration errors after this edit:\n" + "\n".join(errors),
    }}))
sys.exit(0)
