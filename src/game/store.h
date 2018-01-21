#ifndef STORE_H
#define STORE_H

#include "gameobject.h"
#include "mario.h"
#include "levels.h"

#define MAX_OBJECTS 500

Level* level;

Mario mario;

GameObject ObjectsStore[MAX_OBJECTS];

GameObject* RenderObjects[1000];
GameObject* UpdateLoop[100];
GameObject* Colliders[1000];
int ObjectsUpdateCounter;
int ObjectsCounter;
int ObjectsColliderCounter;

void AddObjectToStore(GameObject* object);

void RemoveObjectFromStore(GameObject* object);

void ClearStore();

GameObject* GetNewObject();

#endif