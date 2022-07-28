// <copyright file="Configuration.h" company="Microsoft Research Ltd">
// Copyright (c) 2019 Microsoft Research Ltd. All rights reserved.
// </copyright>
// <author>Christos Gkantsidis</author>
// <date>05/11/2019</date>
// <summary>A helper configuration class for accelerating testing of LDPC encoder.</summary>

// ReSharper disable CppClangTidyBugproneBranchClone
#pragma once

#include <stdexcept>
#include <cassert>

/// <summary>   The maximum size of buffer that can be encoded/decoded with this LDPC implementation. </summary>
static constexpr int MAX_LDPC_CONFIGURATION_SIZE = 1056;

/// <summary>   Configuration class. </summary>
class Configuration
{
public:

    /// <summary>   Makes default configuration from block length. </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length">         The block length (in bytes). </param>
    /// <param name="numerator_rate">             The numerator of the coding rate. </param>
    /// <param name="denominator_rate">           The denominator of the coding rate. </param>
    ///
    /// <returns>   LDPC decoder configuration information. </returns>
    static Configuration MakeFromBlockLength(int block_length, int numerator_rate, int denominator_rate)
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("Length in bits should be positive");
        }
        if (block_length > MAX_LDPC_CONFIGURATION_SIZE)
        {
            throw std::invalid_argument("Length is too large");
        }

        const auto block_length_in_bits = block_length * 8;
        int8_t bg;
        short kb;
        short nrows;

        if (block_length_in_bits > 3840)
        {
            bg = 1;
            kb = 22;
            nrows = 46;
        }
        else
        {
            bg = 2;
            nrows = 42;

            if (block_length_in_bits > 640)
            {
                kb = 10;
            }
            else if (block_length_in_bits > 560)
            {
                kb = 9;
            }
            else if (block_length_in_bits > 192)
            {
                kb = 8;
            }
            else
            {
                kb = 6;
            }
        }

        short lift_size = 0;
        for (int i = 0; i < 51; i++) {  // NOLINT(modernize-loop-convert)
            if (_list_size_indices[i] >= static_cast<double>(block_length_in_bits) / kb) {
                lift_size = _list_size_indices[i];
                break;
            }
        }

        return {bg, kb, nrows, lift_size, numerator_rate, denominator_rate};
    }

    /// <summary>   Gets the number of systematic columns. </summary>
    /// <returns>   Number of systematic columns. </returns>
    [[nodiscard]] int Kb() const { return this->_kb; }

    /// <summary>   Gets the base graph. </summary>
    /// <returns>   The base graph id. </returns>
    [[nodiscard]] short BG() const { return this->_bg; }

    /// <summary>   Gets the lifting factor. </summary>
    /// <returns>   The lifting factor. </returns>
    [[nodiscard]] int Zc() const { return this->_lift_size; }

    /// <summary>   Gets the number of rows. </summary>
    /// <returns>   Number of rows. </returns>
    [[nodiscard]] int Rows() const { return this->_nrows; }

    /// <summary>   Gets the numerator of the coding rate. </summary>
    /// <returns>   Numerator of coding rate. </returns>
    [[nodiscard]] int Numerator() const { return this->_numerator_rate; }

    /// <summary>   Gets the denominator of the coding rate. </summary>
    /// <returns>   Denominator of coding rate. </returns>
    [[nodiscard]] int Denominator() const { return this->_denominator_rate; }

    /// <summary>   Gets the code rate id. </summary>
    /// <returns>   Code rate id. </returns>
    [[nodiscard]] int R() const { return Configuration::_code_rate_vec[this->R_ind()]; }

    /// <summary>   Number of punctured columns. </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   The total number of punctured columns. </returns>
    [[nodiscard]] int NumberOfPuncturedColumns(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        const auto block_length_in_bits = block_length * 8;
        const auto first = (Rows() - 2) * Zc() + block_length_in_bits;
        const auto second = static_cast<float>(block_length_in_bits) * static_cast<float>(_denominator_rate) / static_cast<float>(_numerator_rate);
        auto result = static_cast<float>(first) - second;
        result /= static_cast<float>(Zc());

        return static_cast<int>(result);
    }

    /// <summary>   Number of removed bits. </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   The total number of removed bits. </returns>
    [[nodiscard]] int NumberOfRemovedBits(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        const auto no_punctured_columns = NumberOfPuncturedColumns(block_length);
        const auto block_length_in_bits = 8 * block_length;
        const auto first = (Rows() - no_punctured_columns - 2) * Zc() + block_length_in_bits;
        const auto second = static_cast<float>(block_length_in_bits) * static_cast<float>(Denominator()) / static_cast<float>(Numerator());
        const auto result = static_cast<float>(first) - second;
        return static_cast<int>(result);
    }

    /// <summary>   Last bit position (observe that each transmitted bit is represented by one byte). </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   Position of last bit. </returns>
    [[nodiscard]] int LastBitPosition(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        const auto no_punctured_columns = NumberOfPuncturedColumns(block_length);
        const auto removed_bit = NumberOfRemovedBits(block_length);

        return (Kb() + Rows() - no_punctured_columns) * Zc() - removed_bit;
    }

    /// <summary>   Input channel start (observe that each transmitted bit is represented by one byte). </summary>
    /// <returns>   Index into the buffer of the encoding vector where to start transmission. </returns>
    // ReSharper disable once CppMemberFunctionMayBeStatic
    [[nodiscard]] int InputChannelStart() const
    {
        return 0;
    }

    /// <summary>   Input channel (non-inclusive) end (observe that each transmitted bit is represented by one byte). </summary>
    /// <param name="block_length">   The length (in bytes). </param>
    /// <returns>   The index of the last bit to transmit from the encoding. </returns>
    [[nodiscard]] int InputChannelEnd(int block_length) const
    {
        return LastBitPosition(block_length) - 2 * _lift_size;
    }

    /// <summary>   Number of encoded bits to transmit. </summary>
    /// <param name="block_length">   Length of encoded message (in bytes). </param>
    /// <returns>   Number of encoded bits to transmit. </returns>
    [[nodiscard]] int InputChannelCount(int block_length) const
    {
        return LastBitPosition(block_length) - 2 * _lift_size;
    }

    /// <summary>   Index of first bit to store received information for decoder input. </summary>
    /// <returns>   Index of first bit in decoder input buffer. </returns>
    [[nodiscard]] int OutputChannelStart() const
    {
        return 2 * _lift_size;
    }

    /// <summary>   Index of last bit (non-inclusive) to store received information for decoder input. </summary>
    /// <param name="block_length">   The length of the encoded message (in bytes). </param>
    /// <returns>   An int. </returns>
    [[nodiscard]] int OutputChannelEnd(int block_length) const
    {
        return this->OutputChannelStart() + this->OutputChannelCount(block_length);
    }

    /// <summary>   Number of bits in decoder input. </summary>
    /// <param name="block_length">   The length of the encoded message (in bytes). </param>
    /// <returns>   An int. </returns>
    [[nodiscard]] int OutputChannelCount(int block_length) const
    {
        return LastBitPosition(block_length) - 2 * _lift_size;
    }

private:
    /// <summary>   The list size indices[]. </summary>
    static constexpr short _list_size_indices[]{
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24,
            26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112,
            120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
    };

    /// <summary>   Code that corresponds to coding rates. </summary>
    static constexpr int _code_rate_vec[] {
        15, 13, 25, 12, 23, 34, 56, 89
    };

    /// <summary>   The base graph. </summary>
    int8_t _bg;
    /// <summary>   The kB. </summary>
    short _kb;
    /// <summary>   The number of rows. </summary>
    short _nrows;
    /// <summary>   Size of the lift. </summary>
    short _lift_size;
    /// <summary>   The numerator of the coding rate. </summary>
    int _numerator_rate;
    /// <summary>   The denominator of the coding rate. </summary>
    int _denominator_rate;

    /// <summary>   Initializes a new instance of the Configuration class. </summary>
    /// <param name="bg">           The base graph. </param>
    /// <param name="kb">           The kB. </param>
    /// <param name="nrows">        The number of rows. </param>
    /// <param name="lift_size">    Size of the lift. </param>
    /// <param name="numerator_rate">     The numerator of the coding rate. </param>
    /// <param name="denominator_rate">   The denominator of the coding rate. </param>

    Configuration(int8_t bg, short kb, short nrows, short lift_size, int numerator_rate, int denominator_rate)
        : _bg{ bg }

        /// <summary>   An enum constant representing the kB option. </summary>
        , _kb{ kb }

        /// <summary>   An enum constant representing the nrows option. </summary>
        , _nrows{ nrows }

        /// <summary>   An enum constant representing the lift size option. </summary>
        , _lift_size{ lift_size }

        /// <summary>   An enum constant representing the nom rate option. </summary>
        , _numerator_rate{ numerator_rate }

        /// <summary>   An enum constant representing the Denominator rate option. </summary>
        , _denominator_rate{ denominator_rate }
    {
    }

    /// <summary>   Gets the ind. </summary>
    /// <exception cref="std::runtime_error">
    /// Raised when a runtime error condition occurs.
    /// </exception>
    /// <returns>   An int. </returns>
    [[nodiscard]] inline int R_ind() const {
	    // ReSharper disable once CppInconsistentNaming
	    int R_ind;

        const auto numerator = Numerator();
        const auto denominator = Denominator();

        if (numerator == 1) {
            if (denominator == 5) {
                if (BG() == 2)
                    R_ind = 0;
                else
                    throw std::runtime_error("Configuration not supported");
            }
            else if (denominator == 3) {
                R_ind = 1;
            }
            else if (denominator == 2) {
                //R_ind = 3;
                throw std::runtime_error("Configuration not supported");
            }
            else {
                throw std::runtime_error("Configuration not supported");
            }
        }
        else if (numerator == 2) {
            if (denominator == 5) {
                //R_ind = 2;
                throw std::runtime_error("Configuration not supported");
            }
            else if (denominator == 3) {
                R_ind = 4;
            }
            else {
                throw std::runtime_error("Configuration not supported");
            }
        }
        else if ((numerator == 22) && (denominator == 30)) {
            //R_ind = 5;
            throw std::runtime_error("Configuration not supported");
        }
        else if ((numerator == 22) && (denominator == 27)) {
            //R_ind = 6;
            throw std::runtime_error("Configuration not supported");
        }
        else if ((numerator == 22) && (denominator == 25)) {
            if (BG() == 1) {
                R_ind = 7;
            }
            else {
                throw std::runtime_error("Configuration not supported");
            }
        }
        else {
            throw std::runtime_error("Configuration not supported");
        }

        return R_ind;
    }
};

/// <summary>   Ideal channel transfer. </summary>
/// <param name="configuration">            [in,out] If non-null, the configuration. </param>
/// <param name="output">                   [in,out] If non-null, the output. </param>
/// <param name="input">                    The input. </param>
/// <param name="block_length">             The output length (in bytes). </param>
/// <returns>   Null if it fails, else a pointer to an int8_t. </returns>
inline int8_t * ideal_channel_transfer(
	const Configuration* configuration,
    int8_t* output,
    const int8_t* const input,
	const int block_length
    )
{
	const auto input_channel_start = configuration->InputChannelStart();
    const auto input_channel_size = configuration->InputChannelCount(block_length);
    const auto output_channel_start = configuration->OutputChannelStart();
    const auto output_channel_size = configuration->OutputChannelCount(block_length);

    //Note: The following code is used for testing.
    //constexpr auto input_channel_start = 0;
    //const auto input_channel_size = 68 * 384 - 2 * configuration->Zc();
    //const auto output_channel_start = 2 * configuration->Zc();
    //const auto output_channel_size = 68 * 384 - 2 * configuration->Zc();

    assert(input_channel_size == output_channel_size);

    const int8_t* channel_input = &(input[input_channel_start]);
    int8_t* channel_output = &(output[output_channel_start]);
    memcpy(channel_output, channel_input, output_channel_size * sizeof(int8_t));

    return channel_output;
}

