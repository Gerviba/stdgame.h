/**
 * @file game.h
 * @author Gerviba (Szabo Gergely)
 * @brief Game initialisation and game loop
 */

#ifndef GAME_H_
#define GAME_H_

#include "stdgame.h"
#include "font.h"
#include "object.h"
#include "map.h"
#include "player.h"

/** Maximum allowed lights to render */
#define MAX_NUM_LIGHTS 28

/** Game state */
typedef enum {
	/** Main menu like menu */
	MENU,
	/** Ingame GUI with player movement allowed */
	INGAME,
	/** Ingame GUI or menu with player movement and mob timing disabled */
	PAUSE
} GameState;

/**
 * It contains all the data that the game requires.
 */
struct GameInstance {
	ShaderInfo *shader;
	CameraInfo *camera;
	Map *map;
	LigingInfo *lighting;
	Player *player;
	Font *font;
	Options *options;
	Cursor *cursor;

	GLuint tileVAO;
	GLuint blankTextureId;
	GLFWwindow *window;
	int score;
	GameState state;

	LinkedList /*ReferencePoint*/ *referencePoints;
};

struct Options {
	GLuint msaa;
	GLboolean fullscreen;
	GLuint height;
	GLuint width;
	GLboolean shadow;
	GLboolean cameraMovement;
	GLfloat tanFov;
	GLfloat aspectRatio;

	int moveLeft[2];
	int moveRight[2];
	int jump[2];
	int sneek[3];
	int attack[2];
	int speell1[2];
	int speell2[2];
	int speell3[2];
	int speell4[2];
	int menu;

};

struct InputActionWrapper {
	enum {KEYBOARD, MOUSE, GAMEPAD} type;
	int identifier;
};

struct ShaderInfo {
	GLuint shaderId;
	GLuint cameraPosition;
	GLuint lightPosition;
	GLuint lightColor;
	GLuint lightInfo;
	GLuint texturePosition;
	GLuint numLights;
	GLuint baseColor;
	GLuint projMat;
	GLuint viewMat;
	GLuint moveMat;
	GLuint modelMat;
};

struct CameraInfo {
	GLfloat rotation[3];
	GLfloat position[3];
	GLfloat projMat[16];
	GLfloat viewMat[16];

	GLfloat destinationRotation[3];
	GLfloat destinationPosition[3];
};

struct LigingInfo {
	int numLights;
	GLfloat lightPosition[MAX_NUM_LIGHTS * 3];
	GLfloat lightColor[MAX_NUM_LIGHTS * 3];
	GLfloat lightInfo[MAX_NUM_LIGHTS * 3];
};

/**
 * 3D position type
 *
 * The `position.xyz` returns a 3-dimensional GLfloat array, and `position.x`, `position.y` and
 * `position.z` returns a component of the selected dimension.
 */
union Position {
	GLfloat xyz[3];
	struct {
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};
};

/**
 * RGBA color type
 *
 * The `color.rgba` returns a 4-dimensional GLfloat array, the `color.r` returns the red, the `color.g`
 * returns green, the `color.b` return the blue and the `color.a `returns the alpha component of the
 * RGBA color.
 */
union Color {
	GLfloat rgba[4];
	struct {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a;
	};
};

/** Color value setter */
#define setColor(__color, r, g, b, a) {__color[0] = r; __color[1] = g; __color[2] = b; __color[3] = a;}

/** Position value setter */
#define setPosition(position, x, y, z) {position[0] = x; position[1] = y; position[2] = z;}

/** Rotation value setter */
#define setRotation(rotation, x, y, z) {rotation[0] = x; rotation[1] = y; rotation[2] = z;}

/** Scale value setter */
#define setScale(scale, x, y, z) {scale[0] = x; scale[1] = y; scale[2] = z;}

/** Velocity value setter */
#define setVelocity(velocity, x, y, z) {velocity[0] = x; velocity[1] = y; velocity[2] = z;}

/** Position value setter for array argument */
#define setPositionArray(pos, from) {pos[0] = from[0]; pos[1] = from[1]; pos[2] = from[2];}

void gameInit(GameInstance* this);
void onRender(GameInstance* this);
void updateCamera(GameInstance* this);
void onLogic(GameInstance* this);

#endif /* GAME_H_ */
