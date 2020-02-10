"""
numpyhelper.py
Helper methods to work with numpy arrays
"""

__all__ = ('is_valid', 'get_addr_of_input_buffer', 'get_addr_of_encoder_buffer')

from numpy import ndarray
from .common import MAX_BLOCK_LENGTH, BUFFER_LENGTH


def is_valid(arr: ndarray) -> bool:
    if not isinstance(arr, ndarray):
        return False
    ai = arr.__array_interface__
    if ai["strides"]:
        return False
    if ai["version"] != 3:
        return False
    return True


def _check_and_get_array_address(arr: ndarray, dim: int, ty: str, rw: bool) -> int:
    if not isinstance(arr, ndarray):
        raise TypeError('Not ndarray')
    ai = arr.__array_interface__
    if ai["strides"]:
        raise TypeError("strided arrays not supported")
    if ai["version"] != 3:
        raise TypeError("only __array_interface__ version 3 supported")

    shape = ai["shape"]
    if not(shape == (dim, 1) or shape == (1,dim)):
        raise TypeError(f"Unexpected dimensions for array {shape}")
    item_type = ai['typestr']
    if item_type != ty:
        raise TypeError(f'Unexpected item type {item_type}')

    addr, readonly = ai["data"]
    if rw and readonly:
        raise TypeError('Attempting to access readonly array')
    return addr


def get_addr_of_input_buffer(arr: ndarray, rw: bool = False) -> int:
    return _check_and_get_array_address(arr, MAX_BLOCK_LENGTH, '|u1', rw)


def get_addr_of_encoder_buffer(arr: ndarray, rw: bool = False) -> int:
    return _check_and_get_array_address(arr, BUFFER_LENGTH, '|u1', rw)
