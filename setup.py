import subprocess, argparse, os, sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--build", action="store_true", help="build instead of setting up & configuring")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
AP.add_argument("--standalone", default=False, action="store_true", help="Build standalone executable instead of .asi plugin (for debugging purposes)")
AP.add_argument("--dump-hooks-only", default=False, action="store_true", help="Don't try to write to memory at all, just dump the hooks that would be applied (for debugging purposes) and exit - To provide a path use the `--dump-hooks-to` argument to on the resulting executable")
AP.add_argument("--profile", default="conanprofile.txt", help="custom profile")
args = AP.parse_args()

if not args.standalone and args.dump_hooks_only:
    AP.error("The --dump-hooks-only option can only be used with the --standalone option")
    exit(1)

try:
    # defines passed to cmake via -D<key>=<value> arguments
    defines: dict[str, bool] = {
        "GTASA_STANDALONE": args.standalone,
        "GTASA_DUMP_HOOKS_ONLY": args.dump_hooks_only,
        "GTASA_UNITY_BUILD": not args.no_unity_build
    }
    
    preset = f'default{"-unity" if not args.no_unity_build else ""}'
    genpath = SCRIPT_DIR # FIXME: CMake errors with Ninja: "Does not match the generator used previously: Unix Makefiles"
    if sys.platform == "linux":
        args.profile = "conanprofile-wine.txt"
        preset = f'conan-{args.buildconf.lower()}'
        genpath = ""
        defines["GTASA_NO_EDIT_AND_CONTINUE"] = True

    if args.build:
        subprocess.run(
            f'cmake --build --preset {preset}',
            shell=True, check=True
        )
        exit(0)

    subprocess.run(
        f'conan install {SCRIPT_DIR} --build missing -s build_type="{args.buildconf}" --profile {SCRIPT_DIR}/{args.profile}',
        shell=True, check=True
    )
    
    options = ' '.join([f"-D{key}={'ON' if value else 'OFF'}" for key, value in defines.items()])
    subprocess.run(
        f'cmake --preset {preset} {genpath} {options}',
        shell=True, check=True
    )
except subprocess.CalledProcessError as e:
    print(f'Installation failed with error code {e.returncode}!')
    exit(e.returncode)
else:
    print(f'Installation is done!')
