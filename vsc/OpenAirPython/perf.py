"""
perf.py

Performance testing of LDPC encoding and decoding
"""

import numpy as np
import time
from typing import NamedTuple
from OpenAirLDPC import common, encoder

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


def encode_normal(setup: ExperimentalSetup) -> PerformanceProfile:
    buffer = bytearray(common.MAX_BLOCK_LENGTH)

    for i in range(len(buffer) - 1):
        buffer[i] = i % 255

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


def _aggregate(profile: PerformanceProfile) -> ThroughputProfile:
    return ThroughputProfile(
        profile.total_operations / profile.total_time,
        profile.total_bytes / profile.total_time
    )


def main():
    setup = ExperimentalSetup(10000, 12)
    print(setup)
    # profile = encode_numpy(setup)
    profile = encode_normal(setup)
    print(profile)
    throughput = _aggregate(profile)
    print(throughput)


if __name__ == '__main__':
    main()