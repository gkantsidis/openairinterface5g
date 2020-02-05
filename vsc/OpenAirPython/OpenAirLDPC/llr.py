"""
Helper routines to deal with constructing and using LLRs
"""

import logging
from math import sqrt, log10, e
from typing import List

logger = logging.getLogger('OpenLDPC')


def llr_to_decoder_input(sigma: float, llr: List[float]) -> List[float]:
    """
    Assuming that the caller has already computed a set of LLRs,
    this method scales them before sending them to the decoder.
    This scaling assumes a binomial AWGN channel and uses the
    mapping from the ldpc_test project in the repo.

    :param sigma: The estimated standard deviation of noise
    :param llr: The list of computed LLRs
    :return: LLRs to pass to decoder
    """

    # TODO: verify this model

    # We want to compute: (16*sqrt(2)/loge) * sigma * LLR
    constant = 16.0 * sqrt(2) / log10(e)
    return [constant * sigma * x for x in llr]
