#include "store.h"
#include <string.h>

Level* level = &Levels.level1;

Mario mario;

GameObject ObjectsStore[MAX_OBJECTS];
GameObject* Objects[1000];
GameObject* UpdateLoop[200];
GameObject* Colliders[1000];
int ObjectsUpdateCounter = 0;
int ObjectsCounter = 0;
int ObjectsColliderCounter = 0;

static int GameObjectStoreCounter = 0;

void AddObjectToStore(GameObject* object)
{
    if (object->Sprite)
        RenderObjects[ObjectsCounter++] = object;
    if (object->Collides)
        Colliders[ObjectsColliderCounter++] = object;
    if (object->Update)
        UpdateLoop[ObjectsUpdateCounter++] = object;
}

static void RemoveObjectAt(int i)
{
    RenderObjects[i] = RenderObjects[ObjectsCounter - 1];
    ObjectsCounter--;
}

static void RemoveObjectUpdateAt(int i)
{
    UpdateLoop[i] = UpdateLoop[ObjectsUpdateCounter - 1];
    ObjectsUpdateCounter--;
}

static void RemoveObjectColliderAt(int i)
{
    Colliders[i] = Colliders[ObjectsColliderCounter - 1];
    ObjectsColliderCounter--;
}

void RemoveObjectFromStore(GameObject* object)
{
    if (object->Sprite)
    {
        for (int i = ObjectsCounter - 1; i >= 0; --i)
        {
            if (RenderObjects[i] == object)
            {
                RemoveObjectAt(i);
                break;
            }
        }
    }

    if (object->Collides)
    {
        for (int i = ObjectsColliderCounter - 1; i >= 0; --i)
        {
            if (Colliders[i] == object)
            {
                RemoveObjectColliderAt(i);
                break;
            }
        }
    }

    if (object->Update)
    {
        for (int i = ObjectsUpdateCounter - 1; i >= 0; --i)
        {
            if (UpdateLoop[i] == object)
            {
                RemoveObjectUpdateAt(i);
                break;
            }
        }
    }
}

GameObject* GetNewObject()
{
    return &ObjectsStore[GameObjectStoreCounter++];
}

void ClearStore()
{
    memset(ObjectsStore, 0, sizeof(GameObject) * GameObjectStoreCounter);
    memset(&mario, 0, sizeof(Mario));
    ObjectsUpdateCounter = 0;
    ObjectsColliderCounter = 0;
    ObjectsCounter = 0;
    GameObjectStoreCounter = 0;
}

int POINTS = 0;