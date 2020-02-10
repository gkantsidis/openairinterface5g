"""
Helper to preload the windows libraries
"""

__all__ = ('open_air', 'create_decoder', 'delete_decoder', 'encode_simple', 'encode_full')

import logging
import os
import platform
from ctypes import c_void_p, c_int, c_int64, c_int32, POINTER, c_ubyte, c_byte, c_char, c_int8


logger = logging.getLogger('OpenLDPC')


def _wrap_function(lib, function_name, restype, argument_types):
    """Simplify wrapping ctypes functions"""
    func = lib.__getattr__(function_name)
    func.restype = restype
    func.argtypes = argument_types
    return func


_VC_COMPILER_VER = 'vc142'
_BUILD = 'release'

_architecture = platform.architecture()
if _architecture == ('64bit', 'WindowsPE'):
    _PLATFORM = 'amd64'
elif _architecture == ('32bit', 'WindowsPE'):
    _PLATFORM = 'x86'
elif _architecture == ('64bit', 'ELF'):
    _PLATFORM = 'amd64'
elif _architecture == ('64bit', ''):
    _PLATFORM = 'amd64'
else:
    raise SystemError('Platform not supported')

basedir = os.path.dirname(__file__)
libs_dir = os.path.abspath(os.path.join(basedir, '.libs'))

if os.name == 'nt':
    target = f'OpenAirDll-{_VC_COMPILER_VER}-{_BUILD}-{_PLATFORM}.dll'
    library = os.path.join(libs_dir, target)
    logging.info("Loading library %s from %s", target, library)

    try:
        from ctypes import WinDLL
        open_air = WinDLL(os.path.abspath(library))
    except Exception as ex:
        logger.fatal('Cannot load native library, error {}; aborting', ex)
        raise SystemError(f'Cannot load library; error: {ex}')

elif os.name == 'posix':
    target = f'libLDPC-x64.so'
    library = os.path.join(libs_dir, target)
    logging.info("Loading library %s from %s", target, library)

    try:
        from ctypes import cdll
        open_air = cdll.LoadLibrary(library)

    except Exception as ex:
        logger.fatal('Cannot load native library, error {}; aborting', ex)
        raise SystemError('Cannot load library')

else:
    logger.fatal('OS not support; aborting')
    raise SystemError('OS not supported')


try:
    create_decoder = _wrap_function(open_air, 'create_decoder', c_void_p, [])
    delete_decoder = _wrap_function(open_air, 'free_decoder', None, [c_void_p])

    encode_simple = _wrap_function(open_air, 'ldpc_encode_simple',
                                   c_int, [POINTER(c_ubyte), c_int, POINTER(c_ubyte), c_int])
    encode_full = _wrap_function(open_air, 'ldpc_encode_full',
                                 c_int, [POINTER(c_ubyte), c_int, POINTER(c_ubyte), c_int])

    decode = _wrap_function(open_air, 'ldpc_decode',
                            c_int, [c_void_p, c_int, c_int, c_int, c_int, c_int, POINTER(c_int8), POINTER(c_char)])

    if _PLATFORM == 'amd64':
        encode_full_raw = _wrap_function(open_air, 'ldpc_encode_full', c_int, [c_int64, c_int, c_int64, c_int])
        decode_raw = _wrap_function(open_air, 'ldpc_decode',
                                    c_int, [c_void_p, c_int, c_int, c_int, c_int, c_int, c_int64, c_int64])

    else:
        encode_full_raw = _wrap_function(open_air, 'ldpc_encode_full', c_int, [c_int32, c_int, c_int32, c_int])
        decode_raw = _wrap_function(open_air, 'ldpc_decode',
                                    c_int, [c_void_p, c_int, c_int, c_int, c_int, c_int, c_int32, c_int32])


except Exception as ex:
    logger.fatal('Cannot make bindings to native library, error {}; aborting', ex)
    raise SystemError('Cannot load library')