// <copyright file="LDPC.h" company="Microsoft Research Ltd">
// Copyright (c) 2019 Microsoft Research Ltd. All rights reserved.
// </copyright>
// <author>Christos Gkantsidis</author>
// <date>05/11/2019</date>
// <summary>.NET interface to the LDPC library of OpenAir</summary>

#pragma once

#include <memory>

#include "nrLDPC_encoder\defs.h"
#include "nrLDPC_decoder\nrLDPC_init_mem.h"
#include "nrLDPC_decoder\nrLDPC_decoder.h"


using namespace System;
using namespace System::Diagnostics::Contracts;

namespace OpenAir::LDPC {

    /// <summary>   Values that represent base graphs. </summary>
    public enum class BaseGraph
    {
        BaseGraph1 = 1,
        BaseGraph2 = 2
    };

    /// <summary>   Exception for signaling LDPC errors. </summary>
    [Serializable]
    public ref struct LdpcException : public Exception
    {
    public:

        /// <summary>   Initializes a new instance of the LDPC exception. </summary>
        LdpcException()
            : Exception()
        { }

        /// <summary>   Initializes a new instance of the LDPC exception. </summary>
        /// <param name="message">  Error message. </param>
        LdpcException(String^ message)
            : Exception(message)
        { }

        /// <summary>   Initializes a new instance of the LDPC exception. </summary>
        /// <param name="message">  Error message. </param>
        /// <param name="inner">    Inner exception. </param>
        LdpcException(String^ message, Exception^ inner)
            : Exception(message, inner)
        { }

    protected:

        /// <summary>   Initializes a new instance of the LDPC exception. </summary>
        /// <param name="info">     Serialization information. </param>
        /// <param name="context">  Serialization context. </param>
        LdpcException(
            System::Runtime::Serialization::SerializationInfo^ info,
            System::Runtime::Serialization::StreamingContext context) : Exception(info, context)
        {}
    };

    /// <summary>   Configuration of LDPC encoding and decoding. </summary>
    public ref class Configuration
    {
    public:

        /// <summary>   The maximum length of the block. </summary>
        static const int MAX_BLOCK_LENGTH = 1056;

        /// <summary>   Make configuration information from block length and coding rate. </summary>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        ///
        /// <param name="block_length">         The block length (in bytes). </param>
        /// <param name="nom_rate">             The nominator of the coding rate. </param>
        /// <param name="denom_rate">           The denominator of the coding rate. </param>
        ///
        /// <returns>   Nullptr if it fails, else a Configuration^. </returns>

        static Configuration^ MkFromBlockLength(int block_length, int nom_rate, int denom_rate)
        {
            if (block_length < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "length_in_bits");
            }
            if (block_length > Configuration::MAX_BLOCK_LENGTH)
            {
                throw gcnew ArgumentException("length is too large", "length_in_bits");
            }

            // TODO: Check that nom_rate and denom_rate make sense

            Contract::EndContractBlock();
            // Contract::Ensures(!Object::ReferenceEquals(Contract::Result<Configuration^>(), nullptr));

            auto block_length_in_bits = block_length * 8;
            BaseGraph bg;
            short kb;
            short nrows;
            short lift_size;

            if (block_length_in_bits > 3840)
            {
                bg = BaseGraph::BaseGraph1;
                kb = 22;
                nrows = 46;
            }
            else
            {
                bg = BaseGraph::BaseGraph2;
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
                if (Configuration::_lift_size_indices[i] >= (double)block_length_in_bits / kb) {
                    lift_size = Configuration::_lift_size_indices[i];
                    break;
                }
            }

            return gcnew Configuration(bg, kb, nrows, lift_size, nom_rate, denom_rate);
        }

        /// <summary>   Make default configuration information from block length. </summary>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   Nullptr if it fails, else a Configuration^. </returns>
        static Configuration^ MkFromBlockLength(int block_length)
        {
            return Configuration::MkFromBlockLength(block_length, 1, 3);
        }

        /// <summary>   Get the base graph. </summary>
        property BaseGraph BG {
            BaseGraph get()
            {
                return _bg;
            }
        }

        /// <summary>   Gets the number of systematic columns. </summary>
        property int Kb {
            int get()
            {
                return _kb;
            }
        }

        /// <summary>   Gets the lifting factor. </summary>
        property int Zc {
            int get()
            {
                return _lift_size;
            }
        }

        /// <summary>   Gets the number of rows. </summary>
        property int Rows {
            int get()
            {
                return _nrows;
            }
        }

        /// <summary>   Gets The nominator of the coding rate. </summary>
        property int Nom {
            int get()
            {
                return _nom_rate;
            }
        }

        /// <summary>   Get the denominator of the coding rate. </summary>
        property int Denom {
            int get()
            {
                return _denom_rate;
            }
        }

        /// <summary>   The coding rate. </summary>
        property double Rate
        {
            double get()
            {
                return ((double)_nom_rate) / ((double)_denom_rate);
            }
        }

        property int CodeRate
        {
            int get()
            {
                return _code_rate_vec[R_ind()];
            }
        }

        /// <summary>   Number of punctured columns. </summary>

        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   The total number of punctured columns. </returns>
        int NumberOfPuncturedColumns(int block_length)
        {
            if (block_length < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "block_length");
            }
            Contract::EndContractBlock();

            return number_of_punctured_columns(block_length);
        }

        /// <summary>   Number of removed bits. </summary>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   The total number of removed bits. </returns>
        int NumberOfRemovedBits(int block_length)
        {
            if (block_length < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "block_length");
            }
            Contract::EndContractBlock();

            return number_of_removed_bits(block_length);
        }

        /// <summary>   Last bit position. </summary>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   Position of last bit. </returns>
        int LastBitPosition(int block_length)
        {
            if (block_length < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "block_length");
            }
            Contract::EndContractBlock();

            return last_bit_position(block_length);
        }

        /// <summary>   Get a subset of the input data that is typically transmitted for this configuration. </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <typeparam name="T">    Generic type parameter. </typeparam>
        /// <param name="data">     The input set of data. </param>
        /// <param name="length">   The length of the input vector (in bytes). </param>
        /// <returns>   Subset of the input data </returns>
        generic <typename T>
        ArraySegment<T> SliceInputToChannel(array<T>^ data, int length)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (length < 0)
            {
                throw gcnew ArgumentException("Length cannot be negative", "length");
            }
            if (data->Length < (LastBitPosition(length) - 2 * _lift_size))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<T>(data, 0, count);
        }

        /// <summary>   Get a subset of the input data that is typically transmitted for this configuration.. </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="data">     The input set of data. </param>
        /// <param name="length">   The length of the input vector (in bytes). </param>
        /// <returns>   Subset of the input data </returns>
        ArraySegment<System::Byte> SliceInputToChannel(array<System::Byte>^ data, int length)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (length < 0)
            {
                throw gcnew ArgumentException("Length cannot be negative", "length");
            }
            if (data->Length < (LastBitPosition(length) - 2 * _lift_size))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<System::Byte>(data, 0, count);
        }

        /// <summary>
        /// Get a slice of the output buffer that is used to store data received from the channel.
        /// The rest of the buffer must have zero values.
        /// </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <typeparam name="T">    Generic type parameter. </typeparam>
        /// <param name="data">     The buffer. </param>
        /// <param name="length">   The length of the transmitted message (in bytes). </param>
        /// <returns>   A subset of the input buffer </returns>
        generic <typename T>
        ArraySegment<T> SliceOutputFromChannel(array<T>^ data, int length)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (length < 0)
            {
                throw gcnew ArgumentException("Length cannot be negative", "length");
            }
            if (data->Length < LastBitPosition(length))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<T>(data, 2 * _lift_size, count);
        }

        /// <summary>
        /// Get a slice of the output buffer that is used to store data received from the channel.
        /// The rest of the buffer must have zero values.
        /// </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="data">     The buffer. </param>
        /// <param name="length">   The length of the transmitted message (in bytes). </param>
        /// <returns>   A subset of the input buffer </returns>
        ArraySegment<System::SByte> SliceOutputFromChannel(array<System::SByte>^ data, int length)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (length < 0)
            {
                throw gcnew ArgumentException("Length cannot be negative", "length");
            }
            if (data->Length < LastBitPosition(length))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<System::SByte>(data, 2 * _lift_size, count);
        }

        /// <summary>
        /// Get the typical length (in bytes) of the input buffer to the decoder that
        /// will receive data from the channel.
        /// </summary>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        /// <param name="length">   The length of the transmitted message (in bytes). </param>
        /// <returns>   The number non-zero entries of the input to the decoder. </returns>
        int CountOfOutputChannel(int length)
        {
            if (length < 0)
            {
                throw gcnew ArgumentException("Length cannot be negative", "length");
            }
            Contract::EndContractBlock();

            return last_bit_position(length) - 2 * _lift_size;
        }

    /// <summary> The base graph as an integer (short int representation). </summary>
    internal:
        property short BGShort {
            short get()
            {
                return (short)_bg;
            }
        }

        /// <summary>   The lifting factor (short int representation). </summary>
        property short ZcShort {
            short get()
            {
                return (short)_lift_size;
            }
        }

    private:
        /// <summary>   The lift size indices. </summary>
        static array<short>^ _lift_size_indices = gcnew array<short>(51) {
            2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24,
                26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112,
                120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
        };

        /// <summary>   The code rate vector. </summary>
        static array<int>^ _code_rate_vec = gcnew array<int>(8) {
            15, 13, 25, 12, 23, 34, 56, 89
        };

        /// <summary>   The base graph. </summary>
        BaseGraph _bg;
        /// <summary>   The number of systematic columns. </summary>
        unsigned short _kb;
        /// <summary>   The number of rows. </summary>
        unsigned short _nrows;
        /// <summary>   Size of the lift. </summary>
        unsigned short _lift_size;
        /// <summary>   The nominator of the coding rate. </summary>
        int _nom_rate;
        /// <summary>   The denominator of the coding rate. </summary>
        int _denom_rate;

        /// <summary>   Initializes a new instance of the Configuration class. </summary>
        /// <param name="bg">           The base graph to use. </param>
        /// <param name="kb">           The number of systematic columns. </param>
        /// <param name="nrows">        The number of rows. </param>
        /// <param name="lift_size">    Size of the lift. </param>
        /// <param name="nom_rate">     The nominator of the coding rate. </param>
        /// <param name="denom_rate">   The denominator of the coding rate. </param>

        Configuration(
            BaseGraph bg, unsigned short kb, unsigned short nrows, unsigned short lift_size,
            int nom_rate, int denom_rate
            )
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

        /// <summary>   Number of punctured columns. </summary>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   The total number of punctured columns. </returns>
        int number_of_punctured_columns(int block_length)
        {
            auto block_length_in_bits = block_length * 8;
            auto first = (Rows - 2) * Zc + block_length_in_bits;
            auto second = float(block_length_in_bits) * float(_denom_rate) / float(_nom_rate);
            auto result = float(first) - second;
            result /= float(Zc);
            return (int)result;
        }

        /// <summary>   Number of removed bits. </summary>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   The total number of removed bits. </returns>
        int number_of_removed_bits(int block_length)
        {
            auto block_length_in_bits = 8 * block_length;
            auto no_punctured_columns = number_of_punctured_columns(block_length);
            auto first = (Rows - no_punctured_columns - 2) * Zc + block_length_in_bits;
            auto second = float(block_length_in_bits) * float(Denom) / float(Nom);
            auto result = float(first) - second;
            return (int)result;
        }

        /// <summary>   Last bit position. </summary>
        /// <param name="block_length"> The block length (in bytes). </param>
        /// <returns>   The position of the last bit. </returns>
        int last_bit_position(int block_length)
        {
            auto no_punctured_columns = number_of_punctured_columns(block_length);
            auto removed_bit = number_of_removed_bits(block_length);

            return (Kb + Rows - no_punctured_columns) * Zc - removed_bit;
        }

        /// <summary>   Gets the index into the code rate vector . </summary>
        /// <exception cref="System">   Thrown when a system error condition occurs. </exception>
        /// <returns>   The index. </returns>
        int R_ind()
        {
            int R_ind = 0;

            if (Nom == 1)
                if (Denom == 5)
                    if (BGShort == 2)
                        R_ind = 0;
                    else
                        throw gcnew System::NotSupportedException();
                else if (Denom == 3)
                    R_ind = 1;
                else if (Denom == 2)
                    //R_ind = 3;
                    throw gcnew System::NotSupportedException();
                else
                    throw gcnew System::NotSupportedException();

            else if (Nom == 2)
                if (Denom == 5)
                    //R_ind = 2;
                    throw gcnew System::NotSupportedException();
                else if (Denom == 3)
                    R_ind = 4;
                else
                    throw gcnew System::NotSupportedException();

            else if ((Nom == 22) && (Denom == 30))
                //R_ind = 5;
                throw gcnew System::NotSupportedException();
            else if ((Nom == 22) && (Denom == 27))
                //R_ind = 6;
                throw gcnew System::NotSupportedException();
            else if ((Nom == 22) && (Denom == 25))
                if (BGShort == 1)
                    R_ind = 7;
                else
                    throw gcnew System::NotSupportedException();
            else
                throw gcnew System::NotSupportedException();

            return R_ind;
        }
    };

    public value struct EncoderOutput
    {
    public:
        array<System::Byte>^ Buffer;
        int Start;
        int Count;
        int Length;

        property int Stop
        {
            int get()
            {
                return Start + Count;
            }
        }
    };

    /// <summary>   A simple encoder, which uses the basic implementation of LDPC from OpenAir. </summary>
    public ref class SimpleEncoder
    {
    public:
        /// <summary>   Initializes a new instance of the SimpleEncoder class. </summary>
        SimpleEncoder()
        {
        }

        /// <summary>   Create a buffer for the output channel. </summary>
        /// <returns>   Nullptr if it fails, else the channel output buffer. </returns>
        static array<System::SByte>^ GetChannelOutputBuffer()
        {
            // Observe that the array is automatically initialized to zero.
            return gcnew array<System::SByte>(DEFAULT_BUFFER_SIZE);
        }

        /// <summary>   Encodes the input data with default configuration. </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <param name="data"> The input data to encode. </param>
        /// <returns>   Nullptr if it fails, else the encoded data. </returns>
        array<System::Byte>^ Encode(array<System::Byte>^ data)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            Contract::EndContractBlock();
            // Contract::Ensures(!Object::ReferenceEquals(Contract::Result<array<System::Byte>^>(), nullptr));

            auto configuration = Configuration::MkFromBlockLength(data->Length);
            return Encode(data, configuration);
        }

        /// <summary>   Encodes the input data. </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="LdpcException">
        /// Thrown when an error condition occurs.
        /// </exception>
        /// <param name="data">             The input data to encode. </param>
        /// <param name="configuration">    The configuration to use for encoding. </param>
        /// <returns>   Nullptr if it fails, else the encoded data. </returns>
        array<System::Byte>^ Encode(array<System::Byte>^ data, Configuration^ configuration)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (Object::ReferenceEquals(nullptr, configuration))
            {
                throw gcnew ArgumentNullException("configuration");
            }
            Contract::EndContractBlock();
            // Contract::Ensures(!Object::ReferenceEquals(Contract::Result<array<System::Byte>^>(), nullptr));

            auto result = gcnew array<System::Byte>(DEFAULT_BUFFER_SIZE);
            pin_ptr<System::Byte> pinned_data = &(data[0]);
            pin_ptr<System::Byte> pinned_result = &(result[0]);

            auto error = ldpc_encoder_orig(pinned_data, pinned_result, data->Length * 8, configuration->BGShort, 0);
            if (error != 0)
            {
                throw gcnew LdpcException("Encoder encountered error");
            }

            return result;
        }

        /// <summary>   Encodes the input data and returns all encoder output. </summary>
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="LdpcException">
        /// Thrown when an error condition occurs.
        /// </exception>
        /// <param name="data">             The input data to encode. </param>
        /// <param name="configuration">    The configuration to use for encoding. </param>
        /// <returns>   Nullptr if it fails, else the encoded data. </returns>
        EncoderOutput EncodeFull(array<System::Byte>^ data, Configuration^ configuration)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (Object::ReferenceEquals(nullptr, configuration))
            {
                throw gcnew ArgumentNullException("configuration");
            }
            Contract::EndContractBlock();
            // Contract::Ensures(!Object::ReferenceEquals(Contract::Result<array<System::Byte>^>(), nullptr));

            auto result = gcnew array<System::Byte>(ENCODER_CHANNEL_SIZE);
            pin_ptr<System::Byte> pinned_data = &(data[0]);
            pin_ptr<System::Byte> pinned_result = &(result[0]);

            auto error = ldpc_encoder_orig_full(pinned_data, pinned_result, data->Length * 8, configuration->BGShort);
            if (error.Count < 0 || error.Start < 0)
            {
                throw gcnew LdpcException("Encoder encountered error");
            }

            EncoderOutput output;
            output.Buffer = result;
            output.Start = error.Start;
            output.Count = error.Count;
            output.Length = error.Length;

            return output;
        }

    private:
        // TODO: Give proper names to constants 68 and 384.
        static const int DEFAULT_BUFFER_SIZE = 68 * 384;
    };

    /// <summary>   Managed interface to default LDPC decoder from OpenAir. </summary>
    public ref class Decoder
    {
    public:

        /// <summary>   Initializes a new instance of the Decoder class. </summary>
        Decoder()
        {
            _p_nrLDPC_procBuf = nrLDPC_init_mem();
            _profiler = new t_nrLDPC_time_stats();
        }

        /// <summary>   Finalizes the instance of the Decoder class. </summary>
        ~Decoder()
        {
            if (_disposed)
            {
                return;
            }
            this->!Decoder();
            _disposed = true;
        }

        /// <summary>   Finalizes an instance of the Decoder class. </summary>
        !Decoder()
        {
            if (_p_nrLDPC_procBuf != nullptr)
            {
                nrLDPC_free_mem(_p_nrLDPC_procBuf);
            }
            if (_profiler != nullptr)
            {
                delete _profiler;
            }
            _p_nrLDPC_procBuf = nullptr;
            _profiler = nullptr;
        }

        /// <summary>   Decodes. </summary>
        ///
        /// <remarks>   Chrisgk, 05/11/2019. </remarks>
        ///
        /// <exception cref="ArgumentNullException">
        /// Thrown when one or more required arguments are null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when one or more arguments have unsupported or illegal values.
        /// </exception>
        ///
        /// <param name="data">                     The data. </param>
        /// <param name="configuration">            The configuration. </param>
        /// <param name="maximum_iterations">       The maximum iterations. </param>
        /// <param name="output_length">            The output length (in bytes). </param>
        ///
        /// <returns>   Nullptr if it fails, else the decoded bytes. </returns>
        array<System::Byte>^ Decode(
            array<System::SByte>^ data,
            Configuration^ configuration,
            int maximum_iterations,
            int output_length)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (maximum_iterations <= 0)
            {
                throw gcnew ArgumentException("Number of iterations must be positive", "maximum_iterations");
            }
            if (output_length < 0)
            {
                throw gcnew ArgumentException("Number of bits must be a multiple of 8");
            }
            Contract::EndContractBlock();

            auto output_length_in_bits = 8 * output_length;
            t_nrLDPC_dec_params params;
            params.BG = (uint8_t)configuration->BG;
            params.Z = configuration->ZcShort;
            params.R = configuration->CodeRate;
            params.numMaxIter = maximum_iterations;
            params.outMode = nrLDPC_outMode_BIT;

            auto output_length_in_bits_normalized =
                (output_length_in_bits >= 8 * 8) ? output_length_in_bits : 8 * 8;
            std::unique_ptr<int8_t[]> temp = std::make_unique<int8_t[]>(output_length_in_bits_normalized);
            memset(temp.get(), 0x00, output_length_in_bits_normalized);

            pin_ptr<int8_t> pinned_data = &(data[0]);
            int iterations = nrLDPC_decoder(&params, pinned_data, temp.get(), _p_nrLDPC_procBuf, _profiler);

            auto result = gcnew array<System::Byte>(output_length);
            for (size_t i = 0; i < output_length; i++)
            {
                result[i] = System::Byte(temp[i]);
            }

            return result;
        }

        /// <summary>   Resets the profiler. </summary>
        void ResetProfiler()
        {
            reset_meas(&(_profiler->llr2llrProcBuf));
            reset_meas(&(_profiler->llr2CnProcBuf));
            reset_meas(&(_profiler->cnProc));
            reset_meas(&(_profiler->cnProcPc));
            reset_meas(&(_profiler->bnProc));
            reset_meas(&(_profiler->bnProcPc));
            reset_meas(&(_profiler->cn2bnProcBuf));
            reset_meas(&(_profiler->bn2cnProcBuf));
            reset_meas(&(_profiler->llrRes2llrOut));
            reset_meas(&(_profiler->llr2bit));
        }

    private:
        /// <summary>   Internal buffer for the decoder. </summary>
        t_nrLDPC_procBuf* _p_nrLDPC_procBuf;

        /// <summary>   The profiler. </summary>
        t_nrLDPC_time_stats* _profiler;

        bool _disposed = false;
    };
}
