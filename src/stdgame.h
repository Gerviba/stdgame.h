/**
 * @file stdgame.h
 * @author Gerviba (Szabo Gergely)
 * @brief The main header of the project
 */

#ifndef STDGAME_H__
#define STDGAME_H__

#include <stdio.h>

/** Debug modes: ***************/
//#define DEBUG 1
//#define DEBUG_MOVEMENT 1
//#define DEBUG_MALLOC 1
#define DEBUG_MESSAGES 1
//#define DEBUG_LIGHT 1
#define WARNING_MESSAGES 1
/*******************************/

/**
 * Debug message macro.
 * @warning Requires GCC to process `##__VA_ARGS__`.
 */
#ifdef DEBUG_MESSAGES
	#define DEBUG(module, message, ...) { \
		printf("[%s] ", module); \
		printf(message, ##__VA_ARGS__); \
		printf("\n"); }
#else
	#define DEBUG(...) ;
#endif

/**
 * Error message
 */
#define ERROR(message, ...) { \
	fprintf(stderr, "[Error] "); \
	fprintf(stderr, message, ##__VA_ARGS__); \
	fprintf(stderr, "\n"); }

/**
 * Warning message
 */
#ifdef WARNING_MESSAGES
	#define WARNING(message, ...) { \
		printf("[Warning] "); \
		printf(message, ##__VA_ARGS__); \
		printf("\n"); }
#else
	#define WARNING(...) ;
#endif

// character.h
typedef struct Player Player;

// components.h
typedef struct GenericType GenericType;
typedef struct TextComponent TextComponent;
typedef struct ObjectComponent ObjectComponent;
typedef struct ImageComponent ImageComponent;
typedef struct Component Component;

// font.h
typedef struct CharPart CharPart;
typedef struct Char Char;
typedef struct Font Font;

// map.h
typedef struct Light Light;
typedef struct Texture Texture;
typedef struct TextureBlock TextureBlock;
typedef struct Tile Tile;
typedef struct EventRegion EventRegion;
typedef struct MessageRegion MessageRegion;
typedef struct Map Map;

// menu.h
typedef struct Menu Menu;

// object.h
typedef struct StaticObjectPart StaticObjectPart;
typedef struct PartColor PartColor;
typedef struct StaticObject StaticObject;
typedef struct StaticObjectInstance StaticObjectInstance;
typedef struct ReferencePoint ReferencePoint;
typedef struct DynamicObject DynamicObject;
typedef struct DynamicObjectInstance DynamicObjectInstance;
typedef struct ActiveObject ActiveObject;
typedef struct ActiveObjectInstance ActiveObjectInstance;
typedef struct ObjectInfo ObjectInfo;
typedef struct Cursor Cursor;

// game.h
typedef struct GameInstance GameInstance;
typedef struct Options Options;
typedef struct InputActionWrapper InputActionWrapper;
typedef struct ShaderInfo ShaderInfo;
typedef struct CameraInfo CameraInfo;
typedef struct LigingInfo LigingInfo;
typedef union Position Position, Rotation, Velocity, Scale;
typedef union Color Color;

/** PI constant */
static const float PI = 3.14159265358979323846;

/** @deprecated Use Position.x insted. */
#define X 0
/** @deprecated Use Position.y insted. */
#define Y 1
/** @deprecated Use Position.z insted. */
#define Z 2

/** @deprecated Use Color.r insted. */
#define R 0
/** @deprecated Use Color.g insted. */
#define G 1
/** @deprecated Use Color.b insted. */
#define B 2
/** @deprecated Use Color.a insted. */
#define A 3

#include <string.h>

#include <gl.h>
#include <glext.h>
#include <glu.h>
#include <glfw3.h>
#include "SOIL.h"

#include "linkedlist.h"
#include "font.h"
#include "object.h"
#include "player.h"
#include "map.h"
#include "components.h"
#include "game.h"

/** Minimum of numeric type */
#define min(a, b) a < b ? a : b
/** Maximum of numeric type */
#define max(a, b) a > b ? a : b
/** Sign of numeric type */
#define sig(a) a == 0 ? 0 : a > 0 ? 1 : -1
/** String equals */
#define equals(a, b) strcmp(a, b) == 0
/** Allocate space for new of generic type */
#define new(x) malloc(sizeof(x))

#endif /* STDGAME_H__ */
