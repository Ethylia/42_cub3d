#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

#define PLAYER_ACCEL 10.0f
#define PLAYER_SPEED 2.0f
#define PLAYER_DIR_ACCEL 20.0f
#define PLAYER_DIR_SPEED 5.0f

typedef struct Context_s Context;

typedef struct Player_s
{
	Vec2 pos;
	Vec2 vel;
	float dir;
	float dirvel;
	struct
	{
		float radius;
		float timer;
		float radiustarget;
		float scale;
	} lantern;
} Player;

void playerupdate(Context* ctx, Player* player);

#endif

