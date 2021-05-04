#include <octal/core/logger.h>
#include <octal/core/application.h>
#include <octal/core/layer.h>

class TestLayer : public octal::Layer {

  void OnPush() override {
    DEBUG("Test layer pushed!");
  }

  void OnPop() override {
    DEBUG("Test layer popped!");
  }
};

class Test : public octal::Application {
  public:
    Test(octal::Application::Config conf):
      octal::Application(conf)
      {
        // add layers and stuff
        m_LayerStack.PushLayer(new TestLayer);
      }

    ~Test(){ }
};

octal::Application* octal::CreateApplication() {
  return new Test({
      0, 0,
      1280, 920,
      "Testbed"
      });
}
