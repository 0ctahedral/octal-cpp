#include "octal/defines.h"
#include "octal/core/logger.h"
#include "octal/core/application.h"
#include "platform/platform.h"
#include "octal/ecs/compstore.h"

struct testcomp {
  int a, b, c;
};

using namespace octal;
// Inform the compiler that the create application function is somewhere else
extern octal::Application* octal::CreateApplication();

int main(int argc, char** argv) {
  // test cs
  CompStore<testcomp> cs;

  cs.Add<testcomp>(1, {1, 2 ,3});
  cs.Add<testcomp>(2, {4, 5, 6});
  cs.Remove(1);
  cs.Add<testcomp>(3, {1, 2 ,3});

  printf("4 vs %d\n", cs.Get(2)->a);
  if (auto comp = cs.Get(1)) {
    printf("4 vs %d\n", comp->a);
  } else {
    printf("entity 1 does not have a component\n");
  }

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
