#include <memory>

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

void* parse_decode_params()
{
    DecodeEvalParameters * params = (DecodeEvalParameters *) malloc(sizeof(DecodeEvalParameters));

    params->Seed = 10;
    params->DecoderIterations = 100;

    params->Iterations = 100;

    return (void*)params;
}

void eval_decode(void * params)
{
    if (params == NULL)
    {
        return;
    }

    DecodeEvalParameters* p = (DecodeEvalParameters*)params;

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

    //t_nrLDPC_dec_params params;
    //params.BG = configuration.BG();
    //params.Z = configuration.Zc();
    //params.R = configuration.R();

    //params.numMaxIter = 10;
    //params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT;

    //auto iterations = decode(&params, channel_output.get(), output.get());


    free(params);
}
