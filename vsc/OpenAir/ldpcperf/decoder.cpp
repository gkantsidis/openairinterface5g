#include <memory>
#include <Windows.h>

extern "C"
{
#include "nrLDPC_encoder/defs.h"
#include <nrLDPC_decoder\nrLDPC_init_mem.h>
#include "nrLDPC_decoder\nrLDPC_decoder.h"
}

#include "Configuration.h"
#include "commands.h"

typedef struct _DecodeEvalParameters {
    int Seed;
    int DecoderIterations;
    int Iterations;
} DecodeEvalParameters;

std::shared_ptr<void> parse_decode_params()
{
    auto params = std::make_shared<DecodeEvalParameters>();
    if (params == nullptr)
    {
        // TODO: record error
        return nullptr;
    }

    params->Seed = 10;
    params->DecoderIterations = 100;

    params->Iterations = 100;

    return params;
}

void eval_decode(const std::shared_ptr<void>& params)
{
    if (params == nullptr)
    {
        // TODO: record error
        return;
    }

    auto p = std::reinterpret_pointer_cast<DecodeEvalParameters>(params);

    auto size = 68 * 384;
    auto length = 1056;
    auto bit_length = 8 * length;

    auto configuration = Configuration::MakeFromBlockLength(length, 1, 3);

    std::unique_ptr<unsigned char[]> test_input = std::make_unique<unsigned char[]>(length);
    std::unique_ptr<unsigned char[]> channel_input = std::make_unique<unsigned char[]>(size);

    if (channel_input == NULL || test_input == NULL)
    {
        // We already test above; this is used to avoid the warning below
        return;
    }

    srand(p->Seed);
    for (size_t i = 0; i < length; i++)
    {
        test_input[i] = rand() & 0xFF;
    }

    memset(channel_input.get(), 0x00, sizeof(unsigned char) * size);
    ldpc_encoder_orig(test_input.get(), channel_input.get(), length * 8, configuration.BG(), 0);

    for (size_t i = 0; i < size; i++)
    {
        channel_input[i] = (channel_input[i] == 0) ? 0x7F : -0x80;
    }

    std::unique_ptr<int8_t[]> channel_output = std::make_unique<int8_t[]>(size);
    memset(channel_output.get(), 0x00, size);

    auto decoder_input = (int8_t*)channel_input.get();
    ideal_channel_transfer(&configuration, channel_output.get(), decoder_input, length);

    // The following needs to be 8 x input buffer. Actually, it should also be at least 8x8=64 bytes.
    std::unique_ptr<int8_t[]> output = std::make_unique<int8_t[]>(bit_length);
    memset(output.get(), 0x00, bit_length);

    t_nrLDPC_dec_params decoder_params;
    decoder_params.BG = configuration.BG();
    decoder_params.Z = configuration.Zc();
    decoder_params.R = configuration.R();

    decoder_params.numMaxIter = p->DecoderIterations;
    decoder_params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT;

    t_nrLDPC_time_stats profiler;
    t_nrLDPC_procBuf* _p_nrLDPC_procBuf = nrLDPC_init_mem();
    if (_p_nrLDPC_procBuf == NULL)
    {
        // TODO: record error
        return;
    }

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);

    for (size_t experiment = 0; experiment < p->Iterations; experiment++)
    {
        QueryPerformanceCounter(&StartingTime);
        auto actual_iterations = nrLDPC_decoder(&decoder_params, channel_output.get(), output.get(), _p_nrLDPC_procBuf, &profiler);
        QueryPerformanceCounter(&EndingTime);
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        printf("%d %d %lld\n", experiment, actual_iterations, ElapsedMicroseconds.QuadPart);
        // TODO: record statistics from profiler
    }

    nrLDPC_free_mem(_p_nrLDPC_procBuf);
}
