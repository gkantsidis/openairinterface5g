#pragma once

#include <memory>

#include "nrLDPC_encoder\defs.h"
#include "nrLDPC_decoder\nrLDPC_init_mem.h"
#include "nrLDPC_decoder\nrLDPC_decoder.h"

using namespace System;
using namespace System::Diagnostics::Contracts;

namespace OpenAir::LDPC {

    public enum class BaseGraph
    {
        BaseGraph1 = 1,
        BaseGraph2 = 2
    };

    [Serializable]
    public ref struct LdpcException : public Exception
    {
    public:
        LdpcException()
            : Exception()
        { }

        LdpcException(String^ message)
            : Exception(message)
        { }

        LdpcException(String^ message, Exception^ inner)
            : Exception(message, inner)
        { }

    protected:
        LdpcException(
            System::Runtime::Serialization::SerializationInfo^ info,
            System::Runtime::Serialization::StreamingContext context) : Exception(info, context)
        {}
    };

    public ref class Configuration
    {
    public:
        static Configuration^ MkFromBlockLength(int block_length_in_bits, int nom_rate, int denom_rate)
        {
            if (block_length_in_bits < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "length_in_bits");
            }
            if (block_length_in_bits > 8448)
            {
                throw gcnew ArgumentException("length is too large", "length_in_bits");
            }
            Contract::EndContractBlock();
            // Contract::Ensures(!Object::ReferenceEquals(Contract::Result<Configuration^>(), nullptr));

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

        static Configuration^ MkFromBlockLength(int block_length_in_bits)
        {
            return Configuration::MkFromBlockLength(block_length_in_bits, 1, 3);
        }

        property BaseGraph BG {
            BaseGraph get()
            {
                return _bg;
            }
        }

        property int Kb {
            int get()
            {
                return _kb;
            }
        }

        property int Zc {
            int get()
            {
                return _lift_size;
            }
        }

        property int Rows {
            int get()
            {
                return _nrows;
            }
        }

        property int Nom {
            int get()
            {
                return _nom_rate;
            }
        }

        property int Denom {
            int get()
            {
                return _denom_rate;
            }
        }

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

        int NumberOfPuncturedColumns(int block_length_in_bits)
        {
            if (block_length_in_bits < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "length_in_bits");
            }
            Contract::EndContractBlock();

            return number_of_punctured_columns(block_length_in_bits);
        }

        int NumberOfRemovedBits(int block_length_in_bits)
        {
            if (block_length_in_bits < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "length_in_bits");
            }
            Contract::EndContractBlock();

            auto no_punctured_columns = NumberOfPuncturedColumns(block_length_in_bits);
            return number_of_removed_bits(block_length_in_bits);
        }

        int LastBitPosition(int block_length_in_bits)
        {
            if (block_length_in_bits < 0)
            {
                throw gcnew ArgumentException("length cannot be negative", "length_in_bits");
            }
            Contract::EndContractBlock();

            return last_bit_position(block_length_in_bits);
        }

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
            if (data->Length < (last_bit_position(length) - 2 * _lift_size))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<T>(data, 0, count);
        }

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
            if (data->Length < (last_bit_position(length) - 2 * _lift_size))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<System::Byte>(data, 0, count);
        }

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
            if (data->Length < last_bit_position(length))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<T>(data, 2 * _lift_size, count);
        }

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
            if (data->Length < last_bit_position(length))
            {
                throw gcnew ArgumentException("Length is too large");
            }
            Contract::EndContractBlock();

            auto count = last_bit_position(length) - 2 * _lift_size;
            return ArraySegment<System::SByte>(data, 2 * _lift_size, count);
        }


    internal:
        property short BGShort {
            short get()
            {
                return (short)_bg;
            }
        }

        property short ZcShort {
            short get()
            {
                return (short)_lift_size;
            }
        }

    private:
        static array<short>^ _lift_size_indices = gcnew array<short>(51) {
            2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24,
                26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112,
                120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
        };

        BaseGraph _bg;
        unsigned short _kb;
        unsigned short _nrows;
        unsigned short _lift_size;
        int _nom_rate;
        int _denom_rate;

        Configuration(
            BaseGraph bg, unsigned short kb, unsigned short nrows, unsigned short lift_size,
            int nom_rate, int denom_rate
            )
            : _bg{ bg }
            , _kb{ kb }
            , _nrows{ nrows }
            , _lift_size{ lift_size }
            , _nom_rate{ nom_rate }
            , _denom_rate{ denom_rate }
        {
        }

        int number_of_punctured_columns(int block_length_in_bits)
        {
            auto first = (Rows - 2) * Zc + block_length_in_bits;
            auto second = float(block_length_in_bits) * float(_denom_rate) / float(_nom_rate);
            auto result = float(first) - second;
            result /= float(Zc);
            return (int)result;
        }

        int number_of_removed_bits(int block_length_in_bits)
        {
            auto no_punctured_columns = number_of_punctured_columns(block_length_in_bits);
            auto first = (Rows - no_punctured_columns - 2) * Zc + block_length_in_bits;
            auto second = float(block_length_in_bits) * float(Denom) / float(Nom);
            auto result = float(first) - second;
            return (int)result;
        }

        int last_bit_position(int block_length_in_bits)
        {
            auto no_punctured_columns = number_of_punctured_columns(block_length_in_bits);
            auto removed_bit = number_of_removed_bits(block_length_in_bits);

            return (Kb + Rows - no_punctured_columns) * Zc - removed_bit;
        }
    };

    public ref class SimpleEncoder
    {
    public:
        SimpleEncoder()
        {
        }

        static array<System::SByte>^ GetChannelOutputBuffer()
        {
            return gcnew array<System::SByte>(68 * 384);
        }

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

            auto result = gcnew array<System::Byte>(68 * 384);
            pin_ptr<System::Byte> pinned_data = &(data[0]);
            pin_ptr<System::Byte> pinned_result = &(result[0]);

            auto error = ldpc_encoder_orig(pinned_data, pinned_result, data->Length * 8, configuration->BGShort, 0);
            if (error != 0)
            {
                throw gcnew LdpcException("Encoder encountered error");
            }

            return result;
        }

    private:
    };

    public ref class Decoder
    {
    public:
        Decoder()
        {
            _p_nrLDPC_procBuf = nrLDPC_init_mem();
            _profiler = new t_nrLDPC_time_stats();
        }

        ~Decoder()
        {
            if (_disposed)
            {
                return;
            }
            this->!Decoder();
            _disposed = true;
        }

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

        array<System::Byte>^ Decode(array<System::SByte>^ data, Configuration^ configuration, int maximum_iterations, int output_length_in_bits)
        {
            if (Object::ReferenceEquals(nullptr, data))
            {
                throw gcnew ArgumentNullException("data");
            }
            if (maximum_iterations <= 0)
            {
                throw gcnew ArgumentException("Number of iterations must be positive", "maximum_iterations");
            }
            if ((output_length_in_bits % 8) != 0)
            {
                throw gcnew ArgumentException("Number of bits must be a multiple of 8");
            }
            Contract::EndContractBlock();

            t_nrLDPC_dec_params params;
            params.BG = (uint8_t)configuration->BG;
            params.Z = configuration->ZcShort;
            params.R = _code_rate_vec[configuration->R_ind()];
            params.numMaxIter = maximum_iterations;
            params.outMode = nrLDPC_outMode_BIT;

            std::unique_ptr<int8_t[]> temp(new int8_t[output_length_in_bits]);
            memset(temp.get(), 0x00, output_length_in_bits);

            pin_ptr<int8_t> pinned_data = &(data[0]);
            int iterations = nrLDPC_decoder(&params, pinned_data, temp.get(), _p_nrLDPC_procBuf, _profiler);

            auto result = gcnew array<System::Byte>(output_length_in_bits / 8);
            for (size_t i = 0; i < output_length_in_bits/8; i++)
            {
                result[i] = System::Byte(temp[i]);
            }

            return result;
        }

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
        static array<int>^ _code_rate_vec = gcnew array<int>(8) {
            15, 13, 25, 12, 23, 34, 56, 89
        };

        t_nrLDPC_procBuf* _p_nrLDPC_procBuf;
        t_nrLDPC_time_stats* _profiler;
        bool _disposed = false;
    };
}
