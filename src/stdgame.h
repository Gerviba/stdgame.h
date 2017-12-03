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

#include <string.h>
#include <gl.h>
#include <glext.h>
#include <glfw3.h>
#include "SOIL.h"

/**
 * 3D position type
 *
 * The `position.xyz` returns a 3-dimensional GLfloat array, and `position.x`, `position.y` and
 * `position.z` returns a component of the selected dimension.
 */
typedef union Position {
	GLfloat xyz[3];
	struct {
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};
} Position, Rotation, Velocity, Scale;

/**
 * RGBA color type
 *
 * The `color.rgba` returns a 4-dimensional GLfloat array, the `color.r` returns the red, the `color.g`
 * returns green, the `color.b` return the blue and the `color.a `returns the alpha component of the
 * RGBA color.
 */
typedef union Color {
	GLfloat rgba[4];
	struct {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a;
	};
} Color;

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
typedef struct Region Region;
typedef struct Action Action;
typedef struct PhysicsArea PhysicsArea;
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

/** PI constant */
static const float PI = 3.14159265358979323846f;

/** X constant for coordinate arrays */
#define X 0
/** Y constant for coordinate arrays */
#define Y 1
/** Z constant for coordinate arrays */
#define Z 2

/** Red constant for color arrays */
#define R 0
/** Green constant for color arrays */
#define G 1
/** Blue constant for color arrays */
#define B 2
/** Alpha constant for color arrays */
#define A 3

#include "linkedlist.h"
#include "font.h"
#include "object.h"
#include "player.h"
#include "components.h"
#include "map.h"
#include "game.h"
#include "events.h"

/** Minimum of numeric type */
#define min(a, b) (a < b ? a : b)
/** Maximum of numeric type */
#define max(a, b) (a > b ? a : b)
/** Sign of numeric type */
#define sig(a) (a == 0 ? 0 : a > 0 ? 1 : -1)
/** String equals */
#define equals(a, b) (strcmp(a, b) == 0)
/** Allocate space for new of generic type */
#define new(x) malloc(sizeof(x))
/** Set 3D array */
#define array3(array, a, b, c) ({array[0] = a; array[1] = b; array[2] = c;})
/** Set 3D array */
#define array4(array, a, b, c, d) ({array[0] = a; array[1] = b; array[2] = c; array[3] = d;})

void initGLFW();
void createWindow(GameInstance* this, const GLFWvidmode* mode);
void setupOpenGL(GameInstance* this, double width, double height);
void setupWindowSize(const GLFWvidmode* mode, GameInstance* this);
void setPerspective(GameInstance *this, float fov, float aspect, float near, float far);
void printVersionInfo();
void initCursor(GameInstance* this);
void fixViewport(GameInstance* this);
void doGameLoop(GameInstance* this);
void getGameInstance(GameInstance **this);

#endif /* STDGAME_H__ */
