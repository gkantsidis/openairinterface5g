"""
Common definitions and constants
"""

__all__ = ('MAX_BLOCK_LENGTH', 'BUFFER_LENGTH')

# Maximum number of bytes that can be encoded
MAX_BLOCK_LENGTH = 1056

# Size of intermediate buffer used to output encoding, and decoding input LLR
BUFFER_LENGTH = 68 * 384
