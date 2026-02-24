import logging

class ColoredFormatter(logging.Formatter):
    """Custom formatter with colored output and custom prefixes"""
    COLORS = {
        'DEBUG': '\033[36m',    # Cyan
        'INFO': '\033[32m',     # Green
        'WARNING': '\033[33m',  # Yellow
        'ERROR': '\033[31m',    # Red
        'RESET': '\033[0m',     # Reset
    }
    
    PREFIXES = {
        'DEBUG': '[*]',
        'INFO': '[+]',
        'WARNING': '[!]',
        'ERROR': '[x]',
    }
    
    def format(self, record):
        color = self.COLORS.get(record.levelname, '')
        reset = self.COLORS['RESET']
        prefix = self.PREFIXES.get(record.levelname, '')
        record.msg = f"{color}{prefix} {record.msg}{reset}"
        return super().format(record)

def configure_logging():
    """
    Configure logging with the custom colored formatter
    """
    
    handler = logging.StreamHandler()
    handler.setFormatter(ColoredFormatter('%(message)s'))
    logging.basicConfig(level=logging.INFO, handlers=[handler])
