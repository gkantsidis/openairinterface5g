#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>
#include <iostream>
#include <fstream>

#include "utilities.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace LDPCTests
{
    unique_ptr<char> read_binary_file(const string& filename, size_t expected_length)
    {
        Assert::IsTrue(std::filesystem::exists(filename));

        ifstream ifs(filename, ios::binary | ios::ate);
        ifstream::pos_type pos = ifs.tellg();
        int length = pos;

        Assert::IsTrue(length >= 0);
        if (expected_length > 0)
        {
            Assert::AreEqual(expected_length, (size_t)length);
        }

        std::unique_ptr<char> encoding(new char[length]);
        ifs.seekg(0, std::ios::beg);
        ifs.read(encoding.get(), length);
        ifs.close();

        return std::move(encoding);
    }

}
