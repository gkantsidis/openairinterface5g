

namespace OpenAir.NET.Tests
{
    using System;
    using System.IO;
    using Xunit;
    using OpenAir.LDPC;

    public class Encoder
    {
        [Fact]
        public void AllZeroTest()
        {
            var length = 8448 / 8;
            byte[] data = new byte[length];
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = 0;
            }

            var configuration = Configuration.MkFromBlockLength(data.Length);
            var encoder = new SimpleEncoder();
            var channel_in = encoder.Encode(data, configuration);

            for (int i = 0; i < channel_in.Length; i++)
            {
                Assert.Equal(0, channel_in[i]);
            }
        }

        [Fact]
        public void AllOnesTest()
        {
            var length = 8448 / 8;
            byte[] data = new byte[length];
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = 1;
            }

            var configuration = Configuration.MkFromBlockLength(data.Length);
            var encoder = new SimpleEncoder();
            var channel_in = encoder.Encode(data, configuration);

            var filename = Path.Combine("Results", "encoder_default_8448_all_ones.bin");
            Assert.True(File.Exists(filename));
            var ground = File.ReadAllBytes(filename);

            Assert.Equal(ground.Length, channel_in.Length);
            for (int i = 0; i < channel_in.Length; i++)
            {
                Assert.True(
                    ground[i] == channel_in[i],
                    String.Format("Error in position {0}: expected {1}, got {2}",
                        i, ground[i], channel_in[i])
                );
            }
        }
    }
}
