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
        "id": str,  # Command ID (opcode) in hex form (without 0x prefix)
        "name": str, # Command enum name (e.g. `FOO_BAR`) [Without the `COMMAND_` prefix]
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
