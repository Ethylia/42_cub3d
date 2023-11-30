#include "map/map.h"
#include "render/raycast.h"

#include <math.h>

const float MAXDIST = 6.0f;

Hit raycast(const Ray ray, Map* map)
{
	// http://www.cse.yorku.ca/~amana/research/grid.pdf
	// paper on algo to find the intersections with grid sections of a ray
	// for optimizing a raycasted scene by only checking intersection of
	// the more complex objects if they are part of the intersected grid regions.
	// I'm using it here since our walls are in a grid so it's a perfect fit since
	// it starts from the ray origin and can stop at the first intersection which
	// lets us easily find the closest wall very efficiently.
	Hit hit = {{0.0f, 0.0f}, 0, 10.0f};
	Vec2 delta = {fabsf(1 / ray.dir.x), fabsf(1 / ray.dir.y)}; // distance along ray to move 1 unit in X or Y
	i64 stepx = ray.dir.x < 0.0f ? -1 : 1; // direction to move in X or Y
	i64 stepy = ray.dir.y < 0.0f ? -1 : 1;
	u64 x = (u64)ray.pos.x; // the grid position of the ray origin
	u64 y = (u64)ray.pos.y;
	// Maxt is the distance along the ray to the next X or Y grid line
	// we start them at the distance to the next X or Y grid line from the ray origin
	Vec2 maxt = {ray.dir.x < 0 ? (ray.pos.x - x) * delta.x : (x + 1 - ray.pos.x) * delta.x,
		ray.dir.y < 0 ? (ray.pos.y - y) * fabsf(delta.y) : (y + 1 - ray.pos.y) * fabsf(delta.y)};
	while(1)
	{ // loop until we hit a wall or go too far
		if(maxt.x < maxt.y)
		{ // if the distance to the next X grid line is less than the distance to the next Y grid line
			if(maxt.x > MAXDIST) // if the distance to the next X grid line is greater than the max distance we want to check
				return hit; // return the hit (it's set to 10.0f distance by default)
			x += stepx; // move to the next X grid line
			if(map->map[y * map->width + x] == '#')
			{ // if the grid line we moved to is a wall
				hit.pos.x = ray.dir.x * maxt.x + ray.pos.x; // set the hit position to the intersection point
				hit.pos.y = ray.dir.y * maxt.x + ray.pos.y; // it's the ray's direction times the distance to the intersection point plus the ray's origin
				hit.face = ray.dir.x < 0 ? FACEWEST : FACEEAST; // set the face of the wall that was hit
				hit.dist = maxt.x; // set the distance to the intersection point
				return hit;
			}
			maxt.x += delta.x; // add the distance to the next X grid line
		}
		else
		{ // same but for Y grid lines
			if(maxt.y > MAXDIST)
				return hit;
			y += stepy;
			if(map->map[y * map->width + x] == '#')
			{
				hit.pos.x = ray.dir.x * maxt.y + ray.pos.x;
				hit.pos.y = ray.dir.y * maxt.y + ray.pos.y;
				hit.face = ray.dir.y < 0 ? FACESOUTH : FACENORTH;
				hit.dist = maxt.y;
				return hit;
			}
			maxt.y += delta.y;
		}
	}
}
