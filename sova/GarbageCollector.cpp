#include "GarbageCollector.h"
#include <set>
#include <vector>

std::set<Object *> alive_objects;
std::unordered_map<Object *, int> object_size;

int total_alloced = 0;
int total_freed = 0;

void *Object::operator new(size_t size) {
  void *ptr = malloc(size);
  total_alloced += size;
  alive_objects.insert((Object *)ptr);
  object_size[(Object *)ptr] = size;
  return ptr;
}

void Object::operator delete(void* p) {
  free(p);
}

void gc(GlobalContext &global) {
  int marked_objects = 0;

  //
  // MARK
  //
  for (Object *obj : alive_objects)
    obj->marked_for_gc = false;



  //
  // SWEEP
  //
  std::set<Object *> refs_set;

  for (auto kvp : global.resolve_map) {
    Object *value = kvp.second;
    if (!value || value->marked_for_gc)
      continue;

    if (!refs_set.contains(value))
      refs_set.insert(value);
  }

  std::vector<Object *> obj_refs;

  while (!refs_set.empty()) {
    Object *obj = *refs_set.begin();

    obj_refs.clear();
    obj->iterate_references(obj_refs);

    for (Object *r : obj_refs) {
      if (r && !r->marked_for_gc)
        refs_set.insert(r);
    }

    refs_set.erase(obj);
    obj->marked_for_gc = true;
    marked_objects++;
  }

  std::vector<Object *> to_delete(alive_objects.size() - marked_objects);

  for (Object *obj : alive_objects) {
    if (!obj->marked_for_gc)
      to_delete.push_back(obj);
  }

  //
  // FREE
  //
  int freed = 0;
  for (Object *obj : to_delete) {
    freed += object_size[obj];

    object_size.erase(obj);
    alive_objects.erase(obj);
    delete obj;
  }

  total_freed += freed;
  std::cout << "Freed " << freed << "\nTotal allocated: " << total_alloced << "\nTotal freed: " << total_freed << "\n";
}
