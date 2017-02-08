#include "stdafx.h"
#include "CppUnitTest.h"

#include <AliasList.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
            AliasList list("test");
            list.add("blub");
            list.del("test");

            Assert::IsTrue(list.has("blub"));
            Assert::IsFalse(list.has("test"));
		}

	};
}