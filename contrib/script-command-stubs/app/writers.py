from contextlib import contextmanager
import io

from .jsontypes import Command
from . import typemapper, util
from .args import args


def write_code_line(f: io.TextIOWrapper, line: str, indent_level: int = 0, suffix="\n"):
    """
    Writes a single line of code to the provided file-like object, with optional indentation and commenting out based on command-line arguments.
    If `args.commented_out` is True, the line will be prefixed with "// ".
    """

    f.write("    " * indent_level)
    if args.commented_out:
        f.write("//")
    f.write(line)
    f.write(suffix)


@contextmanager
def write_multi_line_comment(f: io.TextIOWrapper):
    """
    Context manager for writing multi-line comments. When used, it will automatically wrap the content in /* ... */.
    """

    f.write("/*\n")
    yield
    f.write(" */\n")


def write_docs(f: io.TextIOWrapper, cmd: Command):
    """
    Write C++ doxygen-like documentation for the given command to the provided file-like object.
    The documentation includes opcode, command name, class/member info, static/condition attributes, brief description, and parameter/return type information.
    """

    def write_ln(line: str = "\n"):
        f.write(f"{line}\n")

    with write_multi_line_comment(f):
        write_ln(f' * @opcode {cmd["id"]}')
        write_ln(f' * @command {cmd["name"]}')

        if "class" in cmd:
            write_ln(f' * @class {cmd["class"]}')
            if "member" in cmd:
                write_ln(f' * @method {cmd["member"]}')

        if cmd.get("attrs", {}).get("is_static", False):
            write_ln(" * @static")

        if "short_desc" in cmd:
            write_ln(" * ")
            write_ln(f' * @brief {cmd["short_desc"]}')

        if input_params := typemapper.get_transformed_input_parameters(cmd, False):
            write_ln(" * ")
            for param in input_params:
                write_ln(f' * @param {{{param["type"]}}} {param["name"]}')

        if output_params := typemapper.get_transformed_output_parameters(
            cmd.get("output", []), False
        ):
            write_ln(" * ")
            write_ln(
                f' * @returns {", ".join(f"{{{param["type"]}}} {param["name"]}" for param in output_params)}'
            )


def write_handler_function_stub(f: io.TextIOWrapper, cmd: Command):
    """
    Writes C++ handler function stub for the given command to the provided file-like object.
    The function signature is determined based on the command's attributes and output parameters.
    """

    # Function definition line
    write_code_line(
        f,
        f"{util.get_handler_return_type(cmd)} {util.get_handler_name(cmd)}({', '.join(f"{param['type']} {param['name']}" for param in typemapper.get_transformed_input_parameters(cmd, True))}) {{",
    )

    # Function body (stub)
    write_code_line(f, 'NOTSA_UNREACHABLE("Not implemented");', 1)

    # Closing brace for the function
    write_code_line(f, "}", 0)


def write_register_handler(f: io.TextIOWrapper, cmd: Command):
    """
    Writes the appropriate command registration line for the given command to the provided file-like object.
    Depending on whether the command is a no-op or not, it will use either REGISTER_COMMAND_HANDLER or REGISTER_COMMAND_NOP.
    """

    if cmd.get("attrs", {}).get("is_nop", False):
        write_code_line(
            f,
            f'REGISTER_COMMAND_NOP({cmd["name"]}, {util.get_handler_name(cmd)});',
            1,
        )
    else:
        write_code_line(
            f,
            f'REGISTER_COMMAND_HANDLER({cmd["name"]}, {util.get_handler_name(cmd)});',
            1,
        )
