"""
Unit tests for encoder functionality
"""

from .. import encoder


def test_encode_all_zero():
    """All zero buffers, return all zero encodings"""
    buffer = bytearray(1056)

    for i in range(len(buffer) - 1):
        buffer[i] = 0

    channel_in = encoder.encode(buffer)
    assert all(e == 0 for e in channel_in)
