import datetime
import logging
import requests

from .jsontypes import Definitions
from .args import args

logger = logging.getLogger(__name__)

# Load command definitions
DEFINITIONS: Definitions = requests.get(args.definitions, timeout=15).json()
logger.info(
    "Loaded definitions from `%s`, version %s, last updated at %s (UTC)",
    args.definitions,
    DEFINITIONS["meta"]["version"],
    datetime.datetime.fromtimestamp(DEFINITIONS["meta"]["last_update"] / 1000).strftime(
        "%Y-%m-%d %H:%M:%S"
    ),
)

# Load enum definitions to apply additional type mappings
ENUMS = {
    line.split(" ", 1)[1].strip()  # enum name
    for line in requests.get(args.enum_definitions, timeout=15).text.splitlines()
    if line.startswith("enum ")
}
logger.info("Loaded %d enums from `%s`", len(ENUMS), args.enum_definitions)
