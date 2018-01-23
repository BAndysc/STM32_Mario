#include "flag.h"
#include "gfx.h"
#include "store.h"

void UpdateFlag(GameObject* super, float delta)
{
    Flag* flag = (Flag*)super;

    if (flag->Triggered)
    {

        GameObject* Grounded = IsOver(&mario, super->PositionInt, super->Size);

        if (!Grounded)
            super->Position.y++;
        else
        {
            super->Collides = 0;
            flag->Triggered = 0;
            mario.super.Position.y-=2;
        }
    }
}

void InitFlag(Flag* flag, int x, int y)
{
    flag->super.Position = NewVectorf(x * 16 + 8, y * 16 + 12);
    flag->super.PositionInt = NewVector(x * 16 + 8, y * 16 + 12);
    flag->super.Size = NewVector(16, 16);
    flag->super.Sprite = GFX.FLAG;
    flag->super.Alpha = 0b1111100000000000;
    flag->super.Update = &UpdateFlag;
    flag->super.Collides = 0;

    flag->super.Type = OBJECT_FLAG;

    flag->Triggered = 0;
}