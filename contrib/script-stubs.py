import argparse
import io
from pathlib import Path
import re
import requests
from typing import TypedDict, NotRequired


arg_parser = argparse.ArgumentParser(description="Generate function stubs for script commands")
arg_parser.add_argument("commands_pattern", help="Regex pattern to match script commands", default='.')
arg_parser.add_argument("--input", "-i", help="Link containing script command definitions in JSON format", default='https://library.sannybuilder.com/assets/sa/sa.json')
arg_parser.add_argument("--output", "-o", help="Output file for the generated stubs", default='out.cpp')
arg_parser.add_argument("--klass", help="Regex pattern to match class names", default=None)
arg_parser.add_argument("--extension", help="Regex pattern to match extension names", default='default')
arg_parser.add_argument('--with-handlers', action='store_true', help="Generate `REGISTER_COMMAND_HANDLER` stubs for the commands")
arg_parser.add_argument('--commented-out', action='store_true', help="Comment out the generated stubs")
arg_parser.add_argument('--transform-params', action='store_true', help="Transform groups of x, y, (z) parameters into vector types", default=True)
args = arg_parser.parse_args()

TYPE_MAPPING = {
    'Car': 'CVehicle&',
    'Char': 'CPed&',
    'PedType': 'CPed&',
    'Boat': 'CBoat&',
    'model_char': 'eModelID',
    'model_vehicle': 'eModelID',
    'string': 'std::string_view',
}

Meta = TypedDict('Meta', {
    'last_updated': int,
    'version': str,
    'url': str
})

CommandInputParameter = TypedDict('CommandInputParameter', {
    'name': str,
    'type': str
})

CommandOutputParameter = TypedDict('CommandOutputParameter', {
    'name': str,
    'type': str
})

CommandAttributes = TypedDict('CommandAttributes', {
    'nop': NotRequired[bool]
})

Command = TypedDict('Command', {
    'id': str, # Command ID in hex form (without 0x prefix)
    'name': str,
    'num_params': int,
    'short_desc': NotRequired[str],
    
    'input': NotRequired[list[CommandInputParameter]],
    'output': NotRequired[list[CommandOutputParameter]],

    # If the command is a class member, these fields will be present:
    'class': NotRequired[str],
    'member': NotRequired[str],
    
    # If an operator:
    'operator': NotRequired[str],
    
    # Attributes
    'attrs': NotRequired[CommandAttributes]
})

Extension = TypedDict('Extension', {
    'name': str,
    'commands': list[Command]  
})

Definitions = TypedDict('Definitions', {
    'meta': Meta,
    'extensions': list[Extension]
})

def to_camel_case(str: str) -> str:
    return f'{str[0].lower()}{str[1:]}' if str else str

def main():
    definitions: Definitions = requests.get(args.input, timeout=15).json()
    
    output_path = Path(args.output)

    commands = [
        command
        for extension in definitions['extensions']
            if not args.extension or re.search(args.extension, extension['name'])
        for command in extension['commands']
            if re.search(args.commands_pattern, command['name']) and
                (not args.klass or ('class' in command and re.search(args.klass, command['class'])))
    ]
    if not commands:
        return print("No commands matched the given criteria")

    def get_handler_name(command: Command) -> str:
        return ''.join(v.capitalize() for v in command['name'].split('_'))
    
    def write_code_line(f: io.TextIOWrapper, line: str, indent_level: int = 0):
        indent = '    ' * indent_level
        f.write(f'{"// " if args.commented_out else ""}{indent}{line}\n')
    
    with output_path.open('w', encoding='utf-8') as f:
        for cmd in commands:
            input_params: list[CommandInputParameter] = [
                {
                    'name': param['name'],
                    'type': TYPE_MAPPING.get(param['type'], param['type'])
                }
                for param in cmd.get('input', [])
            ]
                   
            handler_input_params: list[CommandInputParameter] = []
            i = 0
            while i < len(input_params):
                param = input_params[i]

                #
                # A group of x, y, (z) values can be represented as a vector type
                #
                if args.transform_params:
                    def is_coord_param(idx: int, coord: str, params: list[CommandInputParameter] = input_params) -> bool:
                        try:
                            param_name = params[idx]['name'].lower()
                            return param_name.startswith(coord) or param_name.endswith(coord)
                        except IndexError:
                            return False

                    if is_coord_param(i, 'x') and is_coord_param(i + 1, 'y'):
                        def get_new_param_name(pattern, param=param, i=i): 
                            return to_camel_case(re.sub(f'[{pattern}]$|^[{pattern}]', '', param['name'], flags=re.IGNORECASE)) or f'vec{i}'
                        
                        if is_coord_param(i + 2, 'z'):
                            handler_input_params.append({
                                'name': get_new_param_name('xyz'),
                                'type': 'CVector'
                            })
                            i += 3
                        else:
                            handler_input_params.append({
                                'name': get_new_param_name('xy'),
                                'type': 'CVector2D'
                            })
                            i += 2
                        continue

                #
                # Not a vector param, add as-is (with type mapping)
                # 
                handler_input_params.append(param)
                i += 1
  
            output_params = [
                {
                    'name': param['name'],
                    'type': TYPE_MAPPING.get(param['type'], param['type'])
                }
                for param in cmd.get('output', [])
            ]
            return_type = \
                'void' if len(output_params) == 0 else \
                output_params[0]['type'] if len(output_params) == 1 else \
                'notsa::script::MultiRet<' + ', '.join(param['type'] for param in output_params) + '>'

            f.write('/*\n')
            f.write(f' * @opcode {cmd["id"]}\n')
            f.write(f' * @command {cmd["name"]}\n')
            if 'class' in cmd:
                f.write(f' * @class {cmd["class"]}\n')
                if 'member' in cmd:
                    f.write(f' * @method {cmd["member"]}\n')
            if 'short_desc' in cmd:
                f.write(' * \n')
                f.write(f' * @brief {cmd["short_desc"]}\n')
            if input_params:
                f.write(' * \n')
                for param in input_params: # Use original input params for documentation, as the handler params are an implementation detail
                    f.write(f' * @param {param["name"]} {param["type"]}\n')
            f.write(' */\n')
            
            write_code_line(f, f"{return_type} {get_handler_name(cmd)}({', '.join(f"{param['type']} {param['name']}" for param in handler_input_params)}) {{")
            if 'attrs' in cmd and cmd['attrs'].get('nop', False) or cmd.get('short_desc') == 'Does nothing':
                write_code_line(f, '/* no-op */', 1)
            else:
                write_code_line(f, 'NOTSA_UNREACHABLE("Not implemented");', 1)
            write_code_line(f, '}', 0)
            f.write('\n')
           
    with output_path.with_stem(f'{output_path.stem}.handlers').open('w', encoding='utf-8') as f:
        if args.with_handlers:
            for cmd in commands:
                write_code_line(f, f'REGISTER_COMMAND_HANDLER({cmd["name"]}, {get_handler_name(cmd)});', 1)

    print(f'Generated stubs for {len(commands)} commands in `{output_path.absolute()}`')

if __name__ == "__main__":
    main()
