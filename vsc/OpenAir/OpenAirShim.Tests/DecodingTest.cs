using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace OpenAirShim.Tests
{
    using OpenAirShim;

    [TestClass]
    public class DecodingTest
    {
        [TestMethod]
        public void DecodeAllZerosTest()
        {
            // This test builds on the observation that when the input is all zero's, then
            // the encoding is also all zeros.

            // For the purposes of this unit test, we fix the configuration parameters to the values that we expect from the library.
            var bg = BaseGraph.BG1;
            var liftingSize = 384;
            var decodingRate = 13;

            byte[] output = new byte[LdpcConstants.PacketSize];
            sbyte[] channel_output = new sbyte[LdpcConstants.BufferSize];

            // We assume a perfect channel.
            for (int i = 0; i < LdpcConstants.BufferSize; i++)
            {
                channel_output[i] = (sbyte)0x7F;
            }

            using (var decoder = new Decoder())
            {
                var number_of_iterations = decoder.Decode(channel_output, output, bg, liftingSize, decodingRate, 10);
            }

            for (int i = 0; i < LdpcConstants.PacketSize; i++)
            {
                Assert.AreEqual((byte)0, output[i], $"Expected zero at position {i} (it is {output[i]})");
            }
        }

        [TestMethod]
        public void SimpleDecodeTest()
        {
            // For the purposes of this unit test, we fix the configuration parameters to the values that we expect from the library.
            var bg = BaseGraph.BG1;
            var liftingSize = 384;
            var decodingRate = 13;

            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.PacketSize];
            byte[] channel_input = new byte[LdpcConstants.BufferSize];
            sbyte[] channel_output = new sbyte[LdpcConstants.BufferSize];

            int seed = 15;
            var rng = new Random(seed);
            rng.NextBytes(input);
            Encoder.EncodeFull(input, channel_input, bg);

            // We assume a perfect channel.
            for (int i = 0; i < LdpcConstants.BufferSize; i++)
            {
                channel_output[i] = (channel_input[i] == 0) ? (sbyte)0x7F : (sbyte)-0x80;
            }

            using (var decoder = new Decoder())
            {
                var number_of_iterations = decoder.Decode(channel_output, output, bg, liftingSize, decodingRate, 10);
            }

            for (int i = 0; i < LdpcConstants.PacketSize; i++)
            {
                Assert.AreEqual(input[i], output[i], $"Output vector differs to input at position {i} ({input[i]} <> {output[i]})");
            }
        }

        [TestMethod]
        public void SimpleDecodeWithReducedBitsTest()
        {
            // For the purposes of this unit test, we fix the configuration parameters to the values that we expect from the library.
            var bg = BaseGraph.BG1;
            var liftingSize = 384;
            var decodingRate = 13;

            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.PacketSize];
            byte[] channel_input = new byte[LdpcConstants.BufferSize];
            sbyte[] channel_output = new sbyte[LdpcConstants.BufferSize];

            int seed = 15;
            var rng = new Random(seed);
            rng.NextBytes(input);
            Encoder.EncodeFull(input, channel_input, bg);

            // We assume a perfect channel, but we send only a subset of the bits
            var start = 2 * liftingSize;
            var stop = start + 8 * input.Length + 284;
            for (int i = 0; i < LdpcConstants.BufferSize; i++)
            {
                if (i < start || i > stop)
                {
                    continue;
                }

                channel_output[i] = (channel_input[i] == 0) ? (sbyte)0x7F : (sbyte)-0x80;
            }

            using (var decoder = new Decoder())
            {
                var number_of_iterations = decoder.Decode(channel_output, output, bg, liftingSize, decodingRate, 10);
            }

            for (int i = 0; i < LdpcConstants.PacketSize; i++)
            {
                Assert.AreEqual(input[i], output[i], $"Output vector differs to input at position {i} ({input[i]} <> {output[i]})");
            }
        }
    }
}
