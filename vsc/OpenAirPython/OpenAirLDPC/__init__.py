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

__all__ = ('encoder', 'decoder', 'errors')

logging.getLogger(__name__).addHandler(logging.NullHandler)
