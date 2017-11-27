#ifndef MAP_H_
#define MAP_H_

#include "object.h"
#include "linkedlist.h"
#include "menu.h"

/**
 * 0bRLTBF
 * T = TOP
 * R = RIGHT
 * B = BOTTOM
 * L = LEFT
 * F = FOREGROUND/BACKGROUND
 *
 * Olyan mint a css box-modellben.
 *
 * NOTE: Az itt feltüntetett irányok a néző és a block alapján van.
 * A rendereléshez lásd:
 * - TTMASK_RENDER_BOTTOM
 * - TTMASK_RENDER_TOP
 * - TTMASK_RENDER_LEFT
 * - TTMASK_RENDER_RIGHT
 * - TTMASK_RENDER_FRONT
 *
 * Lehet rajta járni ha: (TileType & 0b11110) != 0
 */
typedef enum {
	TT_BACKGROUND					= 0b00000,	// 0
	TT_FOREGROUND					= 0b00001,	// 1

	TT_BORDER_BOTTOM				= 0b00011,	// 3
	TT_BORDER_TOP					= 0b00101,	// 5
	TT_BORDER_RIGHT					= 0b01001,	// 9
	TT_BORDER_LEFT					= 0b10001,	// 17

	TT_BORDER_TOP_BOTTOM			= 0b00111,	// 7
	TT_BORDER_BOTTOM_RIGHT			= 0b01011,	// 11
	TT_BORDER_TOP_RIGHT				= 0b01101,	// 13
	TT_BORDER_BOTTOM_LEFT			= 0b10011,	// 19
	TT_BORDER_TOP_LEFT				= 0b10101,	// 21
	TT_BORDER_LEFT_RIGHT			= 0b11001,	// 25

	TT_BORDER_TOP_BOTTOM_RIGHT		= 0b01111,	// 15
	TT_BORDER_TOP_BOTTOM_LEFT		= 0b10111,	// 23
	TT_BORDER_BOTTOM_LEFT_RIGHT		= 0b11011,	// 27
	TT_BORDER_TOP_LEFT_RIGHT		= 0b11101,	// 29

	TT_BORDER_ALL					= 0b11111,	// 31
} TileType;

#define TTMASK_RENDER_BOTTOM	TT_BORDER_TOP
#define TTMASK_RENDER_TOP 		TT_BORDER_BOTTOM
#define TTMASK_RENDER_LEFT 		TT_BORDER_RIGHT
#define TTMASK_RENDER_RIGHT 	TT_BORDER_LEFT
#define TTMASK_RENDER_FRONT 	TT_FOREGROUND

#define MOVE_BLOCK_Y			(TT_BORDER_TOP | TT_BORDER_BOTTOM) & 0b11110
#define MOVE_BLOCK_X			(TT_BORDER_RIGHT | TT_BORDER_LEFT) & 0b11110

struct Light {
	GLfloat position[3];
	GLfloat strength;
	GLfloat color[3];
	GLfloat specular;
	GLfloat intensity;
	ReferencePoint *reference;
};

struct Texture {
	int id;
	GLuint textureId;
};

struct TextureBlock {
	int id;
	Texture *base;
	Texture *top;
	Texture *right;
	Texture *bottom;
	Texture *left;
};

struct Tile {
	float x, y;
	TextureBlock *texture;
	TileType type;
};

struct EventRegion {
	float x1, y1;
	float x2, y2;
	int modHP;
	int modMana;
	float modG;
};

struct MessageRegion {
	float x1, y1;
	float x2, y2;
	char message1[100];
	char message2[100];
};

struct Map {
	char name[64];
	char author[64];
	char description[242];
	float ambient[3];
	float spawn[3];
	float end[3];

	LinkedList /*Tile*/ *tiles;
	LinkedList /*Light*/ *lights;
	LinkedList /*Texture*/ *textures;
	LinkedList /*TextureBlock*/ *textureBlocks;

	ObjectInfo *objects;
	Menu *menu;
};

void getKeyName(GameInstance *this, char *str, int key);
void getOptionCaption(GameInstance *this, char *name, char *str, int id);

#endif /* MAP_H_ */
