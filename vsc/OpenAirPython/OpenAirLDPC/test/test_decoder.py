"""
Unit tests for decoder functionality
"""

import random
import math
from .. import decoder, common, encoder, rate


def test_create_and_destroy_decoder():
    """Test correct creation and destruction of decoder objects"""
    with decoder.Decoder() as decoder_object:
        pass


def test_all_zero():
    """Test sending all zero buffer without errors"""
    decoder_input = bytearray(common.BUFFER_LENGTH)
    for i in range(common.BUFFER_LENGTH - 1):
        decoder_input[i] = 0x7F

    with decoder.Decoder() as decoder_object:
        output = decoder_object.decode(decoder_input)

    assert output.success
    assert all(e == 0 for e in output.decoded)


def test_determined():
    """Test sending a predetermined sequence with no errors"""

    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    channel_in = encoder.encode(buffer)

    decoder_input = bytearray(common.BUFFER_LENGTH)
    for i in range(common.BUFFER_LENGTH - 1):
        decoder_input[i] = 0x7F if channel_in[i] == 0x00 else 0x80

    with decoder.Decoder() as decoder_object:
        output = decoder_object.decode(decoder_input)

    assert output.success

    for i in range(len(buffer)-1):
        assert buffer[i] == output.decoded[i], f'Error in {i}, expected {buffer[i]}, got {output.decoded[i]}'


def test_determined_truncated():
    """Test sending a predetermined sequence with no errors"""

    to_transmit = 10000   # first bits
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    channel_in = encoder.encode(buffer)

    decoder_input = bytearray(common.BUFFER_LENGTH)
    for i in range(common.BUFFER_LENGTH - 1):
        if i < to_transmit:
            decoder_input[i] = 0x7F if channel_in[i] == 0x00 else 0x80
        else:
            decoder_input[i] = 0x00

    with decoder.Decoder() as decoder_object:
        output = decoder_object.decode(decoder_input)

    assert output.success

    for i in range(len(buffer)-1):
        assert buffer[i] == output.decoded[i], f'Error in {i}, expected {buffer[i]}, got {output.decoded[i]}'

    assert len(buffer) == len(output.decoded)


def test_determined_truncated_to_086():
    """Test sending a predetermined sequence with no errors"""

    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    channel_in = encoder.encode(buffer)

    decoder_input = bytearray(common.BUFFER_LENGTH)
    for i in rate.heuristic_rate(0.86):
        decoder_input[i] = 0x7F if channel_in[i] == 0x00 else 0x80

    with decoder.Decoder() as decoder_object:
        output = decoder_object.decode(decoder_input)

    assert output.success

    for i in range(len(buffer)-1):
        assert buffer[i] == output.decoded[i], f'Error in {i}, expected {buffer[i]}, got {output.decoded[i]}'

    assert len(buffer) == len(output.decoded)


def test_bit_flips_truncated_to_086():
    """Test sending a predetermined sequence with no errors"""

    errors = 200
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    channel_in = encoder.encode(buffer)

    decoder_input = bytearray(common.BUFFER_LENGTH)
    to_transmit = rate.heuristic_rate(0.86)

    random.seed(10)
    erroneous = random.sample(to_transmit, errors)
    for error_position in erroneous:
        if channel_in[error_position] == 0:
            # Encode with 0x2 an error where a value of 0 is transmitted as 1
            channel_in[error_position] = 0x2
        else:
            # Encode with 0x3 an error where a value of 1 is transmitted as 0
            channel_in[error_position] = 0x3

    for i in to_transmit:
        if channel_in[i] == 0x00:
            decoder_input[i] = 0x7F
        elif channel_in[i] == 0x01:
            decoder_input[i] = 0x80
        elif channel_in[i] == 0x02:
            # Inject error here: 0 is transmitted as 1
            decoder_input[i] = 0xB0
        elif channel_in[i] == 0x03:
            # Inject error here: 1 is transmitted as 0
            decoder_input[i] = 0x4F
        else:
            assert False, "Invalid value"

    with decoder.Decoder() as decoder_object:
        output = decoder_object.decode(decoder_input)

    # assert output.success

    for i in range(len(buffer)-1):
        assert buffer[i] == output.decoded[i], f'Error in {i}, expected {buffer[i]}, got {output.decoded[i]}'

    assert len(buffer) == len(output.decoded)
