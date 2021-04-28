#include <octal/core/logger.h>
#include <octal/core/asserts.h>
#include <octal/ecs/ecs.h>

int main() {
  octal::ECS ecs(3);
  auto id0 = ecs.CreateEntity();
  auto id1 = ecs.CreateEntity();
  auto id2 = ecs.CreateEntity();

  DEBUG("id0: %d", id0);
  DEBUG("id1: %d", id1);
  DEBUG("id2: %d", id2);


  ecs.DestroyEntity(id1);

  auto id3 = ecs.CreateEntity();
  DEBUG("id3: %d", id3);
  auto id4 = ecs.CreateEntity();
  DEBUG("id4: %d", id4);
}
