#ifndef RAYCAST_H
#define RAYCAST_H

#include "common.h"

enum Face
{
	FACENORTH,
	FACESOUTH,
	FACEEAST,
	FACEWEST
};

typedef struct
{
	Vec2 pos;
	Vec2 dir;
} Ray;

typedef struct
{
	Vec2 pos;
	int face;
	float dist;
} Hit;

Hit raycast(const Ray ray, Map* map);

#endif
