#ifndef CONTEXT_H
#define CONTEXT_H

#include "map/map.h"
#include "game/player.h"
#include "common.h"

typedef struct mlx mlx_t;
typedef struct mlx_image mlx_image_t;
typedef struct mlx_texture mlx_texture_t;

typedef struct Context_s
{
	Map map;
	Player player;
	u32 width;
	u32 height;
	mlx_t* mlx;
	mlx_image_t* fb;
	union
	{
		u32* pixels;
		u8* pixels8;
	};
	mlx_texture_t* tex[3];
} Context;

int initcontext(Context* ctx, const char* mapfile, u32 width, u32 height);
void destroycontext(Context* ctx);

#endif
