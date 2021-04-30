#include <thread>
#include <octal/core/application.h>

class Test : public octal::Application {
  public:
    Test():
      octal::Application()
      {

      }

    ~Test(){}
};

octal::Application* octal::CreateApplication() {
  return new Test;
}
