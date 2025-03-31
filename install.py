import subprocess, argparse

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
args = AP.parse_args()

subprocess.run(
    f'conan install . --build missing -s build_type="{args.buildconf}" --profile conanprofile.txt',
    shell=True, check=True
)

subprocess.run(
    f'cmake --preset default{'-unity' if not args.no_unity_build else ''}',
    shell=True, check=True
)
