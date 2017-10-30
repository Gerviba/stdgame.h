#ifndef GAME_H_
#define GAME_H_

#ifdef __linux__
	#include <GLFW/glfw3.h>
#elif defined APPLE
	#include <GLFW/glfw3.h>
#else
	#include <glfw3.h>
#endif

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
} CameraInfo;

typedef struct {
	int numLights;
	float lightPosition[MAX_NUM_LIGHTS * 3];
	float lightColor[MAX_NUM_LIGHTS * 3];
} LigingInfo;

typedef struct {
	ShaderInfo *shader;
	CameraInfo *camera;
	Map *map;
	LigingInfo *lighting;
	Player *player;

	GLuint tileVAO;
	GLuint blankTextureId;
	GLFWwindow *window;
} GameInstance;

#endif /* GAME_H_ */
