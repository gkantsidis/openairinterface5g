"""
Unit tests for decoder functionality
"""

# import pytest
from .. import decoder


def test_create_and_destroy_decoder():
    """Test correct creation and destruction of decoder objects"""
    decoder_object = decoder.create_decoder()
    decoder.delete_decoder(decoder_object)
