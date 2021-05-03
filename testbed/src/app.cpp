#include <octal/core/logger.h>
#include <octal/core/application.h>

class Test : public octal::Application {
  public:
    Test(octal::Application::Config conf):
      octal::Application(conf)
      {
        // add layers and stuff
      }

    ~Test(){}
};

octal::Application* octal::CreateApplication() {
  return new Test({
      0, 0,
      1280, 920,
      "Testbed"
      });
}
