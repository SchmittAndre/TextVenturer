#include "stdafx.h"
#include "CppUnitTest.h"

#include <../TextVenturer/stdafx.h>
#include <../TextVenturer/AliasList.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AliasListTest
{		
	TEST_CLASS(AliasListTest)
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
			Assert::AreEqual(false,a.add("test"));
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
			printf("%s", name);
			bool assert = name == "test";
			Assert::IsTrue(assert);
		}
	};
}