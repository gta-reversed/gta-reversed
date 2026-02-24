from .jsontypes import Command


def to_camel_case(s: str) -> str:
    return f"{s[0].lower()}{s[1:]}" if s else s

def get_handler_name(command: Command) -> str:
    return "".join(v.capitalize() for v in command["name"].split("_"))

def get_handler_return_type(command: Command) -> str:
    if command.get("attrs", {}).get("is_condition", False):
        return "bool"
    elif len(command.get("output", [])) == 0:
        return "void"
    else:
        return "auto"
