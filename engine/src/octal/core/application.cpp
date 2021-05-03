#include "octal/core/application.h"
#include "platform/platform.h"

namespace octal {

  Application::Application(Config config) { 
    // set state
    m_State.width = config.width;
    // start up window
    Platform::Init(config.name, config.x, config.y, config.width, config.height);
  }

  void Application::Run() {
    bool quit = false;
    // main loop
    while (!quit) {
      if (!Platform::Flush()) {
        quit = true;
      }
    }
  }

  Application::~Application() {
    Platform::Shutdown();
  }

}
