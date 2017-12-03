/**
 * @file game.h
 * @author Gerviba (Szabo Gergely)
 * @brief Game initialisation and game loop (header)
 *
 * @par Definition:
 * 		game.c
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
	GameState state;

	LinkedList /*ReferencePoint*/ *referencePoints;
};

/**
 * @todo Add gamepad support
 */
struct InputActionWrapper {
	GLint id[3];
};

struct Options {
	GLint msaa;
	GLboolean fullscreen;
	GLint height;
	GLint width;
	GLint windowedHeight;
	GLint windowedWidth;
	GLboolean shadow;
	GLboolean cameraMovement;
	GLfloat tanFov;
	GLfloat aspectRatio;

	InputActionWrapper moveLeft;
	InputActionWrapper moveRight;
	InputActionWrapper jump;
	InputActionWrapper sneek;
	InputActionWrapper attack;
	InputActionWrapper use;
	InputActionWrapper spell1;
	InputActionWrapper spell2;
	InputActionWrapper spell3;
	InputActionWrapper menu;

	Component *selectedToSet;
	GLboolean reloadProgram;
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

/** Color value setter */
#define setColor(color, r, g, b, a) {color[0] = r; color[1] = g; color[2] = b; color[3] = a;}

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

GLfloat getDistSquared2D(GLfloat a[3], GLfloat b[3]);
GLfloat getDistSquared2DDelta(GLfloat a[3], GLfloat deltaA[3], GLfloat b[3]);
GLfloat getDistSquaredXY(GLfloat x, GLfloat y, GLfloat b[3]);

#endif /* GAME_H_ */
