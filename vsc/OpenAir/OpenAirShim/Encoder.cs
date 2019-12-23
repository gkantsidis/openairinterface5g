

namespace OpenAirShim
{
    using OpenAirShim.Properties;
    using System;
    using System.Diagnostics.Contracts;
    using System.Runtime.InteropServices;

    public sealed class Encoder
    {
        public static void Encode(byte[] input, byte[] output, BaseGraph bg)
        {
            if (input is null)
            {
                throw new ArgumentNullException(nameof(input));
            }
            if (output is null)
            {
                throw new ArgumentNullException(nameof(output));
            }
            if (input.Length != LdpcConstants.PacketSize)
            {
                throw new OpenAirShimException($"Packet sizes need to have exactly {LdpcConstants.PacketSize}, it has {input.Length}");
            }
            if (output.Length < LdpcConstants.BufferSize)
            {
                throw new OpenAirShimException($"Buffer size must be {LdpcConstants.BufferSize} bytes long; it is {output.Length}");
            }
            Contract.EndContractBlock();

            GCHandle input_handle = GCHandle.Alloc(input);
            GCHandle output_handle = GCHandle.Alloc(output);

            try
            {
                var input_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(input, 0);
                var output_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(output, 0);

                var result = SafeNativeMethods.Encode(input_ptr, input.Length, output_ptr, (int)bg);

                if (result != 0)
                {
                    throw new OpenAirShimException($"Encoding error {result}");
                }
            }
            catch(Exception ex)
            {
                throw new OpenAirShimException(Resources.MessageErrorCallingEncoder, ex);
            }
            finally
            {
                input_handle.Free();
                output_handle.Free();
            }
        }

        public static void EncodeFull(byte[] input, byte[] output, BaseGraph bg)
        {
            if (input is null)
            {
                throw new ArgumentNullException(nameof(input));
            }
            if (output is null)
            {
                throw new ArgumentNullException(nameof(output));
            }
            if (input.Length != LdpcConstants.PacketSize)
            {
                throw new OpenAirShimException($"Packet sizes need to have exactly {LdpcConstants.PacketSize}, it has {input.Length}");
            }
            if (output.Length < LdpcConstants.BufferSize)
            {
                throw new OpenAirShimException($"Buffer size must be {LdpcConstants.BufferSize} bytes long; it is {output.Length}");
            }
            Contract.EndContractBlock();

            GCHandle input_handle = GCHandle.Alloc(input);
            GCHandle output_handle = GCHandle.Alloc(output);

            try
            {
                var input_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(input, 0);
                var output_ptr = Marshal.UnsafeAddrOfPinnedArrayElement(output, 0);

                var result = SafeNativeMethods.EncodeFull(input_ptr, input.Length, output_ptr, (int)bg);

                if (result != 0)
                {
                    throw new OpenAirShimException($"Encoding error {result}");
                }
            }
            catch (Exception ex)
            {
                throw new OpenAirShimException(Resources.MessageErrorCallingEncoder, ex);
            }
            finally
            {
                input_handle.Free();
                output_handle.Free();
            }
        }
    }
}
