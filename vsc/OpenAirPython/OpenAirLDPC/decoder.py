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

    __slots__ = '__decoder'

    def __init__(self):
        __decoder: Optional[c_void_p] = None

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

        # TODO: Avoid buffer copies in decoder
        # It would be ideal to avoid buffer copies in decoder. In particular, this seems possible
        # for the input channel_output. For the output (assuming an in-place implementation),
        # it may be tricky because the C library expects a buffer 8 times the size of the input.

        # TODO: move decoder parameters to common
        lifting = 384
        bg = 1
        decoding_rate = 13
        output_mode = 0

        # TODO: move raw_channel_output and raw_output to common code
        # There is no reason to construct those buffers for every call.
        # However, Python does not want to allow them as attributes to the class.
        raw_channel_output = (c_int8 * BUFFER_LENGTH)()
        for i in range(len(channel_output) - 1):
            raw_channel_output[i] = channel_output[i]

        # Observe that we need number of bits in the buffer given to decoder.
        raw_output = (c_char * (8*MAX_BLOCK_LENGTH))()

        logger.info("Calling decoder")
        iterations = _lib.decode(self.__decoder.value, bg, lifting, decoding_rate, max_iterations, output_mode,
                                 raw_channel_output, raw_output)
        logger.info("Decoder done in %d iterations (%d)", iterations, max_iterations)

        success = iterations <= max_iterations

        decoded = bytearray(raw_output)
        decoded = decoded[:MAX_BLOCK_LENGTH]
        return DecodeResult(success, iterations, decoded)
