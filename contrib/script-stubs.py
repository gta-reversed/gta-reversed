# pylint: disable=missing-function-docstring, missing-module-docstring, missing-class-docstring

import argparse
from contextlib import contextmanager
import datetime
import io
from pathlib import Path
import re
import requests
from typing import TypedDict, NotRequired


#
# JSON structure definitions
#
Meta = TypedDict("Meta", {"last_update": int, "version": str, "url": str})

CommandInputParameter = TypedDict("CommandInputParameter", {"name": str, "type": str})

CommandOutputParameter = TypedDict("CommandOutputParameter", {"name": str, "type": str})

CommandAttributes = TypedDict(
    "CommandAttributes",
    {
        "is_branch": NotRequired[bool],
        "is_condition": NotRequired[bool],
        "is_constructor": NotRequired[bool],
        "is_destructor": NotRequired[bool],
        "is_nop": NotRequired[bool],
        "is_overload": NotRequired[bool],
        "is_segment": NotRequired[bool],
        "is_static": NotRequired[bool],
        "is_unsupported": NotRequired[bool],
        "is_positional": NotRequired[bool],
    },
)

Command = TypedDict(
    "Command",
    {
        "id": str,  # Command ID in hex form (without 0x prefix)
        "name": str,
        "num_params": int,
        "short_desc": NotRequired[str],
        "input": NotRequired[list[CommandInputParameter]],
        "output": NotRequired[list[CommandOutputParameter]],
        # If the command is a class member, these fields will be present:
        "class": NotRequired[str],
        "member": NotRequired[str],
        # If an operator:
        "operator": NotRequired[str],
        # Attributes
        "attrs": NotRequired[CommandAttributes],
    },
)

Extension = TypedDict("Extension", {"name": str, "commands": list[Command]})

Definitions = TypedDict("Definitions", {"meta": Meta, "extensions": list[Extension]})

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
    "--output", "-o", help="Output file for the generated stubs", default="out.cpp"
)
arg_parser.add_argument(
    "--klass", "-k", help="Regex pattern to match class names", default=None
)
arg_parser.add_argument(
    "--extension",
    "-e",
    help="Regex pattern to match extension names",
    default="default",
)
arg_parser.add_argument(
    "--with-handlers",
    action="store_true",
    help="Generate `REGISTER_COMMAND_HANDLER` stubs for the commands",
)
arg_parser.add_argument(
    "--commented-out", action="store_true", help="Comment out the generated stubs"
)
arg_parser.add_argument(
    "--transform-params",
    action="store_true",
    help="Transform groups of x, y, (z) parameters into vector types",
    default=True,
)
args = arg_parser.parse_args()

# Load command definitions
DEFINITIONS: Definitions = requests.get(args.definitions, timeout=15).json()
print(
    f'[+] Loaded definitions from `{args.definitions}`, version {DEFINITIONS["meta"]["version"]}'
    f'last updated at {datetime.datetime.fromtimestamp(DEFINITIONS["meta"]["last_update"] / 1000).strftime("%Y-%m-%d %H:%M:%S")} (UTC)'
)

# Load enum definitions to apply additional type mappings
ENUMS = {
    line.split(" ", 1)[1].strip()  # enum name
    for line in requests.get(args.enum_definitions, timeout=15).text.splitlines()
    if line.startswith("enum ")
}
print(f"[+] Loaded {len(ENUMS)} enums from `{args.enum_definitions}`")

# Reserved keywords in C++ that cannot be used as parameter names without escaping
CPP_RESERVED_KEYWORDS = {
    # Keywords
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "break",
    "case",
    "catch",
    "class",
    "compl",
    "concept",
    "const",
    "consteval",
    "constexpr",
    "constinit",
    "const_cast",
    "continue",
    "co_await",
    "co_return",
    "co_yield",
    "decltype",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "export",
    "extern",
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "reflexpr",
    "register",
    "reinterpret_cast",
    "requires",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_assert",
    "static_cast",
    "struct",
    "switch",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq",
    "true",
    "false",
    # Types
    "char",
    "char8_t",
    "char16_t",
    "char32_t",
    "float",
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "size_t",
    "ptrdiff_t",
    "nullptr_t",
}

# Types mapped for both input/output
TYPE_MAPPING = (
    {
        "model_char": "eModelID",
        "model_vehicle": "eModelID",
    }
    | ({e: f"e{e}" for e in ENUMS})
    | {
        cmd["class"]: f'C{cmd["class"]}'
        for extension in DEFINITIONS["extensions"]
        for cmd in extension["commands"]
        if "class" in cmd
    }
)

# Types only mapped on input parameters
INPUT_PARAMETER_TYPE_MAPPING = TYPE_MAPPING | {
    "Char": "CPed",
    "Car": "CVehicle",
    "string": "std::string_view",
    "label": "std::string_view",
    "int": "int32",
}

# Types only mapped on output parameters
OUTPUT_PARAMETER_TYPE_MAPPING = TYPE_MAPPING | {
    # Nothing special for now
}


def to_camel_case(s: str) -> str:
    return f"{s[0].lower()}{s[1:]}" if s else s


def get_transformed_input_parameters(command: Command, for_handler_input: bool):
    params = command.get("input", [])
    is_static = command.get("attrs", {}).get("is_static", False)
    out: list[CommandInputParameter] = []

    def is_coord_param(param_name: str, coord: str) -> bool:
        lower_param_name = param_name.lower()
        return lower_param_name.startswith(coord) or lower_param_name.endswith(coord)

    i = 0
    while i < len(params):
        param = params[i]

        try:
            if is_coord_param(param["name"], "x") and is_coord_param(
                params[i + 1]["name"], "y"
            ):
                name = (
                    to_camel_case(
                        re.sub("x$|^x", "", param["name"], flags=re.IGNORECASE)
                    )
                    or f"vec{i}"
                )
                if is_coord_param(params[i + 2]["name"], "z"):
                    out.append(
                        {
                            "name": name,
                            "type": "CVector" if for_handler_input else "Vector",
                        }
                    )
                    i += 3
                else:
                    out.append(
                        {
                            "name": name,
                            "type": "CVector2D" if for_handler_input else "Vector2D",
                        }
                    )
                    i += 2
                continue
        except IndexError:
            pass

        # Handle the common case of a class static function where the first parameter is the handle of the instance, by replacing its type with the class name
        if klass := command.get("class"):
            if is_static and i == 0 and param["name"] == "handle":
                param["type"] = f"C{klass}" if for_handler_input else klass
                param["name"] = (
                    to_camel_case(
                        re.sub(
                            "handle$|^handle", "", param["name"], flags=re.IGNORECASE
                        )
                    )
                    or klass.lower()
                )

        # Apply additional C++ type mappings for input parameters, and add pointer/reference symbols as needed for handler inputs
        if not for_handler_input:
            param["type"] = INPUT_PARAMETER_TYPE_MAPPING.get(
                param["type"], param["type"]
            )
            if param["type"].startswith("C"):
                param["type"] += "*" if is_static and i == 0 else "&"

        # Handle reserved keywords in C++
        if param["name"] in CPP_RESERVED_KEYWORDS:
            param["name"] += "_"

        out.append(param)
        i += 1

    return out


def get_transformed_output_parameters(
    params: list[CommandOutputParameter], is_for_handler_output: bool
):
    return [
        {
            **param,
            "type": (
                OUTPUT_PARAMETER_TYPE_MAPPING.get(param["type"], param["type"])
                if is_for_handler_output
                else param["type"]
            ),
        }
        for param in params
    ]


def write_code_line(f: io.TextIOWrapper, line: str, indent_level: int = 0):
    f.write(f'{"// " if args.commented_out else ""}{'    ' * indent_level}{line}\n')


@contextmanager
def write_multi_line_comment(f: io.TextIOWrapper):
    write_code_line(f, "/*")
    yield
    write_code_line(f, "*/")


def write_docs(f: io.TextIOWrapper, cmd: Command):
    with write_multi_line_comment(f):
        f.write(f' * @opcode {cmd["id"]}\n')
        f.write(f' * @command {cmd["name"]}\n')

        if "class" in cmd:
            f.write(f' * @class {cmd["class"]}\n')
            if "member" in cmd:
                f.write(f' * @method {cmd["member"]}\n')

        if cmd.get("attrs", {}).get("is_static", False):
            f.write(" * @static\n")

        if "short_desc" in cmd:
            f.write(" * \n")
            f.write(f' * @brief {cmd["short_desc"]}\n')

        if input_params := get_transformed_input_parameters(cmd, False):
            f.write(" * \n")
            for param in input_params:
                f.write(f' * @param {{{param["type"]}}} {param["name"]}\n')

        if output_params := get_transformed_output_parameters(
            cmd.get("output", []), False
        ):
            f.write(" * \n")
            f.write(
                f' * @returns {", ".join(f"{{{param["type"]}}} {param["name"]}" for param in output_params)}\n'
            )


def main():
    output_path = Path(args.output)

    commands = [
        command
        for extension in DEFINITIONS["extensions"]
        if not args.extension or re.search(args.extension, extension["name"])
        for command in extension["commands"]
        if re.search(args.commands_pattern, command["name"])
        and (
            not args.klass
            or ("class" in command and re.search(args.klass, command["class"]))
        )
    ]
    if not commands:
        return print("[x] No commands matched the given criteria")

    def get_handler_name(command: Command) -> str:
        return "".join(v.capitalize() for v in command["name"].split("_"))

    with output_path.open("w", encoding="utf-8") as f:
        for cmd in commands:
            attrs = cmd.get("attrs", {})

            if attrs.get("is_nop", False):
                print(
                    f'[!] No stub will be generated for command {cmd["name"]} (0x{cmd["id"]}) since it is marked as a no-op'
                )
                continue

            if attrs.get("is_condition", False):
                f.write("bool")
            elif len(cmd.get("output", [])) == 0:
                f.write("void")
            else:
                f.write("auto")
            write_code_line(
                f,
                f" {get_handler_name(cmd)}({', '.join(f"{param['type']} {param['name']}" for param in get_transformed_input_parameters(cmd, True))}) {{",
            )
            write_code_line(f, 'NOTSA_UNREACHABLE("Not implemented");', 1)
            write_code_line(f, "}", 0)
            f.write("\n")

    with output_path.with_stem(f"{output_path.stem}.handlers").open(
        "w", encoding="utf-8"
    ) as f:
        if args.with_handlers:
            for is_nop in [
                False,
                True,
            ]:  # Separately generate handlers and nops to group them together in the output
                for cmd in commands:
                    if cmd.get("attrs", {}).get("is_nop", False) == is_nop:
                        if is_nop:
                            write_code_line(
                                f,
                                f'REGISTER_COMMAND_HANDLER({cmd["name"]}, {get_handler_name(cmd)});',
                                1,
                            )
                        else:
                            write_code_line(
                                f,
                                f'REGISTER_COMMAND_NOP({cmd["name"]}, {get_handler_name(cmd)});',
                                1,
                            )

                        f.write("\n")

    print(f"[ok] Processed {len(commands)} commands to `{output_path.absolute()}`")


if __name__ == "__main__":
    main()
