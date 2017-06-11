#include "stdafx.h"

#include <Location.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LocationTest
{
    TEST_CLASS(LocationTest)
    {

        TEST_METHOD(AliasTest)
        {
            Location loc;
            loc.getAliases().add("tree");
            loc.getAliases().add("big tree");
            loc.setDescription("a big tree");

            Assert::IsTrue(loc.getAliases().has("the tree"));
        }

		TEST_METHOD(DescriptionTest)
		{
			Location loc;
			loc.setDescription("a small tree, it's not even reach the wall");
			Assert::AreEqual("a small tree, it's not even reach the wall", loc.getDescription().c_str());
		}

		//InventoryTests
		TEST_METHOD(addInventoryTest)
		{
			Location loc;
			loc.addInventory("on");
			loc.addInventory("under");
			Assert::IsTrue(loc.hasInventory("on"));
			Assert::IsTrue(loc.hasInventory("under"));
		}

		TEST_METHOD(delInventoryTest)
		{
			Location loc;
			loc.addInventory("on");
			loc.delInventory("on");
			Assert::IsFalse(loc.hasInventory("on"));
		}


		//test names
		TEST_METHOD(getNameOnlyTest)
		{
			Location loc;
			loc.getAliases().add("tree");
			loc.getAliases().add("big tree");
			Assert::AreEqual("tree", loc.getNameOnly().c_str());
		}

		TEST_METHOD(getNameTest)
		{
			Location loc;
			loc.getAliases().add("tree");
			loc.getAliases().add("big tree");
			Assert::AreEqual("a tree", loc.getName().c_str());
		}

		//Flag Tests
		TEST_METHOD(SetFlagTest)
		{
			Location loc;
			loc.setFlag("TestFlag");
			Assert::IsTrue(loc.testFlag("TestFlag"));
		}

		TEST_METHOD(ClrFlagTest)
		{
			Location loc;
			loc.setFlag("TestFlag");
			loc.clearFlag("TestFlag");
			Assert::IsFalse(loc.testFlag("TestFlag"));
		}
		
    };
}