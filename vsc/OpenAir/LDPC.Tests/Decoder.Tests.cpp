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
    TEST_CLASS(DecoderTests)
    {
    public:

        TEST_METHOD(TestDecodeAllOnes)
        {
            std::string reference = "encoder_default_8448_all_ones.bin";
            std::unique_ptr<char> encoding = read_binary_file(reference, 68 * 384);

            // TODO: Write Test
        }

        TEST_METHOD(TestDecodeAllZero)
        {
            auto size = 68 * 384;

            std::unique_ptr<t_nrLDPC_time_stats> profiler(new t_nrLDPC_time_stats());
            std::unique_ptr<char> encoding(new char[size]);
            memset(encoding.get(), 0x00, size);

            t_nrLDPC_dec_params params;

            auto _p_nrLDPC_procBuf = nrLDPC_init_mem();


            nrLDPC_free_mem(_p_nrLDPC_procBuf);
        }

        TEST_METHOD(TestEncodeAndDecode)
        {
            // TODO: Write Test
        }
    };
}
