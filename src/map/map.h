#ifndef MAP_H
#define MAP_H

#include "common.h"

typedef struct {
	u32 width;
	u32 height;
	Vec2 start;
	float startdir;
	char *map;
} Map;

int loadmap(const char* filename, Map *map);

#endif
