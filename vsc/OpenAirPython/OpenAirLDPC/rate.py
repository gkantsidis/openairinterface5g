"""
Helper routines to decide bits to transmit based on desired rate
"""

__all__ = ('BaseGraph', 'CodeRate', 'Rate_1_3', 'heuristic_rate')

import logging
from enum import Enum
from math import ceil
from typing import List
from .common import MAX_BLOCK_LENGTH, BUFFER_LENGTH

logger = logging.getLogger('OpenLDPC')

_lift_size = (2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24,
              26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112,
              120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384)

_code_rate = (15, 13, 25, 12, 23, 34, 56, 89)


class BaseGraph(Enum):
    BaseGraph1 = 1
    BaseGraph2 = 2


class CodeRate:
    __slots__ = ('__base_graph', '__systematic_columns', '__number_of_rows', '__lift_size',
                 '__numerator', '__denominator', '__block_length', '__block_length_in_bits')

    def __init__(self, block_length: int, numerator_rate: int, denominator_rate: int):
        """
        Provides ranges for the coding rate

        :param block_length: Block length (in bytes)
        :param numerator_rate: Nominator of coding rate
        :param denominator_rate: Denominator of coding rate
        """

        if block_length < 0:
            logger.error('Negative block length; aborting')
            raise ValueError(f'Block length cannot be negative')
        if block_length > MAX_BLOCK_LENGTH:
            logger.error('Very large block length; aborting')
            raise ValueError(f'Block length must be <= {MAX_BLOCK_LENGTH}, it is {block_length}')

        self.__numerator = numerator_rate
        self.__denominator = denominator_rate

        block_length_in_bits = block_length * 8
        self.__block_length = block_length
        self.__block_length_in_bits = block_length_in_bits

        if block_length_in_bits > 3840:
            self.__base_graph = BaseGraph.BaseGraph1
            self.__systematic_columns = 22
            self.__number_of_rows = 46
        else:
            self.__base_graph = BaseGraph.BaseGraph2
            self.__number_of_rows = 42

            if block_length_in_bits > 640:
                self.__systematic_columns = 10
            elif block_length_in_bits > 560:
                self.__systematic_columns = 9
            elif block_length_in_bits > 192:
                self.__systematic_columns = 8
            else:
                self.__systematic_columns = 6

        self.__lift_size = 0
        for lift_size in _lift_size:
            if lift_size >= float(block_length_in_bits) / self.__systematic_columns:
                self.__lift_size = lift_size
                break

    @property
    def base_graph(self) -> BaseGraph:
        return self.__base_graph

    @property
    def kb(self) -> int:
        return self.__systematic_columns

    @property
    def code_rate(self) -> int:
        if self.__numerator == 1:
            if self.__denominator == 5:
                if self.__base_graph == 2:
                    return _code_rate[0]
                else:
                    raise NotImplementedError()

            elif self.__denominator == 3:
                return _code_rate[1]
            elif self.__denominator == 2:
                return _code_rate[3]
            else:
                raise NotImplementedError()

        elif self.__numerator == 2:
            if self.__denominator == 5:
                return _code_rate[2]
            elif self.__denominator == 3:
                return _code_rate[4]
            else:
                raise NotImplementedError()

        elif self.__numerator == 22 and self.__denominator == 30:
            return _code_rate[5]

        elif self.__numerator == 22 and self.__denominator == 27:
            return _code_rate[6]

        elif self.__numerator == 22 and self.__denominator == 25:
            if self.base_graph == BaseGraph.BaseGraph1:
                return _code_rate[7]
            else:
                raise NotImplementedError()

        else:
            raise NotImplementedError()

    @property
    def _number_of_punctured_columns(self) -> int:
        first = (self.__number_of_rows - 2) * self.__lift_size + self.__block_length_in_bits
        second = self.__block_length_in_bits * float(self.__denominator) / float(self.__numerator)
        result = first - second
        result = result / self.__lift_size
        return int(result)

    @property
    def _number_of_removed_bits(self) -> int:
        first = (self.__number_of_rows - self._number_of_punctured_columns - 2) * self.__lift_size + \
                self.__block_length_in_bits
        second = float(self.__block_length_in_bits) * float(self.__denominator) / float(self.__numerator)
        result = first - second
        return int(result)

    @property
    def _last_bit_position(self):
        first = (self.__systematic_columns + self.__number_of_rows - self._number_of_punctured_columns) * \
                self.__lift_size
        return first - self._number_of_removed_bits

    @property
    def range_to_use(self) -> List[int]:
        return list(range(2 * self.__lift_size, 2 * self.__lift_size + self._last_bit_position))


Rate_1_3 = CodeRate(MAX_BLOCK_LENGTH, 1, 3)
Rate_2_3 = CodeRate(MAX_BLOCK_LENGTH, 2, 3)


__typical_starting_point = 768


def heuristic_rate(code_rate: float) -> List[int]:
    if code_rate <= 0.0 or code_rate > 1.0:
        logger.error('Invalid code rate %5.3f, it should be between 0.0 and 1.0', code_rate)
        raise ValueError('Invalid code rate')

    bits_to_transmit = ceil(float(MAX_BLOCK_LENGTH * 8) / float(code_rate))
    if __typical_starting_point + bits_to_transmit > BUFFER_LENGTH:
        return list(range(BUFFER_LENGTH - bits_to_transmit, BUFFER_LENGTH))
    else:
        return list(range(__typical_starting_point, __typical_starting_point + bits_to_transmit))
