"""
Base for errors thrown in this module
"""

__all__ = 'LdpcError'


from abc import ABC


class LdpcError(Exception, ABC):
    """
    Common exception for all module related exceptions
    """
