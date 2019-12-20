

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

            var encoder = new Encoder();
            encoder.Encode(input, output, BaseGraph.BG1);
        }

        [TestMethod]
        public void EncodeRandomTest()
        {
            byte[] input = new byte[LdpcConstants.PacketSize];
            byte[] output = new byte[LdpcConstants.BufferSize];

            int seed = 13;
            var rng = new Random(seed);
            rng.NextBytes(input);

            var encoder = new Encoder();
            encoder.Encode(input, output, BaseGraph.BG1);
        }
    }
}
