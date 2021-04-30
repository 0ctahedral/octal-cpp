#include <thread>
#include <octal/core/logger.h>
#include <platform/platform.h>
#include <octal/core/asserts.h>
#include <octal/ecs/ecs.h>
#include <octal/ecs/compstore.h>

int main() {
  octal::Platform::Init();
  bool quit = false;
  while (!quit) {
    if (!octal::Platform::Flush()) {
      quit = true;
    }
  }
  octal::Platform::Stop();
}
