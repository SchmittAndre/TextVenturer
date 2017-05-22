#include "stdafx.h"
#include "CppUnitTest.h"

#include <stdafx.h>
#include <AliasList.h>
#include <FileStream.h>

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

            {
                FileStream f(_T("output.bin"), 1);
                a.save(f);
            }

            {
                FileStream f(_T("output.bin"), 1);
                AliasList b;
                b.load(f);

                Assert::IsTrue(b.has("test"));
            }
		}

	};
}