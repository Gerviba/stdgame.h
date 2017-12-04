/**
 * @file map.h
 * @author Gerviba (Szabo Gergely)
 * @brief Map and menu loader and utilities (header)
 *
 * @par Definition:
 * 		map.c
 */

#ifndef MAP_H_
#define MAP_H_

#include <time.h>
#include "object.h"
#include "linkedlist.h"
#include "menu.h"

/**
 * Format: 0bRLTBF
 *
 * |Char|Meaning                |
 * |----|-----------------------|
 * | T  | TOP                   |
 * | R  | RIGHT                 |
 * | B  | BOTTOM                |
 * | L  | LEFT                  |
 * | F  | FOREGROUND/BACKGROUND |
 *
 * It's like in the CSS box model
 *
 * @note The viewport is same as in the camera.
 * @see TTMASK_RENDER_BOTTOM
 * @see TTMASK_RENDER_TOP
 * @see TTMASK_RENDER_LEFT
 * @see TTMASK_RENDER_RIGHT
 * @see TTMASK_RENDER_FRONT
 *
 * Physics on when: (TileType & 0b11110) != 0
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

#define ACTION_VALUE_DONT_CARE 	-65536
#define ACTION_LOSE_ID 			-10000
#define HEALT_COMPONENT_ID 		-1000
#define SCORE_COMPONENT_ID 		-1001
#define ENTITY_FLOATING_REFERENCEPOINT_ID 8

/**
 * Point light object
 */
struct Light {
	GLint id;
	GLfloat position[3];
	GLfloat strength;
	GLfloat color[3];
	GLfloat specular;
	GLfloat intensity;
	ReferencePoint *reference;
	GLboolean visible;
};

/**
 * Loaded texture
 */
struct Texture {
	int id;
	GLuint textureId;
};

/**
 * Block sides textures
 */
struct TextureBlock {
	int id;
	Texture *base;
	Texture *top;
	Texture *right;
	Texture *bottom;
	Texture *left;
};

/**
 * Map tile
 */
struct Tile {
	float x, y;
	TextureBlock *texture;
	TileType type;
};

/**
 * Type of an action
 * @see Action
 */
typedef enum {
	/** Teleport to: x y */
	ACTION_TELEPORT,
	/** Damage: count */
	ACTION_DAMAGE,
	/** Score: count */
	ACTION_ADD_SCORE,
	/**
	 * Set dynamic object:
	 * dobj.id x y z alpha beta gamma SizeX SizeY SizeZ visible visible
	 * (-10000 = don't care)
	 * @see DynamicObject
	 * @see DynamicObjectInstance
	 */
	ACTION_SET_DOBJ,
	/**
	 * Set active object:
	 * aobj.id x y z alpha beta gamma SizeX SizeY SizeZ visible
	 * (-10000 = don't care)
	 * @see ActiveObject
	 * @see ActiveObjectInstance
	 */
	ACTION_SET_AOBJ,
	/** Set grabbed item to the selectem: itemId */
	ACTION_SET_ITEM,
	/**
	 * Set light object:
	 * light.id x y z strength rrggbb specular intensity reference
	 * @see Light
	 */
	ACTION_SET_LIGHT,
	/**
	 * Object psysics setter:
	 * id allowed
	 * @see PhysicsArea
	 */
	ACTION_OBJECT_PSX,
	/** Win the game */
	ACTION_WIN,
	/** Loose the game  */
	ACTION_LOSE
} ActionType;

/**
 * Action instance
 */
struct Action {
	/** It not required to be unique */
	GLint id;
	ActionType type;
	GenericType *value;
};

/**
 * Physics area
 *
 * Players can't enter if it is enabled
 */
struct PhysicsArea {
	GLint id;
	GLfloat x;
	GLfloat y;
	GLboolean enabled;
};

/**
 * Action activator region
 */
struct Region {
	GLfloat xMin, yMin;
	GLfloat xMax, yMax;
	GLint actionId;
	GLint maxUse;
	GLint itemReq;
	GLboolean notSneek;
};

/**
 * Text message (rendered as 3D object)
 */
struct Message {
	Position position;
	Color color;
	FontSize size;
	char message[100];
};

/**
 * Mobs, monsters
 */
struct Entity {
	GLint id;
	ActiveObjectInstance *obj;
	GLint lightId;
	GLfloat spellSpeed;
	GLfloat damage;
	GLfloat hp;
	GLint score;
	GLfloat fi0;
	GLfloat radius;
};

/**
 * The game map or menu
 */
struct Map {
	char name[64];
	char author[64];
	char description[242];
	float ambient[3];
	float spawn[3];

	LinkedList /*Tile*/ *tiles;
	LinkedList /*Light*/ *lights;
	LinkedList /*Texture*/ *textures;
	LinkedList /*TextureBlock*/ *textureBlocks;
	LinkedList /*Action*/ *actions;
	LinkedList /*Region*/ *regions;
	LinkedList /*Message*/ *messages;
	LinkedList /*PhysicsArea*/ *physics;
	LinkedList /*Entity*/ *entities;

	ObjectInfo *objects;
	Menu *menu;
	Spells *spells;

	int score;
	float healt;
	GLboolean allowMovement;
	time_t startTime;
};

/**
 * Record file line
 */
struct HighScoreValue {
	char name[255];
	int type;
	int value;
};

Map* loadMap(GameInstance *this, char path[]);
void freeMap(Map *map);

void getKeyName(GameInstance *this, char *str, int key);
void getOptionCaption(GameInstance *this, char *name, char *str, int id);

void addTextComponent(Map* map, char text[100], int id, RelativeX relX, RelativeY relY,
		Align align, GLfloat x, GLfloat y, FontSize size);
void addTextComponentColor(Map* map, char text[100], int id, RelativeX relX, RelativeY relY,
		Align align, GLfloat pos[3], GLfloat color[4], FontSize size);
void updateHightScore(GameInstance *this, time_t deltaT);

#endif /* MAP_H_ */
