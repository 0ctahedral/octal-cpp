#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/application.h"
#include "platform/platform.h"
#include "octal/ecs/scene.h"
#include "octal/ecs/entity.h"

struct dummy {
  int a, b, c;
};

struct poopy {
  int g, e, f;
};

using namespace octal;
// Inform the compiler that the create application function is somewhere else
extern octal::Application* octal::CreateApplication();

int main(int argc, char** argv) {
  Scene scene;

  Entity e1 = scene.CreateEntity();
  e1.AddComponent<dummy>({100, 200, 300});
  e1.AddComponent<poopy>({55, 13, 32});

  auto d = e1.GetComponent<dummy>();
  DEBUG("D: a %d b %d c %d", d->a, d->b, d->c);

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
