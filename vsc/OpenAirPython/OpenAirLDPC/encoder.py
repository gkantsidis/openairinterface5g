"""
Interface to LDPC's encoder functionality
"""

__all__ = ('encode', 'EncoderError')

import logging
from ctypes import c_ubyte
from numpy import ndarray
from . import _distributor_init as _lib
from . import _numpyhelper as nph
from .errors import LdpcError
from .common import MAX_BLOCK_LENGTH, BUFFER_LENGTH, BASE_GRAPH_1

logger = logging.getLogger('OpenLDPC')


class EncoderError(LdpcError):
    """Errors during encoding process"""
    pass


def encode(buffer: bytes) -> bytearray:
    """
    Encoded a set of bytes
    :param buffer: Input buffer to encode (must be of common.MAX_BLOCK_LENGTH size)
    :return: A (byte) buffer of (common.BUFFER_LENGTH) 0 and 1 values
    """

    length = len(buffer)
    assert length == MAX_BLOCK_LENGTH
    raw_buffer = (c_ubyte*len(buffer))(*(buffer))
    raw_output = (c_ubyte*BUFFER_LENGTH)()
    result = _lib.encode_full(raw_buffer, length, raw_output, BASE_GRAPH_1)
    if result < 0:
        logger.error("Encoder returned value %d", result)
        raise(EncoderError(f'Encoder returned {result}'))

    return bytearray(raw_output)


def encode_in_place(buffer: bytes, channel_input: bytearray) -> int:
    """
    Encode a set of bytes using as output an existing buffer

    :param buffer: Input buffer to encode (must be of common.MAX_BLOCK_LENGTH size
    :param channel_input: Output buffer (where to store common.BUFFER_LENGTH 0 and 1 values
    :return: 0 if success, error code otherwise
    """

    assert len(buffer) == MAX_BLOCK_LENGTH
    assert len(channel_input) >= BUFFER_LENGTH

    # TODO: Implement in place encoding
    raise NotImplementedError()


def encode_numpy(buffer: ndarray, channel_input: ndarray) -> int:
    """
    Encode using existing numpy arrays

    :param buffer: The input array (bytes to encode)
    :param channel_input: The input to the channel. Every byte in this array is a bit and gets values 0 or 1.
    :return: Result of encoding
    """
    length = len(buffer)
    assert length == MAX_BLOCK_LENGTH

    buffer_addr = nph.get_addr_of_input_buffer(buffer)
    channel_addr = nph.get_addr_of_encoder_buffer(channel_input, rw=True)
    # print(f'Buffer={hex(buffer_addr)}, Channel={hex(channel_addr)}')
    result = _lib.encode_full_raw(buffer_addr, length, channel_addr, BASE_GRAPH_1)
    return result
