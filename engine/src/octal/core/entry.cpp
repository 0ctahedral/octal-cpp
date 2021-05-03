#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/application.h"
#include "platform/platform.h"
#include "octal/ecs/ecs.h"
#include "octal/ecs/compstore.h"

struct dummy {
  int a, b, c;
};

using namespace octal;
// Inform the compiler that the create application function is somewhere else
extern octal::Application* octal::CreateApplication();

int main(int argc, char** argv) {

  ECS ecs;

  u32 e1 = ecs.CreateEntity();
  DEBUG("Created new entity: %d", e1);
  {
  ecs.AddComponent<dummy>(e1, {0, 1, 2});
  auto c = ecs.GetComponent<dummy>(e1);
  DEBUG("Entity %d has component dummy: a=%d, b=%d, c=%d", e1, c->a, c->b, c->c);
  }
  ecs.RemoveComponent<dummy>(e1);
  auto c = ecs.GetComponent<dummy>(e1);
  DEBUG("Entity %d has does not have component: %p", e1, c);

	/*
	// create user defined application
	auto app = octal::CreateApplication();
	if (!app) {
    FATAL("Could not create application!");
  }
  // run itself!
	app->Run();
	delete app;
	*/

	return 0;
}
