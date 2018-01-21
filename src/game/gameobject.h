#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdint.h>
#include "vector.h"

typedef struct GameObject {

	uint16_t* Sprite;

	int FlipSprite;

	Vectorf Position;

	Vectorf Velocity;

	Vector PositionInt;

	Vector Size;

	uint16_t Alpha;

	uint16_t Collides;

	uint8_t Frame;

	uint8_t Type;

	uint8_t Deactive;

	void (*Update)(struct GameObject* this, float delta);

} GameObject;


int IsOverlaping(GameObject* o1, GameObject* o2);
int IsOverlapingPos(Vector pos, Vector size, GameObject* o2);


#endif