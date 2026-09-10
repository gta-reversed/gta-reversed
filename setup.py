from argparse import ArgumentParser
import subprocess

ap = ArgumentParser(description="Setup script for the project")
#ap.add_argument('-o', '--options', dest='options', default=[], nargs='+', help='Options passed to Conan (See `conanfile` for available options)')
#ap.add_argument('-s', '--settings', dest='settings', default=[], nargs='+', help='Settings passed to Conan (See `conanfile` for available settings)')
ap.add_argument('-p', '--profile', dest='profile', default='windows-msvc.txt', help='Conan profile to use for the build')
ap.add_argument('-b', '--build', dest='build', action='store_true', help='Build the project after setup')

def main() -> None:
    args, conan_args = ap.parse_known_args()

    profile = f'./conan/profiles/{args.profile}'

    subprocess.run(["conan", "install", ".", "--build=missing", "--profile", profile, *conan_args], check=True)
    if args.build:
        subprocess.run(["conan", "build", ".", "--profile", profile, *conan_args], check=True)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
