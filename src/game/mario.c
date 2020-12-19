#include "mario.h"
#include "levels.h"
#include "gfx.h"
#include "lcd.h"
#include "store.h"
#include "debug.h"
#include "external.h"

extern Vectorf input;
extern float G;
extern Level* level;

extern int INPUT_JUMP;

extern void ResetGame();

static void UpdateCollision(Mario* this, float delta) {
    if (this->Dead || (this->Won && flag.Triggered))
        return;

    GameObject *super = &this->super;

    if (super->Velocity.y > 10)
        super->Velocity.y = 10;

    Vectorf new = VectorffAdd(super->Position, super->Velocity);
    Vector nnew = Vectorf2Vector(new);

    this->Grounded = IsOver(&mario, NewVector(nnew.x, nnew.y + 15), NewVector(16, 1)) != 0;

    if (this->Grounded && super->Velocity.y > 0)
        super->Velocity.y = 0;

    GameObject *roofed = IsOver(&mario, NewVector(nnew.x + 5, nnew.y - 1), NewVector(5, 1));

    if (roofed && super->Velocity.y < 0)
        super->Velocity.y = 0;

    if (roofed && roofed->Type == 3)
    {
        roofed->Type = 4;
        POINTS += 10;
    }

    GameObject* rightcol = IsOver(&mario, NewVector(nnew.x + 16, nnew.y), NewVector(1, 15));;

    if (rightcol && super->Velocity.x > 0)
        super->Velocity.x = 0;

    GameObject* leftcol = IsOver(&mario, NewVector(nnew.x -1 , nnew.y), NewVector(1, 15));;

    if (leftcol && super->Velocity.x < 0)
        super->Velocity.x = 0;

    if ((leftcol && leftcol->Type == 1 && super->Position.y > leftcol->Position.y - 16) || (rightcol && rightcol->Type == 1 && super->Position.y > rightcol->Position.y - 16))
    {
        this->Dead = 1;
        this->Grounded = 0;
        super->Velocity.y = -8;
        super->Collides = 0;
        Puts("d");
    }

    GameObject* over = IsOverType(&mario, NewVector(nnew.x, nnew.y), mario.super.Size, 2);
    if (over)
    {
        POINTS += 50;
        over->Deactive = 1;
        Puts("c");
    }

    if (IsOverType(&mario, NewVector(nnew.x, nnew.y), mario.super.Size, OBJECT_FLAG) && !mario.Won)
    {
        Puts("f");
        flag.Triggered = 1;
        mario.Won = 1;
    }
}

static void UpdateMovement(Mario* this, float delta)
{
    GameObject* super = &this->super;

    if (super->Position.x >= level->castleX + 62 &&  !WON)
    {
        this->Won = 1;
        WON = 1;
    }

    if (this->Won)
    {
        if (flag.Triggered)
        {
            super->Position.x = flag.super.Position.x - 14;
            super->Position.y = flag.super.Position.y;
        }
        else
        {
            super->Velocity.x = 1;
            super->Velocity = VectorffAdd(super->Velocity, NewVectorf(-0.4f * super->Velocity.x * delta, delta*(super->Velocity.y < 0 ? 0.9f : 1.1f)));
        }
    }
    else
    {
        if (!this->Dead)
            super->Velocity = VectorffAdd(super->Velocity, VectorfMul(input, 1 * delta));

        super->Velocity = VectorffAdd(super->Velocity, NewVectorf(-0.4f * super->Velocity.x * delta, delta*(super->Velocity.y < 0 ? 0.9f : 1.1f)));

        if (INPUT_JUMP && this->Grounded)
        {
            super->Velocity.y = -8.5f;
            Puts("j");
        }
    }
}

static void UpdatePosition(Mario* this, float delta)
{
    GameObject* super = &this->super;

    if (this->Won && flag.Triggered)
        return;

    super->Position = VectorffAdd(super->Position, super->Velocity);

    if (super->Position.y >= 200)
        ResetGame();
}

static void UpdateWalkAnimation(Mario* this, float delta)
{
    this->super.Sprite = GFX.MARIO_MOVE + (this->Frame * 16 * 16);
    this->FrameTime += delta;
    if (this->FrameTime >= 3)
    {
        this->Frame += 1;
        if (this->Frame == 3)
            this->Frame = 0;
        this->FrameTime = 0;
    }
}

static void UpdateAnimation(Mario* this, float delta)
{
    if (this->Won)
    {
        if (flag.Triggered)
            this->super.Sprite = GFX.MARIO_FALL;
        else
            UpdateWalkAnimation(this, delta);
        return;
    }
    if (this->Dead)
    {
        this->super.Sprite = GFX.MARIO_DEAD;
        return;
    }
    if (input.x < 0)
        this->super.FlipSprite = 1;
    else if (input.x > 0)
        this->super.FlipSprite = 0;

    if (!this->Grounded)
        this->super.Sprite = GFX.MARIO_JUMP;
    else
    {
        if (input.x == 0)
            this->super.Sprite = GFX.MARIO;
        else {
            UpdateWalkAnimation(this, delta);
        }
    }
}

static void update(GameObject* go, float delta)
{
    Mario* this = (Mario*)go;

    UpdateMovement(this, delta);
    UpdateCollision(this, delta);
    UpdatePosition(this, delta);
    UpdateAnimation(this, delta);
}

void InitMario(Mario* mario, int x, int y)
{
    mario->super.Update = &update;

    mario->super.Position = NewVectorf(x * 16, y * 16);
    mario->super.PositionInt = NewVector(x * 16, y * 16);
    mario->super.Size = NewVector(16, 16);
    mario->super.Alpha = GFX.MARIO_ALPHA;
    mario->super.Collides = 1;
    mario->super.Frame = 0;
    mario->super.Sprite = GFX.MARIO;
}