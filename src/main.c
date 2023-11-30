#include "map/map.h"
#include "render/raycast.h"
#include "render/context.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <MLX42/MLX42.h>

// norminette hates macros so make some functions instead
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define DISPLAY_SCALE 8

#define POSTERIZE 16

static int darken(int color, float ratio)
{
	if(ratio < 0.0f)
		ratio = 0.0f;
	else if(ratio > 1.0f)
		ratio = 1.0f;
	// we multiply each component of the color by the ratio
	// we access each byte with a byte pointer to the color integer
	u8 *const c = (u8*)&color;
#if POSTERIZE
	// we round the color to the nearest multiple of 8 (postirize filter)
	c[0] = (u8)((u32)(c[0] * ratio) / POSTERIZE * POSTERIZE);
	c[1] = (u8)((u32)(c[1] * ratio) / POSTERIZE * POSTERIZE);
	c[2] = (u8)((u32)(c[2] * ratio) / POSTERIZE * POSTERIZE);
#else
	c[0] *= ratio;
	c[1] *= ratio;
	c[2] *= ratio;
#endif
	return color;
}

static void drawpixel(Context* ctx, u32 x, u32 y, u32 color)
{
	ctx->pixels[y * ctx->width + x] = color; // kind of a pointless function innit
}

static void render(Context* ctx)
{
	const Player* const p = &ctx->player;
	const u32 w = ctx->width;
	for(u32 x = 0; x < w; ++x)
	{ // we only need to raycast for each coloumn of pixels and calculate the height of the wall from the distance
		// ray direction is the direction we're facing plus the angle of the coloumn we're rendering
		const Ray ray = {p->pos, {cosf(p->dir + ((i32)x - (i32)w / 2) * 0.001f * DISPLAY_SCALE), sinf(p->dir + ((i32)x - (i32)w / 2) * 0.001f * DISPLAY_SCALE)}};
		const Hit hit = raycast(ray, &ctx->map); // find the closest wall hit by the ray
		const float dist = hit.dist; // you could do some fish eye correction here but it's not really necessary and it would mean changing the floor/ceiling rendering
		const u32 h = ctx->height;
		const i32 height = (i32)(h / dist); // height of the wall based on distance
		const float sqrtdist = sqrtf(dist) * 0.45f;
		for(i32 y = 0; y < MIN(h / 2 - height / 2, h); ++y)
		{ // we start from the top of the screen and go down to the top of the wall
			const float dist = (h / 2) / (float)(h / 2 - y); // distance from the player to the ceiling
			const Vec2 floorpos = {p->pos.x + ray.dir.x * dist, p->pos.y + ray.dir.y * dist}; // position of the ceilling pixel
			const u32 texx = (u32)(floorpos.x * ctx->tex[2]->width) % ctx->tex[2]->width;
			const u32 texy = (u32)(floorpos.y * ctx->tex[2]->height) % ctx->tex[2]->height;
			const u32 c = ((u32*)ctx->tex[2]->pixels)[(u32)(texy * ctx->tex[2]->width + texx)];
			drawpixel(ctx, x, y, darken(c, p->lantern.radius - sqrtf(dist) * 0.45f) + 0.2f); // draw the pixel with the floor texture and darken it based on distance
		}
		const float uv = (hit.face == FACEEAST || hit.face == FACEWEST) ? fmodf(hit.pos.y, 1.0f) : fmodf(hit.pos.x, 1.0f);
		for(u32 y = MAX((i32)h / 2 - height / 2, 0); y < MIN((i32)h / 2 + height / 2, h); ++y)
		{ // we start from the top of the wall and go down to the bottom of the wall
			// we calculate the texture coordinates based on the ratio of the distance from the top of the wall to the bottom of the wall
			const u32 texy = MIN((y - (h / 2 - height / 2)) * ctx->tex[0]->height / height, ctx->tex[0]->height - 1);
			const u32 texx = (u32)(uv * ctx->tex[0]->width);
			const u32 c = ((u32*)ctx->tex[0]->pixels)[(u32)(texy * ctx->tex[0]->width + texx)];
			drawpixel(ctx, x, y, darken(c, p->lantern.radius - sqrtdist) + 0.2f);
		}
		for(u32 y = h / 2 + height / 2; y < h; ++y)
		{
			const float dist = (h / 2) / (float)(y - h / 2);
			const Vec2 floorpos = {p->pos.x + ray.dir.x * dist, p->pos.y + ray.dir.y * dist};
			const u32 texx = (u32)(floorpos.x * ctx->tex[1]->width) % ctx->tex[1]->width;
			const u32 texy = (u32)(floorpos.y * ctx->tex[1]->height) % ctx->tex[1]->height;
			const u32 c = ((u32*)ctx->tex[1]->pixels)[(u32)(texy * ctx->tex[1]->width + texx)];
			drawpixel(ctx, x, y, darken(c, p->lantern.radius - sqrtf(dist) * 0.45f) + 0.2f);
		}
	}
}

void loop(void* userdata)
{ // This is the main loop function that gets called every frame
	Context* const ctx = userdata;

	playerupdate(ctx, &ctx->player); // update the player

	if(mlx_is_key_down(ctx->mlx, MLX_KEY_UP))
	{ // to play around with the lantern radius
		ctx->player.lantern.scale += ctx->mlx->delta_time * 0.5f;
		printf("%f\n", ctx->player.lantern.scale);
	}
	else if(mlx_is_key_down(ctx->mlx, MLX_KEY_DOWN))
	{
		ctx->player.lantern.scale -= ctx->mlx->delta_time * 0.5f;
		printf("%f\n", ctx->player.lantern.scale);
	}

	render(ctx); // render the scene

	if(mlx_is_key_down(ctx->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(ctx->mlx); // close the window if escape is pressed (it will exit the mlx_loop function in main at the end of this loop)
}

int main()
{
	Context ctx; // this stores all the data we need to share around
	if(!initcontext(&ctx, "map.txt", 1600 / DISPLAY_SCALE, 900 / DISPLAY_SCALE))
		return 1;
	mlx_set_window_size(ctx.mlx, 1600, 900);
	i32 wpos[2] = {0};
	mlx_get_monitor_size(0, wpos, wpos + 1);
	mlx_set_window_pos(ctx.mlx, wpos[0] / 2 - 1600 / 2, wpos[1] / 2 - 900 / 2);
	// puts our "framebuffer" on the screen (it's just an image that we render to)
	mlx_image_to_window(ctx.mlx, ctx.fb, 0, 0);
	mlx_loop_hook(ctx.mlx, &loop, &ctx);
	mlx_loop(ctx.mlx);
	destroycontext(&ctx);
	return 0;
}
