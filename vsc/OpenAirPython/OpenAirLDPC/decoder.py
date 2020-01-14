"""
Interface to LDPC decoder
"""

__all__ = ('create_decoder', 'delete_decoder', 'DecoderError')

import logging
from ctypes import c_void_p, c_int8, c_char, POINTER
from . import _distributor_init as _lib
from .errors import LdpcError

logger = logging.getLogger(__name__)

_max_block_length = 1056
_buffer_length = 68 * 384

class DecoderError(LdpcError):
    pass


def create_decoder() -> c_void_p:
    logger.info('Creating decoder')
    decoder = _lib.create_decoder()
    return c_void_p(decoder)


def delete_decoder(decoder: c_void_p) -> None:
    logger.info('Destroying decoder from {}', decoder)
    _lib.delete_decoder(decoder.value)


def decode(decoder: c_void_p, channel_output: POINTER(c_int8), max_iterations: int = 50) -> bytearray:
    lifting = 384
    bg = 1
    decoding_rate = 13
    output_mode = 0

    raw_channel_output = (c_int8 * len(channel_output))(*(channel_output))
    raw_output = (c_char * _max_block_length)()

    result = _lib.decode(decoder, bg, lifting, decoding_rate, max_iterations, output_mode,
                         raw_channel_output, raw_output)

    return bytearray(result)
