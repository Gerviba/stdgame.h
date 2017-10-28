#ifndef GAME_H_
#define GAME_H_

#include "object.h"
#include "map.h"

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
	ObjectInfo *objects;

	GLuint tileVAO;
	GLuint blankTextureId;
	int windowInstance;
} GameInstance;

#endif /* GAME_H_ */
