#include "gameobject.h"



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
