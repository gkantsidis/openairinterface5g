
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
        public void DecodeAllZero()
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bytes = 1056;
            var nominator = 1;
            var denominator = 3;

            var data = Sources.Constant.Zero(size_in_bytes);

            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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
        public void DecodeAllZeroSmall()
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bytes = 64;
            var nominator = 1;
            var denominator = 3;

            var data = Sources.Constant.Zero(size_in_bytes);

            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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
        public void DecodeAllZeroOneByte()
        {
            var levels = 8;
            var maximum_iterations = 5;

            var size_in_bytes = 1;
            var nominator = 1;
            var denominator = 3;

            var data = Sources.Constant.Zero(size_in_bytes);

            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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
        public void DecodeAllOne()
        {
            int nominator = 1;
            int denominator = 3;
            int size_in_bytes = 1056;

            var levels = 8;
            var maximum_iterations = 5;

            var data = Sources.Constant.One(size_in_bytes);
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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

            var data = Sources.Random.Make(size_in_bytes, seed);
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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

            var data = Sources.Random.Make(size_in_bytes, seed);
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximum_iterations, size_in_bytes);

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
