import re

from .cpp import is_cpp_reserved_keyword
from .data import DEFINITIONS, ENUMS
from .jsontypes import Command, CommandInputParameter, CommandOutputParameter
from . import util

# Types mapped for both input and output parameters
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


def get_transformed_input_parameters(command: Command, is_for_handler: bool):
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
                    util.to_camel_case(
                        re.sub("x$|^x", "", param["name"], flags=re.IGNORECASE)
                    )
                    or f"vec{i}"
                )
                if is_coord_param(params[i + 2]["name"], "z"):
                    out.append(
                        {
                            "name": name,
                            "type": "CVector" if is_for_handler else "Vector",
                        }
                    )
                    i += 3
                else:
                    out.append(
                        {
                            "name": name,
                            "type": "CVector2D" if is_for_handler else "Vector2D",
                        }
                    )
                    i += 2
                continue
        except IndexError:
            pass

        # Handle the common case of a class static function 
        # where the first parameter is the handle of the instance, 
        # by replacing its type with the class name
        if klass := command.get("class"):
            if is_static and i == 0 and param["name"] == "handle":
                param["type"] = f"C{klass}" if is_for_handler else klass
                param["name"] = (
                    util.to_camel_case(
                        re.sub(
                            "handle$|^handle", "", param["name"], flags=re.IGNORECASE
                        )
                    )
                    or klass.lower()
                )

        # Apply additional C++ type mappings for input parameters, 
        # and add pointer/reference symbols as needed for handler inputs
        if is_for_handler:
            param["type"] = INPUT_PARAMETER_TYPE_MAPPING.get(
                param["type"], param["type"]
            )
            if param["type"].startswith("C"):
                param["type"] += "*" if is_static and i == 0 else "&"

        # Handle reserved keywords in C++
        if is_cpp_reserved_keyword(param["name"]):
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
