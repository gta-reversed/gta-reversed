#!/usr/bin/env python3
"""
Minimal client for the ida-pro-mcp JSON-RPC server (IDA GUI plugin, default http://127.0.0.1:13337/mcp).
Lets scripts query IDA without going through an AI context window.

  python .claude/skills/find-reversing-targets/scripts/ida_rpc.py health
  python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export        # -> .claude/cache/idb_export.json
  python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export-xrefs  # -> .claude/cache/xrefs_export.json (calls, globals, enums)
  python .claude/skills/find-reversing-targets/scripts/ida_rpc.py call decompile '{"addr": "0x53CB00"}'

Env: IDA_MCP_URL to override the endpoint. Only `export`/`export-xrefs` run Python inside IDA; both exporters are read-only.
"""
from __future__ import annotations

import json
import os
import sys
import urllib.request
from pathlib import Path

URL = os.environ.get("IDA_MCP_URL", "http://127.0.0.1:13337/mcp")
HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]


def call(tool: str, args: dict, timeout: int = 900) -> dict:
    body = json.dumps({"jsonrpc": "2.0", "id": 1, "method": "tools/call", "params": {"name": tool, "arguments": args}}).encode()
    req = urllib.request.Request(URL, data=body, method="POST", headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        resp = json.load(r)
    if "error" in resp:
        raise RuntimeError(resp["error"])
    res = resp["result"]
    return res.get("structuredContent") or json.loads(res["content"][0]["text"])


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd = sys.argv[1]
    try:
        if cmd == "health":
            print(json.dumps(call("server_health", {}), indent=2))
        elif cmd in ("export", "export-xrefs"):
            script, env, name = (("ida_export_idb.py", "GTASA_IDB_EXPORT", "idb_export.json") if cmd == "export" else
                                 ("ida_export_xrefs.py", "GTASA_XREFS_EXPORT", "xrefs_export.json"))
            out = REPO / ".claude" / "cache" / name
            out.parent.mkdir(parents=True, exist_ok=True)
            code = (f"import os; os.environ[{env!r}] = {str(out)!r}\n"
                    f"exec(open({str(HERE / script)!r}, encoding='utf-8').read(), {{'__name__': '__main__'}})")
            r = call("py_eval", {"code": code})
            print(r.get("stdout", "").strip() or r)
            if r.get("stderr"):
                print(r["stderr"], file=sys.stderr)
        elif cmd == "call":
            print(json.dumps(call(sys.argv[2], json.loads(sys.argv[3]) if len(sys.argv) > 3 else {}), indent=2)[:20000])
        else:
            sys.exit(__doc__)
    except OSError as e:
        sys.exit(f"error: can't reach the IDA MCP server at {URL} ({e}) - is IDA open with the MCP plugin started?")


if __name__ == "__main__":
    main()
