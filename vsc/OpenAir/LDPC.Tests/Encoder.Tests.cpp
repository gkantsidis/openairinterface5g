#include "pch.h"
#include "CppUnitTest.h"
#include <filesystem>
#include <iostream>
#include <fstream>

extern "C"
{
#include "nrLDPC_encoder/defs.h"
#include <nrLDPC_decoder\nrLDPC_init_mem.h>
}

#include "utilities.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define malloc16(x) memalign(32, x)

namespace LDPCTests
{
    TEST_CLASS(EncoderTests)
    {
    public:

        int find_first_difference(unsigned char* ground, unsigned char* test, int length)
        {
            Assert::IsTrue(length >= 0);

            for (size_t i = 0; i < length; i++)
            {
                if (ground[i] != test[i])
                {
                    return i;
                }
            }

            return -1;
        }

        TEST_METHOD(TestEncodeAllOnes)
        {
            short BG = 1;
            short block_length = 1056;
            auto test_input = (unsigned char*)malloc16(sizeof(unsigned char) * block_length);
            auto channel_input = (unsigned char*)malloc16(sizeof(unsigned char) * 68 * 384);

            Assert::IsNotNull(test_input);
            Assert::IsNotNull(channel_input);

            if (channel_input == NULL || test_input == NULL)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
                return;
            }

            memset(channel_input, 0x00, sizeof(unsigned char) * 68 * 384);

            for (size_t i = 0; i < block_length; i++)
            {
                test_input[i] = 1;
            }

            std::string reference = "encoder_default_8448_all_ones.bin";
            std::unique_ptr<char> encoding = read_binary_file(reference, 68 * 384);
            ldpc_encoder_orig(test_input, channel_input, block_length * 8, BG, 0);

            unsigned char* ground = (unsigned char*)encoding.get();
            auto difference = find_first_difference(ground, channel_input, 68 * 384);
            Assert::AreEqual(-1, difference);
        }

        TEST_METHOD(TestEncodeAllZeros)
        {
            short BG = 1;
            short block_length = 1056;
            auto test_input = (unsigned char*)malloc16(sizeof(unsigned char) * block_length);
            auto channel_input = (unsigned char*)malloc16(sizeof(unsigned char) * 68 * 384);

            Assert::IsNotNull(test_input);
            Assert::IsNotNull(channel_input);

            if (channel_input == NULL || test_input == NULL)
            {
                // We already test above; this is used to avoid the warning below
                Assert::Fail();
                return;
            }

            memset(channel_input, 0x00, sizeof(unsigned char) * 68 * 384);

            for (size_t i = 0; i < block_length; i++)
            {
                test_input[i] = 0;
            }

            ldpc_encoder_orig(test_input, channel_input, block_length * 8, BG, 0);

            for (size_t i = 0; i < 68 * 384; i++)
            {
                Assert::AreEqual((unsigned char)0, channel_input[i]);
            }
        }
    };
}
