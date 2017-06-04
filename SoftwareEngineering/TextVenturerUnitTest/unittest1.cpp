#include "stdafx.h"
#include "CppUnitTest.h"

#include <../TextVenturer/stdafx.h>
#include <../TextVenturer/AliasList.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TextVenturerUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(Testadd)
		{
            AliasList a;
            a.add("test");
            Assert::IsTrue(a.has("test"));
		}

		TEST_METHOD(Testdel)
		{
			AliasList a;
			a.add("test");
			a.del("test");
			Assert::IsFalse(a.has("test"));
		}

	};
}