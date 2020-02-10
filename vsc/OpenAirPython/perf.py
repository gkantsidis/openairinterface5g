"""
perf.py

Performance testing of LDPC encoding and decoding
"""

import numpy as np
import time
from typing import NamedTuple
from OpenAirLDPC import common, encoder, decoder

ExperimentalSetup = NamedTuple('ExperimentalSetup',
                               [
                                   ('iterations', int),
                                   ('seed', int)
                               ])

PerformanceProfile = NamedTuple('PerformanceProfile',
                                [
                                    ('total_experiment_time', float),
                                    ('total_time', float),
                                    ('total_bytes', int),
                                    ('total_operations', int)
                                ])

ThroughputProfile = NamedTuple('ThroughputProfile',
                               [
                                   ('ops_rate', float),
                                   ('byte_rate', float)
                               ])


def _create_normal_determined_buffer() -> bytearray:
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

    return buffer


def _create_numpy_buffer(seed: int) -> np.ndarray:
    np.random.seed(seed)
    buffer = np.random.randint(1, 255, size=(common.MAX_BLOCK_LENGTH, 1), dtype=np.uint8)
    return buffer


def encode_normal(setup: ExperimentalSetup) -> PerformanceProfile:
    buffer = _create_normal_determined_buffer()
    total_time = 0.0
    start_all = time.monotonic()
    for i in range(setup.iterations):
        start = time.perf_counter()
        channel_in = encoder.encode(buffer)
        stop = time.perf_counter()
        if stop < start:
            raise Exception(f"Error in timing: stop={stop} < start={start}")
        total_time += stop - start
    end_all = time.monotonic()

    return PerformanceProfile(
        end_all - start_all,
        total_time,
        common.MAX_BLOCK_LENGTH * setup.iterations,
        setup.iterations)


def encode_numpy(setup: ExperimentalSetup) -> PerformanceProfile:
    np.random.seed(setup.seed)
    buffer = np.random.randint(1, 255, size=(common.MAX_BLOCK_LENGTH, 1), dtype=np.uint8)
    channel_in = np.ndarray(shape=(common.BUFFER_LENGTH, 1), dtype=np.uint8)

    total_time = 0.0
    start_all = time.monotonic()
    for i in range(setup.iterations):
        start = time.perf_counter()
        result = encoder.encode_numpy(buffer, channel_in)
        stop = time.perf_counter()
        if stop < start:
            raise Exception(f"Error in timing: stop={stop} < start={start}")
        total_time += stop - start
    end_all = time.monotonic()

    return PerformanceProfile(
        end_all - start_all,
        total_time,
        common.MAX_BLOCK_LENGTH * setup.iterations,
        setup.iterations)


def decode_normal(setup: ExperimentalSetup) -> PerformanceProfile:
    buffer = _create_normal_determined_buffer()
    channel_in = encoder.encode(buffer)
    decoder_input = bytearray(common.BUFFER_LENGTH)
    for i in range(common.BUFFER_LENGTH - 1):
        decoder_input[i] = 0x7F if channel_in[i] == 0x00 else 0x80

    with decoder.Decoder() as decoder_object:
        total_time = 0.0
        start_all = time.monotonic()
        for i in range(setup.iterations):
            start = time.perf_counter()
            output = decoder_object.decode(decoder_input)
            stop = time.perf_counter()
            if stop < start:
                raise Exception(f"Error in timing: stop={stop} < start={start}")
            total_time += stop - start
        end_all = time.monotonic()

    return PerformanceProfile(
        end_all - start_all,
        total_time,
        common.MAX_BLOCK_LENGTH * setup.iterations,
        setup.iterations)


def decode_numpy(setup: ExperimentalSetup) -> PerformanceProfile:
    buffer = _create_numpy_buffer(setup.seed)
    channel_in = np.ndarray(shape=(common.BUFFER_LENGTH, 1), dtype=np.uint8)
    encoder.encode_numpy(buffer, channel_in)
    decoder_input = np.zeros(shape=(common.BUFFER_LENGTH, 1), dtype=np.int8)
    for i in range(common.BUFFER_LENGTH - 1):
        decoder_input[i] = 0x7F if channel_in[i] == 0x00 else 0x80

    output = np.ndarray(shape=(common.BUFFER_LENGTH, 1), dtype=np.uint8)
    with decoder.Decoder() as decoder_object:
        total_time = 0.0
        start_all = time.monotonic()
        for i in range(setup.iterations):
            start = time.perf_counter()
            (success, iterations) = decoder_object.decode_numpy(decoder_input, output)
            stop = time.perf_counter()
            if stop < start:
                raise Exception(f"Error in timing: stop={stop} < start={start}")
            total_time += stop - start
        end_all = time.monotonic()

    return PerformanceProfile(
        end_all - start_all,
        total_time,
        common.MAX_BLOCK_LENGTH * setup.iterations,
        setup.iterations)


def _aggregate(profile: PerformanceProfile) -> ThroughputProfile:
    return ThroughputProfile(
        profile.total_operations / profile.total_time,
        profile.total_bytes / profile.total_time
    )


def main():
    setup = ExperimentalSetup(10000, 12)
    print(setup)
    # profile = encode_numpy(setup)
    # profile = encode_normal(setup)
    # profile = decode_normal(setup)
    profile = decode_numpy(setup)
    print(profile)
    throughput = _aggregate(profile)
    print(throughput)


if __name__ == '__main__':
    main()