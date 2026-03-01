"""
Python script to format garbage RwRenderStateSet calls to be nice
Make sure to copy the garbage looking code to the clipboard, then run the program
And the output will be set on the clipboard.
When pasting to VS make sure to press CTRL Z once, because it automatically gets formatted incorrectly
"""

import pyperclip as pyc # If the program fails here, please run `pip install pyperclip`
from typing import Type
import re
from enum import Enum

def cint(val : str): # C-style int
    return int(val.rstrip('u'))

def cenum(enum_type : Type[Enum]): # enum cast
    return lambda val: enum_type(cint(val)).name

def cbool(val : str): # c-style capitalized bool [TRUE, FALSE]
    return str(bool(val)).upper()

#
# RW renderstate enums
#

class RwTextureAddressMode(Enum):
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0
    rwTEXTUREADDRESSWRAP = 1
    rwTEXTUREADDRESSMIRROR = 2
    rwTEXTUREADDRESSCLAMP = 3
    rwTEXTUREADDRESSBORDER = 4

class RwBlendFunction(Enum):
    rwBLENDNABLEND      = 0
    rwBLENDZERO         = 1
    rwBLENDONE          = 2
    rwBLENDSRCCOLOR     = 3
    rwBLENDINVSRCCOLOR  = 4
    rwBLENDSRCALPHA     = 5
    rwBLENDINVSRCALPHA  = 6
    rwBLENDDESTALPHA    = 7
    rwBLENDINVDESTALPHA = 8
    rwBLENDDESTCOLOR    = 9
    rwBLENDINVDESTCOLOR = 10
    rwBLENDSRCALPHASAT  = 11

class RwAlphaTestFunction(Enum):
    rwALPHATESTFUNCTIONNAALPHATESTFUNCTION = 0
    rwALPHATESTFUNCTIONNEVER = 1
    rwALPHATESTFUNCTIONLESS = 2
    rwALPHATESTFUNCTIONEQUAL = 3
    rwALPHATESTFUNCTIONLESSEQUAL = 4
    rwALPHATESTFUNCTIONGREATER = 5
    rwALPHATESTFUNCTIONNOTEQUAL = 6
    rwALPHATESTFUNCTIONGREATEREQUAL = 7
    rwALPHATESTFUNCTIONALWAYS = 8


# Render state to type mapping
RwRenderState_To_Type = {
    # None - Not yet implemented
    # Any other type means that the value has to be of that type

    'rwRENDERSTATENARENDERSTATE':            cint,
    'rwRENDERSTATETEXTURERASTER':            str, # pointer in reality
    'rwRENDERSTATETEXTUREADDRESS':           cenum(RwTextureAddressMode),
    'rwRENDERSTATETEXTUREADDRESSU':          cenum(RwTextureAddressMode),
    'rwRENDERSTATETEXTUREADDRESSV':          cenum(RwTextureAddressMode),
    'rwRENDERSTATETEXTUREPERSPECTIVE':       None,
    'rwRENDERSTATEZTESTENABLE':              cbool,
    'rwRENDERSTATESHADEMODE':                None,
    'rwRENDERSTATEZWRITEENABLE':             cbool,
    'rwRENDERSTATETEXTUREFILTER':            None,
    'rwRENDERSTATESRCBLEND':                 cenum(RwBlendFunction),
    'rwRENDERSTATEDESTBLEND':                cenum(RwBlendFunction),
    'rwRENDERSTATEVERTEXALPHAENABLE':        cbool,
    'rwRENDERSTATEBORDERCOLOR':              cint,
    'rwRENDERSTATEFOGENABLE':                cbool,
    'rwRENDERSTATEFOGCOLOR':                 cbool,
    'rwRENDERSTATEFOGTYPE':                  None,
    'rwRENDERSTATEFOGDENSITY':               int,
    'rwRENDERSTATECULLMODE':                 None,
    'rwRENDERSTATESTENCILENABLE':            cbool,
    'rwRENDERSTATESTENCILFAIL':              None,
    'rwRENDERSTATESTENCILZFAIL':             None,
    'rwRENDERSTATESTENCILPASS':              None,
    'rwRENDERSTATESTENCILFUNCTION':          None,
    'rwRENDERSTATESTENCILFUNCTIONREF':       str,
    'rwRENDERSTATESTENCILFUNCTIONMASK':      None,
    'rwRENDERSTATESTENCILFUNCTIONWRITEMASK': None,
    'rwRENDERSTATEALPHATESTFUNCTION':        cenum(RwAlphaTestFunction),
    'rwRENDERSTATEALPHATESTFUNCTIONREF':     None,
}

input("Make sure to have copied the code to the clipboard!")

# Example user input
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZTESTENABLE, 1u);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1u);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, 5u);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, 6u);
# RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (unsigned int)gpCloudMaskTex->raster);

# Tuples of (state_name, state_value); [Ex.: ("rwRENDERSTATEZWRITEENABLE", "0")]
input_values : list[tuple[str, str]] = [
    re.search(r'\((\w+), (.+)\)', line).groups()
    for line in pyc.paste().splitlines()
]

# Find longest name
longest = max(len(n) for (n, v) in input_values)

# Format it
output = []
for n, v in input_values:
    value_type = RwRenderState_To_Type[n]
    if value_type is None:
        raise NotImplementedError(f'Value type for state {n} is not yet implemented!')
    output.append(
        f'RwRenderStateSet({n}, {" " * (longest - len(n))}RWRSTATE({value_type(v)}));'
    )
pyc.copy('\n'.join(output))
