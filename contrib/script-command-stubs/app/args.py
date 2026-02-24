import argparse

arg_parser = argparse.ArgumentParser(
    description="Generate function stubs for script commands"
)
arg_parser.add_argument(
    "commands_pattern", help="Regex pattern to match script commands", default="."
)
arg_parser.add_argument(
    "--definitions",
    "-d",
    help="Link containing script command definitions in JSON format",
    default="https://library.sannybuilder.com/assets/sa/sa.json",
)
arg_parser.add_argument(
    "--enum-definitions",
    help="Link containing enum definitions",
    default="https://library.sannybuilder.com/assets/sa/enums.txt",
)
arg_parser.add_argument(
    "--input", '-i', help="Add missing docs and stubs to an existing file instead of generating a new one", default=None
)
arg_parser.add_argument(
    "--output", "-o", help="Output file for the generated stubs or the file to update with missing docs and stubs", default=None
)
arg_parser.add_argument(
    "--klass", "-k", help="Regex pattern to match class names", default=None
)
arg_parser.add_argument(
    "--extension",
    "-e",
    help="Regex pattern to match extension names (From the definitions file)",
    default="default",
)
arg_parser.add_argument(
    "--generate-register-calls",
    action="store_true",
    help="Generate `REGISTER_COMMAND_HANDLER` stubs for the commands",
)
arg_parser.add_argument(
    "--commented-out", action="store_true", help="Comment out the generated code"
)
arg_parser.add_argument(
    "--vectorize-params",
    action="store_true",
    help="Transform groups of x, y, (z) parameters into vector types (CVector2D, CVector for cpp handler, or Vector, Vector2D in script docs)",
    default=True,
)
args = arg_parser.parse_args()
