#include "stdafx.h"

#include <Location.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LocationTest
{
    TEST_CLASS(LocationTest)
    {

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

      TEST_METHOD(addInventoryAliasTest)
      {
        Location loc;
        Location::MultiInventory* loc2;
        loc2 = &(loc.addInventory("on"));
        loc2->addPrepositionAlias("on top");
        Assert::IsTrue(loc2->hasPrepositionAlias("on top"));
      }

      TEST_METHOD(delInventoryAliasTest)
      {
        Location loc;
        Location::MultiInventory* loc2;
        loc.addInventory("on");
        loc2 = &(loc.getInventory("on"));
        loc2 -> addPrepositionAlias("on top");
        loc2->addPrepositionAlias("on2");
        loc2 -> delPrepositionAlias("on top");
        Assert::IsTrue(loc2->hasPrepositionAlias("on2"));
        Assert::IsFalse(loc2->hasPrepositionAlias("on top"));
      }

      TEST_METHOD(getMultiInventoryNameTest)
      {
        Location loc;
        Location::MultiInventory* loc2;
        loc.addInventory("on");
        loc2 = &(loc.getInventory("on"));
        loc2->addPrepositionAlias("on top");
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