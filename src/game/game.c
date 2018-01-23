#include "game.h"
#include "../adc.h"
#include "../debug.h"

uint32_t col = 0;

const float G = 3;

uint32_t newScale = 2;

void horizHandler(void* data, uint16_t val)
{
	INPUT_LEFT = 0;
	INPUT_RIGHT = 0;
	if (val >= 160)
		INPUT_LEFT = 1;
	else if (val <= 50)
		INPUT_RIGHT = 1;
}

extern uint8_t JOYSTICK[2];
void loop(uint32_t timedelta)
{
	float delta = timedelta / 16.f;

	UpdateInput();

	for (int i = 0; i < ObjectsUpdateCounter; ++i)
	{
		GameObject* ob = UpdateLoop[i];
		ob->Update(ob, delta);
	}

	if (INPUT_ANALOG)
	{
		
		//uint16_t val = ain.read(&ain, ADC_RESOLUTION_6);
		INPUT_LEFT = 0;
		INPUT_RIGHT = 0;
		if (JOYSTICK[0] >= 160)
			INPUT_LEFT = 1;
		else if (JOYSTICK[0] <= 50)
			INPUT_RIGHT = 1;

		//uint16_t val2 = vertain.read(&vertain, ADC_RESOLUTION_6);
		INPUT_JUMP = JOYSTICK[1] >= 160;
	}
}

uint8_t GetTile(int x, int y)
{
	if (x < 0)
		return ' ';
	
	if (x >= level->width)
		return ' ';

	if (y < 0)
		return ' ';
	
	if (y >= level->height)
		return ' ';

	return level->data[y * level->width + x];
}

void InitWall(GameObject* go, int x, int y, char type)
{
	go->Position = NewVectorf(x * 16, y * 16);
	go->PositionInt = NewVector(x * 16, y * 16);
	go->Size = NewVector(16, 16);
	go->Alpha = 0xFFFF;

	switch (type)
	{
		case '#':
			if (GetTile(x-1, y) == ' ')
				go->Sprite = GFX.GROUND;
			else if (GetTile(x+1, y) == ' ')
				go->Sprite = GFX.GROUND + 16*16*2;
			else
				go->Sprite = GFX.GROUND + 16*16;
			go->Collides = 1;
			go->Alpha = 0b1111100000000000;
			break;
		case '=':
			go->Sprite = GFX.PLAT;
			go->Collides = 1;
			break;
		case '+':
			go->Sprite = GFX.BLOCK;
			go->Collides = 1;
			break;
		case '<':
			go->Sprite = GFX.TREE_S;
			go->Alpha = 63488;
			break;
		case '.':
			go->Sprite = GFX.TREE_M;
			go->Alpha = 63488;
			break;
		case '>':
			go->Sprite = GFX.TREE_E;
			go->Alpha = 63488;
			break;
		case ',':
			go->Alpha = 0b1111100000000000;
			go->Sprite = GFX.CLOUDS;
			if (GetTile(x-1, y) == ',' && GetTile(x+1, y) == ',' && GetTile(x, y+1) == ',')
				go->Sprite = GFX.CLOUDS + 16*16;
			else if (GetTile(x-1, y) == ',' && GetTile(x+1, y) == ' ' && GetTile(x, y+1) == ',')
				go->Sprite = GFX.CLOUDS + 16*16 * 2;
			else if (GetTile(x-1, y) == ' ' && GetTile(x+1, y) == ',' && GetTile(x, y-1) == ',')
				go->Sprite = GFX.CLOUDS + 16*16 * 3;
			else if (GetTile(x-1, y) == ',' && GetTile(x+1, y) == ',' && GetTile(x, y-1) == ',')
				go->Sprite = GFX.CLOUDS + 16*16 * 4;
			else if (GetTile(x-1, y) == ',' && GetTile(x+1, y) == ' ' && GetTile(x, y-1) == ',')
				go->Sprite = GFX.CLOUDS + 16*16 * 5;
			
			break;
		case '|':
			go->Size = NewVector(32, 16);
			go->Sprite = GFX.PORTAL_MID;
			go->Alpha = 63488;
			go->Collides = 1;
			break;
		case '_':
			go->Size = NewVector(32, 16);
			go->Sprite = GFX.PORTAL_TOP;
			go->Alpha = 63488;
			go->Collides = 1;
			break;
	}
}


GameObject* IsOver(void* skip, Vector pos, Vector size);

void up(GameObject* go, float delta)
{
	//go->PositionInt.x += 1;

	if (go->Velocity.x == 0)
		return;

	go->Frame++;

	if (go->Frame == 2)
		go->Frame = 0;

	go->Sprite = GFX.ENEMY_1 + (go->Frame * 16 * 16);

	if (IsOver(go, NewVector((int32_t)go->Position.x+2, (int32_t)go->Position.y-3), NewVector(16-4, 3)) == (GameObject*)&mario)
	{
		go->Velocity.x = 0;
		go->Velocity.y = 0;
		go->Sprite = GFX.ENEMY_1_DEAD;
		go->Collides = 0;
		mario.super.Velocity.y = -4;
		Debug("s");
		return;
	}

	go->Velocity = VectorffAdd(go->Velocity, NewVectorf(0, delta*(go->Velocity.y < G ? 0.1f : 0)));

	if (IsOver(go, NewVector((int32_t)go->Position.x, (int32_t)go->Position.y+16), NewVector(16, 1)) && go->Velocity.y > 0)
		go->Velocity.y = 0;

	if (go->Velocity.x < 0 && (go->Position.x < 10 || IsOver(go, NewVector((int32_t)go->Position.x-1,(int32_t)go->Position.y), NewVector(1, 14))))
		go->Velocity = VectorfMul(go->Velocity, -1);

	if (go->Velocity.x > 0 && IsOver(go, NewVector((int32_t)go->Position.x+16,(int32_t)go->Position.y), NewVector(1, 14)))
	{
		go->Velocity = VectorfMul(go->Velocity, -1);
	}

	go->Position = VectorffAdd(go->Position, go->Velocity);

}

void up_star(GameObject* go, float delta)
{
	go->Frame++;

	if (go->Frame == 20)
		go->Frame = 0;	

	if (go->Frame < 15)
		go->Sprite = GFX.COIN + ((go->Frame/5) * 16 * 16);
	else
		go->Sprite = GFX.COIN + (1 * 16 * 16);
}

void InitStar(GameObject* go, int x, int y)
{
	go->Position = NewVectorf(x * 16, y * 16);
	go->PositionInt = NewVector(x * 16, y * 16);
	go->Size = NewVector(16, 16);
	go->Sprite = GFX.COIN;
	go->Alpha = 2016;
	go->Update = &up_star;
	go->Collides = 0;

	go->Type = 2;
}

void up_special(GameObject* go, float delta)
{
	if (go->Type == 4)
	{
		go->Sprite = GFX.SPECIAL + (3 * 16 * 16);
	}
	else
	{
		go->Frame++;

		if (go->Frame == 20)
			go->Frame = 0;	

		if (go->Frame < 15)
			go->Sprite = GFX.SPECIAL + ((go->Frame/5) * 16 * 16);
		else
			go->Sprite = GFX.SPECIAL + (1 * 16 * 16);			
	}
}

void InitSpecial(GameObject* go, int x, int y)
{
	go->Position = NewVectorf(x * 16, y * 16);
	go->PositionInt = NewVector(x * 16, y * 16);
	go->Size = NewVector(16, 16);
	go->Sprite = GFX.SPECIAL;
	go->Alpha = 0b0000011111100000;
	go->Update = &up_special;
	go->Collides = 1;

	go->Type = 3;
}

void InitMonster(GameObject* go, int x, int y)
{
	go->Position = NewVectorf(x * 16, y * 16);
	go->PositionInt = NewVector(x * 16, y * 16);
	go->Size = NewVector(16, 16);
	go->Sprite = GFX.ENEMY_1;
	go->Alpha = 2016;
	go->Update = &up;
	go->Collides = 1;

	go->Type = 1;

	go->Velocity = NewVectorf(-0.7f, 0);
}

void UpdateWorldLoop()
{
	loop(10);
}

void Start() {
	for (int i = 0; i < level->width; ++i)
	{
		for (int j = 0; j < level->height; ++j)
		{
			uint8_t block = (level->data[j * level->width + i]);
			if (block != ' ')
			{
				GameObject* object = GetNewObject();
				if (block == 'x')
				{
					InitMonster(object, i, j);
				}
				else if (block == '*')
				{
					InitStar(object, i, j);
				}
				else if (block == '?')
				{
					InitSpecial(object, i, j);
				}
				else
				{
					InitWall(object, i, j, block);
				}

				AddObjectToStore(object);
			}
		}
	}

	InitMario(&mario, 1, 6);
	AddObjectToStore(&mario.super);

	Debug("m");
}

void InitGame() {

	Start();
}


void ResetGame() 
{
	ClearStore();

	Start();
}