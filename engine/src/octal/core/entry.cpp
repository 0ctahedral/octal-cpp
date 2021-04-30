#include "octal/core/logger.h"
#include "octal/core/application.h"
#include "platform/platform.h"

// Inform the compiler that the create application function is somewhere else
extern octal::Application* octal::CreateApplication();

int main(int argc, char** argv) {
	// create user defined application
	auto app = octal::CreateApplication();
	if (!app) {
    FATAL("Could not create application!");
  }
  // run itself!
	app->Run();
	//app->Shutdown();
	delete app;
	return 0;
}
