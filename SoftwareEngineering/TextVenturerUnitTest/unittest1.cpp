#include "stdafx.h"
#include "CppUnitTest.h"

#include <vector>
#include <AliasList.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TextVenturerUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
            AliasList a;
            a.add("test");
            Assert::IsTrue(a.has("test"));
		}

	};
}