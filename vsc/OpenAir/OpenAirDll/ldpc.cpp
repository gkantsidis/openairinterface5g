#include "pch.h"
#include "ldpc.h"

#include "nrLDPC_encoder\defs.h"
#include "nrLDPC_decoder\nrLDPC_init_mem.h"
#include "nrLDPC_decoder\nrLDPC_decoder.h"

int ldpc_encode_simple(unsigned char* input, int input_length, unsigned char* encoded, int base_graph)
{
    if (input == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (encoded == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (input_length <= 0)
    {
        return LDPC_LENGTH_ERROR;
    }
    if (base_graph != BASE_GRAPH_1 && base_graph != BASE_GRAPH_2)
    {
        return LDPC_BASE_GRAPH_INVALID;
    }
    if (input_length > MAX_BLOCK_LENGTH)
    {
        return LDPC_LENGTH_ERROR;
    }

    short BL = input_length * 8;
    auto result = ldpc_encoder_orig(input, encoded, BL, base_graph, 0);

    if (result < 0)
    {
        return LDPC_ENCODE_ALLOCATION_ERROR;
    }
    return result;
}

int ldpc_encode_full(unsigned char* input, unsigned char* encoded, int block_length, int base_graph)
{
    if (input == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (encoded == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (block_length <= 0)
    {
        return LDPC_LENGTH_ERROR;
    }
    if (base_graph != 1 && base_graph != 2)
    {
        return LDPC_BASE_GRAPH_INVALID;
    }
    if (block_length > MAX_BLOCK_LENGTH)
    {
        return LDPC_LENGTH_ERROR;
    }


    short BL = block_length * 8;
    auto result = ldpc_encoder_orig_full(input, encoded, BL, (short)base_graph);
    if (result.Start == -1 || result.Count == -1 || result.Length == 0)
    {
        return LDPC_ENCODE_FAILED;
    }

    return result.Length;
}

DecoderInfo create_decoder()
{
    t_nrLDPC_procBuf* _p_nrLDPC_procBuf = nrLDPC_init_mem();
    if (_p_nrLDPC_procBuf == NULL)
    {
        return (void*)NULL;
    }

    return (void*)_p_nrLDPC_procBuf;
}

void free_decoder(DecoderInfo decoder)
{
    t_nrLDPC_procBuf* _p_nrLDPC_procBuf = (t_nrLDPC_procBuf*) decoder;
    if (decoder == NULL)
    {
        // TODO: do we need to write a warning here?
    }
    else
    {
        nrLDPC_free_mem(_p_nrLDPC_procBuf);
    }
}

int32_t ldpc_decode(DecoderInfo decoder, uint8_t base_graph, uint16_t lifting_size, uint8_t decoding_rate, uint8_t max_iterations, int output_mode, int8_t* p_llr, unsigned char* p_llrOut)
{
    t_nrLDPC_procBuf* _p_nrLDPC_procBuf = (t_nrLDPC_procBuf*)decoder;
    if (_p_nrLDPC_procBuf == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (max_iterations == 0)
    {
        return LDPC_DECODER_INVALID_ITERATIONS;
    }
    if (output_mode < LDPC_OUTPUT_MODE_BIT || output_mode > LDPC_OUTPUT_MODE_LLR)
    {
        return LDPC_DECODER_INVALID_OUTPUT_MODE;
    }
    if (p_llr == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }
    if (p_llrOut == NULL)
    {
        return LDPC_ARGUMENT_NULL_ERROR;
    }

    t_nrLDPC_dec_params params;
    params.BG = base_graph;
    params.numMaxIter = max_iterations;
    params.R = decoding_rate;
    params.Z = lifting_size;

    switch (output_mode)
    {
    case LDPC_OUTPUT_MODE_BIT:
        params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_BIT;
        break;

    case LDPC_OUTPUT_MODE_BIT_INT8:
        params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_BITINT8;
        break;

    case LDPC_OUTPUT_MODE_LLR:
        params.outMode = e_nrLDPC_outMode::nrLDPC_outMode_LLRINT8;
        break;

    default:
        return LDPC_DECODER_INVALID_OUTPUT_MODE;
    }

    t_nrLDPC_time_stats profiler;
    auto result = nrLDPC_decoder(&params, p_llr, (int8_t*)p_llrOut, _p_nrLDPC_procBuf, &profiler);
    return result;
}
