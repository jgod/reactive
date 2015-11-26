//
//  main.cpp
//  component
//
//  Created by Justin Godesky on 11/23/15.
//
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/reactive.h"
using namespace jgod;

class TestComponent : public reactive::Component {
public:
  TestComponent(){}
  TestComponent(const std::string type,
                reactive::Props props,
                reactive::NodeList children)
  : reactive::Component(type, props, children){}
  virtual ~TestComponent(){};
  virtual void render(bool force = false) const override {}
};

reactive::SharedComponent createTestComponent() {
  return std::make_shared<TestComponent>("test", reactive::Props(), reactive::NodeList());
}

TEST_CASE("Children") {
  SECTION("Initializing components with children") {
    auto child = std::make_shared<TestComponent>();
    auto children = reactive::NodeList{child};
    auto component = std::make_shared<TestComponent>("test",
                                                     reactive::Props(),
                                                     children);
    REQUIRE(component->getChildren().size() == 1);

    SECTION("Setting childrens' parent") {
      REQUIRE(child->getParent() == component.get());
    }
  }

  SECTION("Adding children to component") {
    auto child = std::make_shared<TestComponent>();
    auto component = createTestComponent();
    component->addChild(child);
    REQUIRE(component->getChildren().size() == 1);

    SECTION("Setting childrens' parent") {
      REQUIRE(child->getParent() == component.get());
    }
  }

  SECTION("Removing child from component") {
    auto child = std::make_shared<TestComponent>();
    auto component = createTestComponent();
    component->addChild(child);
    component->removeChild(child);
    REQUIRE(component->getChildren().empty());
  }

  SECTION("Removing children from component") {
    auto child1 = std::make_shared<TestComponent>();
    auto child2 = std::make_shared<TestComponent>();
    auto component = createTestComponent();
    component->removeChildren();
    REQUIRE(component->getChildren().empty());
  }
}

TEST_CASE("State") {
  SECTION("Modifying state through setState()") {
    auto component = createTestComponent();
    component->setState(reactive::JSON::parse("{\"key\": \"value\"}"));
    REQUIRE(component->getState()["key"] == "value");
  }
}
