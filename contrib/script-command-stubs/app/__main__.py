from pathlib import Path
from pprint import pprint
import re
import logging

from . import util
from .data import DEFINITIONS
from .writers import write_code_line, write_docs, write_handler_function_stub
from .args import args
from .jsontypes import Command

logger = logging.getLogger(__name__)


def update_existing(commands_by_criteria: list[Command]):
    """
    Update existing file (handles case when args.input is provided)
    """

    all_commands = [
        command
        for extension in DEFINITIONS["extensions"]
        for command in extension["commands"]
    ]
    commands_by_name = {cmd["name"]: cmd for cmd in all_commands}

    with Path(args.input).open("r+", encoding="utf-8") as f:
        lines = f.readlines()

    with Path(args.output or args.input).open("w", encoding="utf-8") as f:
        # Find where `RegisterHandlers` is
        # We assume it's at the end of the file, if not, the code below won't work all that good...
        register_handlers_line_index = next(
            (
                i
                for i, line in enumerate(lines)
                if line.find("RegisterHandlers()") != -1
            ),
            -1,
        )
        if register_handlers_line_index == -1:
            raise NotImplementedError(
                "Could not find `RegisterHandlers()` function in the input file - cannot add missing handlers"
            ) from None

        # Find all already registered handlers in the file to avoid adding duplicate registrations/stubs
        # It matches on commented out register lines as well
        register_handler_macros = {
            "REGISTER_COMMAND_HANDLER",
            "REGISTER_COMMAND_UNIMPLEMENTED",
            "REGISTER_COMMAND_NOP",
            "REGISTER_UNSUPPORTED_COMMAND_HANDLER",
        }
        registered_handler_macros_regex = re.compile(
            r"^\s*(\/\/)?\s*(?P<macro>"
            + "|".join(register_handler_macros)
            + r")\s*\(\s*COMMAND_(?P<command_name>[A-Za-z0-9_]+)\s*,\s*(?P<handler_name>[A-Za-z0-9_]+)\s*\)\s*;"
        )
        register_handlers_by_command = {
            match.group("command_name"): (  # No COMMAND_ prefix
                match.group("handler_name"),
                match.group("macro"),
            )
            for line in lines
            if (match := registered_handler_macros_regex.match(line.strip()))
        }
        # pprint(register_handlers_by_command)

        handler_to_command_map = {
            handler: commands_by_name.get(command_name.removeprefix("COMMAND_"))
            for command_name, (handler, _) in register_handlers_by_command.items()
        }
        # pprint(handler_to_command_map)

        # Find commands that match the criteria but don't have a registered handler in the file
        missing_register_handler_commands = [
            commands_by_name[cmd_name]
            for cmd_name in set(cmd["name"] for cmd in commands_by_criteria)
            - set(register_handlers_by_command.keys())
        ]

        # Old-style single-line docs comment regex (e.g. `// COMMAND_FOO` or `/// COMMAND_FOO - some description` with or without the `COMMAND_` prefix)
        singleline_docs_comment_regex = re.compile(
            rf"^\s*//+\s*(?P<command_name>[COMMAND_]?{'|'.join(cmd for cmd in register_handlers_by_command.keys())})(?:\s*-\s*(?P<description>.*))?$"
        )

        # Regex for handler
        handler_regex = re.compile(
            rf"^\s*(?P<return_type>[A-Za-z0-9_:]+)\s+(?P<handler_name>{'|'.join(handler for handler, _ in register_handlers_by_command.values())})\s*\((?P<params>[^)]*)\)\s*{{"
        )

        # Check if line should be written as-is, or needs to be replaced by new docs comment
        def get_line_info_for_docs(
            line: str,
        ) -> tuple[Command, bool] | tuple[None, None]:
            # Handle old-style single-line docs comments and replace them with new-style
            if match := singleline_docs_comment_regex.match(line):
                command_name = match.group("command_name").removeprefix("COMMAND_")
                command = commands_by_name.get(command_name)

                if command:
                    return command, True

                logger.warning(
                    "Command `%s` found in docs comment but not in definitions, skipping doc generation for it",
                    command_name,
                )

            # Found handler, try to find corresponding command to extract info for docs generation
            elif match := handler_regex.match(line):
                handler_name = match.group("handler_name")
                command = handler_to_command_map.get(handler_name)

                if command:
                    return command, False

                logger.warning(
                    "Handler `%s` found, but has no corresponding register handler call - skipping docs generation",
                    handler_name,
                )

            return None, None

        # Keep track of handlers we've already added docs for
        docs_added_to = set()

        # Add docs to handlers
        for line in lines[:register_handlers_line_index]:
            stripped_line = line.strip()

            command, replace_line = get_line_info_for_docs(stripped_line)
            if command and command["id"] not in docs_added_to:
                write_docs(f, command)
                docs_added_to.add(command["id"])
                if not replace_line:
                    f.write(line)
            else:
                f.write(line)

        logger.info("Added missing docs to %i handlers", len(docs_added_to))

        # Add missing stubs (after existing handlers but before the `RegisterHandlers` function)
        for cmd in missing_register_handler_commands:
            write_docs(f, cmd)
            write_handler_function_stub(f, cmd)
            f.write("\n")

        # Write the line with `RegisterHandlers()` function declaration and `REGISTER_COMMAND_HANDLER_BEGIN` before adding new handlers
        register_command_handler_begin_line_index = next(
            (
                i
                for i, line in enumerate(lines)
                if line.find("REGISTER_COMMAND_HANDLER_BEGIN") != -1
            ),
            -1,
        )
        if register_command_handler_begin_line_index == -1:
            raise NotImplementedError(
                "Could not find `REGISTER_COMMAND_HANDLER_BEGIN` in the input file - cannot add missing handlers"
            ) from None
        for line in lines[register_handlers_line_index:register_command_handler_begin_line_index + 1]:
            f.write(line)

        # Add missing register handler calls
        if missing_register_handler_commands:
            def get_macro_for_command(cmd: Command) -> str:
                if cmd.get("attrs", {}).get("is_nop", False):
                    return "REGISTER_COMMAND_NOP"
                elif cmd.get("attrs", {}).get("is_unsupported", False):
                    return "REGISTER_UNSUPPORTED_COMMAND_HANDLER"
                else:
                    return "REGISTER_COMMAND_HANDLER"
            for line in sorted([
                f'{get_macro_for_command(cmd)}(COMMAND_{cmd["name"]}, {util.get_handler_name(cmd)});'
                for cmd in missing_register_handler_commands
            ]):
                write_code_line(f, line, 1)
            
            logger.info(
                "Added missing handlers for %i commands",
                len(missing_register_handler_commands),
            )

        # Write rest of the file as-is
        for line in lines[register_command_handler_begin_line_index + 1 :]:
            f.write(line)

    logger.info("Added missing docs and stubs to `%s`", args.input)


def main():
    output_path = Path(args.output)

    # Gather commands matching the specified criteria (extension, command name pattern, class name pattern, etc...)
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
        return logger.error("No commands matched the given criteria")

    if args.input:
        return update_existing(commands)

    # Write stubs
    with output_path.open("w", encoding="utf-8") as f:
        for cmd in commands:
            if cmd.get("attrs", {}).get("is_nop", False):
                logger.warning(
                    "No stub will be generated for command %s (0x%x) since it is marked as a no-op",
                    cmd["name"],
                    cmd["id"],
                )
                continue

            write_docs(f, cmd)
            write_handler_function_stub(f, cmd)

    # Write handlers
    with output_path.with_stem(f"{output_path.stem}.handlers").open(
        "w", encoding="utf-8"
    ) as f:
        if args.with_handlers:
            # Separately generate handlers and nops to group them together in the output
            for is_nop in [
                False,
                True,
            ]:
                for cmd in commands:
                    if cmd.get("attrs", {}).get("is_nop", False) == is_nop:
                        if is_nop:
                            write_code_line(
                                f,
                                f'REGISTER_COMMAND_HANDLER({cmd["name"]}, {util.get_handler_name(cmd)});',
                                1,
                            )
                        else:
                            write_code_line(
                                f,
                                f'REGISTER_COMMAND_NOP({cmd["name"]}, {util.get_handler_name(cmd)});',
                                1,
                            )

                        f.write("\n")

    logger.info("Processed %i commands to `%s`", len(commands), output_path.absolute())


if __name__ == "__main__":
    main()
