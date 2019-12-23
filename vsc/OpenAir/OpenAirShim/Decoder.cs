
namespace OpenAirShim
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Runtime.InteropServices;
    using OpenAirShim.Properties;

    public sealed class Decoder
        : IDisposable
    {

        public Decoder()
        {
            _decoder = SafeNativeMethods.CreateDecoder();
        }

        public int Decode(sbyte[] input, byte[] output, BaseGraph bg, int liftingSize, int decodingRate, int maxIterations = 10)
        {
            if (input is null)
            {
                throw new ArgumentNullException(nameof(input));
            }
            if (output is null)
            {
                throw new ArgumentNullException(nameof(output));
            }
            if (input.Length < LdpcConstants.BufferSize)
            {
                throw new OpenAirShimException($"Buffer sizes need to have exactly {LdpcConstants.BufferSize}, it has {input.Length}");
            }
            if (output.Length < LdpcConstants.PacketSize)
            {
                throw new OpenAirShimException($"Packet size must be {LdpcConstants.PacketSize} bytes long; it is {output.Length}");
            }
            Contract.EndContractBlock();

            GCHandle input_handle = GCHandle.Alloc(input);
            GCHandle output_handle = GCHandle.Alloc(output);

            try
            {
                var input_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(input, 0);
                var output_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(output, 0);

                var result = SafeNativeMethods.Decode(_decoder, (int)bg, liftingSize, decodingRate, maxIterations, (int)OutputMode.Bit, input_ptr, output_ptr);

                if (result <= 0)
                {
                    throw new OpenAirShimException($"Decoding error {result}");
                }

                return result;
            }
            catch (Exception ex)
            {
                throw new OpenAirShimException(Resources.MessageErrorCallingDecoder, ex);
            }
            finally
            {
                input_handle.Free();
                output_handle.Free();
            }
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        private void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects).
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.
                if (_decoder != IntPtr.Zero)
                {
                    SafeNativeMethods.FreeDecoder(_decoder);
                }
                _decoder = IntPtr.Zero;

                disposedValue = true;
            }
        }

        ~Decoder()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(false);
        }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            // TODO: uncomment the following line if the finalizer is overridden above.
            GC.SuppressFinalize(this);
        }
        #endregion

        private IntPtr _decoder;
    }
}
