#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
	#include <wingdi.h>
	#define GLUT_DISABLE_ATEXIT_HACK
#else
	#include <sys/time.h>
#endif /* _WIN32 */

#include "includes.h"

extern GLuint shaderAttachFromFile(GLuint, GLenum, const char *);

void setPerspective(const GameInstance *this, float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf(fov / 2.0f);

	this->camera->projMat[0] = f / aspect;
	this->camera->projMat[1] = 0.0f;
	this->camera->projMat[2] = 0.0f;
	this->camera->projMat[3] = 0.0f;

	this->camera->projMat[4] = 0.0f;
	this->camera->projMat[5] = f;
	this->camera->projMat[6] = 0.0f;
	this->camera->projMat[7] = 0.0f;

	this->camera->projMat[8] = 0.0f;
	this->camera->projMat[9] = 0.0f;
	this->camera->projMat[10] = (far + near) / (near - far);
	this->camera->projMat[11] = -1.0f;

	this->camera->projMat[12] = 0.0f;
	this->camera->projMat[13] = 0.0f;
	this->camera->projMat[14] = (2.0f * far * near) / (near - far);
	this->camera->projMat[15] = 0.0f;

	glMultMatrixf(this->camera->projMat);
}

void loadTileVAO(const GameInstance *this) {
	glGenVertexArrays(1, &this->tileVAO);
	glBindVertexArray(this->tileVAO);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLfloat v[] = {
			0.0f, 0.0f, 0.0f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 	0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 	0.0f, 0.0f, 	0.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, sizeof(GLfloat) * 5);
	glEnableVertexAttribArray(2);

	// TODO: cleanup
}

void gameInit(GameInstance *this) {
	this->shader->shaderId = glCreateProgram();
	shaderAttachFromFile(this->shader->shaderId, GL_VERTEX_SHADER, "assets/shaders/shader.vertex");
	shaderAttachFromFile(this->shader->shaderId, GL_FRAGMENT_SHADER, "assets/shaders/shader.fragment");

	glBindAttribLocation(this->shader->shaderId, 0, "position");
	glBindAttribLocation(this->shader->shaderId, 1, "texCoord");
	glBindAttribLocation(this->shader->shaderId, 2, "normal");

	GLint result;
	glLinkProgram(this->shader->shaderId);
	glGetProgramiv(this->shader->shaderId, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		GLint length;
		char *log;

		glGetProgramiv(this->shader->shaderId, GL_INFO_LOG_LENGTH, &length);
		log = malloc(length);
		glGetProgramInfoLog(this->shader->shaderId, length, &result, log);

		fprintf(stderr, "[Error] Shader program linking failed: %s\n", log);
		free(log);

		glDeleteProgram(this->shader->shaderId);
		this->shader->shaderId = 0;
	}

	this->shader->cameraPosition = glGetUniformLocation(this->shader->shaderId, "cameraPosition");
	this->shader->lightPosition = glGetUniformLocation(this->shader->shaderId, "lightPosition");
	this->shader->lightColor = glGetUniformLocation(this->shader->shaderId, "lightColor");
	this->shader->texturePosition = glGetUniformLocation(this->shader->shaderId, "tex");
	this->shader->numLights = glGetUniformLocation(this->shader->shaderId, "numLights");
	this->shader->baseColor = glGetUniformLocation(this->shader->shaderId, "baseColor");
	this->shader->projMat = glGetUniformLocation(this->shader->shaderId, "projMat");
	this->shader->viewMat = glGetUniformLocation(this->shader->shaderId, "viewMat");
	this->shader->moveMat = glGetUniformLocation(this->shader->shaderId, "moveMat");
	this->shader->modelMat = glGetUniformLocation(this->shader->shaderId, "modelMat");

	loadTileVAO(this);
	loadTexture(&this->blankTextureId, "null.png");

	this->state = INGAME;
	this->map = loadMap(this, "assets/maps/test2.map");

	initPlayer(this);
	initFont(this);
	onLogicIngame(this);
	printf("[Render] First logic done\n");

	updateCamera(this);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
}

void updateCamera(GameInstance* this) {
	glLoadIdentity();
	glRotatef(-this->camera->rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(-this->camera->rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(-this->camera->rotation[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(-this->camera->position[0],
			-this->camera->position[1],
			-this->camera->position[2]);
}

static void renderGUI(GameInstance* this) {
	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "You have no $", (GLfloat[]) {6.0f, 7.0625f, 1.0f, 1.0f}, color);

	char str[16];
	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS)
		++this->score;
	sprintf(str, "%d *", this->score);

	renderFontTo(this, str, (GLfloat[]) {this->camera->position[X] + 3.0f - (strlen(str) * 0.0625f * 4),
			this->camera->position[Y] + 1.0625f, 1.0f, 1.0f}, color);
	renderFontTo(this, "$$$", (GLfloat[]) {this->camera->position[X] - 3.0f, this->camera->position[Y] + 1.0625f,
			1.0f, 1.0f}, color);
}

void onRender(GameInstance *this) {
	static const GLfloat TILE_MOVE_MAT[16] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f};
	static const GLfloat BASE_COLOR[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	glUseProgram(this->shader->shaderId);
	glUniform3fv(this->shader->cameraPosition, 1, this->camera->position);
	glUniform3fv(this->shader->lightPosition, this->lighting->numLights, this->lighting->lightPosition);
	glUniform3fv(this->shader->lightColor, this->lighting->numLights, this->lighting->lightColor);
	glUniform1i(this->shader->numLights, this->lighting->numLights);
	glUniform4fv(this->shader->baseColor, 1, BASE_COLOR);
	glUniformMatrix4fv(this->shader->projMat, 1, GL_FALSE, this->camera->projMat);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
	glUniformMatrix4fv(this->shader->viewMat, 1, GL_FALSE, this->camera->viewMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, TILE_MOVE_MAT);

	glBindVertexArray(this->tileVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(this->shader->texturePosition, 0);

	ListElement *it;
	for (it = this->map->tiles->first; it != NULL; it = it->next)
		renderTile(this, (Tile *) it->data);

	glBindTexture(GL_TEXTURE_2D, 0);
	for (it = this->map->objects->staticInstances->first; it != NULL; it = it->next)
		renderStaticObject(this, (StaticObjectInstance *) it->data);

	if (this->state == INGAME) {
		renderGUI(this);
	} else if (this->state == MENU) {

	}


    // Cleanup
    glUseProgram(0);

	updateCamera(this);
}

static unsigned int getTicks(void) {
	#ifdef _WIN32
		return GetTickCount();
	#else
		struct timeval t;
		gettimeofday(&t, NULL);
		return (t.tv_sec * 1000) + (t.tv_usec / 1000);
	#endif /* _WIN32 */
}

void onLogicIngame(GameInstance *this) {
	static unsigned int prevTicks = 0;
	unsigned int ticks;
	float secondsElapsed;
	int i;

	if (prevTicks == 0)
		prevTicks = getTicks();
	ticks = getTicks();
	secondsElapsed = (float) (ticks - prevTicks) / 1000.0f;
	prevTicks = ticks;

	StaticObjectInstance *obj = ((StaticObjectInstance *) this->map->objects->staticInstances->first->data);

	static float plannedCameraRotation = 0, cameraRotation = 0;

	float deltaMoveX = 0;
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		deltaMoveX += -secondsElapsed * 2;
		obj->rotation[Y] = 0;
		plannedCameraRotation = 1;
	}

	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		deltaMoveX += secondsElapsed * 2;
		obj->rotation[Y] = 180;
		plannedCameraRotation = -1;
	}

	if (!(glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) &&
			!(glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
		plannedCameraRotation = 0;
	}

	ListElement *it;
	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((int) tile->y == (int) this->player->position[Y]
			  && (int) tile->x == (int) (deltaMoveX + this->player->position[X])) {
			if (tile->type != 0)
				deltaMoveX = 0;
			break;
		}
	}
	this->player->position[X] += deltaMoveX;

	if (this->player->velocity[Y] == 0 && (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)) {
		this->player->velocity[Y] = 10;
		printf("jump");
	}

	float deltaMoveY = this->player->velocity[Y] * (9.81 / 1000);
	this->player->velocity[Y] -= 0.5;
	if (this->player->velocity[Y] < -8)
		this->player->velocity[Y] = -8;


	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((int) tile->y == (int) (deltaMoveY + this->player->position[Y])
			  && (int) tile->x == (int) (this->player->position[X])) {
			if (tile->type != 0)
				deltaMoveY = 0;
			break;
		}
	}
	this->player->position[Y] += deltaMoveY;

//	ListElement *it;
//	for (it = this->map->tiles->first; it != NULL; it = it->next) {
//		Tile *tile = (Tile *)it->data;
//		if (tile->y >= this->player->position[Y] - 1 && tile->y <= this->player->position[Y] - 1
//			  && (int) tile->x == (int) (this->player->position[X] + 0.5)) {
//
//			if ((tile->type & MOVE_BLOCK_Y) != 0) {
//				this->player->velocity[Y] = 0;
////				this->player->position[Y] = (int) tile->y + 1;
//				deltaMoveY = 0;
//				printf("break\n");
//				break;
//			}
//		}
//	}
//	this->player->position[Y] += deltaMoveY;


//	if (this->player->position[Y] <= 7) {
//		this->player->velocity[Y] = 0;
//		this->player->position[Y] = 7;
//		this->player->jumping = GL_FALSE;
//	}

#ifndef DEBUG_MOVEMENT
	this->camera->position[X] = this->player->position[X] + 2;
	this->camera->position[Y] = this->player->position[Y] + 1;
	this->camera->position[Z] = 4.8;
#endif

	// DEBUG
	obj->position[X] = this->player->position[X];
	obj->position[Y] = this->player->position[Y];
//	obj->rotation[Z] = 270;
	// END DEBUG

	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	double cursorX, cursorY;
	glfwGetCursorPos(this->window, &cursorX, &cursorY);
#ifndef DEBUG_MOVEMENT
	if (plannedCameraRotation == 1) {
		cameraRotation = min(cameraRotation + 0.2, 2);
	} else if (plannedCameraRotation == -1) {
		cameraRotation = max(cameraRotation - 0.2, -2);
	} else if (cameraRotation > 0) {
		cameraRotation -= 0.3;
		if (cameraRotation < 0)
			cameraRotation = 0;
	} else if (cameraRotation < 0) {
		cameraRotation += 0.3;
		if (cameraRotation > 0)
			cameraRotation = 0;
	}
	this->camera->rotation[Y] = cameraRotation;
#endif

	i = 0;
	for (it = this->map->lights->first; it != NULL; it = it->next, ++i) {
		Light light = *(Light *)it->data;
		this->lighting->lightColor[i * 3 + 0] = light.color[0];
		this->lighting->lightColor[i * 3 + 1] = light.color[1];
		this->lighting->lightColor[i * 3 + 2] = light.color[2];
		this->lighting->lightPosition[i * 3 + 0] = light.x;
		this->lighting->lightPosition[i * 3 + 1] = light.y;
		this->lighting->lightPosition[i * 3 + 2] = 1.4f;

#ifdef DEBUG
		glPushMatrix();
		glTranslatef(this->lighting->lightPosition[i * 3 + 0],
				this->lighting->lightPosition[i * 3 + 1],
				this->lighting->lightPosition[i * 3 + 2]);
		glColor3fv(this->lighting->lightColor + (i * 3));
//		glutSolidSphere(0.04, 36, 36);
		glPopMatrix();
#endif
	}
	this->lighting->numLights = i;

}
