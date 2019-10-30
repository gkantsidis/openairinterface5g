// ldpctest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cassert>

extern "C"
{
#include "nrLDPC_encoder/defs.h"
#include <nrLDPC_decoder\nrLDPC_init_mem.h>
#include <nrLDPC_decoder\nrLDPC_decoder.h>
}

int opp_enabled = 0;

typedef struct _configuration {
    short BG;
    short Kb;
    short nrows;
} Configuration;

Configuration find_configuration(short block_length)
{
    short BG, Kb, nrows;

    if (block_length > 3840) {
        BG = 1;
        Kb = 22;
        nrows = 46;             //parity check bits
        //ncols=22; //info bits
    }
    else if (block_length <= 3840) {
        BG = 2;
        nrows = 42;             //parity check bits
        //ncols=10; // info bits

        if (block_length > 640)
            Kb = 10;
        else if (block_length > 560)
            Kb = 9;
        else if (block_length > 192)
            Kb = 8;
        else
            Kb = 6;
    }

    Configuration result;
    result.BG = BG;
    result.Kb = Kb;
    result.nrows = nrows;

    return result;
}

typedef struct _ErrorStatistics {
    unsigned int errors;
    unsigned int errors_bit;
    double errors_bit_uncoded;
    unsigned int crc_misses;
} ErrorStatistics;

typedef enum _Channel {
    Ideal = 0,
    Binary_AWG = 1
} Channel;

typedef struct _AWG_Channel
{
    double SNR;
    unsigned char qbits;
} AWG_Channel;

char quantize(double D, double x, unsigned char B)
{
    double qxd;
    short maxlev;
    qxd = floor(x / D);

    maxlev = 1 << (B - 1);      //(char)(pow(2,B-1));

    //printf("x=%f,qxd=%f,maxlev=%d\n",x,qxd, maxlev);

    if (qxd <= -maxlev)
        qxd = -maxlev;
    else if (qxd >= maxlev)
        qxd = (long long)maxlev - 1;

    return ((char)qxd);
}

int
apply_ideal_channel(
    unsigned char* input, int start_input, int end_input,
    char* output, int start_output, int end_output)
{
    short maxlev = 1 << (8 - 1);
    for (int i = start_input, j = start_output; i < end_input && j < end_output; i++, j++)
    {
        double value = (input[i] == 0) ? 1.0 : -1.0;
        output[j] = (char)((input[i] == 0) ? (maxlev - 1) : -maxlev);
    }

    return 0;
}

int
apply_channel(
    Channel channel_type, void* channel,
    unsigned char* input, int start_input, int end_input,
    char* output, int start_output, int end_output
)
{
    switch (channel_type)
    {
    case Ideal:
        return apply_ideal_channel(
            input, start_input, end_input,
            output, start_output, end_output
        );

    case Binary_AWG:
        fprintf(stderr, "Not implemented\n");
        abort();
        break;

    default:
        fprintf(stderr, "Unknown channel");
        abort();
        break;
    }
}

short lift_size[51] =
{ 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24,
26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112,
120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
};

short find_Zc(short block_length, short Kb)
{
    short Zc = 0;

    for (int i1 = 0; i1 < 51; i1++) {
        if (lift_size[i1] >= (double)block_length / Kb) {
            Zc = lift_size[i1];
            //printf("%d\n",Zc);
            break;
        }
    }

    return Zc;
}

int find_R_ind(int nom_rate, int denom_rate, short BG)
{
    int R_ind = 0;

    if (nom_rate == 1)
        if (denom_rate == 5)
            if (BG == 2)
                R_ind = 0;
            else
                printf("Not supported");
        else if (denom_rate == 3)
            R_ind = 1;
        else if (denom_rate == 2)
            //R_ind = 3;
            printf("Not supported");
        else
            printf("Not supported");

    else if (nom_rate == 2)
        if (denom_rate == 5)
            //R_ind = 2;
            printf("Not supported");
        else if (denom_rate == 3)
            R_ind = 4;
        else
            printf("Not supported");

    else if ((nom_rate == 22) && (denom_rate == 30))
        //R_ind = 5;
        printf("Not supported");
    else if ((nom_rate == 22) && (denom_rate == 27))
        //R_ind = 6;
        printf("Not supported");
    else if ((nom_rate == 22) && (denom_rate == 25))
        if (BG == 1)
            R_ind = 7;
        else
            printf("Not supported");
    else
        printf("Not supported");

    return R_ind;
}

#define malloc16(x) memalign(32, x)

void initialize_random(unsigned char* input, const int block_length, unsigned int seed)
{
    srand(seed);
    for (int i = 0; i < block_length / 8; i++) {
        input[i] = (unsigned char)rand();
    }
}

void initialize_constant(unsigned char* input, const int block_length, unsigned char value)
{
    for (int i = 0; i < block_length / 8; i++) {
        input[i] = value;
    }
}

void initialize_pattern(unsigned char* input, const int block_length, unsigned char periodicity)
{
    unsigned char value = 0;
    for (int i = 0; i < block_length / 8; i++, value++) {
        if (value >= periodicity)
        {
            value = 0;
        }
        input[i] = value;
    }
}

int main()
{
    int n_segments = 1;
    int nom_rate = 1;
    int denom_rate = 3;
    short No_iteration = 5;

    int code_rate_vec[8] = { 15, 13, 25, 12, 23, 34, 56, 89 };

    short block_length = 8448;
    auto p_nrLDPC_procBuf = nrLDPC_init_mem();

    t_nrLDPC_time_stats decoder_profiler;
    t_nrLDPC_time_stats* p_decoder_profiler = &decoder_profiler;

    reset_meas(&decoder_profiler.llr2llrProcBuf);
    reset_meas(&decoder_profiler.llr2CnProcBuf);
    reset_meas(&decoder_profiler.cnProc);
    reset_meas(&decoder_profiler.cnProcPc);
    reset_meas(&decoder_profiler.bnProc);
    reset_meas(&decoder_profiler.bnProcPc);
    reset_meas(&decoder_profiler.cn2bnProcBuf);
    reset_meas(&decoder_profiler.bn2cnProcBuf);
    reset_meas(&decoder_profiler.llrRes2llrOut);
    reset_meas(&decoder_profiler.llr2bit);

    unsigned char* test_input = NULL;
    unsigned char* channel_input = NULL;
    char* channel_output_fixed = NULL;
    unsigned char* estimated_output = NULL;

    Configuration configuration = find_configuration(block_length);
    int R_ind = find_R_ind(nom_rate, denom_rate, configuration.BG);

    ErrorStatistics errors = { 0, 0, 0, 0 };

    srand(14);

    short Zc = find_Zc(block_length, configuration.Kb);

    t_nrLDPC_dec_params decParams;
    decParams.BG = configuration.BG;
    decParams.Z = Zc;
    decParams.R = code_rate_vec[R_ind]; //13;
    decParams.numMaxIter = No_iteration;
    decParams.outMode = nrLDPC_outMode_BIT;

    int no_punctured_columns =
        (int)((configuration.nrows - 2) * Zc + block_length -
            block_length * (1 /
            ((float)nom_rate / (float)denom_rate))) / Zc
        ;
        
    int removed_bit = (configuration.nrows - no_punctured_columns - 2) * Zc +
        block_length -
        (int)(block_length / ((float)nom_rate / (float)denom_rate));

    test_input = (unsigned char*)malloc16(sizeof(unsigned char) * block_length / 8);
    channel_input = (unsigned char*)malloc16(sizeof(unsigned char) * 68 * 384);
    channel_output_fixed = (char*)malloc16(sizeof(char) * 68 * 384);
    estimated_output = (unsigned char*)malloc16(sizeof(unsigned char) * block_length);

    assert(test_input != NULL);
    assert(channel_input != NULL);
    assert(channel_output_fixed != NULL);
    assert(estimated_output != NULL);

    for (size_t attempts = 0; attempts < 1; attempts++)
    {
        //initialize_random(test_input, block_length, attempts);
        initialize_constant(test_input, block_length, 0);
        //initialize_constant(test_input, block_length, 5);
        //initialize_pattern(test_input, block_length, 2);

        memset(channel_input, 0x00, sizeof(unsigned char) * 68 * 384);
        memset(channel_output_fixed, 0x00, sizeof(char) * 68 * 384);

        ldpc_encoder_orig(test_input, channel_input,
            block_length, configuration.BG, 0);

        int last_bit_pos =
            (configuration.Kb + configuration.nrows - no_punctured_columns) * decParams.Z - removed_bit;

        int uncoded_errors = apply_channel(
            Ideal, NULL,
            channel_input, 0, last_bit_pos - 2 * decParams.Z,
            channel_output_fixed, 2 * decParams.Z, last_bit_pos
        );

        errors.errors_bit_uncoded += uncoded_errors;

        int n_iter = nrLDPC_decoder(
            &decParams,
            (int8_t*)channel_output_fixed,
            (int8_t*)estimated_output,
            p_nrLDPC_procBuf, p_decoder_profiler);

        bool has_segment_error = false;
        for (int i = 0; i < (block_length >> 3); i++) {
            if (estimated_output[i] != test_input[i]) {
                auto diff = estimated_output[i] ^ test_input[i];
                while (diff != 0) {
                    diff = diff & (diff - 1);
                    errors.errors_bit++;
                }
                has_segment_error = true;
            }
        }
        if (has_segment_error) errors.errors++;
    }

    _aligned_free(test_input);
    _aligned_free(channel_input);
    _aligned_free(channel_output_fixed);
    _aligned_free(estimated_output);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
