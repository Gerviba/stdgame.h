/**
 * @file object.h
 * @author Gerviba (Szabo Gergely)
 * @brief Static, dynamic and active object loader and renderer. ReferencePoint manager. (header)
 *
 * @par Definition:
 * 		object.c
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "stdgame.h"

/**
 * Object part type
 *
 * **Format**: 0bUDTRBL
 *
 * |Key| Meaning |
 * |:-:|---------|
 * | U | UP      |
 * | D | DOWN    |
 * | T | TOP     |
 * | R | RIGHT   |
 * | B | BOTTOM  |
 * | L | LEFT    |
 *
 * Similar to the CSS box model.
 */
typedef enum {
	PT_NULL 							= 0b000000,
	PT_LEFT								= 0b000001,
	PT_BOTTOM							= 0b000010,
	PT_LEFT_BOTTOM						= 0b000011,
	PT_RIGHT 							= 0b000100,
	PT_RIGHT_LEFT						= 0b000101,
	PT_RIGHT_BOTTOM						= 0b000110,
	PT_RIGHT_BOTTOM_LEFT				= 0b000111,
	PT_TOP								= 0b001000,
	PT_TOP_LEFT							= 0b001001,
	PT_TOP_BOTTOM						= 0b001010,
	PT_TOP_BOTTOM_LEFT					= 0b001011,
	PT_TOP_RIGHT						= 0b001100,
	PT_TOP_RIGHT_LEFT					= 0b001101,
	PT_TOP_RIGHT_BOTTOM 				= 0b001110,
	PT_TOP_RIGHT_BOTTOM_LEFT			= 0b001111,
	PT_DOWN 							= 0b010000,
	PT_DOWN_LEFT						= 0b010001,
	PT_DOWN_BOTTOM						= 0b010010,
	PT_DOWN_LEFT_BOTTOM					= 0b010011,
	PT_DOWN_RIGHT 						= 0b010100,
	PT_DOWN_RIGHT_LEFT					= 0b010101,
	PT_DOWN_RIGHT_BOTTOM				= 0b010110,
	PT_DOWN_RIGHT_BOTTOM_LEFT			= 0b010111,
	PT_DOWN_TOP							= 0b011000,
	PT_DOWN_TOP_LEFT					= 0b011001,
	PT_DOWN_TOP_BOTTOM					= 0b011010,
	PT_DOWN_TOP_BOTTOM_LEFT				= 0b011011,
	PT_DOWN_TOP_RIGHT					= 0b011100,
	PT_DOWN_TOP_RIGHT_LEFT				= 0b011101,
	PT_DOWN_TOP_RIGHT_BOTTOM 			= 0b011110,
	PT_DOWN_TOP_RIGHT_BOTTOM_LEFT		= 0b011111,
	PT_UP 								= 0b100000,
	PT_UP_LEFT							= 0b100001,
	PT_UP_BOTTOM						= 0b100010,
	PT_UP_LEFT_BOTTOM					= 0b100011,
	PT_UP_RIGHT 						= 0b100100,
	PT_UP_RIGHT_LEFT					= 0b100101,
	PT_UP_RIGHT_BOTTOM					= 0b100110,
	PT_UP_RIGHT_BOTTOM_LEFT				= 0b100111,
	PT_UP_TOP							= 0b101000,
	PT_UP_TOP_LEFT						= 0b101001,
	PT_UP_TOP_BOTTOM					= 0b101010,
	PT_UP_TOP_BOTTOM_LEFT				= 0b101011,
	PT_UP_TOP_RIGHT						= 0b101100,
	PT_UP_TOP_RIGHT_LEFT				= 0b101101,
	PT_UP_TOP_RIGHT_BOTTOM 				= 0b101110,
	PT_UP_TOP_RIGHT_BOTTOM_LEFT			= 0b101111,
	PT_UP_DOWN 							= 0b110000,
	PT_UP_DOWN_LEFT						= 0b110001,
	PT_UP_DOWN_BOTTOM					= 0b110010,
	PT_UP_DOWN_LEFT_BOTTOM				= 0b110011,
	PT_UP_DOWN_RIGHT 					= 0b110100,
	PT_UP_DOWN_RIGHT_LEFT				= 0b110101,
	PT_UP_DOWN_RIGHT_BOTTOM				= 0b110110,
	PT_UP_DOWN_RIGHT_BOTTOM_LEFT		= 0b110111,
	PT_UP_DOWN_TOP						= 0b111000,
	PT_UP_DOWN_TOP_LEFT					= 0b111001,
	PT_UP_DOWN_TOP_BOTTOM				= 0b111010,
	PT_UP_DOWN_TOP_BOTTOM_LEFT			= 0b111011,
	PT_UP_DOWN_TOP_RIGHT				= 0b111100,
	PT_UP_DOWN_TOP_RIGHT_LEFT			= 0b111101,
	PT_UP_DOWN_TOP_RIGHT_BOTTOM 		= 0b111110,
	PT_UP_DOWN_TOP_RIGHT_BOTTOM_LEFT	= 0b111111,
} PartType;

/** Render UP part mask */
#define PTMASK_RENDER_UP		PT_UP
/** Render DOWN part mask */
#define PTMASK_RENDER_DOWN		PT_DOWN
/** Render TOP part mask */
#define PTMASK_RENDER_TOP		PT_TOP
/** Render RIGHT part mask */
#define PTMASK_RENDER_RIGHT		PT_RIGHT
/** Render BOTTOM part mask */
#define PTMASK_RENDER_BOTTOM	PT_BOTTOM
/** Render LEFT part mask */
#define PTMASK_RENDER_LEFT		PT_LEFT

/**
 * Cube part of the objects. The smallest unit
 *
 * Used in all the three object types.
 * @see StaticObject
 * @see DynamicObject
 * @see ActiveObject
 */
struct StaticObjectPart {
	PartType type;
	GLfloat *color; // -> colors[4]
	GLfloat position[3];
};

/**
 * Color of a cube. Stored in color banks.
 */
struct PartColor {
	int id;
	GLfloat color[4];
};

/**
 * StaticObject - Precalculated move matrix
 *
 * Cannot be moved
 * @par File format:
 * 		sobj
 * @see StaticObjectInstance
 */
struct StaticObject {
	int id;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLfloat moveMat[16];
	LinkedList /*StaticObjectPart*/ *parts;
	LinkedList /*PartColor*/ *colors;
};

/**
 * Instance of a StaticObject logically
 *
 * @see StaticObject
 */
struct StaticObjectInstance {
	int id;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLfloat moveMat[16];
	GLboolean visible;
	StaticObject *object;
};

/**
 * Dynamically changing reference points for animations
 *
 * @see fileformats.md
 */
struct ReferencePoint {
	GLint id;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLfloat timing;
};

/**
 * DynamicObject - Movable object
 *
 * @par File format:
 * 		dobj
 * @see DynamicObjectInstance
 */
struct DynamicObject {
	int id;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLfloat moveMat[16];
	LinkedList /*StaticObjectPart*/ *parts;
	LinkedList /*PartColor*/ *colors;
};

/**
 * Instance of a DynamicObject logically
 *
 * @see DynamicObject
 */
struct DynamicObjectInstance {
	int id;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLfloat moveMat[16];
	GLboolean visible;
	ReferencePoint *reference;
	DynamicObject *object;
};

/**
 * ActiveObject - Set of DynamicObject
 *
 * Used in animated objects.
 * @par File format:
 * 		aobj
 * @see ActiveObjectInstance
 */
struct ActiveObject {
	int id;
	int size;
	DynamicObject *parts;
};

/**
 * Instance of a ActiveObject logically
 *
 * @see ActiveObject
 */
struct ActiveObjectInstance {
	int id;
	int activePart;
	GLfloat position[3];
	GLfloat rotation[3];
	GLfloat scale[3];
	GLboolean visible;
	ActiveObject *object;
	GLfloat moveMat[16];
	ReferencePoint *reference;
};

/**
 * Object storage
 */
struct ObjectInfo {
	LinkedList /*StaticObject*/ *staticObjects;
	LinkedList /*StaticObjectInstance*/ *staticInstances;

	LinkedList /*DynamicObject*/ *dynamicObjects;
	LinkedList /*DynamicObjectInstance*/ *dynamicInstances;

	LinkedList /*ActiveObject*/ *activeObjects;
	LinkedList /*ActiveObjectInstance*/ *activeInstances;
};

/**
 * Cursor object and info storage
 */
struct Cursor {
	ActiveObject *cursorObject;
	ActiveObjectInstance *pointer;
};

StaticObject *loadStaticObject(char[]);
DynamicObject *loadDynamicObject(char[]);
ActiveObject *loadActiveObject(char[]);

void renderStaticObject(GameInstance*, StaticObjectInstance*);
void renderDynamicObject(GameInstance*, DynamicObjectInstance*);
void renderActiveObject(GameInstance*, ActiveObjectInstance*);
void renderTile(GameInstance*, Tile*);
void initStraticInstance(StaticObjectInstance*);

void initReferencePoints(GameInstance *this);
void updateReferencePoint(GameInstance *this, GLfloat delta);

#endif /* OBJECT_H_ */
