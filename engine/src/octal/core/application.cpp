#include "octal/core/application.h"
#include "platform/platform.h"

namespace octal {

  Application::Application() { 
    Platform::Init(m_Config.name, m_Config.x, m_Config.y, m_Config.width, m_Config.height);
  }

  void Application::Run() {
    bool quit = false;
    while (!quit) {
      if (!Platform::Flush()) {
        quit = true;
      }
    }
  }

  Application::~Application() { }

}
