
namespace OpenAir.NET.Tests
{
    using Xunit;
    using LDPC;
    using Bench;

    public class Decoder
    {
        [Fact]
        public void DecodeAllZero()
        {
            const int levels = 8;
            const int maximumIterations = 5;

            const int sizeInBytes = 1056;
            const int numerator = 1;
            const int denominator = 3;

            var data = Sources.Constant.Zero(sizeInBytes);

            var configuration = Configuration.MkFromBlockLength(sizeInBytes, numerator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (var i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
            }
        }

        [Fact]
        public void DecodeAllZeroSmall()
        {
            const int levels = 8;
            const int maximumIterations = 5;

            const int sizeInBytes = 64;
            const int numerator = 1;
            const int denominator = 3;

            var data = Sources.Constant.Zero(sizeInBytes);

            var configuration = Configuration.MkFromBlockLength(sizeInBytes, numerator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (var i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
            }
        }

        [Fact]
        public void DecodeAllZeroOneByte()
        {
            const int levels = 8;
            const int maximumIterations = 5;

            const int sizeInBytes = 1;
            const int numerator = 1;
            const int denominator = 3;

            var data = Sources.Constant.Zero(sizeInBytes);

            var configuration = Configuration.MkFromBlockLength(sizeInBytes, numerator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (var i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
            }
        }

        [Fact]
        public void DecodeAllOne()
        {
            const int numerator = 1;
            const int denominator = 3;
            const int sizeInBytes = 1056;

            const int levels = 8;
            const int maximumIterations = 5;

            var data = Sources.Constant.One(sizeInBytes);
            var configuration = Configuration.MkFromBlockLength(sizeInBytes, numerator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (int i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
            }
        }

        [Fact]
        public void Block128Test()
        {
            const int levels = 8;
            const int maximumIterations = 5;

            const int sizeInBytes = 128;
            const int numerator = 1;
            const int denominator = 3;
            const int seed = 50;

            var data = Sources.Random.Make(sizeInBytes, seed);
            var configuration = Configuration.MkFromBlockLength(sizeInBytes, numerator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (var i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
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
        public void RandomBlockTest(int seed, int nominator, int denominator, int sizeInBytes)
        {
            const int levels = 8;
            const int maximumIterations = 5;

            var data = Sources.Random.Make(sizeInBytes, seed);
            var configuration = Configuration.MkFromBlockLength(sizeInBytes, nominator, denominator);

            var encoder = new SimpleEncoder();
            var decoder = new OpenAir.LDPC.Decoder();

            var channel_in = encoder.Encode(data, configuration);
            var channel_in_slice = configuration.SliceInputToChannel(channel_in, data.Length);
            var channel_out = SimpleEncoder.GetChannelOutputBuffer();
            var channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length);

            Quantizer.Binary.Map.Apply(levels, channel_in_slice, channel_out_slice);
            var result = decoder.Decode(channel_out, configuration, maximumIterations, sizeInBytes);

            Assert.Equal(data.Length, result.Length);
            for (var i = 0; i < data.Length; i++)
            {
                Assert.True(
                    data[i] == result[i],
                    $"Data differ in position {i}; expected {data[i]} got {result[i]}");
            }
        }
    }
}
