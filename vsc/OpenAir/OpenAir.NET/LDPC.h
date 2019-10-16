#pragma once

#include "nrLDPC_encoder\defs.h"
#include "nrLDPC_decoder\nrLDPC_init_mem.h"
#include "nrLDPC_decoder\nrLDPC_decoder.h"

using namespace System;

namespace OpenAir::LDPC {

    public ref class Encoder
    {
        // TODO: Add your methods for this class here.
    };

    public ref class Decoder
    {
    public:
        Decoder()
        {
            _p_nrLDPC_procBuf = nrLDPC_init_mem();
        }

        ~Decoder()
        {
            if (_disposed)
            {
                return;
            }
            this->!Decoder();
            _disposed = true;
        }

        !Decoder()
        {
            if (_p_nrLDPC_procBuf != nullptr)
            {
                nrLDPC_free_mem(_p_nrLDPC_procBuf);
            }
            _p_nrLDPC_procBuf = nullptr;
        }

    private:
        t_nrLDPC_procBuf* _p_nrLDPC_procBuf;
        bool _disposed = false;
    };
}
