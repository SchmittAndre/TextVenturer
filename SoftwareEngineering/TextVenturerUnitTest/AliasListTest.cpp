#include "stdafx.h"
#include "CppUnitTest.h"

#include <stdafx.h>
#include <ErrorHandling.h>
#include <AliasList.h>
#include <StringUtils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AliasListTest
{		
	TEST_CLASS(AliasListTest)
	{
	public:
		const std::string Aliaslistname = "Aliaslist";
		enum TestMethodID
		{
			tmUnknown,
			tmFile
		};

		TestMethodID testMethod = TestMethodID::tmUnknown;

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

		TEST_METHOD(File)
		{
			testMethod = TestMethodID::tmFile;
			FileStream fs(strconv(Aliaslistname), std::ios::out);
			AliasList a;
			a.add("test");
			a.add("tests", true);
			a.add("hallo");
			a.save(fs);
			fs.close();

			AliasList b;
			FileStream fsIn(strconv(Aliaslistname), std::ios::in);
			b.load(fsIn);
			Assert::IsTrue(b.has("test"));
			Assert::IsTrue(b.has("tests"));
			Assert::IsTrue(b.has("hallo"));
			fsIn.close();
		}

		TEST_METHOD_CLEANUP(cleanup)
		{
			switch (testMethod)
			{
			case TestMethodID::tmFile:
				if (!DeleteFile(strconv(Aliaslistname).c_str()))
				{
					Logger::WriteMessage(("Error at " + Aliaslistname + ": " + getErrorString(GetLastError())).c_str());
				}
				break;
			default:

				break;
			}
		}

	};
}