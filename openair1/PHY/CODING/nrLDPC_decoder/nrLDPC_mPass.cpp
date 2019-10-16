#include <stdint.h>
#include <stdio.h>
#include <intrin.h>
#include <nrLDPC_decoder.h>
#include "nrLDPC_mPass.h"

#ifndef _WINDOWS
#define MAKE_CIRCULAR_SHIFT(type, name, index, base) \
    const type (*name) [index] = (type (*)) [index] base;
#else
template <typename T>
struct VectorAccessor
{
public:
    VectorAccessor(T* ptr)
        : _ptr{ ptr }
    {
    }

    constexpr T& operator[](size_t i) const noexcept
    {
        return _ptr[i];
    }

private:
    T* _ptr;
};

template <typename T>
struct MatrixAccessor
{
public:
    MatrixAccessor(const T** ptr, int dim1)
        : _ptr{ ptr }
        , _dim1{ dim1 }
    {}

    constexpr VectorAccessor<T> operator[](size_t i) const noexcept
    {
        char* p = (char*)_ptr;
        T* target = (T*) (p + i * _dim1 * sizeof(T));
        return VectorAccessor<T>(target);
    }

private:
    const T** _ptr;
    int _dim1;
};

#define MAKE_CIRCULAR_SHIFT(type, name, index, base) const MatrixAccessor<type> name(base, index);

#endif

extern "C"
void nrLDPC_llr2CnProcBuf_BG1(t_nrLDPC_lut* p_lut, int8_t* llr, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG1_R13[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG1_R13[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG1_R13[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG1_R13[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG7, lut_numCnInCnGroups_BG1_R13[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG1_R13[5], p_lut->circShift[5]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG9, lut_numCnInCnGroups_BG1_R13[6], p_lut->circShift[6]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG1_R13[7], p_lut->circShift[7]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG19, lut_numCnInCnGroups_BG1_R13[8], p_lut->circShift[8]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG3, lut_numCnInCnGroups_BG1_R13[0], p_lut->posBnInCnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG4, lut_numCnInCnGroups_BG1_R13[1], p_lut->posBnInCnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG5, lut_numCnInCnGroups_BG1_R13[2], p_lut->posBnInCnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG6, lut_numCnInCnGroups_BG1_R13[3], p_lut->posBnInCnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG7, lut_numCnInCnGroups_BG1_R13[4], p_lut->posBnInCnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG8, lut_numCnInCnGroups_BG1_R13[5], p_lut->posBnInCnProcBuf[5]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG9, lut_numCnInCnGroups_BG1_R13[6], p_lut->posBnInCnProcBuf[6]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG10, lut_numCnInCnGroups_BG1_R13[7], p_lut->posBnInCnProcBuf[7]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG19, lut_numCnInCnGroups_BG1_R13[8], p_lut->posBnInCnProcBuf[8]);

        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        int8_t* cnProcBuf = p_procBuf->cnProcBuf;
        uint32_t i;
        uint32_t j;

        uint32_t idxBn = 0;
        int8_t* p_cnProcBuf;
        uint32_t bitOffsetInGroup;

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            idxBn = lut_posBnInCnProcBuf_CNG3[j][0] * Z;

            nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG3[j][0]);
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG4[j][i] * Z;

                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG4[j][i]);

                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG5[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG5[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 6; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG6[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG6[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 7 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 7; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG7[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG7[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG8[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG8[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 9 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[6] * NR_LDPC_ZMAX;

        for (j = 0; j < 9; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[6] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[6]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG9[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG9[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[7] * NR_LDPC_ZMAX;

        for (j = 0; j < 10; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[7] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[7]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG10[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG10[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 19 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[8] * NR_LDPC_ZMAX;

        for (j = 0; j < 19; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[8] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[8]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG19[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG19[j][i]);
                p_cnProcBuf += Z;
            }
        }

    }

extern "C"
void nrLDPC_llr2CnProcBuf_BG2(t_nrLDPC_lut* p_lut, int8_t* llr, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG2_R15[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG2_R15[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG2_R15[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG2_R15[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG2_R15[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG2_R15[5], p_lut->circShift[5]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG3, lut_numCnInCnGroups_BG2_R15[0], p_lut->posBnInCnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG4, lut_numCnInCnGroups_BG2_R15[1], p_lut->posBnInCnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG5, lut_numCnInCnGroups_BG2_R15[2], p_lut->posBnInCnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG6, lut_numCnInCnGroups_BG2_R15[3], p_lut->posBnInCnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG8, lut_numCnInCnGroups_BG2_R15[4], p_lut->posBnInCnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_posBnInCnProcBuf_CNG10, lut_numCnInCnGroups_BG2_R15[5], p_lut->posBnInCnProcBuf[5]);

        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        int8_t* cnProcBuf = p_procBuf->cnProcBuf;
        uint32_t i;
        uint32_t j;

        uint32_t idxBn = 0;
        int8_t* p_cnProcBuf;
        uint32_t bitOffsetInGroup;

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[0]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG3[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG3[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG4[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG4[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG5[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG5[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 6; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG6[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG6[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG8[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG8[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 10; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_posBnInCnProcBuf_CNG10[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG10[j][i]);
                p_cnProcBuf += Z;
            }
        }
    }

extern "C"
void nrLDPC_cn2bnProcBuf_BG2(t_nrLDPC_lut* p_lut, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG2_R15[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG2_R15[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG2_R15[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG2_R15[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG2_R15[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG2_R15[5], p_lut->circShift[5]);

        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->startAddrBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->startAddrBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->startAddrBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->startAddrBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG8, lut_numCnInCnGroups[4], p_lut->startAddrBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG10, lut_numCnInCnGroups[5], p_lut->startAddrBnProcBuf[5]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->bnPosBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->bnPosBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->bnPosBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->bnPosBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG8, lut_numCnInCnGroups[4], p_lut->bnPosBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG10, lut_numCnInCnGroups[5], p_lut->bnPosBnProcBuf[5]);

        int8_t* cnProcBufRes = p_procBuf->cnProcBufRes;
        int8_t* bnProcBuf = p_procBuf->bnProcBuf;

        int8_t* p_cnProcBufRes;
        uint32_t bitOffsetInGroup;
        uint32_t i;
        uint32_t j;
        uint32_t idxBn = 0;

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[0]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG3[j][i] + lut_bnPosBnProcBuf_CNG3[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG3[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG4[j][i] + lut_bnPosBnProcBuf_CNG4[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG4[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG5[j][i] + lut_bnPosBnProcBuf_CNG5[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG5[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 6; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG6[j][i] + lut_bnPosBnProcBuf_CNG6[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG6[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG8[j][i] + lut_bnPosBnProcBuf_CNG8[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG8[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 10; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG10[j][i] + lut_bnPosBnProcBuf_CNG10[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG10[j][i]);
                p_cnProcBufRes += Z;
            }
        }
    }

extern "C"
void nrLDPC_cn2bnProcBuf_BG1(t_nrLDPC_lut* p_lut, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG1_R13[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG1_R13[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG1_R13[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG1_R13[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG7, lut_numCnInCnGroups_BG1_R13[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG1_R13[5], p_lut->circShift[5]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG9, lut_numCnInCnGroups_BG1_R13[6], p_lut->circShift[6]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG1_R13[7], p_lut->circShift[7]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG19, lut_numCnInCnGroups_BG1_R13[8], p_lut->circShift[8]);

        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->startAddrBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->startAddrBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->startAddrBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->startAddrBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG7, lut_numCnInCnGroups[4], p_lut->startAddrBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG8, lut_numCnInCnGroups[5], p_lut->startAddrBnProcBuf[5]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG9, lut_numCnInCnGroups[6], p_lut->startAddrBnProcBuf[6]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG10, lut_numCnInCnGroups[7], p_lut->startAddrBnProcBuf[7]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG19, lut_numCnInCnGroups[8], p_lut->startAddrBnProcBuf[8]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->bnPosBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->bnPosBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->bnPosBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG7, lut_numCnInCnGroups[4], p_lut->bnPosBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG8, lut_numCnInCnGroups[5], p_lut->bnPosBnProcBuf[5]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG9, lut_numCnInCnGroups[6], p_lut->bnPosBnProcBuf[6]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG10, lut_numCnInCnGroups[7], p_lut->bnPosBnProcBuf[7]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG19, lut_numCnInCnGroups[8], p_lut->bnPosBnProcBuf[8]);

        int8_t* cnProcBufRes = p_procBuf->cnProcBufRes;
        int8_t* bnProcBuf = p_procBuf->bnProcBuf;

        int8_t* p_cnProcBufRes;
        uint32_t bitOffsetInGroup;
        uint32_t i;
        uint32_t j;
        uint32_t idxBn = 0;

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            nrLDPC_inv_circ_memcpy(&bnProcBuf[lut_startAddrBnProcBuf_CNG3[j][0]], p_cnProcBufRes, Z, lut_circShift_CNG3[j][0]);
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG4[j][i] + lut_bnPosBnProcBuf_CNG4[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG4[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG5[j][i] + lut_bnPosBnProcBuf_CNG5[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG5[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 6; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG6[j][i] + lut_bnPosBnProcBuf_CNG6[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG6[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 7 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 7; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG7[j][i] + lut_bnPosBnProcBuf_CNG7[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG7[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG8[j][i] + lut_bnPosBnProcBuf_CNG8[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG8[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 9 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[6] * NR_LDPC_ZMAX;

        for (j = 0; j < 9; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[6] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[6]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG9[j][i] + lut_bnPosBnProcBuf_CNG9[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG9[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[7] * NR_LDPC_ZMAX;

        for (j = 0; j < 10; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[7] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[7]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG10[j][i] + lut_bnPosBnProcBuf_CNG10[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG10[j][i]);
                p_cnProcBufRes += Z;
            }
        }

        // =====================================================================
        // CN group with 19 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[8] * NR_LDPC_ZMAX;

        for (j = 0; j < 19; j++)
        {
            p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[8] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[8]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG19[j][i] + lut_bnPosBnProcBuf_CNG19[j][i] * Z;
                nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG19[j][i]);
                p_cnProcBufRes += Z;
            }
        }

    }

extern "C"
void nrLDPC_bn2cnProcBuf_BG2(t_nrLDPC_lut* p_lut, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG2_R15[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG2_R15[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG2_R15[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG2_R15[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG2_R15[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG2_R15[5], p_lut->circShift[5]);

        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->startAddrBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->startAddrBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->startAddrBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->startAddrBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG8, lut_numCnInCnGroups[4], p_lut->startAddrBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG10, lut_numCnInCnGroups[5], p_lut->startAddrBnProcBuf[5]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->bnPosBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->bnPosBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->bnPosBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->bnPosBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG8, lut_numCnInCnGroups[4], p_lut->bnPosBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG10, lut_numCnInCnGroups[5], p_lut->bnPosBnProcBuf[5]);

        int8_t* cnProcBuf = p_procBuf->cnProcBuf;
        int8_t* bnProcBufRes = p_procBuf->bnProcBufRes;

        int8_t* p_cnProcBuf;
        uint32_t bitOffsetInGroup;
        uint32_t i;
        uint32_t j;
        uint32_t idxBn = 0;

        // For CN groups 3 to 6 no need to send the last BN back since it's single edge
        // and BN processing does not change the value already in the CN proc buf

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 2; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[0]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG3[j][i] + lut_bnPosBnProcBuf_CNG3[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG3[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG4[j][i] + lut_bnPosBnProcBuf_CNG4[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG4[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG5[j][i] + lut_bnPosBnProcBuf_CNG5[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG5[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG6[j][i] + lut_bnPosBnProcBuf_CNG6[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG6[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG8[j][i] + lut_bnPosBnProcBuf_CNG8[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG8[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 10; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG10[j][i] + lut_bnPosBnProcBuf_CNG10[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG10[j][i]);
                p_cnProcBuf += Z;
            }
        }
    }

extern "C"
void nrLDPC_bn2cnProcBuf_BG1(t_nrLDPC_lut* p_lut, t_nrLDPC_procBuf* p_procBuf, uint16_t Z)
    {
        const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
        const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG3, lut_numCnInCnGroups_BG1_R13[0], p_lut->circShift[0]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG4, lut_numCnInCnGroups_BG1_R13[1], p_lut->circShift[1]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG5, lut_numCnInCnGroups_BG1_R13[2], p_lut->circShift[2]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG6, lut_numCnInCnGroups_BG1_R13[3], p_lut->circShift[3]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG7, lut_numCnInCnGroups_BG1_R13[4], p_lut->circShift[4]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG8, lut_numCnInCnGroups_BG1_R13[5], p_lut->circShift[5]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG9, lut_numCnInCnGroups_BG1_R13[6], p_lut->circShift[6]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG10, lut_numCnInCnGroups_BG1_R13[7], p_lut->circShift[7]);
        MAKE_CIRCULAR_SHIFT(uint16_t, lut_circShift_CNG19, lut_numCnInCnGroups_BG1_R13[8], p_lut->circShift[8]);

        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG3, lut_numCnInCnGroups[0], p_lut->startAddrBnProcBuf[0]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->startAddrBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->startAddrBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->startAddrBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG7, lut_numCnInCnGroups[4], p_lut->startAddrBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG8, lut_numCnInCnGroups[5], p_lut->startAddrBnProcBuf[5]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG9, lut_numCnInCnGroups[6], p_lut->startAddrBnProcBuf[6]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG10, lut_numCnInCnGroups[7], p_lut->startAddrBnProcBuf[7]);
        MAKE_CIRCULAR_SHIFT(uint32_t, lut_startAddrBnProcBuf_CNG19, lut_numCnInCnGroups[8], p_lut->startAddrBnProcBuf[8]);

        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG4, lut_numCnInCnGroups[1], p_lut->bnPosBnProcBuf[1]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG5, lut_numCnInCnGroups[2], p_lut->bnPosBnProcBuf[2]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG6, lut_numCnInCnGroups[3], p_lut->bnPosBnProcBuf[3]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG7, lut_numCnInCnGroups[4], p_lut->bnPosBnProcBuf[4]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG8, lut_numCnInCnGroups[5], p_lut->bnPosBnProcBuf[5]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG9, lut_numCnInCnGroups[6], p_lut->bnPosBnProcBuf[6]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG10, lut_numCnInCnGroups[7], p_lut->bnPosBnProcBuf[7]);
        MAKE_CIRCULAR_SHIFT(uint8_t, lut_bnPosBnProcBuf_CNG19, lut_numCnInCnGroups[8], p_lut->bnPosBnProcBuf[8]);

        int8_t* cnProcBuf = p_procBuf->cnProcBuf;
        int8_t* bnProcBufRes = p_procBuf->bnProcBufRes;

        int8_t* p_cnProcBuf;
        uint32_t bitOffsetInGroup;
        uint32_t i;
        uint32_t j;
        uint32_t idxBn = 0;

        // For CN groups 3 to 19 no need to send the last BN back since it's single edge
        // and BN processing does not change the value already in the CN proc buf

        // =====================================================================
        // CN group with 3 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0] * NR_LDPC_ZMAX;

        for (j = 0; j < 2; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];

            nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[lut_startAddrBnProcBuf_CNG3[j][0]], Z, lut_circShift_CNG3[j][0]);
        }

        // =====================================================================
        // CN group with 4 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[1] * NR_LDPC_ZMAX;

        for (j = 0; j < 3; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[1] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[1]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG4[j][i] + lut_bnPosBnProcBuf_CNG4[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG4[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 5 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[2] * NR_LDPC_ZMAX;

        for (j = 0; j < 4; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[2] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[2]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG5[j][i] + lut_bnPosBnProcBuf_CNG5[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG5[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 6 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[3] * NR_LDPC_ZMAX;

        for (j = 0; j < 5; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[3] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[3]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG6[j][i] + lut_bnPosBnProcBuf_CNG6[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG6[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 7 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[4] * NR_LDPC_ZMAX;

        for (j = 0; j < 6; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[4] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[4]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG7[j][i] + lut_bnPosBnProcBuf_CNG7[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG7[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 8 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[5] * NR_LDPC_ZMAX;

        for (j = 0; j < 7; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[5] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[5]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG8[j][i] + lut_bnPosBnProcBuf_CNG8[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG8[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 9 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[6] * NR_LDPC_ZMAX;

        for (j = 0; j < 8; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[6] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[6]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG9[j][i] + lut_bnPosBnProcBuf_CNG9[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG9[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 10 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[7] * NR_LDPC_ZMAX;

        for (j = 0; j < 9; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[7] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[7]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG10[j][i] + lut_bnPosBnProcBuf_CNG10[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG10[j][i]);
                p_cnProcBuf += Z;
            }
        }

        // =====================================================================
        // CN group with 19 BNs

        bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[8] * NR_LDPC_ZMAX;

        for (j = 0; j < 19; j++)
        {
            p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[8] + j * bitOffsetInGroup];

            for (i = 0; i < lut_numCnInCnGroups[8]; i++)
            {
                idxBn = lut_startAddrBnProcBuf_CNG19[j][i] + lut_bnPosBnProcBuf_CNG19[j][i] * Z;
                nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG19[j][i]);
                p_cnProcBuf += Z;
            }
        }

    }
