"""
Interface to LDPC decoder
"""

__all__ = ('Decoder', 'DecoderError')

import logging
from ctypes import c_void_p, c_int8, c_char, c_char_p, pythonapi, cast, py_object, pointer, c_size_t
from typing import NamedTuple, Optional
from . import _distributor_init as _lib
from .errors import LdpcError
from .common import MAX_BLOCK_LENGTH, BUFFER_LENGTH

logger = logging.getLogger('OpenLDPC')


DecodeResult = NamedTuple('DecodeResult', [
    ('success', bool),
    ('iterations', int),
    ('decoded', bytearray)
])


class DecoderError(LdpcError):
    """Errors during decoding process"""
    pass


def get_data_ofs(buf):
    data = c_char_p()
    pythonapi.PyObject_AsCharBuffer(py_object(buf), pointer(data), pointer(c_size_t()))
    return cast(data, c_void_p).value


class Decoder:
    """Object that can be used for decoding"""

    __slots__ = ['__decoder', '__raw_output']

    def __init__(self):
        self.__decoder: Optional[c_void_p] = None
        self.__raw_output = (c_char * (8*MAX_BLOCK_LENGTH))()

    def __enter__(self):
        logger.info('Creating decoder')
        self.__decoder = c_void_p(_lib.create_decoder())
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        logger.info('Deleting decoder {}', self.__decoder)
        _lib.delete_decoder(self.__decoder.value)
        self.__decoder = None

    def decode(self, channel_output: bytes, max_iterations: int = 50) -> DecodeResult:
        """
        Decode a set of

        :param channel_output:
        :param max_iterations:
        :return:
        """

        assert self.__decoder is not None
        assert len(channel_output) == BUFFER_LENGTH

        # TODO: move decoder parameters to common
        lifting = 384
        bg = 1
        decoding_rate = 13
        output_mode = 0

        # Make a handle on the underlying bytes of the input buffer.
        raw_channel_output = (c_int8 * BUFFER_LENGTH).from_buffer(channel_output)

        logger.info("Calling decoder")
        iterations = _lib.decode(self.__decoder.value, bg, lifting, decoding_rate, max_iterations, output_mode,
                                 raw_channel_output, self.__raw_output)
        logger.info("Decoder done in %d iterations (%d)", iterations, max_iterations)

        success = iterations <= max_iterations

        decoded = bytearray(self.__raw_output[:MAX_BLOCK_LENGTH])
        return DecodeResult(success, iterations, decoded)
