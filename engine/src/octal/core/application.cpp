#include "octal/core/logger.h"
#include "octal/core/application.h"
#include "octal/renderer/renderer.h"
#include "platform/platform.h"

namespace octal {

  Application::Application(Config config) { 
    // set state
    m_State.width = config.width;
    // start up window
    Platform::Init(config.name, config.x, config.y, config.width, config.height);

    Renderer renderer;
    if (!renderer.Init()) {
      FATAL("Could not start vulkan :(");
      Platform::Shutdown();
      // bail out here?
      exit(1);
    }
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
