// ldpctest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cassert>

extern "C"
{
#include "nrLDPC_encoder/defs.h"
#include "nrLDPC_decoder/nrLDPC_init_mem.h"
#include "nrLDPC_decoder/nrLDPC_decoder.h"
}

int opp_enabled = 0;

typedef struct {
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
    else {
    	// block_length <= 3840
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

typedef struct {
    unsigned int errors;
    unsigned int errors_bit;
    double errors_bit_uncoded;
    unsigned int crc_misses;
} ErrorStatistics;

typedef enum {
    Ideal = 0,
    Binary_AWG = 1
} Channel;

typedef struct
{
    double SNR;
    unsigned char qbits;
} AWG_Channel;

char quantize(const double D, const double x, const unsigned char B)
{
	double qxd = floor(x / D);
	const auto maximum_level = static_cast<short>(1 << (B - 1));      //(char)(pow(2,B-1));

    //printf("x=%f,qxd=%f,maximum_level=%d\n",x,qxd, maximum_level);

    if (qxd <= -maximum_level)
        qxd = -maximum_level;
    else if (qxd >= maximum_level)
        qxd = static_cast<double>(static_cast<long long>(maximum_level) - 1);

    return static_cast<char>(qxd);
}

int
apply_ideal_channel(
	const unsigned char* input, const int start_input, const int end_input,
    char* output, const int start_output, const int end_output)
{
	constexpr short maximum_level = 1 << (8 - 1);
    for (int i = start_input, j = start_output; i < end_input && j < end_output; i++, j++)
    {
        //double value = (input[i] == 0) ? 1.0 : -1.0;
        output[j] = static_cast<char>((input[i] == 0) ? (maximum_level - 1) : -maximum_level);
    }

    return 0;
}

int
apply_channel(
    Channel channel_type, void* channel,
    const unsigned char* input, const int start_input, const int end_input,
    char* output, const int start_output, const int end_output
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
        //break;

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        fprintf(stderr, "Unknown channel");
        abort();
        //break;
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
        if (lift_size[i1] >= static_cast<double>(block_length) / Kb) {
            Zc = lift_size[i1];
            //printf("%d\n",Zc);
            break;
        }
    }

    return Zc;
}

int find_R_ind(int numerator_rate, int denominator_rate, short BG)
{
    int R_ind = 0;

    if (numerator_rate == 1) {
        if (denominator_rate == 5) {
            if (BG == 2)
                R_ind = 0;
            else
                printf("Not supported");
        }
        else if (denominator_rate == 3)
            R_ind = 1;
        else if (denominator_rate == 2) {  // NOLINT(bugprone-branch-clone)
            //R_ind = 3;
            printf("Not supported");
        }
        else
            printf("Not supported");
    }
    else if (numerator_rate == 2)
        if (denominator_rate == 5) {  // NOLINT(bugprone-branch-clone)
            //R_ind = 2;
            printf("Not supported");
        }
        else if (denominator_rate == 3)
            R_ind = 4;
        else
            printf("Not supported");

    else if ((numerator_rate == 22) && (denominator_rate == 30)) {  // NOLINT(bugprone-branch-clone)
        //R_ind = 5;
        printf("Not supported");
    }
    else if ((numerator_rate == 22) && (denominator_rate == 27)) {
        //R_ind = 6;
        printf("Not supported");
    }
    else if ((numerator_rate == 22) && (denominator_rate == 25)) {
        if (BG == 1)
            R_ind = 7;
        else
            printf("Not supported");
    }
    else
        printf("Not supported");

    return R_ind;
}

#define malloc16(x) memalign(32, x)

void initialize_random(unsigned char* input, const int block_length, const unsigned int seed)
{
    srand(seed);
    for (int i = 0; i < block_length / 8; i++) {
        input[i] = static_cast<unsigned char>(rand());  // NOLINT(concurrency-mt-unsafe)
    }
}

void initialize_constant(unsigned char* input, const int block_length, const unsigned char value)
{
    for (int i = 0; i < block_length / 8; i++) {
        input[i] = value;
    }
}

void initialize_pattern(unsigned char* input, const int block_length, const unsigned char periodicity)
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
    // constexpr int n_segments = 1;
    constexpr int numerator_rate = 1;
    constexpr int denominator_rate = 3;
    constexpr short max_number_of_iterations = 5;

    int code_rate_vec[8] = { 15, 13, 25, 12, 23, 34, 56, 89 };

    constexpr short block_length = 8448;
    constexpr auto encoded_block_length = block_length * denominator_rate / numerator_rate;

    auto p_nrLDPC_procBuf = nrLDPC_init_mem();

    t_nrLDPC_time_stats decoder_profiler{};
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

    unsigned char* test_input = nullptr;
    unsigned char* channel_input = nullptr;
    char* channel_output_fixed = nullptr;
    unsigned char* estimated_output = nullptr;

    Configuration configuration = find_configuration(block_length);
    int R_ind = find_R_ind(numerator_rate, denominator_rate, configuration.BG);

    ErrorStatistics errors = { 0, 0, 0, 0 };

    srand(14);  // NOLINT(cert-msc51-cpp)

    short Zc = find_Zc(block_length, configuration.Kb);

    t_nrLDPC_dec_params decParams;
    decParams.BG = static_cast<uint8_t>(configuration.BG);
    decParams.Z = static_cast<uint16_t>(Zc);
    decParams.R = static_cast<uint8_t>(code_rate_vec[R_ind]); //13;
    decParams.numMaxIter = max_number_of_iterations;
    decParams.outMode = nrLDPC_outMode_BIT;

    int no_punctured_columns = ( (configuration.nrows - 2) * Zc + block_length - encoded_block_length ) / Zc;
        
    int removed_bit = (configuration.nrows - no_punctured_columns - 2) * Zc + block_length - encoded_block_length;

    test_input = static_cast<unsigned char*>(malloc16(sizeof(unsigned char) * block_length / 8));
    channel_input = static_cast<unsigned char*>(malloc16(sizeof(unsigned char) * 68 * 384));
    channel_output_fixed = static_cast<char*>(malloc16(sizeof(char) * 68 * 384));
    estimated_output = static_cast<unsigned char*>(malloc16(sizeof(unsigned char) * block_length));

    assert(test_input != NULL);
    assert(channel_input != NULL);
    assert(channel_output_fixed != NULL);
    assert(estimated_output != NULL);

    for (auto attempts = 0; attempts < 10; attempts++)
    {
        initialize_random(test_input, block_length, attempts);
        //initialize_constant(test_input, block_length, 0);
        //initialize_constant(test_input, block_length, 5);
        //initialize_pattern(test_input, block_length, 2);

        memset(channel_input, 0x00, sizeof(unsigned char) * 68 * 384);
        memset(channel_output_fixed, 0x00, sizeof(char) * 68 * 384);

        ldpc_encoder_orig(test_input, channel_input,
            block_length, configuration.BG, 0);

        int last_bit_pos =
            (configuration.Kb + configuration.nrows - no_punctured_columns) * decParams.Z - removed_bit;

        int uncoded_errors = apply_channel(
            Ideal, nullptr,
            channel_input, 0, last_bit_pos - 2 * decParams.Z,
            channel_output_fixed, 2 * decParams.Z, last_bit_pos
        );

        errors.errors_bit_uncoded += uncoded_errors;

        int number_of_iterations = nrLDPC_decoder(
            &decParams,
            reinterpret_cast<int8_t*>(channel_output_fixed),
            reinterpret_cast<int8_t*>(estimated_output),
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

        printf("At attempt %d: Finished after %d iterations with %d errors\n", attempts, number_of_iterations, errors.errors);
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
