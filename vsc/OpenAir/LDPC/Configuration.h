// <copyright file="Configuration.h" company="Microsoft Research Ltd">
// Copyright (c) 2019 Microsoft Research Ltd. All rights reserved.
// </copyright>
// <author>Christos Gkantsidis</author>
// <date>05/11/2019</date>
// <summary>A helper configuration class for accelerating testing of LDPC encoder.</summary>

#pragma once

#include <exception>
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
    /// <param name="nom_rate">             The nominator of the coding rate. </param>
    /// <param name="denom_rate">           The denominator of the coding rate. </param>
    ///
    /// <returns>   LDPC decoder configuration information. </returns>
    static Configuration MakeFromBlockLength(int block_length, int nom_rate, int denom_rate)
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("Length in bits should be positive");
        }
        if (block_length > MAX_LDPC_CONFIGURATION_SIZE)
        {
            throw std::invalid_argument("Length is too large");
        }

        auto block_length_in_bits = block_length * 8;
        int8_t bg;
        short kb;
        short nrows;
        short lift_size;

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

        lift_size = 0;
        for (int i = 0; i < 51; i++) {
            if (Configuration::_list_size_indices[i] >= (double)block_length_in_bits / kb) {
                lift_size = Configuration::_list_size_indices[i];
                break;
            }
        }

        return Configuration(bg, kb, nrows, lift_size, nom_rate, denom_rate);
    }

    /// <summary>   Gets the number of systematic columns. </summary>
    /// <returns>   Number of systematic columns. </returns>
    inline int Kb() const { return this->_kb; }

    /// <summary>   Gets the base graph. </summary>
    /// <returns>   The base graph id. </returns>
    inline short BG() const { return this->_bg; }

    /// <summary>   Gets the lifting factor. </summary>
    /// <returns>   The lifting factor. </returns>
    inline int Zc() const { return this->_lift_size; }

    /// <summary>   Gets the number of rows. </summary>
    /// <returns>   Number of rows. </returns>
    inline int Rows() const { return this->_nrows; }

    /// <summary>   Gets the nominator of the coding rate. </summary>
    /// <returns>   Nominator of coding rate. </returns>
    inline int Nom() const { return this->_nom_rate; }

    /// <summary>   Gets the denominator of the coding rate. </summary>
    /// <returns>   Denominator of coding rate. </returns>
    inline int Denom() const { return this->_denom_rate; }

    /// <summary>   Gets the code rate id. </summary>
    /// <returns>   Code rate id. </returns>
    inline int R() const { return Configuration::_code_rate_vec[this->R_ind()]; }

    /// <summary>   Number of punctured columns. </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   The total number of punctured columns. </returns>
    inline int NumberOfPuncturedColumns(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        auto block_length_in_bits = block_length * 8;
        auto first = (Rows() - 2) * Zc() + block_length_in_bits;
        auto second = float(block_length_in_bits) * float(_denom_rate) / float(_nom_rate);
        auto result = float(first) - second;
        result /= float(Zc());

        return (int)result;
    }

    /// <summary>   Number of removed bits. </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   The total number of removed bits. </returns>
    inline int NumberOfRemovedBits(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        auto no_punctured_columns = NumberOfPuncturedColumns(block_length);
        auto block_length_in_bits = 8 * block_length;
        auto first = (Rows() - no_punctured_columns - 2) * Zc() + block_length_in_bits;
        auto second = float(block_length_in_bits) * float(Denom()) / float(Nom());
        auto result = float(first) - second;
        return (int)result;
    }

    /// <summary>   Last bit position (observe that each transmitted bit is represented by one byte). </summary>
    /// <exception cref="std::invalid_argument">
    /// Thrown when an invalid argument error condition occurs.
    /// </exception>
    /// <param name="block_length"> The block length (in bytes). </param>
    /// <returns>   Position of last bit. </returns>
    inline int LastBitPosition(int block_length) const
    {
        if (block_length < 0)
        {
            throw std::invalid_argument("length cannot be negative");
        }

        auto no_punctured_columns = NumberOfPuncturedColumns(block_length);
        auto removed_bit = NumberOfRemovedBits(block_length);

        return (Kb() + Rows() - no_punctured_columns) * Zc() - removed_bit;
    }

    /// <summary>   Input channel start (observe that each transmitted bit is represented by one byte). </summary>
    /// <returns>   Index into the buffer of the encoding vector where to start transmission. </returns>
    inline int InputChannelStart() const
    {
        return 0;
    }

    /// <summary>   Input channel (non-inclusive) end (observe that each transmitted bit is represented by one byte). </summary>
    /// <param name="block_length">   The length (in bytes). </param>
    /// <returns>   The index of the last bit to transmit from the encoding. </returns>
    inline int InputChannelEnd(int block_length) const
    {
        return LastBitPosition(block_length) - 2 * _lift_size;
    }

    /// <summary>   Number of encoded bits to transmit. </summary>
    /// <param name="block_length">   Length of encoded message (in bytes). </param>
    /// <returns>   Number of encoded bits to transmit. </returns>
    inline int InputChannelCount(int block_length) const
    {
        return LastBitPosition(block_length) - 2 * _lift_size;
    }

    /// <summary>   Index of first bit to store received information for decoder input. </summary>
    /// <returns>   Index of first bit in decoder input buffer. </returns>
    inline int OutputChannelStart() const
    {
        return 2 * _lift_size;
    }

    /// <summary>   Index of last bit (non-inclusive) to store received information for decoder input. </summary>
    /// <param name="block_length">   The length of the encoded message (in bytes). </param>
    /// <returns>   An int. </returns>
    inline int OutputChannelEnd(int block_length) const
    {
        return this->OutputChannelStart() + this->OutputChannelCount(block_length);
    }

    /// <summary>   Number of bits in decoder input. </summary>
    /// <param name="block_length">   The length of the encoded message (in bytes). </param>
    /// <returns>   An int. </returns>
    inline int OutputChannelCount(int block_length) const
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
    /// <summary>   The nominator of the coding rate. </summary>
    int _nom_rate;
    /// <summary>   The denominator of the coding rate. </summary>
    int _denom_rate;

    /// <summary>   Initializes a new instance of the Configuration class. </summary>
    /// <param name="bg">           The base graph. </param>
    /// <param name="kb">           The kB. </param>
    /// <param name="nrows">        The number of rows. </param>
    /// <param name="lift_size">    Size of the lift. </param>
    /// <param name="nom_rate">     The nominator of the coding rate. </param>
    /// <param name="denom_rate">   The denominator of the coding rate. </param>

    Configuration(int8_t bg, short kb, short nrows, short lift_size, int nom_rate, int denom_rate)
        : _bg{ bg }

        /// <summary>   An enum constant representing the kB option. </summary>
        , _kb{ kb }

        /// <summary>   An enum constant representing the nrows option. </summary>
        , _nrows{ nrows }

        /// <summary>   An enum constant representing the lift size option. </summary>
        , _lift_size{ lift_size }

        /// <summary>   An enum constant representing the nom rate option. </summary>
        , _nom_rate{ nom_rate }

        /// <summary>   An enum constant representing the Denominator rate option. </summary>
        , _denom_rate{ denom_rate }
    {
    }

    /// <summary>   Gets the ind. </summary>
    /// <exception cref="std::runtime_error">
    /// Raised when a runtime error condition occurs.
    /// </exception>
    /// <returns>   An int. </returns>
    inline int R_ind() const {
        int R_ind = 0;

        auto nom = Nom();
        auto denom = Denom();

        if (nom == 1)
            if (denom == 5)
                if (BG() == 2)
                    R_ind = 0;
                else
                    throw std::runtime_error("Configuration not supported");
            else if (denom == 3)
                R_ind = 1;
            else if (denom == 2)
                //R_ind = 3;
                throw std::runtime_error("Configuration not supported");
            else
                throw std::runtime_error("Configuration not supported");

        else if (nom == 2)
            if (denom == 5)
                //R_ind = 2;
                throw std::runtime_error("Configuration not supported");
            else if (denom == 3)
                R_ind = 4;
            else
                throw std::runtime_error("Configuration not supported");

        else if ((nom == 22) && (denom == 30))
            //R_ind = 5;
            throw std::runtime_error("Configuration not supported");
        else if ((nom == 22) && (denom == 27))
            //R_ind = 6;
            throw std::runtime_error("Configuration not supported");
        else if ((nom == 22) && (denom == 25))
            if (BG() == 1)
                R_ind = 7;
            else
                throw std::runtime_error("Configuration not supported");
        else
            throw std::runtime_error("Configuration not supported");

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
    Configuration* configuration,
    int8_t* output,
    const int8_t* const input,
    int block_length
    )
{
    auto input_channel_start = configuration->InputChannelStart();
    auto input_channel_size = configuration->InputChannelCount(block_length);
    auto output_channel_start = configuration->OutputChannelStart();
    auto output_channel_size = configuration->OutputChannelCount(block_length);
    assert(input_channel_size == output_channel_size);

    const int8_t* channel_input = &(input[input_channel_start]);
    int8_t* channel_output = &(output[output_channel_start]);
    memcpy(channel_output, channel_input, output_channel_size * sizeof(int8_t));

    return channel_output;
}

