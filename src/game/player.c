#include "player.h"

#include "render/context.h"

#include <MLX42/MLX42.h>

#include <math.h>
#include <stdlib.h>

void playerupdate(Context* ctx, Player* player)
{
	// we use delta time to make the game framerate independent
	// basically, every time we change a value relatively, we multiply it by delta time
	// which is the time it took to render the last frame so it's always based on time passed

	// turn the player based on look velocity
	player->dir += ctx->mlx->delta_time * player->dirvel;
	if(player->dir < 0.0f) // keep the direction between 0 and 2 * PI (loop around)
		player->dir += 2 * M_PI;
	else if(player->dir > 2 * M_PI)
		player->dir -= 2 * M_PI;

	// check if the player is not colliding with a wall if we move them ahead with the velocity
	if(ctx->map.map[(u32)player->pos.y * ctx->map.width + (u32)(player->pos.x + player->vel.x * ctx->mlx->delta_time + ((player->vel.x > 0.0f) ? 0.05f : -0.05f))] != '#')
		player->pos.x += player->vel.x * ctx->mlx->delta_time; // move the player based on velocity
	// we do it separately for X and Y because we want the player to slide along walls
	// (if we did it together, the player would stop moving in any direction if they hit a wall at all)
	if(ctx->map.map[(u32)(player->pos.y + player->vel.y * ctx->mlx->delta_time + ((player->vel.y > 0.0f) ? 0.05f : -0.05f)) * ctx->map.width + (u32)player->pos.x] != '#')
		player->pos.y += player->vel.y * ctx->mlx->delta_time;

	if(mlx_is_key_down(ctx->mlx, MLX_KEY_A))
	{ // accumulate look velocity
		if(player->dirvel > 0.0f) // if we're already turning right, turn left faster
			player->dirvel -= PLAYER_DIR_ACCEL * 2.0f * ctx->mlx->delta_time;
		else
			player->dirvel -= PLAYER_DIR_ACCEL * ctx->mlx->delta_time;
		if(player->dirvel < -PLAYER_DIR_SPEED) // our max look velocity
			player->dirvel = -PLAYER_DIR_SPEED;
	}
	else if(mlx_is_key_down(ctx->mlx, MLX_KEY_D))
	{
		if(player->dirvel < 0.0f)
			player->dirvel += PLAYER_DIR_ACCEL * 2.0f * ctx->mlx->delta_time;
		else
			player->dirvel += PLAYER_DIR_ACCEL * ctx->mlx->delta_time;
		if(player->dirvel > PLAYER_DIR_SPEED)
			player->dirvel = PLAYER_DIR_SPEED;
	}
	else
	{ // slow down the look velocity if we're not turning
		if(player->dirvel < 0.0f)
		{
			player->dirvel += PLAYER_DIR_ACCEL * 2.0f * ctx->mlx->delta_time;
			if(player->dirvel > 0.0f)
				player->dirvel = 0.0f;
		}
		else if(player->dirvel > 0.0f)
		{
			player->dirvel -= PLAYER_DIR_ACCEL * 2.0f * ctx->mlx->delta_time;
			if(player->dirvel < 0.0f)
				player->dirvel = 0.0f;
		}
	}
	// the vector of the direction we're facing which we use to move the player in that direction
	const Vec2 dir = {cosf(player->dir), sinf(player->dir)}; // look up using cos and sin for circles (it's cool)
	if(mlx_is_key_down(ctx->mlx, MLX_KEY_W))
	{ // accumulate velocity in the direction we're facing
		player->vel.x += dir.x * ctx->mlx->delta_time * PLAYER_ACCEL;
		player->vel.y += dir.y * ctx->mlx->delta_time * PLAYER_ACCEL;
		if(fabsf(player->vel.x) > fabsf(dir.x) * PLAYER_SPEED) // abs because we don't want to go backwards
			player->vel.x = dir.x * PLAYER_SPEED; // clamp the velocity to the max speed
		if(fabsf(player->vel.y) > fabsf(dir.y) * PLAYER_SPEED)
			player->vel.y = dir.y * PLAYER_SPEED;
	}
	else if(mlx_is_key_down(ctx->mlx, MLX_KEY_S))
	{
		player->vel.x -= dir.x * ctx->mlx->delta_time * PLAYER_ACCEL;
		player->vel.y -= dir.y * ctx->mlx->delta_time * PLAYER_ACCEL;
		if(fabsf(player->vel.x) > fabsf(dir.x) * PLAYER_SPEED)
			player->vel.x = -dir.x * PLAYER_SPEED;
		if(fabsf(player->vel.y) > fabsf(dir.y) * PLAYER_SPEED)
			player->vel.y = -dir.y * PLAYER_SPEED;
	}
	else
	{ // slow down the velocity if we're not moving
		player->vel.x -= player->vel.x * ctx->mlx->delta_time * PLAYER_ACCEL;
		player->vel.y -= player->vel.y * ctx->mlx->delta_time * PLAYER_ACCEL;
		if(fabsf(player->vel.x) < 0.01f)
			player->vel.x = 0.0f; // clamp the velocity to 0 if it's close enough
		if(fabsf(player->vel.y) < 0.01f)
			player->vel.y = 0.0f;
	}

	// flicker the lantern by changing its radius target randomly every 10th of a second
	player->lantern.timer += ctx->mlx->delta_time;
	if(player->lantern.timer > 0.1f)
	{
		player->lantern.timer -= 0.1f;
		player->lantern.radiustarget = sqrtf(rand() / (float)RAND_MAX * 0.1f) + player->lantern.scale;
	}
	// inch the lantern radius towards the target
	player->lantern.radius -= (player->lantern.radius - player->lantern.radiustarget) * ctx->mlx->delta_time * 3.0f;
}
