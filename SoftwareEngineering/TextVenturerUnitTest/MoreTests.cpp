#include "stdafx.h"

#include <Location.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LocationTest
{
    TEST_CLASS(LocationTest)
    {

        TEST_METHOD(BasicTest)
        {
            Location loc;
            loc.getAliases().add("tree");
            loc.getAliases().add("big tree");
            loc.setDescription("a big tree");

            Assert::IsTrue(loc.getAliases().has("the tree"));
        }

    };
}