#include "gameobject.h"
#include "store.h"


int IsOverlapingPos(Vector pos, Vector size, GameObject* o2)
{
    if (pos.y + size.y <= o2->PositionInt.y)
        return 0;

    if (pos.x + size.x <= o2->PositionInt.x)
        return 0;

    if (pos.y >= o2->PositionInt.y + o2->Size.y)
        return 0;

    if (pos.x >= o2->PositionInt.x + o2->Size.x)
        return 0;

    return 1;
}

int IsOverlaping(GameObject* o1, GameObject* o2)
{
    return IsOverlapingPos(o1->PositionInt, o1->Size, o2);
}

GameObject* IsOver(void* skip, Vector pos, Vector size)
{
    for (int i = 0; i < ObjectsColliderCounter; ++i)
    {
        if (Colliders[i] == skip || !Colliders[i]->Collides || Colliders[i]->Deactive)
            continue;
        if (IsOverlapingPos(pos, size, Colliders[i]))
            return Colliders[i];
    }

    return 0;
}

GameObject* IsOverType(void* skip, Vector pos, Vector size, uint8_t type)
{
    for (int i = 0; i < ObjectsCounter; ++i)
    {
        if (RenderObjects[i] == skip || RenderObjects[i]->Type != type || RenderObjects[i]->Deactive)
            continue;
        if (IsOverlapingPos(pos, size, RenderObjects[i]))
            return RenderObjects[i];
    }

    return 0;
}
