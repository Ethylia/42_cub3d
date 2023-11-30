#include "context.h"

#include <MLX42/MLX42.h>

#include <stdlib.h>

int initcontext(Context* ctx, const char* mapfile, u32 width, u32 height)
{
	if(!loadmap(mapfile, &ctx->map))
		return 0;
	ctx->player.pos = ctx->map.start;
	ctx->player.dir = ctx->map.startdir;
	ctx->player.vel = (Vec2){0.0f, 0.0f};
	ctx->player.dirvel = 0.0f;
	ctx->width = width;
	ctx->height = height;
	ctx->player.lantern.radius = 0.0f;
	ctx->player.lantern.radiustarget = 0.0f;
	ctx->player.lantern.timer = 0.0f;
	ctx->player.lantern.scale = 0.85f;
	mlx_set_setting(MLX_STRETCH_IMAGE, 1);
	ctx->mlx = mlx_init(width, height, "Cube3D", false);
	if(!ctx->mlx)
		return 0;
	ctx->fb = mlx_new_image(ctx->mlx, width, height);
	if(!ctx->fb)
		return 0;
	ctx->pixels8 = ctx->fb->pixels;
	ctx->tex[0] = mlx_load_png("res/wall.png");
	ctx->tex[1] = mlx_load_png("res/floor.png");
	ctx->tex[2] = mlx_load_png("res/ceil.png");
	if(!ctx->tex[0] || !ctx->tex[1] || !ctx->tex[2])
		return 0;
	return 1;
}

void destroycontext(Context* ctx)
{
	mlx_delete_image(ctx->mlx, ctx->fb);
	mlx_delete_texture(ctx->tex[0]);
	mlx_delete_texture(ctx->tex[1]);
	mlx_delete_texture(ctx->tex[2]);
	mlx_terminate(ctx->mlx);
	free(ctx->map.map);
}
