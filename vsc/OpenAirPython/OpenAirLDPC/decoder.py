"""
Interface to LDPC decoder
"""

__all__ = ('Decoder', 'DecoderError')

import logging
from ctypes import c_void_p, c_int8, c_char, c_char_p, pythonapi, cast, py_object, pointer, c_size_t
from typing import NamedTuple, Optional, Tuple
from numpy import ndarray
from . import _distributor_init as _lib
from . import _numpyhelper as nph
from .errors import LdpcError
from .common import MAX_BLOCK_LENGTH, BUFFER_LENGTH, BASE_GRAPH_1

logger = logging.getLogger('OpenLDPC')


DecodeResult = NamedTuple('DecodeResult', [
    ('success', bool),
    ('iterations', int),
    ('decoded', bytearray)
])


class DecoderError(LdpcError):
    """Errors during decoding process"""


def get_data_ofs(buf):
    """
    Retrieve the pointer of the data of a buffer (?)
    :param buf: Input object
    :return: Address of data
    """
    # TODO: Fix documentation above
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
        logger.info('Deleting decoder %s', str(self.__decoder))
        _lib.delete_decoder(self.__decoder.value)
        self.__decoder = None

    def decode(self, channel_output: bytearray, max_iterations: int = 50) -> DecodeResult:
        """
        Decode a set of input LLRs from values received.

        :param channel_output: The LLRs per each bit. The array needs to be of size BUFFER_LENGTH,
        and have zero values for those bits for which there is no information, and a values from -128 to 127
        expressing the belief of receiving 1 (very negative numbers) or 0 (very positive numbers).
        :param max_iterations: Maximum iterations to run
        :return: A structure that contains a signal of termination before limit, number of iterations,
        and decoded bytes.
        """

        # TODO: The channel_output should be immutable (bytes).
        # Ideally, the channel_output should be a bytes vector, signalling a promise to not change it.
        # This is true since the C code does not change it. However, if we mark it as bytes, the code
        # below will fail.

        assert self.__decoder is not None
        assert len(channel_output) == BUFFER_LENGTH

        # TODO: move decoder parameters to common
        lifting = 384
        base_graph = BASE_GRAPH_1
        decoding_rate = 13
        output_mode = 0

        # Make a handle on the underlying bytes of the input buffer.
        raw_channel_output = (c_int8 * BUFFER_LENGTH).from_buffer(channel_output)

        logger.info("Calling decoder")
        iterations = _lib.decode(self.__decoder.value, base_graph, lifting, decoding_rate, max_iterations, output_mode,
                                 raw_channel_output, self.__raw_output)
        logger.info("Decoder done in %d iterations (%d)", iterations, max_iterations)

        success = iterations <= max_iterations

        decoded = bytearray(self.__raw_output[:MAX_BLOCK_LENGTH])
        return DecodeResult(success, iterations, decoded)

    def decode_numpy(self, llr: ndarray, decoded: ndarray, max_iterations: int = 50) -> Tuple[bool, int]:
        """
        Decode using pre-allocated numpy arrays.
        :param llr: Vector of LLRs
        :param decoded: Vector where to store output
        :param max_iterations: Maximum iterations to run
        :return: Tuple of success (bool) and number of iterations.
        """
        assert self.__decoder is not None
        assert len(llr) == BUFFER_LENGTH

        lifting = 384
        base_graph = BASE_GRAPH_1
        decoding_rate = 13
        output_mode = 0

        llr_addr = nph.get_addr_of_llr(llr)
        decoded_addr = nph.get_addr_of_decoder_output(decoded, rw=True)

        logger.info("Calling decoder")
        iterations = _lib.decode_raw(self.__decoder.value, base_graph, lifting, decoding_rate, max_iterations,
                                     output_mode, llr_addr, decoded_addr)
        logger.info("Decoder done in %d iterations (%d)", iterations, max_iterations)

        success = iterations <= max_iterations
        return success, iterations
