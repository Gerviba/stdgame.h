#ifndef GAME_H_
#define GAME_H_

#include "font.h"
#include "object.h"
#include "map.h"
#include "character.h"

#define MAX_NUM_LIGHTS 28

typedef struct {
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
} ShaderInfo;

typedef struct {
	GLfloat rotation[3];
	GLfloat position[3];
	GLfloat projMat[16];
	GLfloat viewMat[16];

	GLfloat destinationRotation[3];
	GLfloat destinationPosition[3];
} CameraInfo;

typedef struct {
	int numLights;
	float lightPosition[MAX_NUM_LIGHTS * 3];
	float lightColor[MAX_NUM_LIGHTS * 3];
} LigingInfo;

typedef enum {
	MENU,
	INGAME
} GameState;

typedef struct {
	ShaderInfo *shader;
	CameraInfo *camera;
	Map *map;
	LigingInfo *lighting;
	Player *player;
	Font *font;

	GLuint tileVAO;
	GLuint blankTextureId;
	GLFWwindow *window;
	int score;
	GameState state;

	struct {
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

	} options;
} GameInstance;

void gameInit(GameInstance*);
void onRender(GameInstance*);
void onLogic(GameInstance*);
void setPerspective(GameInstance*, float, float, float, float);

void renderStaticObject(GameInstance*, StaticObjectInstance*);
void renderDynamicObject(GameInstance*, DynamicObjectInstance*);
void renderActiveObject(GameInstance*, ActiveObjectInstance*);
void renderTile(GameInstance*, Tile*);
void initStraticInstance(GameInstance*, StaticObjectInstance*);

#endif /* GAME_H_ */
