import subprocess, argparse, os, sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--build", action="store_true", help="build instead of setting up & configuring")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
AP.add_argument("--profile", default="conanprofile.txt", help="custom profile")
args = AP.parse_args()

try:
    preset = f'default{"-unity" if not args.no_unity_build else ""}'
    genpath = SCRIPT_DIR # FIXME: CMake errors with Ninja: "Does not match the generator used previously: Unix Makefiles"
    extcfg = ""
    if sys.platform == "linux":
        args.profile = "conanprofile-wine.txt"
        preset = f'conan-{args.buildconf.lower()}'
        extcfg = "-DGTASA_UNITY_BUILD=ON -DGTASA_NO_EDIT_AND_CONTINUE=ON"
        genpath = ""

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

    subprocess.run(
        f'cmake --preset {preset} {genpath} {extcfg}',
        shell=True, check=True
    )
except subprocess.CalledProcessError as e:
    print(f'Installation failed with error code {e.returncode}!')
    exit(e.returncode)
else:
    print(f'Installation is done!')
