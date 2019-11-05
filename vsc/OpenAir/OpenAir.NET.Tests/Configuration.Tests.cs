
namespace OpenAir.NET.Tests
{
    using System;
    using System.IO;
    using Xunit;
    using OpenAir.LDPC;

    public class ConfigurationTests
    {
        [Fact]
        public void TestSizeOfOutputChannelForFullInput()
        {
            int nominator = 1;
            int denominator = 3;
            int size_in_bytes = 1056;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            var result = configuration.CountOfOutputChannel(size_in_bytes);
            Assert.Equal(10560, result);
        }
    }
}
