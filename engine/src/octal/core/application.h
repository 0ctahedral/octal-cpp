#pragma once
#include <string>
#include "octal/defines.h"
#include "octal/defines.h"

namespace octal {

  class Application {
    public:
      Application();
      virtual ~Application();
      void Run();
    private:
      /// Struct to store configuration of new application
      struct AppConfig {
        /// x position of window
        i16 x{0};
        /// y position of window
        i16 y{0};
        /// width of window
        i16 width{600};
        /// height of window
        i16 height{800};
        /// Title for the window
        std::string name{"Test"};
      };
      AppConfig m_Config;
  };

  Application* CreateApplication();
}
