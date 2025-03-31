import subprocess, argparse, os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
args = AP.parse_args()

try:
    subprocess.run(
        f'conan install {SCRIPT_DIR} --build missing -s build_type="{args.buildconf}" --profile {SCRIPT_DIR}/conanprofile.txt',
        shell=True, check=True
    )

    subprocess.run(
        f'cmake --preset default{'-unity' if not args.no_unity_build else ''} {SCRIPT_DIR}',
        shell=True, check=True
    )
except subprocess.CalledProcessError as e:
    print(f"Installation failed with error code {e.returncode}!")
    exit(e.returncode)
finally:
    print(f"Installation is done!")
