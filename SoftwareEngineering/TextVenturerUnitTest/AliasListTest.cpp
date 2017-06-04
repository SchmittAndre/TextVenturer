#include "stdafx.h"
#include "CppUnitTest.h"

#include <stdafx.h>
#include <AliasList.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AliasListTest
{		
	TEST_CLASS(AliasListTest)
	{
	public:
		TEST_METHOD(TestAdd)
		{
            AliasList a;
            a.add("test");
            Assert::IsTrue(a.has("test"));
		}

		TEST_METHOD(TestDel)
		{
			AliasList a;
			a.add("test");
			a.del("test");
			Assert::IsFalse(a.has("test"));
		}

		TEST_METHOD(TestDelWithWrongstring)
		{
			AliasList a;
			a.add("test");
			a.del("tes");
			Assert::IsTrue(a.has("test"));
		}
		
		TEST_METHOD(TestAddDuplicate)
		{
			AliasList a;
			a.add("test");
			Assert::IsFalse(a.add("test"));
		}

		TEST_METHOD(TestGetName)
		{
			AliasList a;
			a.add("test");
			a.add("something else");
			std::string name = a.getName();
			bool assert = name == "a test";
			Assert::IsTrue(assert);
		}
		TEST_METHOD(TestGetNameonly)
		{
			AliasList a;
			a.add("test");
			a.add("something else");
			std::string name = a.getNameOnly();
			bool assert = name == "test";
			Assert::IsTrue(assert);
		}
	};
}