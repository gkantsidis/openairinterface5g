"""
Helper to preload the windows libraries
"""

__all__ = ('open_air', 'create_decoder', 'delete_decoder', 'encode_simple', 'encode_full')

import logging
import os
from ctypes import WinDLL, c_void_p, c_int, POINTER, c_ubyte, c_byte, c_char, c_int8


def _wrap_function(lib, function_name, restype, argument_types):
    """Simplify wrapping ctypes functions"""
    func = lib.__getattr__(function_name)
    func.restype = restype
    func.argtypes = argument_types
    return func


if os.name == 'nt':
    try:
        basedir = os.path.dirname(__file__)
    except:
        pass
    else:
        libs_dir = os.path.abspath(os.path.join(basedir, '.libs'))
        target = 'OpenAirDll-vc90-release-amd64.dll'
        library = os.path.join(libs_dir, target)
        logging.info("Loading library %s from %s", target, library)
        open_air = WinDLL(os.path.abspath(library))

        create_decoder = _wrap_function(open_air, 'create_decoder', c_void_p, [])
        delete_decoder = _wrap_function(open_air, 'free_decoder', None, [c_void_p])

        encode_simple = _wrap_function(open_air, 'ldpc_encode_simple',
                                       c_int, [POINTER(c_ubyte), c_int, POINTER(c_ubyte), c_int])
        encode_full = _wrap_function(open_air, 'ldpc_encode_full',
                                     c_int, [POINTER(c_ubyte), c_int, POINTER(c_ubyte), c_int])

        decode = _wrap_function(open_air, 'ldpc_decode',
                                c_int, [c_void_p, c_int, c_int, c_int, c_int, c_int, POINTER(c_int8), POINTER(c_char)])
