#include "pch.h"
#include "CppUnitTest.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <tchar.h>

extern "C"
{
#include "nrLDPC_encoder/defs.h"
#include <nrLDPC_decoder\nrLDPC_init_mem.h>
#include "nrLDPC_decoder\nrLDPC_decoder.h"
}

#include "Configuration.h"

#include "utilities.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define malloc16(x) memalign(32, x)

namespace LDPCTests
{
    TEST_CLASS(DecoderTests)
    {
    public:

        TEST_METHOD(TestDecodeAllZero)
        {
            // The values below are copied from the .NET test
            // When the input vector is all zero, then the encoding is all all zero.

            constexpr auto size = 68 * 384;
            constexpr auto block_length = 1056;
            constexpr auto bit_length = 8 * block_length;

            const auto encoding = std::make_unique<int8_t[]>(size);
            const auto channel = std::make_unique<int8_t[]>(size);
            memset(encoding.get(), 0x7F, size);
            memset(channel.get(), 0x00, size);

            const auto configuration = Configuration::MakeFromBlockLength(block_length, 1, 3);
            t_nrLDPC_dec_params params = mk_params(configuration);

            const auto channel_output = ideal_channel_transfer(&configuration, channel.get(), encoding.get(), block_length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
            const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            auto iterations = decode(&params, channel_output, output.get());

            for (size_t i = 0; i < block_length; i++)
            {
                Assert::AreEqual(static_cast<int8_t>(0), output[i]);
            }
        }

        TEST_METHOD(TestDecodeAllZeroSmallSize)
        {
            // The values below are copied from the .NET test
            // When the input vector is all zero, then the encoding is all all zero.

            auto constexpr size = 68 * 384;
            auto constexpr length = 64;
            auto constexpr bit_length = 8 * 1056;

            const auto encoding = std::make_unique<int8_t[]>(size);
            const auto channel = std::make_unique<int8_t[]>(size);
            memset(encoding.get(), 0x7F, size);
            memset(channel.get(), 0x00, size);

            const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);
            t_nrLDPC_dec_params params = mk_params(configuration);

            const auto channel_output = ideal_channel_transfer(&configuration, channel.get(), encoding.get(), length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
            const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            auto iterations = decode(&params, channel_output, output.get());

            for (size_t i = 0; i < length; i++)
            {
                Assert::AreEqual(static_cast<int8_t>(0), output[i]);
            }
        }

        TEST_METHOD(TestDecodeAllZeroOneByteSize)
        {
            // The values below are copied from the .NET test
            // When the input vector is all zero, then the encoding is all all zero.

            auto constexpr size = 8 * 384;
            auto constexpr length = 1;

            const auto encoding = std::make_unique<int8_t[]>(size);
            const auto channel = std::make_unique<int8_t[]>(size);
            memset(encoding.get(), 0x7F, size);
            memset(channel.get(), 0x00, size);

            const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);
            t_nrLDPC_dec_params params = mk_params(configuration);

            auto const channel_output = ideal_channel_transfer(&configuration, channel.get(), encoding.get(), length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
            auto const output = std::make_unique<int8_t[]>(8 * 8);
            memset(output.get(), 0x00, 8 * 8);

            auto iterations = decode(&params, channel_output, output.get());

            for (size_t i = 0; i < length; i++)
            {
                Assert::AreEqual(static_cast<int8_t>(0), output[i]);
            }
        }

        TEST_METHOD(TestEncodeDecodeAllOnes)
        {
            auto constexpr size = 68 * 384;
            auto constexpr length = 1056;
            auto constexpr bit_length = 8 * 1056;

            const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);

            const auto test_input = std::make_unique<unsigned char[]>(length);
            const auto channel_input = std::make_unique<unsigned char[]>(size);

            Assert::IsNotNull(test_input.get());
            Assert::IsNotNull(channel_input.get());

            if (channel_input == nullptr || test_input == nullptr)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
            }

            for (size_t i = 0; i < length; i++)
            {
                test_input[i] = 1;
            }

            memset(channel_input.get(), 0x00, sizeof(unsigned char) * size);
            ldpc_encoder_orig(test_input.get(), channel_input.get(), length * 8, configuration.BG(), 0);

            for (size_t i = 0; i < size; i++)
            {
                channel_input[i] = (channel_input[i] == 0)?0x7F:-0x80;
            }

            const auto channel_output = std::make_unique<int8_t[]>(size);
            memset(channel_output.get(), 0x00, size);

            const auto decoder_input = reinterpret_cast<int8_t*>(channel_input.get());
            ideal_channel_transfer(&configuration, channel_output.get(), decoder_input, length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
            const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            t_nrLDPC_dec_params params;
            params.BG = configuration.BG();
            params.Z = configuration.Zc();
            params.R = configuration.R();

            params.numMaxIter = 10;
            params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT;

            auto iterations = decode(&params, channel_output.get(), output.get());

            for (size_t i = 0; i < length; i++)
            {
                Assert::AreEqual(static_cast<int8_t>(1), output[i]);
            }
        }

        TEST_METHOD(TestEncodeAndDecode)
        {
	        constexpr auto seed = 18;
	        constexpr auto size = 68 * 384;
	        constexpr auto length = 1056;
	        constexpr auto bit_length = 8 * length;

	        const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);

	        auto test_input = std::make_unique<unsigned char[]>(length);
	        auto channel_input = std::make_unique<unsigned char[]>(size);

            Assert::IsNotNull(test_input.get());
            Assert::IsNotNull(channel_input.get());

            if (channel_input == nullptr || test_input == nullptr)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
            }

            srand(seed);  // NOLINT(cert-msc51-cpp)
            for (size_t i = 0; i < length; i++)
            {
                test_input[i] = rand() & 0xFF;  // NOLINT(concurrency-mt-unsafe)
            }

            memset(channel_input.get(), 0x00, sizeof(unsigned char) * size);
            ldpc_encoder_orig(test_input.get(), channel_input.get(), length * 8, configuration.BG(), 0);

            for (size_t i = 0; i < size; i++)
            {
                channel_input[i] = (channel_input[i] == 0) ? 0x7F : -0x80;
            }

	        const auto channel_output = std::make_unique<int8_t[]>(size);
            memset(channel_output.get(), 0x00, size);

	        const auto decoder_input = reinterpret_cast<int8_t*>(channel_input.get());
            ideal_channel_transfer(&configuration, channel_output.get(), decoder_input, length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
	        const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            t_nrLDPC_dec_params params{
                .BG = static_cast<uint8_t>(configuration.BG()),
                .Z = static_cast<uint16_t>(configuration.Zc()),
                .R = static_cast<uint8_t>(configuration.R()),
                .numMaxIter = 10,
                .outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT
            };

            auto iterations = decode(&params, channel_output.get(), output.get());

            for (size_t i = 0; i < length; i++)
            {
                auto expected = static_cast<int8_t>(test_input[i]);
                auto actual = output[i];
                Assert::AreEqual(expected, actual);
            }
        }

        TEST_METHOD(TestEncodeAndDecodeSmallAllOnes)
        {
	        constexpr auto size = 68 * 384;
            constexpr auto length = 64;
            constexpr auto bit_length = 8 * length;

	        const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);

	        auto test_input = std::make_unique<unsigned char[]>(length);
	        auto channel_input = std::make_unique<int8_t[]>(size);

            Assert::IsNotNull(test_input.get());
            Assert::IsNotNull(channel_input.get());

            if (channel_input == nullptr || test_input == nullptr)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
            }

            for (size_t i = 0; i < length; i++)
            {
                test_input[i] = 0xFF;
            }

            memset(channel_input.get(), 0x00, sizeof(unsigned char) * size);
            ldpc_encoder_orig(
                test_input.get(), 
                reinterpret_cast<unsigned char*>(channel_input.get()),
                length * 8, 
                configuration.BG(), 
                0);

            for (size_t i = configuration.InputChannelStart(); i < configuration.InputChannelEnd(length); i++)
            {
                Assert::IsTrue(channel_input[i] == 0 || channel_input[i] == 1);
                channel_input[i] = (channel_input[i] == 0) ? INT8_MAX : INT8_MIN;
            }

	        const auto channel_output = std::make_unique<int8_t[]>(size);
            memset(channel_output.get(), 0x00, size);

	        const auto decoder_input = (int8_t*)channel_input.get();
            ideal_channel_transfer(&configuration, channel_output.get(), decoder_input, length);

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
	        const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            t_nrLDPC_dec_params params{
                .BG = static_cast<uint8_t>(configuration.BG()),
                .Z  = static_cast<uint16_t>(configuration.Zc()),
                .R  = static_cast<uint8_t>(configuration.R()),
                .numMaxIter = 10,
                .outMode    = e_nrLDPC_outMode::nrLDPC_outMode_BIT
            };

            auto iterations = decode(&params, channel_output.get(), output.get());

            for (size_t i = 0; i < length; i++)
            {
                auto expected = static_cast<int8_t>(test_input[i]);
                auto actual = output[i];
                Assert::AreEqual(expected, actual);
            }
        }

        TEST_METHOD(TestEncodeAndDecodeWithRandomBitFlips)
        {
	        constexpr auto seed = 18;
	        constexpr auto size = 68 * 384;
	        constexpr auto length = 1056;
	        constexpr auto bit_length = 8 * length;
	        constexpr auto uncoded_ber = 0.015; // 9824 bits transmitted, around 147 bits of error

            srand(seed);  // NOLINT(cert-msc51-cpp)
            // Around 0.86 efficiency
	        constexpr auto starting_bit = 768;
	        constexpr auto last_bit = 10592;

	        const auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);

            std::unique_ptr<unsigned char[]> input = std::make_unique<unsigned char[]>(length);
            std::unique_ptr<unsigned char[]> channel = std::make_unique<unsigned char[]>(size);

            Assert::IsNotNull(input.get());
            Assert::IsNotNull(channel.get());

            if (channel == nullptr || input == nullptr)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
            }

            srand(seed);  // NOLINT(cert-msc51-cpp)
            for (size_t i = 0; i < length; i++)
            {
                input[i] = i & 0xFF;
            }

            memset(channel.get(), 0x00, sizeof(unsigned char) * size);
            ldpc_encoder_orig_full(input.get(), channel.get(), length * 8, configuration.BG());

            auto errors = 0;
            for (size_t i = 0; i < size; i++)
            {
                if (i < starting_bit || i >= last_bit)
                {
                    // We don't transmit that bit
                    channel[i] = 0x00;
                }
                else
                {
	                // ReSharper disable once CppTooWideScopeInitStatement
	                const auto failure = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);  // NOLINT(concurrency-mt-unsafe)
                    if (failure <= uncoded_ber)
                    {
                        // We flip the bit here.
                        channel[i] = (channel[i] == 1) ? 0x60 : -0x60;
                        // channel[i] = (channel[i] == 1) ? 0x40 : -0x40;
                        errors++;
                    }
                    else
                    {
                        channel[i] = (channel[i] == 0) ? 0x7F : -0x80;
                    }
                }
            }

            // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
	        const auto output = std::make_unique<int8_t[]>(bit_length);
            memset(output.get(), 0x00, bit_length);

            t_nrLDPC_dec_params params{
                .BG = static_cast<uint8_t>(configuration.BG()),
                .Z = static_cast<uint16_t>(configuration.Zc()),
                .R = static_cast<uint8_t>(configuration.R()),
                .numMaxIter = 50,
                .outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT
            };

            auto iterations = decode(&params, reinterpret_cast<int8_t*>(channel.get()), output.get());

            /*
            // Count number of errors
            auto total_byte_errors = 0;
            for (size_t i = 0; i < length; i++)
            {
	            const auto expected = input[i];
	            const auto actual = static_cast<unsigned char>(output[i]);
                if (expected != actual)
                {
                    total_byte_errors++;
                }
            }
        	*/

            for (size_t i = 0; i < length; i++)
            {
                auto expected = static_cast<int8_t>(input[i]);
                auto actual = output[i];
                Assert::AreEqual(expected, actual);
            }
        }

    private:
        static int32_t decode(t_nrLDPC_dec_params * params, int8_t* channel, int8_t* output)
        {
	        const auto _p_nrLDPC_procBuf = nrLDPC_init_mem();
	        const std::unique_ptr<t_nrLDPC_time_stats> profiler(new t_nrLDPC_time_stats());
	        const auto iterations = nrLDPC_decoder(params, channel, output, _p_nrLDPC_procBuf, profiler.get());
            nrLDPC_free_mem(_p_nrLDPC_procBuf);

            return iterations;
        }

        static t_nrLDPC_dec_params mk_params(const Configuration& configuration)
        {
	        const t_nrLDPC_dec_params params{
                .BG = static_cast<uint8_t>(configuration.BG()),
                .Z = static_cast<uint16_t>(configuration.Zc()),
                .R = static_cast<uint8_t>(configuration.R()),
                .numMaxIter = 10,
                .outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT
            };

            return params;
        }
    };
}
