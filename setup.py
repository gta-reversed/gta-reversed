import subprocess, argparse, os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

AP = argparse.ArgumentParser(description="gta-reversed project setup script")
AP.add_argument("--no-unity-build", action="store_true", help="disable unity build")
AP.add_argument("--no-sdl", action="store_true", help="disable SDL3 and use DirectInput instead")
AP.add_argument("--buildconf", default="Debug", choices=["Debug", "Release", "RelWithDebInfo"], help="cmake compilation type")
args = AP.parse_args()

try:
    # In setup.py, when calling Conan
    if args.no_sdl:
        # Use a list to avoid shell escaping issues
        subprocess.run([
            "conan", "install", SCRIPT_DIR,
            "--build", "missing",
            "-s", f"build_type={args.buildconf}",
            "--profile", f"{SCRIPT_DIR}/conanprofile.txt",
            "-o", "use_sdl3=False"
        ], check=True)
    else:
        subprocess.run([
            "conan", "install", SCRIPT_DIR,
            "--build", "missing",
            "-s", f"build_type={args.buildconf}",
            "--profile", f"{SCRIPT_DIR}/conanprofile.txt"
        ], check=True)

    # Choose preset based on arguments
    preset_base = "default"
    if not args.no_unity_build:
        preset_base += "-unity"
    if args.no_sdl:
        preset_base += "-nosdl"
        
    subprocess.run(
        f'cmake --preset {preset_base} {SCRIPT_DIR}',
        shell=True, check=True
    )
except subprocess.CalledProcessError as e:
    print(f"Installation failed with error code {e.returncode}!")
    exit(e.returncode)
else:
    print(f"Installation is done!")