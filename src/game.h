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

	GLuint tileVAO;
	GLuint blankTextureId;
	GLFWwindow *window;
	int score;
	GameState state;

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
	float lightPosition[MAX_NUM_LIGHTS * 3];
	float lightColor[MAX_NUM_LIGHTS * 3];
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

void gameInit(GameInstance* this);
void onRender(GameInstance* this);
void updateCamera(GameInstance* this);
void onLogic(GameInstance* this);
void setPerspective(GameInstance* this, float fov, float aspect, float near, float far);

#endif /* GAME_H_ */
