#ifndef STORE_H
#define STORE_H

#include "gameobject.h"
#include "mario.h"
#include "levels.h"
#include "flag.h"

#define MAX_OBJECTS 500

extern Level* level;

extern Mario mario;

extern Flag flag;

extern GameObject ObjectsStore[MAX_OBJECTS];

extern GameObject* RenderObjects[1000];
extern GameObject* UpdateLoop[250];
extern GameObject* Colliders[1000];
extern int ObjectsUpdateCounter;
extern int ObjectsCounter;
extern int ObjectsColliderCounter;

void AddObjectToStore(GameObject* object);

void RemoveObjectFromStore(GameObject* object);

void ClearStore();

GameObject* GetNewObject();

extern int POINTS;

extern int WON;

extern int LIVES;

#endif