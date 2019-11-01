
namespace OpenAir.NET.Tests
{
    using System;
    using System.IO;
    using Xunit;
    using OpenAir.LDPC;
    using OpenAir.Bench;
    using System.Runtime.InteropServices;

    public class Decoder
    {
        [Fact]
        public void DecodeAllAzero()
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bytes = 1056;
            var nominator = 1;
            var denominator = 3;

            var size_in_bits = size_in_bytes * 8;
            var data = Sources.Constant.Zero(size_in_bytes);

            var configuration = Configuration.MkFromBlockLength(size_in_bits, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bits);

            Assert.Equal(data.Length, result.Length);
            for (int i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    string.Format("Data differ in position {0}; expected {1} got {2}",
                    i, data[i], result[i]));
            }
        }

        [Fact]
        public void Block128Test()
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bytes = 128;
            var nominator = 1;
            var denominator = 3;
            var seed = 50;

            var size_in_bits = size_in_bytes * 8;
            var data = Sources.Random.Make(size_in_bytes, seed);
            var configuration = Configuration.MkFromBlockLength(size_in_bits, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bits);

            Assert.Equal(data.Length, result.Length);
            for (int i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    string.Format("Data differ in position {0}; expected {1} got {2}",
                    i, data[i], result[i]));
            }
        }

        [Theory()]
        [InlineData(14, 2, 3, 1056)]
        [InlineData(14, 1, 3, 1056)]
        [InlineData(24, 2, 3, 1056)]
        [InlineData(24, 1, 3, 1056)]
        [InlineData(34, 2, 3, 1056)]
        [InlineData(34, 1, 3, 1056)]
        [InlineData(44, 2, 3, 1056)]
        [InlineData(44, 1, 3, 1056)]
        [InlineData(44, 1, 3, 512)]
        [InlineData(44, 2, 3, 512)]
        [InlineData(44, 1, 3, 256)]
        [InlineData(44, 2, 3, 256)]
        [InlineData(44, 1, 3, 128)]
        [InlineData(44, 2, 3, 128)]
        public void RandomBlockTest(int seed, int nominator, int denominator, int size_in_bytes)
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bits = size_in_bytes * 8;
            var data = Sources.Random.Make(size_in_bytes, seed);
            var configuration = Configuration.MkFromBlockLength(size_in_bits, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bits);

            Assert.Equal(data.Length, result.Length);
            for (int i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    string.Format("Data differ in position {0}; expected {1} got {2}",
                    i, data[i], result[i]));
            }
        }
    }
}
