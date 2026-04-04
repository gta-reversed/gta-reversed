import subprocess, argparse, os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
AP.add_argument("--standalone", default=False, action="store_true", help="Build standalone executable instead of .asi plugin (for debugging purposes)")
AP.add_argument("--dump-hooks-only", default=False, action="store_true", help="Don't try to write to memory at all, just dump the hooks that would be applied (for debugging purposes) and exit")
args = AP.parse_args()

try:
    subprocess.run(
        f'conan install {SCRIPT_DIR} --build missing -s build_type="{args.buildconf}" --profile {SCRIPT_DIR}/conanprofile.txt',
        shell=True, check=True
    )
    
    options = ' '.join([f"-D{key}={'ON' if value else 'OFF'}" for key, value in {
        "GTASA_STANDALONE": args.standalone,
        "GTASA_DUMP_HOOKS_ONLY": args.dump_hooks_only,
    }.items()])
    subprocess.run(
        f'cmake {options} --preset default{"-unity" if not args.no_unity_build else ""} {SCRIPT_DIR}',
        shell=True, check=True
    )
except subprocess.CalledProcessError as e:
    print(f"Installation failed with error code {e.returncode}!")
    exit(e.returncode)
else:
    print(f"Installation is done!")
