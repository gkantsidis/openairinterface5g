"""
OpenAirLDPC
===========

Provides an interface to access OpenLDPC's LDPC encoder and decoder.

"""

import logging

from . import _distributor_init
from . import errors
from . import encoder
from . import decoder
from . import rate

__all__ = ('encoder', 'decoder', 'errors')

logging.getLogger('OpenLDPC').addHandler(logging.NullHandler)
