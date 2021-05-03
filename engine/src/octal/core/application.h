#pragma once
#include <string>
#include "octal/defines.h"
#include "octal/defines.h"

namespace octal {

  class Application {
    public:
      /// Struct to store configuration of new application
      struct Config {
        /// x position of window
        i16 x{0};
        /// y position of window
        i16 y{0};
        /// width of window
        i16 width{800};
        /// height of window
        i16 height{600};
        /// Title for the window
        std::string name{"Test"};
      };

      /// Create an application
      /// @param config the parameters to configure our application
      Application(Config config);
      /// Destructor
      ~Application();
      /// Run the application
      void Run();

    private:

      struct AppState {
        bool is_running;
        bool is_suspended;
        i16 width;
        i16 height;
        f64 last_time;
      };
      AppState m_State;
  };

  Application* CreateApplication();
}
