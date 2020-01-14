"""
Interface to LDPC's encoder functionality
"""

__all__ = ('encode', 'EncoderError')

import logging
from ctypes import c_ubyte
from . import _distributor_init as _lib
from .errors import LdpcError

logger = logging.getLogger(__name__)

_buffer_length = 68 * 384
_max_block_length = 1056


class EncoderError(LdpcError):
    pass


def encode(buffer: bytes) -> bytearray:
    length = len(buffer)
    assert length == _max_block_length
    raw_buffer = (c_ubyte*len(buffer))(*(buffer))
    raw_output = (c_ubyte*_buffer_length)()
    result = _lib.encode_full(raw_buffer, length, raw_output, 1)
    if result < 0:
        logger.error("Encoder returned value %d", result)
        raise(EncoderError(f'Encoder returned {result}'))

    return bytearray(raw_output)
