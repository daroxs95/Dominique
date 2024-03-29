#pragma once
#include <bitset>
#include <vector>
#include "dengine/utils/uuid.h"

// The next line should be included in some compilation unit of final app
// If using the dominique engine that is handled on core.cpp compilation, so no
// need to include it in any file
// int s_componentCounterDEngine = 0;

extern int s_componentCounterDEngine;

namespace de {
namespace ecs {

// This function will return a unique ID for each component type
template <class T>
int GetId() {
  static int s_componentId = s_componentCounterDEngine++;
  return s_componentId;
}

const int MAX_ENTITIES = 1000;
const int MAX_COMPONENTS = 32;
// Some typedefs to aid in reading
typedef unsigned long long EntityIndex;

struct EntityID {
  EntityIndex index;
  de::utils::uuids::Uuid uuid;  // needed to correctly delete
};

inline bool IsEntityValid(EntityID id) {
  // Check if the index is our invalid index
  return (id.index >> 32) != EntityIndex(-1);
}

typedef std::bitset<MAX_COMPONENTS> ComponentMask;

// Generic component pool, stores a block of memory for each component type
struct ComponentPool {
  ComponentPool(size_t elementsize) {
    // We'll allocate enough memory to hold MAX_ENTITIES, each with element size
    elementSize = elementsize;
    pData = new char[elementSize * MAX_ENTITIES];
  }

  // Copy constructor
  ComponentPool(const ComponentPool& p1) {
    // We'll allocate enough memory to hold MAX_ENTITIES, each with element size
    elementSize = p1.elementSize;
    pData = new char[elementSize * MAX_ENTITIES];
    for (size_t i = 0; i < elementSize * MAX_ENTITIES; i++) {
      pData[i] = p1.pData[i];
    }
  }

  ~ComponentPool() { delete[] pData; }

  inline void* get(size_t index) {
    // looking up the component at the desired index
    return pData + index * elementSize;
  }

  // Not sure why this is needed
  ComponentPool& operator=(const ComponentPool* a) {
    pData = (*a).pData;
    elementSize = (*a).elementSize;
    return *this;
  }

  char* pData{nullptr};
  size_t elementSize{0};
};

// This is the scene, where we keep all our entities and components
struct Scene {
  // All the information we need about each entity
  struct EntityDesc {
    EntityID id;
    ComponentMask mask;
  };
  std::vector<EntityDesc> entities;
  std::vector<EntityIndex> freeEntities;
  std::vector<ComponentPool> componentPools;

  // Get a component from an entity
  template <typename T>
  T* Get(EntityID id) {
    int componentId = GetId<T>();
    if (!entities[id.index].mask.test(componentId)) return nullptr;

    T* pComponent = static_cast<T*>(componentPools[componentId].get(id.index));
    return pComponent;
  }

  // Create a new entity
  EntityID NewEntity() {
    if (!freeEntities.empty()) {
      EntityIndex newIndex = freeEntities.back();
      freeEntities.pop_back();
      EntityID newID = {newIndex, de::utils::uuids::GetUUID()};
      entities[newIndex].id = newID;
      return entities[newIndex].id;
    }
    entities.push_back(
        {{entities.size(), de::utils::uuids::GetUUID()}, ComponentMask()});
    return entities.back().id;
  }

  // Destroys an entity
  void DestroyEntity(EntityID id) {
    EntityID newID = {(EntityIndex)(-1), de::utils::uuids::GetUUID()};
    entities[id.index].id = newID;
    entities[id.index].mask.reset();
    freeEntities.push_back(id.index);
  }

  // Assign a component to an entity
  template <typename T>
  T* Assign(EntityID id) {
    int componentId = GetId<T>();

    if (componentPools.size() <= componentId)  // Not enough component pool
    {
      componentPools.resize(componentId + 1, ComponentPool(sizeof(T)));
    }
    if (componentPools[componentId].pData ==
        nullptr)  // New component, make a new pool
    {
      componentPools[componentId] = new ComponentPool(sizeof(T));
    }

    // Looks up the component in the pool, and initializes it with placement new
    T* pComponent = new (componentPools[componentId].get(id.index)) T();

    entities[id.index].mask.set(componentId);
    return pComponent;
  }

  template <typename T>
  void Remove(EntityID id) {
    // ensures you're not accessing an entity that has been deleted
    if (entities[id.index].id.uuid != id.uuid) return;

    int componentId = GetId<T>();
    entities[id.index].mask.reset(componentId);
  }
};

template <typename... ComponentTypes>
struct SceneView {
  SceneView(Scene& scene) : pScene(&scene) {
    if (sizeof...(ComponentTypes) == 0) {
      all = true;
      return;
    }
    // Unpack the template parameters into an initializer list
    int componentIds[] = {0, GetId<ComponentTypes>()...};
    for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++) {
      componentMask.set(componentIds[i]);
    }
  }

  struct Iterator {
    Iterator(Scene* pScene, EntityIndex index, const ComponentMask& mask,
             bool all)
        : pScene(pScene), index(index), mask(mask), all(all) {}

    EntityID operator*() const {
      // give back the entityID we're currently at
      return pScene->entities[index].id;
    }

    bool operator==(const Iterator& other) const {
      return index == other.index || index == pScene->entities.size();
    }

    bool operator!=(const Iterator& other) const {
      return index != other.index && index != pScene->entities.size();
    }

    bool ValidIndex() {
      return
          // It's a valid entity ID
          IsEntityValid(pScene->entities[index].id) &&
          // It has the correct component mask
          (all || mask == (mask & pScene->entities[index].mask));
    }

    Iterator& operator++() {
      // Move the iterator forward
      do {
        index++;
      } while (index < pScene->entities.size() && !ValidIndex());
      return *this;
    }

    EntityIndex index;
    Scene* pScene;
    ComponentMask mask;
    bool all{false};
  };

  const Iterator begin() const {
    // Give an iterator to the beginning of this view
    int firstIndex = 0;
    while (
        firstIndex < pScene->entities.size() &&
        (componentMask != (componentMask & pScene->entities[firstIndex].mask) ||
         !IsEntityValid(pScene->entities[firstIndex].id))) {
      firstIndex++;
    }
    return Iterator(pScene, firstIndex, componentMask, all);
  }

  const Iterator end() const {
    // Give an iterator to the end of this view
    int lastIndex = pScene->entities.size();
    while (
        lastIndex > 0 &&
        (componentMask != (componentMask & pScene->entities[lastIndex].mask) ||
         !IsEntityValid(pScene->entities[lastIndex].id))) {
      lastIndex--;
    }
    return Iterator(pScene, lastIndex + 1, componentMask, all);
  }

  Scene* pScene{nullptr};
  ComponentMask componentMask;
  bool all{false};
};

}  // namespace ecs
}  // namespace de
