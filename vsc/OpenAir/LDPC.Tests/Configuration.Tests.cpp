#include "pch.h"
#include "CppUnitTest.h"

#include "Configuration.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LDPCTests
{
    TEST_CLASS(ConfigurationTests)
    {
    public:

        TEST_METHOD(TestSizeOfOutputChannelForFullInput)
        {
            auto output_length_in_bits = 8448;
            auto configuration = Configuration::MakeFromBlockLength(output_length_in_bits, 1, 3);

            Assert::AreEqual(10560, configuration.OutputChannelCount(output_length_in_bits));
        }
    };
}
