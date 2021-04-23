#include <octal/core/logger.h>
#include <octal/core/asserts.h>

int main() {
  FATAL("this is a %f", 3.14);
  ERROR("this is a %f", 3.14);
  WARN("this is a %f", 3.14);
  INFO("this is a %f", 3.14);
  DEBUG("this is a %f", 3.14);
  TRACE("this is a %f", 3.14);

  ASSERT(false, "this should fail");
}
