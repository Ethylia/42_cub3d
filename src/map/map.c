#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int loadfile(const char* filename, char** buffer)
{
	FILE* file = fopen(filename, "r");
	if(!file)
		return 0;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);
	*buffer = malloc(size + 1);
	if(!*buffer)
		return 0;
	fread(*buffer, 1, size, file);
	(*buffer)[size] = '\0';
	fclose(file);
	return 1;
}

static int isplayer(char c)
{
	return c == 'S' || c == 'N' || c == 'E' || c == 'W';
}

static int verifymap(Map* map)
{
	if(!map->width || !map->height)
		return 0;

	for(u32 i = 0; i < map->width * map->height; ++i)
	{
		if(map->map[i] != '#' && map->map[i] != ' '
			&& !isplayer(map->map[i]))
			return 0;
		if(isplayer(map->map[i]))
		{
			if(map->start.x > 0.0f)
				return 0;
			map->start.x = (i % map->width) + 0.5f;
			map->start.y = (i / map->width) + 0.5f;
			map->startdir = M_PI * (map->map[i] == 'E' ? 0.0f : map->map[i] == 'N' ? 0.5f : map->map[i] == 'W' ? 1.0f : 1.5f);
		}
		if(!((i % map->width) % (map->width - 1)) || !((i / map->width) % (map->height - 1)))
			map->map[i] = '#'; // Surround the map with walls
	}
	return map->start.x > 0.0f;
}

int loadmap(const char* filename, Map* map)
{
	char* buffer;
	if(!loadfile(filename, &buffer))
		return 0;
	map->width = 0;
	map->height = 0;
	map->start = (Vec2){0.0f, 0.0f};
	u32 linelen = 0;

	for(char* c = buffer;; ++c)
	{
		if(*c == '\n' || *c == '\0')
		{
			if(!map->width)
				map->width = linelen;
			else if(map->width != linelen)
			{
				free(buffer);
				return 0;
			}
			++map->height;
			linelen = 0;
			if(*c == '\0')
				break;
		}
		else
			++linelen;
	}
	map->map = malloc(map->width * map->height);
	if(!map->map)
	{
		free(buffer);
		return 0;
	}
	for(u32 i = 0; i < (map->width + 1) * map->height; ++i)
		if(buffer[i] != '\n')
			map->map[i - (i / (map->width + 1))] = buffer[i];
	if(!verifymap(map))
	{
		free(buffer);
		return 0;
	}
	return 1;
}
