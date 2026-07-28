#!/bin/bash
# Originally made by j0y/gta-reversed-diff-test
#
# Patch msvc-wine cl wrappers to replace /Zi with /Z7
# mspdbsrv.exe doesn't work for parallel builds under Wine (C1902 error)
# /Z7 embeds debug info in .obj files directly (no PDB server needed)

for cl_script in /opt/msvc/bin/*/cl; do
    [ -f "$cl_script" ] || continue

    # Insert the /Zi filter before the wine-msvc.sh invocation
    sed -i '/wine-msvc.sh "$BINDIR"\/cl.exe "$@"/i\
# [PATCH] Filter /Zi -> /Z7 and strip PDB flags for Wine compatibility\
_filtered_args=()\
for _a in "$@"; do\
    case "$_a" in\
        /Zi|-Zi) _filtered_args+=("/Z7") ;;\
        /Fd*) ;;\
        /FS) ;;\
        *) _filtered_args+=("$_a") ;;\
    esac\
done\
set -- "${_filtered_args[@]}"' "$cl_script"

    echo "Patched: $cl_script"
done
