#pragma once

#include <stdint.h>
#include "OpenAirCommon.h"

typedef struct _LDPC_Encoder_Info {
    int Start;
    int Count;
    int Length;
} LDPC_Encoder_Info;

typedef void* DecoderInfo;

#define MAX_BLOCK_LENGTH 1056
#define BUFFER_LENGTH    (68 * 384)

#define BASE_GRAPH_1 1
#define BASE_GRAPH_2 2

/*
 * Error Codes
 */
#define LDPC_ARGUMENT_NULL_ERROR        -100
#define LDPC_LENGTH_ERROR               -101
#define LDPC_BASE_GRAPH_INVALID         -102
#define LDPC_ARGUMENT_ERROR             -103

#define LDPC_ENCODE_ALLOCATION_ERROR            -1
#define LDPC_ENCODE_FAILED                      -2
#define LDPC_ENCODE_UNEXPECTED_OUTPUT_LENGTH    -3

#define LDPC_DECODER_INVALID_ITERATIONS     -50
#define LDPC_DECODER_INVALID_OUTPUT_MODE    -51

/// <summary>   Perform LDPC encoding. </summary>
/// <param name="input">        [in] The input bytes to encode; this *must* be MAX_BLOCK_LENGTH bytes in size. </param>
/// <param name="input_length"> Length of the input (*must* be MAX_BLOCK_LENGTH). </param>
/// <param name="encoded">      [out] Buffer where to store encoded bytes; this *must* be BUFFER_LENGTH bytes in size. </param>
/// <param name="base_graph">   The base graph to use (Use BASE_GRAPH_1). </param>
/// <returns>   0 in success; negative error code when in error. </returns>
extern "C" OPENAIRDLL_API int ldpc_encode_simple(unsigned char* input, int input_length, unsigned char* encoded, int base_graph);

/// <summary>   Perform LDPC encoding and keep the entire vector of generated encodings. </summary>
/// <param name="input">        [in] The input bytes to encode; this *must* be MAX_BLOCK_LENGTH bytes in size. </param>
/// <param name="input_length"> Length of the input (*must* be MAX_BLOCK_LENGTH). </param>
/// <param name="encoded">      [out] Buffer where to store encoded bytes; this *must* be BUFFER_LENGTH bytes in size. </param>
/// <param name="base_graph">   The base graph to use (Use BASE_GRAPH_1). </param>
/// <returns>   0 in success; negative error code when in error. </returns>
extern "C" OPENAIRDLL_API int ldpc_encode_full(unsigned char* input, int input_length, unsigned char* encoded, int base_graph);

extern "C" OPENAIRDLL_API DecoderInfo create_decoder();
extern "C" OPENAIRDLL_API void free_decoder(DecoderInfo decoder);

#define LDPC_OUTPUT_MODE_BIT 0
#define LDPC_OUTPUT_MODE_BIT_INT8 1
#define LDPC_OUTPUT_MODE_LLR 2

extern "C" OPENAIRDLL_API int32_t ldpc_decode(DecoderInfo decoder, int base_graph, int lifting_size, int decoding_rate, int max_iterations, int output_mode, int8_t * p_llr, unsigned char* p_llrOut);
