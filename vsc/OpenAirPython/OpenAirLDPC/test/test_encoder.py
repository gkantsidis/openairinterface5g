"""
Unit tests for encoder functionality
"""

from .. import encoder, common


def test_encode_all_zero():
    """All zero buffers, return all zero encodings"""
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = 0

    channel_in = encoder.encode(buffer)
    assert all(e == 0 for e in channel_in)


def test_encode_not_all_zero():
    """Input contains non-zero elements, result must not be all zero, otherwise we are not passing the correct buffer"""
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    channel_in = encoder.encode(buffer)
    assert all(e == 0 for e in channel_in) is False
