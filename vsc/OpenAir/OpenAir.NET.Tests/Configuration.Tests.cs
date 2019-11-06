
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
            Assert.Equal(25344, result);
        }

        [Fact]
        public void Test_64Byte_13()
        {
            int nominator = 1;
            int denominator = 3;
            int size_in_bytes = 64;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            Assert.Equal(BaseGraph.BaseGraph2, configuration.BG);
            Assert.Equal(64, configuration.Zc);
            Assert.Equal(8, configuration.Kb);
            Assert.Equal(42, configuration.Rows);
            Assert.Equal(24, configuration.NumberOfPuncturedColumns(size_in_bytes));
        }

        [Fact]
        public void Test_64Byte_23()
        {
            int nominator = 2;
            int denominator = 3;
            int size_in_bytes = 64;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            Assert.Equal(BaseGraph.BaseGraph2, configuration.BG);
            Assert.Equal(64, configuration.Zc);
            Assert.Equal(8, configuration.Kb);
            Assert.Equal(42, configuration.Rows);
            Assert.Equal(36, configuration.NumberOfPuncturedColumns(size_in_bytes));
        }

        [Fact]
        public void Test_1056Byte_13()
        {
            int nominator = 1;
            int denominator = 3;
            int size_in_bytes = 1056;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            Assert.Equal(BaseGraph.BaseGraph1, configuration.BG);
            Assert.Equal(384, configuration.Zc);
            Assert.Equal(22, configuration.Kb);
            Assert.Equal(46, configuration.Rows);
            Assert.Equal(0, configuration.NumberOfPuncturedColumns(size_in_bytes));
        }

        [Fact]
        public void Test_1056Byte_23()
        {
            int nominator = 2;
            int denominator = 3;
            int size_in_bytes = 1056;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            Assert.Equal(BaseGraph.BaseGraph1, configuration.BG);
            Assert.Equal(384, configuration.Zc);
            Assert.Equal(22, configuration.Kb);
            Assert.Equal(46, configuration.Rows);
            Assert.Equal(33, configuration.NumberOfPuncturedColumns(size_in_bytes));
        }

        [Fact]
        public void Test_1056Byte_2225()
        {
            int nominator = 22;
            int denominator = 25;
            int size_in_bytes = 1056;
            var configuration = Configuration.MkFromBlockLength(size_in_bytes, nominator, denominator);

            Assert.Equal(BaseGraph.BaseGraph1, configuration.BG);
            Assert.Equal(384, configuration.Zc);
            Assert.Equal(22, configuration.Kb);
            Assert.Equal(46, configuration.Rows);

            // The value of 41 works fine (the ldpctest runs correctly).
            // Interestingly, some other versions of the master repo return the value of 63,
            // however, the ldpctest crashes.
            Assert.Equal(41, configuration.NumberOfPuncturedColumns(size_in_bytes));
        }
    }
}
