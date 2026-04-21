from argparse import ArgumentParser
from pathlib import Path

parser = ArgumentParser(
    description="Convert unscoped enums to scoped enums in C++ code. Should be ran from the `/source` directrory!"
)
parser.add_argument("enum_name", help="The name of the enum to convert.")
parser.add_argument("enum_old_member", help="The old member name to replace.")
parser.add_argument(
    "enum_new_member", help="The new member name to use in the scoped enum."
)
args = parser.parse_args()


def main():
    for pattern in ["*.h", "*.cpp"]:
        for path in Path(".").rglob(pattern):
            try:
                with path.open("r+", encoding="utf-8") as f:
                    content = f.read()
                    f.seek(0)
                    f.truncate(0)
                    new = f"{args.enum_name}::{args.enum_new_member}"
                    f.write(
                        content
                            .replace(f"{args.enum_name}::{args.enum_old_member}", new)
                            .replace(args.enum_old_member, new)
                    )
            except Exception as e:
                print(f"Error processing {path}: {e}")

if __name__ == "__main__":
    main()
