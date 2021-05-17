#pragma once
#include <string>
#include "octal/defines.h"
#include "octal/core/layer.h"
#include "octal/renderer/renderer.h"

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
      /// Stores state for the application
      struct AppState {
        /// Is the application running?
        bool is_running;
        /// Is the application paused?
        bool is_suspended;
        /// Current width of the application window
        i16 width;
        /// Current height of the application window
        i16 height;
        /// The time of the last frame of the application
        f64 last_time;
      };
      /// This application's state
      AppState m_State;

    protected:
      /// The layers this application is storing
      LayerStack m_LayerStack;
  };

  Application* CreateApplication();
}
