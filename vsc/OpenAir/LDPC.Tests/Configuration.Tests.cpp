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
            auto output_length = 1056;
            auto configuration = Configuration::MakeFromBlockLength(output_length, 1, 3);

            Assert::AreEqual(25344, configuration.OutputChannelCount(output_length));
        }
    };
}
