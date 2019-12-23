

namespace OpenAirShim.Tests
{
    using System;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using OpenAirShim;

    [TestClass]
    public class EncodingTest
    {
        [TestMethod]
        public void EncodeZeroTest()
        {
            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.BufferSize];

            Encoder.Encode(input, output, BaseGraph.BG1);
            for (int i = 0; i < output.Length; i++)
            {
                Assert.IsTrue(output[i] == 0 || output[i] == 1);
            }
        }

        [TestMethod]
        public void EncodeRandomTest()
        {
            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.BufferSize];

            int seed = 13;
            var rng = new Random(seed);
            rng.NextBytes(input);

            Encoder.Encode(input, output, BaseGraph.BG1);
            for (int i = 0; i < output.Length; i++)
            {
                Assert.IsTrue(output[i] == 0 || output[i] == 1);
            }
        }

        [TestMethod]
        public void EncodeFullZeroTest()
        {
            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.BufferSize];

            Encoder.EncodeFull(input, output, BaseGraph.BG1);
            for (int i = 0; i < output.Length; i++)
            {
                Assert.IsTrue(output[i] == 0 || output[i] == 1);
            }
        }

        [TestMethod]
        public void EncodeFullRandomTest()
        {
            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.BufferSize];

            int seed = 13;
            var rng = new Random(seed);
            rng.NextBytes(input);

            Encoder.EncodeFull(input, output, BaseGraph.BG1);
            for (int i = 0; i < output.Length; i++)
            {
                Assert.IsTrue(output[i] == 0 || output[i] == 1);
            }
        }
    }
}
